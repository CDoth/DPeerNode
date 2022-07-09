#ifndef DPN_NETWORK_H
#define DPN_NETWORK_H
#include "DPN_NodeConnector.h"
#include "DPN_TransmitTools.h"
#include "DPN_ClientInterface.h"
#include "DPN_Util.h"

namespace DPN::Network {


    struct ProxyPacketFilter
            : public DPN::IO::IOContext
            , public DPN::Client::Underlayer
    {
        DPN_Result process(DPN_ExpandableBuffer &b) override;
    };
    struct __proxy_scheme__
            : public DPN_Propagation::LinearScheme
            , public DPN::Client::Underlayer
    {};
    struct __proxy_mono__ {
        DPN::Thread::ThreadUnitWrapper<__proxy_scheme__> wUnitWrapper;
        ProxyPacketFilter wFilter;
        __channel         wChannel;
        __channel_mono_interface wBackInterface, wForwardInterface;
    };
    struct __proxy_node__
           : public DPN::Client::Underlayer
    {
        __proxy_node__();
        __proxy_node__( DPN::Client::Underlayer &ul );

        DPN_Result connectOne( DPN_NodeConnector *connector, DPN::P2PRole p2pRole );
        bool start();

        DPN::Thread::ThreadUnitWrapper<__proxy_scheme__> wA2E;
        DPN::Thread::ThreadUnitWrapper<__proxy_scheme__> wE2A;

        ProxyPacketFilter wA2EFilter;
        ProxyPacketFilter wE2AFilter;

        __channel        wAChannel;
        __channel        wEChannel;

        __channel_mono_interface wABack, wAForward;
        __channel_mono_interface wEBack, wEForward;
    };
    class ProxyNode
            : private DWatcher< __proxy_node__ >
    {
    public:
        ProxyNode();
        ProxyNode ( DPN::Client::Underlayer &ul );
        DPN_Result connectOne( DPN_NodeConnector *connector, DPN::P2PRole p2pRole );
        bool start();
    };
    class SharedPort {
    public:
        SharedPort();
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
    enum ConnectionFlags {
        CONNECTION__FLG__VIRTUAL
        , CONNECTION__FLG__VISIBLE
    };
    ///
    /// \brief The ConnectionContext class
    /// 1. Default connection with new client
    /// 2. Make shadow TCP connection (not use for shadow UDP connection)
    /// 3. Virtual connection with connected client to make proxy channel
    class ConnectionContext {
    public:
        ProxyNode proxyNode() const;
        void makeProxy( DPN::Client::Core from, DPN::Client::Core to );
        inline uint32_t flags() const { return iFlags; }
    private:
        uint32_t iFlags;
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
        bool isVirtual() const;
        inline const ConnectionContext & context() const { return iContext; }
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
        ConnectionContext iContext;
        std::atomic<ConnectionState> iState;
        DPN_NodeConnector *pConnector;
        __transmit_content iMainContent;
        UNIT(std::string) UNIT_SIGNAL = iMainContent;
        UNIT(std::string) UNIT_SESSION_KEY = iMainContent;
        UNIT(std::string) UNIT_SHADOW_KEY = iMainContent;
        UNIT(PeerAddress) UNIT_AVATAR = iMainContent;
        UNIT(uint32_t) UNIT_FLAGS = iMainContent;
    };

    class OutgoingConnection : public WaitingConnection {
    public:
        OutgoingConnection( const std::string &localName, const PeerAddress &address, const ConnectionContext &context );
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
    OutgoingConnection * createOutgoingConnection(const std::string &localName, const PeerAddress &a , const ConnectionContext &context);
    IncomingConnection * createIncomingConnection( const std::string &localName, DPN_NodeConnector *c );

    //=============================================================================================================


    //=============================================================================================================
    class __client_center__
            : public DPN::Thread::AbstractThreadUnit
            , public DPN::Client::GlobalUnderlayerSpace
            , public DPN::Thread::ThreadUser
            , public DPN::Modules
    {
    public:
        __client_center__();
        void setName( const std::string &name );
        bool sharePort(int port , bool aa);
        bool connectTo(const char *address, int port , const ConnectionContext &context);
        void acceptAll();
        const DArray<DPN::Client::Interface> & remotes() const;
    private:
        bool work() override;
    private:
        bool addVirtualClient( DPN_NodeConnector *connector, const ConnectionContext &context );
        bool newClient( DPN_NodeConnector *connector );
        bool removeClient( DPN::Client::Tag tag );

        bool processConnection( WaitingConnection *w );
        bool listenPorts();
        bool processEvents();
        bool pushInc(DPN_NodeConnector *c, bool accept);
        bool pushOut(const std::string &address, int port, const ConnectionContext &context);
    private:
        mutable std::mutex iMutex;
        std::string iName;
        DArray<DPN::Client::Interface> aRemotes;
        DArray< ProxyNode > aProxys;

        DPN::Util::_ThreadSafeQueue<DPN::Network::SharedPort> aSharedPorts;
        DPN::Util::_ThreadSafeQueue<DPN::Network::IncomingConnection*> aIncs;
        DPN::Util::_ThreadSafeQueue<DPN::Network::OutgoingConnection*> aOuts;
    };
    typedef DPN::Thread::ThreadUnitWrapper< __client_center__ > dClientCenter;
    class ClientCenter
            : public dClientCenter

    {
    public:
        ClientCenter() {}
        ClientCenter( bool makeSource );

        void setName( const std::string &name );
        bool sharePort(int port, bool autoaccept);


        bool connectTo( const PeerAddress &pa );
        bool connectTo( const PeerAddress &pa, const ConnectionContext &context );
        void acceptAll();
    public:
        DPN::Client::GlobalUnderlayerSpace globalUnderlayer();
        DPN::Modules modules();
        DPN::Thread::ThreadUser threadUser();
    public:
        DPN::Client::Tag pa2tag( const DPN::Network::PeerAddress &pa ) const;
        DPN::Client::Interface tag2client( DPN::Client::Tag ) const;
        DArray<DPN::Client::Interface> clients() const;
    };
    class NetworkModule
            : public DPN_AbstractModule
            , public ClientCenter
    {
    public:
        NetworkModule( const std::string &name, ClientCenter &cc ) :
            DPN_AbstractModule(name), ClientCenter(cc)
        {}
    };
    struct ModuleBinder {
        DPN_AbstractModule *(*creator)( ClientCenter &cc );
        std::string name;
    };
}
#endif // DPN_NETWORK_H
