#ifndef DPN_PACKETTYPE_H
#define DPN_PACKETTYPE_H
#include "__dpeernode_global.h"

// - Remote side create keyset for request and send it to host

// - Host check all keys from keyset and create file group with available files

// - Host send [DPN_PACKETTYPE__FILE_TRANSMIT_PREPARE] for each file from group
//   and place group-processor in send queue. On begin all file transports is not
//   available and group-processor just skip all transports.

// - Remote side prepare all files and send [DPN_PACKETTYPE__FILE_RECEIVE_READY_ANSWER]
//   for each file to host.

// - If [DPN_PACKETTYPE__FILE_RECEIVE_READY_ANSWER] for file is true - activate its transport
//   (which already in group). After this action - group-processor will not ingore and skip
//   this transport.
// - If [DPN_PACKETTYPE__FILE_RECEIVE_READY_ANSWER] for file if false - deactivate its transport.
//   Remove it from send map and group

enum DPN_PacketType {
    DPN_PACKETTYPE__NO_TYPE = 0
    ,DPN_PACKETTYPE__TEXT_MESSAGE
    ,DPN_PACKETTYPE__SYNC
    ,DPN_PACKETTYPE__HASH_SUM


    ,DPN_PACKETTYPE__REGISTER_FAST_FILE

    ,DPN_PACKETTYPE__TRANSMIT_ANSWER

    ,DPN_PACKETTYPE__USER_TYPE
    //--------------------------------
    /// >>> HOST(SEND) SIDE:
    /// - Calculate hashsum
    /// - Prepare file to send (open or load it), check pointers and buffers
    /// >>> REMOTE(RECEIVE) SIDE:
    /// - Get hashsum and check it
    /// - Create necessary directories and files, open all files
    /// - Send to host [DPN_PACKETTYPE__FILE_RECEIVE_READY_ANSWER]
    ,DPN_PACKETTYPE__FILE_TRANSMIT_PREPARE


    /// Remote node request file or file set from host catalog:
    ,DPN_PACKETTYPE__REQUEST_FILE

    /// Transmit file data:
    ,DPN_PACKETTYPE__FILE_PACKET
//    ,DPN_PACKETTYPE__RESERVE_FILE_CHANNEL
    //--------------------------------
    ,DPN_PACKETTYPE__MEDIA_ANSWER
    ,DPN_PACKETTYPE__MEDIA_STREAM_PREPARE
    //--------------------------------
    ,DPN_PACKETTYPE__PING
    ,DPN_PACKETTYPE__REQUEST
    ,DPN_PACKETTYPE__LOCAL_ENVIRONMENT
    //--------------------------------
    ,DPN_PACKETTYPE__MAKE_SHADOW_CONNECTION
    //--------------------------------
    ,DPN_PACKETTYPE__INVALID_TYPE

};
uint32_t packetMaxSize(DPN_PacketType);
bool packetSupportEmptySize(DPN_PacketType);
const char * packetTypeName(DPN_PacketType);

#endif // DPN_PACKETTYPE_H
