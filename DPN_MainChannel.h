#ifndef DPN_MAINCHANNEL_H
#define DPN_MAINCHANNEL_H

#include "DPN_Propagation.h"
#include "DPN_TransmitProcessor.h"
//-----------------------------------------------------------------
struct __packet_header {
    uint32_t size;
    union {
      uint32_t __packet_type;
      DPN_PacketType type;
    };
    uint32_t transaction;
};
struct __packet_header2 {

    union {
      uint32_t __packet_type;
      DPN_PacketType type;
    };
    uint32_t transaction;
};
//-----------------------------------------------------------------
class PacketProcessor : public DPN_IO::IOContext {
public:
    PacketProcessor( );
    void init( bool initiator );
    void send( DPN_TransmitProcessor *p );
    DPN_Result generate(DPN_ExpandableBuffer &b) override;
    DPN_Result process(DPN_ExpandableBuffer &b) override;

private:
    bool __resend( DPN_TransmitProcessor *p );
    bool __makePacket(DPN_TransmitProcessor *proc, DPN_ExpandableBuffer &b);
private: // common:
    DPN_TransactionSpace iSpace;
private: // send direction:
    DPN_ProcessorList iQueue;
    DPN_CrossThreadProcessorList iBackQueue;
};
//-----------------------------------------------------------------
namespace DPN::Client {

    class MonoBase : public DPN_Propagation::LinearScheme {
    public:
        MonoBase( ) : pMirror(nullptr) {}
        void setMirror( MonoBase *mirror) { pMirror = mirror; }
    private:
        void fail() override;
    private:
        MonoBase *pMirror;
    };
    struct __main_channel__ {

        bool __init( DPN_NodeConnector *connector );
        bool __send( DPN_TransmitProcessor *processor );
        PacketProcessor  iBase;
        __channel        wChannel;

        MonoBase iForward;
        MonoBase iBackward;

        __channel_mono_interface backInterface, forwardInterface;
    };
    typedef DWatcher< __main_channel__ > dMainChannel;

    class MainChannel :
            public dMainChannel,
            public DPN::Client::Underlayer
    {
    public:
        MainChannel();
        MainChannel( DPN::Thread::ThreadUser &threadUser, DPN::Modules &modules );
        bool baseInit( DPN_NodeConnector *c );
        bool baseSend( DPN_TransmitProcessor *p );

        DPN_Propagation::LinearScheme * forward() ;
        DPN_Propagation::LinearScheme * backward() ;
    };
}
//-----------------------------------------------------------------

//-----------------------------------------------------------------
#endif // DPN_MAINCHANNEL_H
