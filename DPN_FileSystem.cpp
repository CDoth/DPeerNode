#include "DPN_FileSystem.h"
#include "DPN_FileSystemProcessors.h"

/*
DPN_RequestNote::DPN_RequestNote() {
    direciton = NO_DIRECTION;
    done = false;
    remote = -1;
}
DPN_FileSystem::DPN_FileSystem() : DPN_AbstractModule("FileSystem"), remote_catalog("Remote Catalog") {

    host_catalog = nullptr;
    host2remote_verified = false;
    remote2host_verified = false;
    autoreceive_fast_files = true;
    clientFileBufferSize = DPN_DEFAULT_CLIENT_BUFFER_SIZE;

    remote_catalog.createMapFileSystem();
//    DL_INFO(1, "CREATE FILE SYSTEM: [%p]", this);
}
DPN_FileSystem::~DPN_FileSystem() {

    //    DL_INFO(1, "1DELETE FILE SYSTEM: [%p]", this);
}
bool DPN_FileSystem::activateChannel(DPN_DuplexChannel *c, DPN_DirectionType dt, const DPN_ExpandableBuffer &extra) {

}

void DPN_FileSystem::channelActived(DPN_DuplexChannel *c, DPN_DirectionType dt)
{

}
void DPN_FileSystem::setHostCatalog(DPN_Catalog *catalog) {
    host_catalog = catalog;
}
//bool DPN_FileSystem::reserveShadowSender(DPN_Channel *channel, int transaction) {
//    return reserveSender(channel);
//}
//bool DPN_FileSystem::reserveShadowReceiver(DPN_Channel *channel, const DPN_ExpandableBuffer &extra, int transaction) {
//    return reserveReceiver(channel);
//}
void DPN_FileSystem::clientDisconnected(const DPN_AbstractClient *client) {

}
void DPN_FileSystem::stop() {

}
bool DPN_FileSystem::checkRemoteFile(DFileKey key) {

    if(remote2host_verified == false) {
        DL_ERROR(1, "Unverified remote catalog. Need sync");
        return false;
    }
    if(remote_catalog.isValidKey(key) == false) {
        DL_ERROR(1, "Can't find file with key: [%d]", key);
        return false;
    }

    return true;
}
void DPN_FileSystem::unverifyRemote2Host() {

    remote2host_verified = false;
}
void DPN_FileSystem::unverifyHost2Remote() {
    host2remote_verified = false;
}
bool DPN_FileSystem::verifyHost2Remote(const std::string &remoteHashSum) {
    if(host_catalog == nullptr) {
        DL_BADPOINTER(1, "host_catalog");
        return false;
    }
    std::string host_hash = host_catalog->getHash(hashtool);
    bool status = bool(host_hash == remoteHashSum);

//    DL_INFO(1, "status: [%d]", status);
    host2remote_verified = status;
    return host2remote_verified;
}
bool DPN_FileSystem::verifyRemote2Host(bool status) {
    remote2host_verified = status;
    return remote2host_verified;
}
void DPN_FileSystem::setDownloadPath(const std::string &path) {
    __downloadPath = path;
}
DFile DPN_FileSystem::hostFile(DFileKey key) {
    return host_catalog->file(key);
}
const DFile &DPN_FileSystem::hostFile(DFileKey key) const {
    return host_catalog->constFile(key);
}
DFile DPN_FileSystem::remoteFile(DFileKey key) {
    return remote_catalog.file(key);
}
const DFile &DPN_FileSystem::remoteFile(DFileKey key) const {
    return remote_catalog.constFile(key);
}
DPN_FileTransportHandler *DPN_FileSystem::startReceiveFile(int key) {

    DFile file = remote_catalog.file(key);
    if(BADFILE(file)) {
        DL_ERROR(1, "Can't find file with key: [%d]", key);
        return nullptr;
    }


    file.createVirtualPath(__downloadPath);

//    DL_INFO(1, "Virtual file ready: Name: [%s] Path: [%s]  Key: [%d] Size: [%d]",
//            file.name_c(), file.path_c(), file.key(), file.size());

    return mainSession.startReceive(file);
}
DPN_FileTransportHandler *DPN_FileSystem::startSendFile(int key) {

    DFile file = host_catalog->file(key);

    if(BADFILE(file)) {
        DL_ERROR(1, "Can't find file with key: [%d]", key);
        return nullptr;
    }

    return mainSession.startSend(file);
}
bool DPN_FileSystem::stopReceiveFileset(const DArray<DFileKey> &keyset) {

    if( unregisterFastFileset(keyset, false) == false ) {
        DL_FUNCFAIL(1, "unregisterFastFiles");
        return false;
    }
    return mainSession.stopReceive(keyset);
}
bool DPN_FileSystem::stopReceiveFile(DFileKey key) {

    if( unregisterFastFile(key, false) == false ) {
        DL_FUNCFAIL(1, "unregisterFastFile");
        return false;
    }
    return mainSession.stopReceive(key);
}
bool DPN_FileSystem::stopReceive(int note_key) {

    DPN_RequestNote *note = getRequestNote(note_key);
    if( note ) {
        if( note->direciton == DPN_RequestNote::RECEIVING ) {
            note->done = true;
            return stopReceiveFileset(note->keyset);
        } else {
            DL_ERROR(1, "Note [%d] is no receiving", note_key);
            return false;
        }
    }

    DL_WARNING(1, "No note with key: [%d]", note_key);
    return false;
}

bool DPN_FileSystem::stopSendFileset(const DArray<DFileKey> &keyset) {
    if( unregisterFastFileset(keyset, true) == false ) {
        DL_FUNCFAIL(1, "unregisterFastFiles");
        return false;
    }
    return mainSession.stopSend(keyset);
}
bool DPN_FileSystem::stopSendFile(DFileKey key) {

    if( unregisterFastFile(key, true) == false ) {
        DL_FUNCFAIL(1, "unregisterFastFile");
        return false;
    }

    return mainSession.stopSend(key);
}
bool DPN_FileSystem::stopSend(int note_key) {

    DPN_RequestNote *note = getRequestNote(note_key);
    if( note ) {
        if( note->direciton == DPN_RequestNote::SENDING ) {
            note->done = true;
            return stopSendFileset(note->keyset);
        } else {
            DL_ERROR(1, "Note [%d] is no sending", note_key);
            return false;
        }
    }

    DL_WARNING(1, "No note with key: [%d]", note_key);
    return false;
}
bool DPN_FileSystem::registerRemoteFile(DFile &file, DFileKey key) {
    if(BADFILE(file)) {
        DL_BADVALUE(1, "file");
        return false;
    }
    file.createAbsolutePath(__downloadPath);
    return remote_catalog.getFileSystem()->registerFile(file, key);
}
bool DPN_FileSystem::registerLocalFile(DFile &file) {
    if(BADFILE(file)) {
        DL_BADVALUE(1, "file");
        return false;
    }
    if(host_catalog == nullptr) {
        DL_BADPOINTER(1, "Host catalog");
        return false;
    }
    if( host_catalog->getFileSystem()->registerFile(file) == false ) {
        DL_FUNCFAIL(1, "registerFile");
        return false;
    }
    return true;
}
bool DPN_FileSystem::unregisterFile(DFileKey key) {
    return host_catalog->getFileSystem()->unregFile(key);
}
int DPN_FileSystem::makeRequestNote(DFileKey key, DPN_RequestNote::DIRECTION d) {

    if( BADKEY(key) ) return -1;
    if( d == DPN_RequestNote::NO_DIRECTION ) return -1;

    DPN_RequestNote note;
    note.keyset.clear();
    note.keyset.append(key);
    note.direciton = d;
    __requests.push_back(note);
    return __requests.size() - 1;
}
int DPN_FileSystem::makeRequestNote(const DArray<DFileKey> &keyset, DPN_RequestNote::DIRECTION d) {

    if( keyset.empty() ) return -1;
    if( d == DPN_RequestNote::NO_DIRECTION ) return -1;

    DPN_RequestNote note;
    note.keyset = keyset;
    note.direciton = d;
    __requests.push_back(note);
    return __requests.size() - 1;
}
DPN_RequestNote *DPN_FileSystem::getRequestNote(int note_key) {
    if( __requests.inRange( note_key ) ) {
        return &__requests[note_key];
    }
    return nullptr;
}
bool DPN_FileSystem::send(DFileKey key) {

    if( send_directions.empty() ) {
        DL_ERROR(1, "No send directions");
        return false;
    }
    DPN_FileTransportHandler *h = sendingFile(key);
    if( h == nullptr ) {
        DL_ERROR(1, "No sending file [%d]", key);
        return false;
    }

    if( false
//            h->isDistributingAvailable() && h->distributeChannels() > 1
            )
    {

        DArray<DPN_FileSlice*> sliceset;

//        int chs = h->distributeChannels();
        int chs = 0;
        if( chs > send_directions.size() ) {

            chs = send_directions.size();
            sliceset = h->createSlices(chs);
            if( sliceset.empty() || sliceset.size() != chs ) {
                DL_ERROR(1, "Can't create sliceset (1): directions: [%d] req channels: [%d] slices: [%d]",
                         send_directions.size(), chs, sliceset.size());
                return false;
            }

        } else {

            sliceset = h->createSlices(chs);
            if( sliceset.empty() || sliceset.size() != chs ) {
                DL_ERROR(1, "Can't create slices set (2): directions: [%d] req channels: [%d] slices: [%d]",
                         send_directions.size(), chs, sliceset.size());
                return false;
            }

        }

        FOR_VALUE( sliceset.size(), i ) {
            DPN_FileSendDirection *d = send_directions[0];
            FOR_VALUE( send_directions.size(), i ) {
                if( d->size() > send_directions[i]->size() ) {
                    d = send_directions[i];
                }
            }
            d->send(sliceset[i]);
        }

    } else {


        DPN_FileSendDirection *d = send_directions[0];
        FOR_VALUE( send_directions.size(), i ) {
            if( send_directions[i]->size() < d->size() ) {
                d = send_directions[i];
            }
        }
        DPN_FileSlice *slice = h->getMainSlice();
        if( slice == nullptr ) {
            DL_FUNCFAIL(1, "No main slice");
            return false;
        }

        DL_INFO(1, "DPN_FileSystem: Try send full slice [%p]", slice);

        d->send(slice);

    }

    return true;
}
bool DPN_FileSystem::unregisterFastFileset(const DArray<int> &keyset, bool onHost) {

    if( keyset.empty() ) return true;

    DPN_Catalog *cat = onHost ? host_catalog : &remote_catalog;
    if( cat == nullptr ) {
        DL_BADPOINTER(1, "catalog (side: [%d])", onHost);
        return false;
    }
    auto fs = cat->getFileSystem();
    if( fs == nullptr ) {
        DL_BADPOINTER(1, "file system (side: [%d])", onHost);
        return false;
    }
    // Unregister fast files (without catalog)
    FOR_VALUE( keyset.size(), i ) {
        DFile file = fs->file(keyset[i]);
        if( BADFILE(file) ) continue;
        if( file.parent() == nullptr ) {
            DL_INFO(1, "Unregister fast file: [%d]", keyset[i]);
            fs->unregFile(keyset[i]);
        }
    }

    return true;
}
bool DPN_FileSystem::unregisterFastFile(DFileKey key, bool onHost) {
    if( BADKEY(key) ) {
        DL_BADVALUE(1, "key: [%d]", key);
        return false;
    }
    DPN_Catalog *cat = onHost ? host_catalog : &remote_catalog;
    if( cat == nullptr ) {
        DL_BADPOINTER(1, "catalog (side: [%d])", onHost);
        return false;
    }
    auto fs = cat->getFileSystem();
    if( fs == nullptr ) {
        DL_BADPOINTER(1, "file system (side: [%d])", onHost);
        return false;
    }
    DFile file = fs->file(key);
    if( BADFILE(file) ) return true;
    if( file.parent() == nullptr ) {
        DL_INFO(1, "Unregister fast file: [%d]", key);
        fs->unregFile(key);
    }
    return true;
}
bool DPN_FileSystem::send(const DArray<DFileKey> &keyset) {

    if( keyset.empty() ) {
        DL_ERROR(1, "Empty keyset");
        return false;
    }
    FOR_VALUE( keyset.size(), i ) {
        if( send(keyset[i]) == false ) {
            DL_FUNCFAIL(1, "send for key: [%d]", keyset[i]);
        }
    }
    return true;
}
bool DPN_FileSystem::unsreserveSender(const std::string &shadowKey) {

//    FOR_VALUE( specials.size(), i ) {

//        if( specials[i]->checkKey(shadowKey) ) {
//            specials[i]->unreserveForward();
//            break;
//        }
//    }
    return true;
}
bool DPN_FileSystem::unsreserveReceiver(const std::string &shadowKey) {

//    FOR_VALUE( specials.size(), i ) {

//        if( specials[i]->checkKey(shadowKey) ) {
//            specials[i]->unreserveBack();
//            break;
//        }
//    }
    return true;
}

bool DPN_FileSystem::hasSpecialChannels() const {

//    FOR_VALUE( specials.size(), i ) {
//        if( specials[i]->isForwardReserved() ) return true;
//    }
    return false;
}
bool DPN_FileSystem::bindRemoteNote(int local_note, int remote_note) {
    if( __requests.inRange(local_note) ) {
        DPN_RequestNote &note = __requests[local_note];
        if( note.done ) {
            DL_ERROR(1, "Note [%d] already done", local_note);
            return false;
        }
        note.remote = remote_note;
        return true;
    }
    DL_BADVALUE(1, "local note: [%d] request: [%d]", local_note, __requests.size());
    return false;
}

//==================================================================================
DPN_FileDirection::DPN_FileDirection() {

    pFileSystem = nullptr;
    pConnector = nullptr;
    buffer.reserve(header_size);
}
DPN_FileSendDirection::DPN_FileSendDirection() {
    iRead_b = 0;
    iSize = 0;
    s_current = nullptr;

}
void DPN_FileSendDirection::send(DPN_FileSlice *slice) {

    if( slice == nullptr ) {
        DL_BADPOINTER(1, "slice");
        return;
    }

    DL_INFO(1, "push slice [%p]", slice);
    session__integrate(slice);
}
//void DPN_FileSendDirection::send(const DArray<DPN_FileSlice *> &sliceset) {

//    if( sliceset.empty() ) {
//        DL_ERROR(1, "Empty sliceset");
//        return;
//    }

//    DL_INFO(1, "push sliceset: [%d]", sliceset.size());
//    session__integrate(sliceset);
//}
bool DPN_FileSendDirection::proc() {



    if( pConnector == nullptr ) {
        DL_BADPOINTER(1, "connector");
        return false;
    }



    if( iRead_b == 0 ) {
        read();
    }

//    return true;


    if( iRead_b > 0 ) {

        DL_INFO(1, "Try send packet data: buffer: [%d] read: [%d]", buffer.size(), iRead_b);

        pConnector->x_send(buffer, header_size + iRead_b);

        switch ( pConnector->state() ) {
        case DPN_FAIL:
            DL_ERROR(1, "DPN_FileSendDirection: Connection fail");
            errorState = CONNECTION_FAIL;
            return false;
            break;
        case DPN_SUCCESS:

            DL_INFO(1, "sent bytes: [%d] ",
                    pConnector->transportedBytes()
                    );

            s_current->transportIt(iRead_b);

            if( s_current->isTransported() ) {
                DL_INFO(1, "File sent at all");

                pFileSystem->stopSendFile(s_current->key());
                s_current->stop();
                session__pop();
            } else {
                DL_INFO(1, "skip");
                session__skip();
            }

            iRead_b = 0;
            break;
        default: break;
        }
    }
    return true;
}
void DPN_FileSendDirection::read() {

    session__accept();

    if( (s_current = session__get()) == nullptr ) {
        iRead_b = 0;
        return;
    }


//    DL_INFO(1, "Get slice: [%p]", s_current);
//    return;

    buffer.dropTo(header_size);
    if( (iRead_b = s_current->read(buffer)) < 0 ) {
        DL_ERROR(1, "read: [%d]", iRead_b);
        return;
    }


    uint32_t *d = reinterpret_cast<uint32_t*>(buffer.getData());
    d[0] = iRead_b;
    d[1] = s_current->key();

    DL_INFO(1, "get current slice: [%p] read: [%d] key: [%d] buffer: [%d]",
            s_current, iRead_b, s_current->key(), buffer.size());

}
void DPN_FileSendDirection::session__integrate(DPN_FileSlice *slice) {

    if( slice == nullptr ) return;

    DPN_THREAD_GUARD(mu);
    queue.push_back(slice);
    ++iQueueSize;
}
void DPN_FileSendDirection::session__integrate(const DArray<DPN_FileSlice *> &sliceset) {
    if( sliceset.empty() ) return;
    DPN_THREAD_GUARD(mu);
    FOR_VALUE( sliceset.size(), i ) {

        if( sliceset[i] ) {
            queue.push_back(sliceset[i]);
            ++iQueueSize;
        }

    }
}
void DPN_FileSendDirection::session__accept() {
    if( queue.empty() ) return;
    DPN_THREAD_GUARD(mu);

    DL_INFO(1, "slices in queue: [%d]", iQueueSize);

    session.moveTo(queue);

    DL_INFO(1, "moved");

    iSize += iQueueSize;
    iQueueSize = 0;
}
void DPN_FileSendDirection::session__pop() {
    session.popActual();
    --iSize;
}
void DPN_FileSendDirection::session__skip() {
    session.skip();
}
DPN_FileSlice *DPN_FileSendDirection::session__get() {
    return session.getActual();
}

//=====================================================================================  DPN_FileReceiveDirection
DPN_FileReceiveDirection::DPN_FileReceiveDirection() {
    current = nullptr;
}
bool DPN_FileReceiveDirection::proc() {

    if( pConnector == nullptr ) {
        DL_BADPOINTER(1, "connector");
        return false;
    }
    if( pConnector->readable() == false ) return true;

    if( current == nullptr ) {
        DL_INFO(1, "receiving header...");

        pConnector->x_receiveValue(__header);
        switch (pConnector->state()) {
            case DPN_SUCCESS:

            if( __header.size == 0 ) {
                DL_ERROR(1, "Zero packet size");
                return false;
            }
            if( (current = pFileSystem->receivingFile(__header.key)) == nullptr ) {
                DL_ERROR(1, "No receiving file with key: [%d]", __header.key);
                return false;
            }


            DL_INFO(1, "header received: rb: [%d] size: [%d] key: [%d] handler: [%p]",
                    pConnector->transportedBytes(),
                    __header.size, __header.key, current);

                break;
            case DPN_FAIL:
            DL_ERROR(1, "DPN_FileReceiveDirection: Connection fail");
                errorState = CONNECTION_FAIL;
                return false;
            default: break;
        }
    }

    if( current && pConnector->readable() ) {

        DL_INFO(1, "receiving file data... size: [%d]", __header.size);

        if( (pConnector->x_receive(buffer, __header.size)) == DPN_SUCCESS ) {


            DPN_FileSlice * mainSlice = current->getMainSlice();
            if( mainSlice ) {
                mainSlice->transportIt(__header.size);
                mainSlice->write(buffer, __header.size);

                DL_INFO(1, "connector transportedBytes: [%d] transported: [%d] load: [%d]",
                        pConnector->transportedBytes(), mainSlice->transportedBytes(), mainSlice->loadBytes()
                        );


                if( mainSlice->isTransported() ) {
                    DL_INFO(1, "File received at all");
                    pFileSystem->stopReceiveFile(current->key());
                    mainSlice->stop();
                }
            }


            current = nullptr;
        }
        if( pConnector->state() == DPN_FAIL ) {
            DL_ERROR(1, "DPN_FileReceiveDirection: Connection fail");
            errorState = CONNECTION_FAIL;
            return false;
        }

    }


    return true;
}

*/



