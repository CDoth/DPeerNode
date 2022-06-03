#ifndef DPN_TRANSMITPROCESSOR_H
#define DPN_TRANSMITPROCESSOR_H
#include <map>



#include "DPN_TransmitTools.h"
#include "DPN_PacketType.h"



using namespace DPeerNodeSpace;

class DPN_Modules;
class DPN_ProcessorPool;
class DPN_CrossThreadProcessorList;


// File requesting
// Data channel reserving
class DPN_TransactionMaster;
class DPN_TransactionParent {
public:
    virtual bool finishTransaction(DPN_AbstractTransaction *t) = 0;
};
class DPN_AbstractTransaction {
public:
    friend class DPN_TransactionMaster;
    friend class DPN_TransactionMap;
    DPN_AbstractTransaction();
    virtual ~DPN_AbstractTransaction();
    bool finish();
    int index() const;
    void use();
protected:
    virtual void begining() {}
    virtual void finishing() {}
private:
    DPN_TimeRange iTimeRange;
    bool iArchived;
    int iIndex;
    DPN_TransactionParent *pCreator;
    int iTransmitCounter;
};
/*
class DPN_TransactionMaster : public DPN_TransactionParent {
public:
    template <class TransactionImpl>
    inline TransactionImpl * generate() {
        TransactionImpl *t = new TransactionImpl;
        return reinterpret_cast<TransactionImpl*>( regTr(t) );
    }
    inline bool finishTransaction(DPN_AbstractTransaction *t) override { return unregTr(t); }

    DPN_AbstractTransaction * transactionA(int index) const;
    template <class T> inline T * transactionI(int index) const { return reinterpret_cast<T*>( transactionA(index) ); }
private:
    DPN_AbstractTransaction * regTr(DPN_AbstractTransaction *t);
    bool unregTr(DPN_AbstractTransaction *t);
private:
    DArray<DPN_AbstractTransaction*> aLegend;
    DArray<DPN_AbstractTransaction*> aTrs;
    DArray<int> aFreeKeys;
};
*/
class DPN_TransactionMap : public DPN_TransactionParent {
public:


    template <class TransactionImpl>
    inline TransactionImpl * generateTransaction() {
        TransactionImpl *t = new TransactionImpl;
        return reinterpret_cast<TransactionImpl*>( regTr(t) );
    }
    template <class TransactionImpl>
    inline TransactionImpl * registerTransaction(int key) {
        if( !isValidKey(key) ) return nullptr;
        TransactionImpl *t = new TransactionImpl;
        return reinterpret_cast<TransactionImpl*>( regTr(t, key) );
    }


    DPN_AbstractTransaction * transactionA(int index) const;
    template <class T> inline T * transactionI(int index) const { return reinterpret_cast<T*>( transactionA(index) ); }
    inline bool finishTransaction(DPN_AbstractTransaction *t) override { return unregTr(t); }
private:
    bool isValidKey(int key) const;
    DPN_AbstractTransaction * regTr(DPN_AbstractTransaction *tr, int key);
    DPN_AbstractTransaction * regTr(DPN_AbstractTransaction *tr);
    bool unregTr(DPN_AbstractTransaction *t);
    std::map<int, DPN_AbstractTransaction*> iMap;
};

struct __client_context {
    __client_context() {
        __connector = nullptr;
        __client = nullptr;
        __core = nullptr;
        __modules = nullptr;
        __flag__client_blocked = false;
    }
    __client_context & operator=(const __client_context &cc) {
        __connector = cc.__connector;
        __client = cc.__client;
        __core = cc.__core;
        __modules = cc.__modules;
        __flag__client_blocked.store(cc.__flag__client_blocked);
        return *this;
    }
    DPN_NodeConnector *__connector;
    DPN_AbstractClient *__client;
    DPN_AbstractConnectionsCore *__core;
    DPN_Modules *__modules;
    std::atomic<bool> __flag__client_blocked;
};
struct __thread_context {
    __thread_context() {
        __processor_pool = nullptr;
        __back_list = nullptr;
        __hash_tool = nullptr;
    }
    DPN_ProcessorPool *__processor_pool;
    DPN_CrossThreadProcessorList *__back_list;
    DPN_SHA256 *__hash_tool;
    DPN_ExpandableBuffer __buffer;
};
class DPN_TransmitProcessor;
template <DPN_PacketType> struct DPN_ProcessorTransform {
    typedef DPN_TransmitProcessor* proc_type;
    static proc_type create() {return nullptr;}
    static proc_type transform(DPN_TransmitProcessor *p) {return p;}
};
class DPN_ClientContext : public DWatcher<__client_context> {
public:
    DPN_ClientContext() : DWatcher<__client_context>(true) {}
    void init(const __client_context &c) {
        *data() = c;
    }
    DPN_NodeConnector * connector() {
        return data()->__connector;
    }
    DPN_AbstractClient * client() {return data()->__client;}
    DPN_AbstractConnectionsCore * core() {return data()->__core;}

