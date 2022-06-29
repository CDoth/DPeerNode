#ifndef DPN_FILESYSTEM_H
#define DPN_FILESYSTEM_H

//------- perephery
#include "DPN_Catalog.h"
#include "DPN_TransportHandler.h"
//---------------------------
#include "DPN_Channel.h"
#include "DPN_Modules.h"
#include "DPN_ClientData.h"







class DPN_FileSystemData
        : public DWatcher<DPN_ClientCore>,
        public DPN_FILESYSTEM::SystemKernel
{
public:
    DPN_FileSystemData() : DPN_FILESYSTEM::SystemKernel(true) {
        iRemoteCatalog.createMapFileSystem();
    }
    DPN_FileSystemData( DWatcher<DPN_ClientCore> &w ) : DPN_FILESYSTEM::SystemKernel(true) {

        DWatcher<DPN_ClientCore>::copy( w );

        iRemoteCatalog.createMapFileSystem();
    }

    bool fs__send(DPN_TransmitProcessor *p);
    bool fs__activateChannel( DPN_FILESYSTEM::Channel ch );
    bool isBound() const { return DWatcher<DPN_ClientCore>::isCreatedObject(); }


    friend class DPN_FileSystemInterface;
    friend class DPN_FileSystemPrivateInterface;

    DPN_Catalog iRemoteCatalog;

    std::string iDownloadPath;

    bool iStateHost2Remote;
    bool iStateRemote2Host;

    DArray<DPN_FILESYSTEM::Descriptor> iHostRequest;
    DArray<DPN_FILESYSTEM::Descriptor> iServerRequest;

    __interface_map<int, DPN_FILESYSTEM::Data> __host_interface;
    __interface_map<int, DPN_FILESYSTEM::Data> __server_interface;

    DArray<DPN_FILESYSTEM::Channel> aIncomingChannels;
    DArray<DPN_FILESYSTEM::Channel> aOutgoingChannels;

};
class DPN_FileSystemDescriptor : public DWatcher<DPN_FileSystemData> {
public:
    DPN_FileSystemDescriptor() : DWatcher<DPN_FileSystemData>(true) {}
    DPN_FileSystemDescriptor( DWatcher<DPN_ClientCore> &w ) : DWatcher<DPN_FileSystemData>(true, w) {}
    bool isClientBound() const {
        if( isEmptyObject() ) return false;
        return data()->isBound();
    }
    void bind( DWatcher<DPN_ClientCore> &w) {
        if( isEmptyObject() ) return;
        data()->DWatcher<DPN_ClientCore>::copy( w );
    }

};
class DPN_FileSystemInterface : public __dpn_acc_interface< const DPN_ClientTag*, DPN_FileSystemData > {
public:
    bool sync();
    bool requestFileset( const DArray<int> &keyset );
public:
    const DPN_Catalog *remote() const;
};
class DPN_FileSystemPrivateInterface : public __dpn_acc_interface< const DPN_ClientTag*, DPN_FileSystemData > {
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
    bool addChannel( __channel_mono_interface mono, dpn_direction direction, const DPN_ExpandableBuffer &context);
};

class DPN_FileSystem : public DPN_AbstractModule {
public:
    bool useChannel(const DPN_ClientTag *tag, dpn_direction d, __channel_mono_interface mono, const DPN_ExpandableBuffer &context) override;
public:
    DPN_FileSystem(const std::string &name);
    DPN_Catalog & host();


    DPN_FileSystemInterface getIf( DPN_ClientInterface &clientIf );
//private:
    DPN_FileSystemPrivateInterface getPrivateIf( const DPN_ClientTag *clientTag );
    bool compareCatalogHash( const std::string &hash ) const;
private:
    std::map< const DPN_ClientTag*, DPN_FileSystemDescriptor> iClientsData;
    __interface_map<const DPN_ClientTag*, DPN_FileSystemData> im;
    __interface_map<const DPN_ClientTag*, DPN_FileSystemData> imPrivite;
};
DPN_FileSystem * extractFileModule(DPN_Modules &modules);



