#ifndef DPN_THREADBRIDGE_H
#define DPN_THREADBRIDGE_H
#include <DWatcher.h>
#include <thread>


#include "DPN_Direction.h"


class DPN_ThreadBridge;
class DPN_ThreadMaster;

struct DPN_ThreadBridgeData {
public:

    friend class DPN_ThreadBridge;

    DPN_ThreadBridgeData();

    void accept();
    void setNoAcceptMode();
    void setReplaceMode();
    bool planDirection(DPN_Direction *d);
    void clearPlan();

    void forceTask(DPN_Task *t);
    bool pushTask(DPN_Task *t);
    void procTasks();
    void procDirections();
    void procSingle();
private:
    enum DirectionAcceptMode {
        NO_ACCEPT,
        REPLACE,
        __KEEP_REPLACE
    };
private:

    bool __hold_direction(DPN_Direction *d);

    // 1. Add client
    // > add directions to sorted by directions thread list
    //   (choose thread with minimum directions)

    // 2. Add thread
    // > Replan all directions

    // 3. Remove client
    // > Replan all directions

    // 4. Remove thread
    // > Replan all directions


    // original
    DPN_ThreadContext threadContext;
//    DPN_Context context;


    std::atomic_bool play;
    std::atomic_bool pause;
    std::atomic<DirectionAcceptMode> acceptMode;
    std::atomic_bool process_tasks;
    std::mutex mutex;
    uint64_t threadId;
    std::thread::id stdThreadId;
    //-------------------------------
    DArray<DPN_Direction*> planed;
    DArray<DPN_Direction*> accepted;
    DPN_Direction *singleDirection;
    //------------------------------

    std::mutex taskMutex;
    DPN_SimplePtrList<DPN_Task> tasks;
};
class DPN_ThreadBridge : public DWatcher<DPN_ThreadBridgeData> {
public:
    DPN_ThreadBridge();
    friend class DPN_ThreadMaster;
public:
    void clearPlan();
    bool planDirection(DPN_Direction *d);
    void replacePlaned();


    inline void procSingle() {data()->procSingle();}
    inline void procDirections() {data()->procDirections();}
    inline void procTasks() {data()->procTasks();}
    inline bool pushTask(DPN_Task *t) {return data()->pushTask(t);}
    inline void forceTask(DPN_Task *t) {data()->forceTask(t);}

    inline bool playable() const {return data()->play;}
    inline bool inPause() const {return data()->pause;}

    int directionsAccepted() const;
    uint64_t threadId() const;

    void setProcessTasks(bool state) {data()->process_tasks = state;}


    inline bool process_tasks() const {return data()->process_tasks;}
//    inline DArray<DPN_Direction*> & disconnected() {return data()->disconnected;}
private:
    void start();

};

#endif // DPN_THREADBRIDGE_H
