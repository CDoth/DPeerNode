#ifndef DPN_DIRECTION_H
#define DPN_DIRECTION_H

#include "DPN_TransmitProcessor.h"

#include <atomic>
//------------------------------------------------------- DPN_Direction
class DPN_Direction {
public:
    friend class DPN_ThreadBridgeData;
    DPN_ClientContext clientContext;
    DPN_ThreadContext threadContext;

    DPN_Direction();
    virtual ~DPN_Direction();

    virtual bool threadInjection();
    virtual bool proc() = 0;
    virtual bool close() = 0;

    enum error_type {
        NO_FAIL
        ,CONNECTION_FAIL
        ,PROCESSOR_POOL_FAIL
        ,BAD_PACKET_FORMAT
    };

    inline error_type error() const {return errorState;}
    inline bool processing() const {return inThread;}
    bool isBlocked() const {return clientContext.isClientBlocked() || blocked;}

    void block();
protected:
    std::atomic_bool inThread;
    std::atomic_bool blocked;
    error_type errorState;
};
//------------------------------------------------------- DPN_SendDirection
class DPN_SendDirection : public DPN_Direction {
public:
    DPN_SendDirection();
    ~DPN_SendDirection();
    void putProcessor(DPN_TransmitProcessor *proc);
public:
    bool proc() override;
    bool close() override;
private:
    DPN_ProcessorList queue;
};
//------------------------------------------------------- DPN_ReceiveDirection
struct packet_header {
  uint32_t size;
  union {
    uint32_t __packet_type;
    DPN_PacketType type;
  };
//  uint32_t transactionIndex;
};
class Receiver {
public:
    Receiver(DPN_Direction *d);
    inline void restart() {line.restart();}
    DPN_Result receive();
    inline DPN_PacketType type() const {return r_header.type;}
    inline uint32_t size() const {return r_header.size;}
private:
    DPN_Result recv_header();
    DPN_Result recv_packet();
private:
    void makeLine();
    bool checkType();
    bool checkSize();
private:
    DPN_Direction *direction;
    packet_header r_header;
private:
    __action_line<Receiver> line;
};
class DPN_ReceiveDirection : public DPN_Direction {
public:
    DPN_ReceiveDirection();
public:
    bool proc() override;
    bool close() override;
private:
    Receiver R;
    DPN_TransmitProcessor *current_process;
};
//------------------------------------------------------- DPN_ProxyDirection
/*
class DPN_ProxyDirection : public DPN_Direction {
public:
    DPN_ProxyDirection(DPN_NodeConnector *from, DPN_NodeConnector *to);
    bool proc() override;
    bool close() override;
private:
    void __clean();
private:
    DPN_NodeConnector *__from;
    DPN_NodeConnector *__to;
    bool __sendPhase;
};
//-------------------------------------------------------
*/
//------------------------------------------------------- old
/*
class DPN_Direction {
public:
    friend class DPN_ClientData;
    friend class DPN_ThreadBridgeData;
    friend class DPN_ThreadBridge;

    enum {
        NO_DIRECTION = -1
        ,FORWARD
        ,BACK
    };

    void setClientContext(DPN_ClientContext &cc) {
        clientContext = cc;
    }
    void setThreadContext(DPN_ThreadContext &tc) {
        threadContext = tc;
    }
    DPN_ClientContext & cc() {return clientContext;}
    DPN_ThreadContext & tc() {return threadContext;}


    DPN_Direction();
    virtual ~DPN_Direction();
    virtual bool proc() = 0;
    virtual bool close() = 0;
    enum DPN_DirectionError {
        NO_FAIL
        ,CONNECTION_FAIL
        ,PROCESSOR_POOL_FAIL
        ,BAD_PACKET_FORMAT
    };

    DPN_DirectionError error() const {return errorState;}
    const DPN_NodeConnector * connector() const;


public:
    void blockDirection() {__blockDirection();}
    void blockClient() {__blockClient();}
    bool isBlocked() const {return __isClientBlocked() || __isDirectionBlocked();}
    void disconnectClient();
    inline bool processing() const {return inThread;}


protected:
    void __blockClient() {clientContext.blockClient();}
    void __unblockClient() {clientContext.unblockClient();}
    bool __isClientBlocked() const {return clientContext.isClientBlocked();}

    void __blockDirection() {blocked = true;}
    void __unblockDirection() {blocked = false;}
    bool __isDirectionBlocked() const {return blocked;}

protected:
    std::atomic_bool inThread;
    std::atomic_bool blocked;
    DPN_DirectionError errorState;

    DPN_ClientContext clientContext;
    DPN_ThreadContext threadContext;
};

//-------------------------------------------------------
class DPN_BaseDirection : public DPN_Direction {
public:
private:
};
*/





#endif // DPN_DIRECTION_H
