#ifndef DPN_CLIENTDATA_H
#define DPN_CLIENTDATA_H

#include "DPN_Processors.h"
#include "DPN_ClientCore.h"



/*
struct ShadowPermission {
    inline bool operator==(const std::string &__shadowKey) const {return shadowKey == __shadowKey;}
    std::string shadowKey;
};
struct ShadowRequest {
    ShadowRequest() {
        pShadowRequester = nullptr;
    }
    std::string shadowKey;
    DPN_AbstractModule *pShadowRequester;
    DPN_ExpandableBuffer wExtraData;
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

class __q {
public:
    friend class __client_base_channel;
    __q();
    bool init(DPN_NodeConnector *c, DPN_DirectionType dt);
private:
    __io__base *pBase;
    IO__CHANNEL *pChannel;
    DPN_IO::IOPropagationScheme *pScheme;
};
class __client_base_channel {
public:
    __client_base_channel();
    bool init(DPN_NodeConnector *c);
    bool send(DPN_TransmitProcessor *p);
private:
    __q iForward;
    __q iBackward;
    __client_core1 *cc;
};

class DPN_ClientData
//        : public DPN_AbstractClient
{
public:
    enum ClientEvent {
        CE__MAKE_SHADOW_CONNECTION
    };
    friend class DPN_Client;


    DPN_ClientData();
    DPN_ClientData(const ClientInitContext &initContext);
    ~DPN_ClientData();

    bool send(DPN_TransmitProcessor *p);
    //------------------------------------------------------------------------ Virtual
//    bool disconnect() override;
//    inline const PeerAddress & localAddress() const override {return iLocal;}
//    inline const PeerAddress & remoteAddress() const override {return iRemote;}
//    inline const PeerAddress & avatar() const override {return iAvatar;}
//    void setEnvironment(const DArray<PeerAddress> &env) override;

//    const std::string & getSessionKey() const override;
//    void registerShadow(const std::string &key) override;
//    DPN_Result checkShadowPermission(const std::string &sessionKey, const std::string &shadowKey) const override;
//    bool addShadowConnection(const ClientInitContext &init) override;
//    bool addShadowConnection(DPN_NodeConnector *udpPort, const std::string &shadowKey) override;
//    IO__CHANNEL * channel(const std::string shadowKey) override;


//    void shadowConnectionServerAnswer(bool a, int port, const std::string &shadowKey) override;
//    bool addShadowRequester(const std::string &shadowKey, DPN_AbstractModule *module, const DPN_ExpandableBuffer &extraData) override;
//    inline DPN_ClientContext context() override {return wClientContext;}

//    DPN_Result syncChannel(const std::string &shadowKey) override;
//    inline void send(DPN_TransmitProcessor *p) override { iBaseChannel.send(p); }
    //----------------------------------------------------------------------

//    template <class T> T * generateTransaction() {return wClientContext.core()->transactionMaster().generate<T>();}

    bool processing() const;
    bool requestShadowConnection(DXT::Type type, int port = -1);
    void close();
    bool sendMessage(const char *m);
    bool ping();
    void requestEnvironment();


    ClientState state() const {return eState;}
    inline const DArray<IO__CHANNEL*> & getChannels() const {return aChannels;}

//    DPN_Modules & modules() {return iModules;}
//    void addModule(const std::string &name, DPN_AbstractModule *module) { iModules.addModule(module, name); }
public:
//    template <DPN_PacketType t> inline typename DPN_ProcessorTransform<t>::proc_type processor()
//    { return wThreadContext.processor<t>(); }

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
    __client_base_channel __base_channel;
//    DPN_BaseChannel iBaseChannel;
    DPN_ClientContext wClientContext;
//    DPN_Modules iModules;
private:
    DArray<ShadowPermission> aIncomingShadows;
    DArray<ShadowRequest> aRequestedShadows;
    DArray<int> aOpenPorts;
    std::string iSessionKey;

//    DPN_TransactionSpace iClientTransactions;
//    DPN_TransactionSpace iGeneratedTransactions;


    IO__CHANNEL *pMainChannel;
    DArray<IO__CHANNEL*> aChannels;

    std::mutex iClientMutex;

    // base channel
    // shadow channels
    // environment
    // stats (name, address, open ports)
};
*/



class DPN_ClientInterface : private DWatcher<DPN_ClientCore> {
public:
    DPN_ClientInterface( );
    DPN_ClientInterface( const DPN::Thread::ThreadUser &sharing, DPN_NodeConnector *c );
    bool init(DPN_NodeConnector *c );
    void setModules( DPN_Modules modules );

    const DPN_ClientTag * tag() const;
    const DArray<__channel> & channels() const;
    void sendMessage(const char *m);
    void makeShadowConnection(DXT::Type);
    bool send(DPN_TransmitProcessor *cp);

    DWatcher<DPN_ClientCore> & watcher() { return *this; }
};

#endif // DPN_CLIENTDATA_H
