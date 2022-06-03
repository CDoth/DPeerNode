#include "DPN_FileSystemProcessors.h"


#define INFO_LOG DL_INFO(1, "PROC: [%s]", packetTypeName(me()));
//#define FILE_SYSTEM clientContext.modules()->fileSystem()
#define CT_PROC(TYPE) threadContext.processor<TYPE>()
//====================================================================================== DPN_Processor__sync:
DPN_Result DPN_Processor__sync::sendPrefix() {

    INFO_LOG;

    if( pFileSystem == nullptr ) {
        DL_BADPOINTER(1, "File system");
        return DPN_FAIL;
    }

    auto host_catalog = pFileSystem->host();
    if(host_catalog == nullptr) {
        DL_BADPOINTER(1, "Host catalog");
        return DPN_FAIL;
    }
    host_catalog->renew();
    std::string topology = host_catalog->topology(true);
    if(topology.empty()) {
        DL_ERROR(1, "Empty topology");
        return DPN_FAIL;
    }
    pFileSystem->unverifyHost2Remote();

    UNIT_TOPOLOGY = topology;

    return DPN_SUCCESS;
}
DPN_Result DPN_Processor__sync::receiveReaction() {

    INFO_LOG;

    if( pFileSystem == nullptr ) {
        DL_BADPOINTER(1, "File system");
        return DPN_FAIL;
    }

    if( clientContext.modules() == nullptr ) {
        DL_BADPOINTER(1, "modules");
        return DPN_FAIL;
    }
    if( pFileSystem == nullptr ) {
        DL_BADPOINTER(1, "File system");
        return DPN_FAIL;
    }
    if( threadContext.hashtool() == nullptr ) {
        DL_BADPOINTER(1, "hashtool");
        return DPN_FAIL;
    }

    std::string remote_topology = UNIT_TOPOLOGY.get();
    pFileSystem->unverifyRemote2Host();
    std::string host_topology = pFileSystem->remote().sync(remote_topology);

    threadContext.hashtool()->hash_string(host_topology);
    std::string host_hash = threadContext.hashtool()->get();

    auto proc = CT_PROC(DPN_PACKETTYPE__HASH_SUM);
    proc->setCatalogHashsum(host_hash);
    send(proc);

    DL_INFO(1, "HOST HASH: [%s]", host_hash.c_str());

    return DPN_SUCCESS;
}
//====================================================================================== DPN_Processor__reserve_file_channel:
/*
DPN_Result DPN_Processor__reserve_file_channel::receiveReaction() {

    if( pFileSystem == nullptr ) {
        DL_BADPOINTER(1, "File system");
        return DPN_FAIL;
    }

    std::string shadowKey = UNIT_SHADOW_KEY.get();
    PeerAddress source = UNIT_SOURCE.get();
    PeerAddress target = UNIT_TARGET.get();

    DPN_Channel *channel = clientContext.client()->channel(shadowKey);
    auto proc = processor<DPN_PACKETTYPE__TRANSMIT_ANSWER>();
    proc->setType(DPN_ANSWER__CHANNEL_RESERVE);
    proc->setHash(shadowKey);





    if( channel == nullptr ) {
        DL_ERROR(1, "No channel");
        goto fail;
    }
    if( channel->local() != source || channel->remote() != target ) {

        DL_ERROR(1, "Bad stats: channel: local: [%s] remote: [%s] receive: source: [%s] target: [%s]",
                 channel->local().name().c_str(),
                 channel->remote().name().c_str(),
                 source.name().c_str(),
                 target.name().c_str()
                 );
        goto fail;
    }
    if( pFileSystem->reserveReceiver(channel) == false ) {
        DL_FUNCFAIL(1, "reserveFileReceiver");
        goto fail;
    }
    clientContext.core()->replanDirections();

    DL_INFO(1, "channel finded: [%p] and reserved "
               "as receiver", channel);

    proc->setState(true);
    send(proc);

    return DPN_SUCCESS;
fail:
    proc->setState(false);
    proc->setValue1(DPN_BACKWARD);
//    send(proc);
    return DPN_FAIL;
}
*/
//====================================================================================== DPN_Processor__hash_sum:
DPN_Result DPN_Processor__hash_sum::receiveReaction() {

    INFO_LOG;

    if( pFileSystem == nullptr ) {
        DL_BADPOINTER(1, "File system");
        return DPN_FAIL;
    }

    std::string hash = UNIT_HASH.get();
    int key = UNIT_FILE_KEY.get();


    if(key == CATALOG_HASHSUM_KEY) {
        bool v = pFileSystem->verifyHost2Remote(hash);
        DL_INFO(1, "hash: [%s] v: [%d]", hash.c_str(), v);

        auto proc = CT_PROC(DPN_PACKETTYPE__TRANSMIT_ANSWER);
        proc->setType(DPN_ANSWER__REMOTE_CATALOG_VERIFIED);
        proc->setState(v);
        send(proc);
    }


    /*

    if(key == CATALOG_HASHSUM_KEY) {

        DL_INFO(1, "Catalog hashsum FS: [%p]", FILE_SYSTEM);

//        bool v = __file_system->verifyHost2Remote(hash, env->hashtool());

//        auto proc = COMPILETIME_PROCESSOR<DPN_PACKETTYPE__TRANSMIT_ANSWER>(*env).processor();
//        proc->setSource(DPN_ANSWER__REMOTE_CATALOG_VERIFIED);
//        proc->setState(v);
//        env->putBackProcessor(proc);


//        DL_INFO(1, "HASH: [%s] V: [%d]", hash.c_str(), v);
    } else if(key > -1) {

        DL_INFO(1, "Get hash for file. key: [%d] hash: [%s]",
                key, hash.c_str());

//        DPN_FileTransportHandler *h = __env.client()->receivingFile(key);
//        if(h) {
//            DL_INFO(1, " >>> transport: [%p]", h);
//            // check hashsum after transmiting to verify file
//        }
//        DFile file = __env.client()->remoteFile(key);


//        if(!BADFILE(file)) {



//            std::string host_hash;
//            int r = 0;
//            r = __env.hashtool()->hash_file(file.path_c(), 1024 * 4);
//            host_hash = __env.hashtool()->get();

//            DL_INFO(1, " >>> file: [%p] path: [%s] hosthash: [%s] r: [%d]",
//                    file.descriptor(), file.path_c(), host_hash.c_str(), r
//                    );

//            bool v = bool(host_hash == hash);
//            DL_INFO(1, "HASH: ORIGINAL: [%s] ON HOST: [%s] v: [%d]",
//                    hash.c_str(), host_hash.c_str(), v
//                    );
////            // hashsum for file...
//        }

    } else {
        DL_ERROR(1, "Undefined key: [%d]", key);
        return false;
    }


    std::cout << "========================================================="
              << std::endl;

    */

    return DPN_SUCCESS;
}
//====================================================================================== DPN_Processor__transmit_answer:
DPN_Processor__transmit_answer::DPN_Processor__transmit_answer() {
    DL_INFO(1, "create: [%p]", this);
}
DPN_Processor__transmit_answer::~DPN_Processor__transmit_answer() {
    DL_INFO(1, "destruct: [%p]", this);
}
DPN_Result DPN_Processor__transmit_answer::sendPrefix() {
    INFO_LOG;
    return proccesSentAnswer();
}
DPN_Result DPN_Processor__transmit_answer::receiveReaction() {
    INFO_LOG;
    return proccesReceivedAnswer();
}
DPN_Result DPN_Processor__transmit_answer::proccesSentAnswer() {



    auto pFileSystem = extractFileModule(*clientContext.modules());
    if( pFileSystem == nullptr ) {
        DL_BADPOINTER(1, "File system");
        return DPN_FAIL;
    }

    auto a = UNIT_ANSWER_TYPE.get();
    switch (a) {
    case DPN_ANSWER__NEED_RESYNC: {


        pFileSystem->unverifyHost2Remote();

        // autosync:
        send(processor<DPN_PACKETTYPE__SYNC>());
        break;
    }
    default: DL_WARNING(1, "Undefined type: [%d]", a); break;
    }

    return DPN_SUCCESS;
}
DPN_Result DPN_Processor__transmit_answer::proccesReceivedAnswer() {

    auto pFileSystem = extractFileModule(*clientContext.modules());
    if( pFileSystem == nullptr ) {
        DL_BADPOINTER(1, "File system");
        return DPN_FAIL;
    }

    auto a = UNIT_ANSWER_TYPE.get();
    DPN_ANSWER_SUBTYPE subType = UNIT_ANSWER_SUBTYPE.get();
    auto post_reaction = processor<DPN_PACKETTYPE__TRANSMIT_ANSWER>();



    switch (a) {
    case DPN_ANSWER__REMOTE_CATALOG_VERIFIED: {
        bool v = pFileSystem->verifyRemote2Host(UNIT_STATE.get());
        DL_INFO(1, "Catalog verification: [%d]", v);
        break;
        }
    case DPN_ANSWER__READY_RECEIVE: {

        int server_note = UNIT_VALUE1.get();
        DArray<int> finalKeyset = UNIT_KEYSET.get();

        DPN_RequestNote *note = pFileSystem->getRequestNote(server_note);
        if( note == nullptr ) {
            DL_ERROR(1, "No note [%d]", server_note);
            return DPN_FAIL;
        }
        if( finalKeyset.empty() ) {
            DL_ERROR(1, "Empty keyset");
            post_reaction->setType(DPN_ANSWER__NO_DATA);
            post_reaction->setValue1(note->remoteNote());
            goto fail;
        }
        if( pFileSystem->hasSpecialChannels() ) {

            DL_INFO(1, "Start send keyset [%d] through SPECIAL channel", finalKeyset.size());
            pFileSystem->send(finalKeyset);

        } else {

            DL_INFO(1, "Start send keyset [%d] through MAIN channel", finalKeyset.size());

            auto proc = processor<DPN_PACKETTYPE__FILE_PACKET>();
            proc->setKeyset(finalKeyset, server_note);
            send(proc);
        }

        break;
    }
    case DPN_ANSWER__NEED_RESYNC: {
        DL_INFO(1, "Need resync");
        int server_note = UNIT_VALUE1.get();

        pFileSystem->unverifyRemote2Host();
        if( subType == DPN_ANSUB__FILE_SENDING ) {
            DL_INFO(1, "Stop receive files from note [%d]", server_note);
            pFileSystem->stopReceive(server_note);
        }
        break;
    }
    case DPN_ANSWER__NO_DATA: {

        auto as = UNIT_ANSWER_SUBTYPE.get();
        int server_note = UNIT_VALUE1.get();
        DArray<int> keyset = UNIT_KEYSET.get();

        switch (as) {

        case DPN_ANSUB__FILE_RECEIVING:
            DL_INFO(1, "No data to receive");
            if( keyset.empty() ) {
                pFileSystem->stopSend(server_note);
            } else {
                pFileSystem->stopSendFileset(keyset);
            }
            break;
        case DPN_ANSUB__FILE_SENDING:
            DL_INFO(1, "No data to send");
            if( keyset.empty() ) {
                pFileSystem->stopReceive(server_note);
            } else {
                pFileSystem->stopReceiveFileset(keyset);
            }
            break;
        default:
            DL_ERROR(1, "Undefined sub type: [%d]", as);
            break;
        }

        break;
    }
    case DPN_ANSWER__CHANNEL_RESERVE: {

        std::string shadowKey = UNIT_HASH.get();
        bool state = UNIT_STATE.get();
        if( state == false ) {

            int v = UNIT_VALUE1.get();
            DL_WARNING(1, "DPN_ANSWER__CHANNEL_RESERVE: Server can't reserve special channel. shadow key: [%s] d: [%d]", shadowKey.c_str(), v);

            switch (v) {
            case DPN_FORWARD:
                pFileSystem->unsreserveReceiver(shadowKey);
                break;
            case DPN_BACKWARD:
                pFileSystem->unsreserveSender(shadowKey);
                break;
            default:
                DL_INFO(1, "DPN_ANSWER__CHANNEL_RESERVE: Undefined direction");
                break;
            }

            return DPN_FAIL;
        }
        DPN_Channel *channel = clientContext.client()->channel(shadowKey);
        if( channel == nullptr ) {
            DL_ERROR(1, "DPN_ANSWER__CHANNEL_RESERVE: No channel");
            post_reaction->setType(DPN_ANSWER__CHANNEL_RESERVE);
            post_reaction->setHash(shadowKey);
            post_reaction->setState(false);
            post_reaction->setValue1(DPN_FORWARD);
            goto fail;
        }
        if( pFileSystem->reserveSender(channel) == false ) {
            DL_FUNCFAIL(1, "reserveFileSender");
            post_reaction->setType(DPN_ANSWER__CHANNEL_RESERVE);
            post_reaction->setHash(shadowKey);
            post_reaction->setState(false);
            post_reaction->setValue1(DPN_FORWARD);
            goto fail;
        }
        clientContext.core()->replanDirections();
        DL_INFO(1, "channel finded [%p] and reserve as sender", channel);
        break;
    }
    default:
        DL_INFO(1, "Undefined answer type: [%d]", a);
        break;
    }
    return DPN_SUCCESS;
fail:
    send(post_reaction);
    return DPN_FAIL;
}
//====================================================================================== DPN_Processor__register_fast_file:
DPN_Result DPN_Processor__register_fast_file::sendPrefix() {

    INFO_LOG;

    if( pFileSystem == nullptr ) {
        DL_BADPOINTER(1, "File system");
        return DPN_FAIL;
    }

    if( pFileSystem->registerLocalFile(file) == false ) {
        DL_ERROR(1, "Can't register fast file in local system");
        return DPN_FAIL;
    }
    if( BADKEY(file.key()) ) {
        DL_ERROR(1, "Bad key: [%d]", file.key());
        return DPN_FAIL;
    }
    UNIT_FILEKEY = file.key();
    UNIT_FILENAME = file.name();
    UNIT_FILESIZE = file.size();

    DL_INFO(1, "Register fast file: [%s] size: [%d] key: [%d] path: [%s]", file.name_c(), file.size(), file.key(), file.path_c());

    return DPN_SUCCESS;
}
DPN_Result DPN_Processor__register_fast_file::receiveReaction() {

    INFO_LOG;

    if( pFileSystem == nullptr ) {
        DL_BADPOINTER(1, "File system");
        return DPN_FAIL;
    }

    int key = UNIT_FILEKEY.get();
    size_t size = UNIT_FILESIZE.get();
    std::string name = UNIT_FILENAME.get();


    DL_INFO(1, "try register fast file on server: key: [%d] name: [%s] size: [%d]",
            key,
            name.c_str(),
            size);

    if( BADKEY(key) ) {
        DL_BADVALUE(1, "key: [%d]", key);
        sendAnswer(DPN_ANSWER__NO_DATA, DPN_ANSUB__FILE_SENDING, -1, key);
        return DPN_FAIL;
    }
    DFile file(name, size);
    if( pFileSystem->registerRemoteFile(file, key) == false ) {
        DL_FUNCFAIL(1, "registerRemoteFile");
        sendAnswer(DPN_ANSWER__NO_DATA, DPN_ANSUB__FILE_SENDING, -1, key);
        return DPN_FAIL;
    }

    DL_INFO(1, "Fast file [%d] registered on server", key);

    auto proc = CT_PROC(DPN_PACKETTYPE__REQUEST_FILE);;
    proc->appendFile(key);
    send(proc);

    return DPN_SUCCESS;
}
// ========================================== REQUEST =====================================================================
//====================================================================================== DPN_Processor__request_file:
            // HOST
