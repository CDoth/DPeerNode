#ifndef DPN_DATASTREAMPROCESSORS_H
#define DPN_DATASTREAMPROCESSORS_H


#include "DPN_Modules.h"

class DPN_Processor__reserve_data_channel : public DPN_TransmitProcessor {
public:
    DPN_PROCESSOR;
private:
    DPN_Result sendPrefix() override;
    DPN_Result receiveReaction() override;
private:
};


#endif // DPN_DATASTREAMPROCESSORS_H
