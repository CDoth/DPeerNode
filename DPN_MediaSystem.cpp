//#include "DPN_MediaSystem.h"

//#include <DDirReader.h>
//#include <DProfiler.h>

// 1. separate prepring
// 2. closing dffmpeg
// 3. set device from out
// 4. abstract master

/*
using namespace DPeerNodeSpace;

DPN_MediaSystem::DPN_MediaSystem(const std::string &name) : DPN_AbstractModule(name) {

    DL_INFO(1, "Create Media System: [%p]", this);

}
DPN_MediaSystem::~DPN_MediaSystem() {
}

bool DPN_MediaSystem::startStream1(__channel_mono_interface &channel, DPN_MediaType mt) {


    // 0. get interface
    // 1. Local binding: bind channel with direction
    // 2. Local activate: send active state
    // 3. Remote binding: receive active state acception
    // 4. Remote activate: receive remote active state


    if( channel.badInterface() ) {
        DL_ERROR(1, "Bad channel mono interface");
        return false;
    }
//    if( channel.direction() != DPN_FORWARD ) {
//        DL_WARNING(1, "Backward direction");
//        return false;
//    }
//    DPN_MediaStreamSettings settings(mt);
//    channel.reserve( this, settings.parse() );


//    DPN_IO::IOPropagationScheme *stream = new DPN_IO::IOPropagationScheme;

    // integrate channel to scheme
//    DPN_MediaCore *mc = new DPN_MediaCore;
//    mc->setContext( iDefaultSettings );
//    MEDIA_IO::reader *reader = new MEDIA_IO::reader(mc, mt);
//    MEDIA_IO::encoder *encoder = new MEDIA_IO::encoder(mc, mt);

//    stream->setEntry( reader );
//    reader->connect(  encoder );
//    encoder->connect( channel.io() );

    aLockedChannels.append( channel );
    return true;
}
bool DPN_MediaSystem::acceptIncoming() {
//    if( aIncomingChannels.empty() ) {
//        DL_ERROR(1, "No incoming channels");
//        return false;
//    }
//    IO__CHANNEL *channel = aIncomingChannels[0];
//    ChannelInterface i = channel->channelInterface();
//    DPN_IO::IOPropagationScheme *stream = new DPN_IO::IOPropagationScheme;
//    DPN_MediaCore *mc = new DPN_MediaCore;
//    mc->setContext( iDefaultSettings );

//    i.get()->backward.reserve2( stream, this );
//    channel->setNodeFlag( DPN_IO::IOContext::NODE__IMPORTANT);
//    stream->setEntry( channel );


//    if( channel->syncMe() == false ) {
//        DL_FUNCFAIL(1, "syncMe");
//        return false;
//    }

//    DL_INFO(1, "stream: [%p] channel: [%p]", stream, channel);
//    core()->replanDirections();

    return true;
}

void DPN_MediaSystem::setVideoDevice(const VideoEncoderContext &vec) {
    iDefaultSettings.iVideoEncoderContext = vec;
}
void DPN_MediaSystem::setVideoOutput(const VideoDecoderContext &vdc) {
    iDefaultSettings.iVideoDecoderContext = vdc;
}
void DPN_MediaSystem::setAudioDevice(const AudioEncoderContext &aec) {

    iDefaultSettings.iAudioEncoderContext = aec;
}
*/

