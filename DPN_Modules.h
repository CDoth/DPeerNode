#ifndef DPN_MODULES_H
#define DPN_MODULES_H

#include "__dpeernode_global.h"

class DPN_AbstractModule {
public:
    DPN_AbstractModule(const std::string &name) : iModuleName(name) {}
    virtual ~DPN_AbstractModule() {}
    virtual void stop() {};

    virtual bool useChannel( DPN::Client::Tag tag, DPN::Direction d, __channel_mono_interface mono, const DPN_ExpandableBuffer &context ) = 0;
    virtual void clientOver( DPN::Client::Tag tag ) {}


    const std::string & name() const {return iModuleName;}
private:
    std::string iModuleName;
};
namespace DPN {
    struct __modules__ {
        std::map<std::string, DPN_AbstractModule*> iModulesMap;
    };
    class Modules : private DWatcher<__modules__> {
    public:
        using DWatcher< __modules__ >::copy;

        void clientDisconnected( DPN::Client::Tag tag );
        Modules(bool create = false);
        void stopAll();
        bool addModule(DPN_AbstractModule *m, const std::string &name);
        DPN_AbstractModule * module(const std::string &name);
        const DPN_AbstractModule * module(const std::string &name) const;
    };

}








#endif // DPN_MODULES_H
