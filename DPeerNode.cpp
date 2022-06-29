#include "DPeerNode.h"

#include <DLexeme.h>
#include <thread>



using namespace DPeerNodeSpace;
/*
DPN_Core::DPN_Core() {

}
void DPN_Core::setName(const std::string &name) {
    connections_core.setName(name);
}
const std::string &DPN_Core::name() const {
    return connections_core.getName();
}
bool DPN_Core::share(int port, bool autoaccept) {

    return connections_core.share_port(port, autoaccept);

    return true;
}
bool DPN_Core::connect(const char *address, int port) {

    return connections_core.connect_to(address, port);

    return true;
}
bool DPN_Core::sendMessage(const char *message) {


    if(
    connections_core.remotes.empty()
            ) {
        DL_ERROR(1, "No clients");
        return false;
    }


    return connections_core.send_message(message);
}
bool DPN_Core::videoCall() {

//    if( connections_core.remotes.empty() ) {
//        DL_ERROR(1, "No clients");
//        return false;
//    }
//    auto m = extractMediaModule(connections_core.iGlobalModules);
//    DPN_ClientInterface ci = connections_core.remotes[0];
//    auto &channels = ci.channels();
//    if( channels.empty() ) {
//        DL_ERROR(1, "No channels");
//        return false;
//    }
//    __channel c = channels[0];
//    __channel_mono_interface mono = c.getIf( DPN_FORWARD );
//    if( mono.badInterface() ) {
//        DL_ERROR(1, "Bad mono interface");
//        return false;
//    }

//    m->startStream1( mono, DPN_MS__VIDEO );
    return true;
}
//DPN_MediaSystem &DPN_Core::media() {

//    auto m = extractMediaModule(connections_core.iGlobalModules);
//    if( m ) return *m;

//    DL_BADPOINTER(1, "Media module");
//}
void DPN_Core::acceptAll() {
    connections_core.acceptAll();
}
bool DPN_Core::sync() {
    return connections_core.sync();

    return true;
}
bool DPN_Core::requestRemoteFile(int key) {

    if(
    connections_core.remotes.empty()
            ) {
        DL_ERROR(1, "No clients");
        return false;
    }


}
bool DPN_Core::trcm(const char *command) {

    // 1
    if(command == nullptr) {
        DL_BADPOINTER(1, "command");
        return false;
    }
    DLexeme LX_COMMAND;

    DLexeme::lexeme base;
    DLexeme::lexeme path;

    static DLexeme::raw special_base = " :";
    static DLexeme::raw rule_path = "</ | \\> <$min:2>";
    static DLexeme::raw rule_message = "<$min: 1><$spec:\n>";
    static DLexeme::raw rule_address = "<.:3><$min: 7><$max: 15><$permsym: dig, .><$spec: \n>";

    base = LX_COMMAND.base(command, nullptr, special_base);
    if(base.begin == nullptr) {
        DL_ERROR(1, "No lexeme units");
        return false;
    }

    DArray<int> list;
    DLexeme::read_context rc;
    rc.set_min(0);
    rc.setUnique(true);

    list = LX_COMMAND.numbers(base.end, nullptr, &rc);
    // 122

#define LINE_IS(line) (LX_COMMAND.base_equal(base, line))
#define LINE_END break
    do {

        if(LINE_IS("info")) {

            auto sharedPorts = connections_core.sharedPorts();
            auto clients = connections_core.clients();
//            auto dc = connections_core.disconnectedClients();
//            auto incomingConnections = connections_core.incomingConnections();
//            auto outgoingConnections = connections_core.outgoingConnections();
//            auto threads = connections_core.threads();
            auto tasks = connections_core.tasks();

            DL_INFO(1, "INFO: name: [%s]", name().c_str());
            DL_INFO(1, "================================================");

            //---------------------------------------------------------------------------------------------------------------
            DL_INFO(1, ">>> shared ports: [%d]", sharedPorts.size());
            FOR_VALUE(sharedPorts.size(), i) {
                DL_INFO(1, "%d: port: [%d] aa: [%d]", i, sharedPorts[i].port(), sharedPorts[i].isAuto());
            }
            //---------------------------------------------------------------------------------------------------------------
            DL_INFO(1, ">>> clients: [%d]", clients.size());
            FOR_VALUE(clients.size(), i) {
//                DPN_Client &c = clients[i];
//                DL_INFO(1, "%d client [%s] local: [%s] remote: [%s] state: [%d]", i,
//                        c.name().c_str(), c.local().name().c_str(),
//                        c.remote().name().c_str(), c.state()
//                        );
            }
            //---------------------------------------------------------------------------------------------------------------
//            DL_INFO(1, ">>> disconnected clients: [%d]", dc.size());
//            FOR_VALUE( dc.size(), i ) {
//                DPN_Client &c = dc[i];
//                DL_INFO(1, "%d disclient [%s] local: [%s] remote: [%s] state: [%d]", i,
//                        c.name().c_str(),
//                        c.local().name().c_str(),
//                        c.remote().name().c_str(),
//                        c.state()
//                        );
//            }
            //---------------------------------------------------------------------------------------------------------------
//            DL_INFO(1, ">>> incomingConnections: [%d]", incomingConnections.size());
//            FOR_VALUE(incomingConnections.size(), i) {
//                auto c = incomingConnections[i]->connector();
//                DL_INFO(1, "%d: inc conn: name: [%s] local: [%s] peer: [%s]", i,
//                        incomingConnections[i]->name().c_str(),
//                        c->localName().c_str(),
//                        c->peerName().c_str()
//                        );

//            }
            //---------------------------------------------------------------------------------------------------------------
//            DL_INFO(1, ">>> outgoingConnections: [%d]", outgoingConnections.size());
//            FOR_VALUE(outgoingConnections.size(), i) {
//                auto c = outgoingConnections[i]->connector();
//                DL_INFO(1, "%d: out conn: name: [%s] local: [%s] peer: [%s]", i,
//                        outgoingConnections[i]->name().c_str(),
//                        c->localName().c_str(),
//                        c->peerName().c_str()
//                        );
//            }
            //---------------------------------------------------------------------------------------------------------------
//            DL_INFO(1, ">>> threads: [%d]", threads.size());
//            FOR_VALUE(threads.size(), i) {
//                DL_INFO(1, "%d: thread: id: [%d] directions: [%d]", i, threads[i].threadId(), threads[i].directionsAccepted());
//            }
            //---------------------------------------------------------------------------------------------------------------
            DL_INFO(1, ">>> tasks: [%p]", tasks.getActual());

            //---------------------------------------------------------------------------------------------------------------
            DL_INFO(1, "================================================");
        }
        if(LINE_IS("a")) {
            connections_core.acceptAll();
        }
        if(LINE_IS("reject")) {
            connections_core.rejectAll();
        }
        if(LINE_IS("d")) {
            connections_core.disconnectAll();
        }
        if(LINE_IS("connect")) {
            connect("127.0.0.1", 58148);
        }
        if(LINE_IS("ping")) {
            connections_core.ping();
        }
        if(LINE_IS("env")) {

            FOR_VALUE(connections_core.clients().size(), i) {
                auto c = connections_core.clients()[i];
//                if( c.state() == CONNECTED ) {
//                    c.requestLocalEnv();
//                    return true;
//                }
            }
            DL_INFO(1, "No clients");

        }


        if( LINE_IS("vs") ) {
            videoCall();
            break;
        }
        if( LINE_IS("avs") ) {
//            auto m = extractMediaModule( connections_core.iGlobalModules );
//            if( m == nullptr ) {
//                DL_BADPOINTER(1, "media module");
//                break;
//            }
//            if( m->acceptIncoming() == false ) {
//                DL_FUNCFAIL(1, "acceptIncmoming");
//            }
            break;
        }

        if(LINE_IS("renew")) {
            connections_core.renewCatalog();
        }
        if(LINE_IS("sh")) {

            if(connections_core.remotes.empty()) {
                DL_ERROR(1, "No clients");
                break;
            }
//            DPN_Client &c = connections_core.client(0);
//            c.shadow(DXT::TCP);
            break;
        }
        if(LINE_IS("shu")) {

            if(connections_core.remotes.empty()) {
                DL_ERROR(1, "No clients");
                break;
            }
            DPN_ClientInterface c = connections_core.clients()[0];
            c.makeShadowConnection( DXT::UDP );
            break;
        }
        if(LINE_IS("fch")) {

            if(connections_core.remotes.empty()) {
                DL_ERROR(1, "No clients");
                break;
            }
            DPN_ClientInterface c = connections_core.clients()[0];

            auto proc = DPN_PROCS::processor<DPN_PACKETTYPE__RESERVE_CHANNEL>();
            proc->setChannel( c.channels()[0] );
            proc->setBackwardUser( extractFileModule( connections_core.iGlobalModules ) );

            c.send( proc );

            break;
        }
        if(LINE_IS("m")) {

            auto start = LX_COMMAND.sym_unspecial(base.end, nullptr, ": ");
            const char *message = LX_COMMAND.find(start.begin, nullptr, rule_message).begin;

            if( sendMessage(message) == false ) {
                DL_FUNCFAIL(1, "sendMessage");
                return false;
            }

            LINE_END;
        }
        if(LINE_IS("sync")) {

            sync();

            LINE_END;
        }
        if(LINE_IS("catalog")) {


            if( catalog() == nullptr ) {
                DL_BADPOINTER(1, "Catalog");
                break;
            }
            if(catalog()->empty()) {
                std::cout << "Empty catalog" << std::endl;
            } else {
                DL_INFO(1, "Catalog: name: [%s] path: [%s] vpath: [%s]",
                        catalog()->name().c_str(), catalog()->path().c_str(), catalog()->vpath().c_str());

                std::cout << catalog()->parseToText(true);
            }
            LINE_END;


        }
        if(LINE_IS("remote")) {

            if(connections_core.remotes.empty()) {
                DL_ERROR(1, "No clients");
                return false;
            }
            auto client = connections_core.remotes[0];
            auto fs = extractFileModule(connections_core.iGlobalModules);
            DL_INFO(1, "client tag: [%p] file system: [%p]", client.tag(), fs );

            auto i = fs->getIf( client );

            if( i.badInterface() ) {
                DL_ERROR(1, "Bad interface [remote cmd]");
                break;
            }
            DL_INFO(1, "interface tag: [%p]", i.key());

            if( i.remote() == nullptr ) {
                DL_BADPOINTER(1, "remote catalog");
                break;
            }
            if( i.remote()->empty() ) {
                std::cout << "Empty remote catalog" << std::endl;
            } else {
                std::cout << i.remote()->parseToText(true) << std::endl;
            }

            LINE_END;
        }
        if(LINE_IS("f")) {

//            const char *testFile = "F:/DFS_SPACE/UPLOAD/Jolie.jpg";
//            DPN_FileClient fc(connections_core.client(0));
//            fc.sendFastFile(testFile);
        }
        if(LINE_IS("fr")) {

            DArray<int> keyset;

            if(list.size() > 1) {

                std::cout << "list:" << std::endl;
                FOR_VALUE(list.size(), i) {
                    std::cout << list[i] << " ";
                }
                std::cout << std::endl;
                keyset = list;

            } else if(list.size() == 1) {
                std::cout << "single key: " << list[0]
                          << std::endl;
                keyset.append(list[0]);
            }

            auto fs = extractFileModule(connections_core.iGlobalModules);
            auto &client = connections_core.remotes[0];
            auto i = fs->getIf( client );

            i.requestFileset( keyset );
            break;
        }


        if(LINE_IS("at")) {
            addThread();
            LINE_END;
        }
    } while(0);

    return true;
}
bool DPN_Core::start() {
    addThread();
    return true;
}
void DPN_Core::addThread() {

    DPN_ThreadMaster *tm = new DPN_ThreadMaster;
    tm->core = &connections_core;
    connections_core.__add_thread(tm->bridge);

    std::thread t(dpn_thread_stream, tm);
    t.detach();
}
DPN_Catalog *DPN_Core::catalog() {
    DPN_FileSystem *fs = extractFileModule( connections_core.iGlobalModules );
    if( fs == nullptr ) {
        DL_BADPOINTER(1, "file module");
        return nullptr;
    }
    return &fs->host();
}
*/



