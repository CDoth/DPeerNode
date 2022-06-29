#include "DPN_TransportHandler.h"
#include "__dpeernode_global.h"

#include "DPN_Channel.h"
using namespace DPeerNodeSpace;
/*
DPN_FileSlice::DPN_FileSlice() {
    __clear();
}
DPN_FileSlice::~DPN_FileSlice() {
    close();
    __clear();
}
void DPN_FileSlice::setReadWindow(int rw) {
    __read_window = rw > 0 ? rw : 0;
}
bool DPN_FileSlice::openR() {
    return __open("rb");
}
bool DPN_FileSlice::openW() {

    path_create_dir_tree(__file.path());

    return __start ? __open("ab") : __open("wb");
}
void DPN_FileSlice::stop() {
    close();
    __transmit_range.fixLast();
    __speed_tester.clear();
    __progress = 0.0f;
    __speed = 0.0f;
    __test_portion = 0;

    __load_last = 0;
    __load_total = 0;
    __transmit_last = 0;
    __transmit_total = 0;
}
int DPN_FileSlice::write(const DPN_ExpandableBuffer &b, int size) {
    return write(b.getData(), size);
}
int DPN_FileSlice::write(const void *data, int size) {
    __load_last = fwrite(data, 1, size, __source);
    __load_total += __load_last;
    return __load_last;
}
bool DPN_FileSlice::transportIt(int bytes) {


    size_t temp = __transmit_total + bytes;
    if(temp > __size) {

        DL_ERROR(1, "transported bytes more than size: bytes: [%d] total: [%d] size: [%d]",
                 bytes, __transmit_total, __size
                 );
        return false;
    }
    __transmit_last = bytes;
    __transmit_total = temp;

    __progress = (float)__transmit_total * 100.0f / __size;

    if( __speed_tester.isOpen() ) {
        float s = __speed_tester.seconds();
        if( s > 0 ) {
            __speed = __test_portion / s;
            __speed_tester.clear();
            __test_portion = 0;
        }
    } else {
        __speed_tester.fixBegin();
        __test_portion = 0;
    }
    __test_portion += __transmit_last;


    return true;
}
bool DPN_FileSlice::init(DPN_FileTransportHandler *desc, size_t start, size_t size) {
    if( desc == nullptr ) {
        DL_BADPOINTER(1, "desctiptor");
        return false;
    }
    if( BADFILE(desc->base()) || start + size > desc->size() ) {
        DL_BADVALUE(1, "file [%d] start: [%d] size: [%d] file size: [%d]", bool(desc->base()), start, size, desc->base().size());
        return false;
    }
    __desc = desc;
    __start = start;
    __size = size;
    __file = __desc->base();
    return true;
}
bool DPN_FileSlice::close() {
    if(__source) {
        int r = 0;
        if( (r = fflush(__source)) ) {
            DL_INFO(1, "fflush: [%d]", r);
        }
        if( (r = fclose(__source)) ) {
            DL_FUNCFAIL(1, "fclose: [%d]", r);
        }
        __source = nullptr;
    }
    return true;
}
bool DPN_FileSlice::start() {
    __progress = 0;
    __speed = 0;
    __speed_tester.clear();
    __test_portion = 0;

    __transmit_range.clear();
    __transmit_range.fixBegin();
    return true;
}
bool DPN_FileSlice::__open(const char *mode) {

    if( __source ) {
        DL_ERROR(1, "file already open");
        return false;
    }
    const char *path = __file.path_c();
    if( path == nullptr ) {
        DL_BADPOINTER(1, "path");
        return false;
    }
    if( (__source = fopen(path, mode)) == nullptr ) {
        DL_FUNCFAIL(1, "fopen. path: [%s] mode: [%s]", path, mode);
        return false;
    }

    int r;
    if( (r = fseek(__source, __start, SEEK_SET)) ) {
        DL_FUNCFAIL(1, "fseek: [%d] file: [%p] size: [%d] start: [%d]",
                    r, (void*)__source, __size, __start
                    );
        return false;
    }

    __left = __size;

    return true;
}
void DPN_FileSlice::__clear() {
    __read_window = 0;

    __progress = 0.0;
    __speed = 0.0f;
    __test_portion = 0.0f;
    __start = 0;
    __size = 0;
    __left = 0;
    __load_last = 0;
    __load_total = 0;
    __transmit_last = 0;
    __transmit_total = 0;
    __source = nullptr;
    __desc = nullptr;
}
int DPN_FileSlice::__read(DPN_ExpandableBuffer &b) {
    return __read__default(b);
}
int DPN_FileSlice::__read__default(DPN_ExpandableBuffer &b) {

    if( __read_window == 0 ) {
        DL_BADVALUE(1, "Zero read window");
        return -1;
    }
    int read_now = (size_t)__read_window > __left ? __left : __read_window;

    __load_last = b.readData(__source, read_now);
    __left -= __load_last;
    __load_total += __load_last;
    return __load_last;
}
int DPN_FileSlice::__read__buffering(DPN_ExpandableBuffer &b) {

    // test code
    return -1;

    if( __load_last == 0 ) {
        if( (__load_last = __inner_buffer.readData(__source, __read_window)) < 1 ) {
            return __load_last;
        }
    }
    size_t __copy_window = 0;
    int copy_now = __copy_window > __load_last ? __load_last : __copy_window;
    b.append(__inner_buffer.getData() + (__read_window - __load_last), copy_now);
    __load_last -= copy_now;
    return copy_now;
}
int DPN_FileSlice::__read__thread_buffering(DPN_ExpandableBuffer &b) {

    // test code
    return -1;

//    if( __uploader->__status == DPN_REPEAT ) {
//        __uploader->__status = DPN_KEEP;
//        __core->pushTask(__uploader);
//    }
//    if( __read_b == 0 && __uploader->__status == DPN_SUCCESS ) {
//        __uploader->__status = DPN_REPEAT;
//        std::swap(__buffer, __uploader->__buffer);
//        __read_b = __uploader->__read;
//    } else {
//        __status = DPN_REPEAT;
//        return 0;
//    }

//    int copy_now = __copy_window > __read_b ? __read_b : __copy_window;
//    b.append(__buffer.getData() + (__read_window - __read_b), copy_now);
//    __read_b -= copy_now;

//    __bytes2buffer = copy_now;
//     __status = DPN_SUCCESS;
//    return copy_now;
}
//------------------------------------------------------------------------ DPN_FileTransportHandler:
DPN_FileTransportHandler::DPN_FileTransportHandler() {

    __main_slice = nullptr;
    __fast_file = false;
    __strictable = false;
    __shift = 0;
}
DArray<DPN_FileSlice *> DPN_FileTransportHandler::createSlices(int slices_nb) {

    DArray<DPN_FileSlice*> ret;
    return ret;

    if( slices_nb < 1 ) {
        DL_BADVALUE(1, "slices_nb: [%d]", slices_nb);
        return ret;
    }

    size_t fileSize = size() - __shift;
    if( fileSize == 0 ) {
        DL_ERROR(1, "Zero file size");
        return ret;
    }
    if( fileSize < (size_t)slices_nb ) {
        DL_ERROR(1, "File size [%d] less than requered slice number [%d]", fileSize, slices_nb);
        return ret;
    }

    size_t pos = __shift;
    size_t sliceSize = fileSize / slices_nb;
    size_t rem = fileSize % slices_nb;

    FOR_VALUE( slices_nb, i ) {

        DPN_FileSlice *slice = new DPN_FileSlice;

        slice->__start = pos;
        slice->__size = (i == slices_nb-1) ? sliceSize + rem : sliceSize;
        slice->__desc = this;

//        if( slice->open(__base.path_c()) == false ) {
//            DL_FUNCFAIL(1, "open slice: [%d] path: [%s]", i, __base.path_c());
//            goto fail;
//        }

        pos += sliceSize;
        ret.push_back(slice);
    }

//    __slices.appendArray(ret);
    return ret;
fail:
    FOR_VALUE( ret.size(), i ) {
        if( ret[i] ) delete ret[i];
    }
    ret.clear();
    return ret;
}
DPN_FileSlice *DPN_FileTransportHandler::createSlice(int slices_nb, int slice_i) {

    if( slices_nb < 1 || slice_i < 0 || slice_i >= slices_nb ) {
        DL_BADVALUE(1, "slices_nb: [%d] slice_i: [%d]", slices_nb, slice_i);
        return nullptr;
    }

    size_t fileSize = size() - __shift;
    size_t sliceSize = fileSize / slices_nb;
    size_t start = __shift + sliceSize * slice_i;
    size_t rem = fileSize % slices_nb;
    sliceSize = (slice_i == slices_nb - 1) ? sliceSize + rem : sliceSize;

    if( __file_map.find(start) != __file_map.end() ) {
        DL_ERROR(1, "Slice in pos [%d] already exist", start);
        return nullptr;
    }

    size_t p = 0;
    DPN_FileSlice *prev = nullptr;
    DPN_FileSlice *next = nullptr;


    size_t diff = 0;
    for( auto it: __file_map ) {

        p = it.first;

        if( p > start ) {
            diff = p - start;
            if( next == nullptr || diff < next->__start - start ) next = it.second;
        } else {
            diff = start - p;
            if( prev == nullptr || diff < start - prev->__start ) prev = it.second;
          }
    }
    if( prev && (prev->__start + prev->__size) > start) {
        DL_ERROR(1, "Requeried slice collising with previous slice: begin: [%d] size: [%d] end: [%d] req slice begin: [%d]",
                 prev->__start, prev->__size, prev->__start + prev->__size, start
                 );
        return nullptr;
    }
    if( next && (start + sliceSize) > next->__start ) {
        DL_ERROR(1, "Requeried slice collising with next slice: begin: [%d] req slice begin: [%d] size: [%d] end: [%d]",
                 next->__start, start, sliceSize, start + sliceSize
                 );
        return nullptr;
    }

    DPN_FileSlice *ret = nullptr;


    if( fileSize == 0 ) {
        DL_ERROR(1, "Zero file size");
        return nullptr;
    }
    if( fileSize < (size_t)slices_nb ) {
        DL_ERROR(1, "File size [%d] less than requered slice number [%d]", fileSize, slices_nb);
        return nullptr;
    }

    ret = new DPN_FileSlice;
    if( ret->init(this, start, sliceSize) == false ) {
        DL_FUNCFAIL(1, "init");
        delete ret;
        return nullptr;
    }

    __file_map[start] = ret;

    return ret;
}
DPN_FileSlice *DPN_FileTransportHandler::createMainSlice() {
    if( __main_slice ) {
        DL_ERROR(1, "Main slice already exist");
        return nullptr;
    }
    if( (__main_slice = createSlice(1, 0)) == nullptr ) {
        DL_FUNCFAIL(1, "createSlice");
        return nullptr;
    }
    return __main_slice;
}
void DPN_FileTransportHandler::clear() {

    for( auto it: __file_map ) {
        DPN_FileSlice *s = it.second;
        if( s ) delete s;
    }
    __file_map.clear();
    __main_slice = nullptr;
}
*/

