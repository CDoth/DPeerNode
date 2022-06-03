#include "DPN_MediaSystemProcessors.h"



// DPN_KEEP - send processor again
// DPN_FAIL - call fault callback and send processor in error mode

// DPN_REPEAT - recall processor
// DPN_SUCCESS - processor is over
//===================================================================================== DPN_Processor__video_call_prepare
DPN_Result DPN_Processor__media_stream_prepare::sendPrefix() {


    // start dialog: send processor first time
    // receive dialog: receive processor first time

    // [cancel]                     send prefix: first step fault
    // [resend processor: fail]     receive reaction: first step fault

    // [keep sending]               send prefix: middle step done
    // [resend processor]           receive reaction: middle step done:

    // [keep sending: fail]         send prefix: middle step fault
    // [resend processor: fail]     receive reaction: middle step faul

    // [keep sending: fail]         send prefix: last step fault
    // [resend processor: fail]     receive reaction: last step fault

    // [keep sending] send prefix:  last step done
    // [finish] receive reaction:   last step done


    // sending:     [cancel] [keep sending]      [keep sending: fail]
    // receiving:   [finish] [resend processor]  [resend processor: fail]

    INFO_LOG;

    /*
    __transaction *tr = reinterpret_cast<__transaction*>( pTransaction );
    DL_INFO(1, "Transaction: [%p]", tr);
    if( tr == nullptr ) {
        if( (tr = clientContext.generateTransaction<__transaction>()) == nullptr ) {
            DL_ERROR(1, "Can't create transaction");
            return DPN_FAIL;
        }

        tr->actionLine() << &DPN_Processor__media_stream_prepare::hostPreparing
                         << &DPN_Processor__media_stream_prepare::hostCreating
                         << &DPN_Processor__media_stream_prepare::hostRun
                            ;
        tr->actionLine().setFailStep( &DPN_Processor__media_stream_prepare::hostFault );
        setTransaction(tr);
    }
    */


    DPN_Result r;
    __transaction *tr = useTransation<__transaction>();
    if( tr == nullptr ) {
        DL_ERROR(1, "Can't use transaction");
        return DPN_FAIL;
    }
    tr->actionLine().setTarget(this);
    return tr->actionLine().doStep();
}
DPN_Result DPN_Processor__media_stream_prepare::receiveReaction() {


/*
    int trI = UNIT_TRANSACTION.get();

    __transaction *tr = nullptr;
    if( trI < 0 ) {
        // Send <BAD_TRANSACTION>
        DL_BADVALUE(1, "Transaction index: [%d]", trI);
        return DPN_FAIL;
    }
    if( (tr = clientContext.transaction<__transaction>(trI)) == nullptr ) {
        if( (tr = clientContext.registerTransaction<__transaction>(trI)) == nullptr ) {
            // Send <BAD_TRANSACTION>
            DL_ERROR(1, "Can't register new transaction with key: [%d]", trI);
            return DPN_FAIL;
        }
        tr->actionLine() << &DPN_Processor__media_stream_prepare::serverPreparing
                         << &DPN_Processor__media_stream_prepare::serverCreating
                         << &DPN_Processor__media_stream_prepare::serverRun
                            ;
        tr->actionLine().setFailStep( &DPN_Processor__media_stream_prepare::serverFault );

        setTransaction(tr);
        DL_INFO(1, "Set transaction: [%p] [%d]", tr, tr->index());
    } else {
        bool state = UNIT_STATE.get();
        if( state == false ) {
            std::string d = UNIT_FAULT_DETAILS.get();
            DL_INFO(1, "Remote node fault [%s]", d.c_str());
            tr->actionLine().doFailStep();
            return DPN_FAIL;
        }
    }
    */

    // fail behaivor:
    // 1. server got error in receiveReaction  (action line step return DPN_FAIL)
    // 2. server call serverFault to clear work ( see step 4 )
    // 3. server resend itself with fault flag (resendPredicat set fail mode and return true)
    // 4. server ignore action steps in sendPrefix (failureSwitcher call clear action (hosFault/serverFault) and return true to ignore sendPrefix)
    // 5. host receive packet with fault flag
    // 6. host ingore action steps in receiveReaction (failureSwitcher call clear action and return true to ingore receiveReaction)
    // 7. host call hostFault to clear work ( see step 6 )
    // 8. server block resendPredicat to stop dialog (doFailStep set lastResult to DPN_SUCCESS and complete line, so resendPredicat will return true)

    INFO_LOG;

    __transaction *tr = useTransation<__transaction>();
    if( tr == nullptr ) {
        DL_ERROR(1, "Can't use transaction");
        return DPN_FAIL;
    }

    tr->actionLine().setTarget(this);
    return tr->actionLine().doStep();

}
DPN_Result DPN_Processor__media_stream_prepare::failureProcessing() {

    auto tr = useTransation<__transaction>();
    if( tr == nullptr ) {
        DL_BADPOINTER(1, "transaction");
        return DPN_FAIL;
    }
    DL_INFO(1, "DO FAIL STEP");
    tr->actionLine().doFailStep();
    if( checkFlag(PF__GENERATOR) && tr->actionLine().step() == 0) return DPN_FAIL;
    return DPN_SUCCESS;
}
bool DPN_Processor__media_stream_prepare::makeHostLine() {

    INFO_LOG;

    auto tr = useTransation<__transaction>();
    if( tr == nullptr ) {
        DL_BADPOINTER(1, "transaction");
        return false;
    }
    tr->actionLine() << &DPN_Processor__media_stream_prepare::hostPreparing
                     << &DPN_Processor__media_stream_prepare::hostCreating
                     << &DPN_Processor__media_stream_prepare::hostRun
                        ;
    tr->actionLine().setFailStep( &DPN_Processor__media_stream_prepare::hostFault );
    return true;
}
bool DPN_Processor__media_stream_prepare::makeServerLine() {

    INFO_LOG;

    auto tr = useTransation<__transaction>();
    if( tr == nullptr ) {
        DL_BADPOINTER(1, "transaction");
        return false;
    }
    tr->actionLine() << &DPN_Processor__media_stream_prepare::serverPreparing
                     << &DPN_Processor__media_stream_prepare::serverCreating
                     << &DPN_Processor__media_stream_prepare::serverRun
                        ;
    tr->actionLine().setFailStep( &DPN_Processor__media_stream_prepare::serverFault );
    return true;
}
bool DPN_Processor__media_stream_prepare::resendPredicat() {
    INFO_LOG;
    auto tr = useTransation<__transaction>();
    if( tr == nullptr ) {
        DL_BADPOINTER(1, "transaction");
        return false;
    }
    if( tr->actionLine().lastResult() == DPN_SUCCESS && tr->actionLine().isOver() ) {
        DL_INFO(1, "NO RESEND");
        return false;
    }
    if( tr->actionLine().lastResult() == DPN_FAIL ) {
        DL_INFO(1, "RESEND WITH FAIL");
    }
    return true;
}
DPN_Result DPN_Processor__media_stream_prepare::hostPreparing() {
    INFO_LOG;
//    return __prepare_stream();
    return DPN_SUCCESS;
}
DPN_Result DPN_Processor__media_stream_prepare::serverPreparing() {
    INFO_LOG;
//    return __prepare_stream();
    return DPN_SUCCESS;
}
DPN_Result DPN_Processor__media_stream_prepare::serverCreating() {
    INFO_LOG;
//    return __create_stream(DPN_BACKWARD);
    return DPN_SUCCESS;
}
DPN_Result DPN_Processor__media_stream_prepare::hostCreating() {
    INFO_LOG;
//    return __create_stream(DPN_FORWARD);
    return DPN_SUCCESS;
}
DPN_Result DPN_Processor__media_stream_prepare::hostRun() {
    INFO_LOG;
//    return __run_stream(DPN_FORWARD);

    DL_INFO(1, ">>>>> HOST RUN");
    return DPN_FAIL;
}
DPN_Result DPN_Processor__media_stream_prepare::serverRun() {
    INFO_LOG;
//    return __run_stream(DPN_BACKWARD);
    DL_INFO(1, ">>>>> SERVER RUN");
    return DPN_SUCCESS;
}
DPN_Result DPN_Processor__media_stream_prepare::hostFault() {

    INFO_LOG;

    if( pTransaction ) pTransaction->finish();
    // cancel media actions...
    return DPN_SUCCESS;
}
DPN_Result DPN_Processor__media_stream_prepare::serverFault() {

    INFO_LOG;

    if( pTransaction ) pTransaction->finish();
    // cancel media actions...
    return DPN_SUCCESS;
}
DPN_Result DPN_Processor__media_stream_prepare::__prepare_stream() {


    INFO_LOG;

    if( pMediaSystem == nullptr ) {
        DL_BADPOINTER(1, "Media System");
        return DPN_FAIL;
    }
    DPN_MediaType type = UNIT_TYPE.get();
    DPN_MediaType backType = UNIT_BACK_TYPE.get();
    DPN_MediaType backStrictMask = UNIT_BACK_STRICT_MASK.get();
    if( type == DPN_MS__NOTYPE ) {
        DL_ERROR(1, "No stream type");
        return DPN_FAIL;
    }
    if( backType != DPN_MS__NOTYPE && ((backType & backStrictMask) == 0) ) {
        DL_ERROR(1, "Bad strict mask. Back type: [%d] mask: [%d]", backType, backStrictMask);
        return DPN_FAIL;
    }
    bool outputAvailable = pMediaSystem->isAvailable(clientContext.client(), type, DPN_FORWARD);
    bool inputAvailable = pMediaSystem->isAvailable(clientContext.client(), backType, DPN_BACKWARD);

//    if( outputAvailable == false ) {
//        bool maskAvailable = pMediaSystem->isAvailable(clientContext.client(), backStrictMask, DPN_FORWARD);
//        if( maskAvailable ) {
//            outputAvailable = true;
//            backType = backStrictMask;
//        }
//    }
    if( outputAvailable == false || inputAvailable == false ) {
        DL_WARNING(1, "Media channels is busy");
        return DPN_FAIL;
    }

    __transaction *tr = reinterpret_cast<__transaction*>( pTransaction );
    tr->iType = type;

    return DPN_SUCCESS;
}
DPN_Result DPN_Processor__media_stream_prepare::__create_stream(DPN_DirectionType d) {

    INFO_LOG;

    if( pMediaSystem == nullptr ) {
        DL_BADPOINTER(1, "Media System");
        return DPN_FAIL;
    }

    DPN_MediaType type;
    if( pTransaction == nullptr ) {
        DL_BADPOINTER(1, "transaction");
        return DPN_FAIL;
    }
    __transaction *tr = reinterpret_cast<__transaction*>( pTransaction );
    type = tr->type();

    if( pMediaSystem->createStream(clientContext.client(), type, d, &tr->contextSet()) == false ) {
        DL_FUNCFAIL(1, "createStream");
        return DPN_FAIL;
    }

    return DPN_SUCCESS;
}
DPN_Result DPN_Processor__media_stream_prepare::__run_stream(DPN_DirectionType d) {

    INFO_LOG;

    if( pMediaSystem == nullptr ) {
        DL_BADPOINTER(1, "Media System");
        return DPN_FAIL;
    }
    DPN_MediaType type;
    int udpPort;
    if( pTransaction == nullptr ) {
        DL_BADPOINTER(1, "transaction");
        return DPN_FAIL;
    }
    __transaction *tr = reinterpret_cast<__transaction*>( pTransaction );
    type = tr->type();
    if( (udpPort = pMediaSystem->streamPort(type, d)) < 0 ) {
        DL_FUNCFAIL(1, "streamPort");
        return DPN_FAIL;
    }

    if( pMediaSystem->runStream(clientContext.client(), type, d, udpPort) == false ) {
        DL_FUNCFAIL(1, "runStream");
        return DPN_FAIL;
    }

    return DPN_SUCCESS;
}

DPN_MediaProcessor::DPN_MediaProcessor() {
    pMediaSystem = nullptr;
}
void DPN_MediaProcessor::postInition() {
    pMediaSystem = extractMediaModule(*clientContext.modules());
}
