#include "DPN_Propagation.h"


using namespace DPeerNodeSpace;

namespace DPN_Propagation {
    bool Node::innerCheck() {
        return iGenerator ? IO->checkGenerator() : IO->checkProcessor();
    }
    DPN_Result Node::innerStart() {
        return iGenerator ? IO->generatorStart() : IO->processorStart();
    }
    DPN_Result Node::start() {
        if( innerCheck() == false ) {
            return DPN_FAIL;
        }
        return innerStart();
    }
    LinearScheme::LinearScheme() {
        iEntry.makeGenerator();
        iReady = false;
        iStopNode = -1;
    }
    bool LinearScheme::work() {
        return propagate();
    }
    bool LinearScheme::setEntry(DPN_IO::IOContext *io) {
        if( io == nullptr ) {
            return false;
        }
        iEntry.set( io );
        return true;
    }
    bool LinearScheme::connect(DPN_IO::IOContext *io) {

        if( iEntry.isValid() == false || io == nullptr ) {
            return false;
        }
        Node node(io);
        iLine.append( node );
        return true;
    }
    bool LinearScheme::propagate() {

        if( iReady ) {

            wBuffer.clear();
            switch (iEntry.generate(wBuffer)) {
                case DPN_FAIL: return false;
                case DPN_REPEAT: break;
                case DPN_SUCCESS: break;
            }
            auto b = iLine.begin();
            auto e = iLine.end();
            while( b != e ) {
                switch ( (b++)->process(wBuffer) ) {
                    case DPN_FAIL: return false;
                    case DPN_REPEAT: break;
                    case DPN_SUCCESS: break;
                }
            }


        } else {
            return iReady = diagnostics();
        }
        return true;
    }
    bool LinearScheme::diagnostics() {


        DL_INFO(1, "linear: [%p] entry: [%p] line size: [%d]", this, iEntry.io(), iLine.size());
        auto b = iLine.begin();
        auto e = iLine.end();
        Node *__target = e-1;

        int i = iLine.size();
        while( b != e ) {
            e--;
            i--;

            DL_INFO(1, "check node: [%p]", e->io());

            if( __target == nullptr ) {
                switch ( e->start()) {
                case DPN_FAIL:
                    DL_INFO(1, "node [%p] start fail", e->io());
                    return false;
                    break;
                case DPN_REPEAT:
                    DL_INFO(1, "node [%p] start repeat", e->io());
                    iStopNode = i;
                    __target = nullptr;
                    break;
                case DPN_SUCCESS:
                    DL_INFO(1, "node [%p] ready!", e->io());
                    break;
                }
            } else {

            }
        }

        switch( iEntry.start() ) {
        case DPN_FAIL:
            DL_INFO(1, "entry [%p] start fail", iEntry.io());
            return false;
            break;
        case DPN_REPEAT:
            DL_INFO(1, "entry [%p] start repeat", iEntry.io());
            iStopNode = 0;
            break;
        case DPN_SUCCESS:
            DL_INFO(1, "entry [%p] ready!", iEntry.io());
            break;
        }


        return true;
    }


}
