#include "DPN_MediaClient.h"

DPN_MediaClient::DPN_MediaClient(DPN_Client &c) {
    defaultCopy(c);
    pMediaSystem = extractMediaModule(data()->modules());
}
bool DPN_MediaClient::startVideoStream() {

    if( data()->state() == DISCONNECTED ) {
        DL_INFO(1, "Client disconnected");
        return false;
    }
    if( pMediaSystem == nullptr ) {
        DL_BADPOINTER(1, "Media System");
        return false;
    }

    auto proc = data()->processor<DPN_PACKETTYPE__MEDIA_STREAM_PREPARE>();
    proc->setType(DPN_MS__VIDEO);
    data()->send(proc);

    return true;
}
bool DPN_MediaClient::startVideoStream(const VideoEncoderContext &context) {

    if( data()->state() == DISCONNECTED ) {
        DL_INFO(1, "Client disconnected");
        return false;
    }
    if( pMediaSystem == nullptr ) {
        DL_BADPOINTER(1, "Media System");
        return false;
    }

    auto proc = data()->processor<DPN_PACKETTYPE__MEDIA_STREAM_PREPARE>();
    proc->setType(DPN_MS__VIDEO);
    proc->setCustomContext(context);
    data()->send(proc);

    return true;
}
bool DPN_MediaClient::startAudioStream() {

    if( pMediaSystem == nullptr ) {
        DL_BADPOINTER(1, "Media System");
        return false;
    }

//    auto proc = data()->processor<DPN_PACKETTYPE__MEDIA_STREAM_PREPARE>();
//    proc->setType(DPN_MediaSystem::DPN_MS__AUDIO);
//    data()->send(proc);

    return true;
}

void DPN_MediaClient::setGlobal(DPN_MediaSystem *ms) {

    data()->modules().addModule(ms, "Media");
}
