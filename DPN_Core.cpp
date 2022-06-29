#include "DPN_Core.h"
using namespace DPeerNodeSpace;

/*
//====================================================================================================== DPN_PeerMeeting
DPN_PeerMeeting::DPN_PeerMeeting() : line(this) {
    connector = nullptr;
    mi = nullptr;
    makeLine();
}
DPN_Result DPN_PeerMeeting::meet() {
    return line.go();
}
void DPN_PeerMeeting::init(MeetInfo *__mi, DPN_NodeConnector *c) {
    connector = c;
    mi = __mi;
}
//void DPN_PeerMeeting::attachToClient(DPN_AbstractClient *client) {
//    baseClient = client;
//}
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
//    if(baseClient) {
//        UNIT_ATTACH = baseClient->localAddress();
//    } else {
//        UNIT_ATTACH.clearBuffer();
//    }

    content.parseBuffers();
    return DPN_SUCCESS;
}
DPN_Result DPN_PeerMeeting::send() {
    return connector->x_send( content.buffer() );
}
DPN_Result DPN_PeerMeeting::recv_prepare() {

    UNIT_NAME.clearBuffer();
    UNIT_ATTACH.clearBuffer();
    connector->clearInnerBuffer();
    content.clearBuffer();

    return DPN_SUCCESS;
}
DPN_Result DPN_PeerMeeting::recv() {

    if( connector->readable() ) {
        DPN_Result r = connector->x_receivePacket();
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
//    base = nullptr;

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
void DPN_WaitingConnection::close() {
    if( pConnector ) {
        pConnector->close();
    }
}
bool DPN_WaitingConnection::isShadow() const {
    return mi.isShadow;
}
//DPN_AbstractClient *DPN_WaitingConnection::baseClient() {
//    return base;
//}
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
//====================================================================================================== DPN_IncomingConnection
DPN_IncomingConnection::DPN_IncomingConnection(DPN_NodeConnector *c) {
    pConnector = c;
    initConext.initiator = false;
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
//            if( base == nullptr ) {
//                if( (base = core->client(mi.r_attach)) == nullptr ) {
//                   DL_ERROR(1, "Can't find base client to attach. [%s:%d]", mi.r_attach.address.c_str(), mi.r_attach.port);
//                   return DPN_FAIL;
//                }
//                DL_INFO(1, "Find client [%p] with address: [%s:%d]", base, mi.r_attach.address.c_str(), mi.r_attach.port);
//            }

//            state = ACCEPT;
//            makeDialogLine();
//            return DPN_SUCCESS;
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
    return DPN_SUCCESS;
}
DPN_Result DPN_IncomingConnection::transmit1_s2c() {
    return pConnector->x_send(signal_content.buffer());
}
DPN_Result DPN_IncomingConnection::prepare2_afc() {
    if( state == REJECT ) return DPN_FAIL;
    pConnector->clearInnerBuffer();
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

//        initConext.clear();
//        initConext.shadowKey = shadowKey;
//        initConext.connector = pConnector;
//        initConext.avatar = avatar;
//        initConext.initiator = false;
    } else {
//        sessionKey = generateSessionKey();
//        initConext.name = mi.sr_name;
//        initConext.connector = pConnector;
//        initConext.avatar = avatar;
//        initConext.initiator = false;
//        initConext.sessionKey = sessionKey;
//        initConext.core = core;
    }
//    initConext.localVisible = UNIT_VISIBLE.get();

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
//DPN_OutgoingConnection::DPN_OutgoingConnection(DPN_AbstractClient *client, int port) {
//    clear();
//    connectTarget.address = client->remoteAddress().address;
//    connectTarget.port = port;
//    base = client;
//    meeting.attachToClient(client);
//}
DPN_OutgoingConnection::~DPN_OutgoingConnection() {
}
void DPN_OutgoingConnection::setShadowKey(const std::string &shadowKey) {
    innerShadowKey = shadowKey;
}
void DPN_OutgoingConnection::setAttempts(int a) {
    max_attempts = a;
}
void DPN_OutgoingConnection::setChannelRequester(DPN_AbstractModule *m) {
    pChannelRequester = m;
}
DPN_Result DPN_OutgoingConnection::prepare() {

    if( pConnector == nullptr ) {
        pConnector = new DPN_NodeConnector(DXT::TCP);
    }

    if( pConnector->connectTo(connectTarget) ) {

//        if(base) {
//            innerShadowKey = generateShadowKey();
//            initConext.shadowKey = innerShadowKey;
//            base->verifyShadow(pConnector->local(), pConnector->peer(), innerShadowKey);

//        }

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
    return DPN_SUCCESS;
}
DPN_Result DPN_OutgoingConnection::transmit1_sfs() {
    if( pConnector->readable() ) {
        return pConnector->x_receivePacket();
    }
    return DPN_REPEAT;
}
DPN_Result DPN_OutgoingConnection::prepare2_a2s() {

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
//    if(base) {
//        UNIT_SHADOW_KEY = innerShadowKey;
//        UNIT_SESSION_KEY = base->getSessionKey();
//    } else {
        UNIT_SHADOW_KEY.clearBuffer();
        UNIT_SESSION_KEY.clearBuffer();
//    }
    UNIT_AVATAR = pConnector->peer();

    DL_INFO(1, "keys: session: [%s] shadow: [%s]", UNIT_SESSION_KEY.get().c_str(), UNIT_SHADOW_KEY.get().c_str());


    //---------------------------------------------------------------------

    answer_content.clearBuffer();
    pConnector->clearInnerBuffer();

    answer_content.parseBuffers();

    return DPN_SUCCESS;
}
DPN_Result DPN_OutgoingConnection::transmit2_a2s() {
    return pConnector->x_send(answer_content.buffer());
}
DPN_Result DPN_OutgoingConnection::prepare3_afs() {
    answer_content.clearBuffer();
    pConnector->clearInnerBuffer();
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

//    initConext.name = mi.sr_name;
//    initConext.connector = pConnector;
//    initConext.avatar = avatar;
//    initConext.initiator = true;
//    initConext.sessionKey = sessionKey;
//    initConext.core = core;
//    initConext.localVisible = UNIT_VISIBLE.get();

//    if(base) {
//        mi.isShadow = true;
//    }

    DL_INFO(1, "session key: [%s] shadow key: [%s] avatar: [%s:%d] local: [%s] remote: [%s]",
            sessionKey.c_str(), shadowKey.c_str(), avatar.address.c_str(), avatar.port,
            pConnector->localName().c_str(), pConnector->peerName().c_str()
            );

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
    pChannelRequester = nullptr;
    max_attempts = 0;
    attempts_counter = 0;
//    initConext.initiator = true;
//    base = nullptr;
    pConnector = nullptr;
}
*/


