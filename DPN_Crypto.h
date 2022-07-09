#ifndef DPN_CRYPTO_H
#define DPN_CRYPTO_H

#include "DPN_IO.h"
#include <DRSA.h>

namespace DPN::Crypto {
    class IO__Crypto : public DPN::IO::IOContext {
    public:
        DPN_Result process( DPN_ExpandableBuffer &buffer ) override;
        bool checkProcessor() const override;
        DPN_Result processorStart() override;
    private:
        DRSA::Demo::DRSADemo iDemoRSACore;
    };
}


#endif // DPN_CRYPTO_H
