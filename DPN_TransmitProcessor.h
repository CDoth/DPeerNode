#ifndef DPN_TRANSMITPROCESSOR_H
#define DPN_TRANSMITPROCESSOR_H
#include <map>



#include "DPN_TransmitTools.h"
#include "DPN_PacketType.h"
#include "DPN_ClientUnderLayer.h"
#include "DPN_Modules.h"
#include "DPN_Util.h"






class DPN_TransmitProcessor;

template <PacketType> struct DPN_ProcessorTransform {
    typedef DPN_TransmitProcessor* proc_type;
    static proc_type create() {return nullptr;}
    static proc_type transform(DPN_TransmitProcessor *p) {return p;}
};
#define DECL_UNIT(TYPE, NAME) UNIT(TYPE) NAME = content()

class DPN_TransactionSpace;
//========================================================================================================

enum CallerPosition { DPN_HOST, DPN_SERVER };
static const char * getCallerName( CallerPosition p ) {
    return p == DPN_HOST ? "Host" : "Server";
}


    struct SERVER_CALL {};
    struct HOST_CALL {};


    template <class T>
    class ActionUnit {
    public:
        typedef DPN_Result (T::*ServerCallback)( SERVER_CALL );
        typedef DPN_Result (T::*HostCallback)( HOST_CALL );

        ActionUnit( ServerCallback s ) {
            iPosition = DPN_SERVER;
            pS = s;
            pH = nullptr;
            pNext = nullptr;
        }
        ActionUnit( HostCallback h ) {
            iPosition = DPN_HOST;
            pS = nullptr;
            pH = h;
            pNext = nullptr;
        }

        void setNext( ActionUnit *n ) {pNext = n;}
        inline ActionUnit<T> * next() { return pNext; }
        inline bool checkPosition( CallerPosition cp ) const { return iPosition == cp; }
        DPN_Result call( T *obj ) {

            using namespace DPN::Logs;
            if( obj == nullptr ) {
                DL_BADPOINTER(1, "object");
                return DPN_FAIL;
            }
            if( pS ) return DPN_CALL_MEMBER_FN(obj, pS)( SERVER_CALL() );
            if( pH ) return DPN_CALL_MEMBER_FN(obj, pH)( HOST_CALL() );
            return DPN_FAIL;
        }
    private:
        ServerCallback pS;
        HostCallback pH;
        CallerPosition iPosition;
        ActionUnit<T> *pNext;
    };
    class AbstractActionLine {
    public:
        AbstractActionLine() {
            pObject = nullptr;
            iStep = 0;
            iLastResult = DPN_FAIL;
        }
        void setTarget( void *object ) {pObject = object;}

        virtual bool isOver() const = 0;
        virtual DPN_Result go( CallerPosition cp ) = 0;
        virtual void restart() = 0;
        inline int step() const {return iStep;}
    protected:
        void *pObject;
        int iStep;
        DPN_Result iLastResult;
    };
    template <class T>
    struct HostCallbackWrapper {
        typedef DPN_Result (T::*HostCallback)( HOST_CALL );
        HostCallback cb;
        HostCallbackWrapper( HostCallback h ) : cb(h) {}
    };
    template <class T>
    struct ServerCallbackWrapper {
        typedef DPN_Result (T::*ServerCallback)( SERVER_CALL );
        ServerCallback cb;
        ServerCallbackWrapper( ServerCallback s ) : cb(s) {}
    };


    template <class T>
    class ActionLine : public AbstractActionLine {
    public:
        ActionLine() {
            start = nullptr;
            head = nullptr;
            current = nullptr;
        }
        typedef DPN_Result (T::*ServerCallback)( SERVER_CALL );
        typedef DPN_Result (T::*HostCallback)( HOST_CALL );

        ActionLine & operator<< ( ServerCallback s ) { return add( new ActionUnit<T>( s ) ); }
        ActionLine & operator<< ( HostCallback h ) { return add( new ActionUnit<T>( h ) ); }
        ActionLine & operator<< ( HostCallbackWrapper<T> h ) { return add( new ActionUnit<T>( h.cb )); }
        ActionLine & operator<< ( ServerCallbackWrapper<T> s ) { return add( new ActionUnit<T>( s.cb )); }

        DPN_Result go( CallerPosition cp ) override {

            using namespace DPN::Logs;

            DL_INFO(1, "line: [%p]", this);
            while( current && !current->checkPosition( cp ) ) {
                current = current->next();
                ++iStep;
            }

            DL_INFO(1, "current: [%p]", current);

            if( current == nullptr ) {
                return DPN_SUCCESS;
            }
            iLastResult = DPN_SUCCESS;

            while ( iLastResult == DPN_SUCCESS ) {
                iLastResult = current->call( reinterpret_cast<T*>( pObject ) );

                ++iStep;
                if( (current = current->next() ) == nullptr ) return DPN_SUCCESS;
                if( current->checkPosition( cp ) == false ) return iLastResult;
            }

            return iLastResult;
        }
        void restart() override {
            current = start;
            iStep = 0;
        }
        inline bool isOver() const override { return current == nullptr; }
    private:
        ActionLine & add( ActionUnit<T> *u ) {
            if( start == nullptr ) start = u;
            if( current == nullptr ) current = start;
            if( head ) head->setNext( u );
            head = u;
            return *this;
        }
    private:
        ActionUnit<T> *start;
        ActionUnit<T> *head;
        ActionUnit<T> *current;
    };


