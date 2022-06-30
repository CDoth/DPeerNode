#include "DPN_Catalog.h"
#include "__dpeernode_global.h"


using namespace DPN::Logs;

DPN_Catalog::DPN_Catalog() {

    inner.setName("Catalog");
    fileSystem = nullptr;

}
DPN_Catalog::DPN_Catalog(const std::string &name) {

    inner.setName(name);
    fileSystem = nullptr;

}
std::string DPN_Catalog::sync(const std::string &topology) {

    if( fileSystem == nullptr ) {
        DL_BADPOINTER(1, "file system");
        return std::string();
    }

    fileSystem->clear();

    inner.clear();

    inner.create(topology);

    return inner.topology(true);

}
void DPN_Catalog::renew() {
    inner.renew();
}
bool DPN_Catalog::addRegularDirectory(const char *path) {

    return bool(inner.addDirectory(path));
}
bool DPN_Catalog::addVirtualDirectory(const char *name) {

    return bool(inner.addVirtualDirectory(name));
}
bool DPN_Catalog::addRegularFile(const char *path) {

    return inner.addFile(path);
}
bool DPN_Catalog::addVirtualFile(const char *name, size_t size) {

    return bool(inner.addVirtualFile(name, size));
}
DAbstractFileSystem * DPN_Catalog::createLinearFileSystem() {
    if(fileSystem == nullptr) {
        fileSystem = new DLinearFileSystem;
        inner.setFileSystem(fileSystem);
    }
    return fileSystem;
}
DAbstractFileSystem * DPN_Catalog::createMapFileSystem() {
    if(fileSystem == nullptr) {
        fileSystem = new DMapFileSystem;
        inner.setFileSystem(fileSystem);
    }
    return fileSystem;
}
std::string DPN_Catalog::getHash( ) const {

    iHashtool.hash_string(inner.topology(true));
    return iHashtool.get();
}

