#include "DPN_Processors.h"

#include <DProfiler.h>

using namespace DPN::Logs;
namespace DPN_DefaultProcessor {
    //====================================================================================== TextMessage:
    void TextMessage::makeActionLine() {
        line<TextMessage>() << &TextMessage::render ;
    }
    DPN_Result TextMessage::render(SERVER_CALL) {
        INFO_LOG;

        std::string message = UNIT_MESSAGE.get();
        std::cout << " >>>>> receive message: [" << message << "]"
                  << " thread: " << std::this_thread::get_id()
                  << std::endl;

        return DPN_SUCCESS;
    }
    //====================================================================================== MakeShadow:
    DPN_Result MakeShadow::requestShadow(HOST_CALL) {
        INFO_LOG;
        UNIT_TYPE = iType;
        return DPN_SUCCESS;
    }
    DPN_Result MakeShadow::processShadow(SERVER_CALL) {

        INFO_LOG;
        DXT::Type type = UNIT_TYPE.get();
        iType = type;

        if( type == DXT::UDP ) {
            return DPN_SUCCESS;
        } else {
            int port = UNIT_PORT.get();
            bool a = isShadowAvailable(iType, port);
            DL_INFO(1, "Requesting shadow: [%d]", a);
            if( a ) return DPN_SUCCESS;
            return DPN_FAIL;
        }
    }
    DPN_Result MakeShadow::makeShadow(HOST_CALL) {

        INFO_LOG;
        std::string sk = "generate shadow key here";


        if( iType == DXT::UDP ) {
            if( (pPort = dpnOpenUDPPort()) == nullptr ) {
                DL_ERROR(1, "Can't open udp port");
                return DPN_FAIL;
            }
            UNIT_PORT = pPort->localPort();
        }
        shadowKey = sk;
        UNIT_KEY = sk;

        return DPN_SUCCESS;
    }
    DPN_Result MakeShadow::makeShadow(SERVER_CALL) {
        INFO_LOG;

        shadowKey = UNIT_KEY.get();
        if( iType == DXT::UDP ) {
            int remotePort = UNIT_PORT.get();
            if( (pPort = dpnOpenUDPPort()) == nullptr ) {
                DL_ERROR(1, "Can't open udp port");
                return DPN_FAIL;
            }
            DL_INFO(1, "Server: open UDP port: [%d] remote: [%d] [%s]",
                    pPort->localPort(), remotePort, remoteAddress().address.c_str()
                   );

            if( pPort->connectTo( DPN::Network::PeerAddress(remotePort, remoteAddress().address ) ) == false ) {
                DL_ERROR(1, "Can't connect UDP port");
                dpnCloseUDPPort( pPort );
                return DPN_FAIL;
            }
            if( addShadowConnection( pPort, shadowKey ) == false ) {
                DL_FUNCFAIL(1, "addShadowConnection");
                dpnCloseUDPPort( pPort );
                return DPN_FAIL;
            }
            DL_INFO(1, "UDP Port [%d] connected with host UDP port [%d]", pPort->localPort(), remotePort);
            UNIT_PORT = pPort->localPort();
        } else {
            registerShadow( shadowKey );
        }

        return DPN_SUCCESS;
    }
    DPN_Result MakeShadow::connect( HOST_CALL ) {
        INFO_LOG;

        if( iType == DXT::UDP ) {
            int remotePort = UNIT_PORT.get();
            if( pPort->connectTo( DPN::Network::PeerAddress( remotePort, remoteAddress().address )) == false ) {
                DL_ERROR(1, "Can't connect with server UDP port");
                dpnCloseUDPPort( pPort );
                return DPN_FAIL;
            }

            if( addShadowConnection( pPort, shadowKey ) == false ) {
                DL_FUNCFAIL(1, "addShadowConnection");
                dpnCloseUDPPort( pPort );
                return DPN_FAIL;
            }
            DL_INFO(1, "UDP Port [%d] connected with server UDP port [%d]", pPort->localPort(), remotePort);
        } else {
            int port = UNIT_PORT.get();
    //        clientContext.client()->shadowConnectionServerAnswer(true, port, tr->shadowKey);
        }

        return DPN_SUCCESS;
    }
    void MakeShadow::fault(SERVER_CALL) {
        DL_INFO(1, "MakeShadow: Server Fault");
    }
    void MakeShadow::fault(HOST_CALL) {
        DL_INFO(1, "MakeShadow: Host Fault");
    }
    void MakeShadow::makeActionLine() {

        line<MakeShadow>()
                << &MakeShadow::requestShadow
                << &MakeShadow::processShadow
                << HostCallbackWrapper( &MakeShadow::makeShadow )
                << ServerCallbackWrapper( &MakeShadow::makeShadow )
                << &MakeShadow::connect
                   ;
    }
    //====================================================================================== SyncChannel:
    DPN_Result SyncChannel::hostRequest(HOST_CALL) {

        UNIT_SHADOW_KEY = wChannel.shadowKey();
        UNIT_FORWARD_STATE = wChannel.localState( DPN::FORWARD );
        UNIT_BACKWARD_STATE = wChannel.localState( DPN::BACKWARD );
        return DPN_SUCCESS;
    }
    DPN_Result SyncChannel::serverSync(SERVER_CALL) {

        std::string key = UNIT_SHADOW_KEY.get();
        MonoChannelState f = UNIT_FORWARD_STATE.get();
        MonoChannelState b = UNIT_BACKWARD_STATE.get();

//        wChannel = clientUnderlayer().channel( key );

//        __channel_private_interface pi = wChannel.privateInterface();
//        if( pi.badInterface() ) {
//            DL_ERROR(1, "Bad interface");
//            return DPN_FAIL;
//        }
//        pi.setRemoteState( DPN::FORWARD, f );
//        pi.setRemoteState( DPN::BACKWARD, b );

//        return DPN_SUCCESS;
    }
    void SyncChannel::makeActionLine() {

    }
    //====================================================================================== ReserveChannel:
    ReserveChannel::ReserveChannel() {
        pForwardUser = nullptr;
        pBackwardUser = nullptr;
    }
    DPN_Result ReserveChannel::hostRequest(HOST_CALL) {

        INFO_LOG;
        UNIT_FORWARD_MODULE_NAME.clearBuffer();
        UNIT_BACKWARD_MODULE_NAME.clearBuffer();

        while( pForwardUser ) {
            __channel_mono_interface f = wChannel.getMonoIf( DPN::FORWARD );
//            if( f.badInterface() ) {
//                DL_WARNING(1, "Bad forward interface");
//                break;
//            }
            if( pForwardUser->useChannel( tag(), DPN::FORWARD, f, wForwardContext ) == false ) {
                DL_FUNCFAIL(1, "useOutgoingChannel");
            }
            UNIT_FORWARD_MODULE_NAME = pForwardUser->name();
            UNIT_FORWARD_CONTEXT = wForwardContext;
            break;
        }
        while( pBackwardUser ) {
            __channel_mono_interface b = wChannel.getMonoIf( DPN::BACKWARD );
//            if( b.badInterface() ) {
//                DL_WARNING(1, "Bad backward interface");
//                break;
//            }
            if( pBackwardUser->useChannel( tag(), DPN::BACKWARD, b, wBackwardContext ) == false ) {
                DL_FUNCFAIL(1, "useIncomingChannel");
            }
            UNIT_BACKWARD_MODULE_NAME = pBackwardUser->name();
            UNIT_BACKWARD_CONTEXT = wBackwardContext;
            break;
        }
        UNIT_SHADOW_KEY = wChannel.shadowKey();

        return DPN_SUCCESS;
    }
    DPN_Result ReserveChannel::serverProcess(SERVER_CALL) {

        INFO_LOG;

        std::string key = UNIT_SHADOW_KEY.get();
        std::string forwardName = UNIT_FORWARD_MODULE_NAME.get();
        std::string backwardName = UNIT_BACKWARD_MODULE_NAME.get();
        wForwardContext = UNIT_FORWARD_CONTEXT.get();
        wBackwardContext = UNIT_BACKWARD_CONTEXT.get();


        if( forwardName.empty() && backwardName.empty() ) {
            DL_WARNING(1, "No users");
            return DPN_FAIL;
        }
        std::swap( forwardName, backwardName );
        std::swap( wForwardContext, wBackwardContext );

        wChannel = channel( key );
//        if( wChannel.isValid() == false ) {
//            DL_ERROR(1, "Can't find channel with key: [%s]", key.c_str());
//            return DPN_FAIL;
//        }

        if( forwardName.size() ) {
            if( (pForwardUser = module( forwardName )) == nullptr ) {
                DL_ERROR(1, "Can't find module (forward) with name: [%s]", forwardName.c_str());
                return DPN_FAIL;
            }
            __channel_mono_interface mono = wChannel.getMonoIf( DPN::FORWARD );
//            if( mono.badInterface() ) {
//                DL_ERROR(1, "Bad forward interface");
//                return DPN_FAIL;
//            }
            if( pForwardUser->useChannel( tag(), DPN::FORWARD, mono, wForwardContext ) == false ) {
                DL_FUNCFAIL(1, "useOutgoingChannel");
                return DPN_FAIL;
            }
        }
        if( backwardName.size() ) {
            if( (pBackwardUser = module( backwardName )) == nullptr ) {
                DL_ERROR(1, "Can't find module (backward) with name: [%s]", forwardName.c_str());
                return DPN_FAIL;
            }
            __channel_mono_interface mono = wChannel.getMonoIf( DPN::BACKWARD );
//            if( mono.badInterface() ) {
//                DL_ERROR(1, "Bad backward interface");
//                return DPN_FAIL;
//            }
            if( pBackwardUser->useChannel( tag(), DPN::FORWARD, mono, wBackwardContext ) == false ) {
                DL_FUNCFAIL(1, "useIncomingChannel");
                return DPN_FAIL;
            }
        }
        return DPN_SUCCESS;
    }
    void ReserveChannel::makeActionLine() {
        line<ReserveChannel>()
                << &ReserveChannel::hostRequest
                << &ReserveChannel::serverProcess
                   ;
    }

}






