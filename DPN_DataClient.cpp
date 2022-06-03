#include "DPN_DataClient.h"

DPN_DataClient::DPN_DataClient(DPN_Client &client) {
    defaultCopy(client);

    pDataStream = extractDataStreamModule(data()->modules());
}


bool DPN_DataClient::makeChannel(
        DPN_DataChannel::ChannelType channelType,
        DPN_DataChannel::PacketType packetType,
        DPN_DataChannel::ConnectionType connectionType) {


//    auto t = data()->generateTransaction<DPN_DataStreamTransaction>();
//    if( t == nullptr ) {
//        DL_BADPOINTER(1, "transaction");
//        return false;
//    }


//    DL_INFO(1, "Create transaction: [%p] [%d]", t, t->index());
//    t->ePacketType = packetType;
//    t->eChannelType = channelType;
//    t->eConnectionType = connectionType;
//    t->iSubtype = DPN_DataStreamTransaction::ChannelReserving;


    if( connectionType == DPN_DataChannel::ShadowPacketChannel || connectionType == DPN_DataChannel::ShadowStreamChannel ) {

        const DArray<DPN_Channel*> &ch = data()->getChannels();
        if( ch.size() < 2 ) {
            DL_ERROR(1, "No shadow channels");
            return false;
        }
        DPN_Channel *__pick = pickForwardFree(ch);
        if( __pick == nullptr ) {
            DL_ERROR(1, "No free channels");
            return false;
        }
        DL_INFO(1, "pick shadow channel: [%p] [%s]", __pick, __pick->shadowKey().c_str());



//        auto proc = data()->processor<DPN_PACKETTYPE__REQUEST>(t);
//        DPN_ExpandableBuffer extraData;
//        extraData.appendValue(uint32_t(connectionType));
//        extraData.appendValue(uint32_t(packetType));

//        proc->setType(REQUEST_CHANNEL_RESERVING);
//        proc->setHash(__pick->shadowKey());
//        proc->setSourceAddress(__pick->local());
//        proc->setTargetAddress(__pick->remote());
//        proc->setExtraData(extraData);
//        proc->setModuleName("DataStream");

//        data()->send(proc);

    } else {

    }

    return true;

}
DPN_DataStream *extractDataStreamModule(DPN_Modules &modules) {
    return reinterpret_cast<DPN_DataStream*>(modules.module("DataStream"));
}
