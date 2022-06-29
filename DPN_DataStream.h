#ifndef DPN_DATASTREAM_H
#define DPN_DATASTREAM_H

/*
#include "DPN_Channel.h"
// 1. direct window
// 2. cross-thread window
// 3. generative stream

class DPN_DataStream;
class DPN_DataChannel {




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
    DPN_NodeConnector *pConnector;
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

*/





/*
 * Input:
 * callback (callback, base)
 * queue
 * file (file path, window size, auto/handle mode)
 * mem (ptr, size, window size, auto/handle mode)
 * alien udp (local port, peer port, settings)
 * dpn udp (incoming udp channel, settings)
 * tcp (incoming shadow channel, settings)
 * other stream (stream, settings)
 *
 *
 * Output:
 * callback
 * queue
 * file
 * mem
 * alien udp
 * dpn udp
 * tcp
 * other stream
*/













#endif // DPN_DATASTREAM_H
