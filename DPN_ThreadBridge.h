#ifndef DPN_THREADBRIDGE_H
#define DPN_THREADBRIDGE_H
#include <DWatcher.h>
#include <thread>


#include "__dpeernode_global.h"
#include "DPN_ThreadUnit.h"
#include "DPN_Util.h"



namespace DPN::Thread {

    enum Policy {
        THIS_THREAD, DAEMON_THREAD
    };

    //----------------------------------------------------------------
    struct __unit_pool__ {
        __unit_pool__();
        DPN::Util::_ThreadSafeList< DPN::Thread::ThreadUnit > iPool;
        int iSize;
    };
    class UnitPool: private DWatcher< __unit_pool__ > {
    public:
        UnitPool( bool makeSource );
        UnitPool( UnitPool &share );

        DPN::Thread::ThreadUnit get();
        bool put( DPN::Thread::ThreadUnit &unit );
        void put( const DArray<DPN::Thread::ThreadUnit> &u );
    };
    //----------------------------------------------------------------
    class __thread_core__ : public UnitPool {
    public:
        friend class ThreadCore;
        __thread_core__( UnitPool &up );
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
        DArray< DPN::Thread::ThreadUnit > aUnits;
    };
    class ThreadCore : private DWatcher< __thread_core__ > {
    public:
        ThreadCore( UnitPool sharingPool );
        std::thread::id id() const;
        void run();
    };
    inline static void start( ThreadCore tc ) { tc.run(); }
    //----------------------------------------------------------------
    class __thread_user__ : public UnitPool {
    public:
        __thread_user__();
        bool isThreadFree() const;
        DArray< ThreadCore > aThreads;
    };

    class ThreadUser
            : private DWatcher< __thread_user__ >
    {
    public:
        using DWatcher< __thread_user__ >::copy;

        ThreadUser() {}
        ThreadUser( bool makeSource );

        bool putUnit(ThreadUnit unit );
        void startStream( Policy p );
    private:
        UnitPool poolIf();
    };
}


#endif // DPN_THREADBRIDGE_H
