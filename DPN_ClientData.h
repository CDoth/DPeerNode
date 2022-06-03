#ifndef DPN_CLIENTDATA_H
#define DPN_CLIENTDATA_H
#include "DPN_Direction.h"
#include "DPN_Processors.h"
#include "DPN_Channel.h"

#include "DPN_Modules.h"

class DPN_BaseChannel {
public:
    DPN_BaseChannel();
    void init(DPN_Channel *ch, DPN_ClientContext &cc);
    void send(DPN_TransmitProcessor *p);
    bool processing() const;
private:
    DPN_Channel *channel;
    DPN_SendDirection *send_direction;
    DPN_ReceiveDirection *recv_direction;
};
struct ShadowPermission {
    inline bool operator==(const std::string &__shadowKey) const {return shadowKey == __shadowKey;}
    PeerAddress source;
    PeerAddress target;
    std::string shadowKey;
};
struct ClientInitContext {

    ClientInitContext() {
        connector = nullptr;
        core = nullptr;
    }
    void clear() {
        connector = nullptr;
        core = nullptr;
        name.clear();
        avatar = PeerAddress();
        sessionKey.clear();
        shadowKey.clear();
    }
    DPN_NodeConnector *connector;
    DPN_AbstractConnectionsCore *core;
    std::string name;
    bool initiator;
    bool localVisible;
    PeerAddress avatar;
    std::string sessionKey;
    std::string shadowKey;
};
enum ClientState {
    RAW
    ,CONNECTED
    ,DISCONNECTED
    ,DISCONNECTION_PROCESSING
};
//------------------------------------------------

//------------------------------------------------
class DPN_ClientData : public DPN_AbstractClient {
public:
    enum ClientEvent {
        CE__MAKE_SHADOW_CONNECTION
    };
    friend class DPN_Client;


    DPN_ClientData();
    DPN_ClientData(const ClientInitContext &initContext);
    ~DPN_ClientData();
    //------------------------------------------------------------------------ Virtual
    bool disconnect() override;
    inline const PeerAddress & localAddress() const override {return iLocal;}
    inline const PeerAddress & remoteAddress() const override {return iRemote;}
    inline const PeerAddress & avatar() const override {return iAvatar;}
    void setEnvironment(const DArray<PeerAddress> &env) override;

    const std::string & getSessionKey() const override;
    void verifyShadow(const PeerAddress &local, const PeerAddress &remote, const std::string &key) override;
    void registerShadow(const PeerAddress &local, const PeerAddress &remote, const std::string &key) override;
    DPN_Result checkShadowPermission
    (const PeerAddress &source, const PeerAddress &target, const std::string &sessionKey, const std::string &shadowKey) const override;
    bool addShadowConnection(const ClientInitContext &init) override;
    DPN_Channel * channel(const std::string shadowKey) override;

    inline DPN_TransactionMap & clientTransactions() override {return iClientTransactions;}
    inline DPN_TransactionMap & generatedTransactions() override {return iGeneratedTransactions;}

    void shadowConnectionServerAnswer(bool a, int port) override;
    inline DPN_ClientContext context() override {return wClientContext;}
    //----------------------------------------------------------------------

//    template <class T> T * generateTransaction() {return wClientContext.core()->transactionMaster().generate<T>();}

    bool processing() const;
    bool requestShadowConnection(int port = -1);
    void close();
    bool sendMessage(const char *m);
    bool ping();
    void requestEnvironment();


    ClientState state() const {return eState;}
    inline const DArray<DPN_Channel*> & getChannels() const {return aChannels;}
    DPN_Modules & modules() {return iModules;}

    void addModule(const std::string &name, DPN_AbstractModule *module) { iModules.addModule(module, name); }
public:
    template <DPN_PacketType t> inline typename DPN_ProcessorTransform<t>::proc_type processor(DPN_AbstractTransaction *transaction = nullptr)
    { return wThreadContext.processor<t>(transaction); }

    inline void send(DPN_TransmitProcessor *p) { iBaseChannel.send(p); }
private:
    DPN_StateManager<ClientEvent> iStates;
private:
    ClientState eState;
    PeerAddress iLocal;
    PeerAddress iRemote;
    PeerAddress iAvatar;
    std::string iName;
    bool iLocalVisible;
    DArray<PeerAddress> aEnvironment;
private:
    DPN_BaseChannel iBaseChannel;
    DPN_ThreadContext wThreadContext;
    DPN_ClientContext wClientContext;
    DPN_Modules iModules;
private:
    DPN_Channel *pMainChannel;
    DArray<DPN_Channel*> aChannels; //channels[0] == mainChannel == channels[0]
    DArray<ShadowPermission> aIncomingShadows;
    DArray<int> aOpenPorts;
    std::string iSessionKey;

    DPN_TransactionMap iClientTransactions;
    DPN_TransactionMap iGeneratedTransactions;


private:
    DArray<DPN_UDPChannel*> UDPChannels;
};
/*
class DPN_ProxyClientData {
public:
    DPN_ProxyClientData(DPN_NodeConnector *sourceNode, DPN_NodeConnector *targetNode);
    ~DPN_ProxyClientData();

    bool processing() const;
private:
    DPN_ProxyDirection *proxy__src2trt;
    DPN_ProxyDirection *proxy__trt2src;

};
*/


#endif // DPN_CLIENTDATA_H
