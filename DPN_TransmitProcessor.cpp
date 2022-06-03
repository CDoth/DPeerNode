#include "DPN_TransmitProcessor.h"

ProcessorMapWrapper PROCESSOR_CREATOR;
//====================================================================================== DPN_TransmitProcessor:
thread_local __action_line<DPN_TransmitProcessor> DPN_TransmitProcessor::iGlobalSendLine;
thread_local __action_line<DPN_TransmitProcessor> DPN_TransmitProcessor::iGlobalReceiveLine;


DPN_TransmitProcessor::DPN_TransmitProcessor(DPN_AbstractTransaction *transaction) {

    pTransaction = transaction;
    iFlags = 0;
    iResendFlag = false;

    setFlag(PF__GENERATOR);
    makeCascades();
}
void DPN_TransmitProcessor::setTransaction(DPN_AbstractTransaction *t) {
    pTransaction = t;
}
DPN_Result DPN_TransmitProcessor::sendCascade() {

    INFO_LOG
//    DPN_Result r;
//    if( (r = sendPrepare()) == DPN_SUCCESS ) {
//        if( (r = send()) == DPN_SUCCESS ) {
//            if( (r = sendSuffix()) == DPN_SUCCESS ) {
//                return DPN_SUCCESS;
//            }
//        }
//    }
    return iSendCascade.go();
}
DPN_Result DPN_TransmitProcessor::receiveCascade() {

    INFO_LOG
//    DPN_Result r;
//    if( (r = receivePrepare()) == DPN_SUCCESS ) {
//        if( (r = receive()) == DPN_SUCCESS ) {
//            if( (r = receiveSuffix()) == DPN_SUCCESS ) {
//                if( resendPredicat() ) {
//                    resend();
//                }
//                return DPN_SUCCESS;
//            }
//        }
//    }
//    return r;
    return iReceiveCascade.go();
}
//------------------------------------------------------------------
DPN_Result DPN_TransmitProcessor::sendPrepare() {

    INFO_LOG;
//    while(prepared == DPN_KEEP) {

//        if(prepared == DPN_SUCCESS) break;

//        bool isGen = checkFlag(PF__GENERATOR);
//        bool snakeMode = checkFlag(PF__SNAKEMODE);

//        DL_INFO(1, "snake mode: [%d] generator: [%d]", snakeMode, isGen);

//        if( !checkFlag(PF__GENERATOR) && checkFlag(PF__SNAKEMODE) ) return DPN_SUCCESS;

//        if( (prepared = sendPrefix()) == DPN_FAIL ) {
//            setFlag(PF__FAIL);
//            return failureProcessing();
//        }
//    }
//    return prepared;

    if( checkFlag(PF__FAIL) ) {
        DL_INFO(1, "sending in FAIL mode");
        return DPN_SUCCESS;
    }
    bool isGenerator = checkFlag(PF__GENERATOR);
    bool snakeMode = checkFlag(PF__SNAKEMODE);

    DL_INFO(1, "proc: [%p] is generator: [%d] snake: [%d] fail: [%d] flags: [%d]", this, isGenerator, snakeMode, checkFlag(PF__FAIL), iFlags);
    if( !isGenerator && !snakeMode ) return DPN_SUCCESS;

    DPN_Result r;
    r = sendPrefix();
    if( r == DPN_FAIL ) {
        setFlag(PF__FAIL);
        return failureProcessing();
    }
    return DPN_SUCCESS;
}
DPN_Result DPN_TransmitProcessor::send() {


    INFO_LOG


    UNIT_TRANSACTION = pTransaction ? pTransaction->index() : -1;
    UNIT_FLAGS = iFlags;
    content.reservePrefix(1);
    content.parseBuffers();
    content.setPrefix(me(), 0);


    DL_INFO(1, "transaction: [%p] flags: [%d] buffer: [%d]", pTransaction, iFlags, content.buffer().size());

    if( clientContext.connector() == nullptr ) {
        DL_BADPOINTER(1, "connector");
        return DPN_FAIL;
    }

    if( clientContext.connector()->x_send(content.buffer()) == DPN_FAIL ) {
        return DPN_FAIL;
    }


    content.clearBuffer();
    return DPN_SUCCESS;
}
DPN_Result DPN_TransmitProcessor::sendSuffix() {

    INFO_LOG
    return sendReaction();
    return DPN_SUCCESS;
}
//------------------------------------------------------------------
DPN_Result DPN_TransmitProcessor::receivePrepare() {

    INFO_LOG
    return receivePrefix();
}
DPN_Result DPN_TransmitProcessor::receive() {

    INFO_LOG

//    DL_INFO(1, "buffer: [%p]",
//             threadContext.buffer().data()
//            );


    if( content.deparseBuffer(threadContext.buffer()) == false ) {
        DL_FUNCFAIL(1, "deparseBuffer");
        return DPN_FAIL;
    }
    iFlags = UNIT_FLAGS.get();
    inverseFlag(PF__GENERATOR);
    int tri = UNIT_TRANSACTION.get();

    DL_INFO(1, "flags: [%d] {check generator: [%d] fail: [%d] snake: [%d] }transaction: [%d]",
            iFlags, checkFlag(PF__GENERATOR), checkFlag(PF__FAIL), checkFlag(PF__SNAKEMODE), tri);


    threadContext.buffer().clear();
    return DPN_SUCCESS;
}
DPN_Result DPN_TransmitProcessor::receiveSuffix() {

    INFO_LOG;
    if( checkFlag(PF__FAIL) ) {
        DL_INFO(1, "Receive packet in FAIL mode");
        failureProcessing();
        return DPN_FAIL;
    }

    bool isGenerator = checkFlag(PF__GENERATOR);
    bool snakeMode =  checkFlag(PF__SNAKEMODE);

    DL_INFO(1, "is generator: [%d] snake mode: [%d]", isGenerator, snakeMode);

    if( isGenerator && !snakeMode ) return DPN_SUCCESS;

    DPN_Result r = receiveReaction();
    if( r == DPN_FAIL ) {
        setFlag(PF__FAIL);
        return failureProcessing();
    }
    return r;
}
DPN_Result DPN_TransmitProcessor::receiveResender() {
    INFO_LOG;
    if( resendPredicat() ) resend();
    return DPN_SUCCESS;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void DPN_TransmitProcessor::repeat() {
    repeatProcessor();
    iResendFlag = false;
}
void DPN_TransmitProcessor::clear() {
    INFO_LOG
    clearProcessor();

//    prepared = DPN_KEEP;

    iSendCascade.restart();
    iReceiveCascade.restart();

    initionTime.clear();
    packet_buffer.clear();

    pTransaction = nullptr;
    iFlags = 0;
    iResendFlag = false;
    UNIT_TRANSACTION = -1;
    UNIT_FLAGS = 0;

}
bool DPN_TransmitProcessor::complete() {

    return true;
}
//void DPN_TransmitProcessor::setDialog(DPN_Dialog &d) {
//    currentDialog = d;
//}
void DPN_TransmitProcessor::inition(DPN_ClientContext &cc, DPN_ThreadContext &tc) {
    INFO_LOG
    clientContext = cc;
    threadContext = tc;
    initionTime.fixTime();
    iSendCascade.restart();
    iReceiveCascade.restart();
    clearFlags();
    setFlag(PF__GENERATOR);
    postInition();
    iSendCascade.setTarget(this);
    iReceiveCascade.setTarget(this);
}
void DPN_TransmitProcessor::postInition() {}
bool DPN_TransmitProcessor::send(DPN_TransmitProcessor *p) {

    p->inition(clientContext, threadContext);
    threadContext.send(p);
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
}
//====================================================================================== DPN_ProcessorList:
/*
void DPN_ProcessorList::returnAll(DPN_ThreadContext &tc) {
    while(__first != __head) {
        tc.returnProcessor(__first->proc);
        removeNode(__first);
        __first = __first->next;
    }
    zero();
}
*/
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
//========================================================================================================= DPN_ThreadContext
void DPN_ThreadContext::alloc() {
    data()->__back_list = new DPN_CrossThreadProcessorList;
    data()->__hash_tool = new DPN_SHA256;
    data()->__processor_pool = new DPN_ProcessorPool;
}
DPN_CrossThreadProcessorList *DPN_ThreadContext::backList() {
    return data()->__back_list;
}
bool DPN_ThreadContext::send(DPN_TransmitProcessor *proc) {
    if( data() == nullptr  ) {
        DL_BADPOINTER(1, "data");
        return false;
    }
    if( data()->__back_list == nullptr ) {
        DL_BADPOINTER(1, "back list");
        return false;
    }
    data()->__back_list->push_back(proc);
    return true;
}
bool DPN_ThreadContext::returnProcessor(DPN_TransmitProcessor *proc) {
    return data()->__processor_pool->returnProcessor(proc);
}
DPN_TransmitProcessor *DPN_ThreadContext::processor(DPN_PacketType t, DPN_AbstractTransaction *transaction) {
    return data()->__processor_pool->getProcessor(t);
}
//=============================================================================================== DPN_TransactionMaster
DPN_AbstractTransaction::DPN_AbstractTransaction() {
    iArchived = false;
    iIndex = -1;
    pCreator = nullptr;
    iTransmitCounter = 0;

    iTimeRange.fixBegin();
}
DPN_AbstractTransaction::~DPN_AbstractTransaction() {
    finish();
}
bool DPN_AbstractTransaction::finish() {

    if( pCreator ) return pCreator->finishTransaction(this);
    return false;
}
int DPN_AbstractTransaction::index() const {
    return iIndex;
}
void DPN_AbstractTransaction::use() {
    ++iTransmitCounter;
}
//=============================================================================================== DPN_TransactionMap
DPN_AbstractTransaction *DPN_TransactionMap::transactionA(int index) const {
    auto f = iMap.find(index);
    if( f == iMap.end() ) return nullptr;
    return f->second;
}
bool DPN_TransactionMap::isValidKey(int key) const {
    return iMap.find(key) == iMap.end();
}
DPN_AbstractTransaction *DPN_TransactionMap::regTr(DPN_AbstractTransaction *tr, int key) {

    if( tr == nullptr ) {
        DL_BADPOINTER(1, "tr");
        return nullptr;
    }
    if( iMap.find(key) != iMap.end() ) {
        DL_ERROR(1, "Transaction with key [%d] already registered", key);
        return nullptr;
    }
    tr->iIndex = key;
    tr->pCreator = this;

    iMap[key] = tr;
    return tr;
}
DPN_AbstractTransaction *DPN_TransactionMap::regTr(DPN_AbstractTransaction *tr) {
    if( tr == nullptr ) {
        DL_BADPOINTER(1, "tr");
        return nullptr;
    }
    tr->iIndex = iMap.size() + 10;
    tr->pCreator = this;
    iMap[tr->iIndex] = tr;

    return tr;
}
bool DPN_TransactionMap::unregTr(DPN_AbstractTransaction *t) {

    return true;
}
