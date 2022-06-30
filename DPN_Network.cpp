#include "DPN_Network.h"

using namespace DPN::Logs;

namespace DPN::Network {

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

    //============================================================
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
    //============================================================
    OutgoingConnection::OutgoingConnection(const std::string &localName, const PeerAddress &address) {
        setLocalName( localName );
        iConnectionAddress = address;
        iConnectionAttepmts = 0;
        iConnectionMaximumAttemps = 1;

        makeMainLine();
    }
    DPN_Result OutgoingConnection::process() {
        return iActionLine.go();
    }
    DPN_Result OutgoingConnection::connecting() {

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
        UNIT_VISIBLE = true;
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
    //===========================================================
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
    OutgoingConnection *createOutgoingConnection(const std::string &localName, const PeerAddress &a) {
        return new OutgoingConnection( localName, a );
    }
    IncomingConnection *createIncomingConnection(const std::string &localName, DPN_NodeConnector *c) {
        return new IncomingConnection( localName, c );
    }

    ClientCenterInterface::ClientCenterInterface(bool makeSource) : DWatcher<__client_center__>(makeSource) {}
    ClientCenterInterface::ClientCenterInterface(ClientCenterInterface &shared) : DWatcher<__client_center__>(shared) {}

    bool ClientCenterInterface::addClient(DPN_NodeConnector *connector, Thread::ThreadUser &threadUser) {
        if( isEmptyObject() ) {
            DL_ERROR(1, "Empty watcher");
            return false;
        }
        DPN_ClientInterface client( threadUser, connector );
        client.setModules( data()->iModules );
        data()->remotes.append( client );
        return true;
    }
    const DArray<DPN_ClientInterface> &ClientCenterInterface::clients() const {
        return data()->remotes;
    }
    WaitingConnectionsProcessor::WaitingConnectionsProcessor(ClientCenterInterface cci, Thread::ThreadUser threadUser)
        : ClientCenterInterface(cci), DPN::Thread::ThreadUser( threadUser )
    {
    }

    bool WaitingConnectionsProcessor::work() {

//        DL_INFO(1, "proc connections... [%p]", this);
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
        return true;
    }

    void WaitingConnectionsProcessor::setName(const std::string &name) {
        iName = name;
    }
    bool WaitingConnectionsProcessor::sharePort(int port, bool autoaccept) {
        if(!DXT_CHECK_PORT(port)) {
            DL_BADVALUE(1, "port: [%d]", port);
            return false;
        }
        FOR_VALUE( aSharedPorts.size(), i ) {

            if( aSharedPorts.output()[i]->port() == port) {
                DL_INFO(1, "Port [%d] already processing", port);
                return true;
            }
        }

        aSharedPorts.push_back(new DPN::Network::SharedPort( port, autoaccept ) );
        return true;
    }
    bool WaitingConnectionsProcessor::connectTo(const char *address, int port) {
        auto out = DPN::Network::createOutgoingConnection( iName, PeerAddress( port, address) );
        aOuts.push_back( out );
        return true;
    }
    void WaitingConnectionsProcessor::acceptAll() {
        if(aIncs.output().empty()) {
            DL_INFO(1, "No incoming connections");
            return;
        }
        FOR_VALUE(aIncs.size(), i) {
            aIncs.output()[i]->accept();
        }
    }
    bool WaitingConnectionsProcessor::listenPorts() {

        aSharedPorts.accept();

//        DL_INFO(1, "shared ports: [%d]", aSharedPorts.size() );

        FOR_VALUE( aSharedPorts.size(), i) {

            SharedPort *sp = aSharedPorts.output()[i];

            if( sp->open() == false ) {
                continue;
            }

            if( sp->newConnection() ) {

                DL_INFO(1, ">>> new incoming connection...");
                DPN_NodeConnector *connector = sp->accept();
                if(connector == nullptr) {
                    DL_ERROR(1, "Can't accept connection");
                    return false;
                }

                SharedPort::CheckResult r;
                if( (r = sp->check(connector)) != SharedPort::CheckedSuccesfull) {
                    DL_WARNING(1, "Can't accept connection");
                } else {
                    pushInc( connector, sp->isAuto() );
                }
            }
        }
        return true;
    }
    void WaitingConnectionsProcessor::pushInc( DPN_NodeConnector *c, bool accept ) {
        auto inc = createIncomingConnection( iName, c );
        if( accept ) inc->accept();
        aIncs.push_back( inc );
    }
    bool WaitingConnectionsProcessor::processConnection(WaitingConnection *w) {

        auto r = w->process();
        if( r == DPN_SUCCESS ) {

            if( w->state() == DPN::Network::ACCEPT ) {

                if( w->isShadow() ) {
                    DL_INFO(1, "Accept shadow connection");
                } else {
                    DL_INFO(1, "Connection accepted");
                    ClientCenterInterface::addClient( w->connector(), *this );

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

}

