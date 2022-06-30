#include "DPN_TransmitTools.h"

using namespace DPN::Logs;

/*
void DPN_TransmitItem::setData(void *__data) {

    data()->data = __data;
    data()->cdata = nullptr;
}
void DPN_TransmitItem::setConstData(const void *__data) {

    data()->data = nullptr;
    data()->cdata = __data;
}
void DPN_TransmitItem::setSize(int __size) {


//    DL_INFO(1, "Item: [%p] size: [%d]", data(), __size);

    data()->size = __size;
}
void DPN_TransmitItem::setForm(DPN_TransmitForm __form) {

    data()->form = __form;
}
void DPN_TransmitItem::mute(DPN_TransmitMutex m) {
    data()->mutex = m;
}
void DPN_TransmitItem::unmute() {
    data()->mutex = TM_NO_MUTE;
}
//========================================================================== DPN_TransmitItemData:
DPN_TransmitItemData::DPN_TransmitItemData() {
    data = nullptr;
    cdata = nullptr;
    size = 0;
    form = TF_RAW;
    mutex = TM_NO_MUTE;
    specForm = NO_SPECIFIC;
}
bool DPN_TransmitItemData::send(DPN_NodeConnector *c)  {


    bool isMuted = bool(mutex & TM_MUTE_SEND);

//        std::cout << "DPN_TransmitItemData::send:"
//                  << " this: " << (void*)this
//                  << " isMuted: " << isMuted
//                  << " size: " << size
//                  << " data: " << (void*)data
//                  << " cdata: " << (const void*)cdata
//                  << std::endl;

    if(isMuted) return true;




    switch (form) {
    case TF_RAW:

        if( c->x_sendIt(cdata ? cdata : data, size) == DPN_SUCCESS ) {
//            DL_INFO(1, "sent (raw): [%d]", c->transportedBytes());
            return true;
        }
        break;
    case TF_PACKET:

        if( (c->x_sendPacket(cdata ? cdata : data, size)) == DPN_SUCCESS ) { // fix double check
//            DL_INFO(1, "sent (packet): [%d]", c->transportedBytes());
            return true;
        }
        break;
    }

    return false;
}
bool DPN_TransmitItemData::flush(DPN_ExpandableBuffer &out) {

    bool isMuted = bool(mutex & TM_MUTE_SEND);

//    std::cout << "DPN_TransmitItemData::flush:"
//              << " this: " << (void*)this
//              << " isMuted: " << isMuted
//              << " size: " << size
//              << " data: " << (void*)data
//              << " cdata: " << (const void*)cdata
//              << " form: " << form
//              << std::endl;

    if(isMuted) return true;

    switch (form) {
    case TF_RAW:
        if( out.appendData(cdata ? cdata : data, size) == false ) {
//            DL_FUNCFAIL(1, "appendData (raw)");
            return false;
        }
//        DL_INFO(1, "FLUSH RAW: data: [%p] size: [%d]",
//                (void*)(cdata ? cdata : data), size
//                );
        break;
    case TF_PACKET:
        if( out.appendData(&size, sizeof(size)) == false ) {
//            DL_FUNCFAIL(1, "appendData (packet size)");
            return false;
        }
        if( out.appendData(cdata ? cdata : data, size) == false ) {
//            DL_FUNCFAIL(1, "appendData (packet data)");
            return false;
        }
//        DL_INFO(1, "FLUSH PACKET: data: [%p] size: [%d]",
//                (void*)(cdata ? cdata : data), size
//                );
        break;
    }

    return true;
}
bool DPN_TransmitItemData::receive(DPN_NodeConnector *c) {

    bool isMuted = bool(mutex & TM_MUTE_RECEIVE);

//        std::cout << "DPN_TransmitItemData::receive:"
//                  << " isMuted: " << isMuted
//                  << " size: " << size
//                  << " data: " << data
//                  << " cdata: " << cdata
//                  << " form: " << form
//                  << std::endl;

    if(isMuted) return true;


    switch (form) {
    case TF_RAW:
        if( c->x_receiveTo(data, size) == DPN_SUCCESS ) {
//            DL_INFO(1, "received (raw): [%d]", c->transportedBytes());
            return true;
        }
        break;
    case TF_PACKET:
        if(data == nullptr) {
            if( c->x_receivePacket(size) == DPN_SUCCESS ) {
                cdata = c->bufferData();

//                DL_INFO(1, "received (packet 1): [%d] ps: [%d]",
//                        c->transportedBytes(), size);

                return true;
            }
        } else {
            if( c->x_receivePacket(data, size) == DPN_SUCCESS ) {
//                DL_INFO(1, "received (packet 2): [%d]", c->transportedBytes());
                return true;
            }
        }
        break;
    }

    return false;
}
bool DPN_TransmitItemData::sendMapped(DPN_NodeConnector *c, int key) {


    return false;
}
//========================================================================== DPN_TransmitContent:
DPN_TransmitItem DPN_TransmitContent::addItem(DPN_TransmitForm form, int size)  {
    DPN_TransmitItem item;
    item.setForm(form);
    item.setSize(size);

    content.append(item);

//    current_item = content.begin();
    return item;
}
bool DPN_TransmitContent::sendAll()  {

//    DL_INFO(1, "content size: [%d]", content.size());

    while(current != content.size()) {
//        DL_INFO(1, "send one: current: [%d]", current);
        if( !__inner_send_one() ) {
            return false;
        }
    }

    current = 0;
    return true;
}
bool DPN_TransmitContent::flushAll(DPN_ExpandableBuffer &out_buffer) {

    current = 0;
    while(current != content.size()) {

        if(content[current].flush(out_buffer)) {
            ++current;
        } else {
            DL_FUNCFAIL(1, "flush");
            return false;
        }
    }
    current = 0;
    return true;
}
bool DPN_TransmitContent::sendOne()  {



    return false;
}
bool DPN_TransmitContent::sendAllMapped() {

//    DL_INFO(1, "content size: [%d]", content.size());

    auto current_item = content.begin() + current;
    while(current_item != content.end()) {

//        std::cout << "sendAll: send one" << std::endl;

        if( !__inner_send_mapped_one(current) ) {
            return false;
        }
    }

    current = 0;
    return true;
}
bool DPN_TransmitContent::receiveAll()  {

//    DL_INFO(1, "content size: [%d]", content.size());

    while(current != content.size()) {
//        DL_INFO(1, "receive one: current: [%d]", current);
        if( !__inner_receive_one() ) {
            return false;
        }
    }

    current = 0;
    return true;
}
bool DPN_TransmitContent::receiveOne() {



    return false;
}

bool DPN_TransmitContent::deparse(const DPN_ExpandableBuffer &buffer)  {


    const uint8_t *data = buffer.getData();
    if(data == nullptr) {
        DL_BADPOINTER(1, "data");
        return false;
    }
//    std::cout << " >>> deparse"
//              << " content size: " << content.size()
//              << " buffer size: " << buffer.size()
//              << " data: " << (void*)data
//              << std::endl;

    if(content[0].specForm() == CONTENT_SIZE) {

    }

    FOR_VALUE(content.size(), i) {

        DPN_TransmitItem &item = content[i];

//            std::cout << " ---- 0. deparse unreceivable: " << item.isUnReceivable()
//                      << " form: " << item.form()
//                      << std::endl;

        if(item.isUnReceivable()) continue;
        int packetSize = 0;



        switch (item.form()) {
        case TF_RAW:

            item.setConstData(data);
            data += item.size();

//                std::cout << " ---- 0. deparse raw: size: " << item.size()
//                          << std::endl;
            break;

#define EXTRACT_SIZE  \
        packetSize = *reinterpret_cast<const int*>(data); \
        data += sizeof(int);
        case TF_PACKET:

            EXTRACT_SIZE;

            item.setConstData(packetSize ? data : nullptr);
            item.setSize(packetSize);
            data += packetSize;


//                std::cout << " ---- 2. deparse packet size: " << packetSize
//                          << std::endl;
            break;
        }
#undef EXTRACT_SIZE
    }
    return true;
}
const uint8_t *DPN_TransmitContent::deparse(const uint8_t *source) {

    const uint8_t *data = source;

//    std::cout << " >>> deparse"
//              << " content size: " << content.size()
//              << " data: " << (void*)data
//              << std::endl;

    FOR_VALUE(content.size(), i) {

        DPN_TransmitItem &item = content[i];

//            std::cout << " ---- 0. deparse unreceivable: " << item.isUnReceivable()
//                      << " form: " << item.form()
//                      << std::endl;

        if(item.isUnReceivable()) continue;
        int packetSize = 0;



        switch (item.form()) {
        case TF_RAW:

            item.setConstData(data);
            data += item.size();

//                std::cout << " ---- 0. deparse raw: size: " << item.size()
//                          << std::endl;
            break;

#define EXTRACT_SIZE  \
        packetSize = *reinterpret_cast<const int*>(data); \
        data += sizeof(int);
        case TF_PACKET:

            EXTRACT_SIZE;

            item.setConstData(packetSize ? data : nullptr);
            item.setSize(packetSize);
            data += packetSize;


//                std::cout << " ---- 2. deparse packet size: " << packetSize
//                          << std::endl;
            break;
        }
#undef EXTRACT_SIZE
    }
    return data;
}
int DPN_TransmitContent::receivingDataSize() const {
    int __size = 0;

    FOR_VALUE(content.size(), i) {

        const DPN_TransmitItem &item = content[i];

        if(item.isReceivable()) {
            __size += item.size();
            if(item.form() == TF_PACKET)
                __size += sizeof(int);
        }

    }
    return __size;
}
int DPN_TransmitContent::sendingDataSize() const {
    int __size = 0;

    FOR_VALUE(content.size(), i) {

        const DPN_TransmitItem &item = content[i];




        if(item.isSendable()) {
            __size += item.size();
            if(item.form() == TF_PACKET)
                __size += sizeof(int);
        }

//        DL_INFO(1, "mute: [%d] size: [%d] form: [%d] total: [%d] sendable: [%d]",
//                item.mutex(), item.size(), item.form(), __size, item.isSendable()
//                );
    }
    return __size;
}
bool DPN_TransmitContent::__inner_send_one() {

//    DL_INFO(1, "current: [%d] content: [%d]", current, content.size());

    if( content[current].send(connector) ) {
        ++current;
        return true;
    }

    return false;
}
bool DPN_TransmitContent::__inner_receive_one() {

//    DL_INFO(1, "current: [%d] content: [%d]", current, content.size());

    if( content[current].receive(connector) ) {
        ++current;
        return true;
    }

    return false;
}
bool DPN_TransmitContent::__inner_send_mapped_one(int key) {


//    DL_INFO(1, "key: [%d] current: [%d] content: [%d]", key, current, content.size());

    auto current_item = content.begin() + current;
    if( current_item->sendMapped(connector, key)) {
        ++current;
        return true;
    }

    return false;
}

*/

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




