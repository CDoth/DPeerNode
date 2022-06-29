#include "DPN_Processors.h"

#include <DProfiler.h>

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
            bool a = clientUnderlayer().isShadowAvailable(iType, port);
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
                    pPort->localPort(), remotePort, clientUnderlayer().remoteAddress().address.c_str()
                   );

            if( pPort->connectTo( PeerAddress(remotePort, clientUnderlayer().remoteAddress().address ) ) == false ) {
                DL_ERROR(1, "Can't connect UDP port");
                dpnCloseUDPPort( pPort );
                return DPN_FAIL;
            }
            if( clientUnderlayer().addShadowConnection( pPort, shadowKey ) == false ) {
                DL_FUNCFAIL(1, "addShadowConnection");
                dpnCloseUDPPort( pPort );
                return DPN_FAIL;
            }
            DL_INFO(1, "UDP Port [%d] connected with host UDP port [%d]", pPort->localPort(), remotePort);
            UNIT_PORT = pPort->localPort();
        } else {
            clientUnderlayer().registerShadow( shadowKey );
        }

        return DPN_SUCCESS;
    }
    DPN_Result MakeShadow::connect( HOST_CALL ) {
        INFO_LOG;

        if( iType == DXT::UDP ) {
            int remotePort = UNIT_PORT.get();
            if( pPort->connectTo( PeerAddress( remotePort, clientUnderlayer().remoteAddress().address )) == false ) {
                DL_ERROR(1, "Can't connect with server UDP port");
                dpnCloseUDPPort( pPort );
                return DPN_FAIL;
            }

            if( clientUnderlayer().addShadowConnection( pPort, shadowKey ) == false ) {
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
        UNIT_FORWARD_STATE = wChannel.localState( DPN_FORWARD );
        UNIT_BACKWARD_STATE = wChannel.localState( DPN_BACKWARD );
        return DPN_SUCCESS;
    }
    DPN_Result SyncChannel::serverSync(SERVER_CALL) {

        std::string key = UNIT_SHADOW_KEY.get();
        MonoChannelState f = UNIT_FORWARD_STATE.get();
        MonoChannelState b = UNIT_BACKWARD_STATE.get();

        wChannel = clientUnderlayer().channel( key );

        __channel_private_interface pi = wChannel.privateInterface();
        if( pi.badInterface() ) {
            DL_ERROR(1, "Bad interface");
            return DPN_FAIL;
        }
        pi.setRemoteState( DPN_FORWARD, f );
        pi.setRemoteState( DPN_BACKWARD, b );

        return DPN_SUCCESS;
    }
    void SyncChannel::makeActionLine() {

    }
    //====================================================================================== ReserveChannel:
    ReserveChannel::ReserveChannel() {
        pForwardUser = nullptr;
        pBackwardUser = nullptr;
    }
    DPN_Result ReserveChannel::hostRequest(HOST_CALL) {

        UNIT_FORWARD_MODULE_NAME.clearBuffer();
        UNIT_BACKWARD_MODULE_NAME.clearBuffer();

        while( pForwardUser ) {
            __channel_mono_interface f = wChannel.getMonoIf( DPN_FORWARD );
            if( f.badInterface() ) {
                DL_WARNING(1, "Bad forward interface");
                break;
            }
            if( pForwardUser->useChannel( clientUnderlayer().tag(), DPN_FORWARD, f, wForwardContext ) == false ) {
                DL_FUNCFAIL(1, "useOutgoingChannel");
            }
            UNIT_FORWARD_MODULE_NAME = pForwardUser->name();
            UNIT_FORWARD_CONTEXT = wForwardContext;
            break;
        }
        while( pBackwardUser ) {
            __channel_mono_interface b = wChannel.getMonoIf( DPN_BACKWARD );
            if( b.badInterface() ) {
                DL_WARNING(1, "Bad backward interface");
                break;
            }
            if( pBackwardUser->useChannel( clientUnderlayer().tag(), DPN_BACKWARD, b, wBackwardContext ) == false ) {
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

        wChannel = clientUnderlayer().channel( key );
        if( wChannel.isValid() == false ) {
            DL_ERROR(1, "Can't find channel with key: [%s]", key.c_str());
            return DPN_FAIL;
        }

        if( forwardName.size() ) {
            if( (pForwardUser = modules().module( forwardName )) == nullptr ) {
                DL_ERROR(1, "Can't find module (forward) with name: [%s]", forwardName.c_str());
                return DPN_FAIL;
            }
            __channel_mono_interface mono = wChannel.getMonoIf( DPN_FORWARD );
            if( mono.badInterface() ) {
                DL_ERROR(1, "Bad forward interface");
                return DPN_FAIL;
            }
            if( pForwardUser->useChannel( clientUnderlayer().tag(), DPN_FORWARD, mono, wForwardContext ) == false ) {
                DL_FUNCFAIL(1, "useOutgoingChannel");
                return DPN_FAIL;
            }
        }
        if( backwardName.size() ) {
            if( (pBackwardUser = modules().module( backwardName )) == nullptr ) {
                DL_ERROR(1, "Can't find module (backward) with name: [%s]", forwardName.c_str());
                return DPN_FAIL;
            }
            __channel_mono_interface mono = wChannel.getMonoIf( DPN_BACKWARD );
            if( mono.badInterface() ) {
                DL_ERROR(1, "Bad backward interface");
                return DPN_FAIL;
            }
            if( pBackwardUser->useChannel( clientUnderlayer().tag(), DPN_FORWARD, mono, wBackwardContext ) == false ) {
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


//====================================================================================== DPN_Processor__ping:
/*
DPN_Result DPN_Processor__ping::sendPrefix() {

//    DL_INFO(1, "------------ state: [%d] UNIT: [%d]", mode, UNIT_MODE.value());

    if(mode == PING) {

        moment.fixTime();

        UNIT_SECONDS = moment.lt_seconds();
        UNIT_USECONDS = moment.lt_useconds();
        UNIT_MODE = mode;



        DL_INFO(1, "SEND PING: s: [%d] u: [%d] proc: [%p]", moment.lt_seconds(), moment.lt_useconds(), this);
    } else {

        int sec = UNIT_SECONDS.get();
        int usec = UNIT_USECONDS.get();

        UNIT_MODE = mode;
        mode = PING;

        DL_INFO(1, "SEND ANSWER: s: [%d] u: [%d] proc: [%p]", sec, usec, this);
    }
    return DPN_SUCCESS;
}
DPN_Result DPN_Processor__ping::receiveReaction() {

    mode = UNIT_MODE.get();

//    DL_INFO(1, "------------ state: [%d] UNIT: [%d]", mode, UNIT_MODE.value());

    if(mode == PING) {

        int sec = UNIT_SECONDS.get();
        int usec = UNIT_USECONDS.get();
//        UNIT_SECONDS.save();
//        UNIT_USECONDS.save();

        DL_INFO(1, "RECV PING: s: [%d] u: [%d] proc: [%p]", sec, usec, this);

        mode = ANSWER;
//        threadContext.send(this);

    } else {
        int sec = UNIT_SECONDS.get();
        int usec = UNIT_USECONDS.get();

        moment.fixTime();

        struct timeval orig = {sec, usec};
        struct timeval now = moment.lt();
        struct timeval dif = PROFILER::time_dif(&orig, &now);

        mode = PING;
        UNIT_MODE = PING;

        DL_INFO(1, "RECV ANSWER: dif: s: [%d] u: [%d] ",
                dif.tv_sec, dif.tv_usec
                );
    }

    return DPN_SUCCESS;
}
*/
//====================================================================================== DPN_Processor__request:
/*
DPN_Result DPN_Processor__request::sendPrefix() {

    RequestType type = UNIT_REQUEST_TYPE.get();
    switch (type) {

        break;
    }
    return DPN_SUCCESS;
}
DPN_Result DPN_Processor__request::receiveReaction() {

    INFO_LOG;
    bool isAnswer = UNIT_IS_ANSWER.get();
//    DL_INFO(1, "is answer: [%d]", isAnswer);
    if( isAnswer ) return procReceivingAnswer();
    return procReceivingRequest();
}
DPN_Result DPN_Processor__request::procReceivingRequest() {

    INFO_LOG;
    RequestType type = UNIT_REQUEST_TYPE.get();

//    DL_INFO(1, "type: [%d]", type);
    switch (type) {
    case REQUEST_NET_LOCAL_ENVIRONMENT:
        send(processor<DPN_PACKETTYPE__LOCAL_ENVIRONMENT>());
        break;
    case REQUEST_SHADOW_CONNECTION: {

        int port = UNIT_VALUE.get();
        UNIT_STATE = clientContext.core()->isShadowAvailable(clientContext.client(), port);

        DL_INFO(1, "Requesting shadow: [%d]", UNIT_STATE.get());
        makeAnswer();
        send(this);

        break;
    }
    case REQUEST_CHANNEL_RESERVING: {

        DL_INFO(1, "proc REQUEST_CHANNEL_RESERVING");
//        int tr = UNIT_TRANSACTION.get();

        std::string moduleName = UNIT_MODULE_NAME.get();
        std::string shadowKey = UNIT_HASH.get();
        PeerAddress source = UNIT_SOURCE_ADDRESS.get();
        PeerAddress target = UNIT_TARGET_ADDRESS.get();
        DPN_ExpandableBuffer extra = UNIT_EXTRA_DATA.get();

        DPN_Channel *channel = clientContext.client()->channel(shadowKey);
        DPN_AbstractModule *m = clientContext.modules()->module(moduleName);

        DL_INFO(1, "REQUEST_CHANNEL_RESERVING: transaction: [%d] moduleName: [%s] shadow: [%s] source: [%s] target: [%s] extra size: [%d]"
                   " channel: [%p] module: [%p]", 0, moduleName.c_str(), shadowKey.c_str(), source.name().c_str(), target.name().c_str(),
                extra.size(), channel, m
                );

        if( !channel || !m ) {
            DL_BADPOINTER(1, "channel: [%p] module (%s): [%p]", channel, moduleName.c_str(), m);
            setValue(DPN_BACKWARD);
            goto fail;
        }
        if( channel->remote() != source || channel->avatar() != target ) {

            DL_ERROR(1, "Bad stats: channel: remote: [%s] source: [%s] avatar: [%s] target: [%s]",
                     channel->remote().name().c_str(),
                     source.name().c_str(),
                     channel->avatar().name().c_str(),
                     target.name().c_str()
                     );
            setValue(DPN_BACKWARD);
            goto fail;
        }
        if( m->reserveShadowReceiver(channel, extra, 0) == false ) {
            DL_FUNCFAIL(1, "reserveShadowReceiver");
            setValue(DPN_BACKWARD);
            goto fail;
        }


//        makeAnswer();
//        setState(true);
//        send(this);
        break;
    }
    default:break;
    }
    return DPN_SUCCESS;

fail:
//    makeAnswer();
//    setState(false);
//    send(this);
    return DPN_FAIL;

}
DPN_Result DPN_Processor__request::procReceivingAnswer() {

    RequestType type = UNIT_REQUEST_TYPE.get();
    switch (type) {
    case REQUEST_NET_LOCAL_ENVIRONMENT:
        send(processor<DPN_PACKETTYPE__LOCAL_ENVIRONMENT>());
        break;
    case REQUEST_SHADOW_CONNECTION: {
        int port = UNIT_VALUE.get();
        bool state = UNIT_STATE.get();
        clientContext.client()->shadowConnectionServerAnswer(state, port);
        break;
    }
    case REQUEST_CHANNEL_RESERVING: {


//        int tr = UNIT_TRANSACTION.get();
        std::string shadowKey = UNIT_HASH.get();
        bool state = UNIT_STATE.get();
        std::string moduleName = UNIT_MODULE_NAME.get();
        auto module = clientContext.modules()->module(moduleName);
        if( module == nullptr ) {
            DL_ERROR(1, "No module with name: [%s]", moduleName.c_str());
            return DPN_FAIL;
        }

        if( state == false ) {

            int v = UNIT_VALUE.get();
            DL_WARNING(1, "DPN_ANSWER__CHANNEL_RESERVE: Server can't reserve special channel. shadow key: [%s] d: [%d]", shadowKey.c_str(), v);

            switch (v) {
            case DPN_FORWARD:
                module->unreserveShadowReceiver(shadowKey);
                break;
            case DPN_BACKWARD:
                module->unreserveShadowSender(shadowKey);
                break;
            default:
                DL_INFO(1, "DPN_ANSWER__CHANNEL_RESERVE: Undefined direction");
                break;
            }

            return DPN_FAIL;
        }

        DPN_Channel *channel = clientContext.client()->channel(shadowKey);
        if( channel == nullptr ) {
            DL_ERROR(1, "DPN_ANSWER__CHANNEL_RESERVE: No channel");
            setHash(shadowKey);
            setState(false);
            setValue(DPN_FORWARD);
            send(this);
        }
        if( module->reserveShadowSender(channel, 0) == false ) {
            DL_FUNCFAIL(1, "reserveFileSender");
            setHash(shadowKey);
            setState(false);
            setValue(DPN_FORWARD);
            send(this);
        }
        clientContext.core()->replanDirections();
        DL_INFO(1, "channel finded [%p] and reserve as sender", channel);
        break;
    }
    default:break;
    }
    return DPN_SUCCESS;
}
*/
//====================================================================================== DPN_Processor__reserve_shadow_channel:
/*
DPN_Result DPN_Processor__sync_channel::failureProcessing() {
//    auto tr = useTransaction();
//    if( tr == nullptr ) {
//        DL_BADPOINTER(1, "transaction");
//        return DPN_FAIL;
//    }
//    DL_INFO(1, "DO FAIL STEP");
//    tr->line<DPN_Processor__sync_channel>().doFailStep();
//    if( checkFlag(PF__GENERATOR) && tr->line<DPN_Processor__sync_channel>().step() == 0) return DPN_FAIL;
    return DPN_SUCCESS;
}
bool DPN_Processor__sync_channel::makeHostLine() {
    INFO_LOG;

//    auto tr = useTransaction();
//    if( tr == nullptr ) {
//        DL_BADPOINTER(1, "transaction");
//        return false;
//    }
//    tr->makeLine<DPN_Processor__sync_channel>();

//    tr->line<DPN_Processor__sync_channel>()
//            << &DPN_Processor__sync_channel::hostRequest
//            << &DPN_Processor__sync_channel::hostSync
//                                                    ;

//    tr->line<DPN_Processor__sync_channel>().setFailStep( &DPN_Processor__sync_channel::hostFault );
    return true;
}
bool DPN_Processor__sync_channel::makeServerLine() {
    INFO_LOG;

//    auto tr = useTransaction();
//    if( tr == nullptr ) {
//        DL_BADPOINTER(1, "transaction");
//        return false;
//    }
//    tr->makeLine<DPN_Processor__sync_channel>();

//    tr->line<DPN_Processor__sync_channel>()
//            << &DPN_Processor__sync_channel::serverSync
//                                                    ;

//    tr->line<DPN_Processor__sync_channel>().setFailStep( &DPN_Processor__sync_channel::serverFault );
    return true;
}
DPN_Result DPN_Processor__sync_channel::sendPrefix() {
    INFO_LOG;
//    return defaultSendStep<Transaction>();
}
DPN_Result DPN_Processor__sync_channel::receiveReaction() {
    INFO_LOG;
//    return defaultReceiveStep<Transaction>();
}
DPN_Result DPN_Processor__sync_channel::hostRequest() {

//    INFO_LOG;
//    if( pChannel == nullptr ) {
//        DL_BADPOINTER(1, "channel");
//        return DPN_FAIL;
//    }
////    auto tr = useTransaction<Transaction>();
////    if( tr == nullptr ) {
////        DL_BADPOINTER(1, "transaction");
////        return DPN_FAIL;
////    }

//    ChannelInterface i = pChannel->channelInterface();
//    if( i.badInterface() ) {
//        if( i.available() ) {
//            DL_ERROR(1, "bad interface");
//            return DPN_FAIL;
//        }
//        return DPN_REPEAT;
//    }
//    if( (pModule = i.get()->forward.module()) == nullptr ) {
//        DL_BADPOINTER(1, "module");
////        return DPN_FAIL;
//    }


////    tr->pChannel = pChannel;
////    tr->wExtraData = wExtraData;
////    tr->pModule = pModule;



//    UNIT_FORWARD_STATE = i.get()->forward.localState();
//    UNIT_BACKWARD_STATE = i.get()->backward.localState();
//    UNIT_SHADOW_KEY = pChannel->key();
//    if( pModule ) UNIT_MODULE_NAME = pModule->name();
//    UNIT_EXTRA_DATA = wExtraData;

//    DL_INFO(1, "extra: [%d] [%d]", wExtraData.size(), i.get()->forward.settings().size());
//    wExtraData.print();
//    DPN_ExpandableBuffer b = UNIT_EXTRA_DATA.get();
//    b.print();

    return DPN_SUCCESS;
}
DPN_Result DPN_Processor__sync_channel::serverSync() {
    INFO_LOG;
//    auto tr = useTransaction<Transaction>();
//    if( tr == nullptr ) {
//        DL_BADPOINTER(1, "transaction");
//        return DPN_FAIL;
//    }
    MonoChannelState fs = UNIT_FORWARD_STATE.get();
    MonoChannelState bs = UNIT_BACKWARD_STATE.get();
    std::swap(fs, bs); // reverse channels on remote side

    std::string shadowKey = UNIT_SHADOW_KEY.get();
    std::string moduleName = UNIT_MODULE_NAME.get();
    DPN_ExpandableBuffer extra = UNIT_EXTRA_DATA.get();


//    if( (pChannel = clientContext.client()->channel( shadowKey )) == nullptr ) {
//        DL_ERROR(1, "Can't find channel with key: [%s]", shadowKey.c_str());
//        return DPN_FAIL;
//    }
//    if( (pModule = clientContext.modules()->module( moduleName )) == nullptr ) {
//        DL_ERROR(1, "Can't find module with name: [%s]", moduleName.c_str());
//    }
//    ChannelInterface i = pChannel->channelInterface(true);
//    if( i.badInterface() ) {
//        if( i.available() ) {
//            DL_ERROR(1, "bad interface");
//            return DPN_FAIL;
//        }
//        return DPN_REPEAT;
//    }

    DL_INFO(1, "fs: [%d] bs: [%d] extra: [%d]", fs, bs, extra.size());
//    extra.print();

//    i.get()->forward.setRemoteState( fs );
//    i.get()->backward.setRemoteState( bs );


//    if( pModule ) {
//        if( pModule->channeling( pChannel, extra ) == false ) {
//            DL_FUNCFAIL(1, "channeling");
//            return DPN_FAIL;
//        }
//    }

    // Local states should be switched by <channeling> call.
    // If <channeling> no switch local states and they still MS__RAW:
    // host get it like server rejecting channeling

//    UNIT_FORWARD_STATE = i.get()->forward.localState();
//    UNIT_BACKWARD_STATE = i.get()->backward.localState();

    return DPN_SUCCESS;
}
DPN_Result DPN_Processor__sync_channel::hostSync() {
    INFO_LOG;
//    auto tr = useTransaction<Transaction>();
//    if( tr == nullptr ) {
//        DL_BADPOINTER(1, "transaction");
//        return DPN_FAIL;
//    }
//    if( tr->pChannel == nullptr ) {
//        DL_BADPOINTER(1, "channel" );
//        return DPN_FAIL;
//    }
//    ChannelInterface i = tr->pChannel->channelInterface();
//    if( i.badInterface() ) {
//        if( i.available() ) {
//            DL_ERROR(1, "bad interface");
//            return DPN_FAIL;
//        }
//        return DPN_REPEAT;
//    }
//    MonoChannelState fs = UNIT_FORWARD_STATE.get();
//    MonoChannelState bs = UNIT_BACKWARD_STATE.get();
//    std::swap(fs, bs); // reverse channels on remote side

//    DL_INFO(1, "fs: [%d] bs: [%d]", fs, bs);

//    i.get()->forward.setRemoteState( fs );
//    i.get()->backward.setRemoteState( bs );

    setFlag(PF__DIALOG_OVER);
    return DPN_SUCCESS;
}
DPN_Result DPN_Processor__sync_channel::hostFault() {
    INFO_LOG;
    return DPN_SUCCESS;
}
DPN_Result DPN_Processor__sync_channel::serverFault() {
    INFO_LOG;
    return DPN_SUCCESS;
}
//====================================================================================== DPN_Processor__net_environment:
DPN_Result DPN_Processor__local_environment::sendPrefix() {
//    return defaultSendStep();
}
DPN_Result DPN_Processor__local_environment::receiveReaction() {
//    return defaultReceiveStep();
}
DPN_Result DPN_Processor__local_environment::hostRequest() {
    return DPN_SUCCESS;
}
DPN_Result DPN_Processor__local_environment::serverProcess() {
//    DArray<PeerAddress> env = clientContext.core()->getEnvironment();
//    if( env.empty() ) {
//        DL_INFO(1, "Empty environment");
//        return DPN_FAIL;
//    }
//    UNIT_ENV = env;
//    return DPN_SUCCESS;
}
DPN_Result DPN_Processor__local_environment::hostCatch() {
    DArray<PeerAddress> env = UNIT_ENV.get();
    if( env.empty() ) {
        DL_INFO(1, "Empty environment");
        return DPN_FAIL;
    }
//    clientContext.client()->setEnvironment(env);
    return DPN_SUCCESS;
}
DPN_Result DPN_Processor__local_environment::hostFault() {
//    DL_WARNING(1, "Requesting local enivronment of [%s] fault", clientContext.client()->remoteAddress().name().c_str());
    return DPN_SUCCESS;
}
DPN_Result DPN_Processor__local_environment::serverFault() {
    return DPN_SUCCESS;
}
DPN_Result DPN_Processor__local_environment::failureProcessing() {
//    auto tr = useTransaction();
//    if( tr == nullptr ) {
//        DL_BADPOINTER(1, "transaction");
//        return DPN_FAIL;
//    }
//    DL_INFO(1, "DO FAIL STEP");
//    tr->line<DPN_Processor__local_environment>().doFailStep();
//    if( checkFlag(PF__GENERATOR) && tr->line<DPN_Processor__local_environment>().step() == 0) return DPN_FAIL;
    return DPN_SUCCESS;
}
bool DPN_Processor__local_environment::makeHostLine() {
    INFO_LOG;

//    auto tr = useTransaction();
//    if( tr == nullptr ) {
//        DL_BADPOINTER(1, "transaction");
//        return false;
//    }
//    tr->makeLine<DPN_Processor__local_environment>();

//    tr->line<DPN_Processor__local_environment>()

//            << &DPN_Processor__local_environment::hostRequest
//            << &DPN_Processor__local_environment::hostCatch
//                                                    ;

//    tr->line<DPN_Processor__local_environment>().setFailStep( &DPN_Processor__local_environment::hostFault );
    return true;
}
bool DPN_Processor__local_environment::makeServerLine() {
    INFO_LOG;

//    auto tr = useTransaction();
//    if( tr == nullptr ) {
//        DL_BADPOINTER(1, "transaction");
//        return false;
//    }
//    tr->makeLine<DPN_Processor__local_environment>();

//    tr->line<DPN_Processor__local_environment>()
//            << &DPN_Processor__local_environment::serverProcess
//                                                    ;

//    tr->line<DPN_Processor__local_environment>().setFailStep( &DPN_Processor__local_environment::serverFault );
    return true;
}

*/




