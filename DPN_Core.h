#ifndef DPN_CORE_H
#define DPN_CORE_H
#include <DArray.h>



#include "DPN_ThreadBridge.h"
#include "DPN_Modules.h"
#include "DPN_ModulesIncluder.h"



//====================================================================================================== DPN_SharedPort
class DPN_SharedPort {
public:
    DPN_SharedPort();
    DPN_SharedPort(int port, bool autoaccepting);
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
//======================================================================================================
/*
enum ConnectionState {
    WAIT
    ,ACCEPT
    ,REJECT
    ,ATTACH
};
struct MeetInfo {
    std::string sr_name;
    PeerAddress r_attach;
    bool isShadow;
};
class DPN_PeerMeeting {
public:
    DPN_PeerMeeting();
    DPN_Result meet();
    void init(MeetInfo *__mi, DPN_NodeConnector *c);
//    void attachToClient(DPN_AbstractClient *client);
private:
    DPN_Result send_prepare();
    DPN_Result send();
    DPN_Result recv_prepare();
    DPN_Result recv();
    DPN_Result reaction();
    void makeLine();

private:
    DPN_NodeConnector *connector;
//    DPN_AbstractClient *baseClient;
    MeetInfo *mi;
//    DPN_AbstractConnectionsCore *core;
private:
    __transmit_content content;
    UNIT(std::string) UNIT_NAME = content;
    UNIT(PeerAddress) UNIT_ATTACH = content;
private:
    __action_line<DPN_PeerMeeting> line;
};
class DPN_WaitingConnection {
public:
    DPN_WaitingConnection();
    virtual ~DPN_WaitingConnection();
    DPN_Result process();
    void setName(const std::string &__name);
    void close();
    bool isShadow() const;
//    DPN_AbstractClient *baseClient();
    ConnectionState finalState() const {return state;}
//    ClientInitContext clientContext();
    const DPN_NodeConnector * connector() const {return pConnector;}
    const std::string & name() const {return mi.sr_name;}
protected:
    virtual DPN_Result prepare() = 0;
    std::string generateSessionKey();
private:
    void makeLine();
    DPN_Result meetInit();
    DPN_Result meet();
    virtual DPN_Result wait() = 0;
    virtual DPN_Result dialog() = 0;
private:
    __action_line<DPN_WaitingConnection> line;
protected:
    __transmit_content signal_content;
    UNIT(int) UNIT_SIGNAL = signal_content;

    __transmit_content answer_content;
    UNIT(std::string) UNIT_SESSION_KEY = answer_content;
    UNIT(std::string) UNIT_SHADOW_KEY = answer_content;
    UNIT(PeerAddress) UNIT_AVATAR = answer_content;
    UNIT(bool) UNIT_VISIBLE = answer_content;

protected:
//    DPN_AbstractClient *base;
    DPN_PeerMeeting meeting;
    DPN_NodeConnector *pConnector;
    std::atomic<ConnectionState> state;
    MeetInfo mi;
//    ClientInitContext initConext;
};
class DPN_IncomingConnection : public DPN_WaitingConnection {
public:
    DPN_IncomingConnection(DPN_NodeConnector *c);
    void accept();
    void reject();
protected:
    DPN_Result prepare() override;
    DPN_Result wait() override;
    DPN_Result dialog() override;
private:
    DPN_Result prepare1_s2c();
    DPN_Result transmit1_s2c();

    DPN_Result prepare2_afc();
    DPN_Result transmit2_afc();

    DPN_Result prepare3_a2c();
    DPN_Result transmit3_a2c();

    void makeDialogLine();
private:
    void __signal_prepare();
private:
    int clientSignal;
    __action_line<DPN_IncomingConnection> dialogLine;
};
class DPN_OutgoingConnection : public DPN_WaitingConnection {
public:
    DPN_OutgoingConnection(const char *address, int port);
//    DPN_OutgoingConnection(DPN_AbstractClient *client, int port);
    ~DPN_OutgoingConnection();
    void setShadowKey(const std::string &shadowKey);
    void setAttempts(int a);
    void setChannelRequester(DPN_AbstractModule *m);
protected:
    DPN_Result prepare() override;
    DPN_Result wait() override;
    DPN_Result dialog() override;
private:
    DPN_Result prepare1_sfs();
    DPN_Result transmit1_sfs();

    DPN_Result prepare2_a2s();
    DPN_Result transmit2_a2s();

    DPN_Result prepare3_afs();
    DPN_Result transmit3_afs();

    DPN_Result post_proc();

    void makeDialogLine();
    void clear();
private:
    DPN_AbstractModule *pChannelRequester;
    int serverSignal;
    __action_line<DPN_OutgoingConnection> dialogLine;
    PeerAddress connectTarget;
    std::string innerShadowKey;
private:
    int attempts_counter;
    int max_attempts;
};
*/



namespace Connections {

// use wrapper objects instead pointers
// signal as hashsum
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
        __action_line<WaitingConnection> iMeetingLine;

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
        __action_line<OutgoingConnection> iActionLine;
        __action_line<OutgoingConnection> iDialogLine;

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
        __action_line<IncomingConnection> iActionLine;
        __action_line<IncomingConnection> iDialogLine;

    };
    OutgoingConnection * createOutgoingConnection( const std::string &localName, const PeerAddress &a );
    IncomingConnection * createIncomingConnection( const std::string &localName, DPN_NodeConnector *c );
}