    inline DPN_Modules * modules() {return data()->__modules;}
    inline const DPN_Modules * modules() const {return data()->__modules;}

    void blockClient() {data()->__flag__client_blocked = true;}
    void unblockClient() {data()->__flag__client_blocked = false;}
    inline bool isClientBlocked() const {return data()->__flag__client_blocked;}


    template <class T> inline T * _generateTransaction() {return data()->__client->generatedTransactions().generateTransaction<T>();}
    template <class T> inline T * _registerTransaction(int key) {return data()->__client->clientTransactions().registerTransaction<T>(key);}

    template <class T> inline T * innerTransaction(int key) {return data()->__client->generatedTransactions().transactionI<T>(key);}
    template <class T> inline T * _clientTransaction(int key) {return data()->__client->clientTransactions().transactionI<T>(key);}



};
class DPN_ThreadContext : public DWatcher<__thread_context> {
public:
    DPN_ThreadContext() : DWatcher<__thread_context>(true) {}

    void setProcessorPool(DPN_ProcessorPool *pp) {data()->__processor_pool = pp;}
    void setHashTool(DPN_SHA256 *ht) {data()->__hash_tool = ht;}


    DPN_ProcessorPool * processorPool() {return data()->__processor_pool;}
    const DPN_ProcessorPool * processorPool() const {return data()->__processor_pool;}
    DPN_SHA256 * hashtool() {return data()->__hash_tool;}
    const DPN_SHA256 * hashtool() const {return data()->__hash_tool;}
    DPN_ExpandableBuffer & buffer() {return data()->__buffer;}
    const DPN_ExpandableBuffer & buffer() const {return data()->__buffer;}

    //------------------------------------------------------------------------
    void alloc();
    DPN_CrossThreadProcessorList * backList();
    bool send(DPN_TransmitProcessor *proc);
    bool returnProcessor(DPN_TransmitProcessor *proc);
    DPN_TransmitProcessor * processor(DPN_PacketType t, DPN_AbstractTransaction *transaction = nullptr);
    template <DPN_PacketType t>
    typename DPN_ProcessorTransform<t>::proc_type
    processor(DPN_AbstractTransaction *transaction = nullptr);

};

#define UNIT(INNER_TYPE) __hl_item<INNER_TYPE>

class DPN_TransmitProcessor {
public:
    DPN_TransmitProcessor(DPN_AbstractTransaction *transaction = nullptr);
    void setTransaction(DPN_AbstractTransaction *t);

    DPN_Result sendCascade();
    DPN_Result receiveCascade();



    virtual DPN_PacketType me() const {return DPN_PACKETTYPE__NO_TYPE;}

    void repeat();
    void clear();
    bool complete();

//    void setDialog(DPN_Dialog &d);


    void inition(DPN_ClientContext &cc, DPN_ThreadContext &tc);
    virtual void postInition();
//    void inition(DPN_Context &context);

    bool isResendable() const {return iResendFlag;}



protected:
    virtual DPN_Result sendPrefix() {return DPN_SUCCESS;}
    virtual DPN_Result sendReaction() {return DPN_SUCCESS;}

    virtual DPN_Result receivePrefix() {return DPN_SUCCESS;}
    virtual DPN_Result receiveReaction() {return DPN_SUCCESS;}

    virtual void repeatProcessor() {}
    virtual void clearProcessor() {}



    virtual DPN_Result failureProcessing() {return DPN_FAIL;}
    virtual bool resendPredicat() {return false;}
    virtual bool makeHostLine() {return true;}
    virtual bool makeServerLine() {return true;}

    void resend() {iResendFlag = true;}


