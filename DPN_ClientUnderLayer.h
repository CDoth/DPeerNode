#ifndef DPN_CLIENTUNDERLAYER_H
#define DPN_CLIENTUNDERLAYER_H

#include "DPN_Channel.h"
#include "DPN_ThreadBridge.h"
#include "DPN_Modules.h"

namespace DPN::Client {
    struct Stats {
        PeerAddress iRemote;
        PeerAddress iLocal;
        PeerAddress iAvatar;
    };
    struct __underlayer__ {
        __underlayer__() {}
        DArray<__channel> aShadowChannels;
        Stats iStats;
    };
    typedef DWatcher< __underlayer__ > dUnderLayer;
    class Underlayer :
            private dUnderLayer,
            public DPN::Thread::ThreadUser,
            public DPN::Modules
    {
    public:
        Underlayer( DPN::Thread::ThreadUser &threadUser, DPN::Modules &modules );
        void init( DPN_NodeConnector *c );

        bool isShadowAvailable(DXT::Type, int port);
        PeerAddress remoteAddress();
        bool addShadowConnection(DPN_NodeConnector *udpPort, const std::string shadowKey);
        bool registerShadow(const std::string &shadowKey);
        DArray<__channel> shadows() const ;
        __channel channel( const std::string &key );
    };
}




#endif // DPN_CLIENTUNDERLAYER_H