//========================================================================================================
class DPN_TransactionSpace {
public:
    DPN_TransactionSpace();
    void setMinimumIndex(int min);
    void registerTransaction(DPN_TransmitProcessor *t);
    DPN_TransmitProcessor * registerTransaction(PacketType, int key);
    DPN_TransmitProcessor * transaction(int index) const;
private:
    bool isValidKey(int key) const;
private:
    std::map<int, DPN_TransmitProcessor*> iMap;
    int iMinimumIndex;
};
class DPN_ProcessorList {
public:
    DPN_ProcessorList() : __first(nullptr), __head(nullptr), __actual(nullptr) {}

    struct __node {
        __node() : proc(nullptr), prev(nullptr), next(nullptr) {}
        __node(DPN_TransmitProcessor *__p) : proc(__p), prev(nullptr), next(nullptr) {}
        inline void set_proc(DPN_TransmitProcessor *__p) {proc = __p;}
        inline void exclude() {

            if(prev) prev->next = next;
            if(next) next->prev = prev;

        }
        DPN_TransmitProcessor *proc;
        __node *prev;
        __node *next;
    };


    void push_back(DPN_TransmitProcessor *proc);
    void push_front(DPN_TransmitProcessor *proc);
    void insert_to_actual(DPN_TransmitProcessor *proc);
    DPN_TransmitProcessor * getActual();
    void popActual();
    void skip();
    void restart();

    void move(DPN_ProcessorList &list);
    void clear();

    inline bool empty() const {return __first == nullptr;}
    inline bool noEmpty() const {return __first;}
private:

    void zero() {
        __first = nullptr;
        __head = nullptr;
        __actual = nullptr;
    }
    inline __node * getNode(DPN_TransmitProcessor *p) {

        auto n = __node__pool.get();
        n->set_proc(p);
        return n;

        return nullptr;
    }
    inline void removeNode(__node *node) {

        __node__pool.set(node);
    }
private:
    __node *__first;
    __node *__head;
    __node *__actual;

private:
    DPN::Util::SimplePool<__node> __node__pool;
};
class DPN_CrossThreadProcessorList : DPN_ProcessorList {
public:
    using DPN_ProcessorList::empty;
    using DPN_ProcessorList::clear;
    void push_back(DPN_TransmitProcessor *proc);
    void moveTo(DPN_ProcessorList &list);
    inline bool empty() {return DPN_ProcessorList::empty();}
private:
    std::mutex __mutex;
};



