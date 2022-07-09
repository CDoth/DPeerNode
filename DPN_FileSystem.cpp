#include "DPN_FileSystem.h"
#include "DPN_FileSystemProcessors.h"


using namespace DPN::Logs;
DPN_Catalog __global__host_catalog = DPN_Catalog("Host Catalog");

bool DPN_FileSystem::useChannel(DPN::Client::Tag tag, DPN::Direction direction, __channel_mono_interface mono, const DPN_ExpandableBuffer &context) {

    DL_INFO(1, "tag: [%p] direction: [%d]", tag, direction);

//    if( iClientsData.find( tag ) == iClientsData.end() ) {
//        auto client = tag2client( tag );
//        if( client.badClient() ) {
//            DL_ERROR(1, "No client with tag [%p]", tag);
//            return false;
//        }
//        iClientsData[ tag ] = DPN_FileSystemDescriptor( client.core() );
//    }

//    DPN_FileSystemPrivateInterface i;
//    DPN_FileSystemDescriptor &desc = iClientsData[ tag ];
//    auto w = im.get( tag, desc );
//    i.move( w );

//    return i.addChannel( mono, direction, context );
}
void DPN_FileSystem::clientOver(DPN::Client::Tag tag) {
    DL_INFO( 1, "File System client over: [%p]", tag);

//    DPN_FileSystemPrivateInterface i = getPrivateIf( tag );

//    im.close( tag );
//    imPrivite.close( tag );
//    iClientsData.erase( tag );
}
DPN_FileSystem::DPN_FileSystem(const std::string &name, ClientCenter &cc)
    : DPN::Network::NetworkModule(name, cc)
{

    __global__host_catalog.createLinearFileSystem();

    DL_INFO(1, "Create file module: [%p]", this);
}
DPN_Catalog &DPN_FileSystem::host() {
    return __global__host_catalog;
}

bool DPN_FileSystem::compareCatalogHash(const std::string &hash) const {
    std::string host_hash = __global__host_catalog.getHash();
    bool status = bool(host_hash == hash);
    return status;
}

DPN_FileSystemInterface DPN_FileSystem::getIf( DPN::Client::Tag tag ) {
    DPN_FileSystemInterface i;

    if( iClientsData.find( tag ) == iClientsData.end() ) {
        auto client = tag2client( tag );
        if( client.badClient() ) {
            DL_ERROR(1, "No client with tag [%p]", tag);
            return i;
        }
        iClientsData[ tag ] = DPN_FileSystemDescriptor( client.core() );
    }
    DPN_FileSystemDescriptor &d = iClientsData[ tag ];
    auto pi = d.getPublicInterface();
    i.copy( pi );
    return i;
}
DPN_FileSystemPrivateInterface DPN_FileSystem::getPrivateIf( DPN::Client::Tag tag ) {

    DPN_FileSystemPrivateInterface i;

    if( iClientsData.find( tag ) == iClientsData.end() ) {
        auto client = tag2client( tag );
        if( client.badClient() ) {
            DL_ERROR(1, "No client with tag [%p]", tag);
            return i;
        }
        iClientsData[ tag ] = DPN_FileSystemDescriptor( client.core() );
    }

    DPN_FileSystemDescriptor &d = iClientsData[ tag ];
    auto w = d.getPrivateInterface();
    i.copy( w );
    return i;
}

bool DPN_FileSystemInterface::sync() {
    if( badInterface() ) {
        DL_ERROR(1, "Bad interface");
        return false;
    }

    auto proc = DPN_PROCS::processor<PT__FS__SYNC_CATALOGS>();
    DL_INFO(1, "Create sync processor [%p]", proc);
    return inner()->fs__send( proc );
}
const DPN_Catalog *DPN_FileSystemInterface::remote() const {
    if( badInterface() ) {
        DL_ERROR(1, "Bad interface");
        return nullptr;
    }
    return &inner()->iRemoteCatalog;
}
bool DPN_FileSystemInterface::requestFileset(const DArray<int> &keyset) {

    if( badInterface() ) {
        DL_ERROR(1, "Bad interface");
        return false;
    }

    auto proc = DPN_PROCS::processor<PT__FS__REQUEST_FILE>();
    proc->setFileset( keyset );
    return inner()->fs__send( proc );
}