    template <class T>
    T * useTransation() {

        if( pTransaction )
        DL_INFO(1, "Transaction seted: [%p] [%d]",
                pTransaction, pTransaction->index());

        if( pTransaction == nullptr ) {
            int trIndex = UNIT_TRANSACTION.get();
            if( trIndex < 0 ) {
                DL_BADVALUE(1, "transaction index: [%d]", trIndex);
                return nullptr;
            }
            if( checkFlag(PF__GENERATOR) == false ) {

                DL_INFO(1, "Check registered transaction: index: [%d]", trIndex);

                if( (pTransaction = clientContext._clientTransaction<T>(trIndex)) == nullptr ) {
                    DL_INFO(1, "Can't find transaction, register new [%d]", trIndex);
                    if( (pTransaction = clientContext._registerTransaction<T>(trIndex)) == nullptr ) {
                        DL_FUNCFAIL(1, "registerTransaction. key: [%d]", trIndex);
                        return nullptr;
                    }
                    makeServerLine();
                }
            } else {

                DL_INFO(1, "Check generated transaction, index: [%d]", trIndex);

                if( (pTransaction = clientContext.innerTransaction<T>(trIndex)) == nullptr ) {
                    DL_INFO(1, "No transaction, generate it...");
                    if( (pTransaction = clientContext._generateTransaction<T>()) == nullptr ) {
                        DL_FUNCFAIL(1, "generateTransaction");
                        return nullptr;
                    }
                    makeHostLine();
                }
//                DL_INFO(1, "Make host line...");
            }
        }
        if( pTransaction ) pTransaction->use();
        return reinterpret_cast<T*>( pTransaction );
    }
    /*
    template <class T>
    T * useHostTransaction() {

        if( pTransaction == nullptr ) {
            if( (pTransaction = clientContext._generateTransaction<T>()) == nullptr ) {
                DL_FUNCFAIL(1, "generateTransaction");
                return nullptr;
            }
            makeHostLine();
        }
        if( pTransaction ) pTransaction->use();
        return reinterpret_cast<T*>(pTransaction);
    }
    template <class T>
    T * useServerTransaction() {

        int trIndex = UNIT_TRANSACTION.get();
        if( trIndex < 0 ) {
            DL_BADVALUE(1, "Transaction index: [%d]", trIndex);
            return nullptr;
        }
        if( pTransaction == nullptr ) {

            using namespace DPeerNodeSpace;
            DL_INFO(1, "transaction index: [%d]", trIndex);
            if( (pTransaction = clientContext.clientTransaction<T>(trIndex)) == nullptr ) {

                DL_INFO(1, "Can't find transaction, register new [%d]", trIndex);

                if( (pTransaction = clientContext.registerTransaction<T>(trIndex)) == nullptr ) {
                    DL_FUNCFAIL(1, "registerTransaction. key: [%d]", trIndex);
                    return nullptr;
                }
                makeServerLine();

            }

        }
        if( pTransaction ) pTransaction->use();
        return reinterpret_cast<T*>(pTransaction);
    }
    */



    bool send(DPN_TransmitProcessor *p);

    template <DPN_PacketType t>
    typename DPN_ProcessorTransform<t>::proc_type
    inline processor() {
        return threadContext.processor<t>();
    }
private:


    void makeCascades();

    DPN_Result sendPrepare();
    DPN_Result sendSuffix();

    DPN_Result receivePrepare();
    DPN_Result receiveSuffix();
    DPN_Result receiveResender();


private:
    DPN_Result send();
    DPN_Result receive();

    void _init();

private:

//    DPN_Result prepared;

    thread_local static __action_line<DPN_TransmitProcessor> iGlobalSendLine;
    thread_local static __action_line<DPN_TransmitProcessor> iGlobalReceiveLine;
    __action_line<DPN_TransmitProcessor> iSendCascade;
    __action_line<DPN_TransmitProcessor> iReceiveCascade;
    bool iResendFlag;

    DPN_ExpandableBuffer packet_buffer;

protected:
//    template <class T> inline T * transaction() {return reinterpret_cast<T*>(pTransaction);}
    DPN_TimeMoment initionTime;
    DPN_ClientContext clientContext;
    DPN_ThreadContext threadContext;
    __transmit_content content;
    DPN_AbstractTransaction *pTransaction;
    uint8_t iFlags;
    enum ProcessorFlags {
        PF__FAIL = (1 << 0),
        PF__GENERATOR = (1 << 1),
        PF__SNAKEMODE = (1 << 2)
    };
    void setFlag(ProcessorFlags f)     { iFlags |=  f; }
    void disableFlag(ProcessorFlags f) { iFlags &= ~f; }
    void inverseFlag(ProcessorFlags f) { iFlags ^=  f; }
    void clearFlags() {iFlags = 0;}
    bool checkFlag(ProcessorFlags f) const {return iFlags & f;}
    uint8_t flags() const {return iFlags;}

    void setFailMode() {setFlag(PF__FAIL);}
    void disableFailMode() {disableFlag(PF__FAIL);}


private:
    UNIT(int) UNIT_TRANSACTION = content;
    UNIT(uint8_t) UNIT_FLAGS = content;
};
//========================================================================================================



typedef DPN_TransmitProcessor* (*CREATE_PROCESSOR)();
typedef std::map<DPN_PacketType, CREATE_PROCESSOR> PROCMAP_T;
struct ProcessorMapWrapper {

    inline DPN_TransmitProcessor * create(DPN_PacketType t) {
        return (map.find(t) == map.end()) ? nullptr : map[t]();
    }

