#include "DPN_ClientCore.h"

using namespace DPN::Logs;
//==========================================================================
namespace DPN::Client {

    Core::Core()
    {}
    Core::Core(Underlayer ul) : DPN::Client::MainChannel( ul ) {

    }

    void Core::coreInit(DPN_NodeConnector *c) {

        if( baseInit( c ) ) {

            DL_INFO(1, "Put main channel units: ");
            DPN::Thread::ThreadUser::putUnit( forward() );
            DPN::Thread::ThreadUser::putUnit( backward() );
        }
    }
    bool Core::send(DPN_TransmitProcessor *p) {
        DL_INFO( 1, "Core send" );
        return baseSend( p );
    }
}

