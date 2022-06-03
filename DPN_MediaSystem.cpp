#include "DPN_MediaSystem.h"



// 1. separate prepring
// 2. closing dffmpeg
// 3. set device from out
// 4. abstract master

DPN_MediaSystem::DPN_MediaSystem() : DPN_AbstractModule("Media") {

    DL_INFO(1, "Create Media System: [%p]", this);

    eGlobalInputPermission = DPN_MS__AUDIOVIDEO;
//    eInputState = DPN_MS__NOTYPE;
//    eOutputState = DPN_MS__NOTYPE;


    pVideoOutputStream = nullptr;
    pVideoInputStream = nullptr;
    pAudioOutputStream = nullptr;
    pAudioInputStream = nullptr;
    pCurrentClient = nullptr;
}
DPN_MediaSystem::~DPN_MediaSystem() {
//    if( pAudioPort ) pAudioPort->socket().stop();
//    if( pVideoPort ) pVideoPort->socket().stop();

//    iVideoOutput.blockDirection();
//    iVideoInput.blockDirection();
//    iAudioOutput.blockDirection();
//    iAudioInput.blockDirection();
}

void DPN_MediaSystem::clientDisconnected(const DPN_AbstractClient *client) {
    if( client == pCurrentClient ) {
        stop(DPN_MS__AUDIOVIDEO);
    }
}
void DPN_MediaSystem::stop() {
    stop(DPN_MS__AUDIOVIDEO);
}
void DPN_MediaSystem::setVideoDevice(const VideoEncoderContext &vec) {
    iDefaultVideoEncoderContext = vec;
}
void DPN_MediaSystem::setVideoOutput(const VideoDecoderContext &vdc) {
    iDefaultVideoDecoderContext = vdc;
}
void DPN_MediaSystem::setAudioDevice(const AudioEncoderContext &aec) {
    iDefaultAudioEncoderContext = aec;
}

