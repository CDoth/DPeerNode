#ifndef DPN_NODECONNECTOR_H
#define DPN_NODECONNECTOR_H

#include <DXT.h>
#include "DPN_Buffers.h"

#define RESULT_WRAPPER(R) {result = R; return R;}
class DPN_NodeConnector {
public:
    DPN_NodeConnector();
    DPN_NodeConnector(DXT::Type);

    bool openPort(int port);
    bool newConnection(int port);
    DPN_NodeConnector * acceptConnection(int port);
    bool connectTo(const PeerAddress &a);
    bool connectTo(const char *address, int port);
    bool close();

public:
    bool readable();
    bool writable();
public:
    std::string peerAddress() const;
    int peerPort() const;
    std::string localAddress() const;
    int localPort() const;

    std::string peerName() const;
    std::string localName() const;

    PeerAddress local() const;
    PeerAddress peer() const;
public:
    //--------------------------------------------------------------------
    int transportedBytes() const;
    const DPN_ExpandableBuffer & buffer() const {return innerBuffer;}
    const uint8_t *bufferData() const {return innerBuffer.getData();}
    std::string innerDataToString() const;
    DPN_Result state() const {return result;}
    void clearInnerBuffer();
    //--------------------------------------------------------------------
    DPN_Result x_sendIt(const void *data, int size);
    DPN_Result x_send(const DPN_ExpandableBuffer &b);
    DPN_Result x_send(const DPN_ExpandableBuffer &b, int size);
    DPN_Result x_receiveTo(void *place, int size);
    DPN_Result x_receive(int size);
    DPN_Result x_receive(DPN_ExpandableBuffer &b, int size);

    DPN_Result x_receivePacket(void *place, int &packetSize);
    DPN_Result x_receivePacket();
    DPN_Result x_receivePacket(DPN_ExpandableBuffer &b);


    template <class T>
    DPN_Result x_sendValue(const T &value) {


        if( (iTb = c.send(&value, sizeof(T))) < 0 ) {
            RESULT_WRAPPER(DPN_FAIL);
        }
        if(iTb == sizeof(T)) RESULT_WRAPPER(DPN_SUCCESS);
        RESULT_WRAPPER(DPN_REPEAT);
    }
    template <class T>
    DPN_Result x_receiveValue(T &value) {

        if( (iTb = c.receive(&value, sizeof(T))) < 1 ) {
            RESULT_WRAPPER(DPN_FAIL);
        }
        if(iTb == sizeof(T)) RESULT_WRAPPER(DPN_SUCCESS);
        RESULT_WRAPPER(DPN_REPEAT);
    }
    //--------------------------------------------------------------------
private:

    DXT c;
    DPN_ExpandableBuffer innerBuffer;
    DPN_Result result;
    int iTb;
    int iPacketSize;
};
class DPN_UDPPort {
public:
    DPN_UDPPort();
    inline bool bind(int port) { return iUDPSocket.bind(port); }
    inline bool connect(const PeerAddress &r) { return iUDPSocket.connect(r.port, r.address.c_str()); }
    inline DXT & socket() { return iUDPSocket; }
    inline const DXT & socket() const { return iUDPSocket; }
private:
    DXT iUDPSocket;
};

#endif // DPN_NODECONNECTOR_H
