#include "DPN_TransmitProcessor.h"

using namespace DPN::Logs;
ProcessorMapWrapper PROCESSOR_CREATOR;
//====================================================================================== DPN_TransmitProcessor:
/*
thread_local __action_line<DPN_TransmitProcessor> DPN_TransmitProcessor::iGlobalSendLine;
thread_local __action_line<DPN_TransmitProcessor> DPN_TransmitProcessor::iGlobalReceiveLine;


DPN_TransmitProcessor::DPN_TransmitProcessor() {

    iFlags = 0;
    iResendFlag = false;
    content.setTotalSizeParsing( false );
    makeCascades();
}
DPN_TransmitProcessor::~DPN_TransmitProcessor() {

}
DPN_Result DPN_TransmitProcessor::sendCascade() {
    return iSendCascade.go();
}
DPN_Result DPN_TransmitProcessor::receiveCascade() {
    return iReceiveCascade.go();
}
//------------------------------------------------------------------
DPN_Result DPN_TransmitProcessor::sendPrepare() {

//    INFO_LOG;

    if( checkFlag(PF__FAIL) ) {
        DL_INFO(1, "sending in FAIL mode");
        return DPN_SUCCESS;
    }
//    bool isGenerator = checkFlag(PF__GENERATOR);
//    bool snakeMode = checkFlag(PF__SNAKEMODE);

//    DL_INFO(1, "proc: [%p] is generator: [%d] snake: [%d] fail: [%d] flags: [%d]", this, isGenerator, snakeMode, checkFlag(PF__FAIL), iFlags);
//    if( !isGenerator && !snakeMode ) return DPN_SUCCESS;

    DPN_Result r;
    r = sendPrefix();
    if( r == DPN_FAIL ) {
        setFlag(PF__FAIL);
        return failureProcessing();
    }
    return DPN_SUCCESS;
}
DPN_Result DPN_TransmitProcessor::send() {


//    INFO_LOG;

//    if( checkFlag(PF__DIALOG_OVER) ) {
//        disableFlag(PF__DIALOG_OVER);
//        return DPN_SUCCESS;
//    }

    UNIT_FLAGS = iFlags;
//    content.reservePrefix(1);
    content.parseBuffers();
//    content.setPrefix(me(), 0);



//    if( clientContext.connector() == nullptr ) {
//        DL_BADPOINTER(1, "connector");
//        return DPN_FAIL;
//    }
//    if( clientContext.connector()->x_send(content.buffer()) == DPN_FAIL ) {
//        return DPN_FAIL;
//    }
//    content.clearBuffer();
    return DPN_SUCCESS;
}
DPN_Result DPN_TransmitProcessor::sendSuffix() {

//    INFO_LOG
    return sendReaction();
    return DPN_SUCCESS;
}
//------------------------------------------------------------------
DPN_Result DPN_TransmitProcessor::receivePrepare() {

//    INFO_LOG

//    if( content.deparseBuffer(threadContext.buffer()) == false ) {
//        DL_FUNCFAIL(1, "deparseBuffer");
//        return DPN_FAIL;
//    }

    iFlags = UNIT_FLAGS.get();
//    inverseFlag(PF__GENERATOR);
//    int tri = UNIT_TRANSACTION.get();

//    DL_INFO(1, "flags: [%d] {check generator: [%d] fail: [%d] snake: [%d] }transaction: [%d]",
//            iFlags, checkFlag(PF__GENERATOR), checkFlag(PF__FAIL), checkFlag(PF__SNAKEMODE), tri);


//    threadContext.buffer().clear();

    return receivePrefix();
}
DPN_Result DPN_TransmitProcessor::receive() {
//    INFO_LOG
    return DPN_SUCCESS;
}
DPN_Result DPN_TransmitProcessor::receiveSuffix() {

//    INFO_LOG;
    if( checkFlag(PF__FAIL) ) {
        DL_INFO(1, "Receive packet in FAIL mode");
        failureProcessing();
        return DPN_FAIL;
    }

//    bool isGenerator = checkFlag(PF__GENERATOR);
//    bool snakeMode =  checkFlag(PF__SNAKEMODE);
//    DL_INFO(1, "is generator: [%d] snake mode: [%d]", isGenerator, snakeMode);
//    if( isGenerator && !snakeMode ) return DPN_SUCCESS;

    DPN_Result r = receiveReaction();
    if( r == DPN_FAIL ) {
        setFlag(PF__FAIL);
        return failureProcessing();
    }
    return r;
}
DPN_Result DPN_TransmitProcessor::receiveResender() {
//    INFO_LOG;
    if( resendPredicat() ) resend();
    return DPN_SUCCESS;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void DPN_TransmitProcessor::repeat() {
//    INFO_LOG
    repeatProcessor();
    iResendFlag = false;
}
void DPN_TransmitProcessor::clear() {
//    INFO_LOG
    clearProcessor();

    iSendCascade.restart();
    iReceiveCascade.restart();

    initionTime.clear();
    packet_buffer.clear();

//    pTransaction = nullptr;
    iFlags = 0;
    iResendFlag = false;
    UNIT_TRANSACTION = -1;
    UNIT_FLAGS = 0;

}
bool DPN_TransmitProcessor::complete() {

    return true;
}

//void DPN_TransmitProcessor::inition(DPN_ClientContext &cc) {
////    INFO_LOG
//    clientContext = cc;
//    initionTime.fixTime();
//    iSendCascade.restart();
//    iReceiveCascade.restart();
//    clearFlags();
//    setFlag(PF__GENERATOR);
//    postInition();
//    iSendCascade.setTarget(this);
//    iReceiveCascade.setTarget(this);
//}
void DPN_TransmitProcessor::postInition() {}

const DPN_ExpandableBuffer &DPN_TransmitProcessor::product() const {
    return content.buffer();
}

bool DPN_TransmitProcessor::resendPredicat() {

//    INFO_LOG;
//    DL_INFO(1, "Default resend predicat...");
//    if( !checkFlag(PF__GENERATOR) ) {
//        DL_INFO(1, "SERVER DEFAULT RESEND: fail: [%d]", checkFlag(PF__FAIL));
//        return true;
//    }
//    auto tr = useTransaction();
//    if( tr == nullptr ) {
//        DL_BADPOINTER(1, "transaction");
//        return false;
//    }
//    if( tr->line() == nullptr ) {
//        DL_ERROR(1, "No line in transaction");
//        return false;
//    }
//    if( tr->line()->lastResult() == DPN_SUCCESS && tr->line()->isOver() ) {
////        setFlag(PF__DIALOG_OVER);
//        DL_INFO(1, "NO RESEND");
//        return false;
//    }
//    if( tr->line()->lastResult() == DPN_FAIL ) {
////        setFlag(PF__DIALOG_OVER);
//        DL_INFO(1, "RESEND WITH FAIL");
//    }
    return true;
}

void DPN_TransmitProcessor::makeCascades() {

    if( iGlobalSendLine.empty() ) {
        iGlobalSendLine << &DPN_TransmitProcessor::sendPrepare
                     << &DPN_TransmitProcessor::send
                     << &DPN_TransmitProcessor::sendSuffix
                        ;
    }

    if( iGlobalReceiveLine.empty() ) {
        iGlobalReceiveLine << &DPN_TransmitProcessor::receivePrepare
                        << &DPN_TransmitProcessor::receive
                        << &DPN_TransmitProcessor::receiveSuffix
                        << &DPN_TransmitProcessor::receiveResender
                           ;
    }
    iSendCascade.setLine(iGlobalSendLine);
    iReceiveCascade.setLine(iGlobalReceiveLine);

    iSendCascade.setTarget( this );
    iReceiveCascade.setTarget( this );
}
*/
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
//=============================================================================================== DPN_TransactionMaster
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
DPN_TransmitProcessor *DPN_TransactionSpace::registerTransaction(DPN_PacketType t, int key) {

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
DPN_TransmitProcessor *ProcessorMapWrapper::create(DPN_PacketType t) {
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
void ProcessorMapWrapper::fill(DPN_PacketType t, CREATE_PROCESSOR callback) {
    if(map.find(t) == map.end()) {
        map[t] = callback;
    }
}
ProcessorMapFiller::ProcessorMapFiller(DPN_PacketType t, CREATE_PROCESSOR callback, ProcessorMapWrapper *out) {

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
DPN_TransmitProcessor *DPN_ProcessorPool::getProcessor(DPN_PacketType t) {

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
void DPN_ProcessorPool::__insertType(DPN_PacketType t) {
    if( map.find(t) == map.end() ) map[t] = DPN_TransmitProcessorMicroPool();
}





DPN_TransmitProcessor::DPN_TransmitProcessor(Underlayer &underlayer) : DPN::Client::Underlayer( underlayer ) {
    pLine = nullptr;
    iPosition = DPN_HOST;

    iTransportFlags = 0;
    iInnerFlags = 0;
    iUserFlags = 0;

    iContent.setTotalSizeParsing( false );


}
DPN_TransmitProcessor::~DPN_TransmitProcessor() {

}
DPN_Result DPN_TransmitProcessor::action() {

    if( pLine == nullptr ) {
        DL_BADPOINTER(1, "No line");
        return DPN_FAIL;
    }
    DPN_Result r = DPN_SUCCESS;


    disableResendable();
//    DL_INFO(1, "do host action (step: [%d])", pLine->step());
    r = pLine->go( iPosition );
    switch (r) {
    case DPN_FAIL:
        DL_INFO(1, "Host step failed");
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
//        DL_INFO(1, "Host line continue: resend (step: [%d])", pLine->step());
        makeResendable();
    }



    iContent.parseBuffers();
//    DL_INFO(1, "action result: [%d] parsed data: [%d]", r, iContent.buffer().size());


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
//    DL_INFO(1, "do [%s] action (step: [%d])",  getCallerName(iPosition), pLine->step());
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
//    DL_INFO(1, "action result: [%d]", r);

    if( pLine->isOver() == false ) {
//        DL_INFO(1, "Server line continue: resend (step: [%d])", pLine->step());
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
}

const DPN_ExpandableBuffer &DPN_TransmitProcessor::product() const {
    return iContent.buffer();
}
void DPN_TransmitProcessor::bindClient(DPN_ClientUnderlayer &u) {
    wClientUnder = u;
}
void DPN_TransmitProcessor::bindModules(DPN_Modules m) {
    wModules = m;
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