bool DPN_MediaSystem::connect(int vport, int aport, const std::string &address) {


//    if( vport > 0 ) {
//        if( pVideoPort == nullptr ) {
//            DL_BADPOINTER(1, "pVideoPort");
//            return false;
//        }
//        PeerAddress a(vport, address);
//        if( pVideoPort->connect( a ) == false ) {
//            DL_FUNCFAIL(1, "(video) connect");
//            return false;
//        }

//        DL_INFO(1, " >>> Connect video port [%d] with address: [%s]", pVideoPort->socket().localPort(), a.name().c_str());
//    }

//    if( aport > 0 ) {
//        if( pAudioPort == nullptr ) {
//            DL_BADPOINTER(1, "pAudioPort");
//            return false;
//        }
//        PeerAddress a(aport, address);
//        if( pAudioPort->connect( a ) == false ) {
//            DL_FUNCFAIL(1, "(audio) connect");
//            return false;
//        }
//    }

    return true;
}
bool DPN_MediaSystem::stop(DPN_MediaType mediaType) {
    if( stop(mediaType, DPN_FORWARD) == false ) {
        DL_FUNCFAIL(1, "stop (DPN_FORWARD)");
    }
    if( stop(mediaType, DPN_BACKWARD) == false ) {
        DL_FUNCFAIL(1, "stop (DPN_BACKWARD)");
    }
    return true;
}
bool DPN_MediaSystem::stop(DPN_MediaType mediaType, DPN_DirectionType directionType) {

    // block and extract direction from thread
    // stop encoder
    // proc pointer to client
    // proc state


    DL_INFO(1, "STOP: type: [%d] direction: [%d]", mediaType, directionType);

    if( mediaType == DPN_MS__NOTYPE ) return false;

    DPN_THREAD_GUARD(iMutex);


    if( directionType == DPN_FORWARD ) {

        if( mediaType & DPN_MS__AUDIO ) {
            iAudioOutput.stop();
//            eOutputState = remT(eOutputState, DPN_MS__AUDIO);
        }
        if( mediaType & DPN_MS__VIDEO ) {
            iVideoOutput.stop();
//            eOutputState = remT(eOutputState, DPN_MS__VIDEO);
        }
    } else if( directionType == DPN_BACKWARD ){

        if( mediaType & DPN_MS__AUDIO ) {
            iAudioInput.stop();
//            eInputState = remT(eInputState, DPN_MS__AUDIO);
        }
        if( mediaType & DPN_MS__VIDEO ) {
            iVideoInput.stop();
//            eInputState = remT(eInputState, DPN_MS__VIDEO);
        }
    }

//    if(     (!pAudioSender || pAudioSender->isBlocked()) &&
//            (!pAudioReceiver || pAudioReceiver->isBlocked()) &&
//            (!pVideoSender || pVideoSender->isBlocked()) &&
//            (!pVideoReceiver || pVideoReceiver->isBlocked())
//      ) {
//        pCurrentClient = nullptr;
//    }

    return true;
}
bool DPN_MediaSystem::sendVideoFrame() {

//    if( pVideoEncoder == nullptr ) {
//        DL_BADPOINTER(1, "pVideoEncoder");
//        return false;
//    }
//    if( pVideoPort == nullptr ) {
//        DL_BADPOINTER(1, "pVideoPort");
//        return false;
//    }
//    if( pVideoEncoder->read() == false ) {
//        DL_FUNCFAIL(1, "video read");
//        return false;
//    }

//    if( pVideoEncoder->encode() == false ) {
//        DL_FUNCFAIL(1, "video encode");
//        return false;
//    }

//    if( fVideoOutput & O_UDP ) {
//        int sb = 0;
//        sb = pVideoPort->socket().send( pVideoEncoder->encodedData(), pVideoEncoder->encodedSize() );
//        DL_INFO(1, "video: written data: [%d] sent: [%d]", pVideoEncoder->encodedSize(), sb);
//    }
//    if( fVideoOutput & IO_MEM ) {
//        wVideoOutputRecordBuffer.append( pVideoEncoder->encodedData(), pVideoEncoder->encodedSize());
//    }
//    if( fAudioOutput & IO_FILE ) {
//        fwrite( pVideoEncoder->encodedData(), 1, pVideoEncoder->encodedSize(), pVideoOutputRecordFile);
//    }
//    if( fAudioOutput & IO_RENDER ) {
//        //...
//    }

//    pVideoEncoder->clearEncoded();

    return true;
}
bool DPN_MediaSystem::sendAudioFrame() {

//    if( pAudioEncoder == nullptr ) {
//        DL_BADPOINTER(1, "pAudioEncoder");
//        return false;
//    }
//    if( pAudioPort == nullptr ) {
//        DL_BADPOINTER(1, "pAudioPort");
//        return false;
//    }
//    if( pAudioEncoder->read() == false ) {
//        DL_FUNCFAIL(1, "audio read");
//        return false;
//    }
//    if( pAudioEncoder->encode() == false ) {
//        DL_FUNCFAIL(1, "audio encode");
//        return false;
//    }

//    if( fAudioOutput & O_UDP ) {
//        int sb = 0;
//        sb = pAudioPort->socket().send( pAudioEncoder->encodedData(), pAudioEncoder->encodedSize() );
//        DL_INFO(1, "audio: written data: [%d] sent: [%d]", pAudioEncoder->encodedSize(), sb);
//    }
//    if( fAudioOutput & IO_MEM ) {
//        wAudioOutputRecordBuffer.append( pAudioEncoder->encodedData(), pAudioEncoder->encodedSize());
//    }
//    if( fAudioOutput & IO_FILE ) {
//        fwrite( pAudioEncoder->encodedData(), 1, pAudioEncoder->encodedSize(), pAudioOutputRecordFile);
//    }
//    if( fAudioOutput & IO_RENDER ) {
//        //...
//    }
//    pAudioEncoder->clearEncoded();

    return true;
}
bool DPN_MediaSystem::receiveVideoFrame() {

//    if( pVideoPort == nullptr ) {
//        DL_BADPOINTER(1, "pVideoPort");
//        return false;
//    }
//    if( pVideoPort->socket().readable() ) {
//        int rb = 0;
//        rb = pVideoPort->socket().receive( wVideoBuffer.getData(), wVideoBuffer.size() );
//        if( rb == 0 ) return true;
//        if( rb < 0 ) {
//            DL_ERROR(1, "receive fault: [%d]", rb);
//            return false;
//        }

//        DL_INFO(1, "Read video packet: [%d]", rb);

//        if( fVideoInput & IO_MEM ) {
//            wVideoInputRecordBuffer.append( wVideoBuffer, rb );
//        }
//        if( fVideoInput & IO_FILE) {
//            fwrite( wVideoBuffer.getData(), 1, rb, pVideoInputRecordFile);
//        }
//        if( fVideoInput & IO_RENDER ) {
//            if( pRenderCallback ) {

//                if( pVideoDecoder->setEncodedData( wVideoBuffer.getData(), rb ) == false ) {
//                    DL_FUNCFAIL(1, "setVideoData");
//                    return false;
//                }
//                if( pVideoDecoder->decode() == false ) {
//                    DL_FUNCFAIL(1, "decode");
//                    return false;
//                }
//                pRenderCallback( pVideoDecoder->decodedData(), pRenderBase );
//            }
//        }
//    }

    return true;
}
bool DPN_MediaSystem::receiveAudioFrame() {

//    if( pAudioPort == nullptr ) {
//        DL_BADPOINTER(1, "pAudioPort");
//        return false;
//    }
//    if( pAudioPort->socket().readable() ) {
//        int rb = 0;
//        rb = pAudioPort->socket().receive( wAudioBuffer.getData(), wAudioBuffer.size() );
//        if( rb == 0 ) return true;
//        if( rb < 0 ) {
//            DL_ERROR(1, "receive fault: [%d]", rb);
//            return false;
//        }
//        DL_INFO(1, "Receive audio packet: [%d]", rb);
//    }

    return true;
}
bool DPN_MediaSystem::__create_stream(DPN_AbstractClient *client, MediaIOContext **stream, DPN_MediaType mediaType,
                                      IOBehaivor source, IOBehaivor target, MediaContextSet *c) {


    if( *stream == nullptr ) {
        *stream = new MediaIOContext;
    }
    if( (*stream)->processing() ) {
        DL_INFO(1, "Stream already run");
    } else {
        if( client == nullptr ) {
            DL_BADPOINTER(1, "client");
            return false;
        }

        (*stream)->setAudioEncoder( iDefaultAudioEncoderContext );
        (*stream)->setAudioDecoder( iDefaultAudioDecoderContext );
        (*stream)->setVideoEncoder( iDefaultVideoEncoderContext );
        (*stream)->setVideoDecoder( iDefaultVideoDecoderContext );

        if( c ) {
            if( mediaType == DPN_MS__AUDIO ) if( isValidMediaContext( c->aec ) ) (*stream)->setAudioEncoder( c->aec );
            if( mediaType == DPN_MS__AUDIO ) if( isValidMediaContext( c->adc ) ) (*stream)->setAudioDecoder( c->adc );
            if( mediaType == DPN_MS__VIDEO ) if( isValidMediaContext( c->vec ) ) (*stream)->setVideoEncoder( c->vec );
            if( mediaType == DPN_MS__VIDEO ) if( isValidMediaContext( c->vdc ) ) (*stream)->setVideoDecoder( c->vdc );
        }
        (*stream)->clientContext = client->context();
        if( (*stream)->init(mediaType, source, target) == false ) {
            DL_FUNCFAIL(1, "init");
            return false;
        }

//        DL_INFO(1, "stream: [%p] client context: [%p] core: [%p]", *stream, client->context().data(), client->context().core());
//        core()->addGlobalDirection(*stream);
    }

    return true;
}
bool DPN_MediaSystem::__run_stream(MediaIOContext **stream) {

    if( !stream ) return false;

    if( *stream == nullptr ) {
        DL_BADPOINTER(1, "*stream");
        return false;
    }
    if( (*stream)->processing() ) {
        DL_ERROR(1, "Audio output stream already run");
        return false;
    }
    core()->addGlobalDirection(*stream);
    return true;
}
bool DPN_MediaSystem::__prepareVideoSender() {

//    if( pVideoEncoder == nullptr ) pVideoEncoder = createMediaMaster();
//    if( !pVideoEncoder->isReady() ) {

//        if( isValidMediaContext( iVideoEncoderContext ) == false ) {
//            DL_ERROR(1, "No video encoder context");
//            return false;
//        }
//        if( pVideoEncoder->initVideoEncoder( iVideoEncoderContext ) == false ) {
//            DL_FUNCFAIL(1, "initVideoEncoder: name: [%s] codec: [%d] format: [%s] opener: [%s]",
//                        iVideoEncoderContext.name.c_str(), iVideoEncoderContext.codec, iVideoEncoderContext.format.c_str(),
//                        iVideoEncoderContext.opener.c_str());

//            return false;
//        }
//    }

    return true;
}
bool DPN_MediaSystem::__prepareVideoReceiver() {

//    if( pVideoDecoder == nullptr ) pVideoDecoder = createMediaMaster();
//    if( !pVideoDecoder->isReady() ) {

//        if( isValidMediaContext( iVideoDecoderContext ) == false ) {
//            DL_ERROR(1, "No video decoder context");
//            return false;
//        }
//        if( pVideoDecoder->initVideoDecoder( iVideoDecoderContext ) == false ) { //AV_PIX_FMT_RGB24
//            DL_FUNCFAIL(1, "initVideoDecoder");
//            return false;
//        }
//    }

//    wVideoBuffer.reserve( 10 * 1024 * 1024 );
    return true;
}
bool DPN_MediaSystem::__prepareAudioSender() {

//    if( pAudioEncoder == nullptr ) pAudioEncoder = createMediaMaster();
//    if( !pAudioEncoder->isReady() ) {

//        if( isValidMediaContext( iAudioEncoderContext ) == false ) {
//            DL_ERROR(1, "No audio encoder context");
//            return false;
//        }
////        if( pAudioEncoder->initAudioEncoder(AV_CODEC_ID_MP2, "dshow", "audio=Microphone (High Definition Audio Device)") == false ) {
//        if( pAudioEncoder->initAudioEncoder( iAudioEncoderContext ) == false ) {
//            DL_FUNCFAIL(1, "initVideoEncoder");
//            return false;
//        }
//    }



    return true;
}
bool DPN_MediaSystem::__prepareAudioReceiver() {

//    if( pAudioDecoder == nullptr ) pAudioDecoder = createMediaMaster();
//    if( !pAudioDecoder->isReady() ) {
//        if( pAudioDecoder->initAudioDecoder( iAudioDecoderContext ) == false ) {
//            DL_FUNCFAIL(1, "initAudioDecoder");
//            return false;
//        }
//    }


//    wAudioBuffer.reserve( 10 * 1024 * 1024 );

    return true;
}
bool DPN_MediaSystem::isAvailable(DPN_AbstractClient *client, DPN_MediaType mediaType, DPN_DirectionType directionType) {

    if( client == nullptr || (pCurrentClient && pCurrentClient != client) ) return false;
    if( mediaType == DPN_MS__NOTYPE ) return true;

    if( directionType == DPN_FORWARD ) {
        if( (mediaType & DPN_MS__AUDIO) && pAudioOutputStream && pAudioOutputStream->processing() ) return false;
        if( (mediaType & DPN_MS__VIDEO) && pVideoOutputStream && pVideoOutputStream->processing() ) return false;
    } else {
        if( (mediaType & DPN_MS__AUDIO) && pAudioInputStream && pAudioInputStream->processing() ) return false;
        if( (mediaType & DPN_MS__VIDEO) && pVideoInputStream && pVideoInputStream->processing() ) return false;

        if( !checkPermission(eGlobalInputPermission, mediaType) ) return false;
    }
    return true;
}


