#ifndef DPN_TRANSPORTHANDLER_H
#define DPN_TRANSPORTHANDLER_H
#include <DArray.h>
#include <DDirReader.h>


#include "DPN_Buffers.h"
#include "DPN_TransmitTools.h"
#include "DPN_IO.h"
#include "DPN_Channel.h"
#include "DPN_Propagation.h"

#define DPN_DEFAULT_READ_WINDOW (1024)



/*
class DPN_FileTransportHandler;
class DPN_FileUploadTask : public DPN_Task {
public:
    DPN_FileUploadTask();
    DPN_Result proc() override;

    FILE *__file;
    DPN_ExpandableBuffer __buffer;
    size_t __read_now;
    size_t __read;
    std::atomic<DPN_Result> __status;
};
class DPN_FileSlice {
public:
    friend class DPN_FileTransportHandler;
    DPN_FileSlice();
    ~DPN_FileSlice();
    void setReadWindow(int rw);
    bool openR();
    bool openW();

    bool close();
    bool start();
    void stop();
public:
    inline int read(DPN_ExpandableBuffer &b) {return __read(b);}
    int write(const DPN_ExpandableBuffer &b, int size);
    int write(const void *data, int size);


    inline size_t transportedBytes() const {return __transmit_total;}
    inline size_t loadBytes() const {return __load_total;}
    inline size_t transmitLast() const {return __transmit_last;}
    inline size_t loadLast() const {return __load_last;}

    const DPN_TimeRange & range() const {return __transmit_range;}

    bool transportIt(int bytes);
    inline bool isTransported() const {return __transmit_total == __size;}
    inline DFileKey key() const {return __file.key();}
public:
    inline const DFile & base() const {return __file;}
    inline DPN_FileTransportHandler *desc() {return __desc;}
    inline double progress() const {return __progress;}
    inline double speed() const {return __speed;}

private:
    bool init(DPN_FileTransportHandler *desc, size_t start, size_t size);
    bool __open(const char *mode);
    void __clear();
    int __read(DPN_ExpandableBuffer &b);
    int __read__default(DPN_ExpandableBuffer &b);
    int __read__buffering(DPN_ExpandableBuffer &b);
    int __read__thread_buffering(DPN_ExpandableBuffer &b);
private:
    DPN_TimeRange __transmit_range;
    DPN_TimeRange __speed_tester;
    double __progress;
    double __speed;
    float __test_portion;
private:
    size_t __start;
    size_t __size;
    size_t __left;
    FILE *__source;
private:
    int __read_window;
    size_t __load_last;
    size_t __load_total;
    size_t __transmit_last;
    size_t __transmit_total;
    DPN_ExpandableBuffer __inner_buffer;
private:
    DPN_FileTransportHandler *__desc;
    DFile __file;
};
class DPN_FileTransportHandler {
public:
    DPN_FileTransportHandler();
    inline void setBase(const DFile &file) {__base = file;}
    inline void fast() {__fast_file = true;}
    inline void noFast() {__fast_file = false;}
    inline bool isFast() const {return __fast_file;}
    inline void setShift(size_t shift) {__shift = shift;}
    inline size_t shift() const {return __shift;}
    const DFile & base() const {return __base;}
    void setStrictable(bool s) {__strictable = s;}
    inline bool strictable() const {return __strictable;}
    size_t size() const {return __base.size();}
    inline DFileKey key() {return __base.key();}

    DPN_FileSlice * getMainSlice() {return __main_slice;}
    DArray<DPN_FileSlice*> createSlices(int slices_nb);
    DPN_FileSlice * createSlice(int slices_nb, int slice_i);
    DPN_FileSlice * createMainSlice();

    void clear();

public:

//    void setWindows(int __read, int __copy) {__read_window = __read; __copy_window = __copy;}
//    inline int readWindow() const {return __read_window;}
//    inline int copyWindow() const {return __copy_window;}
//    void setDistributable(bool s) {__dsb__available = s;}
//    void enableDistributing() {__dsb__available = true;}
//    void disbleDistributing() {__dsb__available = false;}
//    void setDistributeChannels(int channels_nb) {__dsb__max_channels = channels_nb > 0 ? channels_nb : 0;}
//    inline bool isDistributingAvailable() const {return __dsb__available;}
//    inline int distributeChannels() const {return __dsb__max_channels;}
    //-----------------------------------------------
//    bool openR();
//    bool openW();
//    bool close();
//    bool start();
//    void stop();
    //-----------------------------------------------


private:
//    bool __open(const char *openMode);
private:
    DFile __base;
private:
    bool __fast_file;
    bool __strictable;
    size_t __shift;

    DPN_FileSlice *__main_slice;
    std::map<size_t, DPN_FileSlice*> __file_map;
private:
//    std::string __original_hash;
//    std::string __temp_path;
//    DPN_FileUploadTask *__uploader;
//    DPN_AbstractConnectionsCore *__core;
//    DPN_Result __status;
//    int __read_func;
//    int __copy_window;
//    int __read_window;
//    int __read_b;
//    DPN_ExpandableBuffer __buffer;
//    size_t bytes_transported;
//    size_t __bytes2file;
//    size_t __bytes2buffer;
//    bool __dsb__available;
//    int __dsb__max_channels;
};
class DPN_FileSessionManager {
public:
    typedef DPN_FileTransportHandler* transport_t;
    typedef std::map<int, transport_t> transport_map_t;

    transport_t getSendHandler(int key) const { return __get_handler(key, send_space); }
    transport_t getReceiveHandler(int key) const { return __get_handler(key, receive_space); }

    transport_t startSend(const DFile &file) { return __insert_handler(file, send_space); }
    transport_t startReceive(const DFile &file) { return __insert_handler(file, receive_space); }
    transport_t startReceive(const DFile &file, int remoteKey) { return __insert_handler2(remoteKey, file, receive_space); }
    

    bool stopSend(const DArray<int> &keyset);
    bool stopSend(int key) { return __removeFile(key, send_space); }
    bool stopReceive(int key) { return __removeFile(key, receive_space); }
    bool stopReceive(const DArray<int> &keyset);

private:



    inline transport_t __insert_handler(const DFile &file, transport_map_t  &map) { return __insert_handler2(file.key(), file, map); }
    transport_t __insert_handler2(int key, const DFile &file, transport_map_t  &map);

    transport_t __get_handler(int key, const transport_map_t &map) const;
    bool __removeFile(int key, transport_map_t &map);

    transport_map_t send_space;
    transport_map_t receive_space;

    DPN_SimplePool<DPN_FileTransportHandler> __handlers_pool;
};

*/

