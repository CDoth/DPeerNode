#include "DPN_Buffers.h"

using namespace DPN::Logs;

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




