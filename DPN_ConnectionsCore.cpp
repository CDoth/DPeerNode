#include "DPN_ConnectionsCore.h"
using namespace DPeerNodeSpace;

//====================================================================================================== DPN_WaitingConnection
DPN_PeerMeeting::DPN_PeerMeeting() : line(this) {
    connector = nullptr;
    baseClient = nullptr;
    mi = nullptr;
    core = nullptr;

    makeLine();
}
DPN_Result DPN_PeerMeeting::meet() {
    return line.go();
}
void DPN_PeerMeeting::init(MeetInfo *__mi, DPN_NodeConnector *c) {
    connector = c;
    mi = __mi;
}
void DPN_PeerMeeting::attachToClient(DPN_AbstractClient *client) {
    baseClient = client;
}
DPN_Result DPN_PeerMeeting::send_prepare() {


    if(mi == nullptr) {
        DL_BADPOINTER(1, "mi");
        return DPN_FAIL;
    }
    if(mi->sr_name.empty()) {
        DL_ERROR(1, "Empty name");
        return DPN_FAIL;
    }
    UNIT_NAME = mi->sr_name;
    if(baseClient) {        
        UNIT_ATTACH = baseClient->localAddress();
    } else {
        UNIT_ATTACH.clearBuffer();
    }

    content.parseBuffers();
    return DPN_SUCCESS;
}
DPN_Result DPN_PeerMeeting::send() {
//    DL_INFO(1, "send meet info...");
    return connector->x_send(content.buffer());
}
DPN_Result DPN_PeerMeeting::recv_prepare() {

    UNIT_NAME.clearBuffer();
    UNIT_ATTACH.clearBuffer();
    connector->clearInnerBuffer();
    content.clearBuffer();

    return DPN_SUCCESS;
}
DPN_Result DPN_PeerMeeting::recv() {

//    DL_INFO(1, "wait meet info...");
    if( connector->readable() ) {

//        DL_INFO(1, "receive meet info...");
        DPN_Result r = connector->x_receivePacket();

//        DL_INFO(1, "recv meet info result: [%d]", r);

        return r;
    }
    return DPN_REPEAT;
}
DPN_Result DPN_PeerMeeting::reaction() {


    if( content.deparseBuffer(connector->buffer()) == false ) {
        DL_FUNCFAIL(1, "deparse");
        return DPN_FAIL;
    }

    if(mi == nullptr) {
        DL_BADPOINTER(1, "MeetInfo");
        return DPN_FAIL;
    }
    mi->sr_name = UNIT_NAME.get();
    mi->r_attach = UNIT_ATTACH.get();
    mi->isShadow = mi->r_attach.correct();


    DL_INFO(1, "meet: name: [%s] attach: [%s:%d] shadow: [%d] local: [%s] remote: [%s]",
            mi->sr_name.c_str(),
            mi->r_attach.address.c_str(), mi->r_attach.port,
            mi->isShadow,
            connector->localName().c_str(), connector->peerName().c_str()
            );

//    mi->isShadow = false;



    return DPN_SUCCESS;
}
void DPN_PeerMeeting::makeLine() {

    line << &DPN_PeerMeeting::send_prepare
         << &DPN_PeerMeeting::send
         << &DPN_PeerMeeting::recv_prepare
         << &DPN_PeerMeeting::recv
         << &DPN_PeerMeeting::reaction
            ;
}
//====================================================================================================== DPN_WaitingConnection
DPN_WaitingConnection::DPN_WaitingConnection() : line(this) {
    makeLine();
    state = WAIT;
    pConnector = nullptr;
    base = nullptr;

    UNIT_VISIBLE = true;
}
DPN_WaitingConnection::~DPN_WaitingConnection() {

}
DPN_Result DPN_WaitingConnection::process() {
    return line.go();
}
void DPN_WaitingConnection::setName(const std::string &__name) {
    mi.sr_name = __name;
}
void DPN_WaitingConnection::setCore(DPN_AbstractConnectionsCore *__core) {
    core = __core;
}
void DPN_WaitingConnection::close() {
    if( pConnector ) {
        pConnector->close();
    }
}
bool DPN_WaitingConnection::isShadow() const {
    return mi.isShadow;
}
DPN_AbstractClient *DPN_WaitingConnection::baseClient() {
    return base;
}
ClientInitContext DPN_WaitingConnection::clientContext() {
    return initConext;
}
void DPN_WaitingConnection::makeLine() {

    line << &DPN_WaitingConnection::prepare
         << &DPN_WaitingConnection::meetInit
         << &DPN_WaitingConnection::meet
         << &DPN_WaitingConnection::wait
         << &DPN_WaitingConnection::dialog
            ;
}
DPN_Result DPN_WaitingConnection::meetInit() {
    meeting.init(&mi, pConnector);
    return DPN_SUCCESS;
}
DPN_Result DPN_WaitingConnection::meet() {
    return meeting.meet();
}
std::string DPN_WaitingConnection::generateSessionKey() {
    std::string seed;
    std::string key;


    int r1 = pointerValue(pConnector);
    int r2 = pointerValue(&seed);
    int r3 = pointerValue(this);
    int r4 = time(NULL);

    seed.append(mi.sr_name);
    seed.append(std::to_string(r1));
    seed.append(std::to_string(r2));
    seed.append(std::to_string(r3));
    seed.append(std::to_string(r4));



    DPN_SHA256 hashtool;
    hashtool.hash_string(seed);
    key = hashtool.get();

    return key;
}
std::string DPN_WaitingConnection::generateShadowKey() {
    std::string seed;
    std::string key;

    int r1 = time(NULL);

    seed.append(std::to_string(r1));
    seed.append(mi.sr_name);


    DPN_SHA256 hashtool;
    hashtool.hash_string(seed);
    key = hashtool.get();

    return key;
}
//====================================================================================================== DPN_IncomingConnection
DPN_IncomingConnection::DPN_IncomingConnection(DPN_NodeConnector *c) {
    pConnector = c;
    initConext.initiator = false;
    core = nullptr;
}
void DPN_IncomingConnection::accept() {

//    if(mi.isShadow) return;
    state = ACCEPT;
}
void DPN_IncomingConnection::reject() {
    state = REJECT;
}
DPN_Result DPN_IncomingConnection::prepare() {
    return DPN_SUCCESS;
}
DPN_Result DPN_IncomingConnection::wait() {

    if( state == WAIT ) {
        if( pConnector->readable() ) return DPN_FAIL;

        if( mi.isShadow ) {
            if( base == nullptr ) {
                if( (base = core->client(mi.r_attach)) == nullptr ) {
                   DL_ERROR(1, "Can't find base client to attach. [%s:%d]", mi.r_attach.address.c_str(), mi.r_attach.port);
                   return DPN_FAIL;
                }
                DL_INFO(1, "Find client [%p] with address: [%s:%d]", base, mi.r_attach.address.c_str(), mi.r_attach.port);
            }

            state = ACCEPT;
            makeDialogLine();
            return DPN_SUCCESS;
        }
        return DPN_REPEAT;
    }
    if( state == REJECT ) {}
    if( state == ACCEPT ) {}

    makeDialogLine();
    return DPN_SUCCESS;
}
DPN_Result DPN_IncomingConnection::dialog() {
    return dialogLine.go();
}
DPN_Result DPN_IncomingConnection::prepare1_s2c() {

    if( state == ACCEPT ) {
        UNIT_SIGNAL = pConnector->localPort();
    } else if ( state == REJECT ) {
        UNIT_SIGNAL = -1;
    }

    signal_content.parseBuffers();
//    DL_INFO(1, "finished");
    return DPN_SUCCESS;
}
DPN_Result DPN_IncomingConnection::transmit1_s2c() {
    return pConnector->x_send(signal_content.buffer());
}
DPN_Result DPN_IncomingConnection::prepare2_afc() {

//    DL_INFO(1, "buffer: [%d] tb: [%d]", signal_content.buffer().size(), connector->transportedBytes());
//    return DPN_REPEAT;

    if( state == REJECT ) return DPN_FAIL;
    pConnector->clearInnerBuffer();
//    DL_INFO(1, "finished");
    return DPN_SUCCESS;
}
DPN_Result DPN_IncomingConnection::transmit2_afc() {
    if( pConnector->readable() ) {
        return pConnector->x_receivePacket();
    }
    return DPN_REPEAT;
}
DPN_Result DPN_IncomingConnection::prepare3_a2c() {

    //------------------------------------------------------------------
    if( answer_content.deparseBuffer(pConnector->buffer()) == false ) {
        DL_ERROR(1, "deparsing");
        return DPN_FAIL;
    }

    std::string sessionKey = UNIT_SESSION_KEY.get();
    std::string shadowKey = UNIT_SHADOW_KEY.get();
    PeerAddress avatar = UNIT_AVATAR.get();



    if( mi.isShadow ) {
//        DL_INFO(1, "need check shadow permission");
        auto r = base->checkShadowPermission(pConnector->peer(), avatar, sessionKey, shadowKey);
        if( r != DPN_SUCCESS ) return r;
        initConext.clear();
        initConext.shadowKey = shadowKey;
        initConext.connector = pConnector;
        initConext.avatar = avatar;
        initConext.initiator = false;
    } else {
        sessionKey = generateSessionKey();
        initConext.name = mi.sr_name;
        initConext.connector = pConnector;
        initConext.avatar = avatar;
        initConext.initiator = false;
        initConext.sessionKey = sessionKey;
        initConext.core = core;
    }
    initConext.localVisible = UNIT_VISIBLE.get();

//    DL_INFO(1, "session key: [%s] shadow key: [%s] avatar: [%s:%d] local: [%s] remote: [%s]",
//            sessionKey.c_str(), shadowKey.c_str(), avatar.address.c_str(), avatar.port,
//            connector->localName().c_str(), connector->peerName().c_str()
//            );

    //------------------------------------------------------------------
    UNIT_AVATAR = pConnector->peer();
    UNIT_SESSION_KEY = sessionKey;
    UNIT_SHADOW_KEY.clearBuffer();
    //------------------------------------------------------------------
    answer_content.clearBuffer();
    answer_content.parseBuffers();

//    DL_INFO(1, "finished");
    return DPN_SUCCESS;
}
DPN_Result DPN_IncomingConnection::transmit3_a2c() {
    return pConnector->x_send(answer_content.buffer());
}
void DPN_IncomingConnection::makeDialogLine() {

    dialogLine.setTarget(this);

    dialogLine << &DPN_IncomingConnection::prepare1_s2c
               << &DPN_IncomingConnection::transmit1_s2c

               << &DPN_IncomingConnection::prepare2_afc
               << &DPN_IncomingConnection::transmit2_afc

               << &DPN_IncomingConnection::prepare3_a2c
               << &DPN_IncomingConnection::transmit3_a2c
                  ;
}



