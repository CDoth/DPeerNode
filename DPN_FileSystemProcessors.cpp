#include "DPN_FileSystemProcessors.h"



using namespace DPN::Logs;
namespace DPN_FileProcessor {

    //================================================================================= Base
    Base::Base() {
        pFileSystem = nullptr;
    }
    void Base::injection() {

        pFileSystem = extractFileModule( *this );
        DL_INFO(1, "file system: [%p]", pFileSystem);
    }
    DPN_FileSystemPrivateInterface Base::getInterface(DPN::Client::Tag ct) {

        pFileSystem = extractFileModule( *this );

        DL_INFO(1, "Extract file module [%p] processor: [%p]", pFileSystem, this);

        if( pFileSystem == nullptr ) {
            DL_BADPOINTER(1, "File system");
            return DPN_FileSystemPrivateInterface();
        }
        return pFileSystem->getPrivateIf( ct );
    }
    //================================================================================= SyncCatalogs
    DPN_Result SyncCatalogs::request(HOST_CALL) {
        INFO_LOG;
        DL_INFO(1, "SyncCatalogs::request: file system: [%p] proc: [%p]", pFileSystem, this);
        return DPN_SUCCESS;
    }
    DPN_Result SyncCatalogs::processRequest(SERVER_CALL) {
        INFO_LOG;
        DL_INFO(1, "SyncCatalogs::processRequest: file system: [%p] proc: [%p]", pFileSystem, this);
//        return DPN_SUCCESS;

        if( pFileSystem == nullptr ) {
            DL_BADPOINTER(1, "File system");
            return DPN_FAIL;
        }

        auto &host_catalog = pFileSystem->host();

        host_catalog.renew();
        std::string topology = host_catalog.topology(true);
        if(topology.empty()) {
            DL_ERROR(1, "Empty topology");
            return DPN_FAIL;
        }

        DPN_FileSystemPrivateInterface i = getInterface( tag() ) ;
        if( i.badInterface() ) {
            DL_ERROR(1, "Bad interface");
            return DPN_FAIL;
        }
        i.unverifyHost2Remote();

        UNIT_TOPOLOGY = topology;

        return DPN_SUCCESS;
    }
    DPN_Result SyncCatalogs::prepare(HOST_CALL) {
        INFO_LOG;

        if( pFileSystem == nullptr ) {
            DL_BADPOINTER(1, "File system");
            return DPN_FAIL;
        }

        DPN_FileSystemPrivateInterface i = getInterface( tag() ) ;
        if( i.badInterface() ) {
            DL_ERROR(1, "Bad interface");
            return DPN_FAIL;
        }
        i.unverifyRemote2Host();
        std::string remote_topology = UNIT_TOPOLOGY.get();

        DL_INFO(1, "File system: [%p] client: [%p] topology: [%d]",
                pFileSystem, tag(), remote_topology.size());

        std::string host_topology = i.remote()->sync( remote_topology );

        iHashtool.hash_string( host_topology );
        UNIT_HASH = iHashtool.get();

        return DPN_SUCCESS;
    }
    DPN_Result SyncCatalogs::sync(SERVER_CALL) {
        INFO_LOG;
        DPN_FileSystemPrivateInterface i = getInterface( tag() ) ;
        if( i.badInterface() ) {
            DL_ERROR(1, "Bad interface");
            return DPN_FAIL;
        }
        std::string hash = UNIT_HASH.get();

        DL_INFO(1, "remote hash: [%s], try verify...", hash.c_str());
        bool v = pFileSystem->compareCatalogHash( hash );
        i.verifyHost2Remote( v );
        UNIT_ANSWER = v;
        return DPN_SUCCESS;
    }
    DPN_Result SyncCatalogs::sync(HOST_CALL) {
        INFO_LOG;
        DPN_FileSystemPrivateInterface i = getInterface( tag() ) ;
        if( i.badInterface() ) {
            DL_ERROR(1, "Bad interface");
            return DPN_FAIL;
        }
        bool v = i.verifyRemote2Host( UNIT_ANSWER.get() );
        DL_INFO(1, "Catalog verification: [%d]", v);
        return DPN_SUCCESS;
    }
    void SyncCatalogs::fault(HOST_CALL) {
        INFO_LOG;
    }
    void SyncCatalogs::fault(SERVER_CALL) {
        INFO_LOG;
    }
    void SyncCatalogs::makeActionLine() {

        line< SyncCatalogs> ()
                << &SyncCatalogs::request
                << &SyncCatalogs::processRequest
                << &SyncCatalogs::prepare
                << ServerCallbackWrapper( &SyncCatalogs::sync )
                << HostCallbackWrapper( &SyncCatalogs::sync )
                   ;
    }
    //================================================================================= RequestFile
    DPN_Result RequestFile::hostRequest(HOST_CALL) {
        INFO_LOG;

        if( pFileSystem == nullptr ) {
            DL_BADPOINTER(1, "File system");
            return DPN_FAIL;
        }
        if( aKeyset.empty() ) {
            DL_ERROR(1, "Empty keyset");
            return DPN_FAIL;
        }

        DL_INFO(1, "keyset: size: [%d] array: [%s]", aKeyset.size(), DPN::ia2s( aKeyset ).c_str());


        int key = 0;
        bool catalogDepended = false;
        std::string __hash;


        /*
        DPN_FileSystemPrivateInterface ifc = getInterface( tag() ) ;
        if( ifc.badInterface() ) {
            DL_ERROR(1, "Bad interface");
            return DPN_FAIL;
        }

        aFilesMetaData.clear();
        FOR_VALUE( aKeyset.size(), i ) {
            __file_metadata md;
            key = aKeyset[i];

            DL_INFO(1, "proc file: [%d]", key);
            DFile file = ifc.remoteFile( key );
            if( catalogDepended == false && file.parent() ) catalogDepended = true;

            if( BADFILE(file) ) {
                DL_BADVALUE(1, "file: key: [%d]", key);
                return DPN_FAIL;
            }
            DPN_FILESYSTEM::Interface fileIf = ifc.request( DPN_HOST, key );
            if( fileIf.badInterface() ) {
                DL_ERROR(1, "Bad file [%d] interface", key);
                return DPN_FAIL;
            }
            DPN_FILESYSTEM::FileMap fmap = fileIf.fileMap();
            if( fmap.empty() ) {
                DL_ERROR(1, "Empty file map. key: [%d]", key);
                return DPN_FAIL;
            }

            md.key = key;
            md.name = file.name();
            md.size = file.size();
            md.iFileMap = fmap;

            aFiles.append( fileIf );

            DL_INFO(1, "File metadata: key: [%d] name: [%s] size: [%d] map size: [%d]",
                    md.key, md.name.c_str(), md.size, md.iFileMap.size());

            std::cout << DPN_FILESYSTEM::filemap2text( fmap ) << std::endl;


            aFilesMetaData.append( md );
        }

        if( aFilesMetaData.empty() ) {
            DL_ERROR(1, "No files to receive");
            return DPN_FAIL;
        }
        if( catalogDepended ) {
            UNIT_CATALOG_V = ifc.remote()->getHash();
        }
        else
            UNIT_CATALOG_V.clearBuffer();

        UNIT_FILESET = aFilesMetaData;

        DL_INFO(1, "Successful host file requesting. aFilesMetaData: [%d] catalog v: [%s]",
                aFilesMetaData.size(), UNIT_CATALOG_V.get().c_str());


        */
        return DPN_SUCCESS;
    }
    DPN_Result RequestFile::serverProcess(SERVER_CALL) {
        INFO_LOG;
        if( pFileSystem == nullptr ) {
            DL_BADPOINTER(1, "File system");
            return DPN_FAIL;
        }
        aFilesMetaData = UNIT_FILESET.get();



        /*
        DPN_FileSystemPrivateInterface ifc = getInterface( tag() ) ;
        if( ifc.badInterface() ) {
            DL_ERROR(1, "Bad interface");
            return DPN_FAIL;
        }

        std::string hostCatalogV;
        std::string serverCatalogV;
        bool catalogVersionVerified = false;

        aFiles.clear();
        FOR_VALUE( aFilesMetaData.size(), i ) {

            __file_metadata &md = aFilesMetaData[i];
            DPN_FILESYSTEM::FileMap fm = md.iFileMap;
            int key = md.key;
            size_t size = md.size;
            std::string name = md.name;
            bool strictable = md.strictable;

            DPN_FILESYSTEM::Interface fileIf = ifc.request( DPN_SERVER,  key );
            if( fileIf.badInterface() ) {
                DL_ERROR(1, "Bad interface");
                return DPN_FAIL;
            }
            const DFile &file = pFileSystem->host().constFile( key );
            if( BADFILE(file) ) {
                DL_ERROR(1, "Bad key: [%d]", key);
                return DPN_FAIL;
            }
            if( !catalogVersionVerified && file.parent() ) { // catalog depended

                if( serverCatalogV.empty() ) serverCatalogV = pFileSystem->host().getHash();
                if( serverCatalogV.empty() ) {
                    DL_ERROR(1, "Can't get catalog hash");
                    return DPN_FAIL;
                }
                if( hostCatalogV.empty() ) hostCatalogV = UNIT_CATALOG_V.get();
                if( hostCatalogV.empty() ) {
                    DL_ERROR(1, "Empty host catalog hash");
                    return DPN_FAIL;
                }
                if( serverCatalogV != hostCatalogV ) {
                    DL_ERROR(1, "Different catalogs versiongs: server: [%s] host: [%s]",
                             serverCatalogV.c_str(), hostCatalogV.c_str());
                    return DPN_FAIL;
                }
                DL_INFO(1, "catalog verified!");
                catalogVersionVerified = true;
            }

            if( name != file.name() ) {
                DL_ERROR(1, "file: [%d] Sync error: wrong name: local: [%s] remote: [%s]", key, file.name().c_str(), name.c_str());
                return DPN_FAIL;
            }
            if( size != file.size() ) {
                DL_ERROR(1, "file: [%d] Sync error: wrong size: local: [%d] remote: [%d]", key, size, file.size());
                return DPN_FAIL;
            }
            size_t actual_size = get_file_size(file.path());
            if(actual_size == 0) {
                DL_ERROR(1, "No file data. key: [%d] path: [%s]", key, file.path_c());
                return DPN_FAIL;
            }

            DL_INFO(1, "File [%d] checked", key);
            std::cout << DPN_FILESYSTEM::filemap2text( fm ) << std::endl;

            DPN_FILESYSTEM::FileMap sendingMap;
            if( fm.size() ) {
                std::string hash;
                FOR_VALUE( fm.size(), i ) {
                    if( fm[i].iFlags & DPN_FILESYSTEM::Block::FB__ZEROBLOCK ) {
                        sendingMap.append( fm[i] );
                    } else {
                        fm[i].hashMe(file.path_c());
                        if( hash != fm[i].iHash ) {
                            sendingMap.append( fm[i] );
                        }
                    }
                }
            } else {
                DL_INFO(1, "Ready to send all file from first byte: file: [%d]", key);
            }
            std::cout << DPN_FILESYSTEM::filemap2text( sendingMap ) << std::endl;
            md.iFileMap = sendingMap;

            aFiles.append( fileIf );

        }

        */
        return DPN_SUCCESS;
    }
    DPN_Result RequestFile::hostPrepare(HOST_CALL) {

        INFO_LOG;
        return DPN_SUCCESS;
    }
    DPN_Result RequestFile::serverPrepare(SERVER_CALL) {

        INFO_LOG;
//        DPN_FileSystemPrivateInterface ifc = getInterface( tag() ) ;
//        if( ifc.badInterface() ) {
//            DL_ERROR(1, "Bad interface");
//            return DPN_FAIL;
//        }
//        FOR_VALUE( aFiles.size(), i ) {
//            DL_INFO(1, "send file: [%s]", aFiles[i].file().name_c() );
//            ifc.send( aFiles[i] );
//        }
        return DPN_SUCCESS;
    }
    DPN_Result RequestFile::hostStart(HOST_CALL) {

        INFO_LOG;

//        DPN_FileSystemPrivateInterface ifc = getInterface( tag() ) ;
//        if( ifc.badInterface() ) {
//            DL_ERROR(1, "Bad interface");
//            return DPN_FAIL;
//        }
//        DL_INFO( 1, " files: [%d]", aFiles.size() );
//        FOR_VALUE( aFiles.size(), i ) {
//            DL_INFO(1, "receive file: [%s]", aFiles[i].file().name_c() );
//            ifc.receive( aFiles[i] );
//        }
        return DPN_SUCCESS;
    }
    DPN_Result RequestFile::serverStart(SERVER_CALL) {
        return DPN_SUCCESS;
    }
    void RequestFile::fault(HOST_CALL) {
        DL_INFO(1, "RequestFile: host fault");
    }
    void RequestFile::fault(SERVER_CALL) {
        DL_INFO(1, "RequestFile: server fault");
    }
    void RequestFile::makeActionLine() {
        line<RequestFile>()
                << &RequestFile::hostRequest
                << &RequestFile::serverProcess
                << &RequestFile::hostPrepare
                << &RequestFile::serverPrepare
                << &RequestFile::hostStart
                << &RequestFile::serverStart
                   ;
    }
    //=================================================================================


}
