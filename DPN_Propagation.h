#ifndef DPN_PROPAGATION_H
#define DPN_PROPAGATION_H

#include "DPN_IO.h"
#include "DPN_ThreadUnit.h"

//=====================================================================

namespace DPN_Propagation {

    class Node {
    public:
        friend class LinearScheme;

        Node( DPN::IO::IOContext *io = nullptr ) : IO(io), iGenerator(false) {}
        void set( DPN::IO::IOContext *io ) { IO = io; }
        void makeGenerator() { iGenerator = true; }
        inline bool isValid() const { return IO; }

        const DPN::IO::IOContext *io() const {return IO;}

    private:
        bool started();
        DPN_Result innerStart();
        bool innerCheck();
        DPN_Result start();
        inline DPN_Result generate(DPN_ExpandableBuffer &b) {return IO->generate(b);}
        inline DPN_Result process(DPN_ExpandableBuffer &b) {return IO->process(b);}
    private:
        DPN::IO::IOContext *IO;
        bool iGenerator;
    };
    class LinearScheme : public DPN::Thread::AbstractThreadUnit {
    public:
        LinearScheme();
        bool work() override;
        bool setEntry( DPN::IO::IOContext *io );
        bool connect(DPN::IO::IOContext *io );
        bool propagate();
    private:
        bool diagnostics();
    private:
        DPN_ExpandableBuffer wBuffer;
        Node iEntry;
        DArray<Node, Direct> iLine;
        bool iReady;
        int iStopNode;
    };
}




#endif // DPN_PROPAGATION_H
