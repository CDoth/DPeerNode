#ifndef __DPEERNODE_GLOBAL_H
#define __DPEERNODE_GLOBAL_H



//#define DL_ERROR(f1, f2, ...)
//#define DL_FUNCFAIL(f1, f2, ...)
//#define DL_BADPOINTER(f1, f2, ...)

//----------------------
#include <DLogs.h>
#include <sha256.h>
//----------------------
#include <mutex>
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
class ModuleSystem1 {
public:
    void moduel1_test();
};
class ModuleSystem2 {
public:
    void moduel2_test();
};
template <class ModuleType>
class ModuleWrapper {

public:
    void setPtr(ModuleType *__p) {ptr = __p;}
    ModuleType * getPtr() {return ptr;}


    ModuleType *ptr;
};

template <class T>
class ModuleAccessor {};


class ModuleContainer :
         ModuleWrapper<ModuleSystem1>,
         ModuleWrapper<ModuleSystem2>
{
//    friend void test();
//    friend void test2();

    template <class T>
    friend class ModuleAccessor;

};

#define BIND_MODULE_TO_FUNCTION(MODULE, FUNCTION) \
    template <> class ModuleAccessor<MODULE> { \
    friend FUNCTION; \
    inline static MODULE * ptr(ModuleContainer &mc) {return mc.ModuleWrapper<MODULE>::getPtr();} \
    };


//template <>
//class ModuleAccessor<ModuleSystem1> {
//    friend void test();
//    inline static ModuleSystem1 * ptr(ModuleContainer &mc) { return mc.ModuleWrapper<ModuleSystem1>::getPtr(); }
//};
//template <>
//class ModuleAccessor<ModuleSystem2> {
//    friend void test2();
//    ModuleSystem2 * ptr(ModuleContainer &mc) {
//        return mc.ModuleWrapper<ModuleSystem2>::getPtr();
//    }
//};

BIND_MODULE_TO_FUNCTION(ModuleSystem1, void test());
BIND_MODULE_TO_FUNCTION(ModuleSystem2, void test());



#define GET_MODULE(TYPE)  mc.ModuleWrapper<TYPE>::getPtr()
//void test() {
//    ModuleContainer mc;

//    mc.ModuleWrapper<ModuleSystem1>::setPtr(nullptr);
//    mc.ModuleWrapper<ModuleSystem2>::setPtr(nullptr);

//    ModuleAccessor<ModuleSystem1> ac;
//    ac.ptr(mc);


//    ModuleAccessor<ModuleSystem2> ac2;
//    ac2.ptr(mc);


//}
//void test2() {

//    ModuleContainer mc;

//    mc.ModuleWrapper<ModuleSystem1>::setPtr(nullptr);
//    mc.ModuleWrapper<ModuleSystem2>::setPtr(nullptr);

//    ModuleAccessor<ModuleSystem2> ac;
//    ac.ptr(mc);
//}

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



#define GET_NAME(X) \
    case X: return "["#X"]"; break
//======================================
extern std::mutex __global_mutex;
#define DPN_THREAD_GUARD(M) std::lock_guard<std::mutex> lock(M)

namespace DPeerNodeSpace {
    extern DLogs::DLogsContext log_context;
    extern DLogs::DLogsContextInitializator logsInit;
}
enum DPN_DirectionType {
    DPN_FORWARD
    , DPN_BACKWARD
};

enum DPN_Result {
    DPN_SUCCESS
    ,DPN_REPEAT
    ,DPN_KEEP
    ,DPN_FAIL
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
    inline int hash_file(const char *path, size_t bufferSize, size_t partToHash = 0) {
        clear();
        return sha256__hash_file(path, bufferSize, context, partToHash); }

