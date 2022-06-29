#ifndef DPN_TRANSMITTOOLS_H
#define DPN_TRANSMITTOOLS_H
#include <DArray.h>


#include "DPN_NodeConnector.h"


// Send:

// List of transmit items and their template decriptors

// 1. Set outter pointers or copy to inner buffer send-data
// using item descriptors
// 2. Send all list, item by item. First item is processor type.

// Recv:

// 1. Receive full packet



// How to ignore items on send and deparse it on receive (only for raw items; for packet items just send packet size [0]):
// 1. Send default value
// 2. Send size of all raw items (to send zero size for ignored items to skip it)
// 3. No send ignored items but send keys for items to recognize received items
/*
enum DPN_TransmitForm {
    TF_RAW
    ,TF_PACKET
};
enum DPN_ItemSpecificForm {
    NO_SPECIFIC
    ,CONTENT_SIZE
};

enum DPN_TransmitMutex {

    TM_NO_MUTE = 0b00000000
    ,TM_MUTE_RECEIVE = 0b00000001
    ,TM_MUTE_SEND = 0b00000010
    ,TM_MUTE_BOTH = 0b00000011

};
class DPN_TransmitItemData {
public:
    friend class DPN_TransmitItem;

    DPN_TransmitItemData();
    bool send(DPN_NodeConnector *c);
    bool flush(DPN_ExpandableBuffer &out);
    bool receive(DPN_NodeConnector *c);
    bool sendMapped(DPN_NodeConnector *c, int key);
private:
    void *data; // Outter pointer or inner buffer
    const void *cdata; // Const outter pointer
    int size;
    DPN_TransmitForm form;
    DPN_TransmitMutex mutex;
    DPN_ItemSpecificForm specForm;

};
class DPN_TransmitItem : public DWatcher<DPN_TransmitItemData> {
public:
    DPN_TransmitItem() : DWatcher<DPN_TransmitItemData>(true)
    {}


    void setData(void *__data);
    void setConstData(const void *__data);
    void setSize(int __size);
    void setForm(DPN_TransmitForm __form);
    void mute(DPN_TransmitMutex m);
    void unmute();

    void clear() {
        data()->data = nullptr;
        data()->cdata = nullptr;
        data()->size = 0;
    }

    const void *getData() const {
        return data()->data;
    }
    const void *getConstData() const {
        return data()->cdata ? data()->cdata : data()->data;
    }

    inline DPN_TransmitForm form() const {return data()->form;}
    inline DPN_ItemSpecificForm specForm() const {return data()->specForm;}

    int size() const {return data()->size;}
    DPN_TransmitMutex mutex() const {return data()->mutex;}


    inline bool isUnReceivable() const {return data()->mutex & TM_MUTE_RECEIVE;}
    inline bool isUnSendable() const {return data()->mutex & TM_MUTE_SEND;}
    inline bool isReceivable() const {return !isUnReceivable();}
    inline bool isSendable() const {return !isUnSendable();}



    inline bool send(DPN_NodeConnector *c) {return data()->send(c);}
    inline bool receive(DPN_NodeConnector *c) {return data()->receive(c);}
    inline bool flush(DPN_ExpandableBuffer &out_buffer) {return data()->flush(out_buffer);}

    inline bool sendMapped(DPN_NodeConnector *c, int key) {return data()->sendMapped(c, key);}
};


//======================================================================================================== DPN_TransmitItemBaseDescriptor:
class DPN_TransmitItemBaseDescriptor {
public:
    friend class DPN_TransmitProcessor;

    inline DPN_TransmitItemBaseDescriptor &operator=(DPN_TransmitItem __item) {item = __item; return *this;}

    void clear() {item.clear();}
    inline void mute(DPN_TransmitMutex m) {item.mute(m);}
    inline void unmute() {item.unmute();}
    inline int size() const {return item.size();}
protected:
    DPN_TransmitItem item;
};
//======================================================================================================== DPN_TransmitItemDescriptor:
//------------------------------------ Value:
template <class TargetType>
class DPN_TransmitItemDescriptor : public DPN_TransmitItemBaseDescriptor {
public:

    DPN_TransmitItemDescriptor() {}

    inline DPN_TransmitItemDescriptor &operator=(DPN_TransmitItem __item) {item = __item; return *this;}
    inline DPN_TransmitItemDescriptor &operator=(const TargetType &value) {copyValue(value); return *this;}

    void setValue(const TargetType &data) {
        item.setConstData(&data);
        item.setSize(sizeof(TargetType));
    }
    void copyValue(const TargetType &data) {
        store = data;
        setValue(store);
    }
    void save() {
        copyValue(value());
    }

    TargetType value() const { return item.getConstData() ? *reinterpret_cast<const TargetType*>(item.getConstData()) : TargetType(); }
    const TargetType & getConstData() const { return item.getConstData() ? *reinterpret_cast<const TargetType*>(item.getConstData()) : TargetType(); }

    TargetType store;
};
//------------------------------------ String:
template <>
class DPN_TransmitItemDescriptor<std::string> : public DPN_TransmitItemBaseDescriptor {
public:

    DPN_TransmitItemDescriptor() {}
    inline DPN_TransmitItemDescriptor &operator=(DPN_TransmitItem __item) {item = __item; return *this;}
    inline operator std::string() const {
        return toString();
    }

    inline DPN_TransmitItemDescriptor & operator=(const std::string &value) {copyString(value); return *this;}
    void setData(const std::string &string) {
        item.setConstData(string.data());
        item.setSize(string.size());
    }

    void copyString(const std::string &string) {

        store = string;
        setData(store);
    }

    inline bool empty() const {return store.empty();}
    std::string toString() const {return item.getConstData() ?
                    std::string( reinterpret_cast<const char*>(item.getConstData()), item.size() ) :
                    std::string();
    }

    std::string store;
};
//------------------------------------ Pointer:
template <class TargetType>
class DPN_TransmitItemDescriptor<TargetType*> : public DPN_TransmitItemBaseDescriptor {
public:

    DPN_TransmitItemDescriptor()  {
        item.clear();
    }
    inline DPN_TransmitItemDescriptor &operator=(DPN_TransmitItem __item) {item = __item; return *this;}

    inline DPN_TransmitItemDescriptor & operator=(TargetType *ptr) {item.setData(ptr); return *this;}
    inline DPN_TransmitItemDescriptor & operator=(const TargetType *ptr) {item.setConstData(ptr); return *this;}

    void setData(TargetType *data, int size) {
        item.setData(data);
        item.setSize(size);
    }
    void setConstData(const TargetType *data, int size) {
        item.setConstData(data);
        item.setSize(size);
    }
    void setData(const DPN_ExpandableBuffer &buffer, int size = 0) {

        size = size ? size : buffer.size();

        item.setConstData(buffer.getData());
        item.setSize(size);
    }

    void clear() {
        item.clear();
    }
    const TargetType * getData() const { return reinterpret_cast<const TargetType*>(item.getConstData()); }
};
//======================================================================================================== DPN_TransmitItemSimpleDescriptor:
//------------------------------------ Value:
template <class TargetType>
class DPN_TransmitItemSimpleDescriptor : public DPN_TransmitItemBaseDescriptor {
public:
    DPN_TransmitItemSimpleDescriptor() {}
    inline DPN_TransmitItemSimpleDescriptor &operator=(DPN_TransmitItem __item) {item = __item; return *this;}
    inline DPN_TransmitItemSimpleDescriptor &operator=(const TargetType &source) { setConstValue(source); return *this; }
    inline DPN_TransmitItemSimpleDescriptor &operator=(TargetType &source) { setPlace(source); return *this; }

    void setPlace(TargetType &place) {
        item.setData(&place);
        item.setSize(sizeof(TargetType));
    }
    void setConstValue(const TargetType &source) {
        item.setConstData(&source);
        item.setSize(sizeof(TargetType));
    }
    TargetType value() const { return item.getConstData() ? *reinterpret_cast<const TargetType*>(item.getConstData()) : TargetType(); }
    const TargetType & getConstData() const { return item.getConstData() ? *reinterpret_cast<const TargetType*>(item.getConstData()) : TargetType(); }
};
//------------------------------------ String:
template <>
class DPN_TransmitItemSimpleDescriptor<std::string> : public DPN_TransmitItemBaseDescriptor {
public:

    DPN_TransmitItemSimpleDescriptor() {}
    inline DPN_TransmitItemSimpleDescriptor &operator=(DPN_TransmitItem __item) {item = __item; return *this;}
    inline DPN_TransmitItemSimpleDescriptor &operator=(const std::string &source) {
        setData(source);
        return *this;
    }
    void setData(const std::string &string) {
        item.setConstData(string.data());
        item.setSize(string.size());
    }
    std::string toString() const {return item.getConstData() ?
                    std::string( reinterpret_cast<const char*>(item.getConstData()), item.size() ) :
                    std::string();
    }
};
//------------------------------------ Pointer:
template <class TargetType>
class DPN_TransmitItemSimpleDescriptor<TargetType*> : public DPN_TransmitItemDescriptor<TargetType*> {};
//========================================================================================================
class DPN_TransmitContent {
public:
    DPN_TransmitItem addItem(DPN_TransmitForm form, int size = 0);
public:
    DPN_TransmitContent() {
        connector = nullptr;
        current = 0;
    }
    void setConnector(DPN_NodeConnector *c) {connector = c;}

    bool sendAll();
    bool flushAll(DPN_ExpandableBuffer &out_buffer);
    bool sendOne();

    bool sendAllMapped();

    bool receiveAll();
    bool receiveOne();

    bool deparse(const DPN_ExpandableBuffer &buffer);
    const uint8_t * deparse(const uint8_t *source);


    int size() const {return content.size();}
    int receivingDataSize() const;
    int sendingDataSize() const;
private:
    bool __inner_send_one();
    bool __inner_receive_one();

    bool __inner_send_mapped_one(int key);
private:
    DArray<DPN_TransmitItem> content;
    int current;
    DPN_NodeConnector *connector;
};
// raw, packet, complex, meta-complex
// arrays

*/