DPN_Result DPN_Processor__request_file::sendPrefix() {

    INFO_LOG;

    if( pFileSystem == nullptr ) {
        DL_BADPOINTER(1, "File system");
        return DPN_FAIL;
    }

    int key = 0;
    size_t shift = 0;
    DArray<__file_metadata> md_array;
    DPN_FileTransportHandler *h = nullptr;
    bool catalogDepended = false;
    std::string __hash;


    int note_key = pFileSystem->makeReceiveNote(__keyset);
    if( note_key == -1 ) {
        DL_FUNCFAIL(1, "makeReceiveNote");
        goto fail;
    }
    if(__keyset.size() == 1) {
        strictMode = DPN_STRICT;
    }


    FOR_VALUE(__keyset.size(), i) {

        __file_metadata md;
        key = __keyset[i];
        DFile file = pFileSystem->remoteFile(key);
        if( catalogDepended == false && file.parent() ) catalogDepended = true;

        if( BADFILE(file) ) {
            DL_BADVALUE(1, "file: key: [%d]", key);
            goto fail;
        }
        //-------------------------------------------------------------------------------------------------
        if( pFileSystem->receivingFile(key) ) {
            DL_ERROR(1, "File already receiving: [%d]", key);
            continue;
        }
        //-------------------------------------------------------------------------------------------------
        DL_INFO(1, "check actual size: path: [%s]", file.path().c_str());
        shift = get_file_size(file.path());
        if(shift) {
            DPN_SHA256 *ht = threadContext.hashtool();
            int r;
            if( (r = ht->hash_file(file.path().c_str(), 1024 * 4)) < 0 ) {
                DL_FUNCFAIL(1, "hash_file: [%d]", r);
                goto fail;
            }
            md.shift_hash = ht->get();
        }
        //-------------------------------------------------------------------------------------------------
        if( shift == file.size() ) {
            DL_INFO(1, "File [%d] already received: hash: [%s]", key, md.shift_hash.c_str());
        } else {
            DL_INFO(1, "ready receive file: [%d]", key);
            h = pFileSystem->startReceiveFile(key);
            if( h == nullptr ) {
                DL_FUNCFAIL(1, "startReceiveRemoteFile [%d]", key);
                goto fail;
            }
            h->setShift(shift);
            if( strictMode == DPN_STRICT || (strictMode == DPN_STRICT_SELECTIVE && strictMask.contain(key)) ) {
                md.strictable = true;
                h->setStrictable(true);
            }
        }
        //-------------------------------------------------------------------------------------------------
        md.name = file.name();
        md.size = file.size();
        md.shift = shift;
        md.key = __keyset[i];



//        DL_INFO(1, "Ready receive file: key: [%d] name: [%s]", key, md.name.c_str());

        md_array.push_back(md);
    }
    if( md_array.empty() ) {
        DL_ERROR(1, "No files to receive");
        goto fail;
    }


    if( catalogDepended )
        UNIT_HOST_CATALOG_V = pFileSystem->remote().getHash(*threadContext.hashtool());
    else
        UNIT_HOST_CATALOG_V.clearBuffer();

    __hash = UNIT_HOST_CATALOG_V.get();
    DL_INFO(1, "catalogDepended: [%d] hash: [%s]", catalogDepended, __hash.c_str());


    UNIT_FILESET = md_array;
    UNIT_NOTE = note_key;

    DL_INFO(1, "Successful host file requesting. Host note: [%d] md_array: [%d]", note_key, md_array.size());

    return DPN_SUCCESS;
fail:
    pFileSystem->stopReceive(note_key);
    return DPN_FAIL;
}
            // SERVER
