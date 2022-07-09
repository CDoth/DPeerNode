#ifndef DPN_CATALOG_H
#define DPN_CATALOG_H
#include <DDirReader.h>
#include "__dpeernode_global.h"




class DPN_Catalog  {
public:
    DPN_Catalog();
    DPN_Catalog(const std::string &name);
    std::string sync(const std::string &topology);

    inline bool verify(const std::string &hash) {

//        isActual = compareHash(hash);
//        return isActual;

        return false;
    }
public:

    void renew();
    bool addRegularDirectory(const char *path);
    bool addVirtualDirectory(const char *name);

    bool addRegularFile(const char *path);
    bool addVirtualFile(const char *name, size_t size = 0);

    DAbstractFileSystem * createLinearFileSystem();
    DAbstractFileSystem * createMapFileSystem();

    DAbstractFileSystem * getFileSystem() {return fileSystem;}
public:
    void clear() {inner.clear();}
    std::string parseToText(bool includeFiles) const {return inner.parseToText(includeFiles);}
    std::string topology(bool includeFiles) const {return inner.topology(includeFiles);}
    int files() const {return inner.getFilesNumber();}
    int dirs() const {return inner.getInnerDirsNumber();}
    int size() const {return files() + dirs();}
    bool empty() const {return inner.empty();}
    const std::string & name() const {return inner.getStdName();}
    const std::string & path() const {return inner.getStdPath();}
    const std::string & vpath() const {return inner.getStdVPath();}

    inline const DFile & file(const std::string &name) const {return inner.getConstFile(name);}
    inline const DFile & constFile(DFileKey key) const {return fileSystem->fileConstRef(key);}

    const DDirectory * directory(const std::string &name) const {return inner.getDirectory(name);}

    inline DFile file(const std::string &name) {return inner.getFile(name);}
    inline DFile file(DFileKey key) {return fileSystem->file(key);}


    bool isValidKey(DFileKey key) {return fileSystem->isKeyRegister(key);}


    DDirectory * directory(int index) {return inner.getDirectory(index);}
    DDirectory * directory(const std::string &name) {return inner.getDirectory(name);}

    const DDirectory & getInner() const {return inner;}
    std::string getHash( ) const;

private:
    mutable DPN::SHA256 iHashtool;
    DAbstractFileSystem *fileSystem;
    DDirectory inner;
};


#endif // DPN_CATALOG_H
