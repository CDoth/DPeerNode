#include "DPN_Network.h"

using namespace DPN::Logs;

namespace DPN::Network {


    DPN_Result ProxyPacketFilter::process(DPN_ExpandableBuffer &b) {

        if( b.empty() ) return DPN_SUCCESS;

        if( (size_t)b.size() < sizeof (__packet_header2) ) {
            DL_ERROR(1, "Bad packet size: [%d]", b.size());
            return DPN_FAIL;
        }
        const __packet_header2 *h = reinterpret_cast<const __packet_header2*>( b.getData() );
        DL_INFO(1, "header: type: [%s] transaction: [%d]",
                packetTypeName( h->type ),  h->transaction);

        switch( h->type ) {
            case PT__MAKE_SHADOW_CONNECTION:
                DL_WARNING(1, "Need special PT__MAKE_SHADOW_CONNECTION processing code...");
                return DPN_FAIL;
            break;
            default: return DPN_SUCCESS;
        }
        return DPN_FAIL;
    }
    //======================================================================================= __proxy_node__
    __proxy_node__::__proxy_node__()
    {}
    __proxy_node__::__proxy_node__(Underlayer &ul)
        : DPN::Client::Underlayer( ul )
    {}
    DPN_Result __proxy_node__::connectOne(DPN_NodeConnector *connector, P2PRole p2pRole) {

        switch( p2pRole ) {
        case DPN::ADAM:
            if( wAChannel.init( connector, "A2E") == false ) {
                DL_FUNCFAIL(1, "init (A)");
                return DPN_FAIL;
            }
            wAForward = wAChannel.getMonoIf( DPN::FORWARD );
            wABack = wAChannel.getMonoIf( DPN::BACKWARD );
            break;
        case DPN::EVA:
            if( wEChannel.init( connector, "E2A") == false ) {
                DL_FUNCFAIL(1, "init (E)");
                return DPN_FAIL;
            }
            wEForward = wEChannel.getMonoIf( DPN::FORWARD );
            wEBack = wEChannel.getMonoIf( DPN::BACKWARD );
            break;
        }
        if( wAChannel.isReady() && wEChannel.isReady() ) return DPN_SUCCESS;
        return DPN_REPEAT;
    }
    bool __proxy_node__::start() {

        if( !wAChannel.isReady() || !wEChannel.isReady() ) {
            DL_ERROR(1, "Channels is not ready");
            return false;
        }
        if( wAForward.badInterface() || wABack.badInterface() || wEForward.badInterface() || wEBack.badInterface() ) {
            DL_ERROR(1, "Bad channel interface(s)");
            return false;
        }


        wA2E.data()->setEntry( wABack.io() );
        wA2E.data()->connect( &wA2EFilter );
        wA2E.data()->connect( wEForward.io() );

        wE2A.data()->setEntry( wEBack.io() );
        wE2A.data()->connect( &wE2AFilter );
        wE2A.data()->connect( wAForward.io() );

        if( putUnit( wA2E.unit() ) == false ) {
            DL_FUNCFAIL(1, "putUnit (A2E)");
            return false;
        }
        if( putUnit( wE2A.unit() ) == false ) {
            DL_FUNCFAIL(1, "putUnit (E2A)");
            return false;
        }

        return true;
    }
    //======================================================================================= ProxyNode
    ProxyNode::ProxyNode() {}
    ProxyNode::ProxyNode(Client::Underlayer &ul)
        : DWatcher< __proxy_node__ >(true, ul)
    {}
    DPN_Result ProxyNode::connectOne(DPN_NodeConnector *connector, P2PRole p2pRole) {
        if( isEmptyObject() ) {
            DL_ERROR(1, "Empty watcher");
            return DPN_FAIL;
        }
        return data()->connectOne( connector, p2pRole );
    }
    bool ProxyNode::start() {
        if( isEmptyObject() ) {
            DL_ERROR(1, "Empty watcher");
            return false;
        }
        return data()->start();
    }
    //============================================================================================================== SharedPort
    SharedPort::SharedPort() {
        clear();
    }
    SharedPort::SharedPort(int port, bool autoaccepting) {
        clear();
        iPort = port;
        iAutoaccepting = autoaccepting;
    }
    bool SharedPort::newConnection() {
        if(!iOpened) return false;
        return pConnector ? pConnector->newConnection(iPort) : false;
    }
    DPN_NodeConnector *SharedPort::accept() {
        if(!iOpened) return nullptr;
        if( pConnector ) {
            DPN_NodeConnector *new_connector = pConnector->acceptConnection(iPort);
            return new_connector;
        }

        return nullptr;
    }
    SharedPort::CheckResult SharedPort::check(DPN_NodeConnector *c) {
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
    bool SharedPort::isOpen() const {
        return iOpened;
    }
    bool SharedPort::open() {

//        DL_INFO(1, "SharedPort: try open port... opened: [%d] port: [%d]", iOpened, iPort);
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
    void SharedPort::clear() {
        iPort = -1;
        iAutoaccepting = false;
        pConnector = nullptr;
        iMaximum = -1;
        iConnections = 0;
        iOpened = false;
    }
    //============================================================================================================== WaitingConnection
    WaitingConnection::WaitingConnection() {
        pConnector = nullptr;
        iState = WAIT;
        makeMeetLine();
    }
    WaitingConnection::~WaitingConnection() {

    }
    void WaitingConnection::close() {
        if( pConnector ) {
            pConnector->close();
        }
    }
    bool WaitingConnection::isShadow() const {
        return false;
    }
    DPN_Result WaitingConnection::innerMeeting() {
        return iMeetingLine.go();
    }
    DPN_Result WaitingConnection::sendPacket() {
    //    DPN_CALL_LOG;

        auto r = pConnector->x_send( iMainContent.buffer() );

    //    DL_INFO(1, "sb: [%d] buffer: [%d]", pConnector->transportedBytes(),
    //            iMainContent.buffer().size());

        return r;
    }
    DPN_Result WaitingConnection::receivePacket() {
    //    DPN_CALL_LOG;

        if( pConnector->readable() ) {
            return pConnector->x_receivePacket();
        }
        return DPN_REPEAT;
    }
    void WaitingConnection::setLocalName(const std::string &name) {
        UNIT_MEET_NAME = name;
        iLocalName = name;
    }
    void WaitingConnection::makeMeetLine() {
        iMeetingLine << &WaitingConnection::meetPresend
                     << &WaitingConnection::meetSend
                     << &WaitingConnection::meetPrereceive
                     << &WaitingConnection::meetReceive
                     << &WaitingConnection::meetEnding
                        ;

        iMeetingLine.setTarget( this );
    }
    DPN_Result WaitingConnection::meetPresend() {
    //    DPN_CALL_LOG;

        iMeetContent.parseBuffers();
        return DPN_SUCCESS;
    }
    DPN_Result WaitingConnection::meetSend() {
    //    DPN_CALL_LOG;
        return pConnector->x_send( iMeetContent.buffer() );
    }
    DPN_Result WaitingConnection::meetPrereceive() {
    //    DPN_CALL_LOG;

        UNIT_MEET_NAME.clearBuffer();
        UNIT_MEET_ATTACH.clearBuffer();
        pConnector->clearInnerBuffer();
        iMeetContent.clearBuffer();
        return DPN_SUCCESS;
    }
    DPN_Result WaitingConnection::meetReceive() {
    //    DPN_CALL_LOG;

        if( pConnector->readable() ) {
            return pConnector->x_receivePacket();
        }
        return DPN_REPEAT;
    }
    DPN_Result WaitingConnection::meetEnding() {
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
    //============================================================================================================== OutgoingConnection
    OutgoingConnection::OutgoingConnection(const std::string &localName, const PeerAddress &address, const ConnectionContext &context ) {
        setLocalName( localName );
        iConnectionAddress = address;
        iConnectionAttepmts = 0;
        iConnectionMaximumAttemps = 1;
        iContext = context;

        makeMainLine();
    }
    DPN_Result OutgoingConnection::process() {
        return iActionLine.go();
    }
    DPN_Result OutgoingConnection::connecting() {

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
    }
    DPN_Result OutgoingConnection::waiting(){

    //    DPN_CALL_LOG;
    //    makeDialogLine();

        if( pConnector->readable() ) {
            makeDialogLine();
            return DPN_SUCCESS;
        }
        return DPN_REPEAT;

    //    return DPN_SUCCESS;
    }
    DPN_Result OutgoingConnection::dialog() {
        return iDialogLine.go();
    }
    DPN_Result OutgoingConnection::ending() {
        DPN_CALL_LOG;

        return DPN_SUCCESS;
    }
    DPN_Result OutgoingConnection::dialog__clear() {
    //    DPN_CALL_LOG;

        iMainContent.clearBuffer();
        pConnector->clearInnerBuffer();
        return DPN_SUCCESS;
    }
    DPN_Result OutgoingConnection::dialog__process_signal() {
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
        DPN::SHA256 hashtool;
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
        UNIT_SHADOW_KEY = "xxx outgoing shadowkey xxx";
        UNIT_SESSION_KEY = "xxx outgoing session key xxx";

        iMainContent.parseBuffers();

        return DPN_SUCCESS;
    }
    DPN_Result OutgoingConnection::dialog__end() {
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
    void OutgoingConnection::makeDialogLine() {

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
    void OutgoingConnection::makeMainLine() {

        iActionLine << &OutgoingConnection::connecting
                    << &OutgoingConnection::innerMeeting
                    << &OutgoingConnection::waiting
                    << &OutgoingConnection::dialog
                    << &OutgoingConnection::ending
                       ;
        iActionLine.setTarget( this );
    }
    //============================================================================================================== IncomingConnection
    IncomingConnection::IncomingConnection(const std::string &localName, DPN_NodeConnector *connector) {
        setLocalName( localName );
        makeMainLine();
        pConnector = connector;
    }
    DPN_Result IncomingConnection::process() {
        return iActionLine.go();
    }
    void IncomingConnection::accept() {
        if( iState == WAIT ) iState = ACCEPT;
    }
    void IncomingConnection::reject() {
        if( iState == WAIT ) iState = REJECT;
    }
    DPN_Result IncomingConnection::waiting() {

    //    DPN_CALL_LOG;
        switch (iState) {
        case WAIT: return DPN_REPEAT;
        case ACCEPT: makeDialogLine(); return DPN_SUCCESS;
        case REJECT: return DPN_SUCCESS;
        default: return DPN_FAIL;
        }
        return DPN_FAIL;
    }
    DPN_Result IncomingConnection::dialog() {
        return iDialogLine.go();
    }
    DPN_Result IncomingConnection::ending() {
        return DPN_SUCCESS;
    }
    DPN_Result IncomingConnection::dialog__clear() {
        if( iState == REJECT ) return DPN_FAIL;
        iMainContent.clearBuffer();
        pConnector->clearInnerBuffer();
        return DPN_SUCCESS;
    }
    DPN_Result IncomingConnection::dialog__make_signal() {

        std::string signal;
        if( iState == ACCEPT ) {
            signal.append(pConnector->peerName());
            signal.append(remoteName());

    //        DL_INFO(1, "presignal: [%s]", signal.c_str());

            DPN::SHA256 hashtool;
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
    DPN_Result IncomingConnection::dialog__make_answer() {
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
    void IncomingConnection::makeDialogLine() {

        iDialogLine << &IncomingConnection::dialog__make_signal
                    << &IncomingConnection::sendPacket

                    << &IncomingConnection::dialog__clear
                    << &IncomingConnection::receivePacket

                    << &IncomingConnection::dialog__make_answer
                    << &IncomingConnection::sendPacket
                       ;

        iDialogLine.setTarget( this );
    }
    void IncomingConnection::makeMainLine() {
        iActionLine << &IncomingConnection::innerMeeting
                    << &IncomingConnection::waiting
                    << &IncomingConnection::dialog
                    << &IncomingConnection::ending
                       ;
        iActionLine.setTarget( this );
    }
    OutgoingConnection *createOutgoingConnection(const std::string &localName, const PeerAddress &a, const ConnectionContext &context ) {
        return new OutgoingConnection( localName, a, context );
    }
    IncomingConnection *createIncomingConnection(const std::string &localName, DPN_NodeConnector *c) {
        return new IncomingConnection( localName, c );
    }
    //============================================================================================================== ClientCenter
    __client_center__::__client_center__() :
        DPN::Client::GlobalUnderlayerSpace(true),
        DPN::Thread::ThreadUser(true),
        DPN::Modules(true)
    {}
    void __client_center__::setName(const std::string &name) {

        DPN_THREAD_GUARD( iMutex );

        iName = name;
    }
    bool __client_center__::sharePort(int port, bool aa ) {

//        DPN_THREAD_GUARD( iMutex );

        if(!DXT_CHECK_PORT(port)) {
            DL_BADVALUE(1, "port: [%d]", port);
            return false;
        }
        FOR_VALUE( aSharedPorts.size(), i ) {

            if( aSharedPorts.output()[i].port() == port) {
                DL_INFO(1, "Port [%d] already processing", port);
                return true;
            }
        }

        aSharedPorts.push_back( DPN::Network::SharedPort( port, aa ) );
        return true;
    }
    bool __client_center__::connectTo(const char *address, int port, const ConnectionContext &context ) {
        return pushOut( address, port, context );
    }
    void __client_center__::acceptAll() {
        if(aIncs.output().empty()) {
            DL_INFO(1, "No incoming connections");
            return;
        }
        FOR_VALUE(aIncs.size(), i) {
            aIncs.output()[i]->accept();
        }
    }
    const DArray<Client::Interface> &__client_center__::remotes() const {

        DPN_THREAD_GUARD( iMutex );

        return aRemotes;
    }
    bool __client_center__::work() {
        listenPorts();

        aIncs.accept();
        FOR_VALUE( aIncs.size(), i ) {
            if( processConnection( aIncs.output()[i] ) ) {
                delete aIncs.output()[i];
                aIncs.remove( i-- );
            }
        }
        aOuts.accept();
        FOR_VALUE( aOuts.size(), i ) {
            if( processConnection( aOuts.output()[i] ) ) {
                delete aOuts.output()[i];
                aOuts.remove( i-- );
            }
        }
        processEvents();
        return true;
    }
    bool __client_center__::addVirtualClient(DPN_NodeConnector *connector, const ConnectionContext &context) {

        ProxyNode pn = context.proxyNode();
//        switch( pn.connectOne( connector ) ) {
//        case DPN_SUCCESS:
//            aProxys.append( pn );
//            pn.start();
//            break;
//        case DPN_REPEAT:
//            return true;
//            break;
//        case DPN_FAIL:
//            return false;
//            break;
//        }

//        GlobalUnderlayerSpace::generateEvent()
    }
    bool __client_center__::newClient( DPN_NodeConnector *connector ) {

        DPN_THREAD_GUARD( iMutex );

        DPN::Client::Interface client( *this, *this, *this, connector );
        aRemotes.append( client );
        DPN::Client::GlobalUnderlayerSpace::addPeerNote( client.peer() );
        return true;
    }
    bool __client_center__::removeClient(Client::Tag tag) {

        DPN_THREAD_GUARD( iMutex );

        FOR_VALUE( aRemotes.size(), i ) {
            if( aRemotes[i].tag() == tag ) {
                aRemotes.removeByIndex( i );
                return true;
            }
        }
        return false;
    }
    bool __client_center__::processConnection(WaitingConnection *w){
        auto r = w->process();
        if( r == DPN_SUCCESS ) {

            if( w->state() == DPN::Network::ACCEPT ) {

                if( w->isShadow() ) {
                    DL_INFO(1, "Accept shadow connection");
                } else if( w->isVirtual() ) {
                    addVirtualClient( w->connector(), w->context() );
                } else {
                    DL_INFO(1, "Connection accepted");
                    newClient( w->connector() );
                }

            } else if ( w->state() == DPN::Network::REJECT ) {
                DL_INFO(1, "Connection rejected");
            }  else {
                DL_ERROR(1, "Bad final state: [%d]", w->state());
            }
            return true;

        } else if ( r == DPN_FAIL ) {
            DL_INFO(1, "Connection processing fault");
            w->close();
            return true;
        }
        return false;
    }
    bool __client_center__::listenPorts() {

        aSharedPorts.accept();
        FOR_VALUE( aSharedPorts.size(), i) {

            SharedPort &sp = aSharedPorts.output()[i];

            if( sp.open() == false ) {

                DL_ERROR(1, "Can't open port");
                aSharedPorts.remove( i-- );
                continue;
            }

            if( sp.newConnection() ) {

                DL_INFO(1, ">>> new incoming connection...");
                DPN_NodeConnector *connector = sp.accept();
                if(connector == nullptr) {
                    DL_ERROR(1, "Can't accept connection");
                    return false;
                }

                SharedPort::CheckResult r;
                if( (r = sp.check(connector)) != SharedPort::CheckedSuccesfull) {
                    DL_WARNING(1, "Can't accept connection");
                } else {
                    pushInc( connector, sp.isAuto() );
                }
            }
        }
        return true;
    }
    bool __client_center__::processEvents() {

        DPN::Client::ClientEvent e = takeLastEvent();
        if( e.iType == DPN::Client::CL_EV__NO ) return true;

        switch (e.iType) {
        case DPN::Client::CL_EV__DISCONNECT: {
            DL_INFO(1,"Disconnect client event: [%p]", e.iTag );
            removeClient( e.iTag );
            DPN::Modules::clientDisconnected( e.iTag );
            break;
        }
        default:
            break;
        }
        return true;
    }
    bool __client_center__::pushInc(DPN_NodeConnector *c, bool accept) {

//        DPN_THREAD_GUARD( iMutex );

        auto inc = createIncomingConnection( iName, c );
        if( accept ) inc->accept();
        aIncs.push_back( inc );

        return true;
    }
    bool __client_center__::pushOut(const std::string &address, int port, const ConnectionContext &context ) {

//        DPN_THREAD_GUARD( iMutex );


        auto out = DPN::Network::createOutgoingConnection( iName, PeerAddress( port, address), context );
        aOuts.push_back( out );
        return true;

    }
    //============================================================================================================== ClientCenter
    ClientCenter::ClientCenter(bool makeSource)
        : dClientCenter(makeSource)
    {}

    void ClientCenter::setName(const std::string &name) {
        if( isEmptyObject() ) {
            DL_ERROR( 1, "Empty watcher");
            return;
        }
        data()->setName( name );
    }
    bool ClientCenter::sharePort(int port, bool autoaccept) {
        if( isEmptyObject() ) {
            DL_ERROR( 1, "Empty watcher");
            return false;
        }
        return data()->sharePort( port, autoaccept );
    }
    bool ClientCenter::connectTo( const PeerAddress &pa ) {
        if( isEmptyObject() ) {
            DL_ERROR( 1, "Empty watcher");
            return false;
        }
        ConnectionContext emptyContext;
        return data()->connectTo( pa.address.c_str(), pa.port, emptyContext );
    }
    bool ClientCenter::connectTo(const PeerAddress &pa, const ConnectionContext &context) {
        if( isEmptyObject() ) {
            DL_ERROR( 1, "Empty watcher");
            return false;
        }
        return data()->connectTo( pa.address.c_str(), pa.port, context );
    }
    void ClientCenter::acceptAll() {
        if( isEmptyObject() ) {
            DL_ERROR( 1, "Empty watcher");
            return;
        }
        return data()->acceptAll();
    }
    Client::GlobalUnderlayerSpace ClientCenter::globalUnderlayer() {
        if( isEmptyObject() ) {
            DL_ERROR(1, "Empty watcher");
            return Client::GlobalUnderlayerSpace();
        }
        return *data();
    }
    DPN::Modules ClientCenter::modules() {
        if( isEmptyObject() ) {
            DL_ERROR(1, "Empty watcher");
            return DPN::Modules();
        }
        return *data();
    }
    Thread::ThreadUser ClientCenter::threadUser() {
        if( isEmptyObject() ) {
            DL_ERROR(1, "Empty watcher");
            return Thread::ThreadUser();
        }
        return *data();
    }
    Client::Tag ClientCenter::pa2tag(const PeerAddress &pa) const {
        if( isEmptyObject() ) {
            DL_ERROR(1, "Empty watcher");
            return nullptr;
        }
        const DArray<Client::Interface> rmts = data()->remotes();
        FOR_VALUE( rmts.size(), i ) {
            if( rmts[i].peer() == pa ) return rmts[i].tag();
        }
        return nullptr;
    }
    Client::Interface ClientCenter::tag2client(Client::Tag tag) const {
        if( isEmptyObject() ) {
            DL_ERROR(1, "Empty watcher");
            return DPN::Client::Interface();
        }
        const DArray<Client::Interface> rmts = data()->remotes();
        FOR_VALUE( rmts.size(), i ) {
            if( rmts[i].tag() == tag ) return rmts[i];
        }
        return DPN::Client::Interface();
    }
    DArray<Client::Interface> ClientCenter::clients() const {

        if( isEmptyObject() ) {
            DL_ERROR( 1, "Empty watcher");
            return DArray<Client::Interface>();
        }
        return data()->remotes();
    }










}