//====================================================================================================== DPN_OutgoingConnection
DPN_OutgoingConnection::DPN_OutgoingConnection(const char *address, int port) {

    clear();
    connectTarget.address = address;
    connectTarget.port = port;
}
DPN_OutgoingConnection::DPN_OutgoingConnection(DPN_AbstractClient *client, int port) {
    clear();
    connectTarget.address = client->remoteAddress().address;
    connectTarget.port = port;
    base = client;
    meeting.attachToClient(client);
}
DPN_OutgoingConnection::~DPN_OutgoingConnection() {
}

void DPN_OutgoingConnection::setAttempts(int a) {
    max_attempts = a;
}
DPN_Result DPN_OutgoingConnection::prepare() {

    if( pConnector == nullptr ) {
        pConnector = new DPN_NodeConnector;
    }

    if( pConnector->connectTo(connectTarget) ) {

        if(base) {
            innerShadowKey = generateShadowKey();
            initConext.shadowKey = innerShadowKey;
            base->verifyShadow(pConnector->local(), pConnector->peer(), innerShadowKey);
        }

        return DPN_SUCCESS;

    } else if( attempts_counter++ != max_attempts ) {
        return DPN_REPEAT;
    }

    return DPN_FAIL;
}
DPN_Result DPN_OutgoingConnection::wait() {

    if( pConnector->readable() ) {
        makeDialogLine();
        return DPN_SUCCESS;
    }
    return DPN_REPEAT;
}
DPN_Result DPN_OutgoingConnection::dialog() {
    return dialogLine.go();
}
DPN_Result DPN_OutgoingConnection::prepare1_sfs() {
    pConnector->clearInnerBuffer();

//    DL_INFO(1, "finished");
    return DPN_SUCCESS;
}
DPN_Result DPN_OutgoingConnection::transmit1_sfs() {
    if( pConnector->readable() ) {
        return pConnector->x_receivePacket();
    }
    return DPN_REPEAT;
}
DPN_Result DPN_OutgoingConnection::prepare2_a2s() {


//    DL_INFO(1, "tb: [%d] buffer: [%d]", connector->transportedBytes(), connector->buffer().size());
    //---------------------------------------------------------------------
    if( signal_content.deparseBuffer(pConnector->buffer()) == false ) {
        DL_ERROR(1, "deparsing");
        return DPN_FAIL;
    }

    int v = UNIT_SIGNAL.get();
    if(v == pConnector->peerPort()) {
        state = ACCEPT;
    } else if ( v == -1 ) {
        state = REJECT;
        return DPN_FAIL;
    } else {
        return DPN_FAIL;
    }
    //---------------------------------------------------------------------
    if(base) {
        UNIT_SHADOW_KEY = innerShadowKey;
        UNIT_SESSION_KEY = base->getSessionKey();
    } else {
        UNIT_SHADOW_KEY.clearBuffer();
        UNIT_SESSION_KEY.clearBuffer();
    }
    UNIT_AVATAR = pConnector->peer();

    DL_INFO(1, "keys: session: [%s] shadow: [%s]", UNIT_SESSION_KEY.get().c_str(), UNIT_SHADOW_KEY.get().c_str());


    //---------------------------------------------------------------------

    answer_content.clearBuffer();
    pConnector->clearInnerBuffer();

    answer_content.parseBuffers();

//    DL_INFO(1, "finished");
    return DPN_SUCCESS;
}
DPN_Result DPN_OutgoingConnection::transmit2_a2s() {
    return pConnector->x_send(answer_content.buffer());
}
DPN_Result DPN_OutgoingConnection::prepare3_afs() {
    answer_content.clearBuffer();
    pConnector->clearInnerBuffer();
//    DL_INFO(1, "finished");
    return DPN_SUCCESS;
}
DPN_Result DPN_OutgoingConnection::transmit3_afs() {
    if( pConnector->readable() ) {
        return pConnector->x_receivePacket();
    }
    return DPN_REPEAT;
}
DPN_Result DPN_OutgoingConnection::post_proc() {

    if( answer_content.deparseBuffer(pConnector->buffer()) == false ) {
        DL_ERROR(1, "deparsing");
        return DPN_FAIL;
    }

    std::string sessionKey = UNIT_SESSION_KEY.get();
    std::string shadowKey = UNIT_SHADOW_KEY.get();
    PeerAddress avatar = UNIT_AVATAR.get();

    initConext.name = mi.sr_name;
    initConext.connector = pConnector;
    initConext.avatar = avatar;
    initConext.initiator = true;
    initConext.sessionKey = sessionKey;
    initConext.core = core;
    initConext.localVisible = UNIT_VISIBLE.get();

    if(base) {
        mi.isShadow = true;
    }

    DL_INFO(1, "session key: [%s] shadow key: [%s] avatar: [%s:%d] local: [%s] remote: [%s]",
            sessionKey.c_str(), shadowKey.c_str(), avatar.address.c_str(), avatar.port,
            pConnector->localName().c_str(), pConnector->peerName().c_str()
            );

//    DL_INFO(1, "finished");
    return DPN_SUCCESS;
}