//============================================================================================================ IO__FILE
/*
bool IO__FILE::receive(FileTransport *file) {

    FileSlice *slice = new FileSlice;
    slice->init( file->descriptor() );
    iReceiveMap[file->descriptor().key()] = slice;
    slice->openW();
    return true;
}
bool IO__FILE::send(FileTransport *file) {

    FileMap &map = file->fileMap();

    FOR_VALUE( map.size(), i ) {
        FileSlice *slice = new FileSlice;
        slice->init( file->descriptor().path(), map[i] );
        aSession.append( slice );
    }
    return true;
}
DPN_Result IO__FILE::generate(DPN_ExpandableBuffer &b) {

    FileSlice *current = __get_slice();
    if( current == nullptr ) {
        DL_BADPOINTER(1, "no file slice");
        return DPN_FAIL;
    }
    int rb = current->read();
//    setSource( current->buffer(), rb );

    return DPN_SUCCESS;
}
DPN_Result IO__FILE::process(DPN_ExpandableBuffer &b) {
//    const DPN_ExpandableBuffer &b = constBuffer();
    const FilePacketHeader *header = reinterpret_cast<const FilePacketHeader*>(b.getData());

    FileSlice *current = iReceiveMap[header->key];
    if( current == nullptr ) {
        DL_ERROR(1, "No file slice with key: [%d]", header->key);
        return DPN_FAIL;
    }
    current->write( b );
    return DPN_SUCCESS;
}
FileSlice *IO__FILE::__get_slice() {

}
*/

