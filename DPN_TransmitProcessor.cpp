#include "DPN_TransmitProcessor.h"

using namespace DPN::Logs;
ProcessorMapWrapper PROCESSOR_CREATOR;

//====================================================================================== PacketProcessor:
PacketProcessor::PacketProcessor()
{}
PacketProcessor::PacketProcessor(Underlayer ul)
    : DPN::Client::Underlayer(ul)
{

}
void PacketProcessor::init(bool initiator) {

    int minimum = initiator ? 1 : (INT_MAX / 2) + 1;
    iSpace.setMinimumIndex( minimum );
    DL_INFO(1, "initiator: [%d] minimum: [%d]", initiator, minimum);
}
void PacketProcessor::send(DPN_TransmitProcessor *p) {

//    DL_INFO(1, "PacketProcessor send: processor: [%p]", p);

//    DL_INFO( 1, "binding...");
    p->bind( *this );
//    DL_INFO( 1, "register...");
    iSpace.registerTransaction( p );
//    DL_INFO( 1, "append to queue...");
    iBackQueue.push_back( p );
//    DL_INFO( 1, "ready to send!");

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
    DL_INFO(1, "io_base: [%p] data size: [%d]", this, b.size());

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
        current->bind( *this );
        current->inition();
    }
    DL_INFO(1, "current processor: [%p]", current);

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
//====================================================================================== DPN_ProcessorList:
void DPN_ProcessorList::push_back(DPN_TransmitProcessor *proc)  {




    __node *node = getNode(proc);


    if(__head) {
        __head->next = node;
        node->prev = __head;
    }
    __head = node;

    if(!__actual) __actual = node;


    if(!__first) __first = node;
}
void DPN_ProcessorList::push_front(DPN_TransmitProcessor *proc)  {

    __node *node = getNode(proc);

    if(__first) {
        node->next = __first;
        __first->prev = node;
    }
    if(!__head) __head = node;
    __first = node;

    if(!__actual) __actual = node;

}
void DPN_ProcessorList::insert_to_actual(DPN_TransmitProcessor *proc)  {

    __node *node = getNode(proc);

    if(!__actual) return push_front(proc);
    if(__actual == __first) return push_front(proc);



    if(__actual->prev)
        __actual->prev->next = node;

    node->prev = __actual->prev;
    node->next = __actual;

    __actual->prev = node;

}
DPN_TransmitProcessor *DPN_ProcessorList::getActual()  {

    return __actual ? __actual->proc : nullptr;

}
void DPN_ProcessorList::popActual()  {

    if(!__actual) return;

    // connect sides
    __actual->exclude();

    // change first & head if need
    if(__actual == __first) __first = __actual->next;
    if(__actual == __head) __head = __actual->prev;

    // move actual
    skip();
}
void DPN_ProcessorList::skip()  {

    if(__actual->next) __actual = __actual->next;
    else __actual = __first;
}
void DPN_ProcessorList::restart()  {

    __actual = __first;
}
void DPN_ProcessorList::move(DPN_ProcessorList &list) {
    auto f = list.__first;

    if(__first == nullptr) {
        __first = f;
        __head = list.__head;
        __actual = __first;
    } else {
        __head->next = f;
        f->prev = __head;
        __head = list.__head;
    }

    list.zero();
}
void DPN_ProcessorList::clear() {
    while(__first != __head) {
        removeNode(__first);
        __first = __first->next;
    }
    zero();
}
//========================================================================================================== DPN_CrossThreadProcessorList
void DPN_CrossThreadProcessorList::push_back(DPN_TransmitProcessor *proc) {
    std::lock_guard<std::mutex> lock(__mutex);
    DPN_ProcessorList::push_back(proc);
}
void DPN_CrossThreadProcessorList::moveTo(DPN_ProcessorList &list) {
    std::lock_guard<std::mutex> lock(__mutex);
    list.move(*this);
}
//=============================================================================================== DPN_TransactionSpace
DPN_TransactionSpace::DPN_TransactionSpace() {
    iMinimumIndex = 1;
}
void DPN_TransactionSpace::setMinimumIndex(int min) {
    if( min > 1 ) iMinimumIndex = min;
}
void DPN_TransactionSpace::registerTransaction(DPN_TransmitProcessor *t) {
    if( t == nullptr ) {
        DL_BADPOINTER(1, "tr");
        return;
//        return nullptr;
    }
    if( iMap.size() + iMinimumIndex < 0 ) {
        DL_ERROR(1, "No available index");
        return;
    }
    t->inition();
    t->iTransactionIndex = iMap.size() + iMinimumIndex;
//    t->pCreator = this;
//    setFlag( t->iFlags, DPN_TransmitProcessor::PF__GENERATOR );
    iMap[t->iTransactionIndex] = t;

    //    return t;
}
DPN_TransmitProcessor *DPN_TransactionSpace::registerTransaction(PacketType t, int key) {

    if( iMap.find(key) != iMap.end() ) {
        DL_ERROR(1, "Transaction with key [%d] already registered", key);
        return nullptr;
    }

    auto proc = DPN_PROCS::processor(t);
    if( proc == nullptr ) {
        DL_BADPOINTER(1, "processor");
        return nullptr;
    }
    proc->iTransactionIndex = key;
//    proc->pCreator = this;
    proc->iPosition = DPN_SERVER;
    iMap[key] = proc;
    return proc;
}
DPN_TransmitProcessor *DPN_TransactionSpace::transaction(int index) const {
    auto f = iMap.find(index);
    if( f == iMap.end() ) return nullptr;
    return f->second;
}
bool DPN_TransactionSpace::isValidKey(int key) const {
    return iMap.find(key) == iMap.end();
}
//====================================================================================  ProcessorMapWrapper
DPN_TransmitProcessor *ProcessorMapWrapper::create(PacketType t) {
    if( map.find(t) == map.end() ) {
        DPN_GET_PRC_MAP(PROCESSOR_CREATOR);
        if( map.find(t) == map.end() ) {
            DL_ERROR(1, "No processor creator for type: [%d][%s]", t, packetTypeName(t));
            return nullptr;
        }
    }
    return map[t]();
//    return (map.find(t) == map.end()) ? nullptr : map[t]();
}
void ProcessorMapWrapper::fill(PacketType t, CREATE_PROCESSOR callback) {
    if(map.find(t) == map.end()) {
        map[t] = callback;
    }
}
ProcessorMapFiller::ProcessorMapFiller(PacketType t, CREATE_PROCESSOR callback, ProcessorMapWrapper *out) {

    static ProcessorMapWrapper wrapper;

    if(out)  *out = wrapper;
    else     wrapper.fill(t, callback);
}
//====================================================================================  DPN_ProcessorPool