void DPN_OutgoingConnection::makeDialogLine() {
    dialogLine.setTarget(this);

    dialogLine << &DPN_OutgoingConnection::prepare1_sfs
               << &DPN_OutgoingConnection::transmit1_sfs

               << &DPN_OutgoingConnection::prepare2_a2s
               << &DPN_OutgoingConnection::transmit2_a2s

               << &DPN_OutgoingConnection::prepare3_afs
               << &DPN_OutgoingConnection::transmit3_afs

               << &DPN_OutgoingConnection::post_proc
                  ;
}
void DPN_OutgoingConnection::clear() {
    max_attempts = 0;
    attempts_counter = 0;
    initConext.initiator = true;
    base = nullptr;
    pConnector = nullptr;
}


//============================================================ DPN_ConnectionsCore
DPN_ConnectionsCore::DPN_ConnectionsCore() : host_catalog("Host Catalog")
{
    host_catalog.createLinearFileSystem();

//    iGlobalMediaSystem.setCore(this);

    userThreadContext.alloc();

    GlobalModule *m = defaultModules;
    while( m->creator ) {

        if( !m->unique ) {
            DPN_AbstractModule *module = m->creator();
            if( module == nullptr ) {
                DL_WARNING(1, "Can't create module [%s]", m->name.c_str());
            } else {
                module->setCore(this);
                iGlobalModules.addModule(module, m->name);
            }
        }
        ++m;
    }


    __core_action(pointerValue(this));
}
void DPN_ConnectionsCore::toEachClient(DPN_ClientSystemMessage m, DPN_AbstractClient *source) {

    switch (m) {
    case DPN_CSM__CHECK:
//        FOR_VALUE(remotes.size(), i) {
//            if(remotes[i].data() != source) {

//                DL_INFO(1, "Client: [%s - %s:%d]", remotes[i].params().name.c_str(), remotes[i].params().address.c_str(),
//                        remotes[i].params().port);
//            }
//        }
        break;
    case DPN_CSM__GLOBAL_ENVIRONMENT:
//        FOR_VALUE(remotes.size(), i) {
//            if(remotes[i].data() != source) {
//                auto &c = remotes[i];
//                c.requestLocalEnv();
//            }
//        }
        break;
    }

}
void DPN_ConnectionsCore::pushTask(DPN_Task *t) {

    if( tbs.empty() ) return;

    if( tbs.size() == 1) {
        tbs.front().forceTask(t);
        return;
    }
    FOR_VALUE( tbs.size(), i ) {
        if( tbs[i].pushTask(t) ) {
            break;
        }
    }

}
bool DPN_ConnectionsCore::isShadowAvailable(DPN_AbstractClient *client, int port) {

    FOR_VALUE( openPorts.size(), i ) {
        if( openPorts[i].port() == port && openPorts[i].isOpen() ) {
            // check permissions and client/global settings for shadow connections
            return true;
        }
    }
    return false;
}
void DPN_ConnectionsCore::shadowConnection(DPN_AbstractClient *client, int port) {

    DPN_OutgoingConnection *oc = new DPN_OutgoingConnection(client, port);
    oc->setName(name);
    oc->setCore(this);
    outs.push_back(oc);
}
DPN_AbstractClient *DPN_ConnectionsCore::client(const PeerAddress &pa) {

    FOR_VALUE(remotes.size(), i) {
        if(remotes[i].remote() == pa) {
            return remotes[i].getAbstract();
        }
    }
    return nullptr;
}
void DPN_ConnectionsCore::replanDirections() {
    __replan_directions();
}
void DPN_ConnectionsCore::addGlobalDirection(DPN_Direction *d) {
    if( d == nullptr ) {
        DL_BADPOINTER(1, "direction");
        return;
    }
    aGlobalDirections.append(d);

    DL_INFO(1, "Append global direction: [%p] size: [%d]", d, aGlobalDirections.size());
    __replan_directions();
}
void DPN_ConnectionsCore::disconnect(DPN_AbstractClient *c) {
    FOR_VALUE( remotes.size(), i ) {
        if( remotes[i].getAbstract() == c ) {
            if( remotes[i].disconnect() ) {
                aDisconnectedClients.append(remotes[i]);
                remotes.removeByIndex(i--);
            }
        }
    }
}
DArray<PeerAddress> DPN_ConnectionsCore::getEnvironment() const {
    DArray<PeerAddress> ret;
    FOR_VALUE( remotes.size(), i ) {
        if( remotes[i].isLocalVisible() ) {
            ret.append(remotes[i].remote());
        }
    }
    return ret;
}
DPN_UDPPort *DPN_ConnectionsCore::openUDPPort(int port) {

    FOR_VALUE( udpPorts.size(), i ) {
        if( udpPorts[i]->socket().localPort() == port ) {
            DL_WARNING(1, "Can't bind to port [%d]", port);
            return nullptr;
        }
    }
    DPN_UDPPort *p = new DPN_UDPPort;
    if( p->bind(port) == false ) {
        DL_WARNING(1, "Can't bind to port [%d]", port);
        delete p;
        return nullptr;
    }
    udpPorts.append(p);
    return p;
}
void DPN_ConnectionsCore::renewCatalog() {
    host_catalog.renew();
}
bool DPN_ConnectionsCore::processIncomingConnections() {

    FOR_VALUE(openPorts.size(), i) {

        DPN_SharedPort &sp = openPorts[i];

        if( sp.open() == false ) {
            continue;
        }

        if( sp.newConnection() ) {

//            DL_INFO(1, ">>> new incoming connection...");
            DPN_NodeConnector *connector = sp.accept();
            if(connector == nullptr) {
                DL_ERROR(1, "Can't accept connection");
                return false;
            }

            DPN_SharedPort::CheckResult r;
            if( (r = sp.check(connector)) != DPN_SharedPort::CheckedSuccesfull) {
                __push_bad_inc(connector, r);
            } else if(sp.isAuto()) {
//                __add_client(connector);
            } else {
                __push_inc(connector);
            }
        }
    }
    FOR_VALUE(incs.size(), i) {

        if( __processConnection(incs[i]) ) {
            delete incs[i];
            incs.removeByIndex(i--);
        }
    }

    return true;
}
bool DPN_ConnectionsCore::processOutgoingConnections() {

    FOR_VALUE(outs.size(), i) {

        if( __processConnection(outs[i]) ) {

            delete outs[i];
            outs.removeByIndex(i--);
        }
    }

    return true;
}
bool DPN_ConnectionsCore::__processConnection(DPN_WaitingConnection *connection) {

    __core_action();
    auto r = connection->process();
    if( r == DPN_SUCCESS ) {

        if( connection->finalState() == ACCEPT ) {

            if( connection->isShadow() ) {

                DL_INFO(1, "Accept shadow connection");

                DPN_AbstractClient *c = connection->baseClient();
                c->addShadowConnection(connection->clientContext());


            } else {
                DL_INFO(1, "Connection accepted");
                __add_client(connection->clientContext());
            }

        } else if ( connection->finalState() == REJECT ) {
            DL_INFO(1, "Connection rejected");
        }  else {
            DL_ERROR(1, "Bad final state: [%d]", connection->finalState());
        }
        return true;

    } else if ( r == DPN_FAIL ) {
        DL_INFO(1, "Connection processing fault");
        connection->close();
        return true;
    }
    return false;
    /*
    auto e = connection->proc();
    if( e == DPN_SUCCESS ) {
        if( connection->phase() == DPN_CS_REJECTING ) {
            DL_INFO(1, "Connection rejected");
        } else {
            DL_INFO(1, "Add client");
            __add_client(connection->connector(), connection->remoteName());
        }
        return true;
    }
    if( e == DPN_FAIL ) {
        DL_ERROR(1, "Can't accept incoming connection: [%s] socket: [%d]",
                 connection->connector()->peerName().c_str(),
                 connection->connector()->socket()
                 );

        connection->connector()->close();

        return true;
    }
    return false;
    */
}
bool DPN_ConnectionsCore::share_port(int port, bool autoaccepting) {

    if(!DXT_CHECK_PORT(port)) {
        DL_BADVALUE(1, "port: [%d]", port);
        return false;
    }
    __core_action(port);
    FOR_VALUE(openPorts.size(), i) {

        if( openPorts[i].port() == port) {
            DL_INFO(1, "Port [%d] already processing", port);
            return true;
        }
    }

    openPorts.push_back(DPN_SharedPort(port, autoaccepting));
    return true;
}
bool DPN_ConnectionsCore::connect_to(const char *address, int port) {


    __core_action(port + pointerValue(address));
    DPN_OutgoingConnection *oc = new DPN_OutgoingConnection(address, port);
    oc->setName(name);
    oc->setCore(this);
    outs.push_back(oc);

    return true;
}
bool DPN_ConnectionsCore::send_message(const char *message) {

    return remotes[0].sendMessage(message);
}
bool DPN_ConnectionsCore::sync() {

    return DPN_FileClient(remotes[0]).sync();
    return false;
}
void DPN_ConnectionsCore::ping() {
    remotes[0].ping();
}
void DPN_ConnectionsCore::acceptAll() {


    if(incs.empty()) {
        DL_INFO(1, "No incoming connections");
        return;
    }
    FOR_VALUE(incs.size(), i) {
        incs[i]->accept();
    }

}
void DPN_ConnectionsCore::rejectAll() {

    if(incs.empty()) {
        DL_INFO(1, "No incoming connections");
        return;
    }
    FOR_VALUE(incs.size(), i) {
        incs[i]->reject();
    }
}
void DPN_ConnectionsCore::disconnectAll() {


    FOR_VALUE(remotes.size(), i) {
        if( remotes[i].disconnect() ) {
            remotes.removeByIndex(i--);
        }
    }


}
bool DPN_ConnectionsCore::__open_udp_port(int port) {
    DXT __socket__(DXT::UDP);
    if( __socket__.bind(port) == false ) {
        DL_FUNCFAIL(1, "bind udp socket to port: [%d]", port);
        return false;
    }

}
bool DPN_ConnectionsCore::__push_bad_inc(DPN_NodeConnector *connector, DPN_SharedPort::CheckResult r) {

    DL_INFO(1, "Bad incoming connection: [%p] [%s] cause: [%d]",
            connector, connector->peerName().c_str(),
            r);
    return true;
}
bool DPN_ConnectionsCore::__push_inc(DPN_NodeConnector *connector) {

//    DL_INFO(1, ">>> push incoming: [%p]", connector);

    if(connector == nullptr) {
        DL_BADPOINTER(1, "connector");
        return false;
    }
    DPN_IncomingConnection *connection = new DPN_IncomingConnection(connector);
    connection->setName(name);
    connection->setCore(this);

    if(incs.contain(connection)) {
        DL_ERROR(1, "Connector already in incoming list");
        return false;
    }
    incs.push_back(connection);

    return true;
}
bool DPN_ConnectionsCore::__add_client(const ClientInitContext &c) {



    DL_INFO(1, "name: [%s] local: [%s] remote: [%s] avatart: [%s:%d] session key: [%s]",
            c.name.c_str(), c.connector->localName().c_str(), c.connector->peerName().c_str(), c.avatar.address.c_str(), c.avatar.port,
            c.sessionKey.c_str()
            );


    __core_action(c.avatar.port + c.name.size());

    DPN_Client client(c);

    //------ init client:
    client.setThreadContext(userThreadContext);
    GlobalModule *m = defaultModules;
    while( m->creator ) {

        if( m->unique ) {
            DPN_AbstractModule *module = m->creator();
            if( module == nullptr ) {
                DL_WARNING(1, "Can't create module [%s]", m->name.c_str());
            } else {
                module->setCore(this);
                client.addModule(m->name, module);
            }
        }
        else client.addModule( m->name, iGlobalModules.module(m->name));
        ++m;
    }
    //------------------
    DPN_FileClient fc(client);
    fc.setHostCatalog( &host_catalog );
    fc.setDownloadPath("F:/DFS_SPACE/DOWNLOAD");
    //------------------
    DPN_MediaClient mc(client);
//    mc.setGlobal( &iGlobalMediaSystem );
    //------------------

    remotes.push_back(client);
    __replan_directions();

    return true;
}
bool DPN_ConnectionsCore::__add_thread(DPN_ThreadBridge &bridge) {

    tbs.push_back(bridge);
    __replan_directions();
    __replan_inner();

    return true;
}
bool DPN_ConnectionsCore::__remove_thread(DPN_ThreadBridge &bridge) {
    int i = tbs.indexOf(bridge);
    if(i == -1) {
        DL_BADVALUE(1, "Undefined thread bridge");
        return false;
    }

    tbs.removeByIndex(i);
    if(tbs.empty()) return true;

    __replan_directions();
    __replan_inner();

    return true;
}
bool DPN_ConnectionsCore::__remove_client(DPN_Client &client) {

    if( remotes.remove(client) != -1 ) {
        __replan_directions();
    } else {
        DL_BADVALUE(1, "Undefined client");
        return false;
    }

    return true;
}
bool DPN_ConnectionsCore::__set_threads(const DArray<DPN_ThreadBridge> &set) {

    if(set.empty()|| set.size() > DPN_MAXIMUM_THREADS) {
        DL_BADVALUE(1, "threads number: [%d] (range: [%d:%d])", set.size(), 1, DPN_MAXIMUM_THREADS);
        return false;
    }

    return false;
}
void DPN_ConnectionsCore::__core_action(int s) {
    random.push_back(s + (time(NULL) % 323232) );
}
void DPN_ConnectionsCore::__replan_directions() {

    if(remotes.empty() || tbs.empty()) return;

    DPN_THREAD_GUARD(__mutex);

    FOR_VALUE(tbs.size(), i) tbs[i].clearPlan();

    int threadIndex = 0;
    for(int i=0; i != remotes.size(); ++i) {

        DPN_Client &c = remotes[i];
        if( c.state() == DISCONNECTED ) continue;

        DPN_Direction *d = nullptr;
        const DArray<DPN_Channel*> &ch = c.channels();

        FOR_VALUE(ch.size(), ch_i) {

            FOR_VALUE(2, d_i) {
                d = ch[ch_i]->direction(d_i);

                if( tbs[threadIndex].planDirection(d) ) {
                    if( ++threadIndex == tbs.size() ) {
                        threadIndex = 0;
                    }
                }

            } // directions loop
        } // channels loop
    } // clients loop

    FOR_VALUE( aGlobalDirections.size(), i ) {

        if( tbs[threadIndex].planDirection(aGlobalDirections[i]) ) {
            if( ++threadIndex == tbs.size() ) {
                threadIndex = 0;
            }
        } else {
            DL_WARNING(1, "Can't plan direction: [%p]", aGlobalDirections[i]);
        }
    }

    aGlobalDirections.clear();

    FOR_VALUE(tbs.size(), i) tbs[i].replacePlaned();
}
void DPN_ConnectionsCore::__replan_inner() {

    if(tbs.empty()) return;

    int threadIndex = 0;
//#define INC_INDEX if(++threadIndex == tbs.size()) threadIndex = 0;

    tbs[threadIndex].setProcessTasks(true);
}

