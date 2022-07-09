#ifndef DPN_CLIENTUNDERLAYER_H
#define DPN_CLIENTUNDERLAYER_H

#include "DPN_Channel.h"
#include "DPN_ThreadBridge.h"
#include "DPN_Modules.h"

namespace DPN::Client {

    enum ClientState {
        CLIENT_RAW
        , CLIENT_CONNECTED
        , CLIENT_DISCONNECTED
    };

    struct Stats {
        Stats() : iState(CLIENT_RAW) {}

        DPN::Network::PeerAddress iRemote;
        DPN::Network::PeerAddress iLocal;
        DPN::Network::PeerAddress iAvatar;
        ClientState iState;
    };
    //------------------------------------------------------------------------
    enum ClientEventType {
        CL_EV__NO,
        CL_EV__DISCONNECT
    };
    struct ClientEvent {
        ClientEvent() : iTag(nullptr), iType(CL_EV__NO) {}
        ClientEvent( DPN::Client::Tag tag, ClientEventType type ) : iTag(tag), iType(type) {}
        DPN::Client::Tag iTag;
        ClientEventType iType;
    };
    typedef DArray<DPN::Network::PeerAddress> Environment;
    struct __global_underlayer_space__ {
        mutable DPN::Util::_ThreadSafeQueue2<ClientEvent> iEvents;
        Environment iLocalEnvironment;
    };
    typedef DWatcher< __global_underlayer_space__ > dGlobalUnderlayerSpace;
    class GlobalUnderlayerSpace
            : private dGlobalUnderlayerSpace
    {
    public:
        GlobalUnderlayerSpace() {}
        GlobalUnderlayerSpace( bool makeSource );
        bool generateEvent( DPN::Client::Tag tag, ClientEventType type );
        ClientEvent takeLastEvent();
        const Environment localEnironment() const;
        void addPeerNote( const DPN::Network::PeerAddress &peerAddress);
    };
    //------------------------------------------------------------------------

    struct __underlayer__ {
        __underlayer__() {}
        DArray<__channel> aShadowChannels;
        Stats iStats;
    };
    typedef DWatcher< __underlayer__ > dUnderLayer;
    class Underlayer :
            private dUnderLayer,
            public GlobalUnderlayerSpace,
            public DPN::Thread::ThreadUser,
            public DPN::Modules
    {
    public:
        Underlayer();
        Underlayer( GlobalUnderlayerSpace gus, DPN::Thread::ThreadUser &threadUser, DPN::Modules &modules );
        void init( DPN_NodeConnector *c );
        inline bool empty() const { return dUnderLayer::isEmptyObject(); }

        Tag tag() const;

        void disconnect();
        bool isShadowAvailable(DXT::Type, int port);
        DPN::Network::PeerAddress remoteAddress() const;
        bool addShadowConnection(DPN_NodeConnector *udpPort, const std::string shadowKey);
        bool registerShadow(const std::string &shadowKey);
        DArray<__channel> shadows() const ;
        __channel channel( const std::string &key );


//        bool setEnvironment( const Environment &e );
    };
    //------------------------------------------------------------------------
}




#endif // DPN_CLIENTUNDERLAYER_H