/*
bool DPN_MediaSystem::setStreamTargetFile(DPN_MediaType mt, DPN_DirectionType dt, const std::string &path, const std::string &name) {

    if( dt == DPN_FORWARD ) {
        if( mt & DPN_MS__VIDEO && pVideoOutputStream ) pVideoOutputStream->setFile(path, name);
        if( mt & DPN_MS__AUDIO && pAudioOutputStream ) pAudioOutputStream->setFile(path, name);
    } else {
        if( mt & DPN_MS__VIDEO && pVideoInputStream ) pVideoInputStream->setFile(path, name);
        if( mt & DPN_MS__AUDIO && pAudioInputStream ) pAudioInputStream->setFile(path, name);
    }
    return true;
}
bool DPN_MediaSystem::__inner_create_stream(DPN_AbstractClient *client, MediaIOContext **stream, DPN_MediaType mediaType,
                                      IOBehaivor source, IOBehaivor target, MediaContextSet *c) {


//    if( *stream == nullptr ) {
//        *stream = new MediaIOContext;
//    }
//    if( (*stream)->processing() ) {
//        DL_INFO(1, "Stream already run");
//    } else {
//        if( client == nullptr ) {
//            DL_BADPOINTER(1, "client");
//            return false;
//        }

//        (*stream)->setAudioEncoder( iDefaultAudioEncoderContext );
//        (*stream)->setAudioDecoder( iDefaultAudioDecoderContext );
//        (*stream)->setVideoEncoder( iDefaultVideoEncoderContext );
//        (*stream)->setVideoDecoder( iDefaultVideoDecoderContext );

//        if( c ) {
//            if( mediaType == DPN_MS__AUDIO ) if( isValidMediaContext( c->aec ) ) (*stream)->setAudioEncoder( c->aec );
//            if( mediaType == DPN_MS__AUDIO ) if( isValidMediaContext( c->adc ) ) (*stream)->setAudioDecoder( c->adc );
//            if( mediaType == DPN_MS__VIDEO ) if( isValidMediaContext( c->vec ) ) (*stream)->setVideoEncoder( c->vec );
//            if( mediaType == DPN_MS__VIDEO ) if( isValidMediaContext( c->vdc ) ) (*stream)->setVideoDecoder( c->vdc );
//        }
//        (*stream)->clientContext = client->context();
//        if( (*stream)->init(mediaType, source, target) == false ) {
//            DL_FUNCFAIL(1, "init");
//            return false;
//        }

//    }

    return true;
}
bool DPN_MediaSystem::__inner_create_abs_stream(MediaIOContext **stream, DPN_MediaType mt, IOBehaivor source, IOBehaivor target) {
//    if( *stream == nullptr ) {
//        *stream = new MediaIOContext;
//    }
//    if( (*stream)->processing() ) {
//        DL_INFO(1, "Stream already run");
//    } else {
//        (*stream)->setAudioEncoder( iDefaultAudioEncoderContext );
//        (*stream)->setAudioDecoder( iDefaultAudioDecoderContext );
//        (*stream)->setVideoEncoder( iDefaultVideoEncoderContext );
//        (*stream)->setVideoDecoder( iDefaultVideoDecoderContext );
//        if( (*stream)->init(mt, source, target) == false ) {
//            DL_FUNCFAIL(1, "init");
//            return false;
//        }
//    }
    return true;
}
bool DPN_MediaSystem::__init_stream(MediaIOContext *stream, DPN_MediaType mt, IOBehaivor source, IOBehaivor target) {

    if( stream == nullptr ) {
        DL_BADPOINTER(1, "stream");
        return false;
    }
    if( mt == DPN_MS__NOTYPE ) {
        DL_BADVALUE(1, "mt: [%d]", mt);
        return false;
    }
//    stream->setAudioEncoder( iDefaultAudioEncoderContext );
//    stream->setAudioDecoder( iDefaultAudioDecoderContext );
//    stream->setVideoEncoder( iDefaultVideoEncoderContext );
//    stream->setVideoDecoder( iDefaultVideoDecoderContext );

    stream->init(mt, source, target);
    return true;
}
bool DPN_MediaSystem::__run_stream(MediaIOContext **stream, int port) {

    if( !stream ) return false;

    if( *stream == nullptr ) {
        DL_BADPOINTER(1, "*stream");
        return false;
    }
    if( (*stream)->processing() ) {
        DL_ERROR(1, "Audio output stream already run");
        return true;
    }
    if( port > 0 ) {
        if( (*stream)->connect(port) == false ) {
            DL_FUNCFAIL(1, "connect");
            return false;
        }
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
            return __inner_create_stream(client, &pAudioOutputStream, DPN_MS__AUDIO, IO_DEVICE, IO_UDP, c);
        }
        if( mediaType & DPN_MS__VIDEO ) {
            return __inner_create_stream(client, &pVideoOutputStream, DPN_MS__VIDEO, IO_DEVICE, IO_UDP, c);
        }
    } else {
        if( mediaType & DPN_MS__AUDIO ) {
            return __inner_create_stream(client, &pAudioInputStream, DPN_MS__AUDIO, IO_UDP, IO_DEVICE, c);
        }
        if( mediaType & DPN_MS__VIDEO ) {
            return __inner_create_stream(client, &pVideoInputStream, DPN_MS__VIDEO, IO_UDP, IO_DEVICE, c);
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
        if( mediaType & DPN_MS__AUDIO ) return __run_stream(&pAudioOutputStream, remotePort);
        if( mediaType & DPN_MS__VIDEO ) return __run_stream(&pVideoOutputStream, remotePort);
    } else {
        if( mediaType & DPN_MS__AUDIO ) return __run_stream(&pAudioInputStream, remotePort);
        if( mediaType & DPN_MS__VIDEO ) return __run_stream(&pVideoInputStream, remotePort);
    }
    return false;
}
bool DPN_MediaSystem::stopStream(DPN_MediaType mediaType, DPN_DirectionType directionType) {

    if( directionType == DPN_FORWARD ) {

        if( mediaType & DPN_MS__AUDIO ) {
            if( pAudioOutputStream )
            pAudioOutputStream->stop();
        }
        if( mediaType & DPN_MS__VIDEO ) {
            if( pVideoOutputStream )
            pVideoOutputStream->stop();
        }
    } else if( directionType == DPN_BACKWARD ){

        if( mediaType & DPN_MS__AUDIO ) {
            if( pAudioInputStream )
            pAudioInputStream->stop();
        }
        if( mediaType & DPN_MS__VIDEO ) {
            if( pVideoInputStream )
            pVideoInputStream->stop();
        }
    }
    if(     (!pAudioOutputStream || !pAudioOutputStream->processing()) &&
            (!pVideoOutputStream || !pVideoOutputStream->processing()) &&
            (!pAudioInputStream || !pAudioInputStream->processing()) &&
            (!pVideoInputStream || !pVideoInputStream->processing())
      ) {
        pCurrentClient = nullptr;
    }

    return true;
}
bool DPN_MediaSystem::runStream(DPN_MediaType mt, DPN_DirectionType dt) {
    if( mt == DPN_MS__NOTYPE ) {
        DL_BADVALUE(1, "No media type");
        return false;
    }
    if( dt == DPN_FORWARD ) {
        if( mt & DPN_MS__AUDIO ) return __run_stream(&pAudioOutputStream);
        if( mt & DPN_MS__VIDEO ) return __run_stream(&pVideoOutputStream);
    } else {
        if( mt & DPN_MS__AUDIO ) return __run_stream(&pAudioInputStream);
        if( mt & DPN_MS__VIDEO ) return __run_stream(&pVideoInputStream);
    }
    return true;
}
bool DPN_MediaSystem::createAbstractStream(DPN_MediaType mt) {
    if( mt == DPN_MS__NOTYPE ) {
        DL_BADVALUE(1, "No media type");
        return false;
    }

    if( mt & DPN_MS__AUDIO ) {
        return __inner_create_abs_stream(&pAudioOutputStream, DPN_MS__AUDIO, IO_NO, IO_NO);
    }
    if( mt & DPN_MS__VIDEO ) {
        return __inner_create_abs_stream(&pVideoOutputStream, DPN_MS__VIDEO, IO_NO, IO_NO);
    }

    return true;
}
int DPN_MediaSystem::streamPort(DPN_MediaType mediaType, DPN_DirectionType directionType) {
    if( mediaType == DPN_MS__NOTYPE  ) return -1;
    if( directionType == DPN_FORWARD ) {
        if( mediaType == DPN_MS__AUDIO && pAudioOutputStream ) return pAudioOutputStream->port();
        if( mediaType == DPN_MS__VIDEO && pVideoOutputStream ) return pVideoOutputStream->port();
    } else {
        if( mediaType == DPN_MS__AUDIO && pAudioInputStream ) return pAudioInputStream->port();
        if( mediaType == DPN_MS__VIDEO && pVideoInputStream ) return pVideoInputStream->port();
    }
    return -1;
}
bool DPN_MediaSystem::makeShot() {

    if( pVideoOutputStream == nullptr ) {
        if( createAbstractStream(DPN_MS__VIDEO) == false ) {
            DL_FUNCFAIL(1, "createAbstractStream");
            return false;
        }
    }
    if( pVideoOutputStream == nullptr ) {
        DL_BADPOINTER(1, "pVideoOutputStream");
        return false;
    }
    __init_stream(pVideoOutputStream, DPN_MS__VIDEO, IO_DEVICE, IO_MEM);
    if( runStream(DPN_MS__VIDEO, DPN_FORWARD) == false ) {
        DL_FUNCFAIL(1, "runStream");
        return false;
    }

    pVideoOutputStream->makeShot();
    return true;
}
bool DPN_MediaSystem::recordVideo(int time_s) {
    if( pVideoOutputStream == nullptr ) {
        if( createAbstractStream(DPN_MS__VIDEO) == false ) {
            DL_FUNCFAIL(1, "createAbstractStream");
            return false;
        }
    }
    if( pVideoOutputStream == nullptr ) {
        DL_BADPOINTER(1, "pVideoOutputStream");
        return false;
    }
    __init_stream(pVideoOutputStream, DPN_MS__VIDEO, IO_DEVICE, IO_MEM);
    if( runStream(DPN_MS__VIDEO, DPN_FORWARD) == false ) {
        DL_FUNCFAIL(1, "runStream");
        return false;
    }
    pVideoOutputStream->setRecordTime(time_s);
    pVideoOutputStream->makeStream();
    return true;
}
bool DPN_MediaSystem::recordAudio(int time_s) {
    if( pAudioOutputStream == nullptr ) {
        if( createAbstractStream(DPN_MS__AUDIO) == false ) {
            DL_FUNCFAIL(1, "createAbstractStream");
            return false;
        }
    }
    if( pAudioOutputStream == nullptr ) {
        DL_BADPOINTER(1, "pAudioOutputStream");
        return false;
    }
    __init_stream(pAudioOutputStream, DPN_MS__AUDIO, IO_DEVICE, IO_MEM);
    if( runStream(DPN_MS__AUDIO, DPN_FORWARD) == false ) {
        DL_FUNCFAIL(1, "runStream");
        return false;
    }
    pAudioOutputStream->setRecordTime(time_s);
    pAudioOutputStream->makeStream();
    return true;
}
*/
//=============================================================================== MediaIOContext
/*
bool MediaIOContext::threadInjection() {
    return true;
}
void MediaIOContext::threadKick() {

    __deinit();

    fSourceType = IO_NO;
    fTargetType = IO_NO;
    iSourceSize = 0;
    pSourceData = nullptr;
    iVideoEncoderContext.clear();
    iVideoDecoderContext.clear();
    iAudioEncoderContext.clear();
    iAudioDecoderContext.clear();
    wSourceBuffer.clear();
    wTargetBuffer.clear();
    pSourceBaseOpaque = nullptr;
    pSourceCallback = nullptr;
    pTargetBaseOpaque = nullptr;
    pTargetCallback = nullptr;
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
//            || source == IO_NO || target == IO_NO
            ) {
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
//    case IO_NO:
//    default:
//        DL_BADVALUE(1, "source: [%d]", source);
//        return false;
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

    fSourceType |= source;
    fTargetType |= target;
    return true;

}
bool MediaIOContext::stop() {
    block();
    return true;
}
bool MediaIOContext::read() {

    switch ( fSourceType ) {
    case IO_FILE: return __read_file();
    case IO_DEVICE: return __read_device();
    case IO_UDP: return __read_port();
    case IO_MEM: return __read_mem();
    case IO_CALLBACK: return __read_callback();
//    case IO_NO:
//        DL_ERROR(1, "No source type");
//        return false;
//    default:
//        DL_ERROR(1, "Bad source type: [%d]", fSourceType);
//        return false;
    }
    return true;
}
bool MediaIOContext::write() {
    if( iSourceSize == 0 || pSourceData == nullptr ) {
//        DL_BADVALUE(1, "size: [%d] data: [%p]", iSourceSize, pSourceData);
        return true;
    }

    DL_INFO(1, "target flags: [%d] withfile: [%d] file: [%p] context: [%p]", fTargetType, fTargetType&IO_FILE, pTargetFile, this);
    if( fTargetType & IO_UDP ) __write_port();
    if( fTargetType & IO_MEM ) __write_mem();
    if( fTargetType & IO_FILE ) __write_file();
    if( fTargetType & IO_CALLBACK ) __write_callback();

    if( pMaster ) pMaster->clearEncoded();

    iSourceSize = 0;
    pSourceData = nullptr;

    return true;
}
void MediaIOContext::makeShot() {
    iDeviceMode |= DEV__SHOT;
}
void MediaIOContext::makeStream() {
    iDeviceMode |= DEV__STREAM;
}
void MediaIOContext::setRecordTime(int s) {
    if( s < 1 ) return;
    if( s > iRecordTimeLimit ) iRecordTime = iRecordTimeLimit;
    else iRecordTime = s;
    iRecordMode = true;
}
void MediaIOContext::disableRecording() {
    iRecordTime = 0;
    iRecordMode = false;
}
void MediaIOContext::recording() {
    if( iRecordMode ) {
        if( iRecordStart.tv_sec == 0 ) iRecordStart = PROFILER::gettime();
        iRecordCurrent = PROFILER::gettime();
        int d = PROFILER::sec_dif(&iRecordStart, &iRecordCurrent);
        if( d > iRecordTime ) {
            iRecordMode = false;
            iRecordTime = 0;
            iRecordStart = {0,0};
            iRecordCurrent = {0,0};
            fTargetType = __disable_flag(fTargetType, IO_MEM);
        }

    }
}
bool MediaIOContext::setFile(const std::string &path, const std::string &name) {

    if( path.empty() || name.empty() ) {
        DL_BADVALUE(1, "empty path [%d] or name [%d]", path.empty(), name.empty());
        return false;
    }
    __deinit_file();
    auto p = path_merge(path, name);
    if( get_file_size(p) ) {
        //...
    }
    if( (pTargetFile = fopen(p.c_str(), "wb")) == nullptr ) {
        DL_ERROR(1, "Can't open file: [%s]", p.c_str());
        return false;
    }
    auto prev = fTargetType;
    fTargetType |= IO_FILE;
    DL_INFO(1, "set file flag: prev [%d] flags: [%d] MediaIOContext: [%p], file: [%p] fname: [%s]",
            prev, fTargetType, this, pTargetFile, name.c_str());

    return true;
}
bool MediaIOContext::connect(int port) {
    if( pPort == nullptr ) {
        DL_BADPOINTER(1, "port");
        return false;
    }
//    return pPort->connect( PeerAddress( port, clientContext.client()->remoteAddress().address) );
}
int MediaIOContext::port() const {
    if( pPort ) return pPort->localPort();
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
            DL_INFO(1, "Init Video encoder: name [%s] codec [%d] format [%s] opener [%s]",
                    iVideoEncoderContext.name.c_str(), iVideoEncoderContext.codec, iVideoEncoderContext.format.c_str()
                    ,iVideoEncoderContext.opener.c_str());
            if( isValidMediaContext( iVideoEncoderContext ) ) return pMaster->initVideoEncoder( iVideoEncoderContext );
            DL_ERROR(1, "No video encoder context");
            return false;
        }
    }

    DL_BADVALUE(1, "Media type: [%d]", mediaType);
    return false;
}
bool MediaIOContext::__init_port(IOType iot, int preferredPort) {

//    DPN_UDPPort **__port = &pPort;

//    if( *__port ) return true;
//    auto core = clientContext.core();
//    if( core == nullptr ) {
//        DL_BADPOINTER(1, "core");
//        return false;
//    }
//    if( (*__port = core->openUDPPort(preferredPort)) == nullptr ) {
//        DL_FUNCFAIL(1, "openUDPPort: port: [%d] iot: [%d]", preferredPort, iot);
//        return false;
//    }
    wSourceBuffer.reserve(1024*1024);

    return true;
}
void MediaIOContext::__deinit() {
    if( pMaster ) pMaster->stop();
    pPort = nullptr;
    __deinit_file();
}
void MediaIOContext::__deinit_file() {
    if( pTargetFile ) {
        fflush(pTargetFile);
        fclose(pTargetFile);
        pTargetFile = nullptr;
    }
}
bool MediaIOContext::__read_mem() { return __read_device(); }
bool MediaIOContext::__read_file() { return __read_device(); }
bool MediaIOContext::__read_device() {

    if( iDeviceMode == DEV__EMPTY || fTargetType == IO_NO) {
        fSourceType &= ~IO_DEVICE;
        if( pMaster ) pMaster->stop();
        return true;
    }
    if( pMaster == nullptr ) {
        DL_BADPOINTER(1, "pMaster");
        return false;
    }
    if( pMaster->read() == false ) {
        DL_FUNCFAIL(1, "read");
        return false;
    }
    if( iDeviceMode & DEV__STREAM ) {
        if( pMaster->encode() == false ) {
            DL_FUNCFAIL(1, "encode");
            return false;
        }
        pSourceData = pMaster->encodedData();
        iSourceSize = pMaster->encodedSize();
    }
    if( iDeviceMode & DEV__SHOT ) {
        pMaster->encodeImage();

    }

    return true;
}
bool MediaIOContext::__read_port() {
    if( pPort == nullptr ) {
        DL_BADPOINTER(1, "port");
        return false;
    }
    iSourceSize = 0;
    pSourceData = nullptr;
    if( __port_unlimit() ) {
        DL_INFO(1, "Empty port listening: calls: [%d] (limit: [%d]) time(sec): [%d] (limit: [%d])",
                iPortEmptyCalls, iPortEmptyCallsLimit,
                PROFILER::sec_dif(&iPortEmptyStart, &iPortEmptyCurrent), iPortEmptyTimeLimit);

        fSourceType &= ~IO_UDP;
        return false;
    }
//    if( pPort->socket().readable() == false ) {
//        __port_empty_call();
//        return true;
//    }
//    iSourceSize = pPort->socket().receive( wSourceBuffer.getData(), wSourceBuffer.size() );
//    if( iSourceSize == 0 ) {
//        __port_empty_call();
//        return true;
//    }
    if( iSourceSize < 0 ) {
        DL_ERROR(1, "receive error: [%d]", iSourceSize);
        return false;
    }
    __port_drop_empty_stats();
    DL_INFO(1, "read packet: [%d]", iSourceSize);
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
    sb = pPort->x_sendIt( pSourceData, iSourceSize );
    if( sb < 1 ) {
        DL_ERROR(1, "send error: [%d]", sb);
        return false;
    }
    return true;
}
bool MediaIOContext::__write_mem() {
    if( iDeviceMode & DEV__SHOT ) {

        wShotBuffer.copy(pMaster->shotData(), pMaster->shotSize());
        iDeviceMode = __disable_flag(iDeviceMode, DEV__SHOT);
        if( iDeviceMode == DEV__EMPTY ) {
            fSourceType = __disable_flag(fSourceType, IO_DEVICE);
        }
    }
    if( fSourceType == IO_NO ) {
        fTargetType = __disable_flag(fTargetType, IO_MEM);
    }
    recording();
    wTargetBuffer.append( pSourceData, iSourceSize );
    return true;
}
bool MediaIOContext::__write_file() {
    if( pTargetFile == nullptr ) {
        DL_BADPOINTER(1, "file");
        return false;
    }
    int wb = fwrite( pSourceData, 1, iSourceSize, pTargetFile );
    DL_INFO(1, "write file: [%p] data: [%p] size: [%d] written: [%d]",
            pTargetFile, pSourceData, iSourceSize, wb);


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
void MediaIOContext::__port_drop_empty_stats() {
    iPortEmptyCalls = 0;
    iPortEmptyStart = {0,0};
    iPortEmptyCurrent = {0,0};
}
void MediaIOContext::__port_empty_call() {
    ++iPortEmptyCalls;
    if( iPortEmptyStart.tv_sec == 0 ) {
        iPortEmptyStart = PROFILER::gettime();
    }
    iPortEmptyCurrent = PROFILER::gettime();
}
bool MediaIOContext::__port_unlimit() {
    int d = PROFILER::sec_dif(&iPortEmptyStart, &iPortEmptyCurrent);
    return (iPortEmptyCallsLimit && iPortEmptyCalls > iPortEmptyCallsLimit) || (iPortEmptyTimeLimit && d > iPortEmptyTimeLimit);
}
void MediaIOContext::__zero() {

    iDeviceMode = DEV__EMPTY;
    pSourceBaseOpaque = nullptr;
    pSourceCallback = nullptr;
    pTargetBaseOpaque = nullptr;
    pTargetCallback = nullptr;

    pTargetFile = nullptr;
    pSourceData = nullptr;
    iSourceSize = 0;
    fSourceType = IO_NO;
    fTargetType = IO_NO;

    iPortEmptyCalls = 0;
    iPortEmptyStart = {0,0};
    iPortEmptyCurrent = {0,0};
    iPortEmptyCallsLimit = 1024;
    iPortEmptyTimeLimit = 10;

    iRecordTime = 0;
    iRecordTimeLimit = 10;
    iRecordMode = false;
    iRecordStart = {0,0};
    iRecordCurrent = {0,0};

    pMaster = nullptr;

    pPort = nullptr;
}
*/


