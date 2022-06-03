#ifndef DPN_DATACLIENT_H
#define DPN_DATACLIENT_H

//#include "DPN_MediaSystemProcessors.h"
#include "DPN_DataStream.h"
#include "DPN_Client.h"

class DPN_DataClient : public DWatcher<DPN_ClientData> {
public:
    DPN_DataClient(DPN_Client &client);

    ///
    /// \brief makeChannel
    /// Channel type
    /// Connection type
    /// Packet type
    bool makeChannel(DPN_DataChannel::ChannelType channelType, DPN_DataChannel::PacketType packetType, DPN_DataChannel::ConnectionType connectionType);
private:
    DPN_DataStream *pDataStream;
};
DPN_DataStream * extractDataStreamModule(DPN_Modules &modules);

#endif // DPN_DATACLIENT_H
