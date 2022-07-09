#include "DPN_Crypto.h"




namespace DPN::Crypto {
    DPN_Result IO__Crypto::process(DPN_ExpandableBuffer &buffer) {
        if( buffer.empty() ) return DPN_REPEAT;
        if( iDemoRSACore.encode( buffer.getData(), buffer.size() ) ) {
            const DArray<uint8_t> &e = iDemoRSACore.encoded();
            buffer.clear();
            buffer.append( e.constData(), e.size() );
            return DPN_SUCCESS;
        }

        return DPN_FAIL;
    }
    bool IO__Crypto::checkProcessor() const {
        return DPN_SUCCESS;
    }
    DPN_Result IO__Crypto::processorStart() {
        if( iDemoRSACore.generateKeys() ) return DPN_SUCCESS;
        return DPN_REPEAT;
    }

}
