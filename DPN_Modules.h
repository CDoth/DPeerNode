#ifndef DPN_MODULES_H
#define DPN_MODULES_H

#include "DPN_FileSystem.h"
#include "DPN_MediaSystem.h"
#include "DPN_DataStream.h"



class __file_module {
public:
    __file_module();
    ~__file_module();
    inline DPN_FileSystem * fileSystem() {return ptr;}
    inline const DPN_FileSystem * fileSystem() const {return ptr;}
private:
    DPN_FileSystem *ptr;
};
class __net_module {
public:
    __net_module();
private:
};
class __media_module {
public:
    __media_module() { ptr = nullptr; }
    ~__media_module() { }
    inline void setMediaSystem(DPN_MediaSystem *m) { ptr = m; }
    inline DPN_MediaSystem * mediaSystem() {return ptr;}
    inline const DPN_MediaSystem * mediaSystem() const {return ptr;}
private:
    DPN_MediaSystem *ptr;
};
class __data_module {
public:
    __data_module() {ptr = nullptr;}
    ~__data_module() {}
    inline DPN_DataStream * dataModule() {return ptr;}
    inline const DPN_DataStream * dataModule() const {return ptr;}
private:
    DPN_DataStream *ptr;
};

class DPN_Modules
//        :
//public __file_module,
//public __media_module,
//public __data_module
{
public:
    void stopAll();
    void clientDisconnected(const DPN_AbstractClient *client);
    bool addModule(DPN_AbstractModule *m, const std::string &name);
    DPN_AbstractModule * module(const std::string &name);
    const DPN_AbstractModule * module(const std::string &name) const;
private:
    std::map<std::string, DPN_AbstractModule*> iModulesMap;
};

DPN_MediaSystem * extractMediaModule(DPN_Modules &modules);
DPN_FileSystem * extractFileModule(DPN_Modules &modules);

struct GlobalModule {
    DPN_AbstractModule *(*creator)();
    std::string name;
    bool unique;
};
#define DEFULAT_MODULE(NAME, TYPE, UNIQUE) { []() -> DPN_AbstractModule *{return reinterpret_cast<DPN_AbstractModule*>(new TYPE);}, NAME, UNIQUE}
extern GlobalModule defaultModules[];





#endif // DPN_MODULES_H