//----------------------------------------------------
struct __complex1 {
    int v;
    double d;
    char c;
};
struct __complex2 {

    __complex2() {
        c = 'A';
        d = nullptr;
        d_s = 0;
        v = 0;
        f = 0.0f;
    }
    void testInit() {
        a.appendNs(15);
        a.runIn([](int &v) {v = rand() % 100;});
        //-------------
        c = 'J';
        //-------------
        d_s = 8;
        d = get_zmem<uint8_t>(d_s);
        FOR_VALUE(d_s, i) d[i] = rand() % 25 + 100;
        //-------------
        f = 8543.456;
        //-------------
        s = "{Test __complex2 string}";
        //-------------
        v = 55443;
        //-------------
        str_a.appendNs(10);
        str_a.runIn([](std::string &v) {int r = rand() % 23 + 1; char c = 'a'; v.append(r, char(c+r));});
    }
    char c;
    std::string s;
    uint8_t *d; int d_s;
    int v;
    DArray<std::string> str_a;
    double f;
    DArray<int> a;
};
struct __complex3 {

    int v;
    __complex1 c1;
    double d;
    __complex2 c2;
};
//----------------------------------------------------


struct __raw_data {
    __raw_data() : d(nullptr), s(0) {}
    __raw_data(const void *__d, int __s) : d(__d), s(__s) {}
    const void *d; int s;
};


