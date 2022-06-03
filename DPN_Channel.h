#ifndef DPN_CHANNEL_H
#define DPN_CHANNEL_H
#include "DPN_Direction.h"

// DPN_ChannelRole
/*
enum DPN_ChannelRole {

    DPN_CHANNEL__NO_ROLE
    ,DPN_CHANNEL__MAIN
    ,DPN_CHANNEL__MONOPOLAR
    ,DPN_CHANNEL__MONOPOLAR_BACKLOG
    ,DPN_CHANNEL__MONOPOLAR_STRICT
};
*/

class DPN_Channel {
public:
    DPN_Channel();

    void init(DPN_NodeConnector *connector,
              bool initiator,
              PeerAddress avatar,
              DPN_Channel *mainChannel,
              const std::string &shadowKey
              );

    void stop();
public:
    bool reserve(DPN_Direction *f, DPN_Direction *b);
    bool reserveForward(DPN_Direction *f);
    bool reserveBack(DPN_Direction *b);
    DPN_Direction * unreserveForward();
    DPN_Direction * unreserveBack();
public:
    inline DPN_NodeConnector * connector() {return __connector;}
    inline DPN_Direction * forwardDirection() {return __forward;}
    inline DPN_Direction * backDirection() {return __back;}
    inline DPN_Direction * direction(int getForward) {return getForward ? __forward : __back;}
public:
    inline bool isInitiator() const {return __isInitiator;}
    inline bool checkKey(const std::string &shadowKey) const {return __shadowKey == shadowKey;}
    inline const std::string & shadowKey() const {return __shadowKey;}

    inline const PeerAddress & local() const {return __local;}
    inline const PeerAddress & remote() const {return __remote;}
    inline const PeerAddress & avatar() const {return __avatar;}

    inline bool isForwardReserved() const {return __forward;}
    inline bool isBackReserved() const {return __back;}
    inline bool isFowardFree() const {return __forward == nullptr;}
    inline bool isBackFree() const {return __back == nullptr;}
private:
    void __clear_ptrs();
private:
    std::string __shadowKey;
    PeerAddress __local;
    PeerAddress __remote;
    PeerAddress __avatar;
    bool __isInitiator;
    DPN_TimeMoment __startMoment;

    DPN_NodeConnector *__connector;
    DPN_Channel *__mainChannel;
    DPN_Direction *__forward;
    DPN_Direction *__back;


    /*
     * > Main channel:
     * Both directions for base transmiting:
     * commands, info or specific modules packets
     *
     * > Duplex channel:
     * Both directions has same role and using
     * for specific (one-role) transmiting:
     * special data such as streams
     *
     * > Monopolar channel:
     * One direction has specific role and using
     * for specific-role transmiting,
     * other direction has no role and free for using and could
     * be registered for any role
     *
     * > Monopolar back-info channel:
     * Both directions has same role but
     * one direction using for specific-role transmiting, and
     * other direction registered for backlog or info
     * for this role
     *
     * > Monopolar strict channel:
     * One direction has specific role and using
     * for specific-role transmiting,
     * other direction blocked for any roles
     * and transmitings
    */
};
DPN_Channel * pickForwardFree(const DArray<DPN_Channel*> &channels);
class DPN_UDPChannel {
public:
    DPN_UDPChannel();
    int peerPort() const;
private:
    DPN_Direction *__forward;
    DPN_Direction *__back;
    DXT socket;
};

#endif // DPN_CHANNEL_H