DPN_Catalog *DPN_FileSystemPrivateInterface::remote() {
    if( badInterface() ) {
        DL_ERROR(1, "Bad interface");
        return nullptr;
    }
    return &inner()->iRemoteCatalog;
}
DFile DPN_FileSystemPrivateInterface::remoteFile(int key) const {
    if( badInterface() ) {
        DL_ERROR(1, "Bad interface");
        return DFile();
    }
    return inner()->iRemoteCatalog.constFile( key );
}
void DPN_FileSystemPrivateInterface::unverifyRemote2Host() {
    if( badInterface() ) return;
    inner()->iStateRemote2Host = false;
}
void DPN_FileSystemPrivateInterface::unverifyHost2Remote() {
    if( badInterface() ) return;
    inner()->iStateHost2Remote = false;
}
bool DPN_FileSystemPrivateInterface::verifyHost2Remote(bool status) {
    if( badInterface() ) {
        DL_ERROR(1, "Bad interface");
        return false;
    }

    return inner()->iStateHost2Remote = status;
}
bool DPN_FileSystemPrivateInterface::verifyRemote2Host(bool status) {
    inner()->iStateRemote2Host = status;
    return status;
}
DPN_FILESYSTEM::Interface DPN_FileSystemPrivateInterface::request( CallerPosition p, int key ) {


    DPN_FILESYSTEM::Interface ifc;

    if( badInterface() ) {
        DL_ERROR(1, "Bad system interface");
        return ifc;
    }

    if( p == DPN_HOST ) {


        DFile file = inner()->iRemoteCatalog.file( key );
        if( BADFILE(file) ) {
            DL_ERROR(1, "No remote file with key [%d]", key );
            return ifc;
        }
        DPN_FILESYSTEM::Descriptor fd;
        if( inner()->iHostRequestedFiles.find( key ) == inner()->iHostRequestedFiles.end() ) {
            fd = DPN_FILESYSTEM::Descriptor(file);
            inner()->iHostRequestedFiles[key] = fd;
        } else {
            fd = inner()->iHostRequestedFiles[key];
        }
//        auto w = inner()->iHostFilesInterfaces.get( key, fd );
//        if( w.badInterface() ) {
//            DL_ERROR(1, "Bad file host interface");
//            return ifc;
//        }

        if( inner()->iDownloadPath.empty() ) {
            inner()->iDownloadPath = "F:/DFS_SPACE/DOWNLOAD/";
        }

        file.createAbsolutePath( inner()->iDownloadPath );

//        ifc.move( w );
        return ifc;


    } else {
        DFile file = __global__host_catalog.file( key );
        if( BADFILE(file) ) {
            DL_ERROR(1, "No host file with key [%d]", key );
            return ifc;
        }

        DPN_FILESYSTEM::Descriptor fd;
        if( inner()->iServerRequestedFiles.find( key ) == inner()->iServerRequestedFiles.end() ) {
            fd = DPN_FILESYSTEM::Descriptor(file);
            inner()->iServerRequestedFiles[key] = fd;
        } else {
            fd = inner()->iServerRequestedFiles[key];
        }
//        auto w = inner()->iServerFilesInterfaces.get( key, fd );
//        if( w.badInterface() ) {
//            DL_ERROR(1, "Bad file server interface");
//            return ifc;
//        }

//        ifc.move( w );
        return ifc;
    }

    return ifc;
}
bool DPN_FileSystemPrivateInterface::send(DPN_FILESYSTEM::Interface fileIf) {

    if( badInterface() ) {
        DL_ERROR(1, "Bad interface");
        return false;
    }
    if( inner()->aOutgoingChannels.empty() ) {
        DL_ERROR(1, "No channels");
        return false;
    }
//    if( inner()->iServerRequestedFiles.find( fileIf.key() ) == inner()->iServerRequestedFiles.end() ) {
//        DL_ERROR(1, "No interface in server request");
//        return false;
//    }

    DPN_FILESYSTEM::FileMap m = fileIf.fileMap();

    DL_INFO(1, "Try send file...");
    std::cout << DPN_FILESYSTEM::filemap2text( m ) << std::endl;

    FOR_VALUE( m.size(), mi ) {
        DPN_FILESYSTEM::Channel *pickedChannel = &inner()->aOutgoingChannels[0];
        int minLoad = inner()->aOutgoingChannels[0].load();

        FOR_VALUE( inner()->aOutgoingChannels.size(), i ) {

            if(  inner()->aOutgoingChannels[i].load() < minLoad ) {
                pickedChannel = &inner()->aOutgoingChannels[i];
                minLoad = inner()->aOutgoingChannels[i].load();
            }

        }
        DL_INFO(1, "picked channel: [%p] file: [%s] block: [%d-%d]",
                pickedChannel, fileIf.file().name_c(), m[mi].begin(), m[mi].end() );

        if( pickedChannel ) {

            pickedChannel->send( DPN_FILESYSTEM::Slice( fileIf.file(), m[mi] ) );
            if( pickedChannel->threadUnit().processing() == false ) {
                inner()->fs__activateChannel( *pickedChannel );
            }
        }
    }
    return true;
}
bool DPN_FileSystemPrivateInterface::receive(DPN_FILESYSTEM::Interface fileIf) {

    if( badInterface() ) {
        DL_ERROR(1, "Bad interface");
        return false;
    }
    if( inner()->aIncomingChannels.empty() ) {
        DL_ERROR(1, "No incoming channels");
        return false;
    }
//    if( inner()->iHostRequestedFiles.find( fileIf.key() ) == inner()->iHostRequestedFiles.end() ) {
//        DL_ERROR(1, "No interface in host request");
//        return false;
//    }

    DL_INFO(1, "Put file [%d] to receive session... ", fileIf.file().key() );
    if( inner()->putReceivingFile( fileIf.file() ) == false ) {
        DL_FUNCFAIL(1, "putReceivingFile");
        return false;
    }

    return true;
}
bool DPN_FileSystemPrivateInterface::addChannel(__channel_mono_interface mono, DPN::Direction direction, const DPN_ExpandableBuffer &context) {

    if( badInterface() ) {
        DL_ERROR(1, "Bad interface");
        return false;
    }
    if( mono.badInterface() ) {
        DL_ERROR(1, "Bad mono channel interface");
        return false;
    }

    DPN_FILESYSTEM::Channel channel( *inner(), mono, context);
    if( direction == DPN::FORWARD ) {
        inner()->aOutgoingChannels.append( channel );
    } else {
        inner()->aIncomingChannels.append( channel );
        inner()->fs__activateChannel( channel );
    }


    return true;
}

bool DPN_FileSystemData::fs__send( DPN_TransmitProcessor *p ) {
    return DPN::Client::Core::send( p );
}
bool DPN_FileSystemData::fs__activateChannel(DPN_FILESYSTEM::Channel ch) {

    DPN::Client::Core::putUnit( ch.threadUnit() );
    return true;
}
void DPN_FileSystemData::fs__stop() {


    DPN_FILESYSTEM::SystemKernel::kernelStop();
}
DPN::Interface::InterfaceReference<DPN_FileSystemData> DPN_FileSystemDescriptor::getPublicInterface() {
    return wPublicInterface.getInterface( *this );
}
DPN::Interface::InterfaceReference<DPN_FileSystemData> DPN_FileSystemDescriptor::getPrivateInterface() {
    return wPrivateInterface.getInterface( *this );
}

DPN_FileSystem *extractFileModule(DPN::Modules modules) { return reinterpret_cast<DPN_FileSystem*>(modules.module("FileSystem")); }