template <class T>
class __mediator
//        : public std::conditional<std::is_integral<T>::value || std::is_floating_point<T>::value, __d_valid_type, __d_invalid_type>::type
{
public:

    enum {size = sizeof(T)};
    static void save(const T &v, DPN_ExpandableBuffer &b) {
        b.appendValue(v);
    }
    static void upload(T &v, const DPN_ExpandableBuffer &b, int &p) {
        auto o = reinterpret_cast<const T*>(b.getData() + p);
        p += size;
        v = *o;
    }

};
template <class T>
class __mediator<T*> {
public:

    enum {size = -1};

    static void save(const T *d, int s, DPN_ExpandableBuffer &b) {
        b.appendValue(s);
        b.append(d, s);
    }
    static __raw_data upload(const DPN_ExpandableBuffer &b, int &p) {
        int s = *reinterpret_cast<const int*>(b.getData() + p);
        p += sizeof(int);
        auto o = reinterpret_cast<const T*>(b.getData() + p);
        p += s;
        return {o, s};
    }
    int uploadSize(const DPN_ExpandableBuffer &b, int &p) const {
        auto o = reinterpret_cast<const int*>(b.getData() + p);
        p += sizeof(int);
        return *o;
    }
    const T * uploadData(const DPN_ExpandableBuffer &b, int &p, int s) const {
        auto o = reinterpret_cast<const T*>(b.getData() + p);
        p += s;
        return o;
    }
};
//----------------------------------------------------

