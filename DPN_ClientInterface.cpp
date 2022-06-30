#include "DPN_ClientInterface.h"


using namespace DPN::Logs;

namespace DPN::Client {

    Interface::Interface() {}
    Interface::Interface(Thread::ThreadUser &threadUser, Modules &modules, DPN_NodeConnector *c)  :
        DPN::Client::Core( threadUser, modules )
    {}
    bool Interface::init(DPN_NodeConnector *c) { return baseInit( c ); }
    DArray<__channel> Interface::channels() const { return shadows(); }
    void Interface::sendMessage(const char *m) {
        auto proc = DPN_PROCS::processor<DPN_PACKETTYPE__TEXT_MESSAGE>();
        proc->setMessage(m);
        send(proc);
    }
    void Interface::makeShadowConnection(DXT::Type t) {
        auto proc = DPN_PROCS::processor<DPN_PACKETTYPE__MAKE_SHADOW_CONNECTION>();
        proc->setType( t );
        send(proc);
    }
    bool Interface::send(DPN_TransmitProcessor *p) { return DPN::Client::Core::send( p ); }
}



