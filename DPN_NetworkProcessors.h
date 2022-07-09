#ifndef DPN_NETWORKPROCESSORS_H
#define DPN_NETWORKPROCESSORS_H

#include "DPN_TransmitProcessor.h"
#include "DPN_Network2.h"

namespace DPN_NetworkProcessors {

    class Base : public DPN_TransmitProcessor {
    public:
        Base();
        void injection() override;
    protected:
        inline DPN::Network::NetworkSystem *networkSystem() { return pNetworkSystem; }
        inline bool badModule() const { return pNetworkSystem == nullptr; }
    private:
        DPN::Network::NetworkSystem *pNetworkSystem;
    };

    class RequestLocalEnvironment : public Base {
    public:
        DPN_PROCESSOR
    private:
        void makeActionLine() override;
    private:
        DPN_Result request( HOST_CALL );
        DPN_Result process( SERVER_CALL );
        DPN_Result getEnivronment( HOST_CALL );
    private:
        DECL_UNIT(DArray<DPN::Network::PeerAddress>, UNIT_ENV);
    };
    class Forwarding : public Base {
    public:
        DPN_PROCESSOR;
        void setTargetAddress( const DPN::Network::PeerAddress &address ) {
            UNIT_TARGET = address;
        }
    private:
        void makeActionLine() override;
    private:
        DPN_Result request( HOST_CALL );
        DPN_Result process( SERVER_CALL );
        DPN_Result checkAnswer( HOST_CALL );
    private:
        DECL_UNIT(DPN::Network::PeerAddress, UNIT_TARGET);
    };

    /*
     * Server packets:
     * 1. Server opener
     * 2. Client connect request
     * 3. server metainfo (notifications)
    */
    class ServerOpener : public Base {

    };
    class ServerClientRequest : public Base {

    };
    class ServerNotification : public Base {

    };


}
using namespace DPN_NetworkProcessors;
DPN_PROCESSOR_BIND(RequestLocalEnvironment, PT__NW__LOCAL_ENVIRONMENT);
DPN_PROCESSOR_BIND(Forwarding, PT__NW__FORWARDING);
#endif // DPN_NETWORKPROCESSORS_H
