#ifndef DPN_THREADBRIDGE_H
#define DPN_THREADBRIDGE_H
#include <DWatcher.h>
#include <thread>


#include "__dpeernode_global.h"
#include "DPN_ThreadUnit.h"



namespace DPN::Thread {

    enum Policy {
        THIS_THREAD, DAEMON_THREAD
    };

    //----------------------------------------------------------------
    struct __unit_pool__ {
        __unit_pool__();
        DPN::Util::ThreadSafeList<DPN_ThreadUnit> iPool;
        int iSize;
    };
    class UnitPoolInterface : private DWatcher< __unit_pool__ > {
    public:
        UnitPoolInterface( bool makeSource );
        UnitPoolInterface( UnitPoolInterface &share );

        DPN_ThreadUnit * get();
        void put( DPN_ThreadUnit *unit );
        void put( const DArray<DPN_ThreadUnit *> &u );
    };
    //----------------------------------------------------------------
    class __thread_core__ : public UnitPoolInterface {
    public:
        friend class ThreadCore;
        __thread_core__( UnitPoolInterface &up );
        ~__thread_core__();
    public:
        void threadcore_run();
    private:
        void threadcore_start();
        void threadcore_iterate();
    private:
        void accept();
        void backAll();
    private:
        std::atomic_bool iPlayable;
        uint64_t threadId;
        std::thread::id stdThreadId;
        DArray<DPN_ThreadUnit*> aAccepted;
    };
    class ThreadCore : private DWatcher< __thread_core__ > {
    public:
        ThreadCore( UnitPoolInterface sharingPool );
        std::thread::id id() const;
        void run();
    };
    inline static void start( ThreadCore tc ) { tc.run(); }
    //----------------------------------------------------------------
    class __thread_user__ : public UnitPoolInterface {
    public:
        __thread_user__();
        bool isThreadFree() const;
        DArray< ThreadCore > aThreads;
    };
    class ThreadUser : DWatcher< __thread_user__ > {
    public:
        ThreadUser( bool makeSource );
        ThreadUser( const ThreadUser &sharing );
        void putUnit( DPN_ThreadUnit *unit );

        void startStream( Policy p );
    private:
        UnitPoolInterface poolIf();
    };
}


/*
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

*/

#endif // DPN_THREADBRIDGE_H
