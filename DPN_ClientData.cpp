#include "DPN_ClientData.h"



/*
DPN_ClientData::DPN_ClientData(DPN_NodeConnector *connector) : remote_catalog("Remote Catalog") {
    send = new DPN_SendDirection(connector);
    receive = new DPN_ReceiveDirection(connector);

    host_catalog = nullptr;

    send->env.setClient(this);
    receive->env.setClient(this);

    host2remote_verified = false;
    remote2host_verified = false;

    clientFileBufferSize = DPN_DEFAULT_CLIENT_BUFFER_SIZE;


    remote_catalog.createMapFileSystem();
    autoreceive_fast_files = true;

    main_pool = nullptr;
}
DPN_ClientData::~DPN_ClientData() {
    delete send;
    delete receive;


}
DPN_ExpandableBuffer DPN_ClientData::getFileBuffer() {

       DPN_ExpandableBuffer buffer = send_pool.getBuffer();

       buffer.reserve(clientFileBufferSize);




       return buffer;
}
bool DPN_ClientData::verifyHost2Remote(const std::string &remoteHashSum) {

    if(host_catalog == nullptr) {
        DL_BADPOINTER(1, "host_catalog");
        return false;
    }
    std::string host_hash = host_catalog->getHash();
    bool status = bool(host_hash == remoteHashSum);



    DL_INFO(1, "status: [%d]", status);
    host2remote_verified = status;
    return host2remote_verified;
}
bool DPN_ClientData::verifyRemote2Host(bool status) {


    DL_INFO(1, "status: [%d]", status);
    remote2host_verified = status;
    return remote2host_verified;
}
void DPN_ClientData::unverifyRemote2Host() {
    remote2host_verified = false;
}
void DPN_ClientData::unverifyHost2Remote() {
    host2remote_verified = false;
}
bool DPN_ClientData::checkRemoteFileRequest(int key) {


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
bool DPN_ClientData::checkRemoteFileSetRequest(const DArray<int> &keys) {

    if(remote2host_verified == false) {
        DL_ERROR(1, "Unverified remote catalog. Need sync");
        return false;
    }
    if(keys.empty()) {
        DL_ERROR(1, "Empty keys list");
        return false;
    }
    FOR_VALUE(keys.size(), i) {
        if( remote_catalog.isValidKey(keys[i]) == false ) {
            DL_ERROR(1, "Unvalid key: [%d] on pos %d", keys[i], i);
            return false;
        }
    }
    return true;
}
DPN_FileTransportHandler *DPN_ClientData::startSendHostFile(int key) {

//    if(host2remote_verified == false) {
//        DL_ERROR(1, "Unverified remote catalog. Need sync");
//        return nullptr;
//    }

    DFile file = host_catalog->fileByRawKey(key);

    if(BADFILE(file)) {
        DL_ERROR(1, "Can't find file with key: [%d]", key);
        return nullptr;
    }

    return mainSession.startSend(file);
}
DPN_FileTransportHandler *DPN_ClientData::startReceiveRemoteFile(int key) {

    DFile file = remote_catalog.fileByRawKey(key);
    if(BADFILE(file)) {
        DL_ERROR(1, "Can't find file with key: [%d]", key);
        return nullptr;
    }


    file.createVirtualPath(downloadPath);

    DL_INFO(1, "Virtual file ready: Name: [%s] Path: [%s]  Key: [%d] Size: [%d]",
            file.name_c(), file.path_c(), file.key(), file.size());

    return mainSession.startReceive(file);
}
DPN_FileTransportGroup *DPN_ClientData::startSendFileSet(const DArray<int> &keyset) {

    if(host2remote_verified == false) {
        DL_ERROR(1, "Unverified remote catalog. Need sync");
        return nullptr;
    }
    if(keyset.empty()) {
        DL_ERROR(1, "Empty keyset");
        return nullptr;
    }
    DArray<DFile> fileset;
    FOR_VALUE(keyset.size(), i) {
        DFile file = host_catalog->fileByRawKey(keyset[i]);
        if(BADFILE(file)) {
            DL_ERROR(1, "Can't find file with key: [%d]", keyset[i]);
            return nullptr;
        } else {
            fileset.push_back(file);
        }
    }

    return mainSession.startSend(fileset);
}
bool DPN_ClientData::stopSendFileSet(const DArray<int> &keyset) {
    
}
bool DPN_ClientData::registerFastFile(DFile &file) {

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
bool DPN_ClientData::registerRemoteFastFile(DFile &file, DFileKey key) {

    file.createAbsolutePath(downloadPath);
    return remote_catalog.getFileSystem()->registerFile(file, key);
}
bool DPN_ClientData::verifyHostFile(const DFile &file) const {

    return host_catalog->getFileSystem()->isKeyRegister(file.key());
    return false;
}
bool DPN_ClientData::unregisterFile(DFileKey key) {
    if(host_catalog == nullptr) {
        DL_BADPOINTER(1, "Host catalog");
        return false;
    }

    return host_catalog->getFileSystem()->unregFile(key);
}
bool DPN_ClientData::stopSendFile(int key) {

    if(host_catalog == nullptr) {
        DL_BADPOINTER(1, "Host catalog");
        return false;
    }

    if( mainSession.stopSend(key) == false ) {
        DL_FUNCFAIL(1, "stopSend");
        return false;
    }

    return true;
}
bool DPN_ClientData::stopSendFastFile(DFileKey key) {
    if(host_catalog == nullptr) {
        DL_BADPOINTER(1, "Host catalog");
        return false;
    }
    if( mainSession.stopSend(key) == false ) {
        DL_FUNCFAIL(1, "stopSend");
        return false;
    }
    return host_catalog->getFileSystem()->unregFile(key);
}
bool DPN_ClientData::stopReceiveFile(int key)  {

    return mainSession.stopReceive(key);
}
DFile DPN_ClientData::hostFile(int key) {
    return host_catalog->fileByRawKey(key);
}
const DFile &DPN_ClientData::hostFile(int key) const {
    return host_catalog->constFileByRawKey(key);
}
DFile DPN_ClientData::remoteFile(int key) {
    return remote_catalog.fileByRawKey(key);
}
const DFile &DPN_ClientData::remoteFile(int key) const {
    return remote_catalog.constFileByRawKey(key);
}
bool DPN_ClientData::isFileReceiving(int key) const {



}
void DPN_ClientData::setFastDownloadPath(const std::string &path) {

    downloadPath = path;
    path_correct(downloadPath);
}

*/