/*
namespace MEDIA_IO {
//-----------------------------------------------------------------------------
media_io::media_io(DPN_MediaCore *m, DPN_MediaType t) {
    pMediaCore = m;
    iType = t;
}
//-----------------------------------------------------------------------------
reader::reader(DPN_MediaCore *m, DPN_MediaType t) : media_io(m, t) { }
DPN_Result reader::generate(DPN_ExpandableBuffer &b) {
    if( pMediaCore == nullptr ) {
        DL_BADPOINTER(1, "media core");
        return DPN_FAIL;
    }
    if( pMediaCore->read() ) {
        return DPN_SUCCESS;
    }
    return DPN_FAIL;
}
DPN_Result reader::generatorStart() {
    return (DPN_Result)pMediaCore->initEncoder(iType);
}
//-----------------------------------------------------------------------------
encoder::encoder(DPN_MediaCore *m, DPN_MediaType t) : media_io(m, t) { }

DPN_Result encoder::process(DPN_ExpandableBuffer &b) {
    pMediaCore->master()->clearEncoded();

    if( pMediaCore->encode() == false ) {
        DL_FUNCFAIL(1, "encode");
        return DPN_FAIL;
    }
//    setSource( pMediaCore->master()->encodedData(),
//               pMediaCore->master()->encodedSize());

    return DPN_SUCCESS;
}
DPN_Result encoder::processorStart() {
    return (DPN_Result)pMediaCore->initEncoder( iType );
}
//-----------------------------------------------------------------------------
decoder::decoder(DPN_MediaCore *m, DPN_MediaType t) : media_io(m, t) { }
DPN_Result decoder::process(DPN_ExpandableBuffer &b) {
    if( pMediaCore->decode() ) {
        return DPN_FAIL;
    }
    return DPN_SUCCESS;
}
bool decoder::checkProcessor() const {
    if( pMediaCore == nullptr ) {
        DL_BADPOINTER(1, "media core");
        return false;
    }
    if( iType == DPN_MS__NOTYPE ) {
        DL_BADVALUE(1, "media type");
        return false;
    }
    return true;
}
DPN_Result decoder::processorStart() {
    if( pMediaCore->initDecoder(iType) ) {
        return DPN_SUCCESS;
    }
    return DPN_FAIL;
}
//-----------------------------------------------------------------------------


} //MEDIA_IO namespace
*/


