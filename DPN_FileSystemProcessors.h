#ifndef DPN_FILESYSTEMPROCESSORS_H
#define DPN_FILESYSTEMPROCESSORS_H



#include "DPN_Processors.h"
#include "DPN_Modules.h"

/*
class __test_processor {
public:
    void addFileset(const DArray<DFile> &fileset) {
        __fileset = fileset;
    }
    void sendPrepare() {
        __file_metadata md;
        FOR_VALUE(__fileset.size(), i) {
            const DFile &f = __fileset[i];
//            md.key = f.key();
            md.key = i;
            md.name = f.name();
            md.size = f.size();
            md.shift = 123;
            md.shift_hash = "qwe";

            item5.add(md);
        }

        item1 = 45504;
        item2 = "String QWASD!@#";
        item3.add("+++ array string (POIUY)");
        item3.add("+++ array string (ASDFG)");
        item3.add("+++ array string (ZXCVB)");
        __complex2 c2;
        c2.testInit();
        item4 = c2;
    }
    void send(DTcp &c) {
        content.parseBuffers();

        std::cout << "buffer: " << content.buffer().size() << std::endl;

        int sb = 0;
        sb = c.unlocked_send_it(content.buffer().getData(), content.buffer().size());

        std::cout << "send: " << sb << std::endl;

    }
    void recv(DTcp &c) {
        int packetSize = 0;
        int packetType = 0;
        c.recv_to(&packetSize, sizeof(int));
        c.recv_to(&packetType, sizeof(int));

        content.requestBuffer(packetSize);
        int rb = 0;
        rb = c.recv_to(content.rawBuffer(), packetSize);

        std::cout << "recv: size: " << packetSize
                  << " type: " << packetType
                  << " rb: " << rb
                  << std::endl;

        if( content.deparseBuffer() == false ) {
            std::cout << "DEPARSE FAIL"
                      << std::endl;
        }
        std::cout << "Deparsed"
                  << std::endl;

    }
    void recvSuffix() {

        int v = item1.get();
        std::string s = item2.get();
        DArray<std::string> sa = item3.get();
        __complex2 c2 = item4.get();
        DArray<__file_metadata> fa = item5.get();

        std::cout << "v: " << v
                  << " s: " << s
                  << " sa(size): " << sa.size()
                  << " fa(size): " << fa.size()
                  << std::endl;

        std::cout << "sa:" << std::endl;
        FOR_VALUE(sa.size(), i) {
            std::cout << i
                      << " string: " << sa[i]
                      << std::endl;
        }
        std::cout << "fa:" << std::endl;
        FOR_VALUE(fa.size(), i) {
            std::cout << i
                      << " key: " << fa[i].key
                      << " name: " << fa[i].name
                      << " size: " << fa[i].size
                      << " shift: " << fa[i].shift
                      << " shift_hash: " << fa[i].shift_hash
                      << std::endl;
        }
        std::cout << "c2:" << std::endl;

        std::cout << "in: "
                 << " a: " << c2.a.size()
                 << " c: " << c2.c
                 << " d: " << c2.d_s
                 << " f: " << c2.f
                 << " s: [" << c2.s << "] "
                 << " v: " << c2.v
                 << " str a: " << c2.str_a.size()
                    ;
        std::cout << "c2 Arrays:";
        FOR_VALUE(c2.d_s, i) std::cout << c2.d[i]; std::cout << std::endl;
        c2.a.runOut([](const int &v, int i){std::cout << i << " " << v << std::endl; });
        c2.str_a.runOut([](const std::string &v, int i){std::cout << i << " " << v << std::endl; });

    }
private:
    __transmit_content content;

    __hl_item<int> item1 = content;
    __hl_item<std::string> item2 = content;
    __hl_item<DArray<std::string>> item3 = content;
    __hl_item<__complex2> item4 = content;
    __hl_item<DArray<__file_metadata>> item5 = content;
private:
    DArray<DFile> __fileset;
};
*/


struct __file_metadata {
    __file_metadata() {
        key = -1;
        size = 0;
        shift = 0;
        strictable = false;
        verified = false;
    }
    int key;
    size_t size;
    size_t shift;
    std::string name;
    std::string shift_hash;
    bool strictable;
    bool verified;
};
template <>
class dpn_mediator<__file_metadata> {
public:
    template <class ... Args>
    static void __do(__file_metadata &file, Args && ... a) {
        __tr::transport(file.key, a...);
        __tr::transport(file.size, a...);
        __tr::transport(file.shift, a...);
        __tr::transport(file.name, a...);
        __tr::transport(file.shift_hash, a...);
        __tr::transport(file.strictable, a...);
        __tr::transport(file.verified, a...);
    }
    static void save(const __file_metadata &file, DPN_ExpandableBuffer &b) {
        __do(const_cast<__file_metadata&>(file), b);

    }
    static void upload(__file_metadata &file, const DPN_ExpandableBuffer &b, int &p) {
        __do(file, b, p);
    }
};