template <class T>
class __valid_mediator : public __mediator<T> {};
template <class T>
class __invalid_mediator : __mediator<T> {};

template <class T>
class dpn_mediator :
        public std::conditional<std::is_integral<T>::value || std::is_floating_point<T>::value || std::is_enum<T>::value,
        __valid_mediator<T>, __invalid_mediator<T>>::type {};

template <class T>
class dpn_mediator<T*> : public __mediator<T*> {};
//--------------------------------------------------
class __tr {
public:
    template <class T>
    static void transport(const T &v, DPN_ExpandableBuffer &b) {
        dpn_mediator<T>::save(v, b);
    }
    template <class T>
    static void transport(T &v, const DPN_ExpandableBuffer &b, int &p) {
        dpn_mediator<T>::upload(v, b, p);
    }
    template <class T>
    static void transport(const T *d, int s, DPN_ExpandableBuffer &b) {
        dpn_mediator<T*>::save(d, s * sizeof(T), b);
    }
    template <class T>
    static void transport(T const *&d, int &s, const DPN_ExpandableBuffer &b, int &p) {
        __raw_data rd = dpn_mediator<T*>::upload(b, p);
        d = reinterpret_cast<const T*>(rd.d);
        s = rd.s / sizeof(T);
    }
    template <class T>
    static void transport(T *&d, int &s, const DPN_ExpandableBuffer &b, int &p) {
        __raw_data rd = dpn_mediator<T*>::upload(b, p);

        if(d == nullptr) {
            s = rd.s / sizeof(T);
            d = get_zmem<T>(s);
            copy_mem(d, reinterpret_cast<const T*>(rd.d), s);
        }
    }
};
//----------------------------------------------------
template <>
class dpn_mediator<std::string> {
public:

    static void save(const std::string &s, DPN_ExpandableBuffer &b) {
        __tr::transport(s.data(), s.size(), b);
    }
    static void upload(std::string &s, const DPN_ExpandableBuffer &b, int &p) {
        __raw_data rd = __mediator<char*>::upload(b, p);
        s = std::string(reinterpret_cast<const char*>(rd.d), rd.s);
    }

};
template <class T>
class dpn_mediator<DArray<T>> {
public:

    struct trivial_layer {};
    struct complex_layer {};

    struct layer : public
            std::conditional<

            std::is_integral<T>::value
            || std::is_floating_point<T>::value
            || std::is_pointer<T>::value,

            trivial_layer, complex_layer>::type
    {};


    static void saveValue(const T &v, DPN_ExpandableBuffer &b) {
        __tr::transport(v, b);
    }
    static void save(const DArray<T> &a, DPN_ExpandableBuffer &b) {
        __save(a, b, layer());
    }
    static void upload(DArray<T> &a, const DPN_ExpandableBuffer &b, int &p) {
        __upload(a, b, p, layer());
    }
private:
    static void __save(const DArray<T> &a, DPN_ExpandableBuffer &b, trivial_layer) {
        __tr::transport(a.constData(), a.bytesSize(), b);
    }
    static void __save(const DArray<T> &a, DPN_ExpandableBuffer &b, complex_layer) {
        __tr::transport(a.size(), b);
        FOR_VALUE(a.size(), i) {
            __tr::transport(a[i], b);
        }
    }
    static void __upload(DArray<T> &a, const DPN_ExpandableBuffer &b, int &p, trivial_layer) {
        const T *d; int s;
        __tr::transport(d, s, b, p);
        a.copyData(d, s / sizeof(T));
    }
    static void __upload(DArray<T> &a, const DPN_ExpandableBuffer &b, int &p, complex_layer) {
        int size;
        __tr::transport(size, b, p);
        FOR_VALUE(size, i) {
            T o;
            __tr::transport(o, b, p);
            a.push_back(o);
        }
    }

};
template <>
class dpn_mediator<__complex1>  {
public:

