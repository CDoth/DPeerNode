#ifndef DPN_NETWORK_H
#define DPN_NETWORK_H
#include "DPN_NodeConnector.h"
#include "DPN_TransmitTools.h"
#include "DPN_ClientInterface.h"
#include "DPN_Util.h"

namespace DPN::Network {

    class SharedPort {
    public:
        SharedPort(int port, bool autoaccepting);
    public:
        bool newConnection();
        DPN_NodeConnector * accept();
        enum CheckResult {
            CheckedSuccesfull
            ,PortClosed
            ,MissWhiteList
            ,ContainedInBlackList
            ,MaximumConnections
        };
        CheckResult check(DPN_NodeConnector *c);
        bool isAuto() const {return iAutoaccepting;}
        int port() const {return iPort;}
        std::string address() const {return pConnector ? pConnector->localAddress() : std::string("No address");}
    public:
        bool isOpen() const;
        bool open();
    private:
        void clear();
    private:
        int iPort;
        int iTimeout;
        bool iAutoaccepting;
        DPN_NodeConnector *pConnector;
    private:
        DArray<PeerAddress> aWhiteList;
        DArray<PeerAddress> aBlackList;
        int iMaximum;
        int iConnections;
    private:
        bool iOpened;
    };
    enum ConnectionState {
        WAIT
        ,ACCEPT
        ,REJECT
        ,ATTACH
    };
    class WaitingConnection {
    public:
        WaitingConnection();
        virtual ~WaitingConnection();
        DPN_NodeConnector * connector() {return pConnector;}
        virtual DPN_Result process() = 0;
        inline ConnectionState state() const {return iState;};
        void close();
        bool isShadow() const;
        inline const std::string & remoteName() const {return iRemoteName;}
        inline const std::string & localName() const {return iLocalName;}
    protected:
        DPN_Result innerMeeting();
        DPN_Result sendPacket();
        DPN_Result receivePacket();
        void setLocalName( const std::string &name );
        void makeMeetLine();
    private:
        DPN_Result meetPresend();
        DPN_Result meetSend();
        DPN_Result meetPrereceive();
        DPN_Result meetReceive();
        DPN_Result meetEnding();
    private:
        __transmit_content iMeetContent;
        UNIT(std::string) UNIT_MEET_NAME = iMeetContent;
        UNIT(PeerAddress) UNIT_MEET_ATTACH = iMeetContent;
        DPN::Util::ActionLine<WaitingConnection> iMeetingLine;

        std::string iLocalName;
        std::string iRemoteName;
        PeerAddress iAttachAddress;
    protected:
        std::atomic<ConnectionState> iState;
        DPN_NodeConnector *pConnector;
        __transmit_content iMainContent;
        UNIT(std::string) UNIT_SIGNAL = iMainContent;
        UNIT(std::string) UNIT_SESSION_KEY = iMainContent;
        UNIT(std::string) UNIT_SHADOW_KEY = iMainContent;
        UNIT(PeerAddress) UNIT_AVATAR = iMainContent;
        UNIT(bool) UNIT_VISIBLE = iMainContent;
    };
    class OutgoingConnection : public WaitingConnection {
    public:
        OutgoingConnection( const std::string &localName, const PeerAddress &address );
        DPN_Result process() override;
    private:
        DPN_Result connecting();
        DPN_Result waiting();
        DPN_Result dialog();
        DPN_Result ending();
    private:
        // Host (outgoing) dialog: receive (signal) -> send (main content) -> receive (answer)
        DPN_Result dialog__clear();
        DPN_Result dialog__process_signal();
        DPN_Result dialog__end();
        void makeDialogLine();
        void makeMainLine();
    private:
        DPN::Util::ActionLine<OutgoingConnection> iActionLine;
        DPN::Util::ActionLine<OutgoingConnection> iDialogLine;

        PeerAddress iConnectionAddress;
        int iConnectionAttepmts;
        int iConnectionMaximumAttemps;
    };
    class IncomingConnection : public WaitingConnection {
    public:
        IncomingConnection( const std::string &localName, DPN_NodeConnector *connector );
        DPN_Result process() override;
        void accept();
        void reject();
    private:
        DPN_Result waiting();
        DPN_Result dialog();
        DPN_Result ending();
    private:
        // Server (incoming) dialog: send (signal) -> receive (main content) -> send (answer)
        DPN_Result dialog__clear();
        DPN_Result dialog__make_signal();
        DPN_Result dialog__make_answer();
        void makeDialogLine();
        void makeMainLine();
    private:
        DPN::Util::ActionLine<IncomingConnection> iActionLine;
        DPN::Util::ActionLine<IncomingConnection> iDialogLine;

    };
    OutgoingConnection * createOutgoingConnection( const std::string &localName, const PeerAddress &a );
    IncomingConnection * createIncomingConnection( const std::string &localName, DPN_NodeConnector *c );

    struct __client_center__ {
        __client_center__() : iModules(true) {}
        DArray<DPN_ClientInterface> remotes;
        DPN_Modules iModules;
    };
    class ClientCenterInterface : private DWatcher< __client_center__ > {
    public:
        ClientCenterInterface( bool makeSource );
        ClientCenterInterface( ClientCenterInterface &shared );

        DPN_Modules & modules() { return data()->iModules; }
        bool addClient( DPN_NodeConnector *connector, DPN::Thread::ThreadUser &threadUser );
        const DArray<DPN_ClientInterface> & clients() const;
    };
    class WaitingConnectionsProcessor : public ClientCenterInterface, public DPN::Thread::ThreadUser, public DPN_ThreadUnit {
    public:
        WaitingConnectionsProcessor( ClientCenterInterface cci, DPN::Thread::ThreadUser threadUser );
        bool work() override;
        void setName( const std::string &name );
        bool sharePort(int port, bool autoaccept);
        bool connectTo( const char *address, int port );
        void acceptAll();
    private:
        bool listenPorts();
        void pushInc(DPN_NodeConnector *c, bool accept );

        bool processConnection( WaitingConnection *w );

        std::string iName;
        DPN::Util::ThreadSafeQueue<DPN::Network::SharedPort> aSharedPorts;
        DPN::Util::ThreadSafeQueue<DPN::Network::IncomingConnection> aIncs;
        DPN::Util::ThreadSafeQueue<DPN::Network::OutgoingConnection> aOuts;
    };

}
#endif // DPN_NETWORK_H
