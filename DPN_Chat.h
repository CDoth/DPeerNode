#ifndef DPN_CHAT_H
#define DPN_CHAT_H
#include "DPN_Network.h"

namespace DPN::Chat {

//    namespace DPN_ChatProcessors {
//        class Base;
//    }
    //-------------------------------------------------------
    enum MessageTransmitStatus {
        MSG__SENDING
        , MSG__SENT
        , MSG__DELIVERED
        , MSG__FAILED

        , MSG__RECEIVED
    };
    enum MessageFlags {
        // Flags here...
    };
    struct __chat_message__ {
        std::string iContent;
        DPN::Time::TimeMoment iTimeMoment;
        DPN::Side iSenderType;
        MessageTransmitStatus iTransmitStatus;
        bool iViewed;
    };

    class ChatMessage
            : private DWatcher< __chat_message__ >
    {
    public:
        ChatMessage();
        ChatMessage( const std::string &m, DPN::Side s );
        std::string content() const;
        void setStatus( MessageTransmitStatus s );
    };
    struct GroupChatMessage : public ChatMessage {
        std::string iSenderName;
        DPN::Network::PeerAddress iSenderAddress;
        DPN::Client::Tag pSenderTag;
    };
    struct ChatLegend {
        DArray< ChatMessage > iLegend;
    };
    struct GroupChatLegend {
        DArray< GroupChatMessage > iLegend;
    };
    //------------------------------------------------------------------------
    struct __client_chat_data__
            : public DPN::Client::Core
    {
        std::string iChatName;
        ChatLegend iChatLegend;
    };
    class ClientChatInterface
            : public DPN::Interface::InterfaceReference< __client_chat_data__ > {
    public:
        bool sendMessage( const std::string &message );
    };
    class ClientChatPrivateInterface
            : public DPN::Interface::InterfaceReference< __client_chat_data__ > {
    public:
        void messageSent( ChatMessage m );
        void messageReceived( ChatMessage m );
    };
    class ClientChatDescriptor
            : public DPN::Interface::DataReference< __client_chat_data__ > {
    public:
        ClientChatInterface getInterface();
        ClientChatPrivateInterface getPrivateInterface();
    };
    //------------------------------------------------------------------------
    struct __group_chat_data__ {
        std::string iChatName;
        GroupChatLegend iGroupChatLegend;
        DArray< DPN::Client::Core > iMembers;
    };
    class GroupChatInterface
            : public DPN::Interface::InterfaceReference< __group_chat_data__ > {
    public:
        bool sendMessage( const std::string &message );
    };
    class GroupChatDescriptor
            : public DPN::Interface::DataReference< __client_chat_data__ > {

    };
    //------------------------------------------------------------------------
    enum {
        MAX_MESSAGE_SIZE = 1024
    };
    class ChatSystem
            : public DPN::Network::NetworkModule
    {
    public:
        ChatSystem( const std::string &name, ClientCenter &cc );
        ClientChatInterface getClientInterface( DPN::Client::Tag tag );
        ClientChatPrivateInterface getClientPrivateInterface( DPN::Client::Tag tag );

        bool useChannel( DPN::Client::Tag tag, DPN::Direction d, __channel_mono_interface mono, const DPN_ExpandableBuffer &context ) override;
    private:
        std::map< DPN::Client::Tag, ClientChatDescriptor> iClientsData;
        DArray< GroupChatDescriptor > aGroupChats;
    };
    ChatSystem * extractChatModule( DPN::Modules modules );
};


#endif // DPN_CHAT_H
