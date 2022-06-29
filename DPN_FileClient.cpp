#include "DPN_FileClient.h"

/*
DPN_FileClient::DPN_FileClient(DPN_Client &c) {
    defaultCopy(c);
    pFileSystem = extractFileModule(data()->modules());
}
void DPN_FileClient::setHostCatalog(DPN_Catalog *catalog) {
    if( pFileSystem == nullptr ) {
        DL_BADPOINTER(1, "pClientFileSystem");
        return;
    }
    pFileSystem->setHostCatalog(catalog);
}
void DPN_FileClient::setDownloadPath(const std::string &path) {
    if( pFileSystem == nullptr ) {
        DL_BADPOINTER(1, "pClientFileSystem");
        return;
    }
    pFileSystem->setDownloadPath(path);
}
bool DPN_FileClient::sendFastFile(const char *filePath) {

    DFile file(filePath);

    if(BADFILE(file)) {
        DL_BADPOINTER(1, "file");
        return false;
    }
    auto proc = data()->processor<DPN_PACKETTYPE__REGISTER_FAST_FILE>();
    proc->setSource(file);
    data()->send(proc);

    return true;
}
DPN_FileSystem &DPN_FileClient::fileSystem() {
    return *pFileSystem;
}
bool DPN_FileClient::requestFileset(const DArray<DFileKey> &keyset, DPN_FILESTREAM_STRICT_MODE strictMode, const DArray<DFileKey> *strictMask) {
    DArray<DFile> fileset;
    FOR_VALUE(keyset.size(), i) {

        int key = keyset[i];

        if(pFileSystem->checkRemoteFile(key) == false) {
            DL_FUNCFAIL(1, "checkRemoteFileRequest");
            return false;
        }
        if( pFileSystem->receivingFile(key) ) {

            DL_INFO(1, "File [%d] already receiving",
                    key);
            return true;
        }
        DFile file = pFileSystem->remote().file(key);
        if(BADFILE(file)) {
            DL_BADVALUE(1, "file with key: [%d]", key);
            return false;
        }
        file.createVirtualPath(pFileSystem->downloadPath());
        fileset.push_back(file);
    }
    if(fileset.empty()) {
        DL_INFO(1, "Empty fileset");
        return false;
    }
    //-------------------------------------------------------------------------
    auto proc = data()->processor<DPN_PACKETTYPE__REQUEST_FILE>();
    proc->setFileSet(keyset);


    if( strictMode == DPN_STRICT_SELECTIVE ) {

        if( strictMask && strictMask->size() ) {
            proc->setStrictMask(*strictMask);
        } else {
            strictMode = DPN_STRICT;
        }

    }
    proc->setStrictMode(strictMode);
    data()->send(proc);
    //-------------------------------------------------------------------------

    return true;
}
bool DPN_FileClient::requestFile(DFileKey key) {

    if(pFileSystem->checkRemoteFile(key) == false) {
        DL_FUNCFAIL(1, "checkRemoteFileRequest");
        return false;
    }
    if( pFileSystem->receivingFile(key) ) {
        DL_INFO(1, "File already receiving");
        return true;
    }
    DFile file = pFileSystem->remote().file(key);
    if(BADFILE(file)) {
        DL_BADVALUE(1, "file with key: [%d]", key);
        return false;
    }
    file.createVirtualPath(pFileSystem->downloadPath());
    //-------------------------------------------------------------------------
    auto proc = data()->processor<DPN_PACKETTYPE__REQUEST_FILE>();
    proc->appendFile(file.key());
    data()->send(proc);
    //-------------------------------------------------------------------------

    return true;
}
bool DPN_FileClient::sync() {

//    auto host_catalog = pFileSystem->host();
//    if(host_catalog == nullptr) {
//        DL_BADPOINTER(1, "Host catalog");
//        return false;
//    }
//    std::string topology = host_catalog->topology(true);
//    if(topology.empty()) {
//        DL_ERROR(1, "Empty topology");
//        return false;
//    }
//    pFileSystem->unverifyHost2Remote();

    auto proc = data()->processor<DPN_PACKETTYPE__SYNC>();
    data()->send(proc);
    return true;
}
bool DPN_FileClient::makeSpecialChannel() {

//    const DArray<DPN_Channel*> &ch = data()->getChannels();
//    if( ch.size() < 2 ) {
//        DL_ERROR(1, "No shadow channels");
//        return false;
//    }
//    DPN_Channel *__pick = nullptr;
//    FOR_VALUE(ch.size(), i) {

//        if( ch[i]->isFowardFree() ) {
//            __pick = ch[i];
//            break;
//        }

//    }
//    if( __pick == nullptr ) {
//        DL_ERROR(1, "No free channels");
//        return false;
//    }
//    DL_INFO(1, "pick shadow channel: [%p] [%s]", __pick, __pick->shadowKey().c_str());


//    auto proc = data()->processor<DPN_PACKETTYPE__RESERVE_FILE_CHANNEL>();
//    proc->setContext(__pick->remote(), __pick->local(), __pick->shadowKey());
//    data()->send(proc);

    return true;
}
bool DPN_FileClient::registerFile(DFile &file) {
    if( pFileSystem == nullptr ) {
        DL_BADPOINTER(1, "file system");
        return false;
    }
    return pFileSystem->registerLocalFile(file);
}
*/