/*


class DPN_FileDirection : public DPN_Direction {
public:
    DPN_FileDirection();

    enum {header_size = 2 * sizeof(uint32_t)};

protected:
    DPN_ExpandableBuffer buffer;
    DPN_FileSystem *pFileSystem;
    DPN_NodeConnector *pConnector;
};

class DPN_FileSendDirection : public DPN_FileDirection {
public:

    DPN_FileSendDirection();
    friend class DPN_FileSystem;
    void send(DPN_FileSlice *slice);
//    void send(const DArray<DPN_FileSlice *> &sliceset);
    bool proc() override;
    bool close() override {return true;}
    inline int size() const {return iSize;}
private:
    void read();

    void session__integrate(DPN_FileSlice *slice);
    void session__integrate(const DArray<DPN_FileSlice *> &sliceset);
    void session__accept();
    void session__pop();
    void session__skip();
    DPN_FileSlice * session__get();
private:

//    DPN_SimplePtrList<DPN_FileTransportHandler> q;

    std::mutex mu;
    DPN_SimplePtrList<DPN_FileSlice> queue;
    DPN_SimplePtrList<DPN_FileSlice> session;
    DPN_FileSlice *s_current;
    int iQueueSize;
    int iSize;
    int iRead_b;
};
class DPN_FileReceiveDirection : public DPN_FileDirection {
public:
    friend class DPN_FileSystem;
    DPN_FileReceiveDirection();
    bool proc() override;
    bool close() override {return true;}

    struct header {
        uint32_t size;
        uint32_t key;
    };

private:
    DPN_FileTransportHandler *current;
    header __header;
};


struct DPN_RequestNote {
public:
    friend class DPN_FileSystem;
    DPN_RequestNote();
    inline int remoteNote() const {return remote;}
    DArray<DFileKey> keyset;
private:
    enum DIRECTION {
        NO_DIRECTION
        ,SENDING
        ,RECEIVING
    } direciton;
    bool done;
    int remote;
};
enum DPN_FILESTREAM_STRICT_MODE {
    DPN_NO_STRICT
    ,DPN_STRICT
    ,DPN_STRICT_SELECTIVE
};
class DPN_FileSystem : public DPN_AbstractModule {
public:
    DPN_FileSystem();
    ~DPN_FileSystem();

public:
    const std::string & downloadPath() const {return __downloadPath;}
    void setHostCatalog(DPN_Catalog *catalog);

//    bool reserveShadowSender(DPN_Channel *channel, int transaction) override;
//    bool reserveShadowReceiver(DPN_Channel *channel, const DPN_ExpandableBuffer &extra, int transaction) override;

    void clientDisconnected(const DPN_AbstractClient *client) override;
    void stop() override;
    //=============================================
    inline DPN_Catalog & remote() {return remote_catalog; }
    inline const DPN_Catalog * host() const {return host_catalog;}
    inline DPN_Catalog * host() {return host_catalog;}
    bool checkRemoteFile(DFileKey key);
    void unverifyRemote2Host();
    void unverifyHost2Remote();
    bool verifyHost2Remote(const std::string &remoteHashSum);
    bool verifyRemote2Host(bool status);
    void setDownloadPath(const std::string &path);

    //---------------------------------------
    DFile hostFile(DFileKey key);
    const DFile & hostFile(DFileKey key) const;
    DFile remoteFile(DFileKey key);
    const DFile & remoteFile(DFileKey key) const;
    //---------------------------------------
    DPN_FileTransportHandler * receivingFile(int key) const { return mainSession.getReceiveHandler(key); }
    DPN_FileTransportHandler * sendingFile(int key) const { return mainSession.getSendHandler(key); }
    inline bool autoReceiving() const {return autoreceive_fast_files;}
    //---------------------------------------
    //---------------------------------------
    DPN_FileTransportHandler * startReceiveFile(int key);
    DPN_FileTransportHandler * startSendFile(int key);
    bool stopReceiveFileset(const DArray<DFileKey> &keyset);
    bool stopReceiveFile(DFileKey key);
    bool stopReceive(int note_key);

    bool stopSendFileset(const DArray<DFileKey> &keyset);
    bool stopSendFile(DFileKey key);
    bool stopSend(int note_key);

    bool unregisterFastFileset(const DArray<int> &keyset, bool onHost);
    bool unregisterFastFile(DFileKey key, bool onHost);


    bool registerRemoteFile(DFile &file, DFileKey key);
    bool registerLocalFile(DFile &file);
    //---------------------------------------
    bool unregisterFile(DFileKey key);



    DPN_RequestNote * getRequestNote(int note_key);


    bool send(const DArray<DFileKey> &keyset);


    bool unsreserveSender(const std::string &shadowKey);
    bool unsreserveReceiver(const std::string &shadowKey);
//    bool reserveSender(DPN_Channel *ch);
//    bool reserveReceiver(DPN_Channel *ch);
    bool hasSpecialChannels() const;

public:
    inline int makeReceiveNote(DFileKey key) {return makeRequestNote(key, DPN_RequestNote::RECEIVING);}
    inline int makeReceiveNote(const DArray<DFileKey> &keyset) {return makeRequestNote(keyset, DPN_RequestNote::RECEIVING);}
    inline int makeSendNote(DFileKey key) {return makeRequestNote(key, DPN_RequestNote::SENDING);}
    inline int makeSendNote(const DArray<DFileKey> &keyset) {return makeRequestNote(keyset, DPN_RequestNote::SENDING);}
    bool bindRemoteNote(int local_note, int remote_note);
private:
    int makeRequestNote(DFileKey key, DPN_RequestNote::DIRECTION d);
    int makeRequestNote(const DArray<DFileKey> &keyset, DPN_RequestNote::DIRECTION d);
    bool send(DFileKey key);

private:

    DPN_Catalog remote_catalog;
    DPN_Catalog *host_catalog;
    DPN_FileSessionManager mainSession;
    std::string __downloadPath;
    bool host2remote_verified;
    bool remote2host_verified;
    bool autoreceive_fast_files;
    int clientFileBufferSize;
    DArray<DPN_RequestNote> __requests;
    DPN_SHA256 hashtool;

    DArray<DPN_FileSendDirection*> send_directions;
};
*/

#endif // DPN_FILESYSTEM_H
