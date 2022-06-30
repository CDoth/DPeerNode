#ifndef DPN_CLIENTCORE_H
#define DPN_CLIENTCORE_H

#include "DPN_MainChannel.h"
#include "DPN_ThreadBridge.h"

namespace DPN::Client {
    class Core :
            public DPN::Client::MainChannel
    {
    public:
        friend class DPN_ClientUnderlayerInterface;
        friend class DPN_ClientInterface;

        Core();
        Core( DPN::Thread::ThreadUser &threadUser, DPN::Modules &modules );
        void init(DPN_NodeConnector *c);
        bool send(DPN_TransmitProcessor *p);
        const DArray<__channel> & shadows() const;
    };
}

//-----------------------------------------------------------------


#endif // DPN_CLIENTCORE_H