DPN_Result DPN_Processor__request_file::receiveReaction() {

    INFO_LOG;

    if( pFileSystem == nullptr ) {
        DL_BADPOINTER(1, "File system");
        return DPN_FAIL;
    }

    DArray<__file_metadata> md_array = UNIT_FILESET.get();
    DPN_FileTransportHandler *h = nullptr;


    std::string hostCatalogV;
    std::string serverCatalogV;
    bool catalogVersionVerified = false;

    fileinfo_set.clear();

    int host_note = UNIT_NOTE.get(); //remote
    int server_note = -1; // local




    auto proc = CT_PROC(DPN_PACKETTYPE__FILE_TRANSMIT_PREPARE);
    DPN_ANSWER_TYPE faultAnswerType = DPN_ANSWER__NEED_RESYNC;
    DPN_ANSWER_SUBTYPE faultSubAnswerType = DPN_ANSUB__FILE_SENDING;

    if( host_note < 0 ) {
        DL_BADVALUE(1, "host note");
        goto fail;
    }
    if( md_array.empty() ) {
        DL_ERROR(1, "Empty md_array");
        goto fail;
    }
    __keyset.clear();
    FOR_VALUE(md_array.size(), i) {
        __keyset.push_back(md_array[i].key);
    }
    server_note = pFileSystem->makeSendNote(__keyset);
    if( server_note == -1 ) {
        DL_FUNCFAIL(1, "makeSendNote");
        goto fail;
    }
    if( pFileSystem->bindRemoteNote(server_note, host_note) == false ) {
        DL_FUNCFAIL(1, "bindRemoteNote");
        goto fail;
    }

    FOR_VALUE(md_array.size(), i) {

        __file_metadata md = md_array[i];

        int key = md.key;
        size_t shift = md.shift;
        size_t size = md.size;
        std::string name = md.name;
        std::string shift_hash = md.shift_hash;
        bool strictable = md.strictable;


//        DL_INFO(1, "Process file requesting: key: [%d] shift: [%d] size: [%d] name: [%s]",
//                key, shift, size, name.c_str());

        //----------------------------------------------------------------------------------
        // Check transmiting state:
        if( pFileSystem->sendingFile(key) ) {
            DL_INFO(1, "File already sending: [%d]", key);
            continue;
        }
        // [Critical check] Check key:
//        key = -1;
        const DFile &file = pFileSystem->hostFile(key);
        if( BADFILE(file) ) {
            DL_ERROR(1, "Bad key: [%d]", key);
            goto fail;
        }
        if( !catalogVersionVerified && file.parent() ) { // catalog depended

            if( serverCatalogV.empty() ) serverCatalogV = pFileSystem->host()->getHash(*threadContext.hashtool());
            if( serverCatalogV.empty() ) {
                DL_ERROR(1, "Can't get catalog hash");
                goto fail;
            }
            if( hostCatalogV.empty() ) hostCatalogV = UNIT_HOST_CATALOG_V.get();
            if( hostCatalogV.empty() ) {
                DL_ERROR(1, "Empty host catalog hash");
                goto fail;
            }
            if( serverCatalogV != hostCatalogV ) {
                DL_ERROR(1, "Different catalogs versiongs: server: [%s] host: [%s]",
                         serverCatalogV.c_str(), hostCatalogV.c_str());
                goto fail;
            }
            DL_INFO(1, "catalog verified!");
            catalogVersionVerified = true;
        }
        // [Critical check] Check name and size:
        if( name != file.name() ) {
            DL_ERROR(1, "file: [%d] Sync error: wrong name: local: [%s] remote: [%s]", key, file.name().c_str(), name.c_str());
            goto fail;
        }
        if( size != file.size() ) {
            DL_ERROR(1, "file: [%d] Sync error: wrong size: local: [%d] remote: [%d]", key, size, file.size());
            goto fail;
        }
        // [Critical check] Check file existing:
        size_t actual_size = get_file_size(file.path());
        if(actual_size == 0) {
            DL_ERROR(1, "No file data. key: [%d] path: [%s]", key, file.path_c());
            goto fail;
        }
        //----------------------------------------------------------------------------------
        if(actual_size == size) {

            if(shift) {

                std::string actual_shift_hash;

                int r = 0;
                if( (r = threadContext.hashtool()->hash_file(file.path().c_str(), 1024 * 4, shift)) < 0 ) {
                    DL_FUNCFAIL(1, "hash_file: [%d]", r);
                    goto fail;
                }
                actual_shift_hash = threadContext.hashtool()->get();


                if(actual_shift_hash == shift_hash) {
                    if(shift == size) {
                        DL_INFO(1, "File [%d] already transmited: size: [%d] and has same hash sum: [%s]", key, size, shift_hash.c_str());
//                        continue;
                    } else {
                        DL_INFO(1, "Ready to send file [%d] from shift byte: [%d]", key, shift);
                    }
                    // shift = shift
                } else {
                    shift = 0;
                    DL_INFO(1, "Data changed, transmit all data from first byte. file: [%d]", key);
                }
            } else {
                // shift = 0
                DL_INFO(1, "Ready to send all file from first byte: file: [%d]", key);
            }
        } else {
            DL_ERROR(1, "File data changed. key: [%d] path: [%s] old size: [%d] current size: [%d]",
                     key, file.path_c(), size, actual_size);

            goto fail;
        }
        //----------------------------------------------------------------------------------

        if( shift != size ) {

            h = pFileSystem->startSendFile(key);
            if(h == nullptr) {
                DL_FUNCFAIL(1, "startSendHostFile");
                goto fail;
            }
            h->setStrictable(strictable);
            h->setShift(shift);

        }


        __tfi info;
        info.key = key;
        info.shift = shift;
        info.verified = bool(shift == size);
        fileinfo_set.append(info);
    }
    if(fileinfo_set.empty()) {
        DL_ERROR(1, "Empty transport set");
        faultAnswerType = DPN_ANSWER__NO_DATA;
        goto fail;
    }

    DL_INFO(1, "Ready to transmit prepare: transport set: [%d] host note: [%d] server note: [%d]",
            fileinfo_set.size(), host_note, server_note
            );


    proc->setSource(fileinfo_set, server_note);
    send(proc);

    return DPN_SUCCESS;
fail:
    pFileSystem->stopSend(server_note);
    sendAnswer(faultAnswerType, faultSubAnswerType, host_note);
    return DPN_FAIL;
}
// ========================================== PREPARE =====================================================================
//====================================================================================== DPN_Processor__file_transmit_prepare:
            // SERVER
