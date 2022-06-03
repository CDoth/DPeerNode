#include "DPN_Direction.h"


//-------------------------------------------------------------------- DPN_Direction
DPN_Direction::DPN_Direction() {
    inThread = false;
    blocked = false;
    errorState = NO_FAIL;
}
DPN_Direction::~DPN_Direction() {
}
bool DPN_Direction::threadInjection() {
    return true;
}
void DPN_Direction::block() {
    blocked = true;
}
//-------------------------------------------------------------------- DPN_SendDirection
DPN_SendDirection::DPN_SendDirection() {
//    environment.setBackList(&backList);
}
DPN_SendDirection::~DPN_SendDirection() {
}
void DPN_SendDirection::putProcessor(DPN_TransmitProcessor *proc) {
    proc->inition(clientContext, threadContext);
    threadContext.send(proc);
}
bool DPN_SendDirection::proc() {

    // push & move - mutex
    // push & empty - one pointer check
    // move & empty - in one thread

    if( !threadContext.backList()->empty() ) {
        threadContext.backList()->moveTo(queue);
    }
    auto p = queue.getActual();
    if(p == nullptr) return true;


    switch (p->sendCascade()) {

    case DPN_FAIL:
        if(clientContext.connector()->state() == DPN_FAIL) {

            DL_INFO(1, "DPN_SendDirection: Client disconnected");

            DPN_CONNECTION_FAULT_PROCESSING;

            errorState = CONNECTION_FAIL;
            return false;

            break;
        }
        // Continue to DPN_SUCCESS code (if no connection fail):
    case DPN_SUCCESS:
        p->clear();
        queue.popActual();
        threadContext.returnProcessor(p);
        break;
    case DPN_REPEAT:
        p->repeat();
        queue.skip();
        break;
    case DPN_KEEP:
        queue.skip();
        break;


    default:break;

    }


    return true;
}
bool DPN_SendDirection::close() {
    if(inThread) {
        DL_ERROR(1, "Send direction still in thread");
        return false;
    }

//    queue.returnAll(environment);
//    backList.returnAll(environment);

    return true;
}
//-------------------------------------------------------------------- DPN_ReceiveDirection
Receiver::Receiver(DPN_Direction *d) : line(this) {
    direction = d;
    zero_mem(&r_header, 1);
    makeLine();
}
DPN_Result Receiver::receive() {
    return line.go();
}
DPN_Result Receiver::recv_header() {

    if( direction->clientContext.connector()->x_receiveValue(r_header) == DPN_SUCCESS ) {
        if( checkType() && checkSize() ) return DPN_SUCCESS;
        return DPN_FAIL;
    }
    return direction->clientContext.connector()->state();
}
DPN_Result Receiver::recv_packet() {

    if( r_header.size == 0 ) return packetSupportEmptySize(r_header.type) ? DPN_SUCCESS : DPN_FAIL ;

//    DL_INFO(1, "direction: [%p] tc: [%p] buffer: [%p]",
//            direction, direction->tc().data(), direction->tc().buffer().data()
//            );

    return direction->clientContext.connector()->x_receive(direction->threadContext.buffer(), r_header.size);

}
void Receiver::makeLine() {

    line << &Receiver::recv_header
         << &Receiver::recv_packet
            ;
}
bool Receiver::checkType() {
    return (r_header.__packet_type > 0 && r_header.__packet_type < DPN_PACKETTYPE__INVALID_TYPE);
}
bool Receiver::checkSize() {
    return r_header.size < packetMaxSize(r_header.type);
}
//============================================================================================= DPN_ReceiveDirection:
DPN_ReceiveDirection::DPN_ReceiveDirection() : R(this) {
    current_process = nullptr;
}
bool DPN_ReceiveDirection::proc() {

    if( current_process ) {
        switch ( current_process->receiveCascade() ) {
        case DPN_FAIL:
            if(clientContext.connector()->state() == DPN_FAIL) {

                DL_INFO(1, "DPN_ReceiveDirection: Client disconnected");

                DPN_CONNECTION_FAULT_PROCESSING;
                errorState = CONNECTION_FAIL;
                return false;

                break;
            }
            // Continue to DPN_SUCCESS code (if no connection fail):
        case DPN_SUCCESS:
            if( current_process->isResendable() ) {
                current_process->repeat();
                threadContext.send(current_process);
            } else {
                current_process->clear();
                threadContext.returnProcessor(current_process);
            }
            current_process = nullptr;
            R.restart();
            break;
        case DPN_REPEAT:
            // Call again from begin
            current_process->repeat();
            break;
        case DPN_KEEP:
            // Call again from last point
            break;

        default:break;
        }
    }
    else if ( clientContext.connector()->readable() ) {

        DPN_Result r = R.receive();

        if( r == DPN_SUCCESS ) {

//            DL_INFO(1, "receive: type: [%d] size: [%d] direction: [%p] tc: [%p] buffer: [%p]",
//                    R.type(), R.size(),
//                    this, threadContext.data(), threadContext.buffer().data()
//                    );

            if( (current_process = threadContext.processor(R.type())) == nullptr ) {
                DL_ERROR(1, "Can't get processor");
                errorState = PROCESSOR_POOL_FAIL;
                R.restart();
                return false;
            }
            current_process->inition(clientContext, threadContext);


        } else if ( r == DPN_FAIL ) {

            if( clientContext.connector()->state() == DPN_FAIL ) {
                DL_ERROR(1, "Connection fail");
                R.restart();
                errorState = CONNECTION_FAIL;
                return false;
            }

            DL_ERROR(1, "Bad packet format: type: [%d] size: [%d] max size: [%d]",
                     R.type(), R.size(), packetMaxSize(R.type()));
            errorState = BAD_PACKET_FORMAT;
            R.restart();
            return false;
        }

    }


    return true;
}
bool DPN_ReceiveDirection::close() {
    if(inThread) {
        DL_ERROR(1, "Receive direction still in thread");
        return false;
    }
    return true;
}
//================================================================================================= DPN_ProxyDirection
/*
DPN_ProxyDirection::DPN_ProxyDirection(DPN_NodeConnector *from, DPN_NodeConnector *to) : __from(from), __to(to), __sendPhase(false)
{}
bool DPN_ProxyDirection::proc() {

    DPN_Result r = DPN_REPEAT;

    if( !__sendPhase ) {
        if( __from->readable() ) {
            if( (r = __from->x_receivePacket()) == DPN_FAIL ) return false;
            else if ( r == DPN_SUCCESS ) __sendPhase = !__sendPhase;
        }
    }
    if( __sendPhase ) {
        if( (r = __to->x_sendPacket( __from->bufferData(), __from->receivedPacketSize() ) ) == DPN_FAIL ) return false;
        else if ( r == DPN_SUCCESS ) __sendPhase = !__sendPhase;
    }

    return true;
}
bool DPN_ProxyDirection::close() {
    if(inThread) {
        DL_ERROR(1, "Receive direction still in thread");
        return false;
    }
    __clean();

    return true;
}
void DPN_ProxyDirection::__clean() {
    __from = nullptr;
    __to = nullptr;
    __sendPhase = false;
}
*/


