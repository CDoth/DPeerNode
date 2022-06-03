#ifndef DPN_MEDIASYSTEM_H
#define DPN_MEDIASYSTEM_H
#include "DPN_Channel.h"

#include "DPN_MediaMaster.h"



class DPN_MediaSystem;




/*
class DPN_MediaDirection : public DPN_Direction {
public:
    DPN_MediaDirection();
    inline void setMediaSystem(DPN_MediaSystem *ms) {pMediaSystem = ms;}
    bool close() override;

protected:
    bool prepareVideoSender();
    bool prepareAudioSender();
    bool prepareVideoReceiver();
    bool prepareAudioReceiver();
protected:
    DPN_MediaSystem *pMediaSystem;
    DPN_MediaMaster *pMaster;
};
class DPN_VideoSendDirection : public DPN_MediaDirection {
public:
    bool threadInjection() override;
    bool proc() override;
//    bool close() override;
};
class DPN_VideoReceiveDirection : public DPN_MediaDirection {
public:
    bool threadInjection() override;
    bool proc() override;
//    bool close() override;
};
class DPN_AudioSendDirection : public DPN_MediaDirection {
public:
    bool threadInjection() override;
    bool proc() override;
//    bool close() override;

};
class DPN_AudioReceiveDirection : public DPN_MediaDirection {
public:
    bool threadInjection() override;
    bool proc() override;
//    bool close() override;
};
class DPN_RecordDirection : public DPN_MediaDirection {

};

*/

struct DPN_MediaStreamTransaction : public DPN_AbstractTransaction {
    VideoEncoderContext iVideoEncoderContext;
    VideoDecoderContext iVideoDecoderContext;
    AudioEncoderContext iAudioEncoderContext;
    AudioDecoderContext iAudioDecoderContext;


};

enum DPN_MediaType {
    DPN_MS__NOTYPE          = 0b00000000
    , DPN_MS__AUDIO         = 0b00000001
    , DPN_MS__VIDEO         = 0b00000010
    , DPN_MS__AUDIOVIDEO    = 0b00000011
};
enum IOBehaivor {
    IO_NO      =  0b00000000,
    IO_DEVICE   = 0b10000000,
    IO_UDP     =  0b00000001,
    IO_MEM     =  0b00000010,
    IO_FILE    =  0b00000100,
    IO_CALLBACK  =  0b00001000,
};
struct MediaIOContext : public DPN_Direction {

    bool threadInjection() override;
    bool proc() override;
    bool close() override;

    enum IOType {
        IO_SOURCE, IO_TARGET
    };
    MediaIOContext();

    bool init(DPN_MediaType mediaType, IOBehaivor source, IOBehaivor target);
    bool stop();
    inline IOBehaivor source() const {return fSourceType;}

    bool read();
    bool write();


    void setFile(const std::string &path, const std::string &name);
    void setSourceCallback(bool (*cb)(void *opaque, DPN_ExpandableBuffer &buffer, int &packetSize));
    void setTargetCallback(bool (*cb)(void *opaque, const uint8_t *data, int size));
    void setSourceBase(void *base);
    void setTargetBase(void *base);
    void setPreferredPort(int port);

    void setVideoEncoder(const VideoEncoderContext &context) {iVideoEncoderContext = context;}
    void setVideoDecoder(const VideoDecoderContext &context) {iVideoDecoderContext = context;}
    void setAudioEncoder(const AudioEncoderContext &context) {iAudioEncoderContext = context;}
    void setAudioDecoder(const AudioDecoderContext &context) {iAudioDecoderContext = context;}


    int port() const;
private:
    bool __init_device(IOType iot, DPN_MediaType mediaType);
    bool __init_port(IOType iot, int preferredPort);

    bool __read_device();
    bool __read_port();
    bool __read_mem();
    bool __read_file();
    bool __read_callback();

    bool __write_port();
    bool __write_mem();
    bool __write_file();
    bool __write_callback();

    void __zero();
private:
    void *pSourceBaseOpaque;
    bool (*pSourceCallback)(void *opaque, DPN_ExpandableBuffer &buffer, int &packetSize);
    void *pTargetBaseOpaque;
    bool (*pTargetCallback)(void *opaque, const uint8_t *data, int size);

