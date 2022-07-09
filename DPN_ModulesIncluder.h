#ifndef DPN_MODULESINCLUDER_H
#define DPN_MODULESINCLUDER_H
#include "DPN_FileSystem.h"
#include "DPN_Network.h"
#include "DPN_Network2.h"
#include "DPN_Chat.h"


#define DEF_MODULE(NAME, TYPE) \
{ []( DPN::Network::ClientCenter &cc ) -> DPN_AbstractModule *{return reinterpret_cast<DPN_AbstractModule*>(new TYPE(NAME, cc));}, NAME}
#define END_MODULE_LIST {nullptr, ""}

extern DPN::Network::ModuleBinder boundModules[];


#endif // DPN_MODULESINCLUDER_H
