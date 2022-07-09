#ifndef DPN_CLIENTCORE_H
#define DPN_CLIENTCORE_H

#include "DPN_MainChannel.h"

namespace DPN::Client {
    class Core :
            public DPN::Client::MainChannel
    {
    public:
        Core();
        Core( DPN::Client::Underlayer ul );

        void coreInit(DPN_NodeConnector *c);
        bool send(DPN_TransmitProcessor *p);
    };

}

//-----------------------------------------------------------------


#endif // DPN_CLIENTCORE_H