//============================================================ DPN_ConnectionsCore
/*
DPN_ConnectionsCore::DPN_ConnectionsCore() : iGlobalModules(true), iThreadUser(true) {


    dpnAddUDPPorts( 44440, 20 );

    GlobalModule *m = defaultModules;

    DL_INFO(1, "Try create modules...");
    while( m->creator ) {

        if( !m->unique ) {
            DPN_AbstractModule *module = m->creator();
            if( module == nullptr ) {
                DL_WARNING(1, "Can't create module [%s]", m->name.c_str());
            } else {
                DL_INFO(1, "Created module: [%p][%s]", module, m->name.c_str());
                iGlobalModules.addModule(module, m->name);
            }
        }
        ++m;
    }


}
void DPN_ConnectionsCore::renewCatalog() {
//    host_catalog.renew();
}
bool DPN_ConnectionsCore::processIncomingConnections() {

    FOR_VALUE(openPorts.size(), i) {

        DPN_SharedPort &sp = openPorts[i];

        if( sp.open() == false ) {
            continue;
        }

        if( sp.newConnection() ) {

            DL_INFO(1, ">>> new incoming connection...");
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
    FOR_VALUE(aIncs.size(), i) {

        if( __processConnection(aIncs[i]) ) {
            delete aIncs[i];
            aIncs.removeByIndex(i--);
        }
    }

    return true;
}
bool DPN_ConnectionsCore::processOutgoingConnections() {

    FOR_VALUE(aOuts.size(), i) {

        if( __processConnection(aOuts[i]) ) {
            delete aOuts[i];
            aOuts.removeByIndex(i--);
        }
    }

    return true;
}
bool DPN_ConnectionsCore::__processConnection(Connections::WaitingConnection *connection) {

    __core_action();
    auto r = connection->process();
    if( r == DPN_SUCCESS ) {

        if( connection->state() == Connections::ACCEPT ) {

            if( connection->isShadow() ) {

                DL_INFO(1, "Accept shadow connection");

//                DPN_AbstractClient *c = connection->baseClient();
//                c->addShadowConnection(connection->clientContext());


            } else {
                DL_INFO(1, "Connection accepted");
                __addClient( connection->connector() );
            }

        } else if ( connection->state() == Connections::REJECT ) {
            DL_INFO(1, "Connection rejected");
        }  else {
            DL_ERROR(1, "Bad final state: [%d]", connection->state());
        }
        return true;

    } else if ( r == DPN_FAIL ) {
        DL_INFO(1, "Connection processing fault");
        connection->close();
        return true;
    }
    return false;
}
bool DPN_ConnectionsCore::__addClient(DPN_NodeConnector *c) {

    DL_INFO(1, "create client");
    DPN_ClientInterface client( iThreadUser, c);
    client.setModules( iGlobalModules );

    remotes.append( client );
    __replan_directions();
    return true;
}
bool DPN_ConnectionsCore::share_port(int port, bool autoaccepting) {

    if(!DXT_CHECK_PORT(port)) {
        DL_BADVALUE(1, "port: [%d]", port);
        return false;
    }
    FOR_VALUE(openPorts.size(), i) {

        if( openPorts[i].port() == port) {
            DL_INFO(1, "Port [%d] already processing", port);
            return true;
        }
    }

    openPorts.push_back(DPN_SharedPort( port, autoaccepting));
    return true;
}
bool DPN_ConnectionsCore::connect_to(const char *address, int port) {

    auto out = Connections::createOutgoingConnection( name, PeerAddress( port, address) );
    aOuts.append( out );

    return true;
}
bool DPN_ConnectionsCore::send_message(const char *message) {

    if( remotes.empty() ) {
        DL_ERROR(1, "No clients");
        return false;
    }
    remotes[0].sendMessage( message );
    return true;
}
bool DPN_ConnectionsCore::sync() {

    if( remotes.empty() ) {
        DL_ERROR(1, "No clients");
        return false;
    }
    DPN_ClientInterface &ci = remotes[0];
    auto fm = extractFileModule( iGlobalModules );
    if( fm == nullptr ) {
        DL_BADPOINTER(1, "File Module");
        return false;
    }
//    DL_INFO(1, "File system: [%p]", fm);
    auto i = fm->getIf( ci );
    if( i.badInterface() ) {
        DL_ERROR(1, "Bad interface");
        return false;
    }

//    DL_INFO(1, "Start sync...");
    bool r = i.sync();
//    DL_INFO(1, "sync: [%d]", r);
    return r;
}
void DPN_ConnectionsCore::ping() {
//    remotes[0].ping();
}
void DPN_ConnectionsCore::acceptAll() {


    if(aIncs.empty()) {
        DL_INFO(1, "No incoming connections");
        return;
    }
    FOR_VALUE(aIncs.size(), i) {
        aIncs[i]->accept();
    }

}
void DPN_ConnectionsCore::rejectAll() {

    if(aIncs.empty()) {
        DL_INFO(1, "No incoming connections");
        return;
    }
    FOR_VALUE(aIncs.size(), i) {
        aIncs[i]->reject();
    }
}
void DPN_ConnectionsCore::disconnectAll() {


//    FOR_VALUE(remotes.size(), i) {
//        if( remotes[i].disconnect() ) {
//            remotes.removeByIndex(i--);
//        }
//    }


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

    if(connector == nullptr) {
        DL_BADPOINTER(1, "connector");
        return false;
    }
    auto inc = Connections::createIncomingConnection( name, connector );
    aIncs.append( inc );
    return true;
}
*/

