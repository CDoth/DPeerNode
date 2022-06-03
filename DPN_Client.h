#ifndef DPN_CLIENT_H
#define DPN_CLIENT_H

#include "DPN_ClientData.h"


class DPN_Client : public DWatcher<DPN_ClientData> {
public:
    friend class DPN_ConnectionsCore;
    DPN_Client(const ClientInitContext &initContext);
    ~DPN_Client();
    bool disconnect();
    bool sendMessage(const char *message);
    void ping();
    void shadow();
    ClientState state() const;
    const std::string name() const;
    const PeerAddress & local() const;
    const PeerAddress & remote() const;
    bool isLocalVisible() const;

    void requestEnvironmet();

    void addModule(const std::string &name, DPN_AbstractModule *module);

public:
    DPN_AbstractClient *getAbstract();
    const DArray<DPN_Channel*> & channels() const;
private:
    void setThreadContext(DPN_ThreadContext &tc);
};


/*
class DPN_FileClient : public DWatcher<DPN_ClientData> {
public:
    DPN_FileClient(DPN_Client &client);
public:
    void setHostCatalog(DPN_Catalog *catalog);
    void setDownloadPath(const std::string &path);
    bool sendFastFile(const char *filePath);
    bool sendFastFile(DFile &file);
    DPN_FileSystem & fileSystem();

    bool requestFileset(const DArray<DFileKey> &keyset, DPN_FILESTREAM_STRICT_MODE strictMode = DPN_STRICT, const DArray<DFileKey> *strictMask = nullptr);
    bool requestFile(DFileKey key);
    bool sync();
private:
    bool registerFile(DFile &file);
private:
    DPN_FileSystem *pClientFileSystem;
};

*/

#endif // DPN_CLIENT_H