DPN_ProcessorPool DPN_PROCS::__global_ppool;

DPN_ProcessorPool::DPN_ProcessorPool() {
        DPN_GET_PRC_MAP(PROCESSOR_CREATOR);

//        const PROCMAP_T & __inner = PROCESSOR_CREATOR.inner();
//        if(__inner.empty()) {
//            DL_WARNING(1, "Empty processor creator map");
//        } else {
//            for(auto it: __inner) {
//                map[it.first] = DPN_TransmitProcessorMicroPool();
//            }
//        }
}
DPN_TransmitProcessor *DPN_ProcessorPool::getProcessor(PacketType t) {

//    if(map.find(t) == map.end()) {
//        DL_ERROR(1, "Bad packet type: [%d:%s]", t, packetTypeName(t));
//        return nullptr;
//    }
    __insertType(t);

    DPN_TransmitProcessorMicroPool &micro_pool = map[t];
    DPN_TransmitProcessor *p = micro_pool.runtime__get(t);
    if( p == nullptr ) {
        DL_FUNCFAIL(1, "runtime__get");
        return nullptr;
    }
    return p;
}
bool DPN_ProcessorPool::returnProcessor(DPN_TransmitProcessor *proc) {

    if(proc == nullptr) {
        return false;
    }
    auto f = map.find(proc->me());
    if(f == map.end()) {
        return false;
    }

    f->second.insertProcessor(proc);
    return true;
}
void DPN_ProcessorPool::__insertType(PacketType t) {
    if( map.find(t) == map.end() ) map[t] = DPN_TransmitProcessorMicroPool();
}