//bool DPN_ConnectionsCore::__add_client() {



//    DL_INFO(1, "name: [%s] local: [%s] remote: [%s] avatart: [%s:%d] session key: [%s]",
//            c.name.c_str(), c.connector->localName().c_str(), c.connector->peerName().c_str(), c.avatar.address.c_str(), c.avatar.port,
//            c.sessionKey.c_str()
//            );


//    DPN_ClientInterface client;
//    remotes.push_back(client);
//    __replan_directions();

////    DPN_Client client(c);

//    //------ init client:
////    client.setThreadContext(userThreadContext);
////    GlobalModule *m = defaultModules;
////    while( m->creator ) {

////        if( m->unique ) {
////            DPN_AbstractModule *module = m->creator();
////            if( module == nullptr ) {
////                DL_WARNING(1, "Can't create module [%s]", m->name.c_str());
////            } else {
////                module->setCore(this);
////                client.addModule(m->name, module);
////            }
////        }
////        else client.addModule( m->name, iGlobalModules.module(m->name));
////        ++m;
////    }




//    return true;
//}
/*
void DPN_ConnectionsCore::__core_action(int s) {
    random.push_back(s + (time(NULL) % 323232) );
}
void DPN_ConnectionsCore::__replan_directions() {

//    if(remotes.empty() || tbs.empty()) return;

//    DPN_THREAD_GUARD(__mutex);

//    FOR_VALUE(tbs.size(), i) tbs[i].clearPlan();

//    int threadIndex = 0;
//    for(int i=0; i != remotes.size(); ++i) {

//        DPN_ClientInterface &c = remotes[i];
////        if( c.state() == DISCONNECTED ) continue;

//        auto &units = c.threadUnits();
//        FOR_VALUE( units.size(), i ) {

//            DL_INFO(1, "thread unit: [%p]", units[i]);

//            if( tbs[threadIndex].planDirection( units[i] ) ) {
//                if( ++threadIndex == tbs.size() ) {
//                    threadIndex = 0;
//                }
//            }


//        }
//    }

//    FOR_VALUE(tbs.size(), i) tbs[i].replacePlaned();
}
void DPN_ConnectionsCore::__replan_inner() {

//    if(tbs.empty()) return;

//    int threadIndex = 0;

//    tbs[threadIndex].setProcessTasks(true);
}
*/

