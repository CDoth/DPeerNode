#ifndef DPN_FILESYSTEMPROCESSORS_H
#define DPN_FILESYSTEMPROCESSORS_H


#include "DPN_TransmitProcessor.h"
#include "DPN_FileSystem.h"



struct __file_metadata {
    __file_metadata() {
        key = -1;
        size = 0;
        strictable = false;
        verified = false;
    }

    DPN_FILESYSTEM::FileMap iFileMap;

    int key;
    size_t size;
    std::string name;
    bool strictable;
    bool verified;
};
template <>
class dpn_mediator<__file_metadata> {
public:
    template <class ... Args>
    static void __do(__file_metadata &file, Args && ... a) {
        __tr::transport(file.iFileMap, a...);
        __tr::transport(file.key, a...);
        __tr::transport(file.size, a...);
        __tr::transport(file.name, a...);
        __tr::transport(file.strictable, a...);
        __tr::transport(file.verified, a...);
    }
    inline static void save(const __file_metadata &file, DPN_ExpandableBuffer &b) {  __do(const_cast<__file_metadata&>(file), b); }
    inline static void upload(__file_metadata &file, const DPN_ExpandableBuffer &b, int &p) { __do(file, b, p); }
};
namespace DPN_FileProcessor {

    class Base : public DPN_TransmitProcessor {
    public:
        Base();
    private:
        virtual void injection() override;
    protected:
        DPN_FileSystemPrivateInterface getInterface( const DPN_ClientTag *ct );
    protected:
        DPN_FileSystem *pFileSystem;
    };
    class SyncCatalogs : public Base {
    public:
        DPN_PROCESSOR
    private:
        DPN_Result request( HOST_CALL );
        DPN_Result processRequest( SERVER_CALL );
        DPN_Result prepare( HOST_CALL );
        DPN_Result sync( SERVER_CALL );
        DPN_Result sync( HOST_CALL );
    private:
        void fault( HOST_CALL ) override;
        void fault( SERVER_CALL ) override;
    private:
        void makeActionLine() override;
    private:
        DPN::SHA256 iHashtool;
    private:
        DECL_UNIT(std::string, UNIT_TOPOLOGY);
        DECL_UNIT(std::string, UNIT_HASH);
        DECL_UNIT(bool, UNIT_ANSWER);
    };
    class RequestFile : public Base {
    public:
        DPN_PROCESSOR;
    public:
        void setFileset( const DArray<int> &keyset ) {
            aKeyset = keyset;
        }
    private:

        DPN_Result hostRequest( HOST_CALL );
        DPN_Result serverProcess( SERVER_CALL );
        DPN_Result hostPrepare( HOST_CALL );
        DPN_Result serverPrepare( SERVER_CALL);
        DPN_Result hostStart( HOST_CALL );
        DPN_Result serverStart( SERVER_CALL );
    private:
        void fault( HOST_CALL ) override;
        void fault( SERVER_CALL ) override;
    private:
        void makeActionLine() override;
    private:
        DECL_UNIT(std::string, UNIT_CATALOG_V);
        DECL_UNIT(DArray<__file_metadata>, UNIT_FILESET);
    private:
        DArray<DFileKey> aKeyset;
        DArray<__file_metadata> aFilesMetaData;
        DArray<DPN_FILESYSTEM::Interface> aFiles;
    };
}
using namespace DPN_FileProcessor;
DPN_PROCESSOR_BIND(SyncCatalogs, DPN_PACKETTYPE__SYNC_CATALOGS);
DPN_PROCESSOR_BIND(RequestFile, DPN_PACKETTYPE__REQUEST_FILE);


/*

class DPN_Processor__request_file : public DPN_FileProcessor {
public:
    DPN_PROCESSOR;
private:
    DPN_Result sendPrefix() override;
    DPN_Result receiveReaction() override;
private:
    DPN_Result failureProcessing() override;
    bool makeHostLine() override;
    bool makeServerLine() override;
private:
    DPN_Result hostRequest();
    DPN_Result serverProcess();
    DPN_Result hostPrepare();
    DPN_Result serverPrepare();
    DPN_Result hostStart();
    DPN_Result serverStart();

private:
    DPN_Result hostFault();
    DPN_Result serverFault();
private:
private:
    UNIT(std::string) UNIT_HOST_CATALOG_V = content;
    UNIT(DArray<__file_metadata>) UNIT_FILESET = content;

private:
    DArray<DFileKey> aKeyset;
    DArray<__file_metadata> aFilesMetaData;
};
*/

#endif // DPN_FILESYSTEMPROCESSORS_H
