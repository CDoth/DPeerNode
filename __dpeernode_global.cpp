#include "__dpeernode_global.h"
#include <DProfiler.h>

DLogs::DLogsContext DPN::Logs::log_context;


DLOGS_INIT_GLOBAL_CONTEXT("DPeerNode", DPN::Logs::logsInit);


using namespace DPN::Logs;

int pointerValue(const void *ptr) {
    return ptr ? *reinterpret_cast<const int*>(ptr) : 0;
}
namespace DPN::Time {
void time_range::fill(time_t __seconds) {
    useconds = 0;
    seconds = __seconds;
    minutes = seconds / 60; seconds %= 60;
    hours = minutes / 60; minutes %= 60;
    days = hours / 24; hours %= 24;
}
void time_range::fill2(int __useconds) {
    useconds = __useconds;
    seconds = useconds / 1000000; useconds %= 1000000;
    minutes = seconds / 60; seconds %= 60;
    hours = minutes / 60; minutes %= 60;
    days = hours / 24; hours %= 24;
}
//----------------------------------------------------------------------------------------------------- DPN_TimeMoment:
TimeMoment::TimeMoment(bool fix) {
    if(fix) fixTime();
    else {
        __localTime = 0;
        __runTime = 0;
        timerclear(&__lt_);
    }
}
void TimeMoment::fixTime() {
    gettimeofday(&__lt_, nullptr);

//    DL_INFO(1, "lt: sec: [%d] usec: [%d] total usec: [%d]",
//            __lt_.tv_sec, __lt_.tv_usec, PROFILER::timeval2usec(&__lt_));


    __localTime = ::time(NULL);
    __runTime = clock() / CLOCKS_PER_SEC;
}
void TimeMoment::clear() {
    __localTime = 0;
    __runTime = 0;
}
void TimeMoment::setLocalTime(int s, int u) {
    __lt_.tv_sec = s;
    __lt_.tv_usec = u;
}
timeval TimeMoment::lt() const {
    return __lt_;
}
uint64_t TimeMoment::lt_useconds() const {
    return __lt_.tv_usec;
}
time_t TimeMoment::lt_seconds() const {
    return __localTime;
}
time_t TimeMoment::rt_seconds() const {
    return __runTime;
}
local_date TimeMoment::date() const {
    local_date d;
    struct tm now = *localtime(&__localTime);
    d.day = now.tm_mday;
    d.month = now.tm_mon + 1;
    d.year = now.tm_year + 1900;

    return d;
}
local_time TimeMoment::time() const {
    local_time t;
    struct tm now = *localtime(&__localTime);
    t.second = now.tm_sec;
    t.minute = now.tm_min;
    t.hour = now.tm_hour;
    t.usec = __lt_.tv_usec;

    return t;
}
local_moment TimeMoment::moment() const {
    local_moment m;
    struct tm now = *localtime(&__localTime);
    m.date.day = now.tm_mday;
    m.date.month = now.tm_mon + 1;
    m.date.year = now.tm_year + 1900;
    m.time.second = now.tm_sec;
    m.time.minute = now.tm_min;
    m.time.hour = now.tm_hour;

    return m;
}
time_range TimeMoment::runTime() const {
    time_range t;
    t.fill(__runTime);
    return t;
}
#define PARSE_TO_STRING(STRING, NAME, VALUE) \
    STRING.append("" #NAME ":["); STRING.append(std::to_string(VALUE)); STRING.append("]");


std::string TimeMoment::dateString() const {
    auto d = date();
    std::string s;
    PARSE_TO_STRING(s, day, d.day);
    PARSE_TO_STRING(s, month, d.month);
    PARSE_TO_STRING(s, year, d.year);

    return s;
}
std::string TimeMoment::timeString() const {
    auto t = time();
    std::string s;
    PARSE_TO_STRING(s, hour, t.hour);
    PARSE_TO_STRING(s, minute, t.minute);
    PARSE_TO_STRING(s, second, t.second);
    PARSE_TO_STRING(s, usec, t.usec);

    return s;
}
std::string TimeMoment::momentString() const {
    std::string s;
    s = dateString() + timeString();
    return s;
}
std::string TimeMoment::runTimeString() const {
    auto t = runTime();
    std::string s;
    PARSE_TO_STRING(s, days, t.days);
    PARSE_TO_STRING(s, hours, t.hours);
    PARSE_TO_STRING(s, minutes, t.minutes);
    PARSE_TO_STRING(s, seconds, t.seconds);

    return s;
}
//----------------------------------------------------------------------------------------------------- DPN_TimeRange:
TimeRange::TimeRange(bool fixFirst) : __begin(fixFirst) {

}
void TimeRange::fixBegin() {
    if(__last.fixed()) __last.clear();

    __begin.fixTime();
}
void TimeRange::fixLast() {

    if(__begin.fixed()) __last.fixTime();

}
void TimeRange::clear() {
    __begin.clear();
    __last.clear();
}
void TimeRange::clearLast() {
    __last.clear();
}
uint64_t TimeRange::useconds() const {
    if(__last.fixed())
        return __last.lt_useconds() - __begin.lt_useconds();
    else {
        TimeMoment moment;
        moment.fixTime();
        return moment.lt_useconds() - __begin.lt_useconds();
    }
}
time_t TimeRange::seconds() const {
    if(__last.fixed())
        return __last.lt_seconds() - __begin.lt_seconds();
    else {
        TimeMoment moment;
        moment.fixTime();
        return moment.lt_seconds() - __begin.lt_seconds();
    }
    return 0;
}
time_range TimeRange::range() const {
    time_range t;
    t.fill2(useconds());
    return t;
}
}



