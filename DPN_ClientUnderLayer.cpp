#include "DPN_ClientUnderLayer.h"

using namespace DPN::Logs;

namespace DPN::Client {


    Underlayer::Underlayer() :
        dUnderLayer(false),
        GlobalUnderlayerSpace(false),
        DPN::Thread::ThreadUser(false),
        DPN::Modules(false)
    {}
    Underlayer::Underlayer(GlobalUnderlayerSpace gus, Thread::ThreadUser &threadUser, Modules &modules)  :
            dUnderLayer(true),
            GlobalUnderlayerSpace(gus),
            DPN::Thread::ThreadUser( threadUser ),
            DPN::Modules( modules )
    {

    }


    void Underlayer::init(DPN_NodeConnector *c) {
        if( c == nullptr || dUnderLayer::isEmptyObject() ) {
            DL_ERROR(1, "Can't init under layer: connector: [%p] empty object: [%d]", c, dUnderLayer::isEmptyObject());
            return;
        }
        dUnderLayer::data()->iStats.iLocal = c->local();
        dUnderLayer::data()->iStats.iRemote = c->peer();
    }
    Tag Underlayer::tag() const { return dUnderLayer::data(); }

    void Underlayer::disconnect() {
        if( dUnderLayer::isEmptyObject() ) {
            DL_ERROR(1, "Empty underlayer watcher");
            return;
        }
        if( dUnderLayer::data()->iStats.iState == CLIENT_DISCONNECTED ) {
            return;
        }
        DL_INFO(1,"Disconnect client [%p]", tag() );
        dUnderLayer::data()->iStats.iState = CLIENT_DISCONNECTED;
        DPN::Client::GlobalUnderlayerSpace::generateEvent( tag(), CL_EV__DISCONNECT );
    }
    bool Underlayer::isShadowAvailable(DXT::Type, int port) {
        return true;
    }
    Network::PeerAddress Underlayer::remoteAddress() const {
        if( dUnderLayer::isEmptyObject() ) return DPN::Network::PeerAddress();
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
//    bool Underlayer::setEnvironment(const Environment &e) {
//        if( dUnderLayer::isEmptyObject() ) {
//            DL_ERROR(1, "Empty watcher");
//            return false;
//        }
//        dUnderLayer::data()->iClientEnironment = e;
//        return true;
//    }

    GlobalUnderlayerSpace::GlobalUnderlayerSpace(bool makeSource) :
        dGlobalUnderlayerSpace(makeSource)
    {}

    bool GlobalUnderlayerSpace::generateEvent(Tag tag, ClientEventType type) {
        if( isEmptyObject() ) {
            DL_ERROR(1, "Empty watcher");
            return false;
        }
        ClientEvent e(tag, type);

        data()->iEvents.push_back( e );
        return true;
    }
    ClientEvent GlobalUnderlayerSpace::takeLastEvent() {
        if( isEmptyObject() ) {
            DL_ERROR(1, "Empty watcher");
            return ClientEvent();
        }
        //1
        return data()->iEvents.output().takeLast();
    }
    const Environment GlobalUnderlayerSpace::localEnironment() const {
        if( isEmptyObject() ) {
            DL_ERROR(1, "Empty watcher");
            return Environment();
        }
        return data()->iLocalEnvironment;
    }
    void GlobalUnderlayerSpace::addPeerNote(const Network::PeerAddress &peerAddress) {
        if( isEmptyObject() ) {
            DL_ERROR(1, "Empty watcher");
            return;
        }
        data()->iLocalEnvironment.append( peerAddress );
    }


}

