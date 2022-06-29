#include "DPN_ClientData.h"



DPN_ClientInterface::DPN_ClientInterface() {

}
DPN_ClientInterface::DPN_ClientInterface( const DPN_Thread::ThreadUser &sharing, DPN_NodeConnector *c) : DWatcher<DPN_ClientCore>(true, sharing) {
    DL_INFO(1, "Client if created. init");
    if( c ) init ( c );
}
bool DPN_ClientInterface::init(DPN_NodeConnector *c ) {
    if( isEmptyObject() ) {
        DL_ERROR(1, "No object");
        return false;
    }
    data()->init( c );
    return true;
}
void DPN_ClientInterface::setModules(DPN_Modules modules) {
    data()->setModules( modules );
}
const DPN_ClientTag *DPN_ClientInterface::tag() const {
    return data();
}
const DArray<__channel> &DPN_ClientInterface::channels() const {
    return data()->shadows();
}
const DArray<DPN_ThreadUnit *> &DPN_ClientInterface::threadUnits() const {
    return data()->aThreadUnits;
}
void DPN_ClientInterface::sendMessage(const char *m) {
    auto proc = DPN_PROCS::processor<DPN_PACKETTYPE__TEXT_MESSAGE>();
    proc->setMessage(m);
    send(proc);
}
void DPN_ClientInterface::makeShadowConnection(DXT::Type t) {
    auto proc = DPN_PROCS::processor<DPN_PACKETTYPE__MAKE_SHADOW_CONNECTION>();
    proc->setType( t );
    send(proc);
}
bool DPN_ClientInterface::send(DPN_TransmitProcessor *cp) {
    if( isEmptyObject() ) {
        DL_ERROR(1, "Empty watcher");
        return false;
    }
    return data()->send( cp );
}
