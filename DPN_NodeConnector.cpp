#include "DPN_NodeConnector.h"
#include "__dpeernode_global.h"

using namespace DPeerNodeSpace;
struct __udp_port {
    __udp_port() : __port(-1), __bound(false) {}
    __udp_port(int p) : __port(p), __bound(false) {}
    inline bool operator==(const __udp_port &p) const {return __port == p.__port;}
    inline bool operator==(int port) const {return (int)__port == port;}
    uint16_t __port;
    bool __bound;
};
DArray<__udp_port> __available_udp_ports;
void dpnAddUDPPort(int p) {
    if( __available_udp_ports.contain( p ) == false ) __available_udp_ports.append( p );
}
void unuseUDPPort(int p) {
    if( DXT_CHECK_PORT(p) == false ) return;
    int i = __available_udp_ports.indexOf( p );
    if( i != -1 ) __available_udp_ports[i].__bound = false;
}
void dpnAddUDPPorts(int first, int n) {
    if( n < 1 ) return;
    while(n) dpnAddUDPPort( first + n-- );
}
DPN_NodeConnector *dpnOpenUDPPort() {
    if( __available_udp_ports.empty() ) {
        DL_ERROR(1, "no available UDP ports");
        return nullptr;
    }
    DPN_NodeConnector *p = new DPN_NodeConnector(DXT::UDP);
    FOR_VALUE( __available_udp_ports.size(), i ) {
        if( p->bind( __available_udp_ports[i].__port ) ) {
            __available_udp_ports[i].__bound = true;
            return p;
        }
    }
    DL_ERROR(1, "Can't bind to available ports");
    delete p;
    return nullptr;
}
void dpnCloseUDPPort(DPN_NodeConnector *c) {
    if( c && c->type() == DXT::UDP ) {
        c->close();
        delete c;
    }
}

DPN_NodeConnector::DPN_NodeConnector(DXT::Type type) : c(type) {

    iInitiator = false;
    iPacketSize = 0;
    iTb = 0;
    iForceSending = type == DXT::TCP ? true : false;
    iForceReceiving = type == DXT::TCP ? true : false;
}
DPN_NodeConnector::~DPN_NodeConnector() {
    if( c.type() == DXT::UDP ) unuseUDPPort( c.localPort() );
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
        DPN_NodeConnector *__c = new DPN_NodeConnector( c.type() );
        __c->c = connection;
        return __c;
    }

    return nullptr;
}
bool DPN_NodeConnector::connectTo(const PeerAddress &a) {
    if( c.connect(a.port, a.address.c_str()) ) {
        iInitiator = true;
        return true;
    }
    return false;
}
bool DPN_NodeConnector::connectTo(const char *address, int port) {
    if( c.connect(port, address) ) {
        iInitiator = true;
        return true;
    }
    return false;
}
bool DPN_NodeConnector::close() {
    if( c.type() == DXT::UDP ) unuseUDPPort( c.localPort() );
    c.stop();
    return true;
}
bool DPN_NodeConnector::bind(int port) {
    return c.bind(port);
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
//==========================================================================
DPN_Result DPN_NodeConnector::x_sendIt(const void *data, int size) {

    if( (iTb = c.send(data, size)) < 0 ) {
        RESULT_WRAPPER(DPN_FAIL);
    }
    if( iForceSending && iTb != size ) RESULT_WRAPPER(DPN_REPEAT);
    RESULT_WRAPPER(DPN_SUCCESS);
}
DPN_Result DPN_NodeConnector::x_send(const DPN_ExpandableBuffer &b) {


    if( (iTb = c.send(b.getData(), b.size())) < 0 ) {
        RESULT_WRAPPER(DPN_FAIL);
    }

    if( iForceSending && iTb != b.size() ) RESULT_WRAPPER(DPN_REPEAT);
    RESULT_WRAPPER(DPN_SUCCESS);
}
DPN_Result DPN_NodeConnector::x_send(const DPN_ExpandableBuffer &b, int size) {
    if(size > b.size()) RESULT_WRAPPER(DPN_FAIL);

    if( (iTb = c.send(b.getData(), size)) < 0 ) {
        RESULT_WRAPPER(DPN_FAIL);
    }

    if( iForceSending && iTb != size ) RESULT_WRAPPER(DPN_REPEAT);
    RESULT_WRAPPER(DPN_SUCCESS);
}
DPN_Result DPN_NodeConnector::x_receiveTo(void *place, int size) {

    if( (iTb = c.receive(place, size)) < 1) {
        RESULT_WRAPPER(DPN_FAIL);
    }

    if( iForceReceiving && iTb != size ) RESULT_WRAPPER(DPN_REPEAT);
    RESULT_WRAPPER(DPN_SUCCESS);
}
DPN_Result DPN_NodeConnector::x_receive(int size) {
     innerBuffer.reserve(size);

     if( (iTb = c.receive(innerBuffer.getData(), size)) < 1 ) {
         RESULT_WRAPPER(DPN_FAIL);
     }

     if( iForceReceiving && iTb != size ) RESULT_WRAPPER(DPN_REPEAT);
     RESULT_WRAPPER(DPN_SUCCESS);
}
DPN_Result DPN_NodeConnector::x_receive(DPN_ExpandableBuffer &b, int size) {
    b.reserve(size);

    if( (iTb = c.receive(b.getData(), size)) < 1 ) {
        RESULT_WRAPPER(DPN_FAIL);
    }
    if( iForceReceiving && iTb != size ) RESULT_WRAPPER(DPN_REPEAT);
    RESULT_WRAPPER(DPN_SUCCESS);
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
//==========================================================================

const char *connectionTypeName(DXT::Type t) { return t == DXT::UDP ? "UDP" : "TCP"; }







