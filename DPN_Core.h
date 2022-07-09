#ifndef DPN_CORE_H
#define DPN_CORE_H
#include <DArray.h>



#include "DPN_ThreadBridge.h"
#include "DPN_Modules.h"
#include "DPN_ModulesIncluder.h"
#include "DPN_Network.h"




class __dpn_core__ :
        public DPN::Network::ClientCenter
        {
public:
    __dpn_core__();
    void setName( const std::string &name );
    void createThread( DPN::Thread::Policy p );
    bool sharePort(int port, bool autoaccept);
    bool connectTo(const char *address, int port);
    void acceptAll();
    DArray<DPN::Client::Interface> clients() const;
};
typedef DWatcher< __dpn_core__ > DPN_CORE;

#endif // DPN_CORE_H
