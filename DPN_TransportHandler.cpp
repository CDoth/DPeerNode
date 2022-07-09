#include "DPN_TransportHandler.h"
#include "__dpeernode_global.h"

#include "DPN_Channel.h"
using namespace DPN::Logs;

namespace DPN_FILESYSTEM {

    Block::Block() {
        iStart = 0;
        iSize = 0;
        iFlags = 0;
    }
    Block::Block(size_t start, size_t size, uint8_t blockFlags) {
        iStart = start;
        iSize = size;
        iFlags = blockFlags;
    }
    bool Block::merge(const Block &b) {
        if( iStart + iSize == b.iStart && iFlags == b.iFlags ) {
            iSize += b.iSize;
            return true;
        }
        return false;
    }
    static thread_local DPN::SHA256 __global_hashtool;
    bool Block::hashMe(const std::string &path) {
        iHash.clear();
        if( iFlags & FB__ZEROBLOCK ) return true;

        if( __global_hashtool.hash_file_segment( path, iStart, iSize ) == false ) {
            DL_ERROR(1, "can't hash block: path: [%s] start: [%d] size: [%d]",
                     path.c_str(), iStart, iSize);
            return false;
        }
        iHash = __global_hashtool.get();
        return true;
    }

    std::string createMetaPath(const std::string &filePath) {

        std::string metaFilePath = filePath;
        path_cut_last_section(metaFilePath);

        std::string name = path_get_last_section(filePath);
        name.insert(0, "~dpn_meta~");
        metaFilePath.append( name );

//        metaFilePath.append( "~dpn_meta~" );
        return metaFilePath;
    }

    /*
    FileMap DPN_FILESYSTEM::Interface::fileMap() {
        if( badInterface() ) {
            DL_ERROR(1, "Bad interface");
            return FileMap();
        }
        createFileMap();
        return inner()->iFileMap;
    }
    DFile Interface::file() {

        if( badInterface() ) {
            DL_ERROR(1, "Bad interface");
            return DFile();
        }
        return inner()->iFile;
    }
    bool Interface::createFileMap() {

        FileMap &m = inner()->iFileMap;
        DFile &file = inner()->iFile;

        m.clear();

        size_t s = get_file_size( file.path() );
        if( s == 0 ) {
            m.append( Block( 0, file.size() ) );
            return true;
        }
        if( readMapFromMetafile() ) return true;
        if( readMapFromPrefix() ) return true;
        if( generateMap() ) return true;

        m.append( Block( 0, file.size() ) );
        return true;
    }
    bool Interface::readMapFromMetafile() {

        std::string metaFilePath = createMetaPath( inner()->iFile.path() );

        size_t s = get_file_size( metaFilePath );
        if( s == 0 ) {
            return false;
        }
        FILE *mf = fopen( metaFilePath.c_str(), "rb");

        DPN_ExpandableBuffer b;
        b.readData( mf, s );

        __hl_item<FileMap> i;
        i.roughLoad( b );
        inner()->iFileMap = i.get();

        return true;
    }
    bool Interface::readMapFromPrefix() {
        return false;
    }
    bool Interface::generateMap() {
        return false;
    }
    */
    //==========================================

    Channel::Channel(const SystemKernel &k, __channel_mono_interface i, DPN_ExpandableBuffer context) : DPN::Thread::ThreadUnitWrapper< ChannelData >(true, k, i ) {
        data()->wContext = context;
    }
    Channel::~Channel() {

    }
    void Channel::stop() {
//        if( isCreatedObject() ) {
//            data()->block();
//            data()->stop();
//        }
    }

    int Channel::load() const {
        if( isEmptyObject() ) {
            DL_ERROR(1, "empty watcher");
            return 0;
        }
        return data()->pIO->sendSessionSize();
    }
    bool Channel::send( Slice s ) {
        if( isEmptyObject() ) {
            DL_ERROR(1, "Empty watcher");
            return false;
        }
        if( data()->pIO == nullptr ) {
            DL_BADPOINTER(1, "io");
            return false;
        }
        DL_INFO(1, "IO__FILE: [%p]", data()->pIO);

        return data()->pIO->send( s );
    }
    std::string filemap2text(const FileMap &m) {
        std::string text("filemap: {");
        FOR_VALUE( m.size(), i ) {
            text.append( "[" );
            text.append( std::to_string( m[i].begin() ));
            text.append( "-" );
            text.append( std::to_string( m[i].end() ));
            text.append( "("); text.append( std::to_string( m[i].iFlags)); text.append(")");
            text.append( "]" );
        }
        text.append("}");
        return text;
    }
    
