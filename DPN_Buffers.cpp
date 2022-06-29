#include "DPN_Buffers.h"

using namespace DPeerNodeSpace;

void DPN_ExpandableBufferData::append(const void *data, int size) {
    if(data == nullptr || size < 0) return;
    __buffer.appendData(reinterpret_cast<const uint8_t*>(data), size);
}
int DPN_ExpandableBufferData::readData(FILE *file, int size) {
    int s = __buffer.size();
    reserve(__buffer.size() + size);
    return fread(__buffer.changableData() + s, 1, size, file);
}
int DPN_ExpandableBufferData::writeData(FILE *file, int size) const {
    if( file == nullptr) {
        DL_BADPOINTER(1, "file: [%p]", file);
        return -1;
    }
    return fwrite(__buffer.constData(), 1, size, file);
}
void DPN_ExpandableBufferData::print(int s) const {
    s = s ? s : __buffer.size();
    std::cout << "[buffer " << this << "]: {";
    FOR_VALUE( s, i ) std::cout << (int)__buffer[i] << ' ';
    std::cout << "}" << std::endl;
}


/*
//---------------------------------------------------------------------------------------------- SimpleBuffer:
#define BYTELEN_SIZE (sizeof(size_t))
#define SB_REAL_PTR(BUFFER) (BUFFER - BYTELEN_SIZE)
void __sb_resize(SimpleBuffer &b, size_t s) {

    SimpleBuffer raw_b = b ? SB_REAL_PTR(b) : NULL;
    raw_b = reget_mem(raw_b, BYTELEN_SIZE + s);
    raw_b += BYTELEN_SIZE;

    *reinterpret_cast<size_t*>(SB_REAL_PTR(raw_b)) = s;

    b = raw_b;
}
size_t __sb_size(SimpleBuffer b) {
    if(b) return *reinterpret_cast<size_t*>(SB_REAL_PTR(b));
    else return 0;
}
void __sb_free(SimpleBuffer b) {
    if(b) {
        b -= BYTELEN_SIZE;
        free_mem(b);
    }
}

//---------------------------------------------------------------------------------------------- DPN_ExpandableBuffer:
DPN_ExpandableBuffer &DPN_BuffersPool::getBuffer() {

       if(session_iterator < pool.size()) {
           DPN_ExpandableBuffer &buffer = pool[session_iterator++];
           buffer.clear();
           return buffer;
       } else {
           pool.push_back();
           ++session_iterator;
           return pool.back();
       }
}
*/


