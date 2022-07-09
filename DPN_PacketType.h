#ifndef DPN_PACKETTYPE_H
#define DPN_PACKETTYPE_H
#include "__dpeernode_global.h"

enum PacketType {
    PT__NO_TYPE = 0
    //-------------------------------- Default
    ,PT__TEXT_MESSAGE
    ,PT__MAKE_SHADOW_CONNECTION
    ,PT__SYNC_CHANNEL
    ,PT__RESERVE_CHANNEL
    //-------------------------------- Chat
    ,PT__CHAT__MESSAGE
    //-------------------------------- File System
    ,PT__FS__SYNC_CATALOGS
    ,PT__FS__REGISTER_FAST_FILE
    ,PT__FS__REQUEST_FILE
    //-------------------------------- Network
    ,PT__NW__PING
    ,PT__NW__LOCAL_ENVIRONMENT
    ,PT__NW__FORWARDING
    ,PT__NW__SERVER_OPENER
    //--------------------------------
    ,PT__INVALID_TYPE
};


uint32_t packetMaxSize( PacketType );
bool packetSupportEmptySize(PacketType);
const char * packetTypeName(PacketType);

#endif // DPN_PACKETTYPE_H
