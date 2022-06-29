#ifndef DPN_THREADBRIDGE_H
#define DPN_THREADBRIDGE_H
#include <DWatcher.h>
#include <thread>


#include "__dpeernode_global.h"
#include "DPN_ThreadUnit.h"

class DPN_ThreadBridge;
class DPN_ThreadMaster;

namespace DPN_Thread {

    //----------------------------------------------------------------
    struct __unit_pool__ {
        DPN::Util::ThreadSafeList<DPN_ThreadUnit> iPool;
    };
    class UnitPool : private DWatcher< __unit_pool__ > {
    public:
        DPN_ThreadUnit * get();
        void put( DPN_ThreadUnit *unit );
    };
    //----------------------------------------------------------------
    class __thread_core__ : public UnitPool {

    public:
        void run();
    private:
        void accept();
    private:
        uint64_t threadId;
        std::thread::id stdThreadId;

        DArray<DPN_ThreadUnit*> aPlaned;
        DArray<DPN_ThreadUnit*> aAccepted;
    };
    class ThreadCore : private DWatcher< __thread_core__ > {

    };
    //----------------------------------------------------------------
    struct __thread_user__ {

        DArray<ThreadCore> aThreads;
    };
    class ThreadUser : public UnitPool {
    public:
        ThreadUser( const ThreadUser &sharing );

        void putUnit( DPN_ThreadUnit *unit );
    };
}



struct DPN_ThreadBridgeData {
public:

    friend class DPN_ThreadBridge;

    DPN_ThreadBridgeData();

    void accept();
    void setNoAcceptMode();
    void setReplaceMode();
    bool planDirection(DPN_ThreadUnit *d);
    void clearPlan();

    void procTasks();
    void procDirections();
    void procSingle();
private:
    enum DirectionAcceptMode {
        NO_ACCEPT,
        REPLACE,
        __KEEP_REPLACE
    };
private:

    bool __hold_direction(DPN_ThreadUnit *d);


    std::atomic_bool play;
    std::atomic_bool pause;
    std::atomic<DirectionAcceptMode> acceptMode;
    std::atomic_bool process_tasks;
    std::mutex mutex;
    uint64_t threadId;
    std::thread::id stdThreadId;
    //-------------------------------
    DArray<DPN_ThreadUnit*> planed;
    DArray<DPN_ThreadUnit*> accepted;
    //------------------------------
    std::mutex taskMutex;
};
class DPN_ThreadBridge : public DWatcher<DPN_ThreadBridgeData> {
public:
    DPN_ThreadBridge();
    friend class DPN_ThreadMaster;
public:
    void clearPlan();
    bool planDirection(DPN_ThreadUnit *d);
    void replacePlaned();


    inline void procSingle() {data()->procSingle();}
    inline void procDirections() {data()->procDirections();}
    inline void procTasks() {data()->procTasks();}


    inline bool playable() const {return data()->play;}
    inline bool inPause() const {return data()->pause;}

    int directionsAccepted() const;
    uint64_t threadId() const;

    void setProcessTasks(bool state) {data()->process_tasks = state;}


    inline bool process_tasks() const {return data()->process_tasks;}
private:
    void start();

};
struct __thread_kernel__ {
    DArray<DPN_ThreadBridge> aThreads;
};
class DPN_ThreadKernel : DWatcher< __thread_kernel__ > {
public:
    void replanUnits();
private:

};

#endif // DPN_THREADBRIDGE_H