//============================================================ DPN_SharedPort

DPN_SharedPort::DPN_SharedPort(int port, bool autoaccepting) {
    clear();
    iPort = port;
    iAutoaccepting = autoaccepting;
}
bool DPN_SharedPort::newConnection() {
    if(!iOpened) return false;
    return pConnector ? pConnector->newConnection(iPort) : false;
}
DPN_NodeConnector *DPN_SharedPort::accept() {

    if(!iOpened) return nullptr;
    if( pConnector ) {
        DPN_NodeConnector *new_connector = pConnector->acceptConnection(iPort);
        return new_connector;
    }

    return nullptr;
}
DPN_SharedPort::CheckResult DPN_SharedPort::check(DPN_NodeConnector *c) {

    if(!iOpened) return PortClosed;

    PeerAddress a;
    a.port = c->peerPort();
    a.address = c->peerAddress();

    if( iConnections == iMaximum ) return MaximumConnections;
    if(aWhiteList.size()) {
        if(!aWhiteList.contain(a)) return MissWhiteList;
    }
    if(aBlackList.size()) {
        if(aBlackList.contain(a)) return ContainedInBlackList;
    }

    return CheckedSuccesfull;
}
bool DPN_SharedPort::isOpen() const {
    return iOpened;
}
bool DPN_SharedPort::open() {

    if(iOpened) return true;

    if(pConnector == nullptr) {
        pConnector = new DPN_NodeConnector(DXT::TCP);
    }
    if(pConnector->openPort(iPort)) {
        DL_INFO(1, "Port [%d] opened", iPort);
        iOpened = true;
        return true;
    }

    return false;
}
void DPN_SharedPort::clear() {
    iPort = -1;
    iAutoaccepting = false;
    pConnector = nullptr;
    iMaximum = -1;
    iConnections = 0;
    iOpened = false;
}

