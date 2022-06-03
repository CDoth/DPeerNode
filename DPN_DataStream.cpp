#include "DPN_DataStream.h"

DPN_DataStream::DPN_DataStream() : DPN_AbstractModule("DataStream") {

}
bool DPN_DataStream::reserveShadowReceiver(DPN_Channel *channel, const DPN_ExpandableBuffer &extra, int transaction) {

    if( channel == nullptr ) {
        DL_BADPOINTER(1, "Channel");
        return false;
    }
    if( channel->isBackReserved() ) {
        DL_ERROR(1, "Receive direction already reserved");
        return false;
    }

    DPN_DataChannel::ChannelType channelType;
    DPN_DataChannel::PacketType packetType;
    DPN_DataChannel::ConnectionType connectionType;

    connectionType = static_cast<DPN_DataChannel::ConnectionType>(extra.getData32()[0]);
    packetType = static_cast<DPN_DataChannel::PacketType>(extra.getData32()[1]);

    DPN_DataChannel *dataChannel = nullptr;

    return true;
}
bool DPN_DataStream::reserveShadowSender(DPN_Channel *channel, int transaction) {

    if( channel == nullptr ) {
        DL_BADPOINTER(1, "Channel");
        return false;
    }
    if( channel->isForwardReserved() ) {
        DL_ERROR(1, "Send direction already reserved");
        return false;
    }


//    auto T = core()->transactionMaster().transactionI<DPN_DataStreamTransaction>(transaction);
//    if( T == nullptr ) {
//        DL_ERROR(1, "No transaction with index: [%d]", transaction);
//        return false;
//    }

//    DPN_DataChannel *dataChannel = nullptr;
//    DPN_DataSendDirection *d = new DPN_DataSendDirection;
//    channel->reserveForward(d);

//    if( T->eChannelType == DPN_DataChannel::Generative ) {
//        dataChannel = new DPN_GenerativeDataChannel;
//    } else {
//        dataChannel = new DPN_DirectDataChannel;
//    }

//    d->setChannel(dataChannel);
//    aDataChannels.append(dataChannel);

    return true;
}
void DPN_DataStream::clientDisconnected(const DPN_AbstractClient *client) {

}
void DPN_DataStream::stop() {

}
//=================================================================== DPN_DataSendDirection:
bool DPN_DataSendDirection::proc() {


    if( pChannel == nullptr ) {
        DL_BADPOINTER(1, "channel");
        return false;
    }
//    if( pChannel->dataReady() ) {
        pChannel->send();
//    }
    return true;
}
bool DPN_DataReceiveDirection::proc() {
    if( pChannel == nullptr ) {
        DL_BADPOINTER(1, "channel");
        return false;
    }
    pChannel->receive();
    return true;
}
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
            if( pGenerativeCallback(pOpaque, iSendingData)  == false ) return DPN_REPEAT;
        } else {
            DataMessage &m = aSendQueue.front();
            iSendingData.pData = m.wData.getData();
            iSendingData.iSize = m.wData.size();
            iSendingData.iKey = m.iKey;
            aSendQueue.pop_front();
        }
    }

    if( iSendingData.iSize == 0 || iSendingData.pData == nullptr ) return DPN_REPEAT;


    switch (eConnectionType) {
    case ShadowStreamChannel:
        if( pShadowConnector == nullptr ) {
            DL_BADPOINTER(1, "Shadow connector");
            return DPN_FAIL;
        }
        if( (iLastResult = pShadowConnector->x_sendIt(iSendingData.pData, iSendingData.iSize)) == DPN_FAIL ) {
            DL_FUNCFAIL(1, "x_sendIt");
            return DPN_FAIL;
        }
        break;
    case ShadowPacketChannel:
        if( pShadowConnector == nullptr ) {
            DL_BADPOINTER(1, "Shadow connector");
            return DPN_FAIL;
        }
        if( ePacketType == KEYABLE ) {
            wBuffer.dropTo(KEYABLE);

            wBuffer.getData32()[0] = iSendingData.iSize + sizeof(uint32_t);
            wBuffer.getData32()[1] = iSendingData.iKey;
            wBuffer.append(iSendingData.pData, iSendingData.iSize);

            if( (iLastResult = pShadowConnector->x_send(wBuffer)) == DPN_FAIL ) {
                DL_FUNCFAIL(1, "x_send");
                return DPN_FAIL;
            }
        }
        break;
    case UDPChannel:
        if( pDataPort == nullptr ) {
            DL_BADPOINTER(1, "Data UDP port");
            return DPN_FAIL;
        }
        if( pDataPort->socket().send(iSendingData.pData, iSendingData.iSize) < 0 ) {
            DL_FUNCFAIL(1, "send (udp port)");
            iLastResult = DPN_FAIL;
            return DPN_FAIL;
        }
        break;
    }

    return iLastResult;
}
DPN_Result DPN_GenerativeDataChannel::receive() {

    switch (eConnectionType) {
    case ShadowStreamChannel:
        if( pShadowConnector == nullptr ) {
            DL_BADPOINTER(1, "Shadow connector");
            return DPN_FAIL;
        }
        if( pShadowConnector->readable() ) {
            if( (iLastResult = pShadowConnector->x_receive(wBuffer, wBuffer.size())) == DPN_FAIL ) {
                DL_FUNCFAIL(1, "x_receivePacket");
                return DPN_FAIL;
            }
            if( pProcessorCallback ) {
                GeneratedData data;
                data.pData = wBuffer.getData();
                data.iSize = pShadowConnector->transportedBytes();
                data.iKey = -1;
                pProcessorCallback(pProcessOpaque, data);
            } else {
                DataMessage dm;
                dm.wData.copy(wBuffer);
                dm.iKey = -1;
                aReceivedDataQueue.append(dm);
            }
        } else {
            return DPN_REPEAT;
        }
        break;
    case ShadowPacketChannel:
        if( pShadowConnector == nullptr ) {
            DL_BADPOINTER(1, "Shadow connector");
            return DPN_FAIL;
        }
        if( pShadowConnector->readable() ) {
            if( (iLastResult = pShadowConnector->x_receivePacket(wBuffer)) == DPN_FAIL ) {
                DL_FUNCFAIL(1, "x_receivePacket");
                return DPN_FAIL;
            }
            if( pProcessorCallback ) {
                GeneratedData data;
                data.pData = wBuffer.getData();
                data.iSize = pShadowConnector->transportedBytes();
                if( ePacketType == KEYABLE ) data.iKey = wBuffer.getData32()[0];
                else data.iKey = -1;

                pProcessorCallback(pProcessOpaque, data);
            } else {
                DataMessage dm;
                dm.wData.copy(wBuffer);
                if( ePacketType == KEYABLE ) dm.iKey = wBuffer.getData32()[0];
                else dm.iKey = -1;
                aReceivedDataQueue.append(dm);
            }
        } else {
            return DPN_REPEAT;
        }
        break;
    case UDPChannel:
        if( pDataPort == nullptr ) {
            DL_BADPOINTER(1, "data port");
            return DPN_FAIL;
        }
        if( pDataPort->socket().readable() == false ) return DPN_REPEAT;
        int rb = 0;
        if( (rb = pDataPort->socket().receive(wBuffer.getData(), wBuffer.size())) < 1 ) {
            DL_ERROR(1, "Connection fault");
            return DPN_FAIL;
        }
        if( pProcessorCallback ) {
            GeneratedData data;
            data.pData = wBuffer.getData();
            data.iSize = rb;
            if( ePacketType == KEYABLE ) data.iKey = wBuffer.getData32()[0];
            else data.iKey = -1;

            pProcessorCallback(pProcessOpaque, data);
        } else {
            DataMessage dm;
            dm.wData.copy(wBuffer, rb);
            if( ePacketType == KEYABLE ) dm.iKey = wBuffer.getData32()[0];
            else dm.iKey = -1;
            aReceivedDataQueue.append(dm);
        }
        break;
    }

    return DPN_SUCCESS;
}

DPN_DataStreamTransaction::DPN_DataStreamTransaction() {
    iSubtype = NoType;
}

