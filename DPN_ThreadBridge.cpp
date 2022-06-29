
#include "DPN_ThreadBridge.h"
#include <sstream>
using namespace DPeerNodeSpace;

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

//============================================================

void DPN_Thread::ThreadCore::accept() {


//    if(acceptMode == NO_ACCEPT) {
//        return;
//    }

//    switch (acceptMode) {
//    case REPLACE:

//        FOR_VALUE(accepted.size(), i) {
//            accepted[i]->inThread = false;
//        }
//        accepted.clear();
//        acceptMode = __KEEP_REPLACE;
//        break;
//    case __KEEP_REPLACE:
//        FOR_VALUE(planed.size(), i) {


//            DL_INFO(1, "direction: [%p] inThread: [%d]",
//                    planed[i], bool(planed[i]->inThread)
//                    );


//            if( planed[i]->inThread == false ) {
//                __hold_direction(planed[i]);
//                planed.removeByIndex(i--);
//            }
//        }
//        if(planed.empty()) {
//            acceptMode = NO_ACCEPT;
//        }
//        break;
//    default:break;
//    }
}

void DPN_Thread::__thread_core__::run() {
    accept();

    if( aAccepted.empty() ) return;

    FOR_VALUE( aAccepted.size(), i ) {
        DPN_ThreadUnit *unit = aAccepted[i];

        if( unit->isBlocked() || unit->unitProc() == false ) {
            aAccepted.removeByIndex( i-- );
            unit->kick();
        }
    }
}
void DPN_Thread::__thread_core__::accept() {

}