//============================================================
Connections::WaitingConnection::WaitingConnection() {
    pConnector = nullptr;
    iState = WAIT;
    makeMeetLine();
}
Connections::WaitingConnection::~WaitingConnection() {

}
void Connections::WaitingConnection::close() {
    if( pConnector ) {
        pConnector->close();
    }
}
bool Connections::WaitingConnection::isShadow() const {
    return false;
}
DPN_Result Connections::WaitingConnection::innerMeeting() {
    return iMeetingLine.go();
}
DPN_Result Connections::WaitingConnection::sendPacket() {
//    DPN_CALL_LOG;

    auto r = pConnector->x_send( iMainContent.buffer() );

//    DL_INFO(1, "sb: [%d] buffer: [%d]", pConnector->transportedBytes(),
//            iMainContent.buffer().size());

    return r;
}
DPN_Result Connections::WaitingConnection::receivePacket() {
//    DPN_CALL_LOG;

    if( pConnector->readable() ) {
        return pConnector->x_receivePacket();
    }
    return DPN_REPEAT;
}
void Connections::WaitingConnection::setLocalName(const std::string &name) {
    UNIT_MEET_NAME = name;
    iLocalName = name;
}
void Connections::WaitingConnection::makeMeetLine() {
    iMeetingLine << &WaitingConnection::meetPresend
                 << &WaitingConnection::meetSend
                 << &WaitingConnection::meetPrereceive
                 << &WaitingConnection::meetReceive
                 << &WaitingConnection::meetEnding
                    ;

    iMeetingLine.setTarget( this );
}
DPN_Result Connections::WaitingConnection::meetPresend() {
//    DPN_CALL_LOG;

    iMeetContent.parseBuffers();
    return DPN_SUCCESS;
}
DPN_Result Connections::WaitingConnection::meetSend() {
//    DPN_CALL_LOG;
    return pConnector->x_send( iMeetContent.buffer() );
}
DPN_Result Connections::WaitingConnection::meetPrereceive() {
//    DPN_CALL_LOG;

    UNIT_MEET_NAME.clearBuffer();
    UNIT_MEET_ATTACH.clearBuffer();
    pConnector->clearInnerBuffer();
    iMeetContent.clearBuffer();
    return DPN_SUCCESS;
}
DPN_Result Connections::WaitingConnection::meetReceive() {
//    DPN_CALL_LOG;

    if( pConnector->readable() ) {
        return pConnector->x_receivePacket();
    }
    return DPN_REPEAT;
}
DPN_Result Connections::WaitingConnection::meetEnding() {
//    DPN_CALL_LOG;

    if( iMeetContent.deparseBuffer( pConnector->buffer() ) == false ) {
        DL_ERROR(1, "Can't deparse content");
        return DPN_FAIL;
    }
    iRemoteName = UNIT_MEET_NAME.get();
    iAttachAddress = UNIT_MEET_ATTACH.get();

//    DL_INFO(1, "Meet: name: [%s] attach: [%s]", iRemoteName.c_str(), iAttachAddress.name().c_str());
    return DPN_SUCCESS;
}
//============================================================
Connections::OutgoingConnection::OutgoingConnection(const std::string &localName, const PeerAddress &address) {
    setLocalName( localName );
    iConnectionAddress = address;
    iConnectionAttepmts = 0;
    iConnectionMaximumAttemps = 1;

    makeMainLine();
}
DPN_Result Connections::OutgoingConnection::process() {
    return iActionLine.go();
}
DPN_Result Connections::OutgoingConnection::connecting() {

//    DPN_CALL_LOG;

    if( pConnector == nullptr ) {
        pConnector = new DPN_NodeConnector(DXT::TCP);
    }
    if( pConnector->connectTo(iConnectionAddress) ) {
//        DL_INFO(1, "connected: [%s]", pConnector->peerName().c_str());
        return DPN_SUCCESS;
    } else if( iConnectionAttepmts++ != iConnectionMaximumAttemps ) {
        return DPN_REPEAT;
    }
    return DPN_FAIL;
//    return DPN_SUCCESS;
}
DPN_Result Connections::OutgoingConnection::waiting(){

//    DPN_CALL_LOG;
//    makeDialogLine();

    if( pConnector->readable() ) {
        makeDialogLine();
        return DPN_SUCCESS;
    }
    return DPN_REPEAT;

//    return DPN_SUCCESS;
}
DPN_Result Connections::OutgoingConnection::dialog() {
    return iDialogLine.go();
}
DPN_Result Connections::OutgoingConnection::ending() {
    DPN_CALL_LOG;

    return DPN_SUCCESS;
}
DPN_Result Connections::OutgoingConnection::dialog__clear() {
//    DPN_CALL_LOG;

    iMainContent.clearBuffer();
    pConnector->clearInnerBuffer();
    return DPN_SUCCESS;
}
DPN_Result Connections::OutgoingConnection::dialog__process_signal() {
//    DPN_CALL_LOG;


    if( iMainContent.deparseBuffer( pConnector->buffer() ) == false ) {
        DL_ERROR(1, "Can't deparse buffer");
        return DPN_FAIL;
    }
    std::string signal = UNIT_SIGNAL.get();
    std::string innerSignal;
    innerSignal.append(pConnector->localName());
    innerSignal.append(localName());
//    DL_INFO(1, "presignal: [%s]", innerSignal.c_str());
    DPN_SHA256 hashtool;
    hashtool.hash_string( innerSignal );
    innerSignal = hashtool.get();
//    DL_INFO(1, "signal hash: [%s] remote signal: [%s]", innerSignal.c_str(), signal.c_str());

    if( signal == innerSignal ) {
        iState = ACCEPT;
    } else {
        DL_ERROR(1, "Bad signal or rejected: [%s]", signal);
        iState = REJECT;
        return DPN_FAIL;
    }

    UNIT_AVATAR = PeerAddress( pConnector->peerPort(), pConnector->peerAddress() );
    UNIT_VISIBLE = true;
    UNIT_SHADOW_KEY = "xxx outgoing shadowkey xxx";
    UNIT_SESSION_KEY = "xxx outgoing session key xxx";

    iMainContent.parseBuffers();

    return DPN_SUCCESS;
}
DPN_Result Connections::OutgoingConnection::dialog__end() {
//    DPN_CALL_LOG;

    if( iMainContent.deparseBuffer( pConnector->buffer() ) == false ) {
        DL_ERROR(1, "deparsing");
        return DPN_FAIL;
    }

    std::string sessionKey = UNIT_SESSION_KEY.get();
    std::string shadowKey = UNIT_SHADOW_KEY.get();
    PeerAddress avatar = UNIT_AVATAR.get();

    DL_INFO(1, "session key: [%s] shadow: [%s] avatar: [%s]", sessionKey.c_str(), shadowKey.c_str(), avatar.name().c_str());

    return DPN_SUCCESS;
}
void Connections::OutgoingConnection::makeDialogLine() {

    iDialogLine << &OutgoingConnection::dialog__clear
                << &OutgoingConnection::receivePacket

                << &OutgoingConnection::dialog__process_signal
                << &OutgoingConnection::sendPacket

                << &OutgoingConnection::dialog__clear
                << &OutgoingConnection::receivePacket

                << &OutgoingConnection::dialog__end
                   ;
    iDialogLine.setTarget( this );
}
void Connections::OutgoingConnection::makeMainLine() {

    iActionLine << &OutgoingConnection::connecting
                << &OutgoingConnection::innerMeeting
                << &OutgoingConnection::waiting
                << &OutgoingConnection::dialog
                << &OutgoingConnection::ending
                   ;
    iActionLine.setTarget( this );
}
//===========================================================
Connections::IncomingConnection::IncomingConnection(const std::string &localName, DPN_NodeConnector *connector) {
    setLocalName( localName );
    makeMainLine();
    pConnector = connector;
}
DPN_Result Connections::IncomingConnection::process() {
    return iActionLine.go();
}
void Connections::IncomingConnection::accept() {
    if( iState == WAIT ) iState = ACCEPT;
}
void Connections::IncomingConnection::reject() {
    if( iState == WAIT ) iState = REJECT;
}
DPN_Result Connections::IncomingConnection::waiting() {

//    DPN_CALL_LOG;
    switch (iState) {
    case WAIT: return DPN_REPEAT;
    case ACCEPT: makeDialogLine(); return DPN_SUCCESS;
    case REJECT: return DPN_SUCCESS;
    default: return DPN_FAIL;
    }
    return DPN_FAIL;
}
DPN_Result Connections::IncomingConnection::dialog() {
    return iDialogLine.go();
}
DPN_Result Connections::IncomingConnection::ending() {
    return DPN_SUCCESS;
}
DPN_Result Connections::IncomingConnection::dialog__clear() {
    if( iState == REJECT ) return DPN_FAIL;
    iMainContent.clearBuffer();
    pConnector->clearInnerBuffer();
    return DPN_SUCCESS;
}
DPN_Result Connections::IncomingConnection::dialog__make_signal() {

    std::string signal;
    if( iState == ACCEPT ) {
        signal.append(pConnector->peerName());
        signal.append(remoteName());

//        DL_INFO(1, "presignal: [%s]", signal.c_str());

        DPN_SHA256 hashtool;
        if( hashtool.hash_string( signal ) == false ) {
            DL_FUNCFAIL(1, "hash_string");
            return DPN_FAIL;
        }
        signal = hashtool.get();
//        DL_INFO(1, "signal hash: [%s]", signal.c_str());

    } else if ( iState == REJECT ) {
        signal = "reject";
    }
    UNIT_SIGNAL = signal;
    iMainContent.parseBuffers();
    return DPN_SUCCESS;
}
DPN_Result Connections::IncomingConnection::dialog__make_answer() {
    if( iMainContent.deparseBuffer( pConnector->buffer() ) == false ) {
        DL_ERROR(1, "deparsing");
        return DPN_FAIL;
    }

    std::string sessionKey = UNIT_SESSION_KEY.get();
    std::string shadowKey = UNIT_SHADOW_KEY.get();
    PeerAddress avatar = UNIT_AVATAR.get();

    DL_INFO(1, "session key: [%s] shadow: [%s] avatar: [%s]", sessionKey.c_str(), shadowKey.c_str(), avatar.name().c_str());

    UNIT_SHADOW_KEY = "incoming shadow key";
    UNIT_SESSION_KEY = "incoming session key";
    UNIT_AVATAR = PeerAddress( pConnector->peerPort(),
                               pConnector->peerAddress());

    iMainContent.parseBuffers();

    return DPN_SUCCESS;
}
void Connections::IncomingConnection::makeDialogLine() {

    iDialogLine << &IncomingConnection::dialog__make_signal
                << &IncomingConnection::sendPacket

                << &IncomingConnection::dialog__clear
                << &IncomingConnection::receivePacket

                << &IncomingConnection::dialog__make_answer
                << &IncomingConnection::sendPacket
                   ;

    iDialogLine.setTarget( this );
}
void Connections::IncomingConnection::makeMainLine() {
    iActionLine << &IncomingConnection::innerMeeting
                << &IncomingConnection::waiting
                << &IncomingConnection::dialog
                << &IncomingConnection::ending
                   ;
    iActionLine.setTarget( this );
}
Connections::OutgoingConnection *Connections::createOutgoingConnection(const std::string &localName, const PeerAddress &a) {
    return new OutgoingConnection( localName, a );
}
Connections::IncomingConnection *Connections::createIncomingConnection(const std::string &localName, DPN_NodeConnector *c) {
    return new IncomingConnection( localName, c );
}

//===============================================
__dpn_core__::__dpn_core__() : DPN::Thread::ThreadUser(true) {

}
void __dpn_core__::createThread(DPN::Thread::Policy p) {
    DPN::Thread::ThreadUser::startStream( p );
}
