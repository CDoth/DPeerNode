#include "DPN_Core.h"
using namespace DPN::Logs;


__dpn_core__::__dpn_core__()
    :
    DPN::Network::ClientCenter(true)
{
    dpnAddUDPPorts( 44440, 20 );

    DPN::Network::ModuleBinder *m = boundModules;
    DPN_AbstractModule *module = nullptr;

    DL_INFO(1, "Try create modules...");
    while( m->creator ) {

        if( (module = m->creator( *this ) ) ) {
            modules().addModule( module, m->name );
        } else {
            DL_ERROR(1, "Can't create module [%s]", m->name.c_str());
        }
        ++m;
    }
}
void __dpn_core__::setName(const std::string &name) {
    DPN::Network::ClientCenter::setName( name );
}
void __dpn_core__::createThread(DPN::Thread::Policy p) {
    DPN::Network::ClientCenter::threadUser().startStream( p );
}
bool __dpn_core__::sharePort(int port, bool autoaccept) {
//    return iWp.sharePort( port, autoaccept );
}
bool __dpn_core__::connectTo(const char *address, int port) {
//    return iWp.connectTo( address, port );
}
void __dpn_core__::acceptAll() {
//    iWp.acceptAll();
}
DArray<DPN::Client::Interface> __dpn_core__::clients() const {
    return DPN::Network::ClientCenter::clients();
}
