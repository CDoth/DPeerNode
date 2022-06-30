#ifndef DPN_CORE_H
#define DPN_CORE_H
#include <DArray.h>



#include "DPN_ThreadBridge.h"
#include "DPN_Modules.h"
#include "DPN_ModulesIncluder.h"
#include "DPN_Network.h"




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


class __dpn_core__ :
        public DPN::Thread::ThreadUser,
        public DPN::Network::ClientCenterInterface
        {
public:
    __dpn_core__();
    void setName( const std::string &name );
    void createThread( DPN::Thread::Policy p );
    bool sharePort(int port, bool autoaccept);
    bool connectTo(const char *address, int port);
    void acceptAll();

    DArray<DPN_ClientInterface> clients() const;
private:
    DPN::Network::WaitingConnectionsProcessor iWp;

};
typedef DWatcher< __dpn_core__ > DPN_CORE;

#endif // DPN_CORE_H