/*
namespace DPN_FSPerephery {

    class FileBlock {
    public:
        enum FileBlockFlags {
            FB__ZEROBLOCK = (1 << 1)
        };
        FileBlock();
        FileBlock(size_t start, size_t size, uint8_t blockFlags = 0);
        inline size_t begin() const {return iStart;}
        inline size_t end() const {return iStart + iSize;}
        inline size_t size() const {return iSize;}
        bool merge(const FileBlock &b);
        bool hashMe(const std::string &path);

        size_t iStart;
        size_t iSize;
        uint8_t iFlags;
        std::string iHash;
    };
    typedef DArray<FileBlock> FileMap;
    struct FilePacketHeader {
        int size;
        int key;
        size_t position;
    };
    class FileSlice {
    public:
        FileSlice();
        int read();
        int write(const DPN_ExpandableBuffer &b);
        bool init(const DFile &file);
        bool init(const std::string &path, const FileBlock &block);

        bool openW();
        bool openR();
    public:
        inline DPN_ExpandableBuffer & buffer() {return wInnerBuffer;}
    private:
        int __read_instant();
        int __read_buffering();
        int __read_thread_buffering();
    private:
        size_t op__bytes_left;
        size_t op__load_last;
        size_t op__load_total;
    private:
        size_t iStart;
        size_t iSize;
        int iReadWindow;
        DPN_ExpandableBuffer wInnerBuffer;
    private:
        int iFileKey;
        std::string iPath;
        FILE *pFile;
    };
    class FileTransport {
    public:
        inline FileMap & fileMap() {return iFileMap;}
        inline const FileMap & fileMap() const {return iFileMap;}
        DFile & descriptor() {return wFileDecriptor;}

    private:
        FileMap iFileMap;
        DFile wFileDecriptor;

    };
    class FileSession {
    public:
        typedef std::map<int, FileTransport*> transport_map_t;
        FileTransport * receivingFile(int key) const;
        FileTransport * sendingFile(int key) const;


        FileTransport * startReceive(const DFile &file);
    private:
        transport_map_t iSendSpace;
        transport_map_t iReceiveSpace;
    };

    bool createFileMap(const std::string &path, int minimumZeroBlockSize, FileMap &map);

}
*/
namespace DPN_FILESYSTEM {

    class Block {
    public:
        enum BlockFlags {
            FB__ZEROBLOCK = (1 << 1)
        };
        Block();
        Block(size_t start, size_t size, uint8_t blockFlags = 0);
        inline size_t begin() const {return iStart;}
        inline size_t end() const {return iStart + iSize;}
        inline size_t size() const {return iSize;}
        bool merge(const Block &b);
        bool hashMe(const std::string &path);