DPN_Result DPN_Processor__file_transmit_prepare::sendPrefix() {

    INFO_LOG;

    if( pFileSystem == nullptr ) {
        DL_BADPOINTER(1, "File system");
        return DPN_FAIL;
    }

    int server_note = note;
    DPN_RequestNote *pNote = pFileSystem->getRequestNote(server_note);

    if( pNote == nullptr ) {
        DL_ERROR(1, "Bad serve note");
        return DPN_FAIL;
    }
    int host_note = pNote->remoteNote();

    DL_INFO(1, "notes: server: [%d] host: [%d]", server_note, host_note);

    FOR_VALUE(fileinfo_set.size(), i) {

        int key = fileinfo_set[i].key;
        if( fileinfo_set[i].verified ) {
            DL_INFO(1, "File [%d] already transmited and verified by hash, skip", key );
            pFileSystem->unregisterFastFile(key, true);
        } else {

            auto h = pFileSystem->sendingFile(key);
            if( h == nullptr ) {
                DL_ERROR(1, "No sending file [%d]", key);
                goto fail;
            }
            DPN_FileSlice *mainSlice =  h->createMainSlice();
            if( mainSlice == nullptr ) {
                DL_FUNCFAIL(1, "createMainSlice: file: [%d]", key);
                goto fail;
            }
            if( mainSlice->openR() == false ) {
                if( h->strictable() ) {
                    DL_FUNCFAIL(1, "(strictable) openR key: [%d]", key);
                    goto fail;
                }
            }
            if( mainSlice->start() == false ) {
                if( h->strictable() ) {
                    DL_FUNCFAIL(1, "(strictable) start key: [%d]", key);
                    goto fail;
                }
            }
            DPN_SETTINGS_MARK
            mainSlice->setReadWindow(1024 * 16);
        }
    }
    UNIT_FILE_INFO = fileinfo_set;
    UNIT_SERVER_NOTE = server_note;
    UNIT_HOST_NOTE = host_note;

    return DPN_SUCCESS;

fail:
    pFileSystem->stopSend(server_note);

    // send error answer
    sendAnswer(DPN_ANSWER__SERVER_ERROR, DPN_ANSUB__TRANSMIT_PREPARE, host_note);

    return DPN_FAIL;
}
            // HOST
