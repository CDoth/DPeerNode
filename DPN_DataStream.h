#ifndef DPN_DATASTREAM_H
#define DPN_DATASTREAM_H


#include "DPN_Channel.h"
// 1. direct window
// 2. cross-thread window
// 3. generative stream

class DPN_DataStream;
class DPN_DataChannel {

    /*
     * 1. Key or keyless channel
     * 2. Main, shadow or UDP channel
    */


public:
    enum ConnectionType {
        ShadowStreamChannel,
        ShadowPacketChannel,
        UDPChannel
    };
    enum PacketType {
        KEYLESS = sizeof(uint32_t)
        ,KEYABLE = sizeof(uint64_t)
    };
    enum ChannelType {
        Direct
        , Generative
    };
protected:

protected:
    PacketType ePacketType;
    ConnectionType eConnectionType;
    ChannelType eChannelType;
    DPN_NodeConnector *pShadowConnector;
    DPN_UDPPort *pDataPort;
    DPN_Result iLastResult;
public:
//    virtual bool dataReady() const {return true;};
    virtual DPN_Result send() = 0;
    virtual DPN_Result receive() = 0;
};
class DPN_DirectDataChannel : public DPN_DataChannel {
public:
    DPN_Result send() override {return DPN_SUCCESS;}
    DPN_Result receive() override {return DPN_SUCCESS;}
//    void send(const void *data, int size) {}

private:
};
struct GeneratedData {
    const void *pData;
    int iSize;
    int iKey;
};
struct DataMessage {
    DPN_ExpandableBuffer wData;
    int iKey;
};
class DPN_GenerativeDataChannel : public DPN_DataChannel {
public:
    void setGenerator(bool (*generator)(void *opaque, GeneratedData &data));
    void setGeneratorBase(void *base);

    void send(const void *data, int size, int key = 0);
    void send(const DPN_ExpandableBuffer &b, int size = 0, int key = 0);


    DPN_Result send() override;
    DPN_Result receive() override;
private:
    DPN_ExpandableBuffer wBuffer;


    GeneratedData iSendingData;
    DArray<DataMessage> aSendQueue;
    void *pOpaque;
    bool (*pGenerativeCallback)(void *opaque, GeneratedData &data);


    // process received data in real time:
    void *pProcessOpaque;
    bool (*pProcessorCallback)(void *opaque, const GeneratedData &data);
    // or save it:
    DArray<DataMessage> aReceivedDataQueue;
};

class DPN_DataDirection : public DPN_Direction {
public:
    bool close() override {return true;}
    void setChannel(DPN_DataChannel *c) {pChannel = c;}
protected:
    DPN_DataChannel *pChannel;
};
class DPN_DataSendDirection : public DPN_DataDirection {
public:
    bool proc() override;
};
class DPN_DataReceiveDirection : public DPN_DataDirection {
public:
    bool proc() override;
};

///
/// \brief The DPN_DataStream class
/// Client unique instance
class DPN_DataStream : public DPN_AbstractModule {
public:
    DPN_DataStream();

    bool reserveShadowReceiver(DPN_Channel *channel, const DPN_ExpandableBuffer &extra, int transaction) override;
    bool reserveShadowSender(DPN_Channel *channel, int transaction) override;

    void clientDisconnected(const DPN_AbstractClient *client) override;
    void stop() override;
private:
    DArray<DPN_DataChannel*> aDataChannels;
    DPN_AbstractClient *pClient;
};

class DPN_DataStreamTransaction : public DPN_AbstractTransaction {
public:
    DPN_DataStreamTransaction();
    enum TransactionSubtype {
        NoType,
        ChannelReserving
    };

public:
    DPN_DataChannel::PacketType ePacketType;
    DPN_DataChannel::ChannelType eChannelType;
    DPN_DataChannel::ConnectionType eConnectionType;
    TransactionSubtype iSubtype;

private:

};

#endif // DPN_DATASTREAM_H
