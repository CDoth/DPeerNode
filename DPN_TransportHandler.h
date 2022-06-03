#ifndef DPN_TRANSPORTHANDLER_H
#define DPN_TRANSPORTHANDLER_H
#include <DArray.h>
#include <DDirReader.h>


#include "DPN_Buffers.h"

#define DPN_DEFAULT_READ_WINDOW (1024)



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

    /*
    bool transportIt(int bytes);

    inline int file2buffer(DPN_ExpandableBuffer &buffer) {
        return buffer.readData(__file);
    }
    inline int file2buffer(DPN_ExpandableBuffer &buffer, int size) {
        return buffer.readData(__file, size);
    }
    inline int buffer2file(const DPN_ExpandableBuffer &buffer, size_t bytes) {
        return buffer.writeData(__file, bytes);
    }
    inline int data2file(const void *data, size_t bytes) {
        int w = fwrite(data, 1, bytes, __file);
        __bytes2file += w;
        return w;
    }
    void setBuffer(DPN_ExpandableBuffer b) {
        __buffer = b;
    }
    void setBufferSize(size_t n) {
        __buffer.reserve(n);
    }
    int file2buffer() {
        int r = __buffer.readData(__file);

        __bytes2buffer += r;
        return r;
    }
    int buffer2file(size_t bytes) {return __buffer.writeData(__file, bytes);}
    DPN_ExpandableBuffer & buffer() {return __buffer;}
    const DPN_ExpandableBuffer & buffer() const {return __buffer;}


    inline size_t transported() const {return bytes_transported;}
    inline bool isTransported() const {return bytes_transported == size();}
    void clearTransportProgress();

    void clear();
    void saveOriginalHash(const std::string &hash) {
        __original_hash = hash;
    }

    inline int read(DPN_ExpandableBuffer &b) {return __read(b);}
    inline int read() {return __read(__buffer);}
    */
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

    DPN_FileTransportHandler * getSendHandler(int key) const { return __get_handler(key, send_space); }
    DPN_FileTransportHandler * getReceiveHandler(int key) const { return __get_handler(key, receive_space); }

    DPN_FileTransportHandler * startSend(const DFile &file) { return __insert_handler(file, send_space); }
    DPN_FileTransportHandler * startReceive(const DFile &file) { return __insert_handler(file, receive_space); }
    DPN_FileTransportHandler * startReceive(const DFile &file, int remoteKey) { return __insert_handler2(remoteKey, file, receive_space); }
    

    bool stopSend(const DArray<int> &keyset);

    bool stopSend(int key) { return __removeFile(key, send_space); }
    bool stopReceive(int key) { return __removeFile(key, receive_space); }
    bool stopReceive(const DArray<int> &keyset);

private:

    typedef std::map<int, DPN_FileTransportHandler*> transport_map_t;

    inline DPN_FileTransportHandler * __insert_handler(const DFile &file, transport_map_t  &map) { return __insert_handler2(file.key(), file, map); }
    DPN_FileTransportHandler * __insert_handler2(int key, const DFile &file, transport_map_t  &map);

    DPN_FileTransportHandler * __get_handler(int key, const transport_map_t &map) const;
    bool __removeFile(int key, transport_map_t &map);

    transport_map_t send_space;
    transport_map_t receive_space;

    DPN_SimplePool<DPN_FileTransportHandler> __handlers_pool;
};


#endif // DPN_TRANSPORTHANDLER_H
