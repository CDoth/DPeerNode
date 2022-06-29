#ifndef DPN_IO_H
#define DPN_IO_H




#include <DPN_Buffers.h>

namespace DPN_IO {

typedef bool (*GenerativeCallback)(void *base, DPN_ExpandableBuffer &emptyBuffer);
typedef bool (*TargetCallback)(void *base, const void *data, int size);

class IOContext  {
public:
    friend class PropagationScheme;
//    friend class PropagationNode;
    IOContext();
    virtual ~IOContext();
public:
    virtual DPN_Result generate(DPN_ExpandableBuffer &buffer);
    virtual DPN_Result process(DPN_ExpandableBuffer &buffer);
    virtual inline bool checkGenerator() const {return true;}
    virtual inline bool checkProcessor() const {return true;}

    virtual inline DPN_Result generatorStart() {return DPN_SUCCESS;}
    virtual inline DPN_Result processorStart() {return DPN_SUCCESS;}
};
class IO__CALLBACK : public IOContext {
public:
    IO__CALLBACK();

    DPN_Result generate(DPN_ExpandableBuffer &b) override;
    DPN_Result generatorStart() override;
    bool checkGenerator() const override;

    DPN_Result process(DPN_ExpandableBuffer &b) override;
    DPN_Result processorStart() override;
    bool checkProcessor() const override;
private:
    void *pTargetBase;
    TargetCallback pTargetCallback;

    void *pGenerativeBase;
    GenerativeCallback pGenerativeCallback;
    DPN_ExpandableBuffer wGeneratedData;
};



}

#endif // DPN_IO_H