DPN_Result DPN_Processor__file_transmit_prepare::receiveReaction() {
    INFO_LOG;


    if( pFileSystem == nullptr ) {
        DL_BADPOINTER(1, "File system");
        return DPN_FAIL;
    }

    fileinfo_set = UNIT_FILE_INFO.get();
    int server_note = UNIT_SERVER_NOTE.get();
    int host_note = UNIT_HOST_NOTE.get();
    auto proc = processor<DPN_PACKETTYPE__TRANSMIT_ANSWER>();
    DArray<int> finalKeyset;

    DPN_RequestNote *pNote = pFileSystem->getRequestNote(host_note);
    if( pFileSystem->bindRemoteNote(host_note, server_note) == false ) {
        DL_FUNCFAIL(1, "bindRemoteNote");
        return DPN_FAIL;
    }
    if( pNote == nullptr ) {
        DL_ERROR(1, "No [%d] note", host_note);
        return DPN_FAIL;
    }
    if( fileinfo_set.empty() ) {
        DL_ERROR(1, "Empty fileinfo_set");
        goto fail;
    }

    FOR_VALUE(fileinfo_set.size(), i) {

        int key = fileinfo_set[i].key;
        size_t shift = fileinfo_set[i].shift;
        if( fileinfo_set[i].verified ) {
            DL_INFO(1, "File's data [%d] verified on server!", key);
            pFileSystem->unregisterFastFile(key, false);
            continue;
        }
        DPN_FileTransportHandler *h = pFileSystem->receivingFile(key);
        if(h == nullptr) {
            DL_FUNCFAIL(1, "No receiving file: [%d]", key);
            return DPN_FAIL;
        }
        h->setShift(shift);

        DPN_FileSlice *mainSlice = h->createMainSlice();
        if( mainSlice == nullptr ) {
            DL_FUNCFAIL(1, "createMainSlice: key: [%d]", key);
            goto fail;
        }
        if( mainSlice->openW() == false ) {
            if( h->strictable() ) {
                DL_FUNCFAIL(1, "openW: key: [%d]", key);
                goto fail;
            }
        }
        if( mainSlice->start() == false ) {
            if( h->strictable() ) {
                DL_FUNCFAIL(1, "start: key: [%d]", key);
                goto fail;
            }
        }

//        DL_INFO(1, "append [%d] key to final keyset",
//                key);
        finalKeyset.append(key);
    }
    if( finalKeyset.empty() ) {
        DL_INFO(1, "No files to receive");
        return DPN_SUCCESS;
    }

    proc->setType(DPN_ANSWER__READY_RECEIVE);
    proc->setValue1(server_note);
    proc->setKeyset(finalKeyset);
    send(proc);

    return DPN_SUCCESS;
fail:
    pFileSystem->stopReceive(host_note);
    sendAnswer(DPN_ANSWER__HOST_ERROR, DPN_ANSUB__TRANSMIT_PREPARE, server_note);
    return DPN_FAIL;
}
// ========================================== TRANSMIT =====================================================================
//====================================================================================== DPN_Processor__file_packet:
            // SERVER
