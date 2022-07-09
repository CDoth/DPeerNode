#include "DPN_Chat.h"
#include "DPN_ChatProcessors.h"

using namespace DPN::Network;
using namespace DPN::Logs;
namespace DPN::Chat {
//========================================================================================== ChatMessage
    ChatMessage::ChatMessage() {}
    ChatMessage::ChatMessage(const std::string &m, Side s) : DWatcher< __chat_message__ >(true) {
        data()->iContent = m;
        data()->iSenderType = s;
        data()->iTimeMoment.fixTime();
        data()->iTransmitStatus = (s == DPN::LOCAL) ? MSG__SENDING : MSG__RECEIVED;
        data()->iViewed = false;
    }
    std::string ChatMessage::content() const {
        if( isEmptyObject() ) {
            DL_ERROR(1, "Empty watcher");
            return std::string();
        }
        return data()->iContent;

    }
    void ChatMessage::setStatus(MessageTransmitStatus s) {
        if( isEmptyObject() ) {
            DL_ERROR(1, "Empty watcher");
            return;
        }
        data()->iTransmitStatus = s;
    }
    //========================================================================================== ChatSystem
    ChatSystem::ChatSystem(const std::string &name, ClientCenter &cc) : NetworkModule( name, cc ) {

    }
    ClientChatInterface ChatSystem::getClientInterface(Client::Tag tag) {

    }
    bool ChatSystem::useChannel(Client::Tag tag, Direction d, __channel_mono_interface mono, const DPN_ExpandableBuffer &context) {

    }
    //========================================================================================== ClientChatInterface
    bool ClientChatInterface::sendMessage(const std::string &message) {

        if( badInterface() ) {
            DL_ERROR(1, "Bad interface");
            return false;
        }
        ChatMessage M( message, DPN::LOCAL );
        inner()->iChatLegend.iLegend.append( M );
        auto proc = DPN_PROCS::processor<PT__CHAT__MESSAGE>();
        proc->setMessage( M );
        return inner()->send( proc );
    }

    //========================================================================================== ClientChatPrivateInterface
    void ClientChatPrivateInterface::messageSent(ChatMessage m) {
        m.setStatus( MSG__SENT );
    }
    void ClientChatPrivateInterface::messageReceived(ChatMessage m)
    {

    }
    //==========================================================================================
    ChatSystem *extractChatModule(Modules modules) {
        return reinterpret_cast<ChatSystem*>( modules.module( "ChatModule" ) );
    }
}
