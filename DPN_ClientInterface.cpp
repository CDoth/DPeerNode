#include "DPN_ClientInterface.h"


using namespace DPN::Logs;

namespace DPN::Client {

    Interface::Interface() {}

    Interface::Interface( DPN::Client::GlobalUnderlayerSpace &gus, Thread::ThreadUser &threadUser, Modules modules, DPN_NodeConnector *c)  :
        DPN::Client::Core( DPN::Client::Underlayer( gus, threadUser, modules ) )
    { coreInit( c );

    }

    DArray<__channel> Interface::channels() const { return shadows(); }

    void Interface::sendMessage(const char *m) {
        auto proc = DPN_PROCS::processor<PT__TEXT_MESSAGE>();
        proc->setMessage(m);

        DL_INFO( 1, "message: [%s]", m );
        send(proc);
    }
    void Interface::makeShadowConnection(DXT::Type t) {
        auto proc = DPN_PROCS::processor<PT__MAKE_SHADOW_CONNECTION>();
        proc->setType( t );
        send(proc);
    }
    bool Interface::send(DPN_TransmitProcessor *p) { return DPN::Client::Core::send( p ); }
}



