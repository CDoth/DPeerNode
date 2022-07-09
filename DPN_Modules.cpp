#include "DPN_Modules.h"

#include <algorithm>

using namespace DPN::Logs;

namespace DPN {
void Modules::clientDisconnected(Client::Tag tag) {
    if( isEmptyObject() ) {
        DL_ERROR(1, "Empty watcher");
        return;
    }
    for( const auto &it: data()->iModulesMap ) {
        it.second->clientOver( tag );
    }
}
Modules::Modules(bool create) : DWatcher<__modules__>(create) {

    }
    void Modules::stopAll() {
        if( isEmptyObject() ) return;
        for( const auto &it: data()->iModulesMap ) { it.second->stop(); }
    }
    bool Modules::addModule(DPN_AbstractModule *m, const std::string &name) {
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
        for( const auto &it: data()->iModulesMap ) {
            if( it.second == m ) {
                DL_ERROR(1, "Module [%p] already registered in map", m);
                return false;
            }
        }
        data()->iModulesMap[name] = m;
        return true;
    }
    DPN_AbstractModule *Modules::module(const std::string &name) {
        if( isEmptyObject() ) {
            DL_ERROR(1, "Empty watcher");
            return nullptr;
        }
        auto f = data()->iModulesMap.find(name);
        if( f == data()->iModulesMap.end() ) return nullptr;
        return f->second;
    }
    const DPN_AbstractModule *Modules::module(const std::string &name) const  {
        if( isEmptyObject() ) {
            DL_ERROR(1, "Empty watcher");
            return nullptr;
        }
        auto f = data()->iModulesMap.find(name);
        if( f == data()->iModulesMap.end() ) return nullptr;
        return f->second;
    }
}