using namespace DPN::Logs;
DPN_Catalog __global__host_catalog = DPN_Catalog("Host Catalog");

bool DPN_FileSystem::useChannel(const DPN_ClientTag *tag, DPN::Direction direction, __channel_mono_interface mono, const DPN_ExpandableBuffer &context) {

    DL_INFO(1, "tag: [%p] direction: [%d]", tag, direction);

    if( iClientsData.find( tag ) == iClientsData.end() ) {
        iClientsData[ tag ] = DPN_FileSystemDescriptor( );
    }

    DPN_FileSystemPrivateInterface i;
    DPN_FileSystemDescriptor &desc = iClientsData[ tag ];
    auto w = im.get( tag, desc );
    i.move( w );

    return i.addChannel( mono, direction, context );
}
DPN_FileSystem::DPN_FileSystem(const std::string &name) : DPN_AbstractModule(name) {

    __global__host_catalog.createLinearFileSystem();

    DL_INFO(1, "Create file module: [%p]", this);
}
DPN_Catalog &DPN_FileSystem::host() {
    return __global__host_catalog;
}
DPN_FileSystemInterface DPN_FileSystem::getIf(DPN_ClientInterface &clientIf) {
    DPN_FileSystemInterface i;



    DL_INFO(1, "client core: [%p]", clientIf.watcher().data() );

    if( iClientsData.find( clientIf.tag() ) == iClientsData.end() ) {
        iClientsData[clientIf.tag()] = DPN_FileSystemDescriptor( clientIf.watcher() );
    }

    DPN_FileSystemDescriptor &d = iClientsData[clientIf.tag()];
    if( d.isClientBound() == false ) {
        d.bind( clientIf.watcher() );
    }
    auto w = im.get( clientIf.tag(), d );
    i.move( w );
    return i;
}
DPN_FileSystemPrivateInterface DPN_FileSystem::getPrivateIf(const DPN_ClientTag *clientTag) {

    DPN_FileSystemPrivateInterface i;

    if( iClientsData.find( clientTag ) == iClientsData.end() ) {
        iClientsData[ clientTag ] = DPN_FileSystemDescriptor( );
    }

    DPN_FileSystemDescriptor &d = iClientsData[clientTag];
    auto w = imPrivite.get( clientTag, d );
    i.move( w );
    return i;
}
bool DPN_FileSystem::compareCatalogHash(const std::string &hash) const {
    std::string host_hash = __global__host_catalog.getHash();
    bool status = bool(host_hash == hash);
    return status;
}
bool DPN_FileSystemInterface::sync() {
    if( badInterface() ) {
        DL_ERROR(1, "Bad interface");
        return false;
    }

    auto proc = DPN_PROCS::processor<DPN_PACKETTYPE__SYNC_CATALOGS>();
//    DL_INFO(1, "Create sync processor [%p]", proc);
    return inner()->fs__send( proc );
}
const DPN_Catalog *DPN_FileSystemInterface::remote() const {
    if( badInterface() ) {
        DL_ERROR(1, "Bad interface");
        return nullptr;
    }
    return &inner()->iRemoteCatalog;
}
bool DPN_FileSystemInterface::requestFileset(const DArray<int> &keyset) {
    if( badInterface() ) {
        DL_ERROR(1, "Bad interface");
        return false;
    }

    auto proc = DPN_PROCS::processor<DPN_PACKETTYPE__REQUEST_FILE>();
    proc->setFileset( keyset );
    return inner()->fs__send( proc );
}
DPN_FileSystem *extractFileModule(DPN_Modules &modules) { return reinterpret_cast<DPN_FileSystem*>(modules.module("FileSystem")); }

