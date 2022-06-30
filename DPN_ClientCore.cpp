#include "DPN_ClientCore.h"

using namespace DPN::Logs;
//==========================================================================
namespace DPN::Client {
Core::Core() {

}
Core::Core(DPN::Thread::ThreadUser &threadUser, DPN::Modules &modules) :
        DPN::Client::MainChannel( threadUser, modules )
    {}
    void Core::init(DPN_NodeConnector *c) {
        if( baseInit( c ) ) {

            DPN::Thread::ThreadUser::putUnit( forward() );
            DPN::Thread::ThreadUser::putUnit( backward() );
        }
    }
    bool Core::send(DPN_TransmitProcessor *p) {
        return baseSend( p );
    }

    const DArray<__channel> &Core::shadows() const
    {

    }

}

