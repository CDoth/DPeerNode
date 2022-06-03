#ifndef DPN_PROCESSORS_H
#define DPN_PROCESSORS_H
#include <thread>
#include <DDirReader.h>


//#include "DPN_TransmitProcessor.h"
#include "DPN_Modules.h"







class DPN_Processor__text_message : public DPN_TransmitProcessor {
public:
    DPN_PROCESSOR
    void setMessage(const std::string &__message) {
        UNIT_MESSAGE = __message;
    }
private:
    DPN_Result receiveReaction() override;
private:
    UNIT(std::string) UNIT_MESSAGE = content;
};
class DPN_Processor__ping : public DPN_TransmitProcessor {
public:
    DPN_PROCESSOR

    enum Mode{
      PING = 3, ANSWER = 4
    };
    DPN_Processor__ping() {
        mode = PING;
    }
private:
    DPN_Result sendPrefix() override;
    DPN_Result receiveReaction() override;
private:
    UNIT(Mode) UNIT_MODE = content;
    UNIT(uint64_t) UNIT_SECONDS = content;
    UNIT(uint64_t) UNIT_USECONDS = content;
private:
    DPN_TimeMoment moment;
    Mode mode;
};
enum RequestType {
    NO_TYPE
    ,REQUEST_SHADOW_CONNECTION
    ,REQUEST_CHANNEL_RESERVING
    ,REQUEST_NET_LOCAL_ENVIRONMENT

};
class DPN_Processor__request : public DPN_TransmitProcessor {
public:
    DPN_PROCESSOR

    DPN_Processor__request() {
        makeRequest();
    }
    void setType(RequestType t) {
        UNIT_REQUEST_TYPE = t;
    }
    void setValue(int v) {UNIT_VALUE = v;}
    void setPort(int port) {UNIT_VALUE = port;}
    void setState(bool s) {UNIT_STATE = s;}

    void setHash(const std::string &hash) {UNIT_HASH = hash;}
    void setModuleName(const std::string &moduleName) {UNIT_MODULE_NAME = moduleName;}
    void setSourceAddress(const PeerAddress &source) {UNIT_SOURCE_ADDRESS = source;}
    void setTargetAddress(const PeerAddress &target) {UNIT_TARGET_ADDRESS = target;}
    void setExtraData(const DPN_ExpandableBuffer &e) {UNIT_EXTRA_DATA = e;}

    void makeAnswer() {UNIT_IS_ANSWER = true;}
    void makeRequest() {UNIT_IS_ANSWER = false;}
private:
    DPN_Result sendPrefix() override;
    DPN_Result receiveReaction() override;

    DPN_Result procReceivingRequest();
    DPN_Result procReceivingAnswer();
private:
    UNIT(bool) UNIT_IS_ANSWER = content;
    UNIT(RequestType) UNIT_REQUEST_TYPE = content;
    UNIT(int) UNIT_VALUE = content;
    UNIT(bool) UNIT_STATE = content;

    UNIT(std::string) UNIT_HASH = content;
    UNIT(std::string) UNIT_MODULE_NAME = content;

    UNIT(PeerAddress) UNIT_SOURCE_ADDRESS = content;
    UNIT(PeerAddress) UNIT_TARGET_ADDRESS = content;
    UNIT(DPN_ExpandableBuffer) UNIT_EXTRA_DATA = content;
};
class DPN_Processor__make_shadow_connection : public DPN_TransmitProcessor {
public:
    DPN_PROCESSOR
    DPN_Processor__make_shadow_connection() {
        UNIT_REQUESTED_UDP_PORT = Request;
    }

    void setShadowKey(const std::string &shadowKey ) {
        UNIT_KEY = shadowKey;
    }
    void setSource(const PeerAddress &source, const PeerAddress &target) {
        UNIT_SOURCE = source;
        UNIT_TARGET = target;
    }
    void udpSetOpenedPort(int port) {
        UNIT_REQUESTED_UDP_PORT = port;
    }
    void udpRequestFault() {
        UNIT_REQUESTED_UDP_PORT = RequestFault;
    }
private:
    DPN_Result sendPrefix() override;
    DPN_Result receiveReaction() override;
private:
    enum {
      RequestFault = -1010
        ,Request   = -707
    };
    UNIT(std::string) UNIT_KEY = content;
    UNIT(PeerAddress) UNIT_SOURCE = content;
    UNIT(PeerAddress) UNIT_TARGET = content;
    UNIT(int) UNIT_REQUESTED_UDP_PORT = content;
};
//class DPN_Processor__reserve_shadow_channel : public DPN_TransmitProcessor {
//public:
//    DPN_PROCESSOR
//    void setContext(const PeerAddress &source, const PeerAddress &target, const std::string &shadowKey) {
//        UNIT_SHADOW_KEY = shadowKey;
//        UNIT_SOURCE = source;
//        UNIT_TARGET = target;
//    }
//private:
//    DPN_Result receiveReaction() override;
//private:
//    UNIT(std::string) UNIT_MODULE_NAME = content;
//    UNIT(std::string) UNIT_SHADOW_KEY = content;
//    UNIT(PeerAddress) UNIT_SOURCE = content;
//    UNIT(PeerAddress) UNIT_TARGET = content;
//};

