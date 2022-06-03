#ifndef DPN_FILECLIENT_H
#define DPN_FILECLIENT_H

#include "DPN_FileSystemProcessors.h"
#include "DPN_Client.h"

class DPN_FileClient : public DWatcher<DPN_ClientData> {
public:
    DPN_FileClient(DPN_Client &c);
public:
    void setHostCatalog(DPN_Catalog *catalog);
    void setDownloadPath(const std::string &path);
    bool sendFastFile(const char *filePath);
//    bool sendFastFile(DFile &file);
    DPN_FileSystem & fileSystem();

    bool requestFileset(const DArray<DFileKey> &keyset, DPN_FILESTREAM_STRICT_MODE strictMode = DPN_STRICT, const DArray<DFileKey> *strictMask = nullptr);
    bool requestFile(DFileKey key);
    bool sync();

    bool makeSpecialChannel();
private:
    bool registerFile(DFile &file);
private:
    DPN_FileSystem *pFileSystem;
};


#endif // DPN_FILECLIENT_H
