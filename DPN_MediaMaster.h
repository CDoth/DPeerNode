#ifndef DPN_MEDIAMASTER_H
#define DPN_MEDIAMASTER_H
#include "__dpeernode_global.h"

#ifdef DPN_INCLUDE_FFMPEG_FOR_MEDIA
#include <DFFMpeg.h>
#endif

class DPN_MediaMaster;

DPN_MediaMaster * createMediaMaster();
void destructMediaMaster(DPN_MediaMaster *m);

#ifdef DPN_INCLUDE_FFMPEG_FOR_MEDIA
class DPN_MediaSystem;
struct MediaContext {
    MediaContext() {iValid = false;}
    inline bool isValid() const {return iValid;}
    MediaContext & operator=(const MediaContext &o) {iValid = true; return *this;}
    void clear() {iValid = false;}
private:
    bool iValid;
};
struct VideoEncoderContext : public MediaContext {
    std::string format;
    std::string opener;
    std::string name;
    AVCodecID codec;
};
struct VideoDecoderContext : public MediaContext {
    AVPixelFormat preferredFormat;
};
struct AudioEncoderContext : public MediaContext {
    std::string opener;
    std::string name;
    AVCodecID codec;
};
struct AudioDecoderContext : public MediaContext {};

struct MediaContextSet {
    MediaContextSet & operator=(const MediaContextSet &o) {return *this;}
    VideoEncoderContext vec;
    VideoDecoderContext vdc;
    AudioEncoderContext aec;
    AudioDecoderContext adc;
};

bool isValidMediaContext(const MediaContext &c);


class DPN_MediaMaster {
public:
    DPN_MediaMaster() {pInner = new DFFMpeg;}
    ~DPN_MediaMaster() {delete pInner;}

    inline bool read() { return pInner->read(); }
    inline bool encode() { return pInner->encode(); }
    inline bool encodeImage() { return pInner->encodeImage();}

    inline const uint8_t * shotData() const {return pInner->shotData();}
    inline int shotSize() const {return pInner->shotSize();}

    inline const uint8_t * readedData() const { return pInner->readedData(); }
    inline int readedSize() const { return pInner->readedSize(); }

    inline const uint8_t * encodedData() const { return pInner->writtenData(); }
    inline int encodedSize() const { return pInner->writtenSize(); }
    inline void clearEncoded() { pInner->clearWritten(); }

    inline bool setEncodedData(const uint8_t *data, int size) { return pInner->setVideoData(data, size); }
    inline bool decode() { return pInner->decode(); }

    inline const uint8_t * decodedData() const { return pInner->decodedData(); }
    inline int decodedSize() const { return pInner->decodedSize(); }


    inline bool isReady() const { return !(pInner->isEmpty() || pInner->isFault());}
    inline void stop() { pInner->stop(); }

    inline bool initVideoEncoder(const VideoEncoderContext &c) {
        return pInner->initVideoEncoder(c.format.c_str(), c.codec, c.opener.c_str(), c.name.c_str());
    }
    inline bool initVideoDecoder(const VideoDecoderContext &c) {
        return pInner->initVideoDecoder(c.preferredFormat) ;
    }
    inline bool initAudioEncoder(const AudioEncoderContext &c) {
        return pInner->initAudioEncoder(c.codec, c.opener.c_str(), c.name.c_str()) ;
    }
    inline bool initAudioDecoder(const AudioDecoderContext&) {
        return pInner->initAudioDecoder();
    }
    int encodingTime() const {
        return pInner->encodingSeconds();
    }
private:
    DFFMpeg *pInner;
};

#else

void dpn_init_media(
        DPN_MediaMaster * (*creator)(),
        void (*destructor)(DPN_MediaMaster*)
        );

typedef void* MediaContext;
typedef void* VideoEncoderContext;
typedef void* VideoDecoderContext;
typedef void* AudioEncoderContext;
typedef void* AudioDecoderContext;

bool isValidMediaContext(const MediaContext &c);


class DPN_MediaMaster {
public:
    virtual bool read() = 0;
    virtual bool encode() = 0;
    virtual const uint8_t * encodedData() const = 0;
    virtual int encodedSize() const = 0;
    virtual void clearEncoded() = 0;

    virtual bool setEncodedData(const uint8_t *data, int size) = 0;
    virtual bool decode() = 0;
    virtual const uint8_t * decodedData() const = 0;

    virtual bool isReady() const = 0;
    virtual void stop() = 0;
    virtual int encodingTime() const = 0;

    virtual bool initVideoEncoder(const VideoEncoderContext&) = 0;
    virtual bool initVideoDecoder(const VideoDecoderContext&) = 0;
    virtual bool initAudioEncoder(const AudioEncoderContext&) = 0;
    virtual bool initAudioDecoder(const AudioDecoderContext&) = 0;

};

#endif

#endif // DPN_MEDIAMASTER_H
