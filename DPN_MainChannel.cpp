#include "DPN_MainChannel.h"

using namespace DPN::Logs;

namespace DPN::Client {
    void MonoBase::fail() {
        DL_INFO(1, "mono channel [%p] failed. block mirror", this);
        wMirror.block();
        DPN::Client::Underlayer::disconnect();
    }
    __main_channel__::__main_channel__()
    {}
    __main_channel__::__main_channel__(Underlayer ul) : iBase(ul)
      ,qf(true, ul), qb(true, ul)
    { }

    bool __main_channel__::__init( DPN_NodeConnector *connector ) {
        DL_INFO(1, "try init base channel [%p]", this);

        if( connector == nullptr ) return false;

        wChannel.init( connector, "Main" );
        iBase.init( connector->isInitiator() );



        DL_INFO(1, "try get interfaces [%p]", this);
        DL_INFO(1, "try get forward mono interface... forward if: [%p]", &forwardInterface);
        forwardInterface = wChannel.getMonoIf( DPN::FORWARD );
        DL_INFO(1, "try get backward mono interface...");
        backInterface = wChannel.getMonoIf( DPN::BACKWARD );

//        DL_INFO(1, "got interface: bads: b [%d] f [%d]", backInterface.badInterface(), forwardInterface.badInterface());

        DL_INFO(1, "back io: [%p] forward io: [%p]", backInterface.io(), forwardInterface.io());

        if( forwardInterface.badInterface()  || backInterface.badInterface() ) {
            DL_ERROR(1, "bad mono interface; forward: [%d] back: [%d]",
                     forwardInterface.badInterface(),
                     backInterface.badInterface());
            return false;
        }

        qf.data()->setEntry( &iBase );
        qf.data()->connect( forwardInterface.io() );

        qb.data()->setEntry( backInterface.io() );
        qb.data()->connect( &iBase );

        qf.data()->setMirror( qb.data() );
        qb.data()->setMirror( qf.data() );


        return true;
    }
    bool __main_channel__::__send(DPN_TransmitProcessor *processor) {
        if( processor == nullptr ) {
            DL_BADPOINTER(1, "processor");
            return false;
        }
        DL_INFO(1, "__main_channel__ send: processor: [%p]", processor);
        iBase.send( processor );
        return true;
    }

    //-----------------------------------------------------------
    MainChannel::MainChannel() : dMainChannel(false)
    {}
    MainChannel::MainChannel(Underlayer ul) : dMainChannel(true, ul), DPN::Client::Underlayer( ul )
    {}
    bool MainChannel::badClient() const {
        return dMainChannel::isEmptyObject() || Underlayer::empty();
    }
    bool MainChannel::baseInit( DPN_NodeConnector *c ) {
        return dMainChannel::data()->__init( c );
    }
    bool MainChannel::baseSend(DPN_TransmitProcessor *p) {
        if( dMainChannel::isEmptyObject() ) {
            DL_ERROR(1, "Empty watcher");
            return false;
        }
        DL_INFO( 1, "MainChannel... data: [%p]", dMainChannel::data());
        return dMainChannel::data()->__send( p );
    }
    Thread::ThreadUnit MainChannel::forward() { return dMainChannel::data()->qf.unit(); }
    Thread::ThreadUnit MainChannel::backward() { return dMainChannel::data()->qb.unit(); }
    //-----------------------------------------------------------
}