DPN_Result DPN_Processor__file_packet::sendPrefix() {

//    INFO_LOG;

    if( sessionSize == 0 ) {
        if( __preapre(keyset) == false ) {
            DL_FUNCFAIL(1, "prepare");
            sendAnswer(DPN_ANSWER__NO_DATA, DPN_ANSUB__FILE_SENDING, note);
            return DPN_FAIL;
        }
    }
    if( (current_slice = session.getActual()) == nullptr ) {
        DL_INFO(1, "Empty session");
        sendAnswer(DPN_ANSWER__NO_DATA, DPN_ANSUB__FILE_SENDING, note);
        return DPN_FAIL;
    }
    //-------------------------------------------------------------------

    UNIT_FILE_KEY = current_slice->key();
    UNIT_DATA.clearBuffer();
    UNIT_DATA.reserve(sizeof(uint32_t));
    current_slice->read(UNIT_DATA.buffer());


    if( (packetSize = current_slice->loadLast()) < 0 ) {
        DL_FUNCFAIL(1, "file2buffer");
        sendAnswer(DPN_ANSWER__NO_DATA, DPN_ANSUB__FILE_SENDING, note);
        return DPN_FAIL;
    }
    UNIT_DATA.data32()[0] = packetSize;

    return DPN_SUCCESS;
}
DPN_Result DPN_Processor__file_packet::sendReaction() {

//    INFO_LOG;

    if( pFileSystem == nullptr ) {
        DL_BADPOINTER(1, "File system");
        return DPN_FAIL;
    }

    if( current_slice == nullptr ) {
        DL_INFO(1, "Empty session");
        return DPN_SUCCESS;
    }
    //------------------------------------------------------------------- suffix:
    current_slice->transportIt(packetSize);
    ++packetNumber;


    DL_INFO(1, "key: [%d] transported: [%d] packet: [%d] progress: [%f] speed: [%f]",
            current_slice->key(), current_slice->transportedBytes(), packetNumber, current_slice->progress(), current_slice->speed()
            );

    if(current_slice->isTransported()) {

        threadContext.hashtool()->hash_file(current_slice->base().path_c(), 1024 * 4);
        std::string hash = threadContext.hashtool()->get();

//        auto hash_proc = CT_PROC(DPN_PACKETTYPE__HASH_SUM);
//        hash_proc->setFileHashsum(hash, h->key());
//        env->putBackProcessor(hash_proc);


        int key = current_slice->key();




        DPN_FileTransportHandler *h = current_slice->desc();
        DL_INFO(1, ">>>>> 1 STOP SEND FILE: [%p] [%d]", h, h->key());

        if(h->isFast()) {
            pFileSystem->unregisterFile(h->key());
        }
        DL_INFO(1, ">>>>> 2 STOP SEND FILE: [%p] [%d]", h, h->key());
        pFileSystem->stopSendFile(h->key());




        DPN_TimeRange range = current_slice->range();
        DL_INFO(1, "File sent at all: [%d] hash: [%s] time: [%d]",
                key, hash.c_str(), range.seconds()
                );

        current_slice->stop();
        session.popActual();
        if( --sessionSize == 0 ) {
            return DPN_SUCCESS;
        } else {
            return DPN_REPEAT;
        }
    } else {
        session.skip();
        return DPN_REPEAT;
    }

    return DPN_SUCCESS;
}
            // HOST
