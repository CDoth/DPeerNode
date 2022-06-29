#include "DPN_ThreadUnit.h"


//-------------------------------------------------------------------- DPN_Direction
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