    const PROCMAP_T & inner() const {return map;}

private:
    friend struct ProcessorMapFiller;
    void fill(DPN_PacketType t, CREATE_PROCESSOR callback) {
        if(map.find(t) == map.end()) {
            map[t] = callback;
        }
    }
private:
    PROCMAP_T map;
};
struct ProcessorMapFiller {

    ProcessorMapFiller(DPN_PacketType t, CREATE_PROCESSOR callback, ProcessorMapWrapper *out = nullptr) {

        static ProcessorMapWrapper wrapper;

        if(out)  *out = wrapper;
        else     wrapper.fill(t, callback);
    }

};
#define DPN_FILL_PROC_MAP(TYPE, PROCESSOR) \
    static ProcessorMapFiller ___proc_binder_ ## TYPE ## __ ## PROCESSOR(TYPE, \
    []() -> DPN_TransmitProcessor* \
    {return new PROCESSOR;} \
    );

#define DPN_GET_PRC_MAP(OUT_REF) ProcessorMapFiller(DPN_PACKETTYPE__NO_TYPE, nullptr, &OUT_REF);
//========================================================================================================
extern ProcessorMapWrapper PROCESSOR_CREATOR;


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
    template <DPN_PacketType t>
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
    runtime__get(DPN_PacketType t) {

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
typedef std::map<DPN_PacketType, DPN_TransmitProcessorMicroPool> POOL_MAP_T;

class DPN_ProcessorPool {
public:

    DPN_ProcessorPool() {

        DPN_GET_PRC_MAP(PROCESSOR_CREATOR);

        const PROCMAP_T & __inner = PROCESSOR_CREATOR.inner();
        if(__inner.empty()) {
            DL_WARNING(1, "Empty processor creator map");
        } else {
            for(auto it: __inner) {
                map[it.first] = DPN_TransmitProcessorMicroPool();
            }
        }

    }

    template <DPN_PacketType t>
    typename DPN_ProcessorTransform<t>::proc_type getProcessor(DPN_AbstractTransaction *transaction = nullptr) {


        if(map.find(t) == map.end()) {
            return nullptr;
        }

        DPN_TransmitProcessorMicroPool &micro_pool = map[t];
        typename DPN_ProcessorTransform<t>::proc_type p = micro_pool.compiletime__get<t>();
        p->setTransaction(transaction);
        return  DPN_ProcessorTransform<t>::transform(p);
    }
    DPN_TransmitProcessor * getProcessor(DPN_PacketType t, DPN_AbstractTransaction *transaction = nullptr) {

        if(map.find(t) == map.end()) {
            DL_ERROR(1, "Bad packet type: [%d:%s]", t, packetTypeName(t));
            return nullptr;
        }

        DPN_TransmitProcessorMicroPool &micro_pool = map[t];
        DPN_TransmitProcessor *p = micro_pool.runtime__get(t);

        p->setTransaction(transaction);
        return p;
    }
    bool returnProcessor(DPN_TransmitProcessor *proc) {

//        DL_INFO(1, "RETURN PROCESSOR: [%p]", proc);

        if(proc == nullptr) {
            return false;
        }
        auto f = map.find(proc->me());
        if(f == map.end()) {
            return false;
        }

        f->second.insertProcessor(proc);

        return true;
    }
private:
    POOL_MAP_T map;
};
//========================================================================================================
template<DPN_PacketType t>
typename DPN_ProcessorTransform<t>::proc_type DPN_ThreadContext::processor(DPN_AbstractTransaction *transaction) {
    return data()->__processor_pool ? data()->__processor_pool->getProcessor<t>(transaction) : nullptr ;
}

//---------------------------------------------------------------

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
    DPN_SimplePool<__node> __node__pool;
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

//---------------------------------------------------------------------------

#define DPN_PROCESSOR_BIND(PROCESSOR, TYPE) \
    template <> struct DPN_ProcessorTransform<TYPE> {\
    typedef PROCESSOR* proc_type; \
    static DPN_TransmitProcessor * create() {return new PROCESSOR;} \
    static proc_type transform(DPN_TransmitProcessor *p) {return reinterpret_cast<proc_type>(p);} \
    }; \
    DPN_PacketType PROCESSOR::me() const {return TYPE;} \
    DPN_FILL_PROC_MAP(TYPE, PROCESSOR);



#define DPN_PROCESSOR \
    inline DPN_PacketType me() const override;

#define PROCESSOR_PTR(TYPE) DPN_ProcessorTransform<TYPE>::proc_type
#define INFO_LOG DL_INFO(1, "PROC: [%p][%s]", this, packetTypeName(me()));



#endif // DPN_TRANSMITPROCESSOR_H
