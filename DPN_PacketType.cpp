#include "DPN_PacketType.h"



using namespace DPN;
const char *packetTypeName(PacketType t) {
    switch (t) {

    GET_NAME(PT__NO_TYPE);

    GET_NAME(PT__TEXT_MESSAGE);
    GET_NAME(PT__MAKE_SHADOW_CONNECTION);
    GET_NAME(PT__SYNC_CHANNEL);
    GET_NAME(PT__RESERVE_CHANNEL);

    GET_NAME(PT__CHAT__MESSAGE);

    GET_NAME(PT__FS__SYNC_CATALOGS);
    GET_NAME(PT__FS__REGISTER_FAST_FILE);
    GET_NAME(PT__FS__REQUEST_FILE);

    GET_NAME(PT__NW__PING);
    GET_NAME(PT__NW__LOCAL_ENVIRONMENT);

    GET_NAME(PT__INVALID_TYPE);

    default: return "[NO TYPE]";
    }
}
uint32_t packetMaxSize(PacketType t) {

    DPN_SETTINGS_MARK

    switch (t) {
//    case DPN_PACKETTYPE__FILE_PACKET: return DPN_DEFAULT_MAX_FILE_PACKET_SIZE;
    default: return DPN_DEFAULT_MAX_PACKET_SIZE;
    }
}
bool packetSupportEmptySize(PacketType t) {

    DPN_SETTINGS_MARK

    switch (t) {
//    case DPN_PACKETTYPE__VIDEO_STREAM_PREPARE: return true;
    default: return false;
    }
}