class DPN_Processor__net_environment : public DPN_TransmitProcessor {
public:
    DPN_PROCESSOR
private:
    DPN_Result sendPrefix() override;
    DPN_Result receiveReaction() override;
private:
    UNIT(DArray<PeerAddress>) UNIT_ENV = content;
};


DPN_PROCESSOR_BIND(DPN_Processor__make_shadow_connection, DPN_PACKETTYPE__MAKE_SHADOW_CONNECTION);
DPN_PROCESSOR_BIND(DPN_Processor__text_message, DPN_PACKETTYPE__TEXT_MESSAGE);
DPN_PROCESSOR_BIND(DPN_Processor__ping, DPN_PACKETTYPE__PING);
DPN_PROCESSOR_BIND(DPN_Processor__request, DPN_PACKETTYPE__REQUEST);
DPN_PROCESSOR_BIND(DPN_Processor__net_environment, DPN_PACKETTYPE__LOCAL_ENVIRONMENT);





// Files module:
/*
class DPN_Processor__directory_topolgy : public DPN_TransmitProcessor {
public:
    DPN_PROCESSOR

    DPN_Processor__directory_topolgy() {


        addItem(UNIT_TOPOLOGY, TF_PACKET);
    }

    DPN_Processor__directory_topolgy * setSource(const std::string &topology) {
        UNIT_TOPOLOGY.copyString(topology);
        return this;
    }

private:
    bool sendPrefix() override;
    bool sendReaction() override;
    bool receivePrefix() override;
    bool receiveReaction() override;
private:
    UNIT(std::string) UNIT_TOPOLOGY;
};
class DPN_Processor__register_fast_file : public DPN_TransmitProcessor {
public:
    DPN_PROCESSOR

    DPN_Processor__register_fast_file() {

        addItem(UNIT_FILEKEY, TF_RAW);
        addItem(UNIT_FILESIZE, TF_RAW);
        addItem(UNIT_FILENAME, TF_PACKET);
    }

    DPN_Processor__register_fast_file * setSource(DFile &__file) {

        file = __file;
        return this;
    }


private:
    bool sendPrefix() override;
    bool sendReaction() override;
    bool receivePrefix() override;
    bool receiveReaction() override;

    void clearProcessor() override {
        file.clearObject();
    }
private:

    UNIT(int) UNIT_FILEKEY;
    UNIT(std::string) UNIT_FILENAME;
    UNIT(size_t) UNIT_FILESIZE;

    DFile file;
};
class DPN_Processor__fileset_packet : public DPN_TransmitProcessor {
public:
    DPN_PROCESSOR

    DPN_Processor__fileset_packet() {

        addItem(UNIT_FILE_KEY, TF_RAW);
        addItem(UNIT_PACKET_BUFFER, TF_PACKET);

        group = nullptr;
        packetSize = 0;
        actual = nullptr;
    }

    DPN_Processor__fileset_packet * setSource(DPN_FileTransportGroup *__group) {
        group = __group;
        return this;
    }

private:
    bool sendPrefix() override;
    bool sendReaction() override;
    bool receivePrefix() override;
    bool receiveReaction() override;

    void clearProcessor() override {
        group = nullptr;
        packetSize = 0;
        actual = nullptr;
    }
private:
    UNIT(uint8_t*) UNIT_PACKET_BUFFER;
    UNIT(int) UNIT_FILE_KEY;
private:
    DPN_FileTransportGroup *group;
    int packetSize;
private:
    DPN_FileTransportHandler *actual;
};
class DPN_Processor__request_file : public DPN_TransmitProcessor {
public:
    DPN_PROCESSOR
    DPN_Processor__request_file() {
        addItem(UNIT_FILEKEY, TF_RAW, 0);
        addItem<size_t>(UNIT_SIZE, TF_RAW, 0);
        addItem<size_t>(UNIT_SHIFT, TF_RAW, 0);

        addItem(UNIT_NAME, TF_PACKET);
        addItem(UNIT_SHIFT_HASH, TF_PACKET);

        useMetaPacket();
    }

    DPN_Processor__request_file * appendFile(const DFile &__file) {

        fileset.push_back(__file);
        return this;
    }
    DPN_Processor__request_file * setFileSet(const DArray<DFile> &__fileset) {

        fileset = __fileset;
        return this;
    }
    DPN_Processor__request_file * appendFileSet(const DArray<DFile> &__fileset) {

        fileset.appendArray(__fileset);
        return this;
    }
private:
    bool sendPrefix() override;
    bool sendReaction() override;
    bool receivePrefix() override;
    bool receiveReaction() override;

    void clearProcessor() override {
        fileset.clear();
        transport_set.clear();

    }
    void repeatProcessor() override {

    }
private:
    UNIT(int) UNIT_FILEKEY;
    UNIT(std::string) UNIT_NAME;
    UNIT(size_t) UNIT_SIZE;
    UNIT(size_t) UNIT_SHIFT;
    UNIT(std::string) UNIT_SHIFT_HASH;
private:
    DArray<DFile> fileset;
    DArray<DPN_FileTransportHandler*> transport_set;
};
class DPN_Processor__file_transmit_prepare : public DPN_TransmitProcessor {
public:
    DPN_PROCESSOR

    DPN_Processor__file_transmit_prepare() {

        addItem(UNIT_FILE_KEY, TF_RAW, 0);
        addItem<size_t>(UNIT_ACTUAL_SHIFT, TF_RAW, 0);

        useMetaPacket();
        reaction = nullptr;
    }

    DPN_Processor__file_transmit_prepare * setSource(const DArray<DPN_FileTransportHandler*> &transport) {
        transport_set = transport;
        return this;
    }

private:
    bool sendPrefix() override;
    bool sendReaction() override;
    bool receivePrefix() override;
    bool receiveReaction() override;
    void clearProcessor() override {
        transport_set.clear();
    }
private:


    UNIT(int) UNIT_FILE_KEY;
    UNIT(size_t) UNIT_ACTUAL_SHIFT;
private:
    DArray<DPN_FileTransportHandler*> transport_set;
private:
    PROCESSOR_PTR(DPN_PACKETTYPE__TRANSMIT_ANSWER) reaction;
};
class DPN_Processor__file_packet : public DPN_TransmitProcessor {
public:
    DPN_PROCESSOR

    DPN_Processor__file_packet() {

        addItem(UNIT_FILE_KEY, TF_RAW);
        addItem(UNIT_PACKET_BUFFER, TF_PACKET);

        h = nullptr;
        packetSize = 0;
        packetNumber = 0;

    }

    DPN_Processor__file_packet * setSource(DPN_FileTransportHandler *handler) {
        h = handler;
        return this;
    }

private:
    bool sendPrefix() override;
    bool sendReaction() override;
    bool receivePrefix() override;
    bool receiveReaction() override;

    void clearProcessor() override {
        h = nullptr;
        packetSize = 0;
    }
private:

    UNIT(uint8_t*) UNIT_PACKET_BUFFER;
    UNIT(uint64_t) UNIT_FILE_KEY;

private:
    DPN_FileTransportHandler *h;
    int packetSize;
    int packetNumber;
};

DPN_PROCESSOR_BIND(DPN_Processor__directory_topolgy, DPN_PACKETTYPE__DIRECTORY_TOPOLOGY);
DPN_PROCESSOR_BIND(DPN_Processor__file_packet, DPN_PACKETTYPE__FILE_PACKET);
DPN_PROCESSOR_BIND(DPN_Processor__fileset_packet, DPN_PACKETTYPE__FILESET_PACKET);
DPN_PROCESSOR_BIND(DPN_Processor__request_file, DPN_PACKETTYPE__REQUEST_FILE);
DPN_PROCESSOR_BIND(DPN_Processor__register_fast_file, DPN_PACKETTYPE__REGISTER_FAST_FILE);
DPN_PROCESSOR_BIND(DPN_Processor__file_transmit_prepare, DPN_PACKETTYPE__FILE_TRANSMIT_PREPARE);

*/






#endif // DPN_PROCESSORS_H