DPN_Result DPN_Processor__file_packet::receiveReaction() {

//    INFO_LOG;

    if( pFileSystem == nullptr ) {
        DL_BADPOINTER(1, "File system");
        return DPN_FAIL;
    }

    __raw_data __data;

    int key = UNIT_FILE_KEY.get();
    __data = UNIT_DATA.getData();


    packetSize = __data.s;

//    DL_INFO(1, "receive packet: [%d] key: [%d]", __data.s, key);

    DPN_FileTransportHandler *h = nullptr;
    h = pFileSystem->receivingFile(key);
    if(h == nullptr) {
        DL_ERROR(1, "File not receiving. key: [%d] incoming packet size: [%d]", key, packetSize);
        sendAnswer(DPN_ANSWER__NO_DATA, DPN_ANSUB__FILE_RECEIVING, -1, key);
        return DPN_FAIL;
    }
    DPN_FileSlice *mainSlice = h->getMainSlice();
    if( mainSlice == nullptr ) {
        DL_ERROR(1, "No main slice");
        sendAnswer(DPN_ANSWER__NO_DATA, DPN_ANSUB__FILE_RECEIVING, -1, key);
        return DPN_FAIL;
    }
    if( mainSlice->transportIt(packetSize) == false ) {
        DL_ERROR(1, "transport error: packet size: [%d] packet: [%d]", packetSize, packetNumber);
        sendAnswer(DPN_ANSWER__NO_DATA, DPN_ANSUB__FILE_RECEIVING, -1, key);
        return DPN_FAIL;
    }

    ++packetNumber;

    int w = mainSlice->write(__data.d, __data.s);

    DL_INFO(1, "KEY: [%d] transported: [%d] packet: [%d]",
            key,
            mainSlice->transportedBytes(),
            packetNumber
            );

    if( mainSlice->isTransported() ) {


        DFile f = h->base();
        mainSlice->stop();
        pFileSystem->stopReceiveFile(key);


        threadContext.hashtool()->hash_file(f.path_c(), 1024 * 4);
        std::string hash = threadContext.hashtool()->get();

        DL_INFO(1, " >>> File received <<< key: [%d] transported: [%d] hash: [%s]",
                key,
                mainSlice->transportedBytes(),
                hash.c_str()
                );


    }


    return DPN_SUCCESS;
}