//================================================================================
struct __packet_header {
    uint32_t size;
    union {
      uint32_t __packet_type;
      PacketType type;
    };
    uint32_t transaction;
};
struct __packet_header2 {

    union {
      uint32_t __packet_type;
      PacketType type;
    };
    uint32_t transaction;
};
class PacketProcessor
        : public DPN::IO::IOContext
        , public DPN::Client::Underlayer
{
public:
    PacketProcessor();
    PacketProcessor( DPN::Client::Underlayer ul );
    void init( bool initiator );
    void send( DPN_TransmitProcessor *p );
    DPN_Result generate(DPN_ExpandableBuffer &b) override;
    DPN_Result process(DPN_ExpandableBuffer &b) override;

private:
    bool __resend( DPN_TransmitProcessor *p );
    bool __makePacket(DPN_TransmitProcessor *proc, DPN_ExpandableBuffer &b);
private: // common:
    DPN_TransactionSpace iSpace;
private: // send direction:
    DPN_ProcessorList iQueue;
    DPN_CrossThreadProcessorList iBackQueue;
};

//========================================================================================================
class DPN_TransmitProcessor
        : public DPN::Client::Underlayer
{
public:
    friend class DPN_TransactionSpace;
    friend class PacketProcessor;
    DPN_TransmitProcessor( );
    virtual ~DPN_TransmitProcessor();

    void bind(Underlayer underLayer );

    DPN_Result action();
    DPN_Result action( DPN_ExpandableBuffer &packet );
    virtual PacketType me() const {return PT__NO_TYPE;}

public:
    void clear();
    const DPN_ExpandableBuffer & product() const;
    inline int transaction() const {return iTransactionIndex;}
    inline bool resendable() {return DPN::checkFlag(iInnerFlags, IF__RESENDABLE);}
protected:
    template <class Impl>
    ActionLine<Impl> & line() {
        if( pLine == nullptr ) {
            pLine = new ActionLine<Impl>;
        }
        return *reinterpret_cast<ActionLine<Impl>*>(pLine);
    }
private:
    void makeResendable() { DPN::setFlag( iInnerFlags, IF__RESENDABLE); }
    void disableResendable() { DPN::disableFlag( iInnerFlags, IF__RESENDABLE); }
    bool inition();
    void processFault( CallerPosition p );
protected:
    virtual void makeActionLine() = 0;
    virtual void injection();
    virtual void fault( SERVER_CALL );
    virtual void fault( HOST_CALL );
    inline __transmit_content & content() { return iContent; }
private:
    enum UserFlags {

    };
    enum InnerFlags {
        IF__RESENDABLE = (1 << 0)
    };
    enum TransportFlags {
        TF__FAILED = (1 << 0)
    };

private:
    uint8_t iUserFlags;
    uint8_t iInnerFlags;
    uint8_t iTransportFlags;

    int iTransactionIndex;
    CallerPosition iPosition;
    AbstractActionLine *pLine;
    __transmit_content iContent;
private:
    DECL_UNIT(uint8_t, UNIT_FLAGS);
};
//========================================================================================================
typedef DPN_TransmitProcessor* (*CREATE_PROCESSOR)();
typedef std::map<PacketType, CREATE_PROCESSOR> PROCMAP_T;
struct ProcessorMapWrapper {
    DPN_TransmitProcessor * create(PacketType t);
    inline const PROCMAP_T & inner() const {return map;}
private:
    friend struct ProcessorMapFiller;
    void fill(PacketType t, CREATE_PROCESSOR callback);
private:
    PROCMAP_T map;
};
struct ProcessorMapFiller {
    ProcessorMapFiller(PacketType t, CREATE_PROCESSOR callback, ProcessorMapWrapper *out = nullptr);
};
#define DPN_FILL_PROC_MAP(TYPE, PROCESSOR) \
    static ProcessorMapFiller ___proc_binder_ ## TYPE ## __ ## PROCESSOR(TYPE, \
    []() -> DPN_TransmitProcessor* \
    {return new PROCESSOR;} \
    );
