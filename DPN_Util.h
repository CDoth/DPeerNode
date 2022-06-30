#ifndef DPN_UTIL_H
#define DPN_UTIL_H


#include "__dpeernode_global.h"
namespace DPN::Util {

    template <class Object>
    class SimplePool {
    public:
        inline Object * get() {
            if( aObjects.empty() ) return new Object;

            auto o = aObjects.back();
            aObjects.pop_back();
            return o;
        }
        inline void set(Object *o) {
            aObjects.push_back(o);
        }
        inline bool empty() const {return aObjects.empty();}
    private:
        DArray<Object*> aObjects;
    };

    template <class T>
    struct ListNode {
        ListNode( T *object = nullptr ) :
            pObject(object), pPrev(nullptr), pNext(nullptr)
        {}
        inline void set( T *o) { pObject = o; }
        inline void exclude() {
            if( pPrev ) pPrev->pNext = pNext;
            if( pNext ) pNext->pPrev = pPrev;

            pPrev = nullptr;
            pNext = nullptr;
            pObject = nullptr;
        }
        inline void connect( ListNode *n ) {
            pNext = n;
            n->pPrev = this;
        }
        inline ListNode * prev() { return pPrev; }
        inline ListNode * next() { return pNext; }
        inline T * object() { return pObject; }
        inline const T * object() const { return pObject; }

    private:
        T *pObject;
        ListNode *pPrev;
        ListNode *pNext;
    };

    template <class T>
    class ThreadSafeList {
    public:
        ThreadSafeList() {
            pFirst = nullptr;
            pHead = nullptr;
            iSize = 0;
        }
    public:
        void push_back( T *o ) {

            DPN_THREAD_GUARD( iMutex );

            ListNode<T> *node = getNode(o);
            if( pHead ) pHead->connect( node );
            pHead = node;
            //---------------------------
            ++iSize;
            if( pFirst == nullptr )  pFirst = node;
            //---------------------------
        }
        void pop_back() {
            DPN_THREAD_GUARD( iMutex );
            innerPopBack();
        }
        T * last() {
            if( pHead == nullptr ) return nullptr;

            DPN_THREAD_GUARD( iMutex );

            return pHead->object();
        }
        const T * last() const {
            if( pHead == nullptr ) return nullptr;

            DPN_THREAD_GUARD( iMutex );

            return pHead->object();
        }
        T * takeLast() {
            if( pHead == nullptr ) return nullptr;

            DPN_THREAD_GUARD( iMutex );
            T *o = pHead->object();
            innerPopBack();
            return o;
        }
        void clear() {
            DPN_THREAD_GUARD( iMutex );

            ListNode<T> *b = pFirst.load();
            ListNode<T> *e = pHead;
            while(b != e) {
                removeNode(b);
                b = b->next();
            }
            zero();
        }
        void moveTo( ThreadSafeList &list ) {

            DPN_THREAD_GUARD2( iMutex );
            DPN_THREAD_GUARD( list.iMutex );

            if( list.pFirst == nullptr ) {
                list.pFirst = pFirst;
                list.pHead = pHead;
            } else {
                list.pHead->connect( pFirst );
                list.pHead = pHead;
            }


            zero();
        }
        void moveTo( DArray<T*> &array ) {

            DPN_THREAD_GUARD( iMutex );
//            using namespace DPN::Logs;

            ListNode<T> *b = pFirst.load();
            ListNode<T> *e = pHead->next();
            while(b != e) {
//                DL_INFO(1, "[%p]: move [%p] object to [%p] array", this, b->object(), &array);
                array.append( b->object() );
                removeNode(b);
                b = b->next();
            }
            zero();
        }
        inline int size() const { return iSize; }
        inline bool empty() const { return pFirst == nullptr; }
    private:
        inline void innerPopBack() {
            if( pHead ) {
                ListNode<T> *h = pHead;
                pHead = h->prev();
                removeNode( h );
                --iSize;
            }
        }
        void zero() {
            pFirst = nullptr;
            pHead = nullptr;
            iSize = 0;
        }
        inline ListNode<T> * getNode( T *o ) {

            ListNode<T> *n = iNodePool.get();
            n->set( o );
            return n;

            return nullptr;
        }
        inline void removeNode( ListNode<T> *n ) {
            n->exclude();
            iNodePool.set( n );
        }
    private:
        int iSize;
        std::mutex iMutex;
        std::atomic<ListNode<T>*> pFirst;
        ListNode<T> *pHead;
        SimplePool<ListNode<T>> iNodePool;
    };

