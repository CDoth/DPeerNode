#include "DPN_Processors.h"

#include <DProfiler.h>

//====================================================================================== DPN_Processor__text_message:
DPN_Result DPN_Processor__text_message::receiveReaction() {

    INFO_LOG;

    std::string message = UNIT_MESSAGE.get();
    std::cout << " >>>>> receive message: [" << message << "]"
              << " thread: " << std::this_thread::get_id()
              << std::endl;

    return DPN_SUCCESS;
}
//====================================================================================== DPN_Processor__ping:
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
        threadContext.send(this);

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
//====================================================================================== DPN_Processor__request:
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
//====================================================================================== DPN_Processor__make_shadow_connection:
DPN_Result DPN_Processor__make_shadow_connection::sendPrefix() {
    return DPN_SUCCESS;
}
DPN_Result DPN_Processor__make_shadow_connection::receiveReaction() {

    PeerAddress s = UNIT_SOURCE.get();
    PeerAddress t = UNIT_TARGET.get();
    std::string key = UNIT_KEY.get();


    DL_INFO(1, "Try register shadow");
    clientContext.client()->registerShadow(s, t, key);


    return DPN_SUCCESS;
}
//====================================================================================== DPN_Processor__reserve_shadow_channel:
/*
DPN_Result DPN_Processor__reserve_shadow_channel::receiveReaction() {

    std::string moduleName = UNIT_MODULE_NAME.get();
    std::string shadowKey = UNIT_SHADOW_KEY.get();
    PeerAddress source = UNIT_SOURCE.get();
    PeerAddress target = UNIT_TARGET.get();


    DPN_Channel *channel = clientContext.client()->channel(shadowKey);
    DPN_AbstractModule *m = clientContext.modules()->module(moduleName);

    if( m == nullptr ) {
        DL_ERROR(1, "Can't find module with name: [%s]", moduleName.c_str());
        return DPN_FAIL;
    }

    auto proc = processor<DPN_PACKETTYPE__REQUEST>();
    proc->setType(REQUEST_CHANNEL_RESERVING);
    proc->makeAnswer();
//    proc->setHash(shadowKey);


    if( channel == nullptr ) {
        DL_ERROR(1, "No channel");
        goto fail;
    }
    if( channel->local() != source || channel->remote() != target ) {

        DL_ERROR(1, "Bad stats: channel: local: [%s] remote: [%s] receive: source: [%s] target: [%s]",
                 channel->local().name().c_str(),
                 channel->remote().name().c_str(),
                 source.name().c_str(),
                 target.name().c_str()
                 );
        goto fail;
    }
    if( m->reserveShadowReceiver(channel) == false ) {
        DL_FUNCFAIL(1, "reserveShadowReceiver");
        goto fail;
    }
    ///    clientContext.core()->replanDirections();

    proc->setState(true);
    send(proc);
    return DPN_SUCCESS;
fail:
    proc->setState(false);
    proc->setValue(DPN_BACKWARD);
    send(proc);
    return DPN_FAIL;
}
*/
//====================================================================================== DPN_Processor__net_environment:
DPN_Result DPN_Processor__net_environment::sendPrefix() {

    DArray<PeerAddress> env = clientContext.core()->getEnvironment();
    if( env.empty() ) {
        DL_INFO(1, "Empty environment");
        return DPN_FAIL;
    }
    UNIT_ENV = env;
    return DPN_SUCCESS;
}
DPN_Result DPN_Processor__net_environment::receiveReaction() {

    DArray<PeerAddress> env = UNIT_ENV.get();
    if( env.empty() ) {
        DL_INFO(1, "Empty environment");
        return DPN_FAIL;
    }
    clientContext.client()->setEnvironment(env);
    return DPN_SUCCESS;
}