    bool IO__FILE::send(Slice &s) {

        DL_INFO(1, "append slice to IO__FILE [%p]. send session size: [%d]",
                this, aSendSession.size());

        aSendSession.append( s );
        return true;
    }
    bool IO__FILE::stop() {
        aSendSession.clear();
        aReceiveSession.clear();
        return true;
    }

    DPN_Result IO__FILE::generate(DPN_ExpandableBuffer &buffer) {
        
        DL_INFO(1, "IO__FILE [%p] create file packets... session size: [%d]",
                this, aSendSession.size() );

        if( aSendSession.size() ) {
            Slice &s = aSendSession[0];

            DL_INFO( 1, "sending slice: name: [%s] size: [%d] begin: [%d] end: [%d]",
                     s.file().name_c(), s.file().size(), s.block().begin(), s.block().end() );
        }

//        Slice &s = aSendSession[0];


//        buffer.dropTo( sizeof(PacketHeader) ) ;
//        PacketHeader *h = reinterpret_cast<PacketHeader*>(buffer.getData());

//        h->size = s.read( buffer );
//        h->key = -1;
//        h->position = 0;

        return DPN_SUCCESS;

    }
    DPN_Result IO__FILE::process(DPN_ExpandableBuffer &buffer) {


        if( receivingSessionSize() ) {
            DL_INFO(1, "IO__FILE [%p] parse file packets... session size: [%d]",
                    this, receivingSessionSize());
        }
//        const PacketHeader *h = reinterpret_cast<const PacketHeader*>(buffer.getData());

//        if( aReceiveSession.find( h->key) == aReceiveSession.end() ) {
//            DFile f = getReceivingFile( h->key );
//            if( BADFILE(f) ) {
//                DL_ERROR(1, "No receiving file with key [%d]", h->key );
//                return DPN_FAIL;
//            }
//            aReceiveSession[ h->key ] = Writer( f );
//        }
//        Writer &w = aReceiveSession[h->key];
//        w.write( buffer.getData(), h->size, h->position );
        return DPN_SUCCESS;
    }

    bool Slice::open()
    {

    }

    int Slice::read(DPN_ExpandableBuffer &dst) {

        if( iReadWindow == 0 ) return 0;

        int read_now = (size_t)iReadWindow > op__bytes_left ? op__bytes_left : iReadWindow;
        op__load_last = dst.readData( pFile, read_now );
//        FilePacketHeader *h = reinterpret_cast<FilePacketHeader*>(wInnerBuffer.getData());
//        h->size = op__load_last;
//        h->key = iFileKey;
//        h->position = iStart + op__load_total;

        op__bytes_left -= op__load_last;
        op__load_total += op__load_last;
        return op__load_last;
    }

    Writer::Writer() {
        pFile = nullptr;
    }
    Writer::Writer(DFile file) {
        wFile = file;
        pFile =  nullptr;
    }
    int Writer::write(const void *data, int size, size_t p) {

    }

    bool SystemKernel::putReceivingFile(DFile file) {
        if( isEmptyObject() ) {
            DL_ERROR(1, "Empty watcher");
            return false;
        }
        if( data()->__receiving.find( file.key() ) != data()->__receiving.end() ) {
            DL_INFO(1, "File with key [%d] already in receiving map", file.key());
            return true;
        }
        data()->__receiving[file.key()] = file;
        return true;
    }

    DFile SystemKernel::getReceivingFile(int key) {
        if( isEmptyObject() ) {
            DL_ERROR(1, "Epmty watcher");
            return DFile();
        }
        auto f = data()->__receiving.find( key );
        if( f == data()->__receiving.end() ) return DFile();
        return f->second;
    }
    int SystemKernel::receivingSessionSize() const {
        if( isEmptyObject() ) {
            DL_ERROR(1, "Epmty watcher");
            return -1;
        }
        return data()->__receiving.size();
    }
    bool SystemKernel::kernelStop() {
        data()->__receiving.clear();
        return true;
    }




}