//========================================================================================================= DPN_BaseChannel:
DPN_BaseChannel::DPN_BaseChannel() {
    channel = nullptr;
    send_direction = nullptr;
    recv_direction = nullptr;
}
void DPN_BaseChannel::init(DPN_Channel *ch, DPN_ClientContext &cc) {
    channel = ch;

    if(!send_direction) send_direction = new DPN_SendDirection;
    if(!recv_direction) recv_direction = new DPN_ReceiveDirection;

    send_direction->clientContext = cc;
    recv_direction->clientContext = cc;


    ch->reserve(send_direction, recv_direction);
}
void DPN_BaseChannel::send(DPN_TransmitProcessor *p) {
    if(send_direction == nullptr) {
        return;
    }
    send_direction->putProcessor(p);
}
bool DPN_BaseChannel::processing() const {
    return bool(send_direction->processing() ||
                recv_direction->processing());
}
//========================================================================================================= DPN_ClientData:
DPN_ClientData::DPN_ClientData(){}
DPN_ClientData::DPN_ClientData(const ClientInitContext &initContext) {

    DPN_Channel *base = new DPN_Channel;
    base->init(initContext.connector, initContext.initiator, initContext.avatar, base, initContext.shadowKey);
    aChannels.push_back(base);
    pMainChannel = base;
    //----------------------------------------
    __client_context cc;
    cc.__client = this;
    cc.__modules = &iModules;
    cc.__core = initContext.core;
    cc.__connector = base->connector();
    wClientContext.init(cc);
    //----------------------------------------
    iLocal = base->connector()->local();
    iRemote = base->connector()->peer();
    iAvatar = initContext.avatar;
    iName = initContext.name;
    iLocalVisible = initContext.localVisible;
    //----------------------------------------
    iBaseChannel.init(base, wClientContext);
    //----------------------------------------
    if( base->isInitiator() ) {
        aOpenPorts.push_back(
                    base->connector()->peerPort()
                    );
    }
    //----------------------------------------
    iSessionKey = initContext.sessionKey;
    eState = CONNECTED;

}
DPN_ClientData::~DPN_ClientData() {
//    delete send;
//    delete receive;
}
bool DPN_ClientData::processing() const {
    return iBaseChannel.processing();
}
bool DPN_ClientData::disconnect() {
    /// disconnect():
    /// 1. Exclude directions from threads and block them
    /// 2. Clear directions, but don't destroy
    /// 3. Clear modules, but don't destroy
    /// 4. Stop connector (clear/destroy?)

    if(eState == DISCONNECTED) {
        DL_INFO(1, "Client already disconnected");
        return true;
    }
    if(eState != DISCONNECTION_PROCESSING) {
        // 1
        DL_INFO(1, "Client turn in DISCONNECTION_PROCESSING state");

        wClientContext.blockClient();
        eState = DISCONNECTION_PROCESSING;
    }
    if( processing() == false ) {
        // 2,3,4

        DL_INFO(1, "Client processing stop: close all");

        close();

        eState = DISCONNECTED;

        wClientContext.core()->disconnect(this);

        return true;
    }

    return false;
}
void DPN_ClientData::setEnvironment(const DArray<PeerAddress> &env) {
    aEnvironment = env;
}
const std::string &DPN_ClientData::getSessionKey() const {
    return iSessionKey;
}
void DPN_ClientData::verifyShadow(const PeerAddress &local, const PeerAddress &remote, const std::string &key) {

    auto proc = processor<DPN_PACKETTYPE__MAKE_SHADOW_CONNECTION>();
    proc->setSource(local, remote);
    proc->setShadowKey(key);
    send(proc);

}
void DPN_ClientData::registerShadow(const PeerAddress &local, const PeerAddress &remote, const std::string &key) {

    ShadowPermission sp;
    sp.source = local;
    sp.target = remote;
    sp.shadowKey = key;

    aIncomingShadows.push_back(sp);

    DL_INFO(1, "client: [%p] source: [%s:%d] target: [%s:%d] shadow key: [%s]",
            this, local.address.c_str(), local.port, remote.address.c_str(), remote.port,
            key.c_str());

}
DPN_Result DPN_ClientData::checkShadowPermission(const PeerAddress &source, const PeerAddress &target, const std::string &__sessionKey, const std::string &shadowKey) const {

    DL_INFO(1, "client: [%p] shadows: [%d] shadow key: [%s] session key: [%s]",
            this, aIncomingShadows.size(), shadowKey.c_str(),  __sessionKey.c_str()

            );

    if( aIncomingShadows.empty() ) return DPN_REPEAT;

    FOR_VALUE( aIncomingShadows.size(), i ) {
        const ShadowPermission &sp = aIncomingShadows[i];

        DL_INFO(1, "permission shadow key: [%s]", sp.shadowKey.c_str());

        if( sp == shadowKey ) {

            if( sp.source == source && sp.target == target && iSessionKey == __sessionKey ) {

                DL_INFO(1, "Permission finded");
                return DPN_SUCCESS;
            }
            DL_ERROR(1, "Shadow permission with this key exist but has different stats: "
                        "permission: source: [%s:%d] target: [%s:%d] session key: [%s] "
                        "args: source: [%s:%d] target: [%s:%d] session key: [%s]",
                     sp.source.address.c_str(), sp.source.port, sp.target.address.c_str(), sp.target.port, iSessionKey.c_str(),
                     source.address.c_str(), source.port, target.address.c_str(), target.port, __sessionKey.c_str()
                     );

            return DPN_FAIL;
        }
    }
    return DPN_REPEAT;
}
bool DPN_ClientData::addShadowConnection(const ClientInitContext &init) {

    DL_INFO(1, "incoming: [%d] initiator: [%d]", aIncomingShadows.size(), init.initiator);

    if( init.initiator == false ) {
        int __i = -1;
        FOR_VALUE( aIncomingShadows.size(), i ) {
            const ShadowPermission &sp = aIncomingShadows[i];

            if( sp == init.shadowKey ) {
                __i = i;
                aIncomingShadows.removeByIndex(i);
                break;
            }
        }
        if( __i == -1 ) {
            DL_ERROR(1, "No shadow permission. shadow key: [%s]  incs: [%d]", init.shadowKey.c_str(), aIncomingShadows.size());
            return false;
        }
    }


    DL_INFO(1, "connector: [%p] local: [%s] remote: [%s]",
            init.connector,
//            init.connector->local().address.c_str(), init.connector->local().port,
//            init.connector->peer().address.c_str(), init.connector->peer().port
            init.connector->local().name().c_str(),
            init.connector->peer().name().c_str()
            );


    DPN_Channel *shChannel = new DPN_Channel;
    shChannel->init(init.connector, init.initiator, init.avatar, pMainChannel, init.shadowKey);
    aChannels.push_back(shChannel);



    DL_INFO(1, "Add shadow connection. channels: [%d] local: [%s] remote: [%s]",
            aChannels.size(),
//            shChannel->local().address.c_str(), shChannel->local().port,
//            shChannel->remote().address.c_str(), shChannel->remote().port
            shChannel->local().name().c_str(),
            shChannel->remote().name().c_str()
            );
    return true;
}
DPN_Channel *DPN_ClientData::channel(const std::string shadowKey) {

    FOR_VALUE( aChannels.size(), i ) {
        if( aChannels[i]->checkKey(shadowKey) ) {
            return aChannels[i];
        }
    }
    return nullptr;
}
void DPN_ClientData::shadowConnectionServerAnswer(bool a, int port) {

    ActionState &s = iStates.state(CE__MAKE_SHADOW_CONNECTION);
    if( s.iS != ActionState::Requesting ) {
        DL_WARNING(1, "Shadow connection not requesting");
    }
    if( a ) {
        wClientContext.core()->shadowConnection(this, port);
        s.iS = ActionState::Ready;
    } else {
        DL_INFO(1, "Server reject shadow connection request");
        s.iS = ActionState::Fail;
    }
}
bool DPN_ClientData::requestShadowConnection(int port) {

    ActionState &s = iStates.state(CE__MAKE_SHADOW_CONNECTION);
    if( s.iS == ActionState::Requesting ) {
        DL_WARNING(1, "Shadow connection already requesting");
        return true;
    }
    if( port == -1 ) {
        if( aOpenPorts.empty()) {
            DL_BADVALUE(1, "No available open ports");
            return false;
        }
        port = aOpenPorts.front();
    } else {
        if( aOpenPorts.contain(port) == false ) {
            DL_BADVALUE(1, "port: [%d]", port);
            return false;
        }
    }
    if( !DXT_CHECK_PORT(port) ) {
        DL_BADVALUE(1, "port: [%d]", port);
        return false;
    }

    DL_INFO(1, "Make shadow on port: [%d]", port);

    auto proc = processor<DPN_PACKETTYPE__REQUEST>();
    proc->setType(REQUEST_SHADOW_CONNECTION);
    proc->setPort(port);
    send(proc);
    s.iS = ActionState::Requesting;

    return true;
}
void DPN_ClientData::close() {

    FOR_VALUE(aChannels.size(), i) {
        aChannels[i]->stop();
    }
    iModules.clientDisconnected(this);
}
bool DPN_ClientData::sendMessage(const char *m) {

    auto proc = processor<DPN_PACKETTYPE__TEXT_MESSAGE>();
    proc->setMessage(m);
    send(proc);
    return true;
}
bool DPN_ClientData::ping() {
    auto proc = processor<DPN_PACKETTYPE__PING>();
    if(proc == nullptr) {
        DL_FUNCFAIL(1, "processor");
        return false;
    }
    send(proc);
    return true;
}
void DPN_ClientData::requestEnvironment() {
    auto proc = processor<DPN_PACKETTYPE__REQUEST>();
    proc->setType(REQUEST_NET_LOCAL_ENVIRONMENT);
    send(proc);
}
//=========================================================================================================== DPN_ProxyClientData:
/*
DPN_ProxyClientData::DPN_ProxyClientData(DPN_NodeConnector *sourceNode, DPN_NodeConnector *targetNode) {

    proxy__src2trt = new DPN_ProxyDirection(sourceNode, targetNode);
    proxy__trt2src = new DPN_ProxyDirection(targetNode, sourceNode);

//    source.port = sourceNode->peerPort();
//    source.address = sourceNode->peerAddress();
//    target.port = targetNode->peerPort();
//    target.address = targetNode->peerAddress();
}
DPN_ProxyClientData::~DPN_ProxyClientData() {
    delete proxy__src2trt;
    delete proxy__trt2src;
}
bool DPN_ProxyClientData::processing() const {
    return bool(proxy__src2trt->processing() || proxy__trt2src->processing());
}
*/







