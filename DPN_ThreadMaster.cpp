#include "DPN_ThreadMaster.h"
#include <thread>
using namespace DPeerNodeSpace;
DPN_ThreadMaster::DPN_ThreadMaster() {

}
bool DPN_ThreadMaster::stream() {

    if(core == nullptr) {
        DL_BADPOINTER(1, "core");
        return false;
    }

    bridge.start();
    int i = 0;
    while(bridge.playable()) {

        // -------------------------------------------------------

        bridge.procDirections();

        // -------------------------------------------------------

        proc_all();

    }
    while(bridge.inPause()) {}

    core->threadOver(bridge);

    return true;
}
bool DPN_ThreadMaster::stream2() {
    if(core == nullptr) {
        DL_BADPOINTER(1, "core");
        return false;
    }

    bridge.start();
    int i = 0;

    while(bridge.playable()) {
        bridge.procSingle();
    }

    while(bridge.inPause()) {}
    core->threadOver(bridge);
}
bool DPN_ThreadMaster::fileStream() {

}
void DPN_ThreadMaster::proc_all() {

    if(bridge.process_tasks() == false) return;
    core->processIncomingConnections();
    core->processOutgoingConnections();


}
bool dpn_thread_stream(DPN_ThreadMaster *master) {
    if(master) {
        master->stream();
        delete master;
        return true;
    }
    return false;
}