enum DPN_ANSWER_TYPE {

    // errors:
    DPN_ANSWER__SERVER_ERROR
    ,DPN_ANSWER__HOST_ERROR

    ,DPN_ANSWER__DEFAULT



    ,DPN_ANSWER__NEED_RESYNC
    ,DPN_ANSWER__REMOTE_CATALOG_VERIFIED
    ,DPN_ANSWER__READY_RECEIVE
    ,DPN_ANSWER__NO_DATA
    ,DPN_ANSWER__CHANNEL_RESERVE



};
enum DPN_ANSWER_SUBTYPE {
    DPN_ANSUB__REGISTER_FAST_FILE
    ,DPN_ANSUB__REQUEST_FILE
    ,DPN_ANSUB__TRANSMIT_PREPARE
    ,DPN_ANSUB__FILE_RECEIVING
    ,DPN_ANSUB__FILE_SENDING
};
class DPN_Processor__transmit_answer : public DPN_TransmitProcessor {
public:
    DPN_PROCESSOR

    DPN_Processor__transmit_answer();
    ~DPN_Processor__transmit_answer();

    void setType(DPN_ANSWER_TYPE type) {
        UNIT_ANSWER_TYPE = type;
    }
    void setSubType(DPN_ANSWER_SUBTYPE e) {
        UNIT_ANSWER_SUBTYPE = e;
    }
    void setKeyset(const DArray<int> &__keyset) {
        UNIT_KEYSET = __keyset;
    }
    void addFileKey(DFileKey key) {
        UNIT_KEYSET.add(key);
    }
    int addKey(int key) {
        keyset.push_back(key);
        return keyset.size();
    }
    void setState(bool state) {UNIT_STATE = state;}

    void setHash(const std::string &hash) {
        UNIT_HASH = hash;
    }
    void setValue1(int v) {
        UNIT_VALUE1 = v;
    }
private:
    DPN_Result sendPrefix() override;
    DPN_Result receiveReaction() override;
    void clearProcessor() override {
        keyset.clear();
    }
private:
    DPN_Result proccesSentAnswer();
    DPN_Result proccesReceivedAnswer();
private:
    UNIT(DPN_ANSWER_TYPE) UNIT_ANSWER_TYPE = content;
    UNIT(DPN_ANSWER_SUBTYPE) UNIT_ANSWER_SUBTYPE = content;
    UNIT(bool) UNIT_STATE = content;
    UNIT(DArray<int>) UNIT_KEYSET = content;
    UNIT(std::string) UNIT_HASH = content;
    UNIT(int) UNIT_VALUE1 = content;

private:
    DArray<int> keyset;
};
class DPN_FileProcessor : public DPN_TransmitProcessor {
public:
    DPN_FileProcessor();
protected:

    void postInition() override;
    void createReaction();
    void sendAnswer(DPN_ANSWER_TYPE t, DPN_ANSWER_SUBTYPE st, int local_note, int file_key = -1);
    inline DPN_Processor__transmit_answer * reaction() {return pReaction;}

protected:
    DPN_Processor__transmit_answer *pReaction;
    DPN_FileSystem *pFileSystem;
};


//====================================================================================
class DPN_Processor__hash_sum : public DPN_FileProcessor {
public:
    DPN_PROCESSOR

#define CATALOG_HASHSUM_KEY (-3)

    void setCatalogHashsum(const std::string &hash) {
        UNIT_HASH = hash;
        UNIT_FILE_KEY = CATALOG_HASHSUM_KEY;
    }
    void setFileHashsum(const std::string &hash, int filekey) {
        UNIT_HASH = hash;
        UNIT_FILE_KEY = filekey;
    }
private:
    DPN_Result receiveReaction() override;

private:
        UNIT(std::string) UNIT_HASH = content;
        UNIT(int) UNIT_FILE_KEY = content;
};
class DPN_Processor__sync : public DPN_FileProcessor {
public:
    DPN_PROCESSOR
private:
    DPN_Result sendPrefix() override;
    DPN_Result receiveReaction() override;
private:
    UNIT(std::string) UNIT_TOPOLOGY = content;
};
//class DPN_Processor__reserve_file_channel : public DPN_FileProcessor {
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

//    UNIT(std::string) UNIT_SHADOW_KEY = content;
//    UNIT(PeerAddress) UNIT_SOURCE = content;
//    UNIT(PeerAddress) UNIT_TARGET = content;
//};




