#ifndef __DPEERNODE_GLOBAL_H
#define __DPEERNODE_GLOBAL_H



//----------------------
#include <DLogs.h>
#include <sha256.h>
//----------------------
#include <mutex>
#include <thread>
//----------------------

/* Private fields name prefix:
 *
 * a - DArray
 * i - instance
 * p - pointer
 * w - DWatcher child
 * r - reference
 * e - enum
*/



/*
-------------------------
 1. create folder
 2. send request about stop send if
 receive start fault
 3. Dont remove receiving processors
 SEND SIDE: processors in duplex list
      no remove but skip processors to repeat after
 RECEIVE SIDE: processors in map
      key is DPN_PacketType
      Receive packet type and use specific processor
      withour removing/adding processors

 4. Smart wrapper for DPN_TransmitProcessor pointer
 5. Check file transmiting send/receive sides
 6. File shift
 7. File hash sum to control
 8. !All dwatcher users works like refs by default,
 without detach/copy technique!


 [C] - check part

 File transmit steps:
 -------------------------------------------------------------- Catalog file:
 1. Request file: [host->server] with file [key, name, size]

 2. Server:
 >[C] Check if file already transmiting
 - true: ignore request from [host]
 - false: continue

 >[C] Check [key, name, size]:
 - false: catalogs need sync, send [server->host] info about wrong sync
 - true: go to send-prepare processor


 3. Server send-prepare:

 >[C] Check file existing:
 - false: cancel transmiting, send [server->host] info about cancel, change catalog and force resync for host
 - true: continue checking...
 > File size:

 >[C] If file shift on host == 0:
 - compare actual file size and file size in server catalog:
 - false: cancel transmiting, change size note it server catalog and send [server->host] catalog changes
 - true: continue checking...

 >[C] If file shift on host != 0:
 - compare hash of host part and server part:
 - false: cancel transmiting and send [host->server] info about not-actual data in file, host should drop file part
 - true: check as point [If file shift on host == 0]

 >[C] Try open file on server for sending
 - false: cancel transmiting
 - true: continue...

 >[C] Add file to send map
 - false: cancel transmiting
 - true: continue...

 4. Host receive-prepare:
 - Create dirs and file or open file
*/

/* File hash compare error:
 *
 * 1. Remove temporary bad file
 * 2. Do nothing
 * 3. Request hash again
 * 4. Use file
 * 5. Retransmit file again
 * 6. Check and pepair file
*/

/* File transmiting steps:
 *
 *
 * [DPN_PACKETTYPE__REQUEST_FILE]
 * 1. [host]: send file request:
 * - check file descriptor
 * - check file receiving state
 * - check shift
 * - get hash of data if shift != 0
 * - create receive transport
 * >>> FAULT:
 * @ clear receive transport
 * @ no need answer for server
 *
 *
 * [DPN_PACKETTYPE__REQUEST_FILE]
 * 2. [server]: process  by host:
 * - check file sending state
 * - check key
 * - compare name and size
 * - check file existing
 * - compare actual size and stored size
 * - compare hash of parts if need (with host shift and(or) actual server changes)
 * >>> FAULT:
 * @ send answer about fail to host
 *
 *
 * [DPN_PACKETTYPE__FILE_TRANSMIT_PREPARE]
 * 3. [server]: prepare to sending:
 * - create send transport
 * - open real file in read mode (if need - with shift (seek))
 * >>> FAULT:
 * @ close file, clear send transport
 * @ send answer about fail to host
 *
 *
 * [DPN_PACKETTYPE__FILE_TRANSMIT_PREPARE]
 * 4. [host]: prepare to receiving:
 * - get receiving transport created on step [1]
 * - open file in write or append mode (create file and dirs if need)
 * >>> FAULT:
 * @ close file, clear receive transport
 * @ send answer about fail to server
 *
 *
 * [DPN_PACKETTYPE__TRANSMIT_ANSWER]
 * 5. [host]: send ready or fault of preparing answer
 * >>> FAULT:
 * @ close file, clear receive transport
 * @ send answer about fail to server
 *
 *
 * [DPN_PACKETTYPE__TRANSMIT_ANSWER]
 * 6. [server]: process answer:
 * - if successful: get sending transport created on step [3] and push [DPN_PACKETTYPE__FILE_PACKET]
 * - if fault: clear sending transport
 * >>> FAULT:
 * @ close file, clear transport
 * @ send answer about fail to host
 *
 */