    template <class T>
    class ThreadSafeQueue {
    public:
        inline void push_back( T *o ) { iInput.push_back( o ); }
    public:
        void accept() {
//            using namespace DPN::Logs;

//            DL_INFO(1, "input empty: [%d] input: [%p] output: [%p]", iInput.empty(), &iInput, &iOutput);
            if( !iInput.empty() ) {
                iInput.moveTo( iOutput );
            }
        }
        inline int inputSize() const { return iInput.size(); }
        inline int size() const { return iOutput.size(); }

        inline void remove( int i ) {
            iOutput.removeByIndex( i );
        }
        const DArray<T*> & output() const { return iOutput; }
    private:
        ThreadSafeList<T> iInput;
        DArray<T*> iOutput;
    };

    template <class T>
    class Action {
    public:
        Action<T> *next;
        typedef DPN_Result (T::*ActionCallback)();
        Action(ActionCallback __a) : a(__a), next(nullptr), iLevel(0) {}
        void setLevel(int l) {iLevel = l;}
        inline int level() const {return iLevel;}
        DPN_Result action(T *pObj) {
            using namespace DPN::Logs;
            if(pObj == nullptr || a == nullptr ) {
                DL_BADPOINTER(1, "object [%p] or action [%p]", pObj, a);
                return DPN_FAIL;
            }
            return DPN_CALL_MEMBER_FN(pObj, a)();
        }
    private:
        ActionCallback a;
        int iLevel;
    };
    class BaseActionLine {
    public:
        inline void setTarget(void *target) { pObject = target; }
        inline DPN_Result lastResult() const {return iLastResult;}
        virtual DPN_Result doStep() = 0;
        virtual DPN_Result doStep( int level ) = 0;
        virtual bool isOver() const = 0;
    protected:
        void *pObject;
        DPN_Result iLastResult;
    };
    template <class T>
    class ActionLine : public BaseActionLine {
    public:
        typedef DPN_Result (T::*ActionCallback)();
        ActionLine(T *targetObject = nullptr)  {
            start = nullptr;
            head = nullptr;
            current = nullptr;
            iStep = 0;
            pObject = targetObject;
        }
        void setLine(ActionLine<T> l) {
            start = l.start;
            head  = l.head;
            current = start;
        }
        ActionLine & operator<< (ActionCallback a) {
            return add(new Action<T>(a));
        }
        ActionLine & operator<< (int level ) {
            if( head ) {
                head->setLevel( level );
            }
            return *this;
        }

        DPN_Result go() {

            using namespace DPN::Logs;
            if( current == nullptr ) {
                DL_BADPOINTER(1, "current");
                return DPN_FAIL;
            }

            DPN_Result r = DPN_FAIL;
            while(current) {

                r = current->action(reinterpret_cast<T*>( pObject ));
                if(r == DPN_SUCCESS) {
                    current = current->next;
                    ++iStep;
                } else {
                    return r;
                }
            }
            return DPN_SUCCESS;

        }
        DPN_Result doStep() override {

            using namespace DPN::Logs;
            if( current == nullptr ) {
                DL_BADPOINTER(1, "current");
                return DPN_FAIL;
            }

            DL_INFO(1, "action line: [%p] current: [%p]", this, current);
            iLastResult = current->action(reinterpret_cast<T*>( pObject ));

            if( iLastResult == DPN_SUCCESS ) {
                ++iStep;
                if( (current = current->next) == nullptr ) return DPN_SUCCESS;
            }
            return iLastResult;
        }
        DPN_Result doStep( int level ) override {

            using namespace DPN::Logs;

            while( current && current->level() != level ) current = current->next;

            if( current == nullptr ) {
                DL_BADPOINTER(1, "current");
                return DPN_FAIL;
            }

            DL_INFO(1, "action line: [%p] current: [%p]", this, current);
            iLastResult = current->action(reinterpret_cast<T*>( pObject ));

            if( iLastResult == DPN_SUCCESS ) {
                ++iStep;
                if( (current = current->next) == nullptr ) return DPN_SUCCESS;
            }
            return iLastResult;
        }
        void restart() {
            current = start;
            iStep = 0;
        }
        int step() const {return iStep;}

        inline bool isOver() const override {return current == nullptr;}
        bool empty() const {return start == nullptr;}
    private:
        ActionLine & add(Action<T> *u) {
            if( start == nullptr ) start = u;
            if( current == nullptr ) current = start;
            if( head ) head->next = u;
            head = u;
            return *this;
        }
    private:
        Action<T> *start;
        Action<T> *head;
        Action<T> *current;
        int iStep;
    };
}
#endif // DPN_UTIL_H