/*
DPN_MediaCore::DPN_MediaCore() {
    pMaster = nullptr;
}
void DPN_MediaCore::setContext(const MediaSettings &s) {

    iSettings = s;
    DL_INFO(1, "s: video encoder context: name: [%s] codec: [%d] format: [%s] opener: [%s]",
            s.iVideoEncoderContext.name.c_str(), s.iVideoEncoderContext.codec,
            s.iVideoEncoderContext.format.c_str(), s.iVideoEncoderContext.opener.c_str());
    DL_INFO(1, "video encoder context: name: [%s] codec: [%d] format: [%s] opener: [%s]",
            iSettings.iVideoEncoderContext.name.c_str(), iSettings.iVideoEncoderContext.codec,
            iSettings.iVideoEncoderContext.format.c_str(), iSettings.iVideoEncoderContext.opener.c_str());
}
bool DPN_MediaCore::initEncoder(DPN_MediaType type) {
    if( pMaster == nullptr ) {
        if( (pMaster = createMediaMaster()) == nullptr ) {
            DL_FUNCFAIL(1, "createMediaMaster");
            return false;
        }
    }
    if( pMaster->isReady() ) return true;

    DL_INFO(1, "video encoder context: name: [%s] codec: [%d] format: [%s] opener: [%s]",
            iSettings.iVideoEncoderContext.name.c_str(), iSettings.iVideoEncoderContext.codec,
            iSettings.iVideoEncoderContext.format.c_str(), iSettings.iVideoEncoderContext.opener.c_str());

    switch (type) {
    case DPN_MS__AUDIO: return pMaster->initAudioEncoder( iSettings.iAudioEncoderContext );
    case DPN_MS__VIDEO: return pMaster->initVideoEncoder( iSettings.iVideoEncoderContext );
    default: {
        DL_ERROR(1, "Bad media type [%d]", type);
        return false;
    }
    }
    return false;
}
bool DPN_MediaCore::initDecoder(DPN_MediaType type) {
    if( pMaster == nullptr ) {
        if( (pMaster = createMediaMaster()) == nullptr ) {
            DL_FUNCFAIL(1, "createMediaMaster");
            return false;
        }
    }
    if( pMaster->isReady() ) return true;
    switch (type) {
    case DPN_MS__AUDIO: return pMaster->initAudioDecoder( iSettings.iAudioDecoderContext );
    case DPN_MS__VIDEO: return pMaster->initVideoDecoder( iSettings.iVideoDecoderContext );
    default: {
        DL_ERROR(1, "Bad media type [%d]", type);
        return false;
    }
    }
    return false;
}
*/
//DPN_MediaStreamSettings::DPN_MediaStreamSettings() {
//    setTotalSizeParsing(false);
//    UNIT_TYPE = DPN_MS__NOTYPE;
//}
//DPN_MediaStreamSettings::DPN_MediaStreamSettings(DPN_MediaType mt) {
//    setTotalSizeParsing(false);
//    UNIT_TYPE = mt;
//}
