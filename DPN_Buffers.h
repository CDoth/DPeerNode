#ifndef DPN_BUFFERS_H
#define DPN_BUFFERS_H
#include <DArray.h>

#include "__dpeernode_global.h"

//================================================================
class DPN_ExpandableBufferData {
public:
    friend class DPN_ExpandableBuffer;    
public:
    inline void reserve(int size) {__buffer.reformUp(size);}
    inline void expand(int bytes) {__buffer.appendNs(bytes);}

    void append(const void *data, int size);
    inline void append(const DPN_ExpandableBufferData &b) { append(b.__buffer.constData(), b.__buffer.size()); }
    inline void append(const DPN_ExpandableBufferData &b, int size) { append(b.__buffer.constData(), size); }
    inline void appendString(const std::string &string) { append(string.data(), string.size()); }

    inline void copy(const void *data, int size) {__buffer.copyData(reinterpret_cast<const uint8_t*>(data), size);}
    inline void copy(const DPN_ExpandableBufferData &b) {__buffer.copyArray(b.__buffer);}
    inline void copyString(const std::string &string, std::string::size_type size = 0) {copy(string.data(), size ? size : string.size());}

    int readData(FILE *file, int size);
    int writeData(FILE *file, int size) const;

    inline void dropTo(int size) {__buffer.reformDown(size);}
    inline void clear() {__buffer.drop();}

    void print(int s = 0) const;
public:
    template <class T>
    int appendValue(const T &value) {
        __buffer.appendData(reinterpret_cast<const uint8_t*>(&value), sizeof(T));
        return sizeof(T);
    }
private:
    DArray<uint8_t> __buffer;
};
class DPN_ExpandableBuffer : public DWatcher<DPN_ExpandableBufferData> {
public:
    DPN_ExpandableBuffer() : DWatcher<DPN_ExpandableBufferData>(true) {}
public:
    inline void reserve(int size) { data()->reserve(size); }
    inline void expand(int bytes) { data()->expand(bytes); }

    inline void append(const void *__data, int size) { data()->append(__data, size); }
    inline void append(const DPN_ExpandableBuffer &b) { data()->append(*b.data()); }
    inline void append(const DPN_ExpandableBuffer &b, int size) { data()->append(*b.data(), size);}
    inline void appendString(const std::string &string) { data()->appendString(string); }

    inline void copy(const void *__data, int size) { data()->copy(__data, size); }
    inline void copy(const DPN_ExpandableBuffer &b) { data()->copy(*b.data()); }
    inline void copy(const DPN_ExpandableBuffer &b, int n) { data()->__buffer.copyArray(b.data()->__buffer, n); }

    inline void copyString(const std::string &string, std::string::size_type size = 0) { data()->copyString(string, size); }

    inline int readData(FILE *file, int size = 0) {return data()->readData(file, size);}
    inline int writeData(FILE *file, int size) const {return data()->writeData(file, size);}

    inline void dropTo(int size) {data()->dropTo(size);}
    inline void clear() {data()->clear();}

    template <class T>
    inline int appendValue(const T &value) {return data()->appendValue(value);}


    inline uint8_t * getData() {return data()->__buffer.changableData();}
    inline const uint8_t * getData() const {return data()->__buffer.constData();}
    inline uint32_t * getData32() {return reinterpret_cast<uint32_t*>(data()->__buffer.changableData());}
    inline const uint32_t * getData32() const {return reinterpret_cast<const uint32_t*>(data()->__buffer.constData());}
    inline uint64_t * getData64() {return reinterpret_cast<uint64_t*>(data()->__buffer.changableData());}
    inline const uint64_t * getData64() const {return reinterpret_cast<const uint64_t*>(data()->__buffer.constData());}

    inline const DArray<uint8_t> & buffer() const {return data()->__buffer;}

    inline int size() const {return data()->__buffer.size();}
    inline bool empty() const {return data()->__buffer.empty();}

    inline void print(int s = 0) const {data()->print(s);}
};
//================================================================




#endif // DPN_BUFFERS_H
