#include "DPN_IO.h"

using namespace DPeerNodeSpace;
namespace DPN_IO {
    IOContext::IOContext() { }
    IOContext::~IOContext() { }
    DPN_Result IOContext::generate(DPN_ExpandableBuffer&) { return DPN_SUCCESS; }
    DPN_Result IOContext::process(DPN_ExpandableBuffer&) { return DPN_SUCCESS; }
} // DPN_IO namespace



