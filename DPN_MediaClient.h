#ifndef DPN_MEDIACLIENT_H
#define DPN_MEDIACLIENT_H

#include "DPN_MediaSystemProcessors.h"
#include "DPN_Client.h"

class DPN_MediaClient : public DWatcher<DPN_ClientData> {
public:
    DPN_MediaClient(DPN_Client &c);

    bool startVideoStream();
    bool startVideoStream(const VideoEncoderContext &context);
    bool startAudioStream();


    void setGlobal(DPN_MediaSystem *ms);

private:
    DPN_MediaSystem *pMediaSystem;
};


#endif // DPN_MEDIACLIENT_H
