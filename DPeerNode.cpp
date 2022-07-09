#include "DPeerNode.h"

#include <DLexeme.h>
#include <thread>



using namespace DPN::Logs;




DPeerNode::DPeerNode() : DPN_CORE(true) {}

void DPeerNode::setName(const std::string &name) {
    data()->setName( name );
}
void DPeerNode::start(DPN::Thread::Policy p) {
    data()->createThread( p );
}
DPN_FileSystem *DPeerNode::fileSystem() {
    return extractFileModule( data()->modules() );
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

        if(LINE_IS("m")) {

            auto start = LX_COMMAND.sym_unspecial(base.end, nullptr, ": ");
            const char *message = LX_COMMAND.find(start.begin, nullptr, rule_message).begin;

            auto clients = data()->clients();
            if( clients.empty() ) {
                DL_INFO(1, "No clients");
                break;
            }
            DL_INFO( 1, "Try send message [%s]", message );
            clients[0].sendMessage(message);


            LINE_END;
        }
        if(LINE_IS("catalog")) {

            auto fm = extractFileModule( data()->modules() );
            if( fm == nullptr ) {
                DL_BADPOINTER(1, "File Module");
                break;
            }
            if( fm->host().empty() ) {
                std::cout << "Empty catalog" << std::endl;
            } else {
                std::cout << fm->host().parseToText(true);
            }
            break;

        }
        if(LINE_IS("sync")) {

            auto clients = data()->clients();
            if( clients.empty() ) {
                DL_INFO(1, "No clients");
                break;
            }
            auto fm = extractFileModule( data()->modules() );
            if( fm == nullptr ) {
                DL_BADPOINTER(1, "File Module");
                break;
            }
//            auto fmIf = fm->getIf( clients[0].tag() );

//            fmIf.sync();

            LINE_END;
        }
        if(LINE_IS("remote")) {

            auto clients = data()->clients();
            if( clients.empty() ) {
                DL_INFO(1, "No clients");
                break;
            }
            auto fm = extractFileModule( data()->modules() );
            if( fm == nullptr ) {
                DL_BADPOINTER(1, "File Module");
                break;
            }
//            auto fmIf = fm->getIf( clients[0].tag() );
//            if( fmIf.remote() == nullptr ) {
//                DL_BADPOINTER(1, "remote catalog");
//                break;
//            }

//            DL_INFO(1, "File system: [%p]", fm);
//            if( fmIf.remote()->empty() ) {
//                std::cout << "Empty remote catalog" << std::endl;
//            } else {
//                std::cout << fmIf.remote()->parseToText(true) << std::endl;
//            }

        }

        if(LINE_IS("shu")) {

            auto clients = data()->clients();
            if( clients.empty() ) {
                DL_INFO(1, "No clients");
                break;
            }
            clients[0].makeShadowConnection( DXT::UDP );
            break;
        }
        if(LINE_IS("fch")) {

            auto clients = data()->clients();
            if( clients.empty() ) {
                DL_INFO(1, "No clients");
                break;
            }

            auto proc = DPN_PROCS::processor<PT__RESERVE_CHANNEL>();
            if( clients[0].channels().empty() ) {
                DL_ERROR(1, "No channels");
                break;
            }
            proc->setChannel( clients[0].channels()[0] );
            proc->setBackwardUser( extractFileModule( data()->modules() ) );

            clients[0].send( proc );

            break;
        }
        if(LINE_IS("fr")) {

            DArray<int> keyset;
            if( list.empty() ) {
                DL_ERROR( 1, "Empty list");
                break;
            }

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

            auto clients = data()->clients();
            if( clients.empty() ) {
                DL_INFO(1, "No clients");
                break;
            }
            auto fm = extractFileModule( data()->modules() );
            if( fm == nullptr ) {
                DL_BADPOINTER(1, "File Module");
                break;
            }
//            auto fmIf = fm->getIf( clients[0].tag() );
//            fmIf.requestFileset( keyset );

            break;
        }

        //-----------------------------------
        if( LINE_IS("test") ) {

            DL_INFO(1, ">>> DPN test");
            LINE_END;
        }
        if(LINE_IS("info")) {

        }
        if(LINE_IS("a")) {
            data()->acceptAll();
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
        if(LINE_IS("f")) {

//            const char *testFile = "F:/DFS_SPACE/UPLOAD/Jolie.jpg";
//            DPN_FileClient fc(connections_core.client(0));
//            fc.sendFastFile(testFile);
        }
        //-----------------------------------

    } while(0);

    return true;
}

bool DPeerNode::share(int port, bool autoaccept) {
    return data()->sharePort( port, autoaccept );
}
bool DPeerNode::connect(const char *address, int port) {
    return data()->connectTo( address, port );
}
