#include "__dpeernode_global.h"
#include <DProfiler.h>

DLogs::DLogsContext DPeerNodeSpace::log_context;


DLOGS_INIT_GLOBAL_CONTEXT("DPeerNode", DPeerNodeSpace::logsInit);


using namespace DPeerNodeSpace;

int pointerValue(const void *ptr) {
    return ptr ? *reinterpret_cast<const int*>(ptr) : 0;
}
void dpn_time_range::fill(time_t __seconds) {
    useconds = 0;
    seconds = __seconds;
    minutes = seconds / 60; seconds %= 60;
    hours = minutes / 60; minutes %= 60;
    days = hours / 24; hours %= 24;
}
void dpn_time_range::fill2(int __useconds) {
    useconds = __useconds;
    seconds = useconds / 1000000; useconds %= 1000000;
    minutes = seconds / 60; seconds %= 60;
    hours = minutes / 60; minutes %= 60;
    days = hours / 24; hours %= 24;
}
//----------------------------------------------------------------------------------------------------- DPN_TimeMoment:
DPN_TimeMoment::DPN_TimeMoment(bool fix) {
    if(fix) fixTime();
    else {
        __localTime = 0;
        __runTime = 0;
        timerclear(&__lt_);
    }
}
void DPN_TimeMoment::fixTime() {
    gettimeofday(&__lt_, nullptr);

//    DL_INFO(1, "lt: sec: [%d] usec: [%d] total usec: [%d]",
//            __lt_.tv_sec, __lt_.tv_usec, PROFILER::timeval2usec(&__lt_));


    __localTime = ::time(NULL);
    __runTime = clock() / CLOCKS_PER_SEC;
}
void DPN_TimeMoment::clear() {
    __localTime = 0;
    __runTime = 0;
}
void DPN_TimeMoment::setLocalTime(int s, int u) {
    __lt_.tv_sec = s;
    __lt_.tv_usec = u;
}
timeval DPN_TimeMoment::lt() const {
    return __lt_;
}
uint64_t DPN_TimeMoment::lt_useconds() const {
    return __lt_.tv_usec;
}
time_t DPN_TimeMoment::lt_seconds() const {
    return __localTime;
}
time_t DPN_TimeMoment::rt_seconds() const {
    return __runTime;
}
dpn_local_date DPN_TimeMoment::date() const {
    dpn_local_date d;
    struct tm now = *localtime(&__localTime);
    d.day = now.tm_mday;
    d.month = now.tm_mon + 1;
    d.year = now.tm_year + 1900;

    return d;
}
dpn_local_time DPN_TimeMoment::time() const {
    dpn_local_time t;
    struct tm now = *localtime(&__localTime);
    t.second = now.tm_sec;
    t.minute = now.tm_min;
    t.hour = now.tm_hour;
    t.usec = __lt_.tv_usec;

    return t;
}
dpn_local_moment DPN_TimeMoment::moment() const {
    dpn_local_moment m;
    struct tm now = *localtime(&__localTime);
    m.date.day = now.tm_mday;
    m.date.month = now.tm_mon + 1;
    m.date.year = now.tm_year + 1900;
    m.time.second = now.tm_sec;
    m.time.minute = now.tm_min;
    m.time.hour = now.tm_hour;

    return m;
}
dpn_time_range DPN_TimeMoment::runTime() const {
    dpn_time_range t;
    t.fill(__runTime);
    return t;
}
#define PARSE_TO_STRING(STRING, NAME, VALUE) \
    STRING.append("" #NAME ":["); STRING.append(std::to_string(VALUE)); STRING.append("]");


std::string DPN_TimeMoment::dateString() const {
    auto d = date();
    std::string s;
    PARSE_TO_STRING(s, day, d.day);
    PARSE_TO_STRING(s, month, d.month);
    PARSE_TO_STRING(s, year, d.year);

    return s;
}
std::string DPN_TimeMoment::timeString() const {
    auto t = time();
    std::string s;
    PARSE_TO_STRING(s, hour, t.hour);
    PARSE_TO_STRING(s, minute, t.minute);
    PARSE_TO_STRING(s, second, t.second);
    PARSE_TO_STRING(s, usec, t.usec);

    return s;
}
std::string DPN_TimeMoment::momentString() const {
    std::string s;
    s = dateString() + timeString();
    return s;
}
std::string DPN_TimeMoment::runTimeString() const {
    auto t = runTime();
    std::string s;
    PARSE_TO_STRING(s, days, t.days);
    PARSE_TO_STRING(s, hours, t.hours);
    PARSE_TO_STRING(s, minutes, t.minutes);
    PARSE_TO_STRING(s, seconds, t.seconds);

    return s;
}
//----------------------------------------------------------------------------------------------------- DPN_TimeRange:
DPN_TimeRange::DPN_TimeRange(bool fixFirst) : __begin(fixFirst) {

}
void DPN_TimeRange::fixBegin() {
    if(__last.fixed()) __last.clear();

    __begin.fixTime();
}
void DPN_TimeRange::fixLast() {

    if(__begin.fixed()) __last.fixTime();

}
void DPN_TimeRange::clear() {
    __begin.clear();
    __last.clear();
}
void DPN_TimeRange::clearLast() {
    __last.clear();
}
uint64_t DPN_TimeRange::useconds() const {
    if(__last.fixed())
        return __last.lt_useconds() - __begin.lt_useconds();
    else {
        DPN_TimeMoment moment;
        moment.fixTime();
        return moment.lt_useconds() - __begin.lt_useconds();
    }
}
time_t DPN_TimeRange::seconds() const {
    if(__last.fixed())
        return __last.lt_seconds() - __begin.lt_seconds();
    else {
        DPN_TimeMoment moment;
        moment.fixTime();
        return moment.lt_seconds() - __begin.lt_seconds();
    }
    return 0;
}
dpn_time_range DPN_TimeRange::range() const {
    dpn_time_range t;
    t.fill2(useconds());
    return t;
}



/*
DPN_Dialog DPN_DialogManager::shared_dialog;
DPN_DialogData::DPN_DialogData() {
    __parent = nullptr;
    __index = -1;
    __remoteDialogIndex = -1;
    __active = false;
    __client = nullptr;
}
void DPN_Dialog::stop() {
    if( isEmptyObject() || data()->__parent == nullptr ) {
        return;
    }
    data()->__parent->stopDialog(*this);
}
const DPN_DialogManager *DPN_Dialog::parent() const {
    if( isEmptyObject() ) {
        return nullptr;
    }
    return data()->__parent;
}
int DPN_Dialog::index() const {
    if( isEmptyObject() ) {
        return -1;
    }
    return data()->__index;
}

int DPN_Dialog::remoteIndex() const {
    if( isEmptyObject() ) {
        return -1;
    }
    return data()->__remoteDialogIndex;
}
bool DPN_Dialog::isActive() const {
    if( isEmptyObject() ) {
        return false;
    }
    return data()->__active;
}
bool DPN_Dialog::isValid(const DPN_AbstractClient *client) const {

    DL_INFO(1, "empty: [%d]", isEmptyObject());
    if (isEmptyObject() ) {
        return false;
    }
    DL_INFO(1, "active: [%d] inner client: [%p] client: [%p]", data()->__active, data()->__client, client);
    return data()->__active && data()->__client == client;
}
void DPN_Dialog::bindRemoteDialog(int remoteIndex) {
    if( isEmptyObject() ) return;
    data()->__remoteDialogIndex = remoteIndex;
}

void DPN_DialogManager::stopDialog(DPN_Dialog &dialog) {
    if( dialogs.contain(dialog) && dialog.parent() == this) {
        DL_INFO(1, "stop dialog [%d]", dialog.index());
        dialog.disactivate();
        indexPool.push_back(dialog.index());
    } else {
        DL_ERROR(1, "Can't stop dialog");
    }
}

DPN_Dialog &DPN_DialogManager::dialog(int index) {

    if( dialogs.inRange(index) ) {
        if( dialogs[index].isActive() ) {
            return dialogs[index];
        } else {
            DL_ERROR(1, "Non active dialog");
        }
    } else {
        DL_BADVALUE(1, "Index: [%d] size: [%d]", index, dialogs.size());
    }

    return shared_dialog;
}
*/
