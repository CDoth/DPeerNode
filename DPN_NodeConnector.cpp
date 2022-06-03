#include "DPN_NodeConnector.h"
//#include <DServer.h>
#include "__dpeernode_global.h"

using namespace DPeerNodeSpace;
DPN_NodeConnector::DPN_NodeConnector() {

    iPacketSize = 0;
    iTb = 0;
}
DPN_NodeConnector::DPN_NodeConnector(DXT::Type t) : c(t) {

    iPacketSize = 0;
    iTb = 0;
}
bool DPN_NodeConnector::openPort(int port) {
    return c.makeServer(port);
}
bool DPN_NodeConnector::newConnection(int port) {
    return c.newConnection(port);
}
DPN_NodeConnector *DPN_NodeConnector::acceptConnection(int port) {

    DXT connection(DXT::TCP);
    connection.disconnectInDesctructor(false);

    if( connection.accept(port) ) {
        DPN_NodeConnector *__c = new DPN_NodeConnector;
        __c->c = connection;
        return __c;
    }

    return nullptr;
}
bool DPN_NodeConnector::connectTo(const PeerAddress &a) {
    return c.connect(a.port, a.address.c_str());
}
bool DPN_NodeConnector::connectTo(const char *address, int port) {
    return c.connect(port, address);
}
bool DPN_NodeConnector::close() {
    c.stop();
    return true;
}
bool DPN_NodeConnector::readable() {
    return c.readable();
}
bool DPN_NodeConnector::writable() {
    return c.writable();
}
std::string DPN_NodeConnector::peerAddress() const {
    return c.peerAddress();
}
int DPN_NodeConnector::peerPort() const {
    return c.peerPort();
}
std::string DPN_NodeConnector::localAddress() const {

    return c.localAddress();
}
int DPN_NodeConnector::localPort() const {

    return c.localPort();
}
std::string DPN_NodeConnector::peerName() const {
    std::string name = peerAddress();
    name = name + "::";
    name.append(std::to_string(peerPort()));
    return name;
}
std::string DPN_NodeConnector::localName() const {
    std::string name = localAddress();
    name = name + "::";
    name.append(std::to_string(localPort()));
    return name;
}
PeerAddress DPN_NodeConnector::local() const {
    return PeerAddress(localPort(), localAddress());
}
PeerAddress DPN_NodeConnector::peer() const {
    return PeerAddress(peerPort(), peerAddress());
}
DPN_Result DPN_NodeConnector::x_receivePacket() {
    return x_receivePacket(innerBuffer);
}
DPN_Result DPN_NodeConnector::x_receivePacket(DPN_ExpandableBuffer &b) {

    if( iPacketSize == 0 ) {
        int __ps = 0;
        if( (iTb = c.receive(&__ps, sizeof(int))) < 1 ) {
            RESULT_WRAPPER(DPN_FAIL);
        }
        if( iTb == sizeof(int)) iPacketSize = __ps;
        else {
            RESULT_WRAPPER(DPN_REPEAT);
        }
    }
//    DL_INFO(1, "packet size: [%d]", iPacketSize);

    if( iPacketSize == 0 ) {
        RESULT_WRAPPER(DPN_SUCCESS);
    } else {
        b.reserve(iPacketSize);
        if( (iTb = c.receive(b.getData(), iPacketSize)) < 1 ) {
            RESULT_WRAPPER(DPN_FAIL);
        }
//        DL_INFO(1, "data received: [%d]", iTb);
        if( iTb == iPacketSize ) {
            iPacketSize = 0;
            RESULT_WRAPPER(DPN_SUCCESS);
        }
    }
    RESULT_WRAPPER(DPN_REPEAT);
}
int DPN_NodeConnector::transportedBytes() const {
    return iTb;
}
void DPN_NodeConnector::clearInnerBuffer() {
    innerBuffer.clear();
}
std::string DPN_NodeConnector::innerDataToString() const {
    return std::string(
                reinterpret_cast<const char*>(innerBuffer.getData()),
                innerBuffer.size()
                );
}
DPN_Result DPN_NodeConnector::x_sendIt(const void *data, int size) {

    if( (iTb = c.send(data, size)) < 0 ) {
        RESULT_WRAPPER(DPN_FAIL);
    }
    if( iTb == size ) RESULT_WRAPPER(DPN_SUCCESS);
    RESULT_WRAPPER(DPN_REPEAT);
}
DPN_Result DPN_NodeConnector::x_send(const DPN_ExpandableBuffer &b) {


    if( (iTb = c.send(b.getData(), b.size())) < 0 ) {
        RESULT_WRAPPER(DPN_FAIL);
    }
    if( iTb == b.size() ) RESULT_WRAPPER(DPN_SUCCESS);
    RESULT_WRAPPER(DPN_REPEAT);
}
DPN_Result DPN_NodeConnector::x_send(const DPN_ExpandableBuffer &b, int size) {
    if(size > b.size()) RESULT_WRAPPER(DPN_FAIL);

    if( (iTb = c.send(b.getData(), size)) < 0 ) {
        RESULT_WRAPPER(DPN_FAIL);
    }
    if( iTb == size ) RESULT_WRAPPER(DPN_SUCCESS);
    RESULT_WRAPPER(DPN_REPEAT);
}
DPN_Result DPN_NodeConnector::x_receiveTo(void *place, int size) {

    if( (iTb = c.receive(place, size)) < 1) {
        RESULT_WRAPPER(DPN_FAIL);
    }
    if( iTb == size ) RESULT_WRAPPER(DPN_SUCCESS);
    RESULT_WRAPPER(DPN_REPEAT);
}
DPN_Result DPN_NodeConnector::x_receive(int size) {
     innerBuffer.reserve(size);

     if( (iTb = c.receive(innerBuffer.getData(), size)) < 1 ) {
         RESULT_WRAPPER(DPN_FAIL);
     }
     if( iTb == size ) RESULT_WRAPPER(DPN_SUCCESS);
     RESULT_WRAPPER(DPN_REPEAT);
}
DPN_Result DPN_NodeConnector::x_receive(DPN_ExpandableBuffer &b, int size) {
    b.reserve(size);

    if( (iTb = c.receive(b.getData(), size)) < 1 ) {
        RESULT_WRAPPER(DPN_FAIL);
    }
    if( iTb == size ) RESULT_WRAPPER(DPN_SUCCESS);
    RESULT_WRAPPER(DPN_REPEAT);
}
DPN_Result DPN_NodeConnector::x_receivePacket(void *place, int &packetSize) {

    if( iPacketSize == 0 ) {
        int __ps = 0;
        if( (iTb = c.receive(&__ps, sizeof(int))) < 1 ) {
            RESULT_WRAPPER(DPN_FAIL);
        }
        if( iTb == sizeof(int)) iPacketSize = __ps;
        else {
            RESULT_WRAPPER(DPN_REPEAT);
        }
    }
    if( iPacketSize == 0 ) {
        RESULT_WRAPPER(DPN_SUCCESS);
    } else {
        if( (iTb = c.receive(place, iPacketSize)) < 1 ) {
            RESULT_WRAPPER(DPN_FAIL);
        }
        if( iTb == iPacketSize ) {
            packetSize = iPacketSize;
            iPacketSize = 0;
            RESULT_WRAPPER(DPN_SUCCESS);
        }
    }
    RESULT_WRAPPER(DPN_REPEAT);
}
//DPN_Result DPN_NodeConnector::x_receivePacketSize(int &packetSize) {
//    if( (tb = c.unlocked_recv_packet_size(&packetSize)) <= 0 ) {
//        RESULT_WRAPPER(DPN_FAIL);
//    }
//    if( tb == packetSize ) RESULT_WRAPPER(DPN_SUCCESS);
//    RESULT_WRAPPER(DPN_REPEAT);
//}
//DPN_Result DPN_NodeConnector::x_receivePacketData(void *place) {

//    int packetSize = 0;
//    if( (tb = c.unlocked_recv_packet_data(place, &packetSize)) <= 0 ) {
//        RESULT_WRAPPER(DPN_FAIL);
//    }
//    if( tb == packetSize ) RESULT_WRAPPER(DPN_SUCCESS);
//    RESULT_WRAPPER(DPN_REPEAT);
//}
//DPN_Result DPN_NodeConnector::x_receivePacketData() {
//    if( !c.packetSizeReceived() ) {
//        RESULT_WRAPPER(DPN_REPEAT);
//    }
//    if( ps == 0 ) {
//        RESULT_WRAPPER(DPN_SUCCESS);
//    }
//    innerBuffer.reserve(ps);
//    if( (tb = c.unlocked_recv_packet_data(innerBuffer.getData(), nullptr)) < 1 ) {
//        RESULT_WRAPPER(DPN_FAIL);
//    }
//    if( tb == ps ) {
//        RESULT_WRAPPER(DPN_SUCCESS);
//    }

//    RESULT_WRAPPER(DPN_REPEAT);
//}


DPN_UDPPort::DPN_UDPPort() : iUDPSocket(DXT::UDP)
{

}
