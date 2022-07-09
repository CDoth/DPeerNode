#include "DPN_NetworkProcessors.h"

using namespace DPN::Logs;
namespace DPN_NetworkProcessors {
    //====================================================================================== RequestLocalEnvironment:
    void RequestLocalEnvironment::makeActionLine() {
        line<RequestLocalEnvironment>()
                << &RequestLocalEnvironment::request
                << &RequestLocalEnvironment::process
                << &RequestLocalEnvironment::getEnivronment
                   ;
    }
    DPN_Result RequestLocalEnvironment::request(HOST_CALL) {
        INFO_LOG;
        return DPN_SUCCESS;
    }
    DPN_Result RequestLocalEnvironment::process(SERVER_CALL) {
        INFO_LOG;
        const DPN::Client::Environment e = localEnironment();
        if( e.empty() ) {
            DL_ERROR(1, "Empty environment");
            return DPN_FAIL;
        }
        UNIT_ENV = e;
        return DPN_SUCCESS;
    }
    DPN_Result RequestLocalEnvironment::getEnivronment(HOST_CALL) {
        INFO_LOG;
        const DPN::Client::Environment e = UNIT_ENV.get();
        if( e.empty() ) {
            DL_ERROR(1, "Empty environment");
            return DPN_FAIL;
        }
        if( badModule() ) {
            DL_ERROR(1, "Bad module");
            return DPN_FAIL;
        }
        auto ii = networkSystem()->getClientPrivateInterface( tag() );
        ii.setEnvironment( e );
        return DPN_SUCCESS;
    }

    //====================================================================================== Forwarding:
    void Forwarding::makeActionLine() {
        line<Forwarding>()
                << &Forwarding::request
                << &Forwarding::process
                << &Forwarding::checkAnswer
                   ;
    }
    DPN_Result Forwarding::request(HOST_CALL) {
        INFO_LOG;
        return DPN_SUCCESS;
    }
    DPN_Result Forwarding::process(SERVER_CALL) {
        INFO_LOG;
        if( badModule() ) {
            DL_ERROR(1, "Bad module");
            return DPN_FAIL;
        }

        DPN::Network::PeerAddress target = UNIT_TARGET.get();

        DPN::Client::Tag from = tag();
        DPN::Client::Tag to = networkSystem()->pa2tag( target );
        if( to == nullptr ) {
            DL_ERROR(1, "No target [%s] in local environment", target.name().c_str());
            return DPN_FAIL;
        }
        networkSystem()->makeProxy( from, to );

        return DPN_SUCCESS;
    }
    DPN_Result Forwarding::checkAnswer(HOST_CALL) {
        INFO_LOG;
        return DPN_SUCCESS;
    }
}

