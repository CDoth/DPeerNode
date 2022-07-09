
#include "DPN_ThreadBridge.h"
#include <sstream>
using namespace DPN::Logs;

namespace DPN::Thread {

    //----------------------------------------------------------------
    __unit_pool__::__unit_pool__() : iSize(0) {}
    UnitPool::UnitPool(bool makeSource) : DWatcher<__unit_pool__>(makeSource) { }
    UnitPool::UnitPool(UnitPool &share) : DWatcher<__unit_pool__>( share ) { }
    ThreadUnit UnitPool::get() {

        if( isEmptyObject() ) {
            DL_ERROR(1, "Empty watcher");
            return nullptr;
        }

        if( data()->iPool.empty() == false ) {
            --data()->iSize;
            return data()->iPool.takeLast();
        }
        return nullptr;
    }
    bool UnitPool::put(ThreadUnit &unit) {
        if( isEmptyObject() ) {
            DL_ERROR(1, "Empty watcher");
            return false;
        }
        if( unit.isEmptyObject() ) {
            DL_ERROR(1, "Bad unit");
            return false;
        }

        ++data()->iSize;
        data()->iPool.push_back( unit );
        DL_INFO(1, "unit [%p] in unit pool", unit);
        return true;
    }
    void UnitPool::put(const DArray<ThreadUnit> &u) {
        FOR_VALUE( u.size(), i ) {
            data()->iPool.push_back( u[i] );
        }
        data()->iSize += u.size();
    }
    //----------------------------------------------------------------
    __thread_core__::__thread_core__(UnitPool &up) : UnitPool(up) {
        iPlayable = false;
        threadId = 0;
    }
    __thread_core__::~__thread_core__() {
        backAll();
    }
    void __thread_core__::threadcore_run() {
        threadcore_start();
        while( iPlayable ) {
            threadcore_iterate();
        }
    }
    void __thread_core__::threadcore_start() {

        DL_INFO(1, "thread core: [%p]", this);
        std::stringstream ss;
        ss << std::this_thread::get_id();
        ss >> threadId;
        stdThreadId = std::this_thread::get_id();
        iPlayable = true;

    }
    void __thread_core__::threadcore_iterate() {

        accept();
        if( aUnits.empty() ) return;

        FOR_VALUE( aUnits.size(), i ) {
            DPN::Thread::ThreadUnit &unit = aUnits[i];

//            DL_INFO(1, "Run thread unit: [%p]", unit);

            if( unit.isBlocked() || unit.unitProc() == false ) {
                DL_INFO(1, "kick thread unit: [%p]", unit);
                aUnits.removeByIndex( i-- );
                unit.kick();
            }
        }
    }
    void __thread_core__::accept() {

        DPN::Thread::ThreadUnit unit;
        if( (unit = UnitPool::get()).isEmptyObject() == false ) {
            DL_INFO(1, "accept thread unit: [%p]", unit);
            aUnits.append( unit );
        }
    }
    void __thread_core__::backAll() {
        UnitPool::put( aUnits );
        aUnits.clear();
    }
    ThreadCore::ThreadCore(UnitPool sharingPool) : DWatcher<__thread_core__>(true, sharingPool) {}

    std::thread::id ThreadCore::id() const { return data()->stdThreadId; }

    void ThreadCore::run() { data()->threadcore_run(); }
    //----------------------------------------------------------------
    __thread_user__::__thread_user__() : UnitPool(true) {}
    bool __thread_user__::isThreadFree() const {
        FOR_VALUE( aThreads.size(), i ) {
            if( aThreads[i].id() == std::this_thread::get_id() ) return false;
        }
        return true;
    }
    ThreadUser::ThreadUser(bool makeSource) : DWatcher<__thread_user__>(makeSource) { }


    bool ThreadUser::putUnit(ThreadUnit unit) {
        if( isEmptyObject() ) {
            DL_ERROR(1, "Empty watcher");
            return false;
        }
        return data()->put( unit );
    }
    void ThreadUser::startStream(Policy p) {

        if( !data()->isThreadFree() ) return;

        ThreadCore t( poolIf() );
        data()->aThreads.append( t );

        if( p == THIS_THREAD ) {
            DPN::Thread::start( t );
        } else {
            std::thread __t (

            DPN::Thread::start, t

                        );

            __t.detach();
        }
    }
    UnitPool ThreadUser::poolIf() { return *data(); }
}



