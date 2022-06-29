#ifndef DPN_CLIENTCORE_H
#define DPN_CLIENTCORE_H

#include "DPN_MainChannel.h"
#include "DPN_ThreadBridge.h"


class DPN_ClientCore : public __base_channel, public DPN::Thread::ThreadUser {
public:
    friend class DPN_ClientUnderlayerInterface;
    friend class DPN_ClientInterface;

    DPN_ClientCore();
    DPN_ClientCore( const DPN::Thread::ThreadUser &sharing );
    void init(DPN_NodeConnector *c);
    bool send(DPN_TransmitProcessor *p);
    const DArray<__channel> & shadows() const;
private:
    DPN_ClientUnderlayer wClientUnder; // orig
};
//-----------------------------------------------------------------


#endif // DPN_CLIENTCORE_H
