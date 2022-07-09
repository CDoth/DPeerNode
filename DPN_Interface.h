#ifndef DPN_INTERFACE_H
#define DPN_INTERFACE_H
#include "__dpeernode_global.h"
namespace DPN::Interface {


    // 1. key - unique data || key - shared data
    // 2. copy semantic
    // 3. single interface || multiply interfaces

    /*
     * =============================== Unique data:
     * Key mean solid copy-space
     *
     * key - 0, data - 0, if - A
     * key - 1, data - 1, if - A
     * key - 0, data - 0, if - B
     * key - 1, data - 1, if - B
     * =============================== Shared data:
     * Key mean separated copy-space
     *
     * key - 0, data - 0, if - A
     * key - 1, data - 0, if - A
     * key - 0, data - 0, if - B
     * key - 1, data - 0, if - B
     * ===============================
     *
     * Data, Descriptor, AbstractInterface, Interface
     *
     *
     *
    */


    // anon copy
    // anon destruct
    // default counter and global counter; destruct base only by global counter


    enum WatcherBehaivor {
        WATCHER_BHV__DESCRIPTOR,
        WATCHER_BHV__INTERFACE
    };
    class DataSpecificBaseWatcher : public DBaseWatcher {
    public:
        DataSpecificBaseWatcher() {
//            iInterfaceSpaces = 0;
            iTotalRefs = 0;
        }
        DataSpecificBaseWatcher( void *data ) : DBaseWatcher(data) {
//            iInterfaceSpaces = 0;
            iTotalRefs = 1;
        }
    public:
        void specificLeave( WatcherBehaivor b ) {
            if( iTotalRefs > 0 ) --iTotalRefs;
            if( b == WATCHER_BHV__DESCRIPTOR ) this->leave();
        }
        void specificAdd( WatcherBehaivor b ) {

//            using namespace DPN::Logs;
//            DL_INFO(1, "total ref: [%d]", iTotalRefs);

            ++iTotalRefs;
            if( b == WATCHER_BHV__DESCRIPTOR ) this->addRef();
        }
        inline int totalRefs() const { return iTotalRefs; }



//        int addSpace() { return ++iInterfaceSpaces; }
//        int removeSpace() { return iInterfaceSpaces > 0 ? --iInterfaceSpaces : 0; }
//        int spaces() const { return iInterfaceSpaces; }
        using DBaseWatcher::refs;
    private:
//        int iInterfaceSpaces;
        int iTotalRefs;
    };
    template <class DataType, WatcherBehaivor Behaivor>
    class InnerWatcher : public DWatcher<DataType, DataSpecificBaseWatcher> {
    public:

        using DWatcher<DataType, DataSpecificBaseWatcher>::DWatcher;

        InnerWatcher( bool create = false ) : DWatcher<DataType, DataSpecificBaseWatcher> (create) {}

        template <class ... Args>
        InnerWatcher( bool create, Args && ... a ) : DWatcher<DataType, DataSpecificBaseWatcher> (create, a...) {}

        InnerWatcher ( const InnerWatcher &w ) {
//            using namespace DPN::Logs;
//            DL_INFO(1, "Copy-Create inner watcher [%p] from: [%p]", this, &w);
            this->releaseBase();
            __copy( w );
        }
//        template <WatcherBehaivor sourceBehaivor>
//        InnerWatcher( const InnerWatcher<DataType, sourceBehaivor> &w ) {
//            using namespace DPN::Logs;
//            DL_INFO(1, "Copy-Create inner watcher [%p] from: [%p] (behaivor: [%d])", this, &w, sourceBehaivor);
//            this->releaseBase();
//            __copy( w );
//        }

        InnerWatcher & operator=( const InnerWatcher &w ) {
            __copy( w );
            return *this;
        }
        template <WatcherBehaivor SourceBehaivor>
        InnerWatcher & operator=( const InnerWatcher<DataType, SourceBehaivor> &w ) {
            __copy( w );
            return *this;
        }
        ~InnerWatcher() {
            __kick();
        }
    private:

        template <WatcherBehaivor SourceBehaivor>
        void __copy( const InnerWatcher<DataType, SourceBehaivor> &w ) {

            if( this->getConstBase() ) {
//                if( Behaivor == WATCHER_BHV__INTERFACE ) return;
                __kick();
            }
            this->spreadToMe( w );

            if( this->getConstBase() ) this->getBase()->specificAdd( Behaivor );


        }
        void __kick() {

            using namespace DPN::Logs;

//            DL_INFO(1, "Kick for: [%p] base: [%p] refs: [%d] total refs: [%d]",
//                    this, this->getBase(), this->getBase()->refs(), this->getBase()->totalRefs());

            if( this->getBase() ) {
                this->getBase()->specificLeave( Behaivor );

                if( this->getBase()->refs() == 0 ) {
                    if( this->data() ) {
                        delete this->data();
                    }
                    this->getBase()->clear();
                }


                if( this->getBase()->totalRefs() == 0 ) {
                    this->releaseBase();
                }
                this->throwBase();
            }

        }
    };

    //===========================================================================================
    template <class DataType>
    class DataReference
            : private InnerWatcher<DataType, WATCHER_BHV__DESCRIPTOR >
    {
    public:
        typedef InnerWatcher<DataType, WATCHER_BHV__DESCRIPTOR > base_t;

        inline operator base_t&() { return *this; }

        base_t & innerWatcher() { return *this; }

        template <class T> friend class InterfaceReference;
        using InnerWatcher<DataType, WATCHER_BHV__DESCRIPTOR >::InnerWatcher;
        using InnerWatcher<DataType, WATCHER_BHV__DESCRIPTOR >::isEmptyObject;
        using InnerWatcher<DataType, WATCHER_BHV__DESCRIPTOR >::data;

        int totalRefs() const {
            const DataSpecificBaseWatcher *base = this->getConstBase();
            return base ? base->totalRefs() : -1;
        }


    };
    template <class DataType>
    class InterfaceReference
            : private DWatcher< InnerWatcher< DataType, WATCHER_BHV__INTERFACE > >
    {
    public:
        typedef InnerWatcher< DataType, WATCHER_BHV__INTERFACE > base_t;

        using DWatcher< InnerWatcher< DataType, WATCHER_BHV__INTERFACE > >::isEmptyObject;
        using DWatcher< InnerWatcher< DataType, WATCHER_BHV__INTERFACE > >::isUnique;

        base_t & innerWatcher() { return *this->data(); }


        void copy( InterfaceReference &ir ) {
            DWatcher< InnerWatcher< DataType, WATCHER_BHV__INTERFACE > >::copy(ir);
        }

        int totalRefs() const {
            const DataSpecificBaseWatcher *base = specBase();
            return base ? base->totalRefs() : -1;
        }
        InterfaceReference() {
            using namespace DPN::Logs;
//            DL_INFO(1, "Create empty InterfaceReference : [%p]", this);
        }
        InterfaceReference( DataReference<DataType> &w ) {

//            using namespace DPN::Logs;
//            DL_INFO(1, " >>> 0: Create InterfaceReference [%p] (data: [%p]) with DataReference: [%p] (data: [%p])",
//                    this, this->data(),
//                    &w, w.data());
            this->createInner();
            base_t *d = this->data();
            *d = w;
        }
        void closeInterface() {
            this->clearObject();
        }



        DataType * inner() {

//            using namespace DPN::Logs;
//            DL_INFO(1, " Try access InterfaceReference::inner: object: [%p] data: [%p] empty: [%d] ",
//                    this, this->data(), this->isEmptyObject() );

            if( this->isEmptyObject() ) {
                return nullptr;
            }
            base_t *d = this->data();
            return d->data();
        }
        const DataType * inner() const {

//            using namespace DPN::Logs;
//            DL_INFO(1, " Try access InterfaceReference::inner: object: [%p] data: [%p] empty: [%d] ",
//                    this, this->data(), this->isEmptyObject() );

            if( this->isEmptyObject() ) {
                return nullptr;
            }
            const base_t *d = this->data();
            return d->data();
        }
        bool badInterface() const {
            if( this->isEmptyObject() ) return true;
            const base_t *d = this->data();
            if( d->isEmptyObject() ) return true;
            if( d->data() == nullptr ) return true;
            return false;
        }
//    private:
        DataSpecificBaseWatcher *specBase() {
            return this->data() ? this->data()->getBase() : nullptr;
        }
        const DataSpecificBaseWatcher *specBase() const {
            return this->data() ? this->data()->getConstBase() : nullptr;
        }

    };
    //===========================================================================================
    template <class DataType>
    class InterfaceCenter {
    public:
        InterfaceCenter() {
//            using namespace DPN::Logs;
//            DL_INFO(1, "Create InterfaceCenter: [%p]", this);
        }
        InterfaceReference<DataType> takeInterface( DataReference<DataType> &dataRef ) {

//            using namespace DPN::Logs;
            DPN_THREAD_GUARD( iMutex );
//            DL_INFO(1, " >>> wReference empty: [%d]", wReference.isEmptyObject() );

            if( wReference.isEmptyObject() ) {
                wReference = InterfaceReference<DataType>(dataRef);
//                DL_INFO(1, " >>> create interface reference [%p] and return", &wReference );
                return wReference;
            }
            if( wReference.isUnique() ) {
//                DL_INFO(1, " >>> creturn existed reference [%p]", &wReference );
//                *wReference.data() = dataRef;
                wReference.innerWatcher() = dataRef.innerWatcher();
                return wReference;
            }
//            DL_INFO(1, " >>> return bad reference", &wReference );
            return InterfaceReference<DataType>();
        }
    private:
        std::mutex iMutex;
        InterfaceReference<DataType> wReference;
    };
    template <class KeyType, class DataType>
    class InterfaceMapCenter {
    public:
        InterfaceMapCenter() {
//            using namespace DPN::Logs;
//            DL_INFO(1, "Create InterfaceMapCenter: [%p]", this);
        }
        InterfaceReference<DataType> takeMappedInterface( const KeyType &key, DataReference<DataType> &dataRef ) {


//            using namespace DPN::Logs;
            DPN_THREAD_GUARD( iMutex );
//            DL_INFO(1, " >>> InterfaceMapCenterReference: [%p] map size: [%d]",
//                    this, iReferenceMap.size());

            if( iReferenceMap.find( key ) == iReferenceMap.end() ) {

//                DL_INFO(1, " >>> 0: No interface in map... key: [%d]", key );

                InterfaceReference<DataType> ir(dataRef);
                iReferenceMap[key] = ir;

//                DL_INFO(1, " >>> 1: Create interface [%p] (data: [%p]) for key: [%d]", &ir, ir.data(), key );
//                return ir;
            }
            InterfaceReference<DataType> &ir = iReferenceMap[key];
            if( ir.isEmptyObject() ) {
                ir = InterfaceReference<DataType>(dataRef);
//                DL_INFO(1, " >>>  Return interface [%p] data: [%p]", &ir, ir.data() );
                return ir;
            }
            if( ir.isUnique() ) {
//                DL_INFO(1, " >>>  Return unique interface" );
                return ir;
            }
//            DL_INFO(1, " >>>  Return bad interface" );
            return InterfaceReference<DataType>();
        }
    private:
        std::mutex iMutex;
        std::map< KeyType, InterfaceReference<DataType> > iReferenceMap;
    };
    //===========================================================================================
    template <class DataType>
    class InterfaceCenterReference : private DWatcher< InterfaceCenter<DataType> > {
    public:
        InterfaceCenterReference() : DWatcher<InterfaceCenter<DataType>> (true) {
//            using namespace DPN::Logs;
//            DL_INFO(1, "Create InterfaceCenterReference: [%p] data: [%p]", this, this->data());
        }