struct __tfi {
    __tfi() {
        key = -1;
        shift = 0;
        verified = false;
    }
    int key;
    size_t shift;
    bool verified;
};
template <>
struct dpn_mediator<__tfi> {
private:
  template <class ... Args>
    static void __do(__tfi &o, Args & ... a) {
        __tr::transport(o.key, a...);
        __tr::transport(o.shift, a...);
        __tr::transport(o.verified, a...);
    }
public:
    static void save(const __tfi &o, DPN_ExpandableBuffer &b) {
        __do(const_cast<__tfi&>(o), b);
    }
    static void upload(__tfi &o, const DPN_ExpandableBuffer &b, int &p) {
        __do(o, b, p);
    }
};
class DPN_Processor__register_fast_file : public DPN_FileProcessor {
public:
    DPN_PROCESSOR


    void setSource(DFile &__file) {
        file = __file;
    }


private:
    DPN_Result sendPrefix() override;
    DPN_Result receiveReaction() override;

private:

    UNIT(int) UNIT_FILEKEY = content;
    UNIT(std::string) UNIT_FILENAME = content;
    UNIT(size_t) UNIT_FILESIZE = content;

    DFile file;
private:
};
class DPN_Processor__request_file : public DPN_FileProcessor {
public:
    DPN_PROCESSOR
    DPN_Processor__request_file() {

    }

    void appendFile(DFileKey __key) {

        __keyset.push_back(__key);
    }
    void setFileSet(const DArray<DFileKey> &keyset) {

        __keyset = keyset;
    }
    void appendFileSet(const DArray<DFileKey> &keyset) {

        __keyset.appendArray(keyset);
    }
    void setStrictMode(DPN_FILESTREAM_STRICT_MODE m) {
        strictMode = m;
    }
    void setStrictMask(const DArray<DFileKey> &m) {
        strictMask = m;
    }
private:
    DPN_Result sendPrefix() override;
    DPN_Result receiveReaction() override;

    void clearProcessor() override {
        fileinfo_set.clear();
        __keyset.clear();
    }
    void repeatProcessor() override {

    }
private:
    UNIT(std::string) UNIT_HOST_CATALOG_V = content;
    UNIT(int) UNIT_NOTE = content;
    UNIT(DArray<__file_metadata>) UNIT_FILESET = content;

private:
    DPN_FILESTREAM_STRICT_MODE strictMode;
    DArray<DFileKey> strictMask;
    DArray<DFileKey> __keyset;
    DArray<__tfi> fileinfo_set;
};
class DPN_Processor__file_transmit_prepare : public DPN_FileProcessor {
public:
    DPN_PROCESSOR


    void setSource(const DArray<__tfi> &transport, int serverNote) {
        fileinfo_set = transport;
        note = serverNote;
    }

private:
    DPN_Result sendPrefix() override;
    DPN_Result receiveReaction() override;
    void clearProcessor() override {
        fileinfo_set.clear();
    }
private:

    UNIT(int) UNIT_HOST_NOTE = content;
    UNIT(int) UNIT_SERVER_NOTE = content;
    UNIT(DArray<__tfi>) UNIT_FILE_INFO = content;

private:
    int note;
    DArray<__tfi> fileinfo_set;
};
class DPN_Processor__file_packet : public DPN_FileProcessor {
public:
    DPN_PROCESSOR
    DPN_Processor__file_packet() {
        packetSize = 0;
        packetNumber = 0;
        sessionSize = 0;
    }
    void setKeyset( const DArray<DFileKey> &__keyset, int __note ) {
        keyset = __keyset;
        note = __note;
    }

private:
    bool __preapre(const DArray<DFileKey> &keyset);
private:
    DPN_Result sendPrefix() override;
    DPN_Result sendReaction() override;
    DPN_Result receiveReaction() override;

    void clearProcessor() override {
        packetSize = 0;
        keyset.clear();
    }
private:
    UNIT(uint64_t) UNIT_FILE_KEY = content;
    UNIT(uint8_t*) UNIT_DATA = content;

private:
    DArray<DFileKey> keyset;
    int sessionSize;
    DPN_SimplePtrList<DPN_FileSlice> session;
    DPN_FileSlice *current_slice;


    int note;
    int packetSize;
    int packetNumber;
};

DPN_PROCESSOR_BIND(DPN_Processor__transmit_answer, DPN_PACKETTYPE__TRANSMIT_ANSWER);
DPN_PROCESSOR_BIND(DPN_Processor__hash_sum, DPN_PACKETTYPE__HASH_SUM);
DPN_PROCESSOR_BIND(DPN_Processor__sync, DPN_PACKETTYPE__SYNC);
DPN_PROCESSOR_BIND(DPN_Processor__file_packet, DPN_PACKETTYPE__FILE_PACKET);
DPN_PROCESSOR_BIND(DPN_Processor__request_file, DPN_PACKETTYPE__REQUEST_FILE);
DPN_PROCESSOR_BIND(DPN_Processor__register_fast_file, DPN_PACKETTYPE__REGISTER_FAST_FILE);
DPN_PROCESSOR_BIND(DPN_Processor__file_transmit_prepare, DPN_PACKETTYPE__FILE_TRANSMIT_PREPARE);




#endif // DPN_FILESYSTEMPROCESSORS_H
