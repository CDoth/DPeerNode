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
 *
 * 'd' prefix in class name - decriptor (DWatcher) of data subclass
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

#define DEF_FLAG(FLAG, V) FLAG = (1 << V)

//======================================
#define DPN_THREAD_GUARD(M) std::lock_guard<std::mutex> lock(M)
#define DPN_THREAD_GUARD2(M) std::lock_guard<std::mutex> lock2(M)

namespace DPN {

    namespace Logs {}
    namespace Util {}
    namespace Thread {}
    namespace Network {}
    namespace Time {}
    namespace Client {}
    namespace Channel {}
}

namespace DPN {

    enum Direction {
        FORWARD
        ,BACKWARD
    };

    template <class FlagType> static void setFlag(FlagType &to, uint64_t f) { to |=  f; }
    template <class FlagType> static void disableFlag(FlagType &to, uint64_t f) { to &= ~f; }
    template <class FlagType> static void inverseFlag(FlagType &to, uint64_t f) { to ^=  f; }
    template <class FlagType> static void clearFlags(FlagType &flags) {flags = 0;}
    template <class FlagType1, class FlagType2> static bool checkFlag(FlagType1 flags, FlagType2 f)  {return flags & f;}

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

    class SHA256 {
    public:
        SHA256() {
            context = nullptr;
            context = sha256__create_context();
        }
        ~SHA256() {
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
}
namespace DPN::Logs {
    extern DLogs::DLogsContext log_context;
    extern DLogs::DLogsContextInitializator logsInit;
}
namespace DPN::Time {
    struct local_date {
        int day;
        int month;
        int year;
    };
    struct local_time {
        int usec;
        int second;
        int minute;
        int hour;
    };
    struct local_moment {
        local_date date;
        local_time time;
    };
    struct time_range {
        void fill(time_t __seconds);
        void fill2(int __useconds);

        int useconds;
        int seconds;
        int minutes;
        int hours;
        int days;
    };
    class TimeMoment {
    public:
        explicit TimeMoment(bool fix = false);
        void fixTime();
        void clear();

        void setLocalTime(int s, int u);

        struct timeval lt() const;

        uint64_t lt_useconds() const;
        time_t lt_seconds() const;
        time_t rt_seconds() const;

        local_date date() const;
        local_time time() const;
        local_moment moment() const;
        time_range runTime() const;

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
    class TimeRange {
    public:
        explicit TimeRange(bool fixFirst = false);

        void fixBegin();
        void fixLast();
        void clear();
        void clearLast();

        inline const TimeMoment & begin() const {return __begin;}
        inline const TimeMoment & last() const {return __last;}

        uint64_t useconds() const;
        time_t seconds() const;
        time_range range() const;

        inline bool empty() const {return __begin.empty() && __last.empty();}
        inline bool isCompleted() const {return __begin.fixed() && __last.fixed();}
        inline bool isOpen() const {return __begin.fixed() && __last.empty();}

    private:
        TimeMoment __begin;
        TimeMoment __last;
    };
}
namespace DPN::Network {
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
}
namespace DPN {

    //----------------------------------------------
    template <class Q> class InterfaceMaster;
    template <class T>
    class Interface : private DWatcher<T> {
    public:
        Interface() {}

        Interface( DWatcher<T> &w, InterfaceMaster<T> *parent) {
            pParent = parent;
            this->copy( w );
        }
        void move( Interface &o ) {
            pParent = o.pParent;
            this->moveFrom( o );
            o.pParent = nullptr;
        }
        inline bool validInterface() const {return this->isCreatedObject();}
        inline bool badInterface() const {return this->isEmptyObject();}
        ~Interface() {close();}
        void close();
    protected:
        inline T * inner() {return this->data();}
    private:
        InterfaceMaster<T> *pParent;
    };
    template <class T>
    class InterfaceMaster {
    public:

        Interface<T> get( DWatcher<T> &w ) {

            if( iUsing ) return Interface<T>();
            iUsing = true;
            return Interface<T>( w, this );

        }
        void close() {
            iUsing = false;
        }
    private:
        bool iUsing;
    };
    template <class T> void Interface<T>::close() {
        if( pParent) pParent->close();
    }
    //----------------------------------------------
    template <class t1, class t2> class MappedInterfaceMaster;
    template < class Key, class I >
    class MappedInterface : private DWatcher<I> {
    public:
        MappedInterface() {
            pParent = nullptr;
        }
        MappedInterface( DWatcher<I> &w, const Key &key, MappedInterfaceMaster<Key, I> *parent) {
            pParent = parent;
            iKey = key;
            this->copy( w );
        }
        void move( MappedInterface &o ) {
            pParent = o.pParent;
            iKey = o.iKey;
            this->moveFrom( o );
            o.pParent = nullptr;
        }

        inline Key & key() { return iKey; }


    public:
        inline bool validInterface() const {return this->isCreatedObject();}
        inline bool badInterface() const {return this->isEmptyObject();}
        ~MappedInterface() { if( this->isCreatedObject() ) close(); }
        void close();

    protected:
        inline I * inner() {return this->data();}
        inline const I * inner() const { return this->data(); }
    private:
        MappedInterfaceMaster<Key, I> *pParent;
        Key iKey;
    };
    template < class Key, class I >
    class MappedInterfaceMaster {
    public:
        MappedInterface<Key, I> get( const Key &k, DWatcher<I> &w ) {

            if( iMap.find( k ) == iMap.end() ) iMap[k] = false;
            bool &u = iMap[k];
            if( u ) return MappedInterface<Key, I>();
            u = true;
            return MappedInterface<Key, I>( w, k, this );
        }
        void close( const Key &k ) {
            auto f = iMap.find( k );
            if( f != iMap.end() ) f->second = false;
        }
    private:
        std::map<Key, bool> iMap;
    };
    template <class K, class T> void MappedInterface<K, T>::close() {
        if( pParent) pParent->close( iKey );
    }

}

enum DPN_Result {
    DPN_FAIL = 0,
    DPN_SUCCESS = 1,
    DPN_REPEAT = 2
};
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
    virtual bool useChannel( const DPN_ClientTag *tag, DPN::Direction d, __channel_mono_interface mono, const DPN_ExpandableBuffer &context ) = 0;


    const std::string & name() const {return iModuleName;}
private:
    std::string iModuleName;
};
//=====================================
class DPN_ClientTag {};
//=====================================
int pointerValue(const void *ptr);
#endif // __DPEERNODE_GLOBAL_H