bool DPN_MediaSystem::checkPermission(DPN_MediaType permission, DPN_MediaType type) {
    return (permission & type) == type;
}

bool DPN_MediaSystem::receiveStream(DPN_AbstractClient *source, DPN_MediaType mediaType) {

    DPN_THREAD_GUARD(iMutex);


    if( mediaType == DPN_MS__NOTYPE ) {
        DL_BADVALUE(1, "media type");
        return false;
    }
    if( pCurrentClient == nullptr ) {
        pCurrentClient = source;
    } else if( pCurrentClient != source ) {
        DL_ERROR(1, "Current client already set. current: [%p] new: [%p]", pCurrentClient, source);
        return false;
    }

//    if( mediaType & DPN_MS__VIDEO ) {

//        if( pVideoReceiver == nullptr ) {
//            pVideoReceiver = new DPN_VideoReceiveDirection;
//            pVideoReceiver->setMediaSystem(this);
//        }
//        if( pVideoReceiver->processing() ) {
//            DL_WARNING(1, "Video receive direction already in thread");
//        } else {
//            core()->addGlobalDirection(pVideoReceiver);
//        }
//    }
//    if( mediaType & DPN_MS__AUDIO ) {

//        if( pAudioReceiver == nullptr ) {
//            pAudioReceiver = new DPN_AudioReceiveDirection;
//            pAudioReceiver->setMediaSystem(this);
//        }
//        if( pAudioReceiver->processing() ) {
//            DL_WARNING(1, "Audio receive direction already in thread");
//        } else {
//            core()->addGlobalDirection(pAudioReceiver);
//        }
//    }
//    eInputState = addT(eInputState, mediaType);
    return true;
}


