#ifndef DPN_CONNECTIONSCORE_H
#define DPN_CONNECTIONSCORE_H
#include <DArray.h>



#include "DPN_ThreadBridge.h"
#include "DPN_Client.h"

#include "DPN_FileClient.h"
#include "DPN_MediaClient.h"
#include "DPN_DataClient.h"



//====================================================================================================== DPN_SharedPort
class DPN_SharedPort {
public:
    enum Type {

    };
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

    bool isAuto() const {return __autoaccepting;}
    int port() const {return __port;}
    std::string address() const {return __connector ? __connector->localAddress() : std::string("No address");}
public:
    bool isOpen() const;
    bool open();
private:
    void clear();
private:
    int __port;
    int __new_connection_timeout;
    bool __autoaccepting;
    DPN_NodeConnector *__connector;
private:
    DArray<PeerAddress> __white_list;
    DArray<PeerAddress> __black_list;
    int __maximum;
    int __connections_counter;
private:
    bool __open;
};
//======================================================================================================
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
    void attachToClient(DPN_AbstractClient *client);

private:
    DPN_Result send_prepare();
    DPN_Result send();
    DPN_Result recv_prepare();
    DPN_Result recv();
    DPN_Result reaction();
    void makeLine();

private:
    DPN_NodeConnector *connector;
    DPN_AbstractClient *baseClient;
    MeetInfo *mi;
    DPN_AbstractConnectionsCore *core;
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
    void setCore(DPN_AbstractConnectionsCore *__core);
    void close();
    bool isShadow() const;
    DPN_AbstractClient *baseClient();
    ConnectionState finalState() const {return state;}
    ClientInitContext clientContext();
    const DPN_NodeConnector * connector() const {return pConnector;}
    const std::string & name() const {return mi.sr_name;}
protected:
    virtual DPN_Result prepare() = 0;
    std::string generateSessionKey();
    std::string generateShadowKey();
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
    DPN_AbstractClient *base;
    DPN_AbstractConnectionsCore *core;
    DPN_PeerMeeting meeting;
    DPN_NodeConnector *pConnector;
    std::atomic<ConnectionState> state;
    MeetInfo mi;
    ClientInitContext initConext;
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
    DPN_OutgoingConnection(DPN_AbstractClient *client, int port);
    ~DPN_OutgoingConnection();
    void setAttempts(int a);
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
    int serverSignal;
    __action_line<DPN_OutgoingConnection> dialogLine;
    PeerAddress connectTarget;
    std::string innerShadowKey;
private:
    int attempts_counter;
    int max_attempts;
};
//======================================================================================================
class DPN_ConnectionsCore : public DPN_AbstractConnectionsCore {
public:
    friend class DPN_ThreadMaster;
    friend class DPN_Core;

    DPN_ConnectionsCore();

    //-------------------------------------------------------------------------------------- Virtual:
    void toEachClient(DPN_ClientSystemMessage m, DPN_AbstractClient *source) override;
    void pushTask(DPN_Task *t) override;

    bool isShadowAvailable(DPN_AbstractClient *client, int port) override;
    void shadowConnection(DPN_AbstractClient *client, int port) override;
    DPN_AbstractClient *client(const PeerAddress &pa) override;
    DPN_UDPPort * openUDPPort(int port) override;

    void replanDirections() override;
    void addGlobalDirection(DPN_Direction *d) override;
    void disconnect(DPN_AbstractClient *c) override;
    DArray<PeerAddress> getEnvironment() const override;
    //--------------------------------------------------------------------------------------

    void renewCatalog();

    bool processIncomingConnections();
    bool processOutgoingConnections();

    DPN_Client & client(int index) {
        if(index >= 0 && index < remotes.size())
            return remotes[index];
    }


    void threadOver(DPN_ThreadBridge &bridge) {__remove_thread(bridge);}

    inline const DArray<DPN_SharedPort> & sharedPorts() const {return openPorts;}
    inline const DArray<DPN_Client> & clients() const {return remotes;}
    inline const DArray<DPN_Client> & disconnectedClients() const {return aDisconnectedClients;}
    inline const DArray<DPN_IncomingConnection*> & incomingConnections() const {return incs;}
    inline const DArray<DPN_OutgoingConnection*> & outgoingConnections() const {return outs;}
    inline const DArray<DPN_ThreadBridge> & threads() const {return tbs;}
    inline const DPN_SimplePtrList<DPN_Task> & tasks() const {return __tasks;}


    inline int threadsNumber() const { return tbs.size(); }
private:

    bool __processConnection(DPN_WaitingConnection *connection);
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
    bool __add_client(const ClientInitContext &c);
    bool __add_shadow();
private:
    bool __push_bad_inc(DPN_NodeConnector *connector, DPN_SharedPort::CheckResult r);
    bool __push_inc(DPN_NodeConnector *connector);
    bool __add_thread(DPN_ThreadBridge &bridge);
    bool __remove_thread(DPN_ThreadBridge &bridge);
    bool __remove_client(DPN_Client &client);
    bool __set_threads(const DArray<DPN_ThreadBridge> &set);

private:
    void __core_action(int s = 0);

private:
    void __replan_directions();
    void __replan_inner();
private:
    std::mutex __mutex;
    std::string name;
    DPN_SimplePtrList<DPN_Task> __tasks;
    DArray<DPN_ThreadBridge> tbs;
private:
    DArray<DPN_IncomingConnection*> incs;
    DArray<DPN_OutgoingConnection*> outs;
private:
    DArray<DPN_SharedPort> openPorts;
    DArray<DPN_UDPPort*> udpPorts;

    DArray<DPN_Client> remotes;
    DArray<DPN_Client> aDisconnectedClients;
private:
    DPN_ThreadContext userThreadContext;
private:
    DArray<uint64_t> random;

private:
// modules:
    DArray<DPN_Direction*> aGlobalDirections;
    // file system:
    DPN_Catalog host_catalog;
    // media system:
//    DPN_MediaSystem iGlobalMediaSystem;

//    DPN_TransactionMaster iTransactionMaster;
    //------------------------------- external:
//    DArray<DPN_AbstractModule*> aGlobalModules;
    DPN_Modules iGlobalModules;
};

#endif // DPN_CONNECTIONSCORE_H
