#include "DPN_Channel.h"

MonoChannel::MonoChannel() {

}
bool MonoChannel::reserve(DPN_AbstractModule *module, const DPN_ExpandableBuffer &context) {

}
using namespace DPeerNodeSpace;
IO__CHANNEL::IO__CHANNEL(DPN_NodeConnector *c)  {
    pConnector = c;
}
bool IO__CHANNEL::init(DPN_NodeConnector *connector) {
    if( pConnector ) {
        DL_WARNING(1, "Inited");
        return false;
    }
    pConnector = connector;
    return true;
}
bool IO__CHANNEL::checkGenerator() const {
    if( pConnector == nullptr ) {
        DL_BADPOINTER(1, "connector");
        return false;
    }
    return true;
}
bool IO__CHANNEL::checkProcessor() const {
    if( pConnector == nullptr ) {
        DL_BADPOINTER(1, "connector");
        return false;
    }
    return true;
}
DPN_Result IO__CHANNEL::generatorStart() {

//    if( iLocalState == MS__ACTIVE && iRemoteState == MS__ACTIVE ) {
//        return DPN_SUCCESS;
//    }
//    iLocalState = MS__ACTIVE;
//    if( iRemoteState == MS__REQUESTING || iRemoteState == MS__BOUND ) {
//        DL_INFO(1, "Wait remote state in [%d]", iRemoteState );
//        return DPN_REPEAT;
//    }
//    if( pClient == nullptr ) {
//        DL_BADPOINTER(1, "client");
//        return DPN_FAIL;
//    }
//    DPN_Result r;
//    DL_INFO(1, "Try sync channel... forward settings: [%d]", wSettings.size());
//    auto proc = DPN_PROCS::processor<DPN_PACKETTYPE__SYNC_CHANNEL>();


//    if( ( r = pClient->syncChannel( iShadowKey ) ) == DPN_FAIL ) {
//        DL_FUNCFAIL(1, "syncChannel");
//        return DPN_FAIL;
//    }
//    wBuffer.reserve( 1024 * 1024 * 10 );

//    return DPN_REPEAT;

    return DPN_SUCCESS;
}
DPN_Result IO__CHANNEL::processorStart() {

//    if( iForward.iLocalState == MS__ACTIVE && iForward.iRemoteState == MS__ACTIVE ) {
//        return DPN_SUCCESS;
//    }
//    iForward.iLocalState = MS__ACTIVE;
//    if( iForward.iRemoteState == MS__REQUESTING || iForward.iRemoteState == MS__BOUND ) {
//        DL_INFO(1, "Wait remote state in [%d]", iForward.iRemoteState );
//        return DPN_REPEAT;
//    }
//    if( pClient == nullptr ) {
//        DL_BADPOINTER(1, "client");
//        return DPN_FAIL;
//    }
//    DPN_Result r;
//    DL_INFO(1, "Try sync channel... forward settings: [%d]", iForward.wSettings.size());
//    if( ( r = pClient->syncChannel( iShadowKey ) ) == DPN_FAIL ) {
//        DL_FUNCFAIL(1, "syncChannel");
//        return DPN_FAIL;
//    }
//    DL_INFO(1, "sync started [%d]", r);
//    iForward.iRemoteState = MS__REQUESTING;
//    return DPN_REPEAT;

    return DPN_SUCCESS;
}
DPN_Result IO__CHANNEL::generate(DPN_ExpandableBuffer &b) {


    if( !pConnector->readable() ) return DPN_REPEAT;

//    DL_INFO(1, "Try receive... channel: [%p]", this);

    if( pConnector->type() == DXT::TCP ) {
        if( pConnector->x_receivePacket() == DPN_FAIL ) {
            DL_FUNCFAIL(1, "x_receivePacket");
            return DPN_FAIL;
        }
    } else {
        if( pConnector->x_receive( b, b.size() ) == DPN_FAIL ) {
            DL_FUNCFAIL(1, "x_receive");
            return DPN_FAIL;
        }
    }

//    DL_INFO(1, "Received: [%d] channel: [%p]", pConnector->transportedBytes(), this);
//    pConnector->buffer().print( pConnector->transportedBytes() );
    b.copy( pConnector->buffer(), pConnector->transportedBytes() );
    return DPN_SUCCESS;
}
DPN_Result IO__CHANNEL::process(DPN_ExpandableBuffer &b) {

    if( b.size() == 0 ) {
//        DL_INFO(1, "No data to send. channel: [%p]", this);
        return DPN_SUCCESS;
    }
//    DL_INFO(1, "Try send... channel: [%p] size: [%d]",
//            this, b.size());

    if( pConnector->x_send( b ) == DPN_FAIL ) {
        DL_FUNCFAIL(1, "x_sendIt");
        return DPN_FAIL;
    }
//    DL_INFO(1, "sent: [%d]", pConnector->transportedBytes());

    return DPN_SUCCESS;
}


__channel_data::__channel_data() {
//    pForward = nullptr;
//    pBackward = nullptr;
    pChannel = nullptr;
}
__channel_data::~__channel_data() {
//    if( !pForward ) delete pForward;
//    if( !pBackward ) delete pBackward;
    if( !pChannel ) delete pChannel;
}
bool __channel_data::init(DPN_NodeConnector *c, const std::string &shadowKey) {
    if( inited() ) {
        DL_WARNING(1, "inited");
        return false;
    }
    iKey = shadowKey;
    pChannel = new IO__CHANNEL( c );
    return true;
}
__channel::__channel() : DWatcher<__channel_data>(true) { }
bool __channel::init(DPN_NodeConnector *c, const std::string &shadowKey) {
    return data()->init( c, shadowKey );
}

__channel_mono_interface __channel::getMonoIf(dpn_direction d) {
    __channel_mono_interface i;
    DL_INFO(1, "Try get interface [%d]", d);
     __dpn_acc_interface<dpn_direction, __channel_data> w = monoIf.get( d, *this );
     DL_INFO(1, "got interface for key: [%d] valid: [%d]", d, w.validInterface());
     i.move( w );
     return i;
}
__channel_private_interface __channel::privateInterface() {
    __channel_private_interface i;
    auto w = privateIf.get( *this );
    i.move( w );
    return i;
}
MonoChannelState __channel::localState(dpn_direction d) const {
    if( isEmptyObject() ) {
        DL_ERROR(1, "Empty watcher");
        return MS__RAW;
    }

    return d == DPN_FORWARD ? data()->iForward.iLocalState : data()->iBackward.iLocalState;
}
MonoChannelState __channel::remoteState(dpn_direction d) const {
    if( isEmptyObject() ) {
        DL_ERROR(1, "Empty watcher");
        return MS__RAW;
    }

    return d == DPN_FORWARD ? data()->iForward.iRemoteState : data()->iBackward.iRemoteState;
}
std::string __channel::shadowKey() const {
    if( isEmptyObject() ) {
        DL_ERROR(1, "Empty watcher");
        return std::string();
    }
    return data()->iKey;
}



void __channel_private_interface::setLocalState(dpn_direction d, MonoChannelState s) {
    if( badInterface() ) {
        DL_ERROR(1, "Bad interface");
        return;
    }
    d == DPN_FORWARD ? inner()->iForward.iLocalState = s : inner()->iBackward.iLocalState = s;
}
void __channel_private_interface::setRemoteState(dpn_direction d, MonoChannelState s) {
    if( badInterface() ) {
        DL_ERROR(1, "Bad interface");
        return;
    }
    d == DPN_FORWARD ? inner()->iForward.iLocalState = s : inner()->iBackward.iLocalState = s;
}
