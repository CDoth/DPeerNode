#include "DPN_Client.h"



//1


/*
bool DPN_Client::sendFastFile(const char *filePath) {

    DFile file(filePath);
    return sendFastFile(file);
}
bool DPN_Client::sendFastFile(DFile &file) {

       if(BADFILE(file)) {
           DL_BADPOINTER(1, "file");
           return false;
       }
       if( data()->registerFastFile(file) == false ) {
           DL_FUNCFAIL(1, "registerFastFile");
           return false;
       }

       auto proc = data()->main_pool->getProcessor<DPN_PACKETTYPE__REGISTER_FAST_FILE>();
       proc->setSource(file);
       putSendRequest(proc);

       return true;
}
bool DPN_Client::requestRemoteFile(int key) {


    if(data()->checkRemoteFileRequest(key) == false) {
        DL_FUNCFAIL(1, "checkRemoteFileRequest");
        return false;
    }
    DFile file = remote().fileByRawKey(key);
    if(BADFILE(file)) {
        DL_BADVALUE(1, "file with key: [%d]", key);
        return false;
    }
    file.createVirtualPath(data()->downloadPath);
    //-------------------------------------------------------------------------
    auto proc = data()->main_pool->getProcessor<DPN_PACKETTYPE__REQUEST_FILE>();
    if(proc == nullptr) {
        DL_ERROR(1, "No processor");
        return false;
    }
    proc->appendFile(file);
    putSendRequest(proc);
    //-------------------------------------------------------------------------

    return true;
}
bool DPN_Client::requestRemoteFileSet(const DArray<int> &keyset) {

    if(data()->checkRemoteFileSetRequest(keyset) == false) {
        DL_FUNCFAIL(1, "checkRemoteFileSetRequest");
        return false;
    }

    //-------------------------------------------------------------------------
    auto proc = data()->main_pool->getProcessor<DPN_PACKETTYPE__REQUEST_FILE>();
    if(proc == nullptr) {
        DL_ERROR(1, "No processor");
        return false;
    }

    DArray<DFile> fileset;
    FOR_VALUE(keyset.size(), i) {
        DFile file = remote().fileByRawKey(keyset[i]);
        if(BADFILE(file)) {
            DL_BADVALUE(1, "file with key: [%d]", keyset[i]);
            return false;
        }
        file.createVirtualPath(data()->downloadPath);
        fileset.push_back(file);
    }

    proc->setFileSet(fileset);
    putSendRequest(proc);
    //-------------------------------------------------------------------------

    return true;
}
bool DPN_Client::sync() {

    auto host_catalog = host();
    if(host_catalog == nullptr) {
        DL_BADPOINTER(1, "Host catalog");
        return false;
    }
    std::string topology = host_catalog->topology(true);

//    auto host_hash = host_catalog->getHash();
//    DL_INFO(1, "HOST TOPOLOGY: [%d] HOST HASH: [%s]", topology.size(), host_hash.c_str());

    if(topology.empty()) {
        DL_ERROR(1, "Empty topology");
        return false;
    }
    auto proc_pool = procPool();
    if(proc_pool == nullptr) {
        DL_BADPOINTER(1, "Processors pool");
        return false;
    }
    auto proc = proc_pool->getProcessor<DPN_PACKETTYPE__DIRECTORY_TOPOLOGY>()->setSource(topology);

    data()->unverifyHost2Remote();

    putSendRequest(proc);

    return true;
}
*/
//================================================================================================================ DPN_Client:
#define CT_PROC(TYPE) COMPILETIME_PROCESSOR<TYPE>(sendEnvironment()).processor()
#define CT_PROC2(ENV, TYPE) COMPILETIME_PROCESSOR<TYPE>(ENV).processor()

DPN_Client::DPN_Client(const ClientInitContext &initContext) : DWatcher<DPN_ClientData>(true, initContext){

}
DPN_Client::~DPN_Client() {

}
bool DPN_Client::disconnect() {
    return data()->disconnect();
}
bool DPN_Client::sendMessage(const char *message) {
    return data()->sendMessage(message);
}
void DPN_Client::ping() {
    data()->ping();
}
void DPN_Client::shadow() {
    data()->requestShadowConnection();
}
ClientState DPN_Client::state() const {
    return data()->eState;
}
const std::string DPN_Client::name() const {
    return data()->iName;
}
const PeerAddress &DPN_Client::local() const {
    return data()->iLocal;
}
const PeerAddress &DPN_Client::remote() const {
    return data()->iRemote;
}
bool DPN_Client::isLocalVisible() const {
    return data()->iLocalVisible;
}
void DPN_Client::requestEnvironmet() {

}
void DPN_Client::addModule(const std::string &name, DPN_AbstractModule *module) {
    data()->addModule(name, module);
}
DPN_AbstractClient *DPN_Client::getAbstract() {
    return reinterpret_cast<DPN_AbstractClient*>(data());
}
const DArray<DPN_Channel *> &DPN_Client::channels() const {
    return data()->aChannels;
}
void DPN_Client::setThreadContext(DPN_ThreadContext &tc) {
    data()->wThreadContext = tc;
}

