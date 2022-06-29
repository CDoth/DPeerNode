#ifndef DPN_MEDIASYSTEM_H
#define DPN_MEDIASYSTEM_H

//#include "DPN_MediaMaster.h"
//#include "DPN_Channel.h"
//#include "DPN_TransmitTools.h"

//class DPN_MediaSystem;

/*
enum DPN_MediaType {
    DPN_MS__NOTYPE          = 0b00000000
    , DPN_MS__AUDIO         = 0b00000001
    , DPN_MS__VIDEO         = 0b00000010
};



struct MediaSettings {
    VideoEncoderContext iVideoEncoderContext;
    VideoDecoderContext iVideoDecoderContext;
    AudioEncoderContext iAudioEncoderContext;
    AudioDecoderContext iAudioDecoderContext;
};

class DPN_MediaCore {
public:
    DPN_MediaCore();
    void setContext( const MediaSettings & s ) ;
    bool initEncoder(DPN_MediaType type);
    bool initDecoder(DPN_MediaType type);

    inline bool encode() {return pMaster ? pMaster->encode() : false;}
    inline bool read() {return pMaster ? pMaster->read() : false; }
    inline bool decode() {return pMaster ? pMaster->decode() : false;}
    inline DPN_MediaMaster * master() {return pMaster;}
    inline const DPN_MediaMaster * master() const {return pMaster;}

private:
    DPN_MediaMaster *pMaster;
    MediaSettings iSettings;
};

namespace MEDIA_IO {
using namespace DPN_IO;
    class media_io : public IOContext {
    public:
        media_io(DPN_MediaCore *m, DPN_MediaType t);
    protected:
        DPN_MediaType iType;
        DPN_MediaCore *pMediaCore;
    };

    class reader : public media_io {
    public:
        reader(DPN_MediaCore *m, DPN_MediaType t);
        DPN_Result generate(DPN_ExpandableBuffer &b) override;
    private:
        DPN_Result generatorStart() override;
    };
    class encoder : public media_io {
    public:
        encoder(DPN_MediaCore *m, DPN_MediaType t);
        DPN_Result process(DPN_ExpandableBuffer &b) override;
        DPN_Result processorStart() override;
    };
    class decoder : public media_io {
    public:
        decoder(DPN_MediaCore *m, DPN_MediaType t);
        DPN_Result process(DPN_ExpandableBuffer &b) override;
        bool checkProcessor() const override;
        DPN_Result processorStart() override;
    };
};


//class DPN_MediaStreamSettings : public DPN_Content {
//public:
//    DPN_MediaStreamSettings();
//    DPN_MediaStreamSettings(DPN_MediaType mt);
//    void setType(DPN_MediaType mt) {UNIT_TYPE = mt;}
//    DPN_MediaType type() const {return UNIT_TYPE.get();}
//private:

//    UNIT(DPN_MediaType) UNIT_TYPE = content;
//};

class DPN_MediaSystem : public DPN_AbstractModule {
public:
    friend class DPN_MediaDirection;
public:
    DPN_MediaSystem(const std::string &name);
    ~DPN_MediaSystem();

//    bool channeling( __channel_mono_interface &channel, const DPN_ExpandableBuffer &context) override;
    //---------------------------------------- io test:
    bool startStream1(__channel_mono_interface &channel, DPN_MediaType mt);
    bool acceptIncoming();
    //----------------------------------------
    void setVideoDevice(const VideoEncoderContext &vec);
    void setVideoOutput(const VideoDecoderContext &vdc);
    void setAudioDevice(const AudioEncoderContext &aec);

private:
    DArray<__channel_mono_interface> aLockedChannels;


    std::mutex iMutex;
    MediaSettings iDefaultSettings;
};
*/

#endif // DPN_MEDIASYSTEM_H
