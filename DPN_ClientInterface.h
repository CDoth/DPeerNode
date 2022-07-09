#ifndef DPN_CLIENTDATA_H
#define DPN_CLIENTDATA_H

#include "DPN_Processors.h"
#include "DPN_ClientCore.h"



namespace DPN::Client {
    class Interface : private DPN::Client::Core {
    public:
        Interface( );
        Interface( DPN::Client::GlobalUnderlayerSpace &gus, DPN::Thread::ThreadUser &threadUser, DPN::Modules modules, DPN_NodeConnector *c );

        using Underlayer::tag;
        using MainChannel::badClient;

        DArray<__channel> channels() const;
        void sendMessage(const char *m);
        void makeShadowConnection(DXT::Type t);
        bool send(DPN_TransmitProcessor *p);

        DPN::Client::Core & core() { return *this; }
    public:
        DPN::Network::PeerAddress peer() const { return DPN::Client::Underlayer::remoteAddress(); }
    };

}



#endif // DPN_CLIENTDATA_H
