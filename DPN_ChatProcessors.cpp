#include "DPN_ChatProcessors.h"

using namespace DPN::Logs;
namespace DPN_ChatProcessors {
    //------------------------------------------------------------------------------
    Base::Base() {
        pChatSystem = nullptr;
    }
    void Base::injection() {
        if( (pChatSystem = DPN::Chat::extractChatModule( *this )) == nullptr ) {
            DL_ERROR(1, "Can't extract chat module");
        }
    }
    DPN::Chat::ClientChatPrivateInterface Base::privateInterface() {
        if( badModule() ) return DPN::Chat::ClientChatPrivateInterface();
        return pChatSystem->getClientPrivateInterface( tag() );
    }
    //------------------------------------------------------------------------------
    void Message::makeActionLine() {
        line<Message>()
                << &Message::sending
                << &Message::receiving
                << &Message::confirmation
                   ;
    }
    DPN_Result Message::sending(HOST_CALL) {
        INFO_LOG;

        std::string content = wMessage.content();
        if( content.empty() || content.size() > DPN::Chat::MAX_MESSAGE_SIZE ) {
            DL_BADVALUE(1, "Message size: [%d] (min: [1]; max: [%d])", content.size(), DPN::Chat::MAX_MESSAGE_SIZE );
            return DPN_FAIL;
        }
        auto ii = privateInterface();
        if( ii.badInterface() ) {
            DL_ERROR(1, "Bad interface");
            return DPN_FAIL;
        }
        ii.messageSent( wMessage );
//        wMessage.setStatus( DPN::Chat::MSG__SENT );
        return DPN_SUCCESS;
    }
    DPN_Result Message::receiving(SERVER_CALL) {
        INFO_LOG;
        std::string message = UNIT_MESSAGE.get();
        if( message.empty() || message.size() > DPN::Chat::MAX_MESSAGE_SIZE ) {
            DL_BADVALUE(1, "Message size: [%d] (min: [1]; max: [%d])", message.size(), DPN::Chat::MAX_MESSAGE_SIZE );
            return DPN_FAIL;
        }
        auto ii = privateInterface();
        if( ii.badInterface() ) {
            DL_ERROR(1, "Bad interface");
            return DPN_FAIL;
        }
        DPN::Chat::ChatMessage M( message, DPN::PEER );
        ii.messageReceived( M );

        return DPN_SUCCESS;
    }
    DPN_Result Message::confirmation(HOST_CALL) {
        INFO_LOG;

        wMessage.setStatus( DPN::Chat::MSG__DELIVERED );

        return DPN_SUCCESS;
    }



}
