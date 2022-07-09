


#include "DPN_MediaSystem.h"
using namespace DPN::Logs;
namespace DPN::MediaSystem {
    //--------------------------------------------- MediaCore
    MediaCore::MediaCore() {}
    void MediaCore::setContext(const MediaSettings &s) {
        iSettings = s;
    }
    bool MediaCore::initEncoder(MediaType type) {
        switch (type) {
            case MEDIA__AUDIO: return initAudioEncoder( iSettings.iAudioEncoderContext );
            case MEDIA__VIDEO: return initVideoEncoder( iSettings.iVideoEncoderContext );
            case MEDIA__NO_TYPE:
                DL_ERROR(1, "No media type");
                return false;
        }
    }
    bool MediaCore::initDecoder(MediaType type) {
        switch (type) {
            case MEDIA__AUDIO: return initAudioDecoder( iSettings.iAudioDecoderContext );
            case MEDIA__VIDEO: return initVideoDecoder( iSettings.iVideoDecoderContext );
            case MEDIA__NO_TYPE:
                DL_ERROR(1, "No media type");
                return false;
        }
    }
    //--------------------------------------------- IO__Media
    IO__Media::IO__Media(MediaType type, MediaCore *core) {
        iType = type;
        pMediaCore = core;
    }
    //--------------------------------------------- IO__Reader
    DPN_Result IO__Reader::generate(DPN_ExpandableBuffer &b) {
        if( pMediaCore == nullptr ) {
            DL_BADPOINTER(1, "media core");
            return DPN_FAIL;
        }
        if( pMediaCore->read() ) {
//            b.clear();
//            b.append( pMediaCore->readedData(), pMediaCore->readedSize() );
            return DPN_SUCCESS;
        }
        return DPN_FAIL;
    }
    DPN_Result IO__Reader::generatorStart() {
        return (DPN_Result)pMediaCore->initEncoder(iType);
    }
    //--------------------------------------------- IO__Encoder
    DPN_Result IO__Encoder::process(DPN_ExpandableBuffer &b) {

        if( pMediaCore == nullptr ) {
            DL_BADPOINTER(1, "media core");
            return DPN_FAIL;
        }
        pMediaCore->clearEncoded();


        if( pMediaCore->encode() == false ) {
            DL_FUNCFAIL(1, "encode");
            return DPN_FAIL;
        }
        b.clear();
        b.append( pMediaCore->encodedData(), pMediaCore->encodedSize() );

        return DPN_SUCCESS;
    }
    DPN_Result IO__Encoder::processorStart() {
        return (DPN_Result)pMediaCore->initEncoder(iType);
    }
    //--------------------------------------------- IO__Decoder
    DPN_Result IO__Decoder::process(DPN_ExpandableBuffer &b) {
        if( pMediaCore == nullptr ) {
            DL_BADPOINTER(1, "media core");
            return DPN_FAIL;
        }
        pMediaCore->setEncodedData( b.getData(), b.size() );

        if( pMediaCore->decode() == false ) {
            DL_FUNCFAIL(1, "decode");
            return DPN_FAIL;
        }
        b.clear();
        b.append( pMediaCore->decodedData(), pMediaCore->decodedSize() );
        return DPN_SUCCESS;
    }
    DPN_Result IO__Decoder::processorStart() {
        return (DPN_Result)pMediaCore->initDecoder(iType);
    }
    //--------------------------------------------- MediaSystem
    MediaSystem::MediaSystem(const std::string &name, ClientCenter &cc)
        : DPN::Network::NetworkModule( name, cc )
    {

    }

}