//============================================================ DPN_SharedPort
DPN_SharedPort::DPN_SharedPort() {
    clear();
}
DPN_SharedPort::DPN_SharedPort(int port, bool autoaccepting) {
    clear();
    __port = port;
    __autoaccepting = autoaccepting;
}
bool DPN_SharedPort::newConnection() {
    if(!__open) return false;
    return __connector ? __connector->newConnection(__port) : false;
}
DPN_NodeConnector *DPN_SharedPort::accept() {

    if(!__open) return nullptr;

    if( __connector ) {

//        DL_INFO(1, ">>> try accept...");
        DPN_NodeConnector *new_connector = __connector->acceptConnection(__port);
//        DL_INFO(1, ">>> accept: [%p]", new_connector);

        return new_connector;
    }

    return nullptr;
}
DPN_SharedPort::CheckResult DPN_SharedPort::check(DPN_NodeConnector *c) {

    if(!__open) return PortClosed;

    PeerAddress a;
    a.port = c->peerPort();
    a.address = c->peerAddress();

    if(__connections_counter == __maximum) return MaximumConnections;
    if(__white_list.size()) {
        if(!__white_list.contain(a)) return MissWhiteList;
    }
    if(__black_list.size()) {
        if(__black_list.contain(a)) return ContainedInBlackList;
    }

    return CheckedSuccesfull;
}
bool DPN_SharedPort::isOpen() const {
    return __open;
}
bool DPN_SharedPort::open() {

    if(__open) return true;

    if(__connector == nullptr) {
        __connector = new DPN_NodeConnector;
    }
    if(__connector->openPort(__port)) {
        DL_INFO(1, "Port [%d] opened", __port);
        __open = true;
        return true;
    }

    return false;
}
void DPN_SharedPort::clear() {
    __port = -1;
    __autoaccepting = false;
    __connector = nullptr;
    __maximum = -1;
    __connections_counter = 0;
    __open = false;
}

