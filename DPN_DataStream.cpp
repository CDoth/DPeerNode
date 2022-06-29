#include "DPN_DataStream.h"


/*
bool DPN_DataStream::reserveShadowReceiver(DPN_Channel *channel, const DPN_ExpandableBuffer &extra) {

    if( channel == nullptr ) {
        DL_BADPOINTER(1, "channel");
        return false;
    }
    if( channel->isBackReserved() ) {
        DL_ERROR(1, "Back reserved");
        return false;
    }
    if( iServiceContainer.extract(extra) == false ) {
        DL_ERROR(1, "Can't extract types");
        return false;
    }
    ReservedDataChannel i;
    i.pt = iServiceContainer.packetType();
    i.ct = iServiceContainer.connectionType();
    i.name = iServiceContainer.name();
    i.channel = channel;
    aIncomingDataChannels.append(i);

    return true;
}
bool DPN_DataStream::reserveShadowSender(DPN_Channel *channel, const DPN_ExpandableBuffer &extra) {

    if( channel == nullptr ) {
        DL_BADPOINTER(1, "Channel");
        return false;
    }
    if( channel->forwardRequester() != this ) {
        DL_ERROR(1, "Different requesters");
        return false;
    }
    if( iServiceContainer.extract(extra) == false ) {
        DL_ERROR(1, "Can't extract types");
        return false;
    }
    ReservedDataChannel i;
    i.pt = iServiceContainer.packetType();
    i.ct = iServiceContainer.connectionType();
    i.name = iServiceContainer.name();
    i.channel = channel;
    aOutgoingDataChannels.append(i);

    return true;
}
bool DPN_DataStream::reserveUDPReceiver(int remotePort, const DPN_ExpandableBuffer &extra) {
    if( DXT_CHECK_PORT(remotePort) == false ) {
        DL_BADVALUE(1, "remote port: [%d]", remotePort);
        return false;
    }
    if( iServiceContainer.extract(extra) == false ) {
        DL_ERROR(1, "Can't extract types");
        return false;
    }
    ReservedDataChannel i;
    i.pt = iServiceContainer.packetType();
    i.ct = iServiceContainer.connectionType();
    i.name = iServiceContainer.name();
    i.channel = nullptr;
    i.udpPort = remotePort;
    aIncomingDataChannels.append(i);

    return true;
}
bool DPN_DataStream::reserveUDPSender(int remotePort, const DPN_ExpandableBuffer &extra) {
    if( DXT_CHECK_PORT(remotePort) == false ) {
        DL_BADVALUE(1, "remote port: [%d]", remotePort);
        return false;
    }
    if( iServiceContainer.extract(extra) == false ) {
        DL_ERROR(1, "Can't extract types");
        return false;
    }
    ReservedDataChannel i;
    i.pt = iServiceContainer.packetType();
    i.ct = iServiceContainer.connectionType();
    i.name = iServiceContainer.name();
    i.channel = nullptr;
    i.udpPort = remotePort;
    aOutgoingDataChannels.append(i);

    return true;
}
*/



/*
void DPN_GenerativeDataChannel::send(const void *data, int size, int key) {
    if( data == nullptr || size < 1 ) {
        DL_BADVALUE(1, "data [%p] or size: [%d]", data, size);
        return;
    }
    DataMessage m;
    m.wData.copy(data, size);
    m.iKey = key;
    aSendQueue.append(m);
}
void DPN_GenerativeDataChannel::send(const DPN_ExpandableBuffer &b, int size, int key) {
    DataMessage m;
    m.wData.copy(b, size);
    m.iKey = key;
    aSendQueue.append(m);
}

DPN_Result DPN_GenerativeDataChannel::send() {

    if( pGenerativeCallback == nullptr ) return DPN_REPEAT;


    if( iLastResult == DPN_SUCCESS ) {
        if( aSendQueue.empty() ) {
            if( pGenerativeCallback(pOpaque, iSendingData) == false ) return DPN_REPEAT;
        } else {
            DataMessage &m = aSendQueue.front();
            iSendingData.pData = m.wData.getData();
            iSendingData.iSize = m.wData.size();
            iSendingData.iKey = m.iKey;
            aSendQueue.pop_front();
        }
    }

    if( iSendingData.iSize == 0 || iSendingData.pData == nullptr ) return DPN_REPEAT;


    return iLastResult;
}
DPN_Result DPN_GenerativeDataChannel::receive() {


    return DPN_SUCCESS;
}

*/