    template <class ... Args>
    static void __do(__complex1 &c1, Args && ... a) {

        __tr::transport(c1.c, a...);
        __tr::transport(c1.d, a...);
        __tr::transport(c1.v, a...);

    }
    static void save(const __complex1 &c1, DPN_ExpandableBuffer &b) {
        __do(const_cast<__complex1&>(c1), b);
    }
    static void upload(__complex1 &c1, const DPN_ExpandableBuffer &b, int &p) {
        __do(c1, b, p);
    }

};
template <>
class dpn_mediator<__complex2>  {
public:

    template< class ... Args >
    static void __do(__complex2 &t, Args && ... a) {

        __tr::transport(t.c, a...);
        __tr::transport(t.s, a...);
        __tr::transport(t.d, t.d_s, a...);
        __tr::transport(t.v, a...);
        __tr::transport(t.str_a, a...);
        __tr::transport(t.f, a...);
        __tr::transport(t.a, a...);

    }
    static void save(const __complex2 &c2, DPN_ExpandableBuffer &b) {
        __do(const_cast<__complex2&>(c2), b);
    }

    static void upload(__complex2 &c2, const DPN_ExpandableBuffer &b, int &p) {
        __do(c2, b, p);
    }
};
template <>
class dpn_mediator<__complex3> {
public:

    template < class ... Args >
    static void __do(__complex3 &t, Args && ... a) {
        __tr::transport(t.d, a...);
        __tr::transport(t.v, a...);
        __tr::transport(t.c1, a...);
        __tr::transport(t.c2, a...);
    }
    static void save(const __complex3 &c3, DPN_ExpandableBuffer &b) {
        __do(const_cast<__complex3&>(c3), b);
    }
    static void upload(__complex3 &c3, const DPN_ExpandableBuffer &b, int &p) {
        __do(c3, b, p);
    }
};
template <>
class dpn_mediator<DPN_TimeMoment> {
public:

    static void save(const DPN_TimeMoment &m, DPN_ExpandableBuffer &b) {
        __tr::transport(m.lt_seconds(), b);
        __tr::transport(m.lt_useconds(), b);
    }
    static void upload(DPN_TimeMoment &m, DPN_ExpandableBuffer &b, int &p) {
        int s, u;
        __tr::transport(s, b, p);
        __tr::transport(u, b, p);
        m.setLocalTime(s, u);
    }
};
template <>
class dpn_mediator<PeerAddress> {
public:
    static void save(const PeerAddress &pa, DPN_ExpandableBuffer &b) {
        __do(const_cast<PeerAddress&>(pa), b);
    }
    static void upload(PeerAddress &pa, const DPN_ExpandableBuffer &b, int &p) {
        __do(pa, b, p);
    }
private:
    template <class ... Args>
    static void __do(PeerAddress &pa, Args && ... a) {
        __tr::transport(pa.port, a...);
        __tr::transport(pa.address, a...);
    }
};

//----------------------------------------------------
class __base_hl_item;
class __transmit_content {
public:
    __transmit_content();
    void registerItem(__base_hl_item *pItem);

    void setTotalSizeParsing(bool s);
    void clearBuffer();
    void clearItems();
    void clear();
    void parseBuffers();
    bool deparseBuffer();
    bool deparseBuffer(const DPN_ExpandableBuffer &b);
    const DPN_ExpandableBuffer & buffer() const {return __buffer;}
    uint8_t * rawBuffer() {return __buffer.getData();}
//    void requestBuffer(int size);

    void setPrefix(uint32_t v, int i);
    const uint32_t & prefix(int i) const;