DPN_Catalog *DPN_FileSystemPrivateInterface::remote() {
    if( badInterface() ) {
        DL_ERROR(1, "Bad interface");
        return nullptr;
    }
    return &inner()->iRemoteCatalog;
}
DFile DPN_FileSystemPrivateInterface::remoteFile(int key) const {
    if( badInterface() ) {
        DL_ERROR(1, "Bad interface");
        return DFile();
    }
    return inner()->iRemoteCatalog.constFile( key );
}
void DPN_FileSystemPrivateInterface::unverifyRemote2Host() {
    if( badInterface() ) return;
    inner()->iStateRemote2Host = false;
}
void DPN_FileSystemPrivateInterface::unverifyHost2Remote() {
    if( badInterface() ) return;
    inner()->iStateHost2Remote = false;
}
bool DPN_FileSystemPrivateInterface::verifyHost2Remote(bool status) {
    if( badInterface() ) {
        DL_ERROR(1, "Bad interface");
        return false;
    }

    return inner()->iStateHost2Remote = status;
}
bool DPN_FileSystemPrivateInterface::verifyRemote2Host(bool status) {
    inner()->iStateRemote2Host = status;
    return status;
}
DPN_FILESYSTEM::Interface DPN_FileSystemPrivateInterface::request(CallerPosition p, int key) {
    DPN_FILESYSTEM::Interface i;

    if( badInterface() ) {
        DL_ERROR(1, "Bad interface");
        return i;
    }

    if( p == DPN_HOST ) {

        if( inner()->iDownloadPath.empty() ) {
            inner()->iDownloadPath = "F:/DFS_SPACE/DOWNLOAD/";
        }

        DFile f = inner()->iRemoteCatalog.file( key );
        f.createAbsolutePath( inner()->iDownloadPath );
        DPN_FILESYSTEM::Descriptor fd(f);
        inner()->iHostRequest.append( fd );
        auto w = inner()->__host_interface.get( key, fd );
        i.move( w );

//        DL_INFO(1, "file: [%d] path: [%s] vpath: [%s] name: [%s] size: [%d] badfile: [%d]",
//                key, f.path_c(), f.vpath_c(),
//                f.name_c(), f.size(),
//                BADFILE(f)
//                );

    } else {
        DFile f = __global__host_catalog.file( key );
        DPN_FILESYSTEM::Descriptor fd(f);
        inner()->iServerRequest.append( fd );
        auto w = inner()->__server_interface.get( key, fd );
        i.move( w );

    }

    return i;
}
bool DPN_FileSystemPrivateInterface::send(DPN_FILESYSTEM::Interface fileIf) {

    if( badInterface() ) {
        DL_ERROR(1, "Bad interface");
        return false;
    }
    if( inner()->aOutgoingChannels.empty() ) {
        DL_ERROR(1, "No channels");
        return false;
    }
    FOR_VALUE( inner()->iServerRequest.size(), i ) {
        if( inner()->iServerRequest[i] == fileIf) {
            break;
        }
        DL_ERROR(1, "No interface in server request");
        return false;
    }
    DPN_FILESYSTEM::FileMap m = fileIf.fileMap();

    DL_INFO(1, "Try send file...");
    std::cout << DPN_FILESYSTEM::filemap2text( m ) << std::endl;

    FOR_VALUE( m.size(), mi ) {
        DPN_FILESYSTEM::Channel *pickedChannel = &inner()->aOutgoingChannels[0];
        int minLoad = inner()->aOutgoingChannels[0].load();

        FOR_VALUE( inner()->aOutgoingChannels.size(), i ) {

            if(  inner()->aOutgoingChannels[i].load() < minLoad ) {
                pickedChannel = &inner()->aOutgoingChannels[i];
                minLoad = inner()->aOutgoingChannels[i].load();
            }

        }
        DL_INFO(1, "picked channel: [%p] file: [%s] block: [%d-%d]",
                pickedChannel, fileIf.file().name_c(), m[mi].begin(), m[mi].end() );

        if( pickedChannel ) {

            pickedChannel->send( DPN_FILESYSTEM::Slice( fileIf.file(), m[mi] ) );
        }
    }
    return true;
}
bool DPN_FileSystemPrivateInterface::receive(DPN_FILESYSTEM::Interface fileIf) {

    if( badInterface() ) {
        DL_ERROR(1, "Bad interface");
        return false;
    }
    if( inner()->aIncomingChannels.empty() ) {
        DL_ERROR(1, "No incoming channels");
        return false;
    }
    FOR_VALUE( inner()->iHostRequest.size(), i ) {
        if( inner()->iServerRequest[i] == fileIf) {
            break;
        }
        DL_ERROR(1, "No interface in host request");
        return false;
    }

    if( inner()->putReceivingFile( fileIf.file() ) == false ) {
        DL_FUNCFAIL(1, "putReceivingFile");
        return false;
    }

    return true;
}
bool DPN_FileSystemPrivateInterface::addChannel(__channel_mono_interface mono, DPN::Direction direction, const DPN_ExpandableBuffer &context) {

    if( badInterface() ) {
        DL_ERROR(1, "Bad interface");
        return false;
    }
    if( mono.badInterface() ) {
        DL_ERROR(1, "Bad mono channel interface");
        return false;
    }

    DPN_FILESYSTEM::Channel channel( *inner(), mono, context);
    if( direction == DPN::FORWARD ) {
        inner()->aOutgoingChannels.append( channel );
    } else {
        inner()->aIncomingChannels.append( channel );
    }

    inner()->fs__activateChannel( channel );

    return true;
}
bool DPN_FileSystemData::fs__send( DPN_TransmitProcessor *p ) {
    if( DWatcher<DPN_ClientCore>::isEmptyObject() ) {
        DL_ERROR(1, "Empty watcher");
        return false;
    }
    if( DWatcher<DPN_ClientCore>::data() == nullptr ) {
        DL_BADPOINTER(1, "data");
        return false;
    }
    DL_INFO(1, "send proc: [%p] data: [%p]", p, DWatcher<DPN_ClientCore>::data());
    return DWatcher<DPN_ClientCore>::data()->send( p );
}
bool DPN_FileSystemData::fs__activateChannel(DPN_FILESYSTEM::Channel ch) {

    if( DWatcher<DPN_ClientCore>::isEmptyObject() ) {
        DL_ERROR(1, "Empty watcher");
        return false;
    }
    DWatcher<DPN_ClientCore>::data()->putUnit( ch.threadUnit() );
    return true;
}
