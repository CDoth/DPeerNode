#ifndef DPN_FILESYSTEM_H
#define DPN_FILESYSTEM_H

//------- perephery
#include "DPN_Catalog.h"
#include "DPN_TransportHandler.h"
//---------------------------
#include "DPN_Channel.h"
#include "DPN_Modules.h"
#include "DPN_ClientInterface.h"
#include "DPN_Network.h"








class DPN_FileSystemData
        : public DPN::Client::Core,
        public DPN_FILESYSTEM::SystemKernel
{
public:
    DPN_FileSystemData() :
        DPN_FILESYSTEM::SystemKernel(false)
    {}
    DPN_FileSystemData( DPN::Client::Core c ) :
        DPN::Client::Core(c),
        DPN_FILESYSTEM::SystemKernel(true)
    {
        iRemoteCatalog.createMapFileSystem();
    }
public:

    bool fs__send(DPN_TransmitProcessor *p);
    bool fs__activateChannel( DPN_FILESYSTEM::Channel ch );
    void fs__stop();


    friend class DPN_FileSystemInterface;
    friend class DPN_FileSystemPrivateInterface;

private:
    DPN_Catalog iRemoteCatalog;

    std::string iDownloadPath;

    bool iStateHost2Remote;
    bool iStateRemote2Host;

    std::map<int, DPN_FILESYSTEM::Descriptor> iHostRequestedFiles;
    std::map<int, DPN_FILESYSTEM::Descriptor> iServerRequestedFiles;

    DArray<DPN_FILESYSTEM::Channel> aIncomingChannels;
    DArray<DPN_FILESYSTEM::Channel> aOutgoingChannels;

};

class DPN_FileSystemDescriptor
        : public DPN::Interface::DataReference< DPN_FileSystemData >
{
public:

    DPN_FileSystemDescriptor() {}
    DPN_FileSystemDescriptor( DPN::Client::Core c ) : DPN::Interface::DataReference<DPN_FileSystemData>(true, c) {}

    DPN::Interface::InterfaceReference< DPN_FileSystemData > getPublicInterface();
    DPN::Interface::InterfaceReference< DPN_FileSystemData > getPrivateInterface();

private:
    DPN::Interface::InterfaceCenterReference< DPN_FileSystemData > wPublicInterface;
    DPN::Interface::InterfaceCenterReference< DPN_FileSystemData > wPrivateInterface;

};
class DPN_FileSystemInterface : public DPN::Interface::InterfaceReference< DPN_FileSystemData > {
public:
    bool sync();
    bool requestFileset( const DArray<int> &keyset );
public:
    const DPN_Catalog *remote() const;
};
class DPN_FileSystemPrivateInterface : public DPN::Interface::InterfaceReference< DPN_FileSystemData > {
public:
    DPN_Catalog *remote();
    DFile remoteFile( int key ) const;

    void unverifyRemote2Host();
    void unverifyHost2Remote();
    bool verifyHost2Remote(bool status);
    bool verifyRemote2Host(bool status);
public:
    DPN_FILESYSTEM::Interface request(CallerPosition p, int key );
    bool send( DPN_FILESYSTEM::Interface i );
    bool receive( DPN_FILESYSTEM::Interface i );
public:
    bool addChannel( __channel_mono_interface mono, DPN::Direction direction, const DPN_ExpandableBuffer &context);
};


class DPN_FileSystem :
        public DPN::Network::NetworkModule
{
public:
    bool useChannel( DPN::Client::Tag tag, DPN::Direction d, __channel_mono_interface mono, const DPN_ExpandableBuffer &context) override;
    void clientOver( DPN::Client::Tag tag ) override;
public:
    DPN_FileSystem(const std::string &name, DPN::Network::ClientCenter &cc);
    DPN_Catalog & host();


    DPN_FileSystemInterface getIf(DPN::Client::Tag tag );
    DPN_FileSystemPrivateInterface getPrivateIf( DPN::Client::Tag tag );
    bool compareCatalogHash( const std::string &hash ) const;
private:
    std::map< DPN::Client::Tag, DPN_FileSystemDescriptor> iClientsData;
};
DPN_FileSystem * extractFileModule(DPN::Modules modules);




#endif // DPN_FILESYSTEM_H
