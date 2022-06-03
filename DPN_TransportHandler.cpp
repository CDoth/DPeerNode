#include "DPN_TransportHandler.h"
#include "__dpeernode_global.h"
using namespace DPeerNodeSpace;

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
DPN_FileTransportHandler *DPN_FileSessionManager::__get_handler(int key, const DPN_FileSessionManager::transport_map_t &map) const  {


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
    __status = DPN_KEEP;
}
DPN_Result DPN_FileUploadTask::proc() {

    if( (__read = __buffer.readData(__file, __read_now)) < 0 ) {
        __status = DPN_FAIL;
        return DPN_FAIL;
    }
    __status = DPN_SUCCESS;
    return DPN_SUCCESS;
}


