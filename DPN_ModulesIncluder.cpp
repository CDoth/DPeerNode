#include "DPN_ModulesIncluder.h"

//==============================================================================================
//DPN_MediaSystem *extractMediaModule(DPN_Modules &modules) { return reinterpret_cast<DPN_MediaSystem*>(modules.module("Media")); }

DPN::Network::ModuleBinder boundModules[] = {
    DEF_MODULE("FileModule", DPN_FileSystem),
    DEF_MODULE("NetworkModule", DPN::Network::NetworkSystem),
    DEF_MODULE("ChatModule", DPN::Chat::ChatSystem),


    END_MODULE_LIST
};
