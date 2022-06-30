#include "DPN_MainChannel.h"

using namespace DPN::Logs;
//=======================================================================
PacketProcessor::PacketProcessor() {

}
void PacketProcessor::init(bool initiator, DPN_ClientUnderlayer &u) {
    int minimum = initiator ? 1 : (INT_MAX / 2) + 1;
    iSpace.setMinimumIndex( minimum );
    DL_INFO(1, "initiator: [%d] minimum: [%d]", initiator, minimum);
    wClientUnder = u;
}
void PacketProcessor::send(DPN_TransmitProcessor *p) {

    p->bindClient( wClientUnder );
    p->bindModules( wModules );
    iSpace.registerTransaction( p );
    iBackQueue.push_back( p );

}
DPN_Result PacketProcessor::generate(DPN_ExpandableBuffer &b) {

//    DL_INFO(1, "io_base: [%p]", this);

    if( !iBackQueue.empty() ) { iBackQueue.moveTo(iQueue); }
    auto p = iQueue.getActual();
    if(p == nullptr) return DPN_SUCCESS;

//    DL_INFO(1, "Get actual processor to send: [%p]", p);

    if( p->resendable() ) {
        DL_INFO(1, "Resendable processor: [%p]", p);
        __makePacket( p, b );
        iQueue.popActual();
        return DPN_SUCCESS;
    }
    switch ( p->action() ) {

    case DPN_FAIL:
        DL_INFO(1, "send action fail");
        p->clear();
        iQueue.popActual();
        DPN_PROCS::returnProcessor(p);
        break;
    case DPN_SUCCESS:

//        DL_INFO(1, "send cascade done: product size: [%d]",
//                p->product().size());

        __makePacket( p, b );
        if( !p->resendable() ) {
            p->clear();
            DPN_PROCS::returnProcessor(p);
        }
        iQueue.popActual();
        break;
    case DPN_REPEAT:
//        p->repeat();
//        iQueue.skip();
        break;
    default:break;
    }


    return DPN_SUCCESS;
}
DPN_Result PacketProcessor::process(DPN_ExpandableBuffer &b) {

    if( b.empty() ) return DPN_SUCCESS;
//    DL_INFO(1, "io_base: [%p] data size: [%d]", this, b.size());

//    b.print();

    if( (size_t)b.size() < sizeof (__packet_header2) ) {
        DL_ERROR(1, "Bad packet size: [%d]", b.size());
        return DPN_FAIL;
    }
    const __packet_header2 *h = reinterpret_cast<const __packet_header2*>( b.getData() );
    DL_INFO(1, "header: type: [%s] transaction: [%d]",
            packetTypeName( h->type ),  h->transaction);

    DPN_TransmitProcessor *current = nullptr;
    if( (current = iSpace.transaction( h->transaction )) == nullptr ) {
//        DL_INFO(1, "No transaction [%d], register it", h->transaction);
        if( (current = iSpace.registerTransaction(h->type, h->transaction)) == nullptr ) {
            DL_ERROR(1, "Can't register transaction [%d]", h->transaction);
            return DPN_FAIL;
        }
        current->bindClient( wClientUnder );
        current->bindModules( wModules );
        current->inition();
    }
//    DL_INFO(1, "current processor: [%p]", current);

    if( current ) {


        DPN_ExpandableBuffer packet;
        packet.copy( b.getData() + sizeof(__packet_header2), b.size() - sizeof(__packet_header2));
//        packet.print();
//        DL_INFO(1, "data size: [%d] header2 size: [%d] packet size: [%d]",
//                b.size(), sizeof(__packet_header2), packet.size());


        switch ( current->action( packet ) ) {
        case DPN_FAIL: break;
        case DPN_SUCCESS:
//            DL_INFO(1, "current process [%p] done", current);
            if( current->resendable() ) {
                __resend( current );
            } else {
                current->clear();
                DPN_PROCS::returnProcessor( current );
            }

            break;
        case DPN_REPEAT: break;
        }
    }
    return DPN_SUCCESS;
}
void PacketProcessor::setModules(DPN_Modules m) {
    wModules = m;
}
bool PacketProcessor::__resend(DPN_TransmitProcessor *p) {
//    DL_INFO(1, "Resend processor: [%p]", p);
    iBackQueue.push_back( p );
    return true;
}
bool PacketProcessor::__makePacket(DPN_TransmitProcessor *proc, DPN_ExpandableBuffer &b) {

    __packet_header h;
    h.size = proc->product().size() + sizeof(__packet_header) - sizeof(uint32_t);
    h.type = proc->me();
    h.transaction = proc->transaction();



//    DPN_ExpandableBuffer packet;

    b.clear();
    b.appendValue( h );
    b.append( proc->product() );

//    DL_INFO(1, "make packet: size: [%d] type: [%s] transaction: [%d] packet size: [%d]",
//            h.size, packetTypeName(h.type), h.transaction, b.size());

//    b.print();

    return true;
}

//=======================================================================

namespace DPN::Client {
    void MonoBase::fail() {
        DL_INFO(1, "mono channel [%p] failed. block mirror: [%p]", this, pMirror);
        if( pMirror ) {
            pMirror->block();
        }
    }
    bool __main_channel__::__init(DPN_NodeConnector *connector) {
        DL_INFO(1, "try init base channel");

        if( connector == nullptr ) return false;

        wChannel.init( connector, "Main" );
        iBase.init( connector->isInitiator() );



        forwardInterface = wChannel.getMonoIf( DPN::FORWARD );
        backInterface = wChannel.getMonoIf( DPN::BACKWARD );

        DL_INFO(1, "got interface: [%d] [%d]", backInterface.validInterface(), forwardInterface.validInterface());

        DL_INFO(1, "back io: [%p] forward io: [%p]", backInterface.io(), forwardInterface.io());

        iForward.setEntry( &iBase );
        iForward.connect(  forwardInterface.io() );

        iBackward.setEntry( backInterface.io() );
        iBackward.connect( &iBase );

        iForward.setMirror( &iBackward );
        iBackward.setMirror( &iForward );

        return true;
    }
    bool __main_channel__::__send(DPN_TransmitProcessor *processor) {
        if( processor == nullptr ) {
            DL_BADPOINTER(1, "processor");
            return false;
        }
        iBase.send( processor );
        return true;
    }
    MainChannel::MainChannel() {

    }
    MainChannel::MainChannel(DPN::Thread::ThreadUser &threadUser, DPN::Modules &modules) :
        dMainChannel(true),
        DPN::Client::Underlayer( threadUser, modules )
    {}
    bool MainChannel::baseInit(DPN_NodeConnector *c) { return dMainChannel::data()->__init( c ); }
    bool MainChannel::baseSend(DPN_TransmitProcessor *p) { return dMainChannel::data()->__send( p ); }
    DPN_Propagation::LinearScheme *MainChannel::forward() { return &dMainChannel::data()->iForward; }
    DPN_Propagation::LinearScheme *MainChannel::backward() { return &dMainChannel::data()->iBackward; }





}