        size_t iStart;
        size_t iSize;
        uint8_t iFlags;
        std::string iHash;
    };
    typedef DArray<Block> FileMap;
    std::string filemap2text(const FileMap &m);

    ///
    /// \brief createFileMap
    /// \param path
    /// \param map
    /// \return
    /// 1. Meta file
    /// 2. Prefix map
    /// 3. Create map by analysys
//    bool createFileMap( const std::string &path, FileMap &map );

    class Data {
    public:
        Data() {}
        Data( DFile file ) : iFile(file) {}
        DFile iFile;
        FileMap iFileMap;
    };
    class Interface : public __dpn_acc_interface<int, Data> {
    public:
        friend class Descriptor;
        FileMap fileMap();
        DFile file();
    private:
        bool createFileMap( );
    private:
        bool readMapFromMetafile();
        bool readMapFromPrefix();
        bool generateMap();
    };
    class Descriptor : public DWatcher<Data> {
    public:
        Descriptor() {}
        Descriptor( DFile file ) : DWatcher<Data>(true, file) {}
        bool operator==( const Interface &oi ) const {
            return data() && data() == oi.inner();
        }
    };

    class Slice {
    public:
        Slice( DFile f, Block b ) : wFile(f), iBlock(b) {}

        bool open();
        int read( DPN_ExpandableBuffer &dst );

    private:
        int iReadWindow;
        size_t op__bytes_left;
        size_t op__load_last;
        size_t op__load_total;
        FILE *pFile;
    private:
        DFile wFile;
        Block iBlock;
    };
    class Writer {
    public:
        Writer();
        Writer( DFile file );
        int write( const void *data, int size, size_t p);
    private:
        DFile wFile;
        FILE *pFile;
    };

    struct PacketHeader {
        int size;
        int key;
        size_t position;
    };
    struct PacketHeader2 {
        int key;
        size_t position;
    };

    struct __kernel__ {
        std::map<int, DFile> __receiving;
    };
    class SystemKernel : private DWatcher<__kernel__> {
    public:
        SystemKernel( bool isSource ) : DWatcher<__kernel__>(isSource) {}
        SystemKernel( const SystemKernel &o ) : DWatcher<__kernel__>(o) {}
        bool putReceivingFile( DFile file );

        DFile getReceivingFile( int key );
    };
    class IO__FILE : public DPN_IO::IOContext, public SystemKernel {
    public:
        IO__FILE( const SystemKernel &k ) : SystemKernel(k) {}
        bool send( Slice &s );
        inline int sendSessionSize() const { return aSendSession.size(); }
    public:
        virtual DPN_Result generate(DPN_ExpandableBuffer &buffer) override;
        virtual DPN_Result process(DPN_ExpandableBuffer &buffer) override;
    private:
        DArray<Slice> aSendSession;
        std::map<int, Writer> aReceiveSession;
    };

    class ChannelData : public SystemKernel, public DPN_Propagation::LinearScheme {
    public:
        ChannelData( const SystemKernel &k, __channel_mono_interface i ) : SystemKernel(k), ifLockedMono(i) {
            pIO = new IO__FILE(k);

            if( i.key() == DPN_FORWARD ) {
                setEntry( pIO );
                connect( ifLockedMono.io() );
            } else {
                setEntry( ifLockedMono.io() );
                connect( pIO );
            }
        }
        ~ChannelData() {
            if( pIO ) delete pIO;
        }
        __channel_mono_interface ifLockedMono;
        IO__FILE *pIO;
        DPN_ExpandableBuffer wContext;
    };

    class Channel : private DWatcher<ChannelData>
             {
    public:
        Channel( const SystemKernel &k, __channel_mono_interface i, DPN_ExpandableBuffer context );
        DPN_ThreadUnit * threadUnit() { return this->data(); }
    public:
        int load() const;
        bool send(Slice s );
    };
}
template <>
class dpn_mediator<DPN_FILESYSTEM::Block> {
public:
    template <class ... Args>
    static void __do(DPN_FILESYSTEM::Block &block, Args && ... a) {
        __tr::transport(block.iStart, a...);
        __tr::transport(block.iSize, a...);
        __tr::transport(block.iFlags, a...);
        __tr::transport(block.iHash, a...);
    }
    inline static void save(const DPN_FILESYSTEM::Block &block, DPN_ExpandableBuffer &b) {  __do(const_cast<DPN_FILESYSTEM::Block&>(block), b); }
    inline static void upload(DPN_FILESYSTEM::Block &block, const DPN_ExpandableBuffer &b, int &p) { __do(block, b, p); }
};



#endif // DPN_TRANSPORTHANDLER_H
