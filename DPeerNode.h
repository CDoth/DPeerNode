#ifndef DPEERNODE_H
#define DPEERNODE_H

#include "DPN_Core.h"



class DPeerNode : DPN_CORE {
public:
    DPeerNode();
    void setName( const std::string &name );
    void start( DPN::Thread::Policy p = DPN::Thread::DAEMON_THREAD );

    DPN_FileSystem *fileSystem();

    ///
    /// \brief trcm
    /// \param command
    /// \return
    /// Try command. Text command interface
    bool trcm(const char *command);

    bool share(int port, bool autoaccept);
    bool connect(const char *address, int port);
};

#endif // DPEERNODE_H
