#include "DPN_MediaMaster.h"






#ifdef DPN_INCLUDE_FFMPEG_FOR_MEDIA
DPN_MediaMaster *createMediaMaster() { return new DPN_MediaMaster; }
void destructMediaMaster(DPN_MediaMaster *m) { if(m) delete m; }
bool isValidMediaContext(const MediaContext &c) { return c.isValid(); }
#else
DPN_MediaMaster * (*__dpn_media_master_create_callback)() = nullptr;
void              (*__dpn_media_master_destruct_callback)(DPN_MediaMaster *m) = nullptr;

void dpn_init_media(DPN_MediaMaster *(*creator)(), void (*destructor)(DPN_MediaMaster *)) {
    __dpn_media_master_create_callback = creator;
    __dpn_media_master_destruct_callback = destructor;
}
DPN_MediaMaster *createMediaMaster() {
    if( __dpn_media_master_create_callback == nullptr ) return nullptr;
    return __dpn_media_master_create_callback();
}
void destructMediaMaster(DPN_MediaMaster *m) {
    if( __dpn_media_master_destruct_callback == nullptr ) return;
    __dpn_media_master_destruct_callback(m);
}
bool isValidMediaContext(const MediaContext &c) { return c; }
#endif





