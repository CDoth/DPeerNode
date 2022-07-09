#ifndef DPN_MEDIASYSTEM_H
#define DPN_MEDIASYSTEM_H

#include "DPN_Network.h"
#include "DPN_MediaMaster.h"

namespace DPN::MediaSystem {
    enum MediaType {
        MEDIA__NO_TYPE
        , MEDIA__AUDIO
        , MEDIA__VIDEO
    };
    //----------------------------------------------------
    struct MediaSettings {
        VideoEncoderContext iVideoEncoderContext;
        VideoDecoderContext iVideoDecoderContext;
        AudioEncoderContext iAudioEncoderContext;
        AudioDecoderContext iAudioDecoderContext;
    };
    class MediaCore
            : public DPN_MediaMaster
    {
    public:
        MediaCore();
        void setContext( const MediaSettings & s ) ;
        bool initEncoder( MediaType type );
        bool initDecoder( MediaType type );
    private:
        MediaSettings iSettings;
    };
    //----------------------------------------------------
    class IO__Media
            : public DPN::IO::IOContext
    {
    public:
        IO__Media( MediaType type, MediaCore *core );
    protected:
        inline bool mediaCheck() const { return iType != MEDIA__NO_TYPE && pMediaCore; }
    protected:
        MediaType iType;
        MediaCore *pMediaCore;
    };
    class IO__Reader
            : public IO__Media
    {
    public:
        using IO__Media::IO__Media;

        inline bool checkGenerator() const override { return mediaCheck(); }
        DPN_Result generate(DPN_ExpandableBuffer &b) override;
    private:
        DPN_Result generatorStart() override;
    };
    class IO__Encoder
            : public IO__Media
    {
    public:
        using IO__Media::IO__Media;

        inline bool checkProcessor() const override { return mediaCheck(); }
        DPN_Result process(DPN_ExpandableBuffer &b) override;
        DPN_Result processorStart() override;
    };
    class IO__Decoder
            : public IO__Media
    {
    public:
        using IO__Media::IO__Media;

        inline bool checkProcessor() const override { return mediaCheck(); }
        DPN_Result process(DPN_ExpandableBuffer &b) override;
        DPN_Result processorStart() override;
    };
    //----------------------------------------------------
    struct __client_data__ {

    };
    class ClientInterface
            : public DPN::Interface::InterfaceReference< __client_data__ > {

    };
    class ClientDescriptor
            : public DPN::Interface::DataReference< __client_data__ > {

    };
    class MediaSystem
            : public DPN::Network::NetworkModule
    {
    public:
        MediaSystem( const std::string &name, ClientCenter &cc );
    private:
        std::map< DPN::Client::Tag, ClientDescriptor> iClientsData;
    };
}



#endif // DPN_MEDIASYSTEM_H
