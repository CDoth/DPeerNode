#include "DPN_IO.h"

namespace DPN::IO {
    IOContext::IOContext() { }
    IOContext::~IOContext() { }
    DPN_Result IOContext::generate(DPN_ExpandableBuffer&) { return DPN_SUCCESS; }
    DPN_Result IOContext::process(DPN_ExpandableBuffer&) { return DPN_SUCCESS; }
} // DPN_IO namespace