DPN_TransmitProcessor::DPN_TransmitProcessor() : DPN::Client::Underlayer() {
    pLine = nullptr;
    iPosition = DPN_HOST;

    iTransportFlags = 0;
    iInnerFlags = 0;
    iUserFlags = 0;

    iContent.setTotalSizeParsing( false );


}
DPN_TransmitProcessor::~DPN_TransmitProcessor() {

}
void DPN_TransmitProcessor::bind(Underlayer underLayer) {


    this->Underlayer::operator=( underLayer );
}
DPN_Result DPN_TransmitProcessor::action() {

    if( pLine == nullptr ) {
        DL_BADPOINTER(1, "No line");
        return DPN_FAIL;
    }
    DPN_Result r = DPN_SUCCESS;


    disableResendable();
    DL_INFO(1, "do [%s] action (step: [%d])", getCallerName( iPosition ), pLine->step());
    r = pLine->go( iPosition );
    switch (r) {
    case DPN_FAIL:
        DL_INFO(1, "[%s] step failed", getCallerName( iPosition ));
        processFault( iPosition );
        DPN::setFlag( iTransportFlags, TF__FAILED );

        return DPN_FAIL;
        break;
    case DPN_REPEAT:
        break;
    case DPN_SUCCESS:
        break;
    }

    if( pLine->isOver() == false ) {
        DL_INFO(1, "Host line continue: resend (step: [%d])", pLine->step());
        makeResendable();
    }



    iContent.parseBuffers();
    DL_INFO(1, "[%s] action result: [%d] parsed data: [%d]", getCallerName( iPosition ), r, iContent.buffer().size());


    return DPN_SUCCESS;
}
DPN_Result DPN_TransmitProcessor::action(DPN_ExpandableBuffer &packet) {
    if( pLine == nullptr ) {
        DL_BADPOINTER(1, "No line");
        return DPN_FAIL;
    }
    if( iContent.deparseBuffer( packet ) == false ) {
        DL_FUNCFAIL(1, "deparseBuffer");
        return DPN_FAIL;
    }
    iTransportFlags = UNIT_FLAGS.get();
    disableResendable();

    if( DPN::checkFlag( iTransportFlags, TF__FAILED) ) {
        DL_WARNING(1, "Remote side failed");
        processFault( iPosition );
        return DPN_FAIL;
    }
    DPN_Result r;
    DL_INFO(1, "do [%s] action (step: [%d])",  getCallerName(iPosition), pLine->step());
    r = pLine->go( iPosition );
    switch (r) {
    case DPN_FAIL:
        DL_INFO(1, "Server step failed");
        processFault( iPosition );
        DPN::setFlag( iTransportFlags, TF__FAILED );
        break;
    case DPN_REPEAT:
        break;
    case DPN_SUCCESS:
        break;
    }
    DL_INFO(1, "action result: [%d]", r);

    if( pLine->isOver() == false ) {
        DL_INFO(1, "Server line continue: resend (step: [%d])", pLine->step());
        makeResendable();
        UNIT_FLAGS = iTransportFlags;
        iContent.parseBuffers();
    }

    return DPN_SUCCESS;

}
void DPN_TransmitProcessor::clear() {
    if( pLine ) {
        pLine->restart();
    }
    iPosition = DPN_HOST;
}

const DPN_ExpandableBuffer &DPN_TransmitProcessor::product() const {
    return iContent.buffer();
}
bool DPN_TransmitProcessor::inition() {
    if( pLine == nullptr ) {
        makeActionLine();
    }
    if( pLine == nullptr ) {
        DL_WARNING(1, "makeActionLine does't create action line");
        return false;
    }
    injection();
    pLine->setTarget( this );
    return true;
}
void DPN_TransmitProcessor::processFault(CallerPosition p) {
    return p == DPN_HOST ? fault( HOST_CALL() ) : fault( SERVER_CALL() );
}

void DPN_TransmitProcessor::injection(){}
void DPN_TransmitProcessor::fault(SERVER_CALL) {}
void DPN_TransmitProcessor::fault(HOST_CALL) {}