/* File transmiting faults:
 *
 * 1. [host, request file]: file already receiving
 * + 2. [host, request file]: shift == size, file already received at all
 *
 * 3. [server, request file]: file already sending
 * + 4. [server, request file]: BADFILE from catalog (bad key)
 * + 5. [server, request file]: wrong name/size
 * + 6. [server, request file]: actual_size == 0, No file
 *
 * ---------------------------------------------------------------------
 * + 7. [server, request file]:
 *     actual_size != size - file changed on server
 *     shift != 0 - host already has part of file (shift bytes)
 *     actual_size < shift - data on server now less than host download before
 *
 *
 * ? 8. [server, request file]:
 *      actual_size != size - file changed on server
 *      shift != 0 - host already has part of file (shift bytes)
 *      actual_size >= shift - shift bytes on host and same bytes on server
 *  can be equal -> compare hash
 *      same hash
 *
 *
 * ? 9. [server, request file]: same, but wrong hash
 * 10. [server, request file]: actual_size != size, shift == 0
 *
 * + a. [server, request file]: actual_size == size, shift != 0, same hash
 * b. [server, request file]:
 *      actual_size == size,
 *      shift != 0,
 *      different hash
 *      (server file data changed (but still same size) or wrong receiving before)
 *
 *
 * + c. [server, request file]: actual_size == size, shift == 0
 *      usual case, just transmit file
 * ---------------------------------------------------------------------
 * 11. [server, transmit prepare]: startSendHostFile fail
 * 12. [server, transmit prepare]: open, start or buffer fail
 * 13. [host, transmit prepare]: no receiving transport
 * 14. [host, transmit prepare]: open or start fail
 *
 * 15. [host, request file]: inner errors
 * 16. [server, request file]: inner errors
 * 17. [server, transmit prepare]: inner errors
 * 18. [server, answer proc]: inner errors
*/

/* External DPN module:
 *
 * 1. Trasnmit code (directions)
 * 2. Main code (DPN_MediaSystem)
 * 3. Processors for main channel (media processors)
 * 4. Global data (media system instance in core)
 * 5. Client unique data (calls history)
 * 6. Interface (DPN_MediaClient)
 *
*/

/*
 * multicontents in processors
 * processors key-binding
 * no packet types enum ?
*/


#define DPN_CHECK_PORT(port) (port > 0 && port <= 65535)
#define DPN_CONNECTION_FAULT_PROCESSING
#define DPN_DEFAULT_CLIENT_BUFFER_SIZE (1024 * 5)
#define DPN_MAXIMUM_THREADS (4)
#define DPN_CALL_MEMBER_FN(object,ptrToMember)  ((object)->*(ptrToMember))
#define DPN_DEFAULT_MAX_PACKET_SIZE (1024 * 10)
#define DPN_DEFAULT_MAX_FILE_PACKET_SIZE (1024 * 1024 * 64)
//#define DPN_DEFAULT_MAX_FILE_PACKET_SIZE (INT_MAX)

#define DPN_SETTINGS_MARK
#define DPN_INCLUDE_FFMPEG_FOR_MEDIA
#define DPN_CALL_LOG DL_INFO(1, "object: [%p]", this);


#define GET_NAME(X) \
    case X: return "["#X"]"; break