/*
bool DPN_FileTransportHandler::openW() {

    path_create_dir_tree(__base.path());

    return __shift ? __open("ab") : __open("wb");
}
bool DPN_FileTransportHandler::close() {
    if(__file) {
        int r = 0;
        if( (r = fflush(__file)) ) {
            DL_INFO(1, "fflush: [%d]", r);
        }
        if( (r = fclose(__file)) ) {
            DL_FUNCFAIL(1, "fclose: [%d]", r);
        }
        __file = nullptr;
    }
    return true;
}
bool DPN_FileTransportHandler::__open(const char *openMode)  {
    if(__file) {
        DL_ERROR(1, "File already open: [%p]", __file);
        return false;
    }
    if(BADFILE(__base)) {
        DL_ERROR(1, "No file");
        return false;
    }

    DL_INFO(1, "File path: [%s] mode: [%s]",
            __base.path_c(), openMode);

    if( (__file = fopen(__base.path_c(), openMode)) == nullptr ) {
        DL_FUNCFAIL(1, "fopen");
        return false;
    }

    return true;
}

bool DPN_FileTransportHandler::openR() {

      if( __open("rb") == false ) {
          DL_FUNCFAIL(1, "__open");
          return false;
      }
      if(__shift > 0 && __shift < __base.size()) {

          int r;
          if( (r = fseek(__file, __shift, SEEK_SET)) ) {
              DL_FUNCFAIL(1, "fseek: [%d] file: [%p] size: [%d] shift: [%d]",
                          r, (void*)__file, __base.size(), __shift
                          );
              return false;
          }

      }
      return true;
}
bool DPN_FileTransportHandler::start() {
    if(play) {
        DL_ERROR(1, "File already in tranmit");
        return false;
    }
//    if(__file == nullptr) {
//        DL_BADPOINTER(1, "File not open");
//        return false;
//    }

    if( __read_window < __copy_window ) {
        __read_window = __copy_window;
    }
    if( __read_window == 0 ) {
        __read_window = DPN_DEFAULT_READ_WINDOW;
    }
    if( __copy_window == 0 ) {
        __copy_window = __read_window;
    }

    currentRange.clear();
    currentRange.fixBegin();

    __read_func = 1;


    bytes_transported = 0;



    __bytes2file = 0;
    __bytes2buffer = 0;

    play = true;
    return play;
}
void DPN_FileTransportHandler::stop() {
    close();

    currentRange.fixLast();

    bytes_transported = 0;
    play = false;

    __bytes2file = 0;
    __bytes2buffer = 0;
}
bool DPN_FileTransportHandler::transportIt(int bytes)  {

//    DL_INFO(1, "key: [%d] transport: [%d] size: [%d]",
//            __base.key(), bytes, size());

    size_t temp = bytes_transported + bytes;
    if(temp > size()) {

        DL_ERROR(1, "transported bytes more than size: bytes: [%d] total: [%d] size: [%d]",
                 bytes, bytes_transported, size()
                 );
        return false;
    }
    bytes_transported = temp;

    return true;
}
void DPN_FileTransportHandler::clearTransportProgress()  {
    bytes_transported = 0;
    play = false;

    __bytes2file = 0;
    __bytes2buffer = 0;
}
void DPN_FileTransportHandler::clear()  {

    close();

    __base.clearObject();
    bytes_transported = 0;
    play = false;

    __bytes2file = 0;
    __bytes2buffer = 0;
}
*/
//------------------------------------------------------------------------ DPN_FileTransportGroup:
/*
void DPN_FileTransportGroup::addHandler(DPN_FileTransportHandler *handler) {
    if(handler) {
        group.push_back(handler);
    }
}
int DPN_FileTransportGroup::size() const {
    return group.size();
}
void DPN_FileTransportGroup::changeActual() {

    // Here group could manage and tune
    // frequency of file's packets.

    // You could set rule(s)
    // which will determine how much packets
    // will transmit per each file.



    // Default behaivor (1 packet per file):
    if(group.empty() || ++actual_pos == group.size()) {
        actual_pos = 0;
    }

//    DL_INFO(1, "actual pos: [%d]", actual_pos);
}
DPN_FileTransportGroup::DPN_FileTransportGroup() {
    actual_pos = 0;
}
DArray<int> DPN_FileTransportGroup::keyset() const {

    DArray<int> set;

    FOR_VALUE(group.size(), i) {
        set.push_back(group[i]->key());
    }

    return set;
}
bool DPN_FileTransportGroup::startSend() {

    FOR_VALUE(group.size(), i) {

        if( (group[i]->openR() == false) ) {
//            return false;
        }
        if( (group[i]->start() == false) ) {
//            return false;
        }
    }

    return true;
}
void DPN_FileTransportGroup::setBuffer(const DPN_ExpandableBuffer &buffer) {
    FOR_VALUE(group.size(), i) {
        group[i]->setBuffer(buffer);
    }
}
DPN_FileTransportHandler *DPN_FileTransportGroup::actual() const {

    if(group.empty()) {
        return nullptr;
    }
    auto h = group[actual_pos];

    return h;
}
bool DPN_FileTransportGroup::transportIt(int bytes) {

    if(!group.empty()) {

        DPN_FileTransportHandler *h = group[actual_pos];


        h->transportIt(bytes);

        if(h->isTransported()) {
            h->stop();

            group.removeByIndex(actual_pos);
            if(actual_pos == group.size()) {
                actual_pos = 0;
            }
            return true;
        } else {
            changeActual();
        }
    }
    return false;
}
*/
//------------------------------------------------------------------------ DPN_FileSessionManager:
/*
bool DPN_FileSessionManager::stopSend(const DArray<int> &keyset) {

    FOR_VALUE(keyset.size(), i) {
        stopSend(keyset[i]);
    }

    return true;
}
bool DPN_FileSessionManager::stopReceive(const DArray<int> &keyset) {
    FOR_VALUE(keyset.size(), i) {
        stopReceive(keyset[i]);
    }
    return true;
}
DPN_FileTransportHandler *DPN_FileSessionManager::__insert_handler2(int key, const DFile &file, DPN_FileSessionManager::transport_map_t &map)  {

    if(BADFILE(file)) {
        DL_BADPOINTER(1, "File descriptor");
        return nullptr;
    }
    if( map.find(key) != map.end()) {
        // File already in transport state
        // check details..
    } else {

        DPN_FileTransportHandler *handler = __handlers_pool.get();

        if(handler == nullptr) {
            DL_BADPOINTER(1, "Transport file handler");
            return nullptr;
        }
        handler->setBase(file);

        map[key] = handler;


        return handler;
    }

    return nullptr;
}
DPN_FileSessionManager::transport_t DPN_FileSessionManager::__get_handler(int key, const DPN_FileSessionManager::transport_map_t &map) const  {


    auto f = map.find(key);
    if(f != map.end()) {
        return f->second;
    }
    return nullptr;
}
bool DPN_FileSessionManager::__removeFile(int key, DPN_FileSessionManager::transport_map_t &map)  {

    auto f = map.find(key);
    if(f == map.end()) {
        DL_BADVALUE(1, "key: [%d]", key);
        return false;
    }
    DPN_FileTransportHandler *h = f->second;
    DL_INFO(1, "remove file handler from map: [%p] key: [%d]", h, key);
    h->clear();
    __handlers_pool.set(h);

    bool r = map.erase(key);
    return r;
}
DPN_FileUploadTask::DPN_FileUploadTask() {
    __file = nullptr;
    __read_now = 0;
    __read = 0;
    __status = DPN_REPEAT;
}
DPN_Result DPN_FileUploadTask::proc() {

    if( (__read = __buffer.readData(__file, __read_now)) < 0 ) {
        __status = DPN_FAIL;
        return DPN_FAIL;
    }
    __status = DPN_SUCCESS;
    return DPN_SUCCESS;
}
*/
//===============================================================

