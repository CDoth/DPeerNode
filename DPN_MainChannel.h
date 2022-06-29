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
    void init( bool initiator, DPN_ClientUnderlayer &u );
    void send( DPN_TransmitProcessor *p );
    DPN_Result generate(DPN_ExpandableBuffer &b) override;
    DPN_Result process(DPN_ExpandableBuffer &b) override;

    void setModules( DPN_Modules m );
private:
    bool __resend( DPN_TransmitProcessor *p );
    bool __makePacket(DPN_TransmitProcessor *proc, DPN_ExpandableBuffer &b);
private:
    DPN_ClientUnderlayer wClientUnder;
    DPN_Modules wModules;
private: // common:
    DPN_TransactionSpace iSpace;
private: // send direction:
    DPN_ProcessorList iQueue;
    DPN_CrossThreadProcessorList iBackQueue;
};
//-----------------------------------------------------------------
class __base_mono : public DPN_Propagation::LinearScheme {
public:
    __base_mono( ) : pMirror(nullptr) {}
    void setMirror( __base_mono *mirror) { pMirror = mirror; }
private:
    void fail() override;
private:
    __base_mono *pMirror;
};
//-----------------------------------------------------------------
class __base_channel : public DPN_ClientTag {
public:
    __base_channel();
    bool baseInit( DPN_NodeConnector *c, DPN_ClientUnderlayer &u );
    bool baseSend( DPN_TransmitProcessor *p );
    void setModules( DPN_Modules m );

    DPN_Propagation::LinearScheme * forward() {return &iForward;}
    DPN_Propagation::LinearScheme * backward() {return &iBackward;}
private:
    PacketProcessor  iBase;
    __channel        wChannel;

    __base_mono iForward;
    __base_mono iBackward;
private:
    __channel_mono_interface backInterface, forwardInterface;
};
//-----------------------------------------------------------------
#endif // DPN_MAINCHANNEL_H