//======================================
#define DPN_THREAD_GUARD(M) std::lock_guard<std::mutex> lock(M)
#define DPN_THREAD_GUARD2(M) std::lock_guard<std::mutex> lock2(M)

namespace DPN {
    namespace Util {}
    namespace Thread {}
}

namespace DPN {
    namespace Util {
        template <class Object>
        class SimplePool {
        public:
            inline Object * get() {
                if( aObjects.empty() ) return new Object;

                auto o = aObjects.back();
                aObjects.pop_back();
                return o;
            }
            inline void set(Object *o) {
                aObjects.push_back(o);
            }
            inline bool empty() const {return aObjects.empty();}
        private:
            DArray<Object*> aObjects;
        };

        template <class T>
        class ThreadSafeList {
        public:
            struct Node {
                friend class ThreadSafeList;

                Node( T *object = nullptr ) :
                    pObject(object), pPrev(nullptr), pNext(nullptr)
                {}
                inline void set( T *o) { pObject = o; }
                inline void exclude() {
                    if( pPrev ) pPrev->pNext = pNext;
                    if( pNext ) pNext->pPrev = pPrev;

                    pPrev = nullptr;
                    pNext = nullptr;
                    pObject = nullptr;
                }
                inline void connect( Node *n ) {
                    pNext = n;
                    n->pPrev = this;
                }
                inline Node * next() { return pNext; }

            private:
                T *pObject;
                Node *pPrev;
                Node *pNext;
            };
        public:
            void push_back( T *o ) {

                DPN_THREAD_GUARD( iMutex );

                Node *node = getNode(o);
                if( pHead ) pHead->connect( node );
                pHead = node;
                //---------------------------
                if( pFirst == nullptr )  pFirst = node;
                //---------------------------
            }
            void pop_back() {
                DPN_THREAD_GUARD( iMutex );
                innerPopBack();
            }
            T * last() {
                if( pHead == nullptr ) return nullptr;

                DPN_THREAD_GUARD( iMutex );

                return pHead->pObject;
            }
            const T * last() const {
                if( pHead == nullptr ) return nullptr;

                DPN_THREAD_GUARD( iMutex );

                return pHead->pObject;
            }
            T * takeLast() {
                if( pHead == nullptr ) return nullptr;

                DPN_THREAD_GUARD( iMutex );
                T *o = pHead->pObject;
                innerPopBack();
                return o;
            }
            void clear() {
                DPN_THREAD_GUARD( iMutex );

                Node *b = pFirst.load();
                Node *e = pHead;
                while(b != e) {
                    removeNode(b);
                    b = b->next();
                }
                zero();
            }
            void moveTo( ThreadSafeList &list ) {

                DPN_THREAD_GUARD( list.iMutex );
                
                if( list.pFirst == nullptr ) {
                    list.pFirst = pFirst;
                    list.pHead = pHead;
                } else {
                    list.pHead->connect( pFirst );
                    list.pHead = pHead;
                }

                DPN_THREAD_GUARD2( iMutex );
                zero();
            }
            inline bool empty() const { return pFirst == nullptr; }
        private:
            inline void innerPopBack() {
                if( pHead ) {
                    Node *h = pHead;
                    pHead = h->pPrev;
                    removeNode( h );
                }
            }
            void zero() {
                pFirst = nullptr;
                pHead = nullptr;
            }
            inline Node * getNode( T *o ) {

                Node *n = iNodePool.get();
                n->set( o );
                return n;

                return nullptr;
            }
            inline void removeNode( Node *n ) {
                n->exclude();
                iNodePool.set( n );
            }
        private:
            std::mutex iMutex;
            std::atomic<Node*> pFirst;
            Node *pHead;
            SimplePool<Node> iNodePool;
        };
    }
}

extern std::mutex __global_mutex;

