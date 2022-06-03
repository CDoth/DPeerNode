#ifndef DPN_MEDIASYSTEMPROCESSORS_H
#define DPN_MEDIASYSTEMPROCESSORS_H

#include "DPN_Modules.h"


enum DPN_VideoAnswerType {
    DVS__STREAM_RECEIVE_PREPARING
};

class DPN_MediaProcessor : public DPN_TransmitProcessor {
public:
    DPN_MediaProcessor();
private:
    void postInition() override;
protected:
    DPN_MediaSystem *pMediaSystem;
};
//class DPN_Processor__media_answer : public DPN_MediaProcessor {
//public:
//    DPN_PROCESSOR

//    void setType(DPN_VideoAnswerType t) { UNIT_TYPE = t; }
//    void setState(bool s) { UNIT_STATE = s; }
//    void setDetails( const std::string &d ) { UNIT_DETAILS = d; }
//    void setStreamPort(int p) { UNIT_STREAM_PORT = p; }
//    void setMediaType(DPN_MediaType t) { UNIT_MEDIA_TYPE = t; }
//private:
//    DPN_Result sendPrefix() override;
//    DPN_Result receiveReaction() override;
//private:
//    DPN_Result videoAnswerSend();
//    DPN_Result videoAnswerReceive();
//private:
//    UNIT(DPN_VideoAnswerType) UNIT_TYPE = content;
//    UNIT(bool) UNIT_STATE = content;
//    UNIT(std::string) UNIT_DETAILS = content;

//    UNIT(int) UNIT_STREAM_PORT = content;
//    UNIT(DPN_MediaType) UNIT_MEDIA_TYPE = content;

//};

class DPN_Processor__media_stream_prepare;
class __transaction : public DPN_AbstractTransaction {
public:
    typedef __action_line<DPN_Processor__media_stream_prepare> ActionLine;
    inline ActionLine & actionLine() {return iLine;}
    DPN_MediaType type() const {return iType;}
    MediaContextSet & contextSet() {return iContextSet;}

    DPN_MediaType iType;
private:
    ActionLine iLine;
    MediaContextSet iContextSet;
};

class DPN_Processor__media_stream_prepare : public DPN_MediaProcessor {
public:
    DPN_PROCESSOR

    DPN_Processor__media_stream_prepare() {
        UNIT_BACK_TYPE = DPN_MS__NOTYPE;
        UNIT_BACK_STRICT_MASK = DPN_MS__NOTYPE;
    }
    void setCustomContext(const VideoEncoderContext &context) {
        iCutomContext = context;
    }
    void setType(DPN_MediaType type) {
        UNIT_TYPE = type;
    }
    void setBackType(DPN_MediaType type, DPN_MediaType strictMask) {
        UNIT_BACK_TYPE = type;
        UNIT_BACK_STRICT_MASK = strictMask;
    }

private:
    DPN_Result sendPrefix() override;
    DPN_Result receiveReaction() override;

    DPN_Result failureProcessing() override;
    bool makeHostLine() override;
    bool makeServerLine() override;
    bool resendPredicat() override;

    void setFaultDetails(const char *details) {
        UNIT_FAULT_DETAILS = details;
    }

private:

    // DEFAULT MODE
    // HOST         SERVER
    // [action1] -> [action1]
    // [action2] -> [action2]
    // [action3] -> [action3]


    // SNAKE MODE:
    // HOST         SERVER
    // [action1] -> [action1]
    //                  V
    // [action2] <- [action2]
    //     V
    // [action3] -> [action3]


    // check streams available
    DPN_Result hostPreparing();
    DPN_Result serverPreparing();

    // create streams
    DPN_Result hostCreating();
    DPN_Result serverCreating();

    // run streams
    DPN_Result hostRun();
    DPN_Result serverRun();


    // faults:
    DPN_Result hostFault();
    DPN_Result serverFault();

private:
    DPN_Result __prepare_stream();
    DPN_Result __create_stream(DPN_DirectionType d);
    DPN_Result __run_stream(DPN_DirectionType d);
private:
    VideoEncoderContext iCutomContext;

    UNIT(DPN_MediaType) UNIT_TYPE = content;
    UNIT(DPN_MediaType) UNIT_BACK_TYPE = content;
    UNIT(DPN_MediaType) UNIT_BACK_STRICT_MASK = content;
    UNIT(int) UNIT_STREAM_PORT = content;

    UNIT(std::string) UNIT_FAULT_DETAILS = content;
};

//DPN_PROCESSOR_BIND(DPN_Processor__media_answer, DPN_PACKETTYPE__MEDIA_ANSWER);
DPN_PROCESSOR_BIND(DPN_Processor__media_stream_prepare, DPN_PACKETTYPE__MEDIA_STREAM_PREPARE);


#endif // DPN_MEDIASYSTEMPROCESSORS_H