        InterfaceReference<DataType> getInterface( DataReference<DataType> &dataRef ) {
            if( this->isEmptyObject() ) {
                return InterfaceReference<DataType>();
            }
            InterfaceCenter<DataType> *c = this->data();
            InterfaceReference<DataType> ir = c->takeInterface( dataRef );
            return ir;
        }
    };
    template <class KeyType, class DataType>
    class InterfaceMapCenterReference : private DWatcher< InterfaceMapCenter<KeyType, DataType> > {
    public:
        InterfaceMapCenterReference() : DWatcher<InterfaceMapCenter<KeyType, DataType>> (true) {
//            using namespace DPN::Logs;
//            DL_INFO(1, "Create InterfaceMapCenterReference: [%p] data: [%p]", this, this->data());
        }

        InterfaceReference<DataType> getInterface( const KeyType &key, DataReference<DataType> &dataRef ) {

//            using namespace DPN::Logs;
//            DL_INFO(1, "InterfaceMapCenterReference: [%p] data: [%p]", this, this->data());

            if( this->isEmptyObject() ) {
                return InterfaceReference<DataType>();
            }
            InterfaceMapCenter<KeyType, DataType> *c = this->data();
            InterfaceReference<DataType> ir = c->takeMappedInterface( key, dataRef );
            return ir;
        }
    };
    //===========================================================================================
}


#endif // DPN_INTERFACE_H
