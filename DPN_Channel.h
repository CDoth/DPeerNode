#ifndef DPN_CHANNEL_H
#define DPN_CHANNEL_H



#include "DPN_IO.h"
#include "DPN_NodeConnector.h"

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
    bool reserve( DPN_AbstractModule *module, const DPN_ExpandableBuffer &context );
private:
    DPN_AbstractModule *pModule;
    DPN_ExpandableBuffer wSettings;
    dpn_direction iDirectionType;
    MonoChannelState iLocalState;
    MonoChannelState iRemoteState;
};
class IO__CHANNEL : public DPN_IO::IOContext {
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
class __channel_data {
public:
    friend class __channel;
    friend class __channel_mono_interface;
    friend class __channel_private_interface;
    __channel_data();
    ~__channel_data();
    bool init( DPN_NodeConnector *c, const std::string &shadowKey );
    inline bool inited() const {return iKey.size();}
    bool reserve( DPN_AbstractModule *m, const DPN_ExpandableBuffer &context );

    inline bool checkKey(const std::string &shadowKey) const {return iKey.size() && iKey == shadowKey;}
    inline const std::string &key() const {return iKey;}
private:
    std::string iKey;
    IO__CHANNEL *pChannel;
private:
    MonoChannel iForward;
    MonoChannel iBackward;
};
class __channel_mono_interface : public __dpn_acc_interface<dpn_direction, __channel_data> {
public:
    __channel_mono_interface() {}
    inline DPN_IO::IOContext * io() {return validInterface() ? inner()->pChannel : nullptr;}
};
class __channel_private_interface : public __dpn_interface< __channel_data > {
public:
    void setLocalState( dpn_direction d, MonoChannelState s );
    void setRemoteState( dpn_direction d, MonoChannelState s );
};
class __channel : private DWatcher<__channel_data> {
public:
    __channel();
    inline bool isValid() const { return this->isCreatedObject(); }

    bool init(DPN_NodeConnector *c, const std::string &shadowKey);
    __channel_mono_interface getMonoIf( dpn_direction d );
    __channel_private_interface privateInterface();
public:
    MonoChannelState localState( dpn_direction d ) const;
    MonoChannelState remoteState( dpn_direction d ) const;
    std::string shadowKey() const;
private:
    __interface_map<dpn_direction, __channel_data> monoIf;
    __interface_master<__channel_data> privateIf;
};
//===================================










#endif // DPN_CHANNEL_H