namespace DPeerNodeSpace {
    extern DLogs::DLogsContext log_context;
    extern DLogs::DLogsContextInitializator logsInit;
}
enum dpn_direction {
    DPN_FORWARD
    , DPN_BACKWARD
};
enum DPN_Result {
    DPN_FAIL = 0,
    DPN_SUCCESS = 1,
    DPN_REPEAT = 2
};
#define DEF_FLAG(FLAG, V) FLAG = (1 << V)


template <class FlagType> static void setFlag(FlagType &to, uint64_t f) { to |=  f; }
template <class FlagType> static void disableFlag(FlagType &to, uint64_t f) { to &= ~f; }
template <class FlagType> static void inverseFlag(FlagType &to, uint64_t f) { to ^=  f; }
template <class FlagType> static void clearFlags(FlagType &flags) {flags = 0;}
template <class FlagType1, class FlagType2> static bool checkFlag(FlagType1 flags, FlagType2 f)  {return flags & f;}

typedef uint64_t dpn_flag;
enum IOBehaivor {

    IO_NO      =    0,
    DEF_FLAG(IO_DEVICE, 1),
    DEF_FLAG(IO_UDP, 2),
    DEF_FLAG(IO_TCP, 3),
    DEF_FLAG(IO_MEM, 4),
    DEF_FLAG(IO_FILE, 5),
    DEF_FLAG(IO_CALLBACK, 6),
    DEF_FLAG(IO_SPECIFIC, 7),
    DEF_FLAG(IO_INNER, 8)
};
class DPN_Task {
public:
    DPN_Task() {__autoremovable = true;}
    virtual ~DPN_Task() {}
    virtual DPN_Result proc() = 0;

    inline bool isRemovable() const {return __autoremovable;}
    void setRemovable(bool s) {__autoremovable = s;}
private:
    bool __autoremovable;
};
template <class Object>
class DPN_SimplePool {
public:
    ~DPN_SimplePool() {
        FOR_VALUE(__pool.size(), i) {
            delete __pool[i];
        }
    }
    inline Object * get() {
        if(__pool.empty()) return new Object;

        auto o = __pool.back();
        __pool.pop_back();
        return o;
    }
    inline void set(Object *o) {
        __pool.push_back(o);
    }
    inline bool empty() const {return __pool.empty();}
private:
    DArray<Object*> __pool;
};
template <class T>
class DPN_SimplePtrList {
public:
    DPN_SimplePtrList() : __first(nullptr), __head(nullptr), __actual(nullptr) {}

    struct __node {
        __node() : ptr(nullptr), prev(nullptr), next(nullptr) {}
        __node(T *__p) : ptr(__p), prev(nullptr), next(nullptr) {}
        inline void set_proc(T *__p) {ptr = __p;}
        inline void exclude() {

            if(prev) prev->next = next;
            if(next) next->prev = prev;

        }
        T *ptr;
        __node *prev;
        __node *next;
    };

    void push_back(T *p) {

        __node *node = getNode(p);
        if(__head) {
            __head->next = node;
            node->prev = __head;
        }
        if(!__first) __first = node;
        __head = node;

        if(!__actual) __actual = node;
    }
    T * getActual() {
        return __actual ? __actual->ptr : nullptr;
    }
    void popActual() {
        if(!__actual) return;

        // connect sides
        __actual->exclude();

        // change first & head if need
        if(__actual == __first) __first = __actual->next;
        if(__actual == __head) __head = __actual->prev;

        // move actual
        skip();
    }
    void skip() {

        if(__actual->next) __actual = __actual->next;
        else __actual = __first;

    }
    void restart() {
        __actual = __first;
    }
    void moveTo(DPN_SimplePtrList &list) {
        auto f = list.__first;

        if( __first ) {
            if( !__head || !f ) {
                using namespace DPeerNodeSpace;
                DL_BADPOINTER(1, "head: [%p] f: [%p]", __head, f);
                return;
            }
            __head->next = f;
            f->prev = __head;
            __head = list.__head;
        } else {
            __first = f;
            __head = list.__head;
            __actual = __first;
        }
        list.zero();
    }
    void clear() {
        while(__first != __head) {
            removeNode(__first);
            __first = __first->next;
        }
        zero();
    }

