#include "DPN_ModulesIncluder.h"

//==============================================================================================
//DPN_MediaSystem *extractMediaModule(DPN_Modules &modules) { return reinterpret_cast<DPN_MediaSystem*>(modules.module("Media")); }

GlobalModule defaultModules[] = {
   DEF_MODULE("FileSystem", DPN_FileSystem, false),
//   DEFULAT_MODULE("Media", DPN_MediaSystem, false),
//   DEFULAT_MODULE("DataStream", DPN_DataStream, false),

    {nullptr, "", false}
};
