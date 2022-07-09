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


    class Data {
    public:
        Data() {}
        Data( DFile file ) : iFile(file) {}
        DFile iFile;
        FileMap iFileMap;
    };
    class Interface : public DPN::Interface::InterfaceReference<Data> {
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
    class Descriptor : public DPN::Interface::DataReference<Data> {
    public:
        Descriptor() {}
        Descriptor( DFile file ) : DPN::Interface::DataReference<Data>(true, file) {}
        bool operator==( const Interface &oi ) const {
            return data() && data() == oi.inner();
        }
        Interface getFileInterface() {
            Interface ii;
            auto w = W.getInterface( *this );
//            ii.moveFrom( w );
            return ii;
        }
    private:
        DPN::Interface::InterfaceCenterReference<Data> W;
    };

    class Slice {
    public:
        Slice( DFile f, Block b ) : wFile(f), iBlock(b) {}

        bool open();
        int read( DPN_ExpandableBuffer &dst );

        inline const Block & block() const { return iBlock; }
        inline const DFile & file() const { return wFile; }

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
        int receivingSessionSize() const;
        bool kernelStop();
    };
    class IO__FILE : public DPN::IO::IOContext, public SystemKernel {
    public:
        IO__FILE( const SystemKernel &k ) : SystemKernel(k) {}
        bool send( Slice &s );
        inline int sendSessionSize() const { return aSendSession.size(); }

        bool stop();
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

//            if( i.key() == DPN::FORWARD ) {
//                setEntry( pIO );
//                connect( ifLockedMono.io() );
//            } else {
//                setEntry( ifLockedMono.io() );
//                connect( pIO );
//            }
        }
        ~ChannelData() {
            if( pIO ) delete pIO;
        }
        void stop() {
//            ifLockedMono.close();
            pIO->stop();

        }
        __channel_mono_interface ifLockedMono;
        IO__FILE *pIO;
        DPN_ExpandableBuffer wContext;
    };


    class Channel
            : DPN::Thread::ThreadUnitWrapper< ChannelData >
             {
    public:
        Channel( const SystemKernel &k, __channel_mono_interface i, DPN_ExpandableBuffer context );
        ~Channel();
        DPN::Thread::ThreadUnit threadUnit() { return this->unit(); }
    public:
        void stop();
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