    DPN_ExpandableBuffer wSourceBuffer;
    DPN_ExpandableBuffer wTargetBuffer;
    FILE *pTargetFile;
    const uint8_t *pSourceData;
    int iSourceSize;
    IOBehaivor fSourceType;
    IOBehaivor fTargetType;
    DPN_UDPPort *pPort;

    DPN_MediaMaster *pMaster;

    VideoEncoderContext iVideoEncoderContext;
    VideoDecoderContext iVideoDecoderContext;
    AudioEncoderContext iAudioEncoderContext;
    AudioDecoderContext iAudioDecoderContext;
};
class DPN_MediaSystem : public DPN_AbstractModule {
public:
    friend class DPN_MediaDirection;



public:
    DPN_MediaSystem();
    ~DPN_MediaSystem();



    void clientDisconnected(const DPN_AbstractClient *client) override;
    void stop() override;
    void innerClientDisconnected(DPN_AbstractClient *c);


//    void setCore(DPN_AbstractConnectionsCore *c) {pCore = c;}

    class InnerDeviceContext {
        bool iValid;
        void *pOpaque;
        friend class DPN_MediaSystem;
    public:
        InnerDeviceContext() : iValid(false) {}
        void setOpqueData(void *opaque) {pOpaque = opaque;}
    };

    void setVideoDevice(const VideoEncoderContext &vec);
    void setVideoOutput(const VideoDecoderContext &vdc);
    void setAudioDevice(const AudioEncoderContext &aec);


    //==============================================


    bool createStream(DPN_AbstractClient *client, DPN_MediaType mediaType, DPN_DirectionType directionType, MediaContextSet *c = nullptr);
    bool runStream(DPN_AbstractClient *client, DPN_MediaType mediaType, DPN_DirectionType directionType, int remotePort);
    int streamPort(DPN_MediaType mediaType, DPN_DirectionType directionType);


    bool isAvailable(DPN_AbstractClient *client, DPN_MediaType mediaType, DPN_DirectionType directionType);
    bool receiveStream(DPN_AbstractClient *source, DPN_MediaType mediaType);
    bool sendStream(DPN_AbstractClient *target, DPN_MediaType mediaType);
    bool connect(int vport, int aport, const std::string &address);

    bool stop(DPN_MediaType mediaType);
    bool stop(DPN_MediaType mediaType, DPN_DirectionType directionType);
    //==============================================

    bool sendVideoFrame();
    bool sendAudioFrame();
    bool receiveVideoFrame();
    bool receiveAudioFrame();




    static DPN_MediaType addT(DPN_MediaType t1, DPN_MediaType t2) { return DPN_MediaType(t1 | t2); }
    static DPN_MediaType remT(DPN_MediaType t1, DPN_MediaType t2) { return DPN_MediaType( t1 & ~t2); }
private:



    bool __create_stream(DPN_AbstractClient *client, MediaIOContext **stream, DPN_MediaType mediaType, IOBehaivor source, IOBehaivor target, MediaContextSet *c = nullptr);
    bool __run_stream(MediaIOContext **stream);

    bool __prepareVideoSender();
    bool __prepareVideoReceiver();
    bool __prepareAudioSender();
    bool __prepareAudioReceiver();


    static bool checkPermission(DPN_MediaType permission, DPN_MediaType type);
private:
    std::map<DPN_AbstractClient*, DPN_MediaType> iClientInputPermissionMap;
    DPN_MediaType eGlobalInputPermission;

    DPN_AbstractClient *pCurrentClient;
    MediaIOContext *pVideoOutputStream;
    MediaIOContext *pVideoInputStream;
    MediaIOContext *pAudioOutputStream;
    MediaIOContext *pAudioInputStream;
private:


    void addIOBehaivor(const IOBehaivor &add, IOBehaivor &to);
    void remIOBehaivor(const IOBehaivor &rem, IOBehaivor &from);

private:

    std::mutex iMutex;

    VideoEncoderContext iDefaultVideoEncoderContext;
    VideoDecoderContext iDefaultVideoDecoderContext;
    AudioEncoderContext iDefaultAudioEncoderContext;
    AudioDecoderContext iDefaultAudioDecoderContext;


    MediaIOContext iAudioOutput;
    MediaIOContext iVideoOutput;
    MediaIOContext iAudioInput;
    MediaIOContext iVideoInput;


};

#endif // DPN_MEDIASYSTEM_H