    void reservePrefix(uint8_t n) {__prefix_size = n;}
    uint8_t prefixSize() const {return __prefix_size;}
private:
    DArray<__base_hl_item*> __content;
    DPN_ExpandableBuffer __buffer;
    uint8_t __prefix_size;
    bool __parse_total_size;
};
class __base_hl_item {
public:
    __base_hl_item();
    __base_hl_item(__transmit_content *content);
    ~__base_hl_item();
    inline DPN_ExpandableBuffer & buffer() {return __buffer;}
    inline const DPN_ExpandableBuffer & buffer() const {return __buffer;}
    inline uint8_t * data() {return __buffer.getData();}
    inline uint32_t * data32() {return reinterpret_cast<uint32_t*>(__buffer.getData());}
    void roughLoad(const uint8_t *d, int s);
    inline void roughLoad(const DPN_ExpandableBuffer &b) {__buffer = b;}
    inline void reserve(int bytes) {__buffer.reserve(bytes);}

    inline void clearBuffer() { __buffer.clear(); }
protected:
    DPN_ExpandableBuffer __buffer;
};
template <class T>
class __hl_item : public __base_hl_item {
public:
    __hl_item() {}
    __hl_item(__transmit_content &content) : __base_hl_item(&content) {}
    void operator=(const T &v) {
        __buffer.clear();
//        __mediator<T>::save(v, __buffer);
        dpn_mediator<T>::save(v, __buffer);
    }
    T get() const {
        if(__buffer.empty()) return T();

        T v;
        int p = 0;
        dpn_mediator<T>::upload(v, __buffer, p);
        return v;
    }
};
template <class T>
class __hl_item<T*> : public __base_hl_item {
public:
    __hl_item() {}
    __hl_item(__transmit_content &content) : __base_hl_item(&content) {}
    void operator=(const T *p) {
        dpn_mediator<T*>::save(p, __buffer);
    }
    void operator=(const __raw_data &rd) {
        dpn_mediator<T*>::save(reinterpret_cast<const T*>(rd.d),
                              rd.s, __buffer);
    }
    void setData(const T *d, int s) {
        dpn_mediator<T*>::save(d, s, __buffer);
    }
    __raw_data getData() const {
        if(__buffer.empty()) return __raw_data();
        int p = 0;
        return dpn_mediator<T*>::upload(__buffer, p);
    }
    void getData(T *p) const {
        dpn_mediator<T*>::upload(p, __buffer, p);
    }
};
template <class T>
class __hl_item<DArray<T>> : public __base_hl_item {
public:
    __hl_item() {}
    __hl_item(__transmit_content &content) : __base_hl_item(&content) {}

    void add(const T &v) {
        if(__buffer.empty()) {
            dpn_mediator<int>::save(1, __buffer);
        } else {
            ++reinterpret_cast<int*>(__buffer.getData())[0];
        }
        dpn_mediator<DArray<T>>::saveValue(v, __buffer);

    }
    void operator=(const DArray<T> &a) {
        __buffer.clear();
        dpn_mediator<DArray<T>>::save(a, __buffer);
    }
    DArray<T> get() const {
        DArray<T> a;
        if(__buffer.empty()) return a;
        int p = 0;
        dpn_mediator<DArray<T>>::upload(a, __buffer, p);
        return a;
    }
};
template <>
class __hl_item<DPN_ExpandableBuffer> : public __base_hl_item {
public:
    __hl_item() {}
    __hl_item(__transmit_content &content) : __base_hl_item(&content) {}

    void operator=(const DPN_ExpandableBuffer &b) {
        __buffer = b;
    }
    DPN_ExpandableBuffer get() const {
        return __buffer;
    }
};



class DPN_Content {
public:
    inline void setTotalSizeParsing(bool s) {content.setTotalSizeParsing(s);}
    bool deparse(const DPN_ExpandableBuffer &buffer) {
        return content.deparseBuffer(buffer);
    }
    const DPN_ExpandableBuffer & parse() const {
        content.parseBuffers();
        return content.buffer();
    }
protected:
    mutable __transmit_content content;
};

#endif // DPN_TRANSMITTOOLS_H
