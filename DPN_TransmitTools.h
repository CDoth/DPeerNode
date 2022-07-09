#ifndef DPN_TRANSMITTOOLS_H
#define DPN_TRANSMITTOOLS_H
#include <DArray.h>
#include "DPN_NodeConnector.h"



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
class dpn_mediator<DPN::Time::TimeMoment> {
public:

    static void save(const DPN::Time::TimeMoment &m, DPN_ExpandableBuffer &b) {
        __tr::transport(m.lt_seconds(), b);
        __tr::transport(m.lt_useconds(), b);
    }
    static void upload(DPN::Time::TimeMoment &m, DPN_ExpandableBuffer &b, int &p) {
        int s, u;
        __tr::transport(s, b, p);
        __tr::transport(u, b, p);
        m.setLocalTime(s, u);
    }
};
template <>
class dpn_mediator<DPN::Network::PeerAddress> {
public:
    static void save(const DPN::Network::PeerAddress &pa, DPN_ExpandableBuffer &b) {
        __do(const_cast<DPN::Network::PeerAddress&>(pa), b);
    }
    static void upload(DPN::Network::PeerAddress &pa, const DPN_ExpandableBuffer &b, int &p) {
        __do(pa, b, p);
    }
private:
    template <class ... Args>
    static void __do(DPN::Network::PeerAddress &pa, Args && ... a) {
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

#define UNIT(INNER_TYPE) __hl_item<INNER_TYPE>


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
