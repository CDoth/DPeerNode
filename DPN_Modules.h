#ifndef DPN_MODULES_H
#define DPN_MODULES_H

#include "__dpeernode_global.h"



namespace DPN {
    struct __modules__ {
        std::map<std::string, DPN_AbstractModule*> iModulesMap;
    };
    class Modules : private DWatcher<__modules__> {
    public:
        Modules(bool create = false);
        void stopAll();
        bool addModule(DPN_AbstractModule *m, const std::string &name);
        DPN_AbstractModule * module(const std::string &name);
        const DPN_AbstractModule * module(const std::string &name) const;
    };
    struct GlobalModule {
        DPN_AbstractModule *(*creator)();
        std::string name;
        bool unique;
    };
}








#endif // DPN_MODULES_H
