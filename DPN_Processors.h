#ifndef DPN_PROCESSORS_H
#define DPN_PROCESSORS_H
#include <thread>
#include <DDirReader.h>


#include "DPN_TransmitProcessor.h"


namespace DPN_DefaultProcessor {

    class TextMessage : public DPN_TransmitProcessor {
    public:
        DPN_PROCESSOR
        void setMessage(const std::string &message) { UNIT_MESSAGE = message; }
    private:
        void makeActionLine() override;
    private:
        DPN_Result render( SERVER_CALL );
    private:
        DECL_UNIT(std::string, UNIT_MESSAGE);
    };
    class MakeShadow : public DPN_TransmitProcessor {
    public:
        DPN_PROCESSOR
        MakeShadow() {
            pRequester = nullptr;
            iType = DXT::TCP;
        }
        void setType(DXT::Type type) { iType = type; }
        void setRequester( DPN_AbstractModule *module ) { pRequester = module; }
        void setExtraData( const DPN_ExpandableBuffer &extra ) { wExtraData = extra; }
        void setPort( int port ) { UNIT_PORT = port; }
    private:
        DPN_Result requestShadow( HOST_CALL );
        DPN_Result processShadow( SERVER_CALL );

        DPN_Result makeShadow( HOST_CALL );
        DPN_Result makeShadow( SERVER_CALL );

        DPN_Result connect( HOST_CALL );

        void fault( SERVER_CALL ) override;
        void fault( HOST_CALL ) override;
    private:
        void makeActionLine() override;
    private:
        DECL_UNIT(int, UNIT_PORT);
        DECL_UNIT(std::string, UNIT_KEY);
        DECL_UNIT(DXT::Type, UNIT_TYPE);
    private:
        DPN_NodeConnector *pPort;
        DXT::Type iType;
        std::string shadowKey;

        DPN_AbstractModule *pRequester;
        DPN_ExpandableBuffer wExtraData;
    };
    class SyncChannel : public DPN_TransmitProcessor {
    public:
        DPN_PROCESSOR;
    private:
        DPN_Result hostRequest( HOST_CALL );
        DPN_Result serverSync( SERVER_CALL );
    private:
        void makeActionLine() override;
    private:
        DECL_UNIT(std::string, UNIT_SHADOW_KEY);
        DECL_UNIT(MonoChannelState, UNIT_FORWARD_STATE);
        DECL_UNIT(MonoChannelState, UNIT_BACKWARD_STATE);
    private:
        __channel wChannel;
    };
    class ReserveChannel : public DPN_TransmitProcessor {
    public:
        DPN_PROCESSOR;

        ReserveChannel();
        void setChannel( __channel c ) { wChannel = c; }
        void setForwardUser( DPN_AbstractModule *module ) { pForwardUser = module; }
        void setForwardContext( DPN_ExpandableBuffer &context ) { wForwardContext = context; }

        void setBackwardUser( DPN_AbstractModule *module ) { pBackwardUser = module; }
        void setBackwardContext( DPN_ExpandableBuffer &context ) { wBackwardContext = context; }

    private:
        DPN_Result hostRequest( HOST_CALL );
        DPN_Result serverProcess( SERVER_CALL );
    private:
        void makeActionLine() override;
    private:
        DPN_AbstractModule *pForwardUser;
        DPN_AbstractModule *pBackwardUser;

        DPN_ExpandableBuffer wForwardContext;
        DPN_ExpandableBuffer wBackwardContext;

        __channel wChannel;
    private:
        DECL_UNIT(std::string, UNIT_SHADOW_KEY);
        DECL_UNIT(std::string, UNIT_FORWARD_MODULE_NAME);
        DECL_UNIT(std::string, UNIT_BACKWARD_MODULE_NAME);

        DECL_UNIT(DPN_ExpandableBuffer, UNIT_FORWARD_CONTEXT);
        DECL_UNIT(DPN_ExpandableBuffer, UNIT_BACKWARD_CONTEXT);
    };


//    class Ping : public DPN_TransmitProcessor {
//    public:
//        DPN_PROCESSOR

//        enum Mode{
//          PING = 3, ANSWER = 4
//        };
//        DPN_Processor__ping() {
//            mode = PING;
//        }
//    private:
//        DPN_Result sendPrefix() override;
//        DPN_Result receiveReaction() override;
//    private:
//        UNIT(Mode) UNIT_MODE = content;
//        UNIT(uint64_t) UNIT_SECONDS = content;
//        UNIT(uint64_t) UNIT_USECONDS = content;
//    private:
//        DPN_TimeMoment moment;
//        Mode mode;
//    };
}


using namespace DPN_DefaultProcessor;
DPN_PROCESSOR_BIND(MakeShadow, PT__MAKE_SHADOW_CONNECTION);
DPN_PROCESSOR_BIND(TextMessage, PT__TEXT_MESSAGE);
DPN_PROCESSOR_BIND(SyncChannel, PT__SYNC_CHANNEL);
DPN_PROCESSOR_BIND(ReserveChannel, PT__RESERVE_CHANNEL);











#endif // DPN_PROCESSORS_H

