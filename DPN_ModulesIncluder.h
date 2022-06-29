#ifndef DPN_MODULESINCLUDER_H
#define DPN_MODULESINCLUDER_H
#include "DPN_FileSystem.h"
//#include "DPN_MediaSystem.h"
#include "DPN_Modules.h"

//DPN_MediaSystem * extractMediaModule(DPN_Modules &modules);

#define DEF_MODULE(NAME, TYPE, UNIQUE) { []() -> DPN_AbstractModule *{return reinterpret_cast<DPN_AbstractModule*>(new TYPE(NAME));}, NAME, UNIQUE}
extern GlobalModule defaultModules[];


#endif // DPN_MODULESINCLUDER_H
