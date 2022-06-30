#include "DPN_ClientUnderLayer.h"

using namespace DPN::Logs;

namespace DPN::Client {


    Underlayer::Underlayer(Thread::ThreadUser &threadUser, Modules &modules)  :
        dUnderLayer(true),
        DPN::Thread::ThreadUser( threadUser ),
        DPN::Modules( modules )
    {}
    void Underlayer::init(DPN_NodeConnector *c) {
        if( c == nullptr || dUnderLayer::isEmptyObject() ) {
            DL_ERROR(1, "Can't init under layer: connector: [%p] empty object: [%d]", c, dUnderLayer::isEmptyObject());
            return;
        }
        dUnderLayer::data()->iStats.iLocal = c->local();
        dUnderLayer::data()->iStats.iRemote = c->peer();
    }
    bool Underlayer::isShadowAvailable(DXT::Type, int port) {
        return true;
    }
    PeerAddress Underlayer::remoteAddress() {
        if( dUnderLayer::isEmptyObject() ) return PeerAddress();
        return dUnderLayer::data()->iStats.iRemote;
    }
    bool Underlayer::addShadowConnection(DPN_NodeConnector *udpPort, const std::string shadowKey) {
        if( udpPort == nullptr || shadowKey.empty() ) {
            DL_BADVALUE(1, "port [%p] or key [%d]", udpPort, shadowKey.size());
            return false;
        }
        __channel ch;
        ch.init( udpPort, shadowKey );
        dUnderLayer::data()->aShadowChannels.append( ch );
        return true;
    }
    bool Underlayer::registerShadow(const std::string &shadowKey) {
        return false;
    }
    DArray<__channel> Underlayer::shadows() const  {
        if( dUnderLayer::isEmptyObject() ) return DArray<__channel>();

        return dUnderLayer::data()->aShadowChannels;
    }
    __channel Underlayer::channel(const std::string &key) {
        if( dUnderLayer::isEmptyObject() ) return __channel();

        FOR_VALUE( dUnderLayer::data()->aShadowChannels.size(), i ) {
            if( dUnderLayer::data()->aShadowChannels[i].shadowKey() == key ) return dUnderLayer::data()->aShadowChannels[i];
        }
        return __channel();
    }


}

