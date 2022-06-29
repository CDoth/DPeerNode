#ifndef DPN_THREADUNIT_H
#define DPN_THREADUNIT_H

#include <atomic>
//------------------------------------------------------- DPN_Direction
class DPN_ThreadUnit {
public:
    friend class DPN_ThreadBridgeData;

    DPN_ThreadUnit();
    virtual ~DPN_ThreadUnit();

    virtual bool threadInjection();
    virtual void fail();


    void kick();
    bool unitProc();


    enum error_type {
        NO_FAIL
        ,CONNECTION_FAIL
        ,PROCESSOR_POOL_FAIL
        ,BAD_PACKET_FORMAT
    };

    inline error_type error() const {return errorState;}
    inline bool processing() const {return inThread;}
    bool isBlocked() const {return blocked;}
    void block();
protected:
    virtual void threadKick();
    virtual bool work() = 0;
protected:
    std::atomic_bool inThread;
    std::atomic_bool blocked;
    error_type errorState;
};
#endif // DPN_THREADUNIT_H
