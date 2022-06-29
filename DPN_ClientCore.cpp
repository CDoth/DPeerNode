#include "DPN_ClientCore.h"


//==========================================================================
DPN_ClientCore::DPN_ClientCore() : DPN::Thread::ThreadUser(false) {

}
DPN_ClientCore::DPN_ClientCore(const ThreadUser &sharing) : DPN::Thread::ThreadUser(sharing), wClientUnder( this, true) {
    DL_INFO(1, "Client core [%p] created", this);
}
void DPN_ClientCore::init(DPN_NodeConnector *c) {
    if( baseInit( c, wClientUnder ) ) {

        DPN::Thread::ThreadUser::putUnit( forward() );
        DPN::Thread::ThreadUser::putUnit( backward() );
    }
}
bool DPN_ClientCore::send(DPN_TransmitProcessor *p) {
    return baseSend( p );
    return false;
}
const DArray<__channel> &DPN_ClientCore::shadows() const { return wClientUnder.shadows(); }