#define DPN_GET_PRC_MAP(OUT_REF) ProcessorMapFiller(PT__NO_TYPE, nullptr, &OUT_REF);
extern ProcessorMapWrapper PROCESSOR_CREATOR;
//========================================================================================================
class DPN_TransmitProcessorMicroPool {
public:
    friend class DPN_ProcessorPool;
    DPN_TransmitProcessorMicroPool() {}

    DPN_TransmitProcessor * get() {
        if(pool.size()) {
            auto p = pool.front();
            pool.pop_front();
            return p;
        }
        return nullptr;
    }
    template <PacketType t>
    typename DPN_ProcessorTransform<t>::proc_type
    compiletime__get() {
        typename DPN_ProcessorTransform<t>::proc_type p = nullptr;
        if(pool.size()) {
            p = DPN_ProcessorTransform<t>::transform(pool.front());
            pool.pop_front();
        } else {
            p =  DPN_ProcessorTransform<t>::transform(DPN_ProcessorTransform<t>::create());
        }
        return p;
    }
    DPN_TransmitProcessor *
    runtime__get(PacketType t) {

        DPN_TransmitProcessor *p = get();

        return p ? p : PROCESSOR_CREATOR.create(t);

        return nullptr;
    }
private:
    void insertProcessor(DPN_TransmitProcessor *p) {
        pool.push_back(p);
    }
private:
    DArray<DPN_TransmitProcessor*> pool;

//    DPN_SimplePool<DPN_TransmitProcessor> __pool2;
};
typedef std::map<PacketType, DPN_TransmitProcessorMicroPool> POOL_MAP_T;
class DPN_ProcessorPool {
public:
    DPN_ProcessorPool();

    template <PacketType t>
    typename DPN_ProcessorTransform<t>::proc_type getProcessor() {
//        if(map.find(t) == map.end()) {
//            return nullptr;
//        }
        __insertType(t);

        DPN_TransmitProcessorMicroPool &micro_pool = map[t];
        typename DPN_ProcessorTransform<t>::proc_type p = micro_pool.compiletime__get<t>();
        return  DPN_ProcessorTransform<t>::transform(p);
    }
    DPN_TransmitProcessor * getProcessor(PacketType t);
    bool returnProcessor(DPN_TransmitProcessor *proc);
private:
    void __insertType(PacketType t);
private:
    POOL_MAP_T map;
};
namespace DPN_PROCS {
    extern DPN_ProcessorPool __global_ppool;
    template <PacketType t>
    typename DPN_ProcessorTransform<t>::proc_type
    inline processor() {return __global_ppool.getProcessor<t>();}
    inline DPN_TransmitProcessor * processor(PacketType t) {return  __global_ppool.getProcessor(t);}
    inline bool returnProcessor(DPN_TransmitProcessor *p) {return __global_ppool.returnProcessor(p);}
}
//========================================================================================================

//---------------------------------------------------------------------------

#define DPN_PROCESSOR_BIND(PROCESSOR, TYPE) \
    template <> struct DPN_ProcessorTransform<TYPE> {\
    typedef PROCESSOR* proc_type; \
    static DPN_TransmitProcessor * create() {return new PROCESSOR;} \
    static proc_type transform(DPN_TransmitProcessor *p) {return reinterpret_cast<proc_type>(p);} \
    }; \
    PacketType PROCESSOR::me() const {return TYPE;} \
    DPN_FILL_PROC_MAP(TYPE, PROCESSOR);



#define DPN_PROCESSOR \
    inline PacketType me() const override;

#define PROCESSOR_PTR(TYPE) DPN_ProcessorTransform<TYPE>::proc_type
#define INFO_LOG DL_INFO(1, "PROC: [%p][%s]", this, packetTypeName(me()));



#endif // DPN_TRANSMITPROCESSOR_H