bool DPN_MediaSystem::createStream(DPN_AbstractClient *client, DPN_MediaType mediaType, DPN_DirectionType directionType, MediaContextSet *c) {


    if( pCurrentClient && pCurrentClient != client ) {
        DL_ERROR(1, "Different client: [%p] current: [%p]", client, pCurrentClient);
        return false;
    }
    if( mediaType == DPN_MS__NOTYPE ) {
        DL_BADVALUE(1, "No media type");
        return false;
    }
    if( directionType == DPN_FORWARD ) {
        if( mediaType & DPN_MS__AUDIO ) {
            return __create_stream(client, &pAudioOutputStream, DPN_MS__AUDIO, IO_DEVICE, IO_UDP, c);
        }
        if( mediaType & DPN_MS__VIDEO ) {
            return __create_stream(client, &pVideoOutputStream, DPN_MS__VIDEO, IO_DEVICE, IO_UDP, c);
        }
    } else {
        if( mediaType & DPN_MS__AUDIO ) {
            return __create_stream(client, &pAudioInputStream, DPN_MS__AUDIO, IO_UDP, IO_DEVICE, c);
        }
        if( mediaType & DPN_MS__VIDEO ) {
            return __create_stream(client, &pVideoInputStream, DPN_MS__VIDEO, IO_UDP, IO_DEVICE, c);
        }
    }

    return false;
}
bool DPN_MediaSystem::runStream(DPN_AbstractClient *client, DPN_MediaType mediaType, DPN_DirectionType directionType, int remotePort) {

    if( pCurrentClient && pCurrentClient != client ) {
        DL_ERROR(1, "Different client: [%p] current: [%p]", client, pCurrentClient);
        return false;
    }
    if( mediaType == DPN_MS__NOTYPE ) {
        DL_BADVALUE(1, "No media type");
        return false;
    }
    if( !DXT_CHECK_PORT(remotePort) ) {
        DL_BADVALUE(1, "remotePort: [%d]", remotePort);
        return false;
    }
    if( directionType == DPN_FORWARD ) {
        if( mediaType & DPN_MS__AUDIO ) return __run_stream(&pAudioOutputStream);
        if( mediaType & DPN_MS__VIDEO ) return __run_stream(&pVideoOutputStream);
    } else {
        if( mediaType & DPN_MS__AUDIO ) return __run_stream(&pAudioInputStream);
        if( mediaType & DPN_MS__VIDEO ) return __run_stream(&pVideoInputStream);
    }
    return false;
}
int DPN_MediaSystem::streamPort(DPN_MediaType mediaType, DPN_DirectionType directionType) {
    if( mediaType == DPN_MS__NOTYPE || mediaType == DPN_MS__AUDIOVIDEO ) return -1;
    if( directionType == DPN_FORWARD ) {
        if( mediaType == DPN_MS__AUDIO && pAudioOutputStream ) return pAudioOutputStream->port();
        if( mediaType == DPN_MS__VIDEO && pVideoOutputStream ) return pVideoOutputStream->port();
    } else {
        if( mediaType == DPN_MS__AUDIO && pAudioInputStream ) return pAudioInputStream->port();
        if( mediaType == DPN_MS__VIDEO && pVideoInputStream ) return pVideoInputStream->port();
    }
    return -1;
}
bool DPN_MediaSystem::sendStream(DPN_AbstractClient *target, DPN_MediaType mediaType) {

    DPN_THREAD_GUARD(iMutex);

    if( mediaType == DPN_MS__NOTYPE ) {
        DL_BADVALUE(1, "media type");
        return false;
    }
    if( pCurrentClient == nullptr ) {
        pCurrentClient = target;
    } else if( pCurrentClient != target ) {
        DL_ERROR(1, "Current client already set. current: [%p] new: [%p]", pCurrentClient, target);
        return false;
    }
//    if( mediaType & DPN_MS__AUDIO ) {

//        if( pAudioSender == nullptr ) {
//            pAudioSender = new DPN_AudioSendDirection;
//            pAudioSender->setMediaSystem(this);
//        }
//        if( pAudioSender->processing() ) {
//            DL_WARNING(1, "Video send direction already in thread");
//        } else {
//            core()->addGlobalDirection(pAudioSender);
//        }
//    }
//    if( mediaType & DPN_MS__VIDEO ) {

//        DL_INFO(1, "pVideoSender before preparing: [%p]", pVideoSender);

//        if( pVideoSender == nullptr ) {
//            pVideoSender = new DPN_VideoSendDirection;
//            pVideoSender->setMediaSystem(this);
//        }
//        if( pVideoSender->processing() ) {
//            DL_WARNING(1, "Audio send direction already in thread");
//        } else {
//            DL_INFO(1, "put pVideoSender [%p] to thread", pVideoSender);
//            core()->addGlobalDirection(pVideoSender);
//        }
//        addIOBehaivor(O_UDP, fVideoOutput);
//    }

//    eOutputState = addT(eOutputState, mediaType);
    return true;
}
//=============================================================================== DPN_MediaDirection
/*
DPN_MediaDirection::DPN_MediaDirection() {
    pMediaSystem = nullptr;
    pMaster = nullptr;
}
bool DPN_MediaDirection::close() {
    if( pMaster ) pMaster->stop();
    return true;
}
bool DPN_MediaDirection::prepareVideoSender() {
    if( pMediaSystem == nullptr ) {
        DL_BADPOINTER(1, "Media System");
        return false;
    }
    bool r;
//    if( (r = pMediaSystem->__prepareVideoSender()) ) {
//        pMaster = pMediaSystem->iVideoOutput.pMaster;
//    }
    return r;
}
bool DPN_MediaDirection::prepareAudioSender() {
    if( pMediaSystem == nullptr ) {
        DL_BADPOINTER(1, "Media System");
        return false;
    }
    return pMediaSystem->__prepareVideoSender();
}
bool DPN_MediaDirection::prepareVideoReceiver() {
    if( pMediaSystem == nullptr ) {
        DL_BADPOINTER(1, "Media System");
        return false;
    }
    return pMediaSystem->__prepareVideoReceiver();
}
bool DPN_MediaDirection::prepareAudioReceiver() {
    if( pMediaSystem == nullptr ) {
        DL_BADPOINTER(1, "Media System");
        return false;
    }
    return pMediaSystem->__prepareAudioReceiver();
}
//=============================================================================== DPN_VideoSendDirection
bool DPN_VideoSendDirection::threadInjection() {
    return prepareVideoSender();
}
bool DPN_VideoSendDirection::proc() {

    if( pMediaSystem == nullptr ) {
        DL_BADPOINTER(1, "media system");
        return false;
    }
    if( pMediaSystem->sendVideoFrame() == false ) {
        DL_FUNCFAIL(1, "sendVideoFrame");
        return false;
    }

    return true;
}
//=============================================================================== DPN_VideoReceiveDirection
bool DPN_VideoReceiveDirection::threadInjection() {
    return prepareVideoReceiver();
}
bool DPN_VideoReceiveDirection::proc() {

    if( pMediaSystem == nullptr ) {
        DL_BADPOINTER(1, "media system");
        return false;
    }
    if( pMediaSystem->receiveVideoFrame() == false ) {
        DL_FUNCFAIL(1, "receiveVideoFrame");
        return false;
    }

    return true;
}
//=============================================================================== DPN_AudioSendDirection
bool DPN_AudioSendDirection::threadInjection() {
    return prepareAudioSender();
}
bool DPN_AudioSendDirection::proc() {

    if( pMediaSystem == nullptr ) {
        DL_BADPOINTER(1, "media system");
        return false;
    }
    if( pMediaSystem->sendAudioFrame() == false ) {
        DL_FUNCFAIL(1, "sendAudioFrame");
        return false;
    }

    return true;
}
//=============================================================================== DPN_AudioReceiveDirection
bool DPN_AudioReceiveDirection::threadInjection() {
    return prepareAudioReceiver();
}
bool DPN_AudioReceiveDirection::proc() {

    if( pMediaSystem == nullptr ) {
        DL_BADPOINTER(1, "media system");
        return false;
    }
    if( pMediaSystem->receiveAudioFrame() == false ) {
        DL_FUNCFAIL(1, "receiveAudioFrame");
        return false;
    }

    return true;
}
*/
//=============================================================================== MediaIOContext
bool MediaIOContext::threadInjection() {
    return true;
}
bool MediaIOContext::proc() {
    if( read() == false ) {
        DL_FUNCFAIL(1, "read");
        return false;
    }
    if( write() == false ) {
        DL_FUNCFAIL(1, "write");
        return false;
    }
    return true;
}
bool MediaIOContext::close() {
    return true;
}
MediaIOContext::MediaIOContext() {
    __zero();
}
bool MediaIOContext::init(DPN_MediaType mediaType, IOBehaivor source, IOBehaivor target) {

    if( (mediaType != DPN_MS__AUDIO && mediaType != DPN_MS__VIDEO)
            || source == IO_NO || target == IO_NO ) {
        DL_BADVALUE(1, "Input args: media type: [%d] source: [%d] target: [%d]", mediaType, source, target);
        return false;
    }
    switch (source) {
    case IO_MEM:
    case IO_FILE:
    case IO_DEVICE:
        if( __init_device(IO_SOURCE, mediaType) == false ) {
            DL_FUNCFAIL(1, "__init_device (source)");
            return false;
        }
        break;
    case IO_UDP:
        if( __init_port(IO_SOURCE, 55770) == false ) {
            DL_FUNCFAIL(1, "__init_port (source)");
            return false;
        }
    case IO_CALLBACK: break;
    case IO_NO:
    default:
        DL_BADVALUE(1, "source: [%d]", source);
        return false;
    }

    if( target & IO_DEVICE ) {
        if( __init_device(IO_TARGET, mediaType) == false ) {
            DL_FUNCFAIL(1, "__init_device (target)");
            return false;
        }
    }
    if( target & IO_UDP ) {
        if( __init_port(IO_TARGET, 55771) == false ) {
            DL_FUNCFAIL(1, "__init_port (target) ");
            return false;
        }
    }
    if( target & IO_MEM ) {
        wTargetBuffer.reserve(1024 * 1024);
    }
    if( target & IO_FILE ) {
        // open file
    }
    if( target & IO_CALLBACK ) {

    }

    return true;

}
bool MediaIOContext::stop() {

//    if( pDirection == nullptr ) {
//        DL_BADPOINTER(1, "direction");
//        return false;
//    }
//    if( pDirection->processing() ) pDirection->block();
//    if( pDirection->processing() == false ) {
//        if( pMaster == nullptr ) {
//            DL_BADPOINTER(1, "master");
//            return false;
//        }
//        DL_INFO(1, "stop master: [%p]", pMaster);
//        pMaster->stop();
//    }
    return true;
}
bool MediaIOContext::read() {

    switch ( fSourceType ) {
    case IO_FILE: return __read_file();
    case IO_DEVICE: return __read_device();
    case IO_UDP: return __read_port();
    case IO_MEM: return __read_mem();
    case IO_CALLBACK: return __read_callback();
    case IO_NO:
        DL_ERROR(1, "No source type");
        return false;
    default:
        DL_ERROR(1, "Bad source type: [%d]", fSourceType);
        return false;
    }

}
bool MediaIOContext::write() {
    if( iSourceSize == 0 || pSourceData == nullptr ) {
        DL_BADVALUE(1, "size: [%d] data: [%p]", iSourceSize, pSourceData);
        return false;
    }

    if( fTargetType & IO_UDP ) __write_port();
    if( fTargetType & IO_MEM ) __write_mem();
    if( fTargetType & IO_FILE ) __write_file();
    if( fTargetType & IO_CALLBACK || fTargetType & IO_DEVICE ) __write_callback();
}
int MediaIOContext::port() const {
    if( pPort ) return pPort->socket().localPort();
    return -1;
}
bool MediaIOContext::__init_device(IOType iot, DPN_MediaType mediaType) {
    if( pMaster == nullptr ) {
        if( (pMaster = createMediaMaster()) == nullptr ) {
            DL_BADALLOC(1, "Media master");
            return false;
        }
    }
    if( pMaster->isReady() ) return true;

    if( iot == IO_TARGET ) {

        if( mediaType == DPN_MS__AUDIO ) {
            if( isValidMediaContext( iAudioDecoderContext ) ) return pMaster->initAudioDecoder( iAudioDecoderContext );
            DL_ERROR(1, "No audio decoder context");
            return false;
        }
        if( mediaType == DPN_MS__VIDEO ) {
            if( isValidMediaContext( iVideoDecoderContext ) ) return pMaster->initVideoDecoder( iVideoDecoderContext );
            DL_ERROR(1, "No video decoder context");
            return false;
        }
    } else {
        if( mediaType == DPN_MS__AUDIO ) {
            if( isValidMediaContext( iAudioEncoderContext ) ) return pMaster->initAudioEncoder( iAudioEncoderContext );
            DL_ERROR(1, "No audio encoder context");
            return false;
        }
        if( mediaType == DPN_MS__VIDEO ) {
            if( isValidMediaContext( iVideoEncoderContext ) ) return pMaster->initVideoEncoder( iVideoEncoderContext );
            DL_ERROR(1, "No video encoder context");
            return false;
        }
    }

    DL_BADVALUE(1, "Media type: [%d]", mediaType);
    return false;
}
bool MediaIOContext::__init_port(IOType iot, int preferredPort) {

    DPN_UDPPort **__port = &pPort;

    if( *__port ) return true;
    auto core = clientContext.core();
    if( core == nullptr ) {
        DL_BADPOINTER(1, "core");
        return false;
    }
    if( (*__port = core->openUDPPort(preferredPort)) == nullptr ) {
        DL_FUNCFAIL(1, "openUDPPort: port: [%d] iot: [%d]", preferredPort, iot);
        return false;
    }

    return true;
}
bool MediaIOContext::__read_mem() { return __read_device(); }
bool MediaIOContext::__read_file() { return __read_device(); }
bool MediaIOContext::__read_device() {
    if( pMaster == nullptr ) {
        DL_BADPOINTER(1, "pMaster");
        return false;
    }
    if( pMaster->read() == false ) {
        DL_FUNCFAIL(1, "read");
        return false;
    }
    if( pMaster->encode() == false ) {
        DL_FUNCFAIL(1, "encode");
        return false;
    }
    pSourceData = pMaster->encodedData();
    iSourceSize = pMaster->encodedSize();
    return true;
}
bool MediaIOContext::__read_port() {
    if( pPort == nullptr ) {
        DL_BADPOINTER(1, "port");
        return false;
    }
    if( pPort->socket().readable() == false ) return true;
    iSourceSize = pPort->socket().receive( wSourceBuffer.getData(), wSourceBuffer.size() );
    if( iSourceSize == 0 ) return true;
    if( iSourceSize < 0 ) {
        DL_ERROR(1, "receive error: [%d]", iSourceSize);
        return false;
    }
    pSourceData = wSourceBuffer.getData();
    return true;
}
bool MediaIOContext::__read_callback() {
    if( pSourceCallback == nullptr ) {
        DL_BADPOINTER(1, "source callback");
        return false;
    }
    if( pSourceCallback(pSourceBaseOpaque, wSourceBuffer, iSourceSize) == false ) {
        DL_FUNCFAIL(1, "pSourceCallback");
        return false;
    }
    pSourceData = wSourceBuffer.getData();

    return true;
}
bool MediaIOContext::__write_port() {
    int sb;
    sb = pPort->socket().send( pSourceData, iSourceSize );
    if( sb < 1 ) {
        DL_ERROR(1, "send error: [%d]", sb);
        return false;
    }
    return true;
}
bool MediaIOContext::__write_mem() {
    wTargetBuffer.append( pSourceData, iSourceSize );
    return true;
}
bool MediaIOContext::__write_file() {
    if( pTargetFile == nullptr ) {
        DL_BADPOINTER(1, "file");
        return false;
    }
    fwrite( pSourceData, 1, iSourceSize, pTargetFile );
    return true;
}
bool MediaIOContext::__write_callback() {
    if( pTargetCallback == nullptr ) {
        DL_BADPOINTER(1, "target callback");
        return false;
    }
    if( fTargetType & IO_DEVICE ) {
        pMaster->setEncodedData(pSourceData, iSourceSize);
        pMaster->decode();
        pSourceData = pMaster->decodedData();
        iSourceSize = 0;
    }
    if( pTargetCallback(pTargetBaseOpaque, pSourceData, iSourceSize) == false ) {
        DL_FUNCFAIL(1, "pTargetCallback");
        return false;
    }

    return true;
}
void MediaIOContext::__zero() {
    pSourceBaseOpaque = nullptr;
    pSourceCallback = nullptr;
    pTargetBaseOpaque = nullptr;
    pTargetCallback = nullptr;

    pTargetFile = nullptr;
    pSourceData = nullptr;
    iSourceSize = 0;
    fSourceType = IO_NO;
    fTargetType = IO_NO;

    pMaster = nullptr;

    pPort = nullptr;
}

