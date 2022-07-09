#ifndef DPN_CHATPROCESSORS_H
#define DPN_CHATPROCESSORS_H

#include "DPN_TransmitProcessor.h"
#include "DPN_Chat.h"

namespace DPN_ChatProcessors {
    class Base : public DPN_TransmitProcessor {
    public:
        Base();
        void injection() override;
    protected:
        inline DPN::Chat::ChatSystem *chatSystem() { return pChatSystem; }
        inline bool badModule() const { return pChatSystem == nullptr; }
        DPN::Chat::ClientChatPrivateInterface privateInterface();
    private:
        DPN::Chat::ChatSystem *pChatSystem;
    };

    class Message : public Base {
    public:
        DPN_PROCESSOR;
        void setMessage( DPN::Chat::ChatMessage message ) {
            wMessage = message;
        }
    private:
        void makeActionLine() override;
        DPN_Result sending( HOST_CALL );
        DPN_Result receiving( SERVER_CALL );
        DPN_Result confirmation( HOST_CALL );
    private:
        DPN::Chat::ChatMessage wMessage;
    private:
        DECL_UNIT(std::string, UNIT_MESSAGE);
    };
}
using namespace DPN_ChatProcessors;
DPN_PROCESSOR_BIND(Message, PT__CHAT__MESSAGE);



#endif // DPN_CHATPROCESSORS_H
