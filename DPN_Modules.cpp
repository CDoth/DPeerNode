#include "DPN_Modules.h"

#include <algorithm>

using namespace DPeerNodeSpace;
__file_module::__file_module() {
    ptr = new DPN_FileSystem;
}
__file_module::~__file_module() {
    delete ptr;
}
void DPN_Modules::stopAll() {
    for( auto it: iModulesMap ) { it.second->stop(); }
}
void DPN_Modules::clientDisconnected(const DPN_AbstractClient *client) {
    for( auto it: iModulesMap ) { it.second->clientDisconnected(client); }
}
bool DPN_Modules::addModule(DPN_AbstractModule *m, const std::string &name) {
    if( m == nullptr || name.empty() ) {
        DL_BADVALUE(1, "module: [%p] name size: [%d]", m, name.size());
        return false;
    }
    if( iModulesMap.find(name) != iModulesMap.end() ) {
        DL_ERROR(1, "Name [%s] already regsitered in map", name.c_str());
        return false;
    }
    for( auto it: iModulesMap ) {


        if( it.second == m ) {
            DL_ERROR(1, "Module [%p] already registered in map", m);
            return false;
        }
    }
    iModulesMap[name] = m;
    return true;
}
DPN_AbstractModule *DPN_Modules::module(const std::string &name) {
    auto f = iModulesMap.find(name);
    if( f == iModulesMap.end() ) return nullptr;
    return f->second;
}
const DPN_AbstractModule *DPN_Modules::module(const std::string &name) const  {
    auto f = iModulesMap.find(name);
    if( f == iModulesMap.end() ) return nullptr;
    return f->second;
}
//==============================================================================================
DPN_FileSystem *extractFileModule(DPN_Modules &modules) {
    return reinterpret_cast<DPN_FileSystem*>(modules.module("FileSystem"));
}
DPN_MediaSystem *extractMediaModule(DPN_Modules &modules) {
    return reinterpret_cast<DPN_MediaSystem*>(modules.module("Media"));
}

GlobalModule defaultModules[] = {
   DEFULAT_MODULE("FileSystem", DPN_FileSystem, true),
   DEFULAT_MODULE("Media", DPN_MediaSystem, false),
   DEFULAT_MODULE("DataStream", DPN_DataStream, true),

    {nullptr, "", false}
};