    inline bool empty() const {return __first == nullptr;}
    inline bool noEmpty() const {return __first;}

private:
    void push_front(T *p) {

        __node *node = getNode(p);
        if(__first) {
            node->next = __first;
            __first->prev = node;
        }
        if(!__head) __head = node;
        __first = node;
        if(!__actual) __actual = node;
    }
    void insert_to_actual(T *p) {
        __node *node = getNode(p);

        if(!__actual) return push_front(p);
        if(__actual == __first) return push_front(p);
        if(__actual->prev)
            __actual->prev->next = node;

        node->prev = __actual->prev;
        node->next = __actual;

        __actual->prev = node;
    }
private:

    void zero() {
        __first = nullptr;
        __head = nullptr;
        __actual = nullptr;
    }
    inline __node * getNode(T *p) {

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
template <class T>
class DPN_ThreadSafeList {
    struct __node {
        __node() : ptr(nullptr), prev(nullptr), next(nullptr) {}
        __node(T *__p) : ptr(__p), prev(nullptr), next(nullptr) {}
        inline void set_proc(T *__p) {ptr = __p;}
        inline void exclude() {

            if(prev) prev->next = next;
            if(next) next->prev = prev;

        }
        T *ptr;
        __node *prev;
        __node *next;
    };
private:
    __node *__first;
    __node *__head;
    __node *__actual;
};

class DPN_TaskManager {
public:
    DPN_Task * pullTask() {

        if(doit.empty()) return nullptr;
        auto task = doit.front();
        doit.pop_front();
        return task;
    }
    void pushTask(DPN_Task *task) {

        doit.push_back(task);
    }

    int size() const {return doit.size();}
private:
    DArray<DPN_Task*> doit;
};
//======================================
template <class T>
class dpn_pointer {
public:
    dpn_pointer(T *p = nullptr) : ptr(p) {}

    inline operator T*() {return ptr;}
    inline operator const T*() const {return ptr;}

    inline dpn_pointer & operator=(T *p) {ptr = p;}
    inline dpn_pointer & operator=(const dpn_pointer &p) {ptr = p.ptr;}

    inline T* operator->() {return ptr;}
    inline const T* operator->() const {return ptr;}

    inline T* pointer() {return ptr;}
    inline const T* pointer() const {return ptr;}

    inline void set(T *p) {ptr = p;}
    inline void clear() {ptr = nullptr;}

    inline void deleteMe() { if(ptr) {delete ptr; ptr = nullptr;} }
    inline void freeMe() { free_mem(ptr); ptr = nullptr; }

    inline bool isNull() const {return ptr == nullptr;}
    inline bool isPointer() const {return ptr;}
private:
    T *ptr;
};
#define PTR(T) dpn_pointer<T>
//======================================
struct dpn_local_date {
    int day;
    int month;
    int year;
};
struct dpn_local_time {
    int usec;
    int second;
    int minute;
    int hour;
};
struct dpn_local_moment {
    dpn_local_date date;
    dpn_local_time time;
};
struct dpn_time_range {
    void fill(time_t __seconds);
    void fill2(int __useconds);

    int useconds;
    int seconds;
    int minutes;
    int hours;
    int days;
};
class DPN_TimeMoment {
public:
    explicit DPN_TimeMoment(bool fix = false);
    void fixTime();
    void clear();

    void setLocalTime(int s, int u);

    struct timeval lt() const;

    uint64_t lt_useconds() const;
    time_t lt_seconds() const;
    time_t rt_seconds() const;

    dpn_local_date date() const;
    dpn_local_time time() const;
    dpn_local_moment moment() const;
    dpn_time_range runTime() const;

    std::string dateString() const;
    std::string timeString() const;
    std::string momentString() const;
    std::string runTimeString() const;

    inline bool empty() const {return __localTime == 0;}
    inline bool fixed() const {return __localTime != 0;}

private:
    struct timeval __lt_;
private:
    time_t __localTime;
    time_t __runTime;
};
class DPN_TimeRange {
public:
    explicit DPN_TimeRange(bool fixFirst = false);

    void fixBegin();
    void fixLast();
    void clear();
    void clearLast();

    inline const DPN_TimeMoment & begin() const {return __begin;}
    inline const DPN_TimeMoment & last() const {return __last;}

    uint64_t useconds() const;
    time_t seconds() const;
    dpn_time_range range() const;

    inline bool empty() const {return __begin.empty() && __last.empty();}
    inline bool isCompleted() const {return __begin.fixed() && __last.fixed();}
    inline bool isOpen() const {return __begin.fixed() && __last.empty();}

private:
    DPN_TimeMoment __begin;
    DPN_TimeMoment __last;
};
//======================================
class DPN_SHA256 {
public:
    DPN_SHA256() {
        context = nullptr;
        context = sha256__create_context();
    }
    ~DPN_SHA256() {
        sha256__free_context(context);
    }

    inline int hash_data(uint8_t *data, size_t size) {
        clear();
        return sha256__hash_data(data, size, context);
    }
    inline int hash_file(const char *path, size_t bufferSize, size_t firstPartToHash = 0) {
        clear();
        return sha256__hash_file(path, bufferSize, context, firstPartToHash);
    }
    inline bool hash_file_segment(const std::string &path, size_t start, size_t segmentSize) {
        clear();
        if( sha256__hash_file_segment(path.c_str(), 1024, context, start, segmentSize) < 0 ) return false;
        return true;
    }
    inline bool hash_string(const std::string &s) {
        clear();
        if( sha256__hash_data(reinterpret_cast<const uint8_t*>(s.data()), s.size(), context) < 0 ) {
            return false;
        }
        return true;
    }

    inline std::string get() const { return sha256_hash_to_string(context); }
    inline void print() const { sha256__print_hash(context); }

    void clear() {
        sha256__clear_context(context);
    }
private:
    SHA256Context *context;
};
//======================================
struct PeerAddress {
    PeerAddress() {
        port = -1;
    }
    PeerAddress(int __port, const std::string __address) {
        port = __port;
        address = __address;
    }
    inline bool operator==(const PeerAddress &o) const {return port == o.port && address == o.address;}
    inline bool operator!=(const PeerAddress &o) const {return port != o.port || address != o.address;}
    std::string name() const {
        std::string n = address + ":" + std::to_string(port);
        return n;
    }

    inline bool correct() const {return DPN_CHECK_PORT(port) && address.size() > 6;}
    int port;
    std::string address;
};
//======================================
//======================================
struct ActionState {
    enum {
        Empty,
        Requesting,
        Ready,
        Fail
    } iS;
};
template <class TagType>
class DPN_StateManager {
public:
    ActionState & state(TagType tag) {
        auto f = iStateMap.find(tag);
        if( f == iStateMap.end() ) {
            iStateMap[tag] = ActionState();
        }
        return iStateMap[tag];
    }
private:
    std::map<TagType, ActionState> iStateMap;
};
//======================================


class __channel_mono_interface;
class DPN_ExpandableBuffer;
class DPN_ClientTag;

class DPN_AbstractModule {
public:
    DPN_AbstractModule(const std::string &name) : iModuleName(name) {}
    virtual ~DPN_AbstractModule() {}
    virtual void stop() {};
    virtual bool useChannel( const DPN_ClientTag *tag, dpn_direction d, __channel_mono_interface mono, const DPN_ExpandableBuffer &context ) = 0;


    const std::string & name() const {return iModuleName;}
private:
    std::string iModuleName;
};



enum DPN_ClientSystemMessage {
    DPN_CSM__CHECK
    ,DPN_CSM__GLOBAL_ENVIRONMENT
};


//======================================
template <class Q> class __interface_master;
template <class T>
class __dpn_interface : private DWatcher<T> {
public:
    __dpn_interface() {}

    __dpn_interface( DWatcher<T> &w, __interface_master<T> *parent) {
        pParent = parent;
        this->copy( w );
    }
    void move( __dpn_interface &o ) {
        pParent = o.pParent;
        this->moveFrom( o );
        o.pParent = nullptr;
    }
    inline bool validInterface() const {return this->isCreatedObject();}
    inline bool badInterface() const {return this->isEmptyObject();}
    ~__dpn_interface() {close();}
    void close();


protected:
    inline T * inner() {return this->data();}
private:
    __interface_master<T> *pParent;
};
template <class T>
class __interface_master {
public:

    __dpn_interface<T> get( DWatcher<T> &w ) {

        if( iUsing ) return __dpn_interface<T>();
        iUsing = true;
        return __dpn_interface<T>( w, this );

    }

    void close() {
        iUsing = false;
    }
private:
    bool iUsing;
};
template <class T> void __dpn_interface<T>::close() {
    if( pParent) pParent->close();
}


template <class t1, class t2> class __interface_map;

template < class Key, class I >
class __dpn_acc_interface : private DWatcher<I> {
public:
    __dpn_acc_interface() {
        pParent = nullptr;
    }
    __dpn_acc_interface( DWatcher<I> &w, const Key &key, __interface_map<Key, I> *parent) {
        pParent = parent;
        iKey = key;
        this->copy( w );
    }
    void move( __dpn_acc_interface &o ) {
        pParent = o.pParent;
        iKey = o.iKey;
        this->moveFrom( o );
        o.pParent = nullptr;
    }

    inline Key & key() { return iKey; }


public:
    inline bool validInterface() const {return this->isCreatedObject();}
    inline bool badInterface() const {return this->isEmptyObject();}
    ~__dpn_acc_interface() { if( this->isCreatedObject() ) close(); }
    void close();

protected:
    inline I * inner() {return this->data();}
    inline const I * inner() const { return this->data(); }
private:
    __interface_map<Key, I> *pParent;
    Key iKey;
};
template < class Key, class I >
class __interface_map {
public:
    __dpn_acc_interface<Key, I> get( const Key &k, DWatcher<I> &w ) {

        if( iMap.find( k ) == iMap.end() ) iMap[k] = false;
        bool &u = iMap[k];
        if( u ) return __dpn_acc_interface<Key, I>();
        u = true;
        return __dpn_acc_interface<Key, I>( w, k, this );
    }
    void close( const Key &k ) {
        auto f = iMap.find( k );
        if( f != iMap.end() ) f->second = false;
    }
private:
    std::map<Key, bool> iMap;
};
template <class K, class T> void __dpn_acc_interface<K, T>::close() {
    if( pParent) pParent->close( iKey );
}
//======================================

static std::string ia2s(const DArray<int> &a) {
    std::string s;
    s.append("[");
    FOR_VALUE(a.size(), i) {
        s.append(std::to_string(a[i]));
        s.append(", ");
    }
    s.append("]");
    return s;
}
//======================================
template <class T>
class __action_unit {
public:

    __action_unit<T> *next;
    typedef DPN_Result (T::*Action)();
    __action_unit(Action __a) : a(__a), next(nullptr), iLevel(0) {}
    void setLevel(int l) {iLevel = l;}
    inline int level() const {return iLevel;}
    DPN_Result action(T *pObj) {
        using namespace DPeerNodeSpace;
        if(pObj == nullptr || a == nullptr ) {
            DL_BADPOINTER(1, "object [%p] or action [%p]", pObj, a);
            return DPN_FAIL;
        }
        return DPN_CALL_MEMBER_FN(pObj, a)();
    }
private:
    Action a;
    int iLevel;
};
class __base_action_line {
public:
    inline void setTarget(void *target) { pObject = target; }
    inline DPN_Result lastResult() const {return iLastResult;}
    virtual DPN_Result doStep() = 0;
    virtual DPN_Result doStep( int level ) = 0;
    virtual bool isOver() const = 0;
protected:
    void *pObject;
    DPN_Result iLastResult;
};
template <class T>
class __action_line : public __base_action_line {
public:
    typedef DPN_Result (T::*Action)();
    __action_line(T *targetObject = nullptr)  {
        start = nullptr;
        head = nullptr;
        current = nullptr;
        fail = nullptr;
        iStep = 0;
        pObject = targetObject;
    }
    void setLine(__action_line<T> l) {
        start = l.start;
        head  = l.head;
        fail  = l.fail;
        current = start;
    }
    __action_line & operator<< (Action a) {
        return add(new __action_unit<T>(a));
    }
    __action_line & operator<< (int level ) {
        if( head ) {
            head->setLevel( level );
        }
        return *this;
    }

    DPN_Result go() {

        using namespace DPeerNodeSpace;
        if( current == nullptr ) {
            DL_BADPOINTER(1, "current");
            return DPN_FAIL;
        }

        DPN_Result r = DPN_FAIL;
        while(current) {

            r = current->action(reinterpret_cast<T*>( pObject ));
            if(r == DPN_SUCCESS) {
                current = current->next;
                ++iStep;
            } else {
                return r;
            }
        }
        return DPN_SUCCESS;

    }
    DPN_Result doStep() override {

        using namespace DPeerNodeSpace;
        if( current == nullptr ) {
            DL_BADPOINTER(1, "current");
            return DPN_FAIL;
        }

        DL_INFO(1, "action line: [%p] current: [%p]", this, current);
        iLastResult = current->action(reinterpret_cast<T*>( pObject ));

        if( iLastResult == DPN_SUCCESS ) {
            ++iStep;
            if( (current = current->next) == nullptr ) return DPN_SUCCESS;
        }
        if( iLastResult == DPN_FAIL && fail ) {
//            fail->action(pObj);
            return DPN_FAIL;
        }
        return iLastResult;
    }
    DPN_Result doStep( int level ) override {

        using namespace DPeerNodeSpace;

        while( current && current->level() != level ) current = current->next;

        if( current == nullptr ) {
            DL_BADPOINTER(1, "current");
            return DPN_FAIL;
        }


        DL_INFO(1, "action line: [%p] current: [%p]", this, current);
        iLastResult = current->action(reinterpret_cast<T*>( pObject ));

        if( iLastResult == DPN_SUCCESS ) {
            ++iStep;
            if( (current = current->next) == nullptr ) return DPN_SUCCESS;
        }
        if( iLastResult == DPN_FAIL && fail ) {
//            fail->action(pObj);
            return DPN_FAIL;
        }
        return iLastResult;
    }
    void doFailStep() {
        if( fail ) {
            fail->action(reinterpret_cast<T*>( pObject ));
            iLastResult = DPN_FAIL;
            current = nullptr;
        }
    }
    void restart() {
        current = start;
        iStep = 0;
    }
    void setFailStep(Action a) {
        fail = new __action_unit<T>(a);
    }

    int step() const {return iStep;}

    inline bool isOver() const override {return current == nullptr;}
    bool empty() const {return start == nullptr;}
private:
    __action_line & add(__action_unit<T> *u) {
        if( start == nullptr ) start = u;
        if( current == nullptr ) current = start;
        if( head ) head->next = u;
        head = u;
        return *this;
    }
private:

    __action_unit<T> *start;
    __action_unit<T> *head;
    __action_unit<T> *current;
    __action_unit<T> *fail;


    int iStep;
};
//=====================================
class DPN_ClientTag {

};

//=====================================
int pointerValue(const void *ptr);
#endif // __DPEERNODE_GLOBAL_H
