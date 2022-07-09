#include "DPN_ThreadUnit.h"


/*
DPN_ThreadUnit::DPN_ThreadUnit() {
    inThread = false;
    blocked = false;
    errorState = NO_FAIL;
}
DPN_ThreadUnit::~DPN_ThreadUnit() {
}
bool DPN_ThreadUnit::threadInjection() {
    return true;
}
void DPN_ThreadUnit::threadKick() { }
void DPN_ThreadUnit::fail() { }

void DPN_ThreadUnit::kick() {
    threadKick();
    inThread = false;
    blocked = false;
}
bool DPN_ThreadUnit::unitProc() {
    if( work() == false ) {
        fail();
        return false;
    }
    return true;
}
void DPN_ThreadUnit::block() {
    blocked = true;
}
*/



using namespace DPN::Logs;
namespace DPN::Thread {
    AbstractThreadUnit::AbstractThreadUnit() {
        inThread = false;
        blocked = false;
    }
    AbstractThreadUnit::~AbstractThreadUnit() {}
    void AbstractThreadUnit::block() {
        blocked = true;
    }
    void AbstractThreadUnit::kick() {
        threadKick();
        inThread = false;
        blocked = false;
    }
    bool AbstractThreadUnit::unitProc() {
        if( work() == false ) {
            fail();
            return false;
        }
        return true;
    }
    bool AbstractThreadUnit::threadInjection() {
        return true;
    }
    void AbstractThreadUnit::fail() { }
    void AbstractThreadUnit::threadKick() { }
    //========================================================== ThreadUnit
    void ThreadUnit::block() {
        if( isEmptyObject() ) {
            DL_ERROR(1, "Empty watcher");
            return;
        }
        return data()->block();
    }
    void ThreadUnit::kick() {
        if( isEmptyObject() ) {
            DL_ERROR(1, "Empty watcher");
            return;
        }
        return data()->kick();
    }

}



