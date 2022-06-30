
#include "DPN_ThreadBridge.h"
#include <sstream>
using namespace DPN::Logs;
/*
DPN_ThreadBridge::DPN_ThreadBridge() : DWatcher<DPN_ThreadBridgeData>(true)
{


}
void DPN_ThreadBridge::clearPlan() {
    data()->clearPlan();
}
bool DPN_ThreadBridge::planDirection(DPN_ThreadUnit *d) {
    return data()->planDirection(d);
}
void DPN_ThreadBridge::replacePlaned() {
    data()->setReplaceMode();
}
int DPN_ThreadBridge::directionsAccepted() const {
    return data()->accepted.size();
}
uint64_t DPN_ThreadBridge::threadId() const {
    return data()->threadId;
}
void DPN_ThreadBridge::start() {
    std::stringstream ss;
    ss << std::this_thread::get_id();
    ss >> data()->threadId;
    data()->stdThreadId = std::this_thread::get_id();
}
DPN_ThreadBridgeData::DPN_ThreadBridgeData() {
    play = true;
    pause = false;
    threadId = 0;

    process_tasks = false;
    acceptMode = NO_ACCEPT;
}
void DPN_ThreadBridgeData::accept() {

    if(acceptMode == NO_ACCEPT) {
        return;
    }

//    DL_INFO(1, "DIRECTIONS ACCEPTING: mode: [%d] planed: [%d]",
//            bool(acceptMode), planed.size()
//            );


    switch (acceptMode) {
    case REPLACE:

        FOR_VALUE(accepted.size(), i) {
            accepted[i]->inThread = false;
        }
        accepted.clear();
        acceptMode = __KEEP_REPLACE;
        break;
    case __KEEP_REPLACE:
        FOR_VALUE(planed.size(), i) {


            DL_INFO(1, "direction: [%p] inThread: [%d]",
                    planed[i], bool(planed[i]->inThread)
                    );


            if( planed[i]->inThread == false ) {
                __hold_direction(planed[i]);
                planed.removeByIndex(i--);
            }
        }
        if(planed.empty()) {
            acceptMode = NO_ACCEPT;
        }
        break;
    default:break;
    }
}
void DPN_ThreadBridgeData::setNoAcceptMode() {
    acceptMode = NO_ACCEPT;
}
void DPN_ThreadBridgeData::setReplaceMode() {
    DPN_THREAD_GUARD(mutex);
    acceptMode = REPLACE;
}
bool DPN_ThreadBridgeData::planDirection(DPN_ThreadUnit *d) {
    if(d == nullptr) {
        DL_BADPOINTER(1, "direction");
        return false;
    }
    DPN_THREAD_GUARD(mutex);

    if( d->isBlocked() ) {
        DL_WARNING(1, "direction [%p] blocked", d);
        return false;
    }

    planed.push_back(d);
    return true;
}
void DPN_ThreadBridgeData::clearPlan() {
    DPN_THREAD_GUARD(mutex);
    planed.clear();
}
void DPN_ThreadBridgeData::procDirections() {
    accept();

    if( accepted.empty() ) return;


    FOR_VALUE( accepted.size(), i ) {
        auto u = accepted[i];

        if( u->isBlocked() ) {

            DL_INFO(1, "Thread Unit [%p] blocked and leave thread: [%d:%p]", u, threadId, this);
            accepted.removeByIndex(i--);
            u->inThread = false;
            u->blocked = false;

            u->threadKick();
            u->close();

        } else {
            if( u->unitProc() == false ) {
                DL_INFO(1, "Thread Unit [%p] leave thread: [%d:%p]", u, threadId, this);

                accepted.removeByIndex(i--);
                u->inThread = false;
                u->blocked = false;
            }
        }
    }
}
void DPN_ThreadBridgeData::procSingle() {


}
bool DPN_ThreadBridgeData::__hold_direction(DPN_ThreadUnit *d) {
    if( d->threadInjection() == false ) {
        DL_ERROR(1, "Thread injection fault. direction: [%p]", d);
        return false;
    }
    accepted.push_back(d);
    d->inThread = true;
    return true;
}
*/




namespace DPN::Thread {

    //----------------------------------------------------------------
    __unit_pool__::__unit_pool__() : iSize(0) {}
    UnitPoolInterface::UnitPoolInterface(bool makeSource) : DWatcher<__unit_pool__>(makeSource) { }
    UnitPoolInterface::UnitPoolInterface(UnitPoolInterface &share) : DWatcher<__unit_pool__>( share ) { }
    DPN_ThreadUnit *UnitPoolInterface::get() {

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
    void UnitPoolInterface::put(DPN_ThreadUnit *unit) {
        if( isEmptyObject() ) {
            DL_ERROR(1, "Empty watcher");
            return;
        }
        if( unit ) {
            ++data()->iSize;
            data()->iPool.push_back( unit );
            DL_INFO(1, "unit [%p] in unit pool", unit);
        }
    }
    void UnitPoolInterface::put(const DArray<DPN_ThreadUnit *> &u) {
        FOR_VALUE( u.size(), i ) {
            data()->iPool.push_back( u[i] );
        }
        data()->iSize += u.size();
    }
    //----------------------------------------------------------------
    __thread_core__::__thread_core__(UnitPoolInterface &up) : UnitPoolInterface(up) {
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

//        DL_INFO(1, "thread core: [%p]", this);
        accept();
        if( aAccepted.empty() ) return;

        FOR_VALUE( aAccepted.size(), i ) {
            DPN_ThreadUnit *unit = aAccepted[i];

            if( unit->isBlocked() || unit->unitProc() == false ) {
                DL_INFO(1, "kick thread unit: [%p]", unit);
                aAccepted.removeByIndex( i-- );
                unit->kick();
            }
        }
    }
    void __thread_core__::accept() {

        DPN_ThreadUnit *unit = nullptr;
        if( (unit = UnitPoolInterface::get()) ) {
            DL_INFO(1, "accept thread unit: [%p]", unit);
            aAccepted.append( unit );
        }
    }
    void __thread_core__::backAll() {
        UnitPoolInterface::put( aAccepted );
        aAccepted.clear();
    }
    ThreadCore::ThreadCore(UnitPoolInterface sharingPool) : DWatcher<__thread_core__>(true, sharingPool) {}

    std::thread::id ThreadCore::id() const { return data()->stdThreadId; }

    void ThreadCore::run() { data()->threadcore_run(); }
    //----------------------------------------------------------------
    __thread_user__::__thread_user__() : UnitPoolInterface(true) {}
    bool __thread_user__::isThreadFree() const {
        FOR_VALUE( aThreads.size(), i ) {
            if( aThreads[i].id() == std::this_thread::get_id() ) return false;
        }
        return true;
    }
    ThreadUser::ThreadUser(bool makeSource) : DWatcher<__thread_user__>(makeSource) { }
    ThreadUser::ThreadUser(const ThreadUser &sharing) : DWatcher<__thread_user__>(sharing) {}
    void ThreadUser::putUnit(DPN_ThreadUnit *unit) {
        if( isEmptyObject() ) {
            DL_ERROR(1, "Empty watcher");
            return;
        }
        data()->put( unit );
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
    UnitPoolInterface ThreadUser::poolIf() { return *data(); }
}