/*
int DPN_Client::localPort() const {
    return connector()->localPort();
}
std::string DPN_Client::localAddress() const {
    return connector()->localAddress();
}
std::string DPN_Client::localName() const {
    std::string name = localAddress();
    name = name + "::";
    name.append(std::to_string(localPort()));
    return name;
}
int DPN_Client::openPort() const {
    if(data()->peerparams->openPorts.empty()) {
        return -1;
    }
    return data()->peerparams->openPorts.constFront();
}
const DArray<int> &DPN_Client::openPorts() const {
    return data()->peerparams->openPorts;
}
int DPN_Client::peerPort() const {
    return data()->peerparams->address.port;
}
std::string DPN_Client::peerAddress() const {
    return data()->peerparams->address.address;
}
std::string DPN_Client::peerName() const {
    std::string name = peerAddress();
    name = name + "::";
    name.append(std::to_string(peerPort()));
    return name;
}
const char *DPN_Client::stateName() const {
    switch (data()->state) {

    GET_NAME(CONNECTED);
    GET_NAME(DISCONNECTED);
    GET_NAME(DISCONNECTION_PROCESSING);

    }

    return nullptr;
}
bool DPN_Client::isDisconnecting() const {
    return (data()->state == DISCONNECTION_PROCESSING || bool(data()->common.__flag__disconnect));
}
const DPN_TimeRange &DPN_Client::lastConnectionRange() const {
    return data()->connections.constBack();
}
int DPN_Client::connectionPeriods() const {
    return data()->connections.size();
}
const DArray<DPN_TimeRange> &DPN_Client::connectionHistory() const {
    return data()->connections;
}
const PeerParams &DPN_Client::params() const {
    return *data()->peerparams;
}
bool DPN_Client::sendMessage(const char *message) {

    auto proc = CT_PROC(DPN_PACKETTYPE__TEXT_MESSAGE);
    proc->setSource(message);
    putSendRequest(proc);


    return true;
}
void DPN_Client::ping() {

    auto proc = CT_PROC(DPN_PACKETTYPE__PING);
    putSendRequest(proc);
}
void DPN_Client::requestLocalEnv() {

    auto proc = CT_PROC(DPN_PACKETTYPE__REQUEST);
    proc->setType(REQUEST_NET_LOCAL_ENVIRONMENT);
    putSendRequest(proc);
}
bool DPN_Client::disconnect() {
    return data()->disconnect();
}
bool DPN_Client::shadow(int port) {

    if( !CHECK_PORT(port) ) {
        DL_BADVALUE(1, "port: [%d]", port);
        return false;
    }
    const PeerParams *pp = data()->peerparams;
    if(pp == nullptr) {
        DL_BADPOINTER(1, "peerparams");
        return false;
    }
    if(port == -1) port = pp->address.port;
    else if(pp->openPorts.contain(port) == false) {
        DL_ERROR(1, "Port unavailable: [%d]", port);
        DL_INFO(1, "Open ports: %s", ia2s(pp->openPorts).c_str());
        return false;
    }

    auto proc = CT_PROC(DPN_PACKETTYPE__MAKE_SHADOW_CONNECTION);
//    proc->set
//    common.__core->shadowConnection(this, peerparams->address.address.c_str(), port);

    return true;
}


void DPN_Client::setName(const std::string &__name) {
    if(data()->peerparams == nullptr) {
        DL_BADPOINTER(1, "peerparams");
        return;
    }
    data()->peerparams->name = __name;
}
void DPN_Client::setOpenPorts(const DArray<int> &ports) {
    data()->peerparams->openPorts = ports;
}
void DPN_Client::addOpenPort(int port) {
    data()->peerparams->openPorts.push_back(port);
}
DPN_Modules &DPN_Client::modules() {
    return data()->modules;
}

*/
//================================================================================================================ DPN_FileClient:
/*
DPN_FileClient::DPN_FileClient(DPN_Client &client) {

    defaultCopy(client);
    pClientFileSystem = data()->getModules().fileSystem();
}
void DPN_FileClient::setHostCatalog(DPN_Catalog *catalog) {
    if( pClientFileSystem == nullptr ) {
        DL_BADPOINTER(1, "pClientFileSystem");
        return;
    }
    pClientFileSystem->setHostCatalog(catalog);
}
void DPN_FileClient::setDownloadPath(const std::string &path) {
    if( pClientFileSystem == nullptr ) {
        DL_BADPOINTER(1, "pClientFileSystem");
        return;
    }
    pClientFileSystem->setDownloadPath(path);
}
bool DPN_FileClient::sendFastFile(const char *filePath) {
    DFile file(filePath);
    return sendFastFile(file);
}
bool DPN_FileClient::sendFastFile(DFile &file) {

    if(BADFILE(file)) {
        DL_BADPOINTER(1, "file");
        return false;
    }
    if( registerFile(file) == false ) {
        DL_FUNCFAIL(1, "registerFastFile");
        return false;
    }
//    int dialogIndex = data()->startDialog();
//    if( pClientFileSystem->makeRequestNote(dialogIndex, file.key()) == false ) {
//        DL_FUNCFAIL(1, "makeRequestNote");
//        return false;
//    }
    DL_INFO(1, "ready to register file: [%s]", file.name_c());

//    auto proc = CT_PROC2(data()->sendEnvironment(), DPN_PACKETTYPE__REGISTER_FAST_FILE);
//    proc->setDialog(dialogIndex);
//    proc->setSource(file);
//    data()->putSendRequest(proc);

    return true;
}
DPN_FileSystem &DPN_FileClient::fileSystem() {
    return *pClientFileSystem;
}
bool DPN_FileClient::requestFileset(const DArray<DFileKey> &keyset, DPN_FILESTREAM_STRICT_MODE strictMode, const DArray<DFileKey> *strictMask) {

    DArray<DFile> fileset;
    FOR_VALUE(keyset.size(), i) {

        int key = keyset[i];

        if(fileSystem().checkRemoteFile(key) == false) {
            DL_FUNCFAIL(1, "checkRemoteFileRequest");
            return false;
        }
        if( pClientFileSystem->receivingFile(key) ) {

            DL_INFO(1, "File already receiving");
            return true;
        }
        DFile file = pClientFileSystem->remote().file(key);
        if(BADFILE(file)) {
            DL_BADVALUE(1, "file with key: [%d]", key);
            return false;
        }
        file.createVirtualPath(fileSystem().downloadPath());
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

    if(fileSystem().checkRemoteFile(key) == false) {
        DL_FUNCFAIL(1, "checkRemoteFileRequest");
        return false;
    }
    if( pClientFileSystem->receivingFile(key) ) {
        DL_INFO(1, "File already receiving");
        return true;
    }
    DFile file = pClientFileSystem->remote().file(key);
    if(BADFILE(file)) {
        DL_BADVALUE(1, "file with key: [%d]", key);
        return false;
    }
    file.createVirtualPath(fileSystem().downloadPath());
    //-------------------------------------------------------------------------
    auto proc = data()->processor<DPN_PACKETTYPE__REQUEST_FILE>();
    proc->appendFile(file.key());
    data()->send(proc);
    //-------------------------------------------------------------------------

    return true;
}
bool DPN_FileClient::sync() {

    auto host_catalog = fileSystem().host();
    if(host_catalog == nullptr) {
        DL_BADPOINTER(1, "Host catalog");
        return false;
    }
    std::string topology = host_catalog->topology(true);
    if(topology.empty()) {
        DL_ERROR(1, "Empty topology");
        return false;
    }
    fileSystem().unverifyHost2Remote();

    auto proc = data()->processor<DPN_PACKETTYPE__CATALOG_TOPOLOGY>();
    proc->setSource(topology);
    data()->send(proc);

    return true;
}
bool DPN_FileClient::registerFile(DFile &file) {

    if( pClientFileSystem->host() == nullptr) {
        DL_BADPOINTER(1, "Host catalog");
        return false;
    }
    if( pClientFileSystem->host()->getFileSystem()->registerFile(file) == false ) {
        DL_FUNCFAIL(1, "registerFile");
        return false;
    }
    return true;
}

*/