bool DPN_Processor__file_packet::__preapre(const DArray<DFileKey> &keyset) {

    DL_INFO(1, "try fill session... [%d]", keyset.size());

    if( pFileSystem == nullptr ) {
        DL_BADPOINTER(1, "File system");
        return DPN_FAIL;
    }

    if( keyset.empty() ) {
        DL_ERROR(1, "Empty keyset");
        return false;
    }
    FOR_VALUE( keyset.size(), i ) {

        DPN_FileTransportHandler *h = pFileSystem->sendingFile(keyset[i]);
        if( h == nullptr ) {
            DL_ERROR(1, "No sending file with key: [%d]", keyset[i]);
            session.clear();
            sessionSize = 0;
            return false;
        }
        DL_INFO(1, "key: [%d] h: [%p]", keyset[i], h);

        if( h->shift() == h->size() ) {
            DL_INFO(1, "File [%d] transmited and verified, skip", h->key());
            continue;
        }

        DPN_FileSlice *slice = h->getMainSlice();
        if( slice == nullptr ) {
            DL_ERROR(1, "No main slice: key: [%d]", keyset[i]);
            session.clear();
            sessionSize = 0;
            return false;
        }

        session.push_back(slice);
        ++sessionSize;
    }

    DL_INFO(1, "Create session [%d]", sessionSize);

    return true;
}
//====================================================================================== DPN_Processor__fileset_packet:


DPN_FileProcessor::DPN_FileProcessor() {
    pReaction = nullptr;
    pFileSystem = nullptr;
}
void DPN_FileProcessor::postInition() {
    pFileSystem = extractFileModule(*clientContext.modules());
}
void DPN_FileProcessor::createReaction() {

    if( (pReaction = processor<DPN_PACKETTYPE__TRANSMIT_ANSWER>()) == nullptr ) {
        DL_ERROR(1, "Can't get processor");
        return;
    }
}
void DPN_FileProcessor::sendAnswer(DPN_ANSWER_TYPE t, DPN_ANSWER_SUBTYPE st, int local_note, int file_key)  {

    if( pFileSystem == nullptr ) {
        DL_BADPOINTER(1, "File system");
        return;
    }

    if(pReaction == nullptr) createReaction();

    if( file_key > -1 ) {
        pReaction->addFileKey(file_key);
        pReaction->setValue1(-1);
    } else {
        DPN_RequestNote *note = pFileSystem->getRequestNote(local_note);
        if( note ) {
            pReaction->setValue1(note->remoteNote());
        }
    }
    pReaction->setType(t);
    pReaction->setSubType(st);

    send(pReaction);
}