/*
namespace DPN_FSPerephery {

FileSlice::FileSlice()
{

}

int FileSlice::read() {
        return __read_instant();
    }
    int FileSlice::write(const DPN_ExpandableBuffer &b) {
        if( b.size() < (int)sizeof(FilePacketHeader) + 1) {
            DL_BADVALUE(1, "buffer size: [%d]", b.size());
            return -1;
        }
        const FilePacketHeader *header = reinterpret_cast<const FilePacketHeader*>(b.getData());
        if( header->size < 1 || header->size > (b.size() - (int)sizeof(FilePacketHeader)) ) {
            DL_BADVALUE(1, "packet size: [%d] buffer size: [%d]", header->size, b.size());
            return -1;
        }
        if( header->position > iStart || header->position + header->size > iStart + iSize ) {
            DL_BADVALUE(1, "position: [%d] size: [%d] file: start: [%d] size: [%d]",
                        header->position, header->size, iStart, iSize);
            return -1;
        }
        if( pFile == nullptr ) {
            DL_BADPOINTER(1, "file");
            return -1;
        }
        const void *data = header + 1;
        fseek( pFile, header->position, SEEK_SET );
        op__load_last = fwrite( data, 1, header->size, pFile );
        return op__load_last;
    }
    bool FileSlice::init(const DFile &file) {
        iPath = file.path();
        iStart = 0;
        iSize = file.size();
        return true;
    }
    bool FileSlice::init(const std::string &path, const FileBlock &block) {
        iPath = path;
        iStart = block.iStart;
        iSize = block.iSize;
        return true;
    }
    bool FileSlice::openW() {
        if( pFile ) {
            DL_ERROR(1, "File already open");
            return false;
        }
        if( (pFile = fopen(iPath.c_str(), "a+b")) == nullptr ) {
            return false;
        }
        return true;
    }
    bool FileSlice::openR() {
        pFile = fopen(iPath.c_str(), "rb");
        fseek( pFile, iStart, SEEK_SET );
        return true;
    }
    int FileSlice::__read_instant() {

        if( iReadWindow == 0 ) return 0;

        int read_now = (size_t)iReadWindow > op__bytes_left ? op__bytes_left : iReadWindow;

        wInnerBuffer.dropTo( sizeof(FilePacketHeader) );
        op__load_last = wInnerBuffer.readData( pFile, read_now );
        FilePacketHeader *h = reinterpret_cast<FilePacketHeader*>(wInnerBuffer.getData());
        h->size = op__load_last;
        h->key = iFileKey;
        h->position = iStart + op__load_total;

        op__bytes_left -= op__load_last;
        op__load_total += op__load_last;
        return op__load_last;
    }
    int FileSlice::__read_buffering() {
        return 0;
    }
    int FileSlice::__read_thread_buffering() {
        return 0;
    }

    static thread_local DPN_ExpandableBuffer __global_file_buffer;
#define GLOBAL_BUFFER_START_SIZE (1024 * 1024)

    std::string createMetaPath(const std::string &filePath) {
        std::string metaFilePath = filePath;
        path_cut_last_section(metaFilePath);
        std::string name = path_get_last_section(filePath);
        name.insert(0, "~dpn_meta~");
        metaFilePath.append( name );
        return  metaFilePath;
    }
    bool writeFileMap(const std::string &filePath, const FileMap &map) {
        std::string metaFilePath = createMetaPath( filePath );
        FILE *mf = nullptr;
        if( (mf = fopen( metaFilePath.c_str(), "wb")) == nullptr ) {
            DL_ERROR(1, "Can't create meta file for file: [%s]", filePath.c_str());
            return false;
        }

        __hl_item<FileMap> i;
        i = map;

        fwrite( i.buffer().getData(), 1, i.buffer().size(), mf );

        return true;
    }
    bool readFileMap(const std::string &filePath, FileMap &map) {
        std::string metaFilePath = createMetaPath( filePath );

        size_t s = get_file_size( metaFilePath );
        if( s == 0 ) {
            return false;
        }
        FILE *mf = fopen( metaFilePath.c_str(), "rb");

        DPN_ExpandableBuffer b;
        b.readData( mf, s );

        __hl_item<FileMap> i;
        i.roughLoad( b );
        map = i.get();

        return true;
    }
    inline bool __is_zero_block(const uint64_t *data, int data_size) {
        auto e = data + data_size;
        while( data != e ) if( *data++ ) return false;
        return true;
    }
    FileBlock __find_zero_block(const uint8_t *data, int data_size, int minimum) {
//        if( data == nullptr || data_size < 1 || block_size < 1 ) {

//            DL_BADVALUE(1, "data: [%p] data_size: [%d] block_size: [%d]", data, data_size, block_size);
//            return -1;
//        }
//        block_size = block_size > data_size ? data_size : block_size;
//        const uint64_t *block_end = data + block_size;
//        const uint64_t *data_end = data + data_size;
//        int pos = 0;
//        while( block_end != data_end ) {
//            if( *block_end == 0 && *(block_end - block_size) == 0 ) {
//                if( __is_zero_block( block_end - block_size, block_size) ) return pos;
//            }
//            ++pos;
//        }
//        return -1;

        int block_end = minimum;
        int __left = -1;
        int __right = 0;


        while( true ) {

            if( *(data + block_end) == 0 ) {

                __left = block_end - 1;
                __right = block_end + 1;
                while( !(*(data + __left)) ) --__left;
                while( !(*(data + __right)) ) ++__right;



            }

            block_end += minimum;
            if( block_end > data_size ) break;
        }

    }
    bool createFileMap(const std::string &path, int minimumZeroBlockSize, FileMap &map) {
        if( path.empty() ) {
            DL_ERROR(1, "Empty path");
            return false;
        }
        const size_t fileSize = get_file_size( path );
        if( fileSize == 0 ) {
            map.clear();
            return true;
        }
        if( fileSize < (size_t)__global_file_buffer.size() ) {
            FileBlock single(0, fileSize);
            map.append(single);
            return true;
        }
        FILE *file = fopen(path.c_str(), "rb");
        if( file == nullptr ) {
            DL_ERROR(1, "Can't open file [%s]", path.c_str());
            return false;
        }

        if( __global_file_buffer.size() == 0  ) {
            __global_file_buffer.reserve( GLOBAL_BUFFER_START_SIZE );
        }

        const size_t blockSize = __global_file_buffer.size();

        size_t bytesLeft = fileSize;
        while( bytesLeft > (size_t)minimumZeroBlockSize ) {

            int read_now = blockSize > bytesLeft ? bytesLeft : blockSize;

            fread( __global_file_buffer.getData(), 1, read_now, file );



//            int p = 0;
//            FileBlock block;
//            if( (p = __find_zero_block( __global_file_buffer.getData(), read_now, blockSize )) > -1 ) {
//                block = FileBlock(p, minimumZeroBlockSize, FileBlock::FB__ZEROBLOCK);
//            } else {
//                block = FileBlock(fileSize - bytesLeft, read_now);
//            }
//            if( map.empty() || !map.back().merge( block )) {
//                map.append( block );
//            }



            bytesLeft -= read_now;
        }
        if( bytesLeft ) {
            FileBlock block(fileSize - bytesLeft, bytesLeft);
            map.append(block);
        }
        FOR_VALUE( map.size(), i ) map[i].hashMe( path );



        fclose(file);
        return true;
    }

    FileTransport *FileSession::receivingFile(int key) const
    {

    }

    FileTransport *FileSession::sendingFile(int key) const
    {

    }

    FileTransport *FileSession::startReceive(const DFile &file) {
        if( iReceiveSpace.find(file.key()) != iReceiveSpace.end() ) {
            DL_ERROR(1, "File already in receive session");
            return nullptr;
        }
        FileMap map;

        if( readFileMap( file.path(), map) == false ) {
            if( createFileMap( file.path(), 1024, map ) == false ) {
                DL_FUNCFAIL(1, "createFileMap");
                return nullptr;
            }
        }

        FileTransport *t = new FileTransport;
        t->fileMap() = map;
        iReceiveSpace[file.key()] = t;
        return t;
    }
    FileBlock::FileBlock()
    {

    }

    FileBlock::FileBlock(size_t start, size_t size, uint8_t blockFlags)
    {

    }

    bool FileBlock::merge(const FileBlock &b) {
        if( iStart + iSize == b.iStart && iFlags == b.iFlags ) {
            iSize += b.iSize;
            return true;
        }
        return false;
    }
    static thread_local DPN_SHA256 __global_hashtool;
    bool FileBlock::hashMe(const std::string &path) {
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

}
*/


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
    static thread_local DPN_SHA256 __global_hashtool;
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
    //==========================================

    Channel::Channel(const SystemKernel &k, __channel_mono_interface i, DPN_ExpandableBuffer context) : DWatcher<ChannelData>(true, k, i ) {
        data()->wContext = context;
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

    DPN_Result IO__FILE::generate(DPN_ExpandableBuffer &buffer) {
        
        Slice &s = aSendSession[0];


        buffer.dropTo( sizeof(PacketHeader) ) ;
        PacketHeader *h = reinterpret_cast<PacketHeader*>(buffer.getData());

        h->size = s.read( buffer );
        h->key = -1;
        h->position = 0;

        return DPN_SUCCESS;

    }
    DPN_Result IO__FILE::process(DPN_ExpandableBuffer &buffer) {

        const PacketHeader *h = reinterpret_cast<const PacketHeader*>(buffer.getData());

        if( aReceiveSession.find( h->key) == aReceiveSession.end() ) {
            DFile f = getReceivingFile( h->key );
            if( BADFILE(f) ) {
                DL_ERROR(1, "No receiving file with key [%d]", h->key );
                return DPN_FAIL;
            }
            aReceiveSession[ h->key ] = Writer( f );
        }
        Writer &w = aReceiveSession[h->key];
        w.write( buffer.getData(), h->size, h->position );
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




}

