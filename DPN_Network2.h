#ifndef DPN_NETWORK2_H
#define DPN_NETWORK2_H

#include "DPN_Network.h"

namespace DPN::Network {


    class MulticastProcessor : public DPN_TransmitProcessor {

    };
    enum ServerFlags {
        SERVER__FLG__BROADCAST_SUPPORTED
        , SERVER__FLG__MULTICAST_SUPPURTED
        , SERVER__FLG__UNICAST_SUPPORTED
    };
    enum ServerVisible {
        SV__EMPTY, SV__PUBLIC, SV__PRIVATE
    };

    // chat
    struct __server__
            : public DPN::Network::ClientCenter
    {

        void setServerVisible( ServerVisible sv );
        bool start( );

        DArray< DPN::Client::Core > aClients;
        DArray< DPN_AbstractModule* > aSupportedModules;
        uint32_t iFlags;
        ServerVisible iVisible;

//        DArray<__channel> aIncoming
    };
    ///
    /// \brief The Server class
    /// Server context:
    /// >>> supportable packet types:
    /// 1. client - server
    /// 2. client - server - client
    /// 3. multicast
    /// 4. broadcast
    ///
    /// >>> channel semantic
    /// 1. Channels arch (number & types)
    /// 2. using modules
    ///
    ///
    class Server
    {
    public:
    };
    class ServerAdapter {
    public:
        bool establishConnection( DXT::Type );
    };

    //================================================================================
    struct __client_network_data__ {
        DPN::Client::Environment iClientEnvironment;

    };
    class ClientNetworkInterface
            : public DPN::Interface::InterfaceReference< __client_network_data__ >
    {};
    class ClientNetworkPrivateInterface
            : public DPN::Interface::InterfaceReference< __client_network_data__ >
    {
    public:
        void setEnvironment( const DPN::Client::Environment &e );
    };
    class ClientNetworkDescriptor
            : public DPN::Interface::DataReference< __client_network_data__ >
    {};

    class NetworkSystem
            : public DPN::Network::NetworkModule
    {
    public:
        NetworkSystem(const std::string &name, DPN::Network::ClientCenter &cc);
        bool useChannel( DPN::Client::Tag tag, DPN::Direction d, __channel_mono_interface mono, const DPN_ExpandableBuffer &context ) override;

        ClientNetworkInterface getClientInterface( DPN::Client::Tag tag );
        ClientNetworkPrivateInterface getClientPrivateInterface( DPN::Client::Tag tag );

        void makeProxy( DPN::Client::Tag from, DPN::Client::Tag to );
        Server makeServer();
        void serverOpened();
    private:
        DArray< ServerAdapter > aRemoteServers;
    };
    NetworkSystem *extractNetworkModule( DPN::Modules m );
}


#endif // DPN_NETWORK2_H
