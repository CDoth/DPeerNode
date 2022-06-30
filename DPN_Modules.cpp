#include "DPN_Modules.h"

#include <algorithm>

using namespace DPN::Logs;


DPN_Modules::DPN_Modules(bool create) : DWatcher<__modules__>(create) {

}
void DPN_Modules::stopAll() {
    if( isEmptyObject() ) return;
    for( auto it: data()->iModulesMap ) { it.second->stop(); }
}
bool DPN_Modules::addModule(DPN_AbstractModule *m, const std::string &name) {
    if( isEmptyObject() ) {
        DL_ERROR(1, "Empty watcher");
        return false;
    }
    if( m == nullptr || name.empty() ) {
        DL_BADVALUE(1, "module: [%p] name size: [%d]", m, name.size());
        return false;
    }
    if( data()->iModulesMap.find(name) != data()->iModulesMap.end() ) {
        DL_ERROR(1, "Name [%s] already regsitered in map", name.c_str());
        return false;
    }
    for( auto it: data()->iModulesMap ) {
        if( it.second == m ) {
            DL_ERROR(1, "Module [%p] already registered in map", m);
            return false;
        }
    }
    data()->iModulesMap[name] = m;
    return true;
}
DPN_AbstractModule *DPN_Modules::module(const std::string &name) {
    if( isEmptyObject() ) {
        DL_ERROR(1, "Empty watcher");
        return nullptr;
    }
    auto f = data()->iModulesMap.find(name);
    if( f == data()->iModulesMap.end() ) return nullptr;
    return f->second;
}
const DPN_AbstractModule *DPN_Modules::module(const std::string &name) const  {
    if( isEmptyObject() ) {
        DL_ERROR(1, "Empty watcher");
        return nullptr;
    }
    auto f = data()->iModulesMap.find(name);
    if( f == data()->iModulesMap.end() ) return nullptr;
    return f->second;
}


