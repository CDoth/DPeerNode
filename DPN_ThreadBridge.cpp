
#include "DPN_ThreadBridge.h"
#include <sstream>
using namespace DPeerNodeSpace;

DPN_ThreadBridge::DPN_ThreadBridge() : DWatcher<DPN_ThreadBridgeData>(true)
{


}
void DPN_ThreadBridge::clearPlan() {
    data()->clearPlan();
}
bool DPN_ThreadBridge::planDirection(DPN_Direction *d) {
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

    threadContext.alloc();
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
bool DPN_ThreadBridgeData::planDirection(DPN_Direction *d) {
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
void DPN_ThreadBridgeData::forceTask(DPN_Task *t) {
    DPN_THREAD_GUARD(taskMutex);
    tasks.push_back(t);
}
bool DPN_ThreadBridgeData::pushTask(DPN_Task *t) {

    if( std::this_thread::get_id() == stdThreadId) {
        return false;
    }
    DPN_THREAD_GUARD(taskMutex);
    tasks.push_back(t);
    return true;
}
void DPN_ThreadBridgeData::procTasks() {


    DPN_Task *t = tasks.getActual();

    if( t == nullptr ) return;

    DPN_Result r = t->proc();
    if( r == DPN_SUCCESS || r == DPN_FAIL ) {

        DPN_THREAD_GUARD(taskMutex);
        tasks.popActual();

        if( t->isRemovable() ) {
            delete t;
        }
    }

}
void DPN_ThreadBridgeData::procDirections() {
    accept();

    if( accepted.empty() ) return;


    FOR_VALUE( accepted.size(), i ) {
        auto d = accepted[i];

        if( d->isBlocked() ) {

            DL_INFO(1, " >>>>>>>>> 0: proc leaving direction: [%p] i: [%d] size: [%d] client: [%p]",
                    d, i, accepted.size(), d->clientContext.client());



            accepted.removeByIndex(i--);
            d->inThread = false;
            d->blocked = false;

            if( d->clientContext.client() == nullptr ) {
                DL_BADPOINTER(1, "client");
            } else {
                d->clientContext.client()->disconnect();
            }

            d->close();

        } else {
            if( d->proc() == false ) {
                if(d->error() == DPN_Direction::CONNECTION_FAIL) {

                    DL_INFO(1, " >>>>>>>>> 1: proc leaving direction: [%p] i: [%d] size: [%d]",
                            d, i, accepted.size());


                    accepted.removeByIndex(i--);
                    d->inThread = false;
                    d->blocked = false;
                    if( d->clientContext.client() == nullptr ) {
                        DL_BADPOINTER(1, "client");
                    } else {
                        d->clientContext.client()->disconnect();
                    }
                }
            }
        }
    }
}
void DPN_ThreadBridgeData::procSingle() {

    auto d = singleDirection;
    if(d->isBlocked()) {
        singleDirection = nullptr;
        d->inThread = false;
        d->clientContext.client()->disconnect();
    } else if( d->proc() == false ) {
        if(d->error() == DPN_Direction::CONNECTION_FAIL) {
            singleDirection = nullptr;
            d->inThread = false;
            d->clientContext.client()->disconnect();
        }
    }
}
bool DPN_ThreadBridgeData::__hold_direction(DPN_Direction *d) {
    if( d->threadInjection() == false ) {
        DL_ERROR(1, "Thread injection fault. direction: [%p]", d);
        return false;
    }
    accepted.push_back(d);

    d->inThread = true;
    d->threadContext = threadContext;

    return true;
}
