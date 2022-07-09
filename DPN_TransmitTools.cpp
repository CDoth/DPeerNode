#include "DPN_TransmitTools.h"

using namespace DPN::Logs;



__base_hl_item::__base_hl_item() {
//    DL_INFO(1, "Create: [%p]", this);
}
__base_hl_item::__base_hl_item(__transmit_content *content) {
    content->registerItem(this);
//    DL_INFO(1, "Create: [%p] content: [%p]", this, content);
}
__base_hl_item::~__base_hl_item() {
//    DL_INFO(1, "Destruct item: [%p]", this);
}
void __base_hl_item::roughLoad(const uint8_t *d, int s) {
   __buffer.clear();
   __buffer.append(d, s);
   //   DL_INFO(1, "item: [%p] d: [%p] s: [%d]", this, d, s);
}
__transmit_content::__transmit_content() {
    __prefix_size = 0;
    __parse_total_size = true;
//    DL_INFO(1, "Create: [%p]", this);
}
void __transmit_content::registerItem(__base_hl_item *pItem) {
//    DL_INFO(1, "Register: item: [%p] content: [%p]", pItem, this);
    __content.push_back(pItem);
}
void __transmit_content::setTotalSizeParsing(bool s) {
    __parse_total_size = s;
}
void __transmit_content::clearBuffer() {
    __buffer.clear();
}
void __transmit_content::clearItems() {
    FOR_VALUE(__content.size(), i) {
        __content[i]->clearBuffer();
    }
}
void __transmit_content::clear() {
    clearBuffer();
    clearItems();
}

void __transmit_content::parseBuffers() {

    __buffer.clear();

    if( __parse_total_size ) __buffer.appendValue<uint32_t>(0); //size
    int size = 0;

    FOR_VALUE(__prefix_size, i) {
        __buffer.appendValue<uint32_t>(0);
    }



    FOR_VALUE(__content.size(), i) {
        const DPN_ExpandableBuffer &lb = __content[i]->buffer();

        int s = 0;
        s = __buffer.appendValue(lb.size());
//        DL_INFO(1, "metasize: [%d] total: [%d]", s, size);
        size += s;
        __buffer.append(lb);
        s = lb.size();
//        DL_INFO(1, "size: [%d] total: [%d]", s, size);
        if(s > 0) size += s;

//        DL_INFO(1, "parse item buffer: [%d]", lb.size());

    }

//    DL_INFO(1, "content: [%d] size: [%d]", __content.size(), size);

    if( __parse_total_size ) {
        int *pSize = reinterpret_cast<int*>(__buffer.getData());
        if( pSize == nullptr ) {
            DL_BADPOINTER(1, "pSize");
            return;
        }
        *pSize = size;
    }

}

bool __transmit_content::deparseBuffer() {
    return deparseBuffer(__buffer);
}
bool __transmit_content::deparseBuffer(const DPN_ExpandableBuffer &eb) {

    const uint8_t *b = eb.getData();
    const uint8_t *e = eb.getData() + eb.size();
    int i = 0;
    int itemSize = 0;
    int bufferSize = eb.size();
    int p = 0;
    bool __check_buffer_end;
    bool __check_content_end;


    if( bufferSize == 0 ) return true;

//    DL_INFO(1, "buffer: [%d] content: [%d]",
//            eb.size(), __content.size());


    // Minimum buffer size: 4 bytes of single item size (can be 0)
    if(bufferSize < 4) {
        // error: buffer check
        DL_ERROR(1, "Buffered data invalid size: [%d] (minimum: [%d])", bufferSize, 4);
        return false;
    }
    if(__content.empty()) {
        DL_ERROR(1, "Empty content");
        return false;
        // error: content check
    }


    while( p < bufferSize && i < __content.size() ) {

        itemSize = *reinterpret_cast<const int*>(b + p);
        if( itemSize < 0 ) {
            DL_BADVALUE(1, "Item size in buffer: [%d] item: [%d]", itemSize, i);
            goto deparse_fail;
            // error: size check
        }
        if( (p + (int)sizeof(int) + itemSize) > bufferSize ) {
            // error: range check
            DL_ERROR(1, "Invalid buffer size: [%d] p: [%d] item size: [%d]", bufferSize, p, itemSize);
            goto deparse_fail;
        }
//        DL_INFO(1, "item: [%p] size: [%d] p: [%d] i: [%d] buffer size: [%d] sum: [%d]",
//                __content[i], itemSize, p, i, bufferSize, (p + (int)sizeof(int) + itemSize));

        p += sizeof(int);
        __content[i]->roughLoad(b + p, itemSize);
        p += itemSize;
        ++i;
    }

    __check_buffer_end = bool(p == bufferSize);
    __check_content_end = bool(i == __content.size());

//    DL_INFO(1, "DEPARSED: checks: b: [%d] c: [%d]", __check_buffer_end, __check_content_end);
    if(__check_buffer_end == false || __check_content_end == false ) {
        // error: data validable
        DL_ERROR(1, "Data deparsed incorrectly: buffer: [%d] content: [%d]", __check_buffer_end, __check_content_end);
        goto deparse_fail;
    }

    return true;
deparse_fail:
    FOR_VALUE(__content.size(), i) {
        __content[i]->clearBuffer();
    }
    return false;
}

//void __transmit_content::requestBuffer(int size) {
//    __buffer.clear();
//    __buffer.reserve(size);
//}
void __transmit_content::setPrefix(uint32_t v, int i) {
    if(__buffer.size() < (int)(sizeof(uint32_t) * (i+1)) ) {
        DL_ERROR(1, "buffer size too less [%d]", __buffer.size());
        return;
    }
    reinterpret_cast<uint32_t*>(__buffer.getData())[1 + i] = v;
}
const uint32_t &__transmit_content::prefix(int i) const {
    return reinterpret_cast<const uint32_t*>(__buffer.getData())[1 + i];
}