    inline int hash_string(const std::string &s) {
        clear();
        return sha256__hash_data(reinterpret_cast<const uint8_t*>(s.data()), s.size(), context);
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
class DPN_DialogManager;
class DPN_AbstractClient;
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
//class DPN_TransactionMaster;
class DPN_TransactionMap;
class DPN_AbstractTransaction;
class DPN_Channel;
class DPN_AbstractConnectionsCore;
class DPN_ExpandableBuffer;

class DPN_AbstractModule {
public:
    DPN_AbstractModule(const std::string &name) : iModuleName(name), pCore(nullptr) {}
    virtual ~DPN_AbstractModule() {}
    void setCore(DPN_AbstractConnectionsCore *core) {pCore = core;}

    virtual bool reserveShadowReceiver(DPN_Channel *channel, const DPN_ExpandableBuffer &extra, int transaction) {return false;}
    virtual bool reserveShadowSender(DPN_Channel *channel, int transaction) {return false;}
    virtual void unreserveShadowReceiver(const std::string &shadowKey) {};
    virtual void unreserveShadowSender(const std::string &shadowKey) {};

    virtual void clientDisconnected(const DPN_AbstractClient *client) = 0;
    virtual void stop() = 0;

    const std::string & name() const {return iModuleName;}
    DPN_AbstractConnectionsCore * core() {return pCore;}
    const DPN_AbstractConnectionsCore * core() const {return pCore;}
private:
    std::string iModuleName;
    DPN_AbstractConnectionsCore *pCore;
};

//======================================
struct ClientInitContext;
class DPN_ClientContext;
class DPN_UDPPort;
class DPN_Direction;

enum DPN_ClientSystemMessage {
    DPN_CSM__CHECK
    ,DPN_CSM__GLOBAL_ENVIRONMENT
};
class DPN_AbstractClient {
public:
    virtual bool disconnect() = 0;
    virtual const PeerAddress & localAddress() const = 0;
    virtual const PeerAddress & remoteAddress() const = 0;
    virtual const PeerAddress & avatar() const = 0;

    virtual DPN_TransactionMap & clientTransactions() = 0;
    virtual DPN_TransactionMap & generatedTransactions() = 0;


    virtual void setEnvironment(const DArray<PeerAddress> &env) = 0;

    virtual const std::string & getSessionKey() const = 0;
    virtual void verifyShadow(const PeerAddress &local, const PeerAddress &remote, const std::string &key) = 0;
    virtual void registerShadow(const PeerAddress &local, const PeerAddress &remote, const std::string &key) = 0;
    virtual DPN_Result checkShadowPermission
    (const PeerAddress &source, const PeerAddress &target, const std::string &sessionKey, const std::string &shadowKey) const = 0;
    virtual bool addShadowConnection(const ClientInitContext &i) = 0;
    virtual DPN_Channel * channel(const std::string shadowKey) = 0;

    virtual void shadowConnectionServerAnswer(bool a, int port) = 0;

    virtual DPN_ClientContext context() = 0;
};
class DPN_AbstractConnectionsCore {
public:
    virtual void toEachClient(DPN_ClientSystemMessage m, DPN_AbstractClient *sourceForException) = 0;
    virtual void pushTask(DPN_Task *t) = 0;

    virtual bool isShadowAvailable(DPN_AbstractClient *client, int port) = 0;
    virtual void shadowConnection(DPN_AbstractClient *client, int port) = 0;
    virtual DPN_AbstractClient * client(const PeerAddress &pa) = 0;
    virtual DPN_UDPPort * openUDPPort(int port) = 0;

    virtual void replanDirections() = 0;
    virtual void addGlobalDirection(DPN_Direction *d) = 0;
    virtual void disconnect(DPN_AbstractClient *c) = 0;

    virtual DArray<PeerAddress> getEnvironment() const = 0;
};

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
    __action_unit(Action __a) : a(__a), next(nullptr) {}
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
};

template <class T>
class __action_line {
public:
    typedef DPN_Result (T::*Action)();
    __action_line(T *targetObject = nullptr) : pObj(targetObject) {
        start = nullptr;
        head = nullptr;
        current = nullptr;
        fail = nullptr;
        iStep = 0;
    }
    void setTarget(T *obj) {
        pObj = obj;
    }
    void setLine(__action_line<T> l) {
        start = l.start;
        head  = l.head;
        fail  = l.fail;
    }
    __action_line & operator<< (Action a) {
        return add(new __action_unit<T>(a));
    }

    DPN_Result go() {

        using namespace DPeerNodeSpace;
        if( current == nullptr ) {
            DL_BADPOINTER(1, "current");
            return DPN_FAIL;
        }

        DPN_Result r = DPN_FAIL;
        while(current) {

            r = current->action(pObj);
            if(r == DPN_SUCCESS) {
                current = current->next;
                ++iStep;
            } else {
                return r;
            }
        }
        return DPN_SUCCESS;

    }
    DPN_Result doStep() {

        using namespace DPeerNodeSpace;
        if( current == nullptr ) {
            DL_BADPOINTER(1, "current");
            return DPN_FAIL;
        }

        DL_INFO(1, "action line: [%p] current: [%p]", this, current);
        iLastResult = current->action(pObj);

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
            fail->action(pObj);
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
    DPN_Result lastResult() const {return iLastResult;}
    bool isOver() const {return current == nullptr;}
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
    T *pObj;
private:

    __action_unit<T> *start;
    __action_unit<T> *head;
    __action_unit<T> *current;
    __action_unit<T> *fail;

    DPN_Result iLastResult;
    int iStep;
};
//=====================================
int pointerValue(const void *ptr);
#endif // __DPEERNODE_GLOBAL_H
