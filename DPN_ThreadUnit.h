#ifndef DPN_THREADUNIT_H
#define DPN_THREADUNIT_H

#include <atomic>
#include "__dpeernode_global.h"

/*
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
*/
namespace DPN::Thread {
    class AbstractThreadUnit {
    public:
        AbstractThreadUnit();
        virtual ~AbstractThreadUnit();
        void block();
        void kick();
        bool unitProc();
    public:
        virtual bool threadInjection();
        virtual void fail();

        inline bool isBlocked() const {return blocked;}
        inline bool processing() const {return inThread;}
    protected:
        virtual void threadKick();
        virtual bool work() = 0;
    private:
        std::atomic_bool inThread;
        std::atomic_bool blocked;
    };
    class AbstractPlug : public AbstractThreadUnit { bool work() override {return false;} };

    class ThreadUnit : private DWatcher< AbstractPlug > {
    public:
        template <class TUImpl> friend class ThreadUnitWrapper;
        using DWatcher< AbstractPlug >::DWatcher;
        using DWatcher< AbstractPlug>::isEmptyObject;


        inline bool processing() const { return data()->processing(); }
        inline bool isBlocked() const { return data()->isBlocked(); }
        inline bool unitProc() { return data()->unitProc(); }

        void block();
        void kick();
    };
    template <class TUImpl>
    class ThreadUnitWrapper
            : private ThreadUnit
    {
    public:
        using ThreadUnit::isEmptyObject;

        ThreadUnitWrapper();

        ThreadUnitWrapper( bool makeSource ) {
            if( makeSource ) {
                this->createInner<TUImpl>();
            }
        }
        template <class ... Args>
        ThreadUnitWrapper( bool makeSource, Args && ... a ) {
            if( makeSource ) {
                this->createInner<TUImpl>(a...);
            }
        }

        ThreadUnit &unit() { return *this; }
        const ThreadUnit &unit() const { return *this; }
        TUImpl * data() { return reinterpret_cast<TUImpl*>( ThreadUnit::data() );}
        const TUImpl * data() const { return reinterpret_cast<const TUImpl*>( ThreadUnit::data() );}
    };
}
#endif // DPN_THREADUNIT_H
