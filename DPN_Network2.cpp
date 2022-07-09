#include "DPN_Network2.h"

using namespace DPN::Logs;
namespace DPN::Network {
    NetworkSystem::NetworkSystem(const std::string &name, ClientCenter &cc)
        : DPN::Network::NetworkModule( name, cc )
    {   }
    bool NetworkSystem::useChannel(Client::Tag tag, Direction d, __channel_mono_interface mono, const DPN_ExpandableBuffer &context) {
        return false;
    }
    void NetworkSystem::makeProxy(Client::Tag from, Client::Tag to) {

        auto F = tag2client( from );
        auto T = tag2client( to );
        ConnectionContext context;
        context.makeProxy( F.core(), T.core() );

        ClientCenter::connectTo( F.peer(), context );
        ClientCenter::connectTo( T.peer(), context );
    }
    //======================================================================================================================================
    NetworkSystem *extractNetworkModule(Modules m) {
        return reinterpret_cast<NetworkSystem*>( m.module( "NetworkModule") );
    }
    void ClientNetworkPrivateInterface::setEnvironment(const Client::Environment &e) {
        if( badInterface() ) {
            DL_ERROR(1, "Bad interface");
            return;
        }
        inner()->iClientEnvironment = e;
    }

    //===============================


    bool __server__::start() {

        if( iVisible == SV__EMPTY ) {
            DL_ERROR(1, "Server visible unseted");
            return false;
        }
        auto cls = clients();
        if( cls.empty() ) {
            DL_ERROR(1, "No clients");
            return false;
        }
        if( iVisible == SV__PUBLIC ) {
            auto proc = DPN_PROCS::processor<PT__NW__SERVER_OPENER>();
            // Set server context to proc...
            FOR_VALUE( cls.size(), i ) {
                cls[i].send( proc );
            }
        }
        return true;
    }

}
