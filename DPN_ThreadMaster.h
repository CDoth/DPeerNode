#ifndef DPN_THREADMASTER_H
#define DPN_THREADMASTER_H


#include "DPN_ConnectionsCore.h"



class DPN_ThreadMaster {
public:
    friend bool dpn_thread_stream(DPN_ThreadMaster *master);
    friend class DPN_Core;
    DPN_ThreadMaster();
private:


    DPN_ConnectionsCore *core;
    DPN_ThreadBridge bridge;

private:
    bool stream();
    bool stream2();
    bool fileStream();
private:
//    void proc_disconnected();
    void proc_all();
    void do_tasks();
    void proc_incoming();
    void proc_outgoing();
};
bool dpn_thread_stream(DPN_ThreadMaster *master);
#endif // DPN_THREADMASTER_H
