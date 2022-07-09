#ifndef DPN_MAINCHANNEL_H
#define DPN_MAINCHANNEL_H

#include "DPN_Propagation.h"
#include "DPN_TransmitProcessor.h"

//-----------------------------------------------------------------
namespace DPN::Client {

    class MonoBase
            : public DPN_Propagation::LinearScheme
            , public DPN::Client::Underlayer
    {
    public:
        MonoBase()  {}
        MonoBase( DPN::Client::Underlayer &ul ) : DPN::Client::Underlayer(ul) {}
        void setMirror( DPN::Thread::ThreadUnit mirror) { wMirror = mirror; }
    private:
        void fail() override;
    private:
        DPN::Thread::ThreadUnit wMirror;
    };
    struct __main_channel__ {


        __main_channel__();
        __main_channel__( Underlayer ul );

        bool __init( DPN_NodeConnector *connector );
        bool __send( DPN_TransmitProcessor *processor );

        PacketProcessor  iBase;
        __channel        wChannel;

        DPN::Thread::ThreadUnitWrapper<MonoBase> qf;
        DPN::Thread::ThreadUnitWrapper<MonoBase> qb;


        __channel_mono_interface backInterface, forwardInterface;
    };

    typedef DWatcher< __main_channel__ > dMainChannel;

    class MainChannel :
            public dMainChannel,
            public DPN::Client::Underlayer
    {
    public:
        MainChannel();
        MainChannel( DPN::Client::Underlayer ul );

        bool badClient() const;


        bool baseInit( DPN_NodeConnector *c );
        bool baseSend( DPN_TransmitProcessor *p );

        DPN::Thread::ThreadUnit forward() ;
        DPN::Thread::ThreadUnit backward() ;
    };
}


//-----------------------------------------------------------------
#endif // DPN_MAINCHANNEL_H