//======================================================================================================
/*
class DPN_ConnectionsCore  {
public:
    friend class DPN_ThreadMaster;
    friend class DPN_Core;

    DPN_ConnectionsCore();
    void renewCatalog();

    bool processIncomingConnections();
    bool processOutgoingConnections();




    inline const DArray<DPN_SharedPort> & sharedPorts() const {return openPorts;}
    inline const DArray<DPN_ClientInterface> & clients() const {return remotes;}
//    inline const DArray<DPN_ClientInterface> & disconnectedClients() const {return aDisconnectedClients;}
//    inline const DArray<DPN_IncomingConnection*> & incomingConnections() const {return incs;}
//    inline const DArray<DPN_OutgoingConnection*> & outgoingConnections() const {return outs;}
    inline const DPN_SimplePtrList<DPN_Task> & tasks() const {return __tasks;}


private:

    bool __processConnection(Connections::WaitingConnection *connection);
    bool __addClient( DPN_NodeConnector *c);
    void setName(const std::string &n) {name = n;}
    const std::string &getName() const {return name;}

    bool share_port(int port, bool autoaccepting);
    bool connect_to(const char *address, int port);
    bool send_message(const char *message);
    bool sync();
    void ping();

    void acceptAll();
    void rejectAll();
    void disconnectAll();
private:
    bool __open_udp_port(int port);
//    bool __add_client(const ClientInitContext &c);
    bool __add_shadow();
private:
    bool __push_bad_inc(DPN_NodeConnector *connector, DPN_SharedPort::CheckResult r);
    bool __push_inc(DPN_NodeConnector *connector);

private:
    void __core_action(int s = 0);

private:
    void __replan_directions();
    void __replan_inner();
private:
    std::mutex __mutex;
    std::string name;
    DPN_SimplePtrList<DPN_Task> __tasks;
private:
    DArray<Connections::IncomingConnection*> aIncs;
    DArray<Connections::OutgoingConnection*> aOuts;
private:
    DArray<DPN_SharedPort> openPorts;
    DArray<DPN_ClientInterface> remotes;
//    DArray<DPN_Client> aDisconnectedClients;
private:
    DArray<uint64_t> random;
    DArray<int> aUdpAvailablePorts;
private:
    DPN_Modules iGlobalModules;
private:
    DPN_Thread::ThreadUser iThreadUser;
};
*/

class __dpn_core__ : public DPN::Thread::ThreadUser {
public:
    __dpn_core__();
    void createThread( DPN::Thread::Policy p );
private:
};
typedef DWatcher< __dpn_core__ > DPN_CORE;

#endif // DPN_CORE_H
