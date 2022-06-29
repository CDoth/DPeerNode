#ifndef DPN_CLIENTUNDERLAYER_H
#define DPN_CLIENTUNDERLAYER_H

#include "DPN_Channel.h"


struct ClientStats {
    PeerAddress iRemote;
    PeerAddress iLocal;
    PeerAddress iAvatar;
};
struct __client_underlayer__ {
    __client_underlayer__() : pTag(nullptr) {}
    DArray<__channel> aShadowChannels;
    ClientStats iStats;
    DPN_ClientTag *pTag;
};
class DPN_ClientUnderlayer : private DWatcher< __client_underlayer__ > {
public:
    void init( DPN_NodeConnector *c );
    DPN_ClientUnderlayer();
    DPN_ClientUnderlayer( DPN_ClientTag *t, bool create = false);
    bool isShadowAvailable(DXT::Type, int port);
    PeerAddress remoteAddress();
    bool addShadowConnection(DPN_NodeConnector *udpPort, const std::string shadowKey);
    bool registerShadow(const std::string &shadowKey);
    const DArray<__channel> & shadows() const ;
    const DPN_ClientTag * tag() const;
    __channel channel( const std::string &key );
};

#endif // DPN_CLIENTUNDERLAYER_H
