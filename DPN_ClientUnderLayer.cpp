#include "DPN_ClientUnderLayer.h"

using namespace DPeerNodeSpace;
void DPN_ClientUnderlayer::init(DPN_NodeConnector *c) {
    if( c == nullptr || isEmptyObject() ) {
        DL_ERROR(1, "Can't init under layer: connector: [%p] empty object: [%d]", c, isEmptyObject());
        return;
    }
    data()->iStats.iLocal = c->local();
    data()->iStats.iRemote = c->peer();
}
DPN_ClientUnderlayer::DPN_ClientUnderlayer() : DWatcher<__client_underlayer__>(false) {

}
DPN_ClientUnderlayer::DPN_ClientUnderlayer(DPN_ClientTag *t, bool create) : DWatcher<__client_underlayer__>(create) {
    if( isCreatedObject() )
        data()->pTag = t;
}
bool DPN_ClientUnderlayer::isShadowAvailable(DXT::Type, int port) {
    return true;
}
PeerAddress DPN_ClientUnderlayer::remoteAddress() {
    if( isEmptyObject() ) return PeerAddress();
    return data()->iStats.iRemote;
}
bool DPN_ClientUnderlayer::addShadowConnection(DPN_NodeConnector *udpPort, const std::string shadowKey) {
    if( udpPort == nullptr || shadowKey.empty() ) {
        DL_BADVALUE(1, "port [%p] or key [%d]", udpPort, shadowKey.size());
        return false;
    }
    __channel ch;
    ch.init( udpPort, shadowKey );
    data()->aShadowChannels.append( ch );
    return true;
}
bool DPN_ClientUnderlayer::registerShadow(const std::string &shadowKey) {
    return false;
}
const DArray<__channel> &DPN_ClientUnderlayer::shadows() const  {
    return data()->aShadowChannels;
}
const DPN_ClientTag *DPN_ClientUnderlayer::tag() const {
    if( isEmptyObject() ) return nullptr;
    return data()->pTag;
}
__channel DPN_ClientUnderlayer::channel(const std::string &key) {
    if( isEmptyObject() ) return __channel();

    FOR_VALUE( data()->aShadowChannels.size(), i ) {
        if( data()->aShadowChannels[i].shadowKey() == key ) return data()->aShadowChannels[i];
    }
    return __channel();
}
