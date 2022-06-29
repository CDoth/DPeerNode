#ifndef DPEERNODE_H
#define DPEERNODE_H


//1

#include <DArray.h>


#include "DPN_Core.h"

/*
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

//    DPN_MediaSystem &media();

    void acceptAll();
public:
    bool trcm(const char *command);

public:
    bool start();
    void addThread();

    //File module:
public:
    DPN_Catalog *catalog();
    bool sync();
    bool requestRemoteFile(int key);
    bool requestRemoteFileSet(const DArray<int> &keys);
    bool requestRemoteDirectory(const DDirectory *directory);
    bool requestRemoteCatalog();
private:

    DPN_ConnectionsCore connections_core;

};
*/

class DPeerNode : DPN_CORE {
public:
    DPeerNode();
    void start( DPN::Thread::Policy p = DPN::Thread::DAEMON_THREAD );

    ///
    /// \brief trcm
    /// \param command
    /// \return
    /// Try command. Text command interface
    bool trcm(const char *command);
};

#endif // DPEERNODE_H