DPeerNode::DPeerNode() : DPN_CORE(true) {}
void DPeerNode::start(DPN::Thread::Policy p) {
    data()->createThread( p );
}


bool DPeerNode::trcm(const char *command) {

    // 1
    if(command == nullptr) {
        DL_BADPOINTER(1, "command");
        return false;
    }
    DLexeme LX_COMMAND;

    DLexeme::lexeme base;
    DLexeme::lexeme path;

    static DLexeme::raw special_base = " :";
    static DLexeme::raw rule_path = "</ | \\> <$min:2>";
    static DLexeme::raw rule_message = "<$min: 1><$spec:\n>";
    static DLexeme::raw rule_address = "<.:3><$min: 7><$max: 15><$permsym: dig, .><$spec: \n>";

    base = LX_COMMAND.base(command, nullptr, special_base);
    if(base.begin == nullptr) {
        DL_ERROR(1, "No lexeme units");
        return false;
    }

    DArray<int> list;
    DLexeme::read_context rc;
    rc.set_min(0);
    rc.setUnique(true);

    list = LX_COMMAND.numbers(base.end, nullptr, &rc);

#define LINE_IS(line) (LX_COMMAND.base_equal(base, line))
#define LINE_END break
    do {

        if(LINE_IS("info")) {

        }
        if(LINE_IS("a")) {
//            connections_core.acceptAll();
        }
        if(LINE_IS("reject")) {
        }
        if(LINE_IS("d")) {
        }
        if(LINE_IS("connect")) {
        }
        if(LINE_IS("ping")) {
        }
        if(LINE_IS("env")) {


        }
        if( LINE_IS("vs") ) {

        }
        if(LINE_IS("renew")) {
        }
        if(LINE_IS("sh")) {
        }
        if(LINE_IS("shu")) {

//            if(connections_core.remotes.empty()) {
//                DL_ERROR(1, "No clients");
//                break;
//            }
//            DPN_ClientInterface c = connections_core.clients()[0];
//            c.makeShadowConnection( DXT::UDP );
//            break;
        }
        if(LINE_IS("fch")) {

//            if(connections_core.remotes.empty()) {
//                DL_ERROR(1, "No clients");
//                break;
//            }
//            DPN_ClientInterface c = connections_core.clients()[0];

//            auto proc = DPN_PROCS::processor<DPN_PACKETTYPE__RESERVE_CHANNEL>();
//            proc->setChannel( c.channels()[0] );
//            proc->setBackwardUser( extractFileModule( connections_core.iGlobalModules ) );

//            c.send( proc );

            break;
        }
        if(LINE_IS("m")) {

//            auto start = LX_COMMAND.sym_unspecial(base.end, nullptr, ": ");
//            const char *message = LX_COMMAND.find(start.begin, nullptr, rule_message).begin;

//            if( sendMessage(message) == false ) {
//                DL_FUNCFAIL(1, "sendMessage");
//                return false;
//            }

            LINE_END;
        }
        if(LINE_IS("sync")) {

//            sync();

            LINE_END;
        }
        if(LINE_IS("catalog")) {


//            if( catalog() == nullptr ) {
//                DL_BADPOINTER(1, "Catalog");
//                break;
//            }
//            if(catalog()->empty()) {
//                std::cout << "Empty catalog" << std::endl;
//            } else {
//                DL_INFO(1, "Catalog: name: [%s] path: [%s] vpath: [%s]",
//                        catalog()->name().c_str(), catalog()->path().c_str(), catalog()->vpath().c_str());

//                std::cout << catalog()->parseToText(true);
//            }
//            LINE_END;


        }
        if(LINE_IS("remote")) {

//            if(connections_core.remotes.empty()) {
//                DL_ERROR(1, "No clients");
//                return false;
//            }
//            auto client = connections_core.remotes[0];
//            auto fs = extractFileModule(connections_core.iGlobalModules);
//            DL_INFO(1, "client tag: [%p] file system: [%p]", client.tag(), fs );

//            auto i = fs->getIf( client );

//            if( i.badInterface() ) {
//                DL_ERROR(1, "Bad interface [remote cmd]");
//                break;
//            }
//            DL_INFO(1, "interface tag: [%p]", i.key());

//            if( i.remote() == nullptr ) {
//                DL_BADPOINTER(1, "remote catalog");
//                break;
//            }
//            if( i.remote()->empty() ) {
//                std::cout << "Empty remote catalog" << std::endl;
//            } else {
//                std::cout << i.remote()->parseToText(true) << std::endl;
//            }

//            LINE_END;
        }
        if(LINE_IS("f")) {

//            const char *testFile = "F:/DFS_SPACE/UPLOAD/Jolie.jpg";
//            DPN_FileClient fc(connections_core.client(0));
//            fc.sendFastFile(testFile);
        }
        if(LINE_IS("fr")) {

//            DArray<int> keyset;

//            if(list.size() > 1) {

//                std::cout << "list:" << std::endl;
//                FOR_VALUE(list.size(), i) {
//                    std::cout << list[i] << " ";
//                }
//                std::cout << std::endl;
//                keyset = list;

//            } else if(list.size() == 1) {
//                std::cout << "single key: " << list[0]
//                          << std::endl;
//                keyset.append(list[0]);
//            }

//            auto fs = extractFileModule(connections_core.iGlobalModules);
//            auto &client = connections_core.remotes[0];
//            auto i = fs->getIf( client );

//            i.requestFileset( keyset );
//            break;
        }

    } while(0);

    return true;
}
