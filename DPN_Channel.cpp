#include "DPN_Channel.h"

DPN_Channel::DPN_Channel() {
    __clear_ptrs();
}
void DPN_Channel::init(DPN_NodeConnector *connector, bool initiator, PeerAddress avatar, DPN_Channel *mainChannel, const std::string &shadowKey) {
    __connector = connector;
    __local = __connector->local();
    __remote = __connector->peer();
    __isInitiator = initiator;
    __avatar = avatar;
    __mainChannel = mainChannel;
    __shadowKey = shadowKey;
}
void DPN_Channel::stop() {
    __connector->close();
}
bool DPN_Channel::reserve(DPN_Direction *f, DPN_Direction *b) {
    if( reserveForward(f) == false ) return false;
    if( reserveBack(b) == false ) return false;
    return true;
}
bool DPN_Channel::reserveForward(DPN_Direction *f) {
    if( __forward ) {
        DL_ERROR(1, "Forward directon already reserved");
        return false;
    }
    __forward = f;
    return true;
}
bool DPN_Channel::reserveBack(DPN_Direction *b) {
    if( __back ) {
        DL_ERROR(1, "Back direction already reserved");
        return false;
    }
    __back = b;
    return true;
}
DPN_Direction *DPN_Channel::unreserveForward() {
    auto temp = __forward;
    __forward = nullptr;
    return temp;
}
DPN_Direction *DPN_Channel::unreserveBack() {
    auto temp = __back;
    __back = nullptr;
    return temp;
}
void DPN_Channel::__clear_ptrs() {
    __connector = nullptr;
    __mainChannel = nullptr;
    __forward = nullptr;
    __back = nullptr;
}

//=============================================
DPN_UDPChannel::DPN_UDPChannel() : socket(DXT::UDP) {
    __forward = nullptr;
    __back = nullptr;
}
DPN_Channel *pickForwardFree(const DArray<DPN_Channel *> &channels) {

    FOR_VALUE(channels.size(), i) {
        if( channels[i]->isFowardFree() ) {
            return channels[i];
        }
    }
    return nullptr;
}
