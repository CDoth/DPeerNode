#ifndef DPN_CORE_H
#define DPN_CORE_H


//1

#include <DArray.h>

#include "DPN_ThreadMaster.h"


class DPN_Core {
public:
    DPN_Core();

public:

    void setName(const std::string &name);
    const std::string & name() const;
    bool share(int port, bool autoaccept);
    bool connect(const char *address, int port);
    bool sendMessage(const char *message);
    bool videoCall();

    DPN_Client & client(int index);
    DPN_MediaSystem &media();

    void acceptAll();
public:
    bool trcm(const char *command);

public:
    bool start();
    void addThread();

    //File module:
public:
    DPN_Catalog & catalog() {return connections_core.host_catalog;}
    bool sync();
    bool requestRemoteFile(int key);
    bool requestRemoteFileSet(const DArray<int> &keys);
    bool requestRemoteDirectory(const DDirectory *directory);
    bool requestRemoteCatalog();
private:

    DPN_ConnectionsCore connections_core;

};

#endif // DPN_CORE_H
