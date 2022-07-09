#ifndef DPN_CHANNEL_H
#define DPN_CHANNEL_H



#include "DPN_IO.h"
#include "DPN_NodeConnector.h"
#include "DPN_Interface.h"

namespace DPN::Channel {

}

enum MonoChannelState {
    MS__RAW,
    MS__BOUND,
    MS__ACTIVE,
    MS__REQUESTING
};
class MonoChannel {
public:
    friend class __channel;
    friend class __channel_private_interface;
    MonoChannel();
private:
    DPN_ExpandableBuffer wSettings;
    DPN::Direction iDirectionType;
    MonoChannelState iLocalState;
    MonoChannelState iRemoteState;
};
class IO__CHANNEL : public DPN::IO::IOContext {
public:
    IO__CHANNEL(DPN_NodeConnector *c);
    bool init(DPN_NodeConnector *connector);
private:
    bool checkGenerator() const override;
    bool checkProcessor() const override;
    DPN_Result generatorStart() override;
    DPN_Result processorStart() override;
    DPN_Result generate(DPN_ExpandableBuffer &b) override;
    DPN_Result process(DPN_ExpandableBuffer &b) override;

private:
    DPN_NodeConnector *pConnector;
    DPN_ExpandableBuffer wBuffer;
};
//=========================================================================================================== Data:
class __channel_data {
public:
    friend class __channel;
    friend class __channel_mono_interface;
    friend class __channel_private_interface;
    __channel_data();
    ~__channel_data();
    bool init( DPN_NodeConnector *c, const std::string &shadowKey );
    inline bool inited() const { return iKey.size() && pChannel; }

    inline bool checkKey(const std::string &shadowKey) const {return iKey.size() && iKey == shadowKey;}
    inline const std::string &key() const {return iKey;}
private:
    std::string iKey;
    IO__CHANNEL *pChannel;
private:
    MonoChannel iForward;
    MonoChannel iBackward;
};

//=========================================================================================================== Interfaces:
class __channel_mono_interface
        : public DPN::Interface::InterfaceReference< __channel_data >
{
public:
    DPN::IO::IOContext *io();
};
class __channel_private_interface
        : public DPN::Interface::InterfaceReference< __channel_data >
{
public:
    void setLocalState( DPN::Direction d, MonoChannelState s );
    void setRemoteState( DPN::Direction d, MonoChannelState s );
};
//=========================================================================================================== Descriptor:
class __channel
        : private DPN::Interface::DataReference< __channel_data >
{
public:
    __channel();

    bool isReady() const;
    bool init(DPN_NodeConnector *c, const std::string &shadowKey);
    __channel_mono_interface getMonoIf( DPN::Direction d );
    __channel_private_interface privateInterface();
public:
    MonoChannelState localState( DPN::Direction d ) const;
    MonoChannelState remoteState( DPN::Direction d ) const;
    std::string shadowKey() const;
private:
    DPN::Interface::InterfaceMapCenterReference< DPN::Direction, __channel_data> wMonoInterfaces;
    DPN::Interface::InterfaceCenterReference< __channel_data> wPrivateInterface;
};

//===================================










#endif // DPN_CHANNEL_H
