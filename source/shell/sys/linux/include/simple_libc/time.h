#ifndef __SIMPLE_LIBC_TIME_H__
#define __SIMPLE_LIBC_TIME_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   if VSF_LINUX_USE_SIMPLE_LIBC == ENABLED
#       include "./stddef.h"
#   else
#       include <stddef.h>
#   endif
// for time_t
#   include "../sys/types.h"
// for sigevent
#   include "../signal.h"
#else
#   include <stddef.h>
// for time_t
#   include <sys/types.h>
// for sigevent
#   include <signal.h>
#endif

// Note:
//  libc/time.h can not include timeval, so can not include sys/time.h,
//      or it will conflict with timeval in winsock.h.

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_LIBC_CFG_WRAPPER == ENABLED
#define clock_gettime       VSF_LINUX_LIBC_WRAPPER(clock_gettime)
#define clock_settime       VSF_LINUX_LIBC_WRAPPER(clock_settime)
#define clock_getres        VSF_LINUX_LIBC_WRAPPER(clock_getres)
#define clock_nanosleep     VSF_LINUX_LIBC_WRAPPER(clock_nanosleep)
#define timer_create        VSF_LINUX_LIBC_WRAPPER(timer_create)
#define timer_settime       VSF_LINUX_LIBC_WRAPPER(timer_settime)
#define timer_gettime       VSF_LINUX_LIBC_WRAPPER(timer_gettime)
#define timer_delete        VSF_LINUX_LIBC_WRAPPER(timer_delete)
#define timer_getoverrun    VSF_LINUX_LIBC_WRAPPER(timer_getoverrun)
#define nanosleep           VSF_LINUX_LIBC_WRAPPER(nanosleep)
#define clock               VSF_LINUX_LIBC_WRAPPER(clock)
#define timespec_get        VSF_LINUX_LIBC_WRAPPER(timespec_get)
#define strftime            VSF_LINUX_LIBC_WRAPPER(strftime)
#define strptime            VSF_LINUX_LIBC_WRAPPER(strptime)
#define tzset               VSF_LINUX_LIBC_WRAPPER(tzset)
#define timegm              VSF_LINUX_LIBC_WRAPPER(timegm)
#endif

#define TIMER_ABSTIME       1

struct tm {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
    long int tm_gmtoff;
    const char *tm_zone;
};

struct timespec {
    time_t          tv_sec;
    long            tv_nsec;
};

struct itimerspec {
    struct timespec it_interval;
    struct timespec it_value;
};

struct timespec64 {
    time64_t        tv_sec;
    long            tv_nsec;
};
#ifdef __WIN__
typedef struct timespec64 _timespec64;
#endif

struct itimerspec64 {
    struct timespec64 it_interval;
    struct timespec64 it_value;
};

#if VSF_LINUX_APPLET_USE_LIBC_TIME == ENABLED
typedef struct vsf_linux_libc_time_vplt_t {
    vsf_vplt_info_t info;

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(clock);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(clock_gettime);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(clock_settime);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(clock_getres);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(clock_nanosleep);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(time);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(difftime);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(asctime);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(asctime_r);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(ctime);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(ctime_r);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(gmtime);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(gmtime_r);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(timegm);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(localtime);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(localtime_r);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(mktime);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(strftime);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(strptime);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(nanosleep);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(timer_create);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(timer_delete);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(timer_settime);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(timer_gettime);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(timer_getoverrun);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(timespec_get);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(tzset);
} vsf_linux_libc_time_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_libc_time_vplt_t vsf_linux_libc_time_vplt;
#   endif
#endif

#if     defined(__VSF_APPLET__) && (defined(__VSF_APPLET_LIB__) || defined(__VSF_APPLET_LINUX_LIBC_TIME_LIB__))\
    &&  VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_APPLET_USE_LIBC_TIME == ENABLED

#ifndef VSF_LINUX_APPLET_LIBC_TIME_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_LIBC_TIME_VPLT                                  \
            ((vsf_linux_libc_time_vplt_t *)(VSF_LINUX_APPLET_VPLT->libc_time_vplt))
#   else
#       define VSF_LINUX_APPLET_LIBC_TIME_VPLT                                  \
            ((vsf_linux_libc_time_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_LIBC_TIME_ENTRY(__NAME)                                \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_LIBC_TIME_VPLT, __NAME)
#define VSF_LINUX_APPLET_LIBC_TIME_IMP(...)                                     \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_LIBC_TIME_VPLT, __VA_ARGS__)

VSF_LINUX_APPLET_LIBC_TIME_IMP(clock, clock_t, void) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_TIME_ENTRY(clock)();
}
VSF_LINUX_APPLET_LIBC_TIME_IMP(clock_gettime, int, clockid_t clockid, struct timespec *ts) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_TIME_ENTRY(clock_gettime)(clockid, ts);
}
VSF_LINUX_APPLET_LIBC_TIME_IMP(clock_settime, int, clockid_t clockid, const struct timespec *ts) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_TIME_ENTRY(clock_settime)(clockid, ts);
}
VSF_LINUX_APPLET_LIBC_TIME_IMP(clock_getres, int, clockid_t clockid, struct timespec *res) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_TIME_ENTRY(clock_getres)(clockid, res);
}
VSF_LINUX_APPLET_LIBC_TIME_IMP(clock_nanosleep, int, clockid_t clockid, int flags, const struct timespec *request, struct timespec *remain) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_TIME_ENTRY(clock_nanosleep)(clockid, flags, request, remain);
}
VSF_LINUX_APPLET_LIBC_TIME_IMP(time, time_t, time_t *t) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_TIME_ENTRY(time)(t);
}
VSF_LINUX_APPLET_LIBC_TIME_IMP(difftime, double, time_t time1, time_t time2) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_TIME_ENTRY(difftime)(time1, time2);
}
VSF_LINUX_APPLET_LIBC_TIME_IMP(asctime, char *, const struct tm *tm) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_TIME_ENTRY(asctime)(tm);
}
VSF_LINUX_APPLET_LIBC_TIME_IMP(asctime_r, char *, const struct tm *tm, char *buf) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_TIME_ENTRY(asctime_r)(tm, buf);
}
VSF_LINUX_APPLET_LIBC_TIME_IMP(ctime, char *, const time_t *t) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_TIME_ENTRY(ctime)(t);
}
VSF_LINUX_APPLET_LIBC_TIME_IMP(ctime_r, char *, const time_t *t, char *buf) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_TIME_ENTRY(ctime_r)(t, buf);
}
VSF_LINUX_APPLET_LIBC_TIME_IMP(gmtime, struct tm *, const time_t *t) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_TIME_ENTRY(gmtime)(t);
}
VSF_LINUX_APPLET_LIBC_TIME_IMP(gmtime_r, struct tm *, const time_t *t, struct tm *result) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_TIME_ENTRY(gmtime_r)(t, result);
}
VSF_LINUX_APPLET_LIBC_TIME_IMP(localtime, struct tm *, const time_t *t) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_TIME_ENTRY(localtime)(t);
}
VSF_LINUX_APPLET_LIBC_TIME_IMP(localtime_r, struct tm *, const time_t *t, struct tm *result) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_TIME_ENTRY(localtime_r)(t, result);
}
VSF_LINUX_APPLET_LIBC_TIME_IMP(mktime, time_t, struct tm *tm) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_TIME_ENTRY(mktime)(tm);
}
VSF_LINUX_APPLET_LIBC_TIME_IMP(strftime, size_t, char *str, size_t maxsize, const char *format, const struct tm *tm) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_TIME_ENTRY(strftime)(str, maxsize, format, tm);
}
VSF_LINUX_APPLET_LIBC_TIME_IMP(strptime, char *, const char *str, const char *format, struct tm *tm) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_TIME_ENTRY(strptime)(str, format, tm);
}
VSF_LINUX_APPLET_LIBC_TIME_IMP(nanosleep, int, const struct timespec *requested_time, struct timespec *remaining) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_TIME_ENTRY(nanosleep)(requested_time, remaining);
}
VSF_LINUX_APPLET_LIBC_TIME_IMP(timer_create, int, clockid_t clockid, struct sigevent *sevp, timer_t *timerid) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_TIME_ENTRY(timer_create)(clockid, sevp, timerid);
}
VSF_LINUX_APPLET_LIBC_TIME_IMP(timer_delete, int, timer_t timerid) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_TIME_ENTRY(timer_delete)(timerid);
}
VSF_LINUX_APPLET_LIBC_TIME_IMP(timer_settime, int, timer_t timerid, int flags, const struct itimerspec *new_value, struct itimerspec *old_value) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_TIME_ENTRY(timer_settime)(timerid, flags, new_value, old_value);
}
VSF_LINUX_APPLET_LIBC_TIME_IMP(timer_gettime, int, timer_t timerid, struct itimerspec *curr_value) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_TIME_ENTRY(timer_gettime)(timerid, curr_value);
}
VSF_LINUX_APPLET_LIBC_TIME_IMP(timespec_get, int, struct timespec *ts, int base) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_TIME_ENTRY(timespec_get)(ts, base);
}
VSF_LINUX_APPLET_LIBC_TIME_IMP(tzset, void, void) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_LINUX_APPLET_LIBC_TIME_ENTRY(tzset)();
}
VSF_LINUX_APPLET_LIBC_TIME_IMP(timegm, time_t, struct tm *tm) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_LIBC_TIME_ENTRY(timegm)(tm);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_LIBC_TIME

clock_t clock(void);
int clock_gettime(clockid_t clockid, struct timespec *ts);
int clock_settime(clockid_t clockid, const struct timespec *ts);
int clock_getres(clockid_t clockid, struct timespec *res);
int clock_nanosleep(clockid_t clockid, int flags, const struct timespec *request,
                        struct timespec *remain);

int timer_create(clockid_t clockid, struct sigevent *sevp, timer_t *timerid);
int timer_settime(timer_t timerid, int flags, const struct itimerspec *new_value,
        struct itimerspec *old_value);
int timer_gettime(timer_t timerid, struct itimerspec *curr_value);
int timer_getoverrun(timer_t timerid);
int timer_delete(timer_t timerid);

time_t time(time_t *t);
double difftime(time_t time1, time_t time2);

char *asctime(const struct tm *tm);
char *asctime_r(const struct tm *tm, char *buf);

char *ctime(const time_t *t);
char *ctime_r(const time_t *t, char *buf);

struct tm *gmtime(const time_t *t);
struct tm *gmtime_r(const time_t *t, struct tm *result);

time_t timegm(struct tm *tm);

struct tm *localtime(const time_t *t);
struct tm *localtime_r(const time_t *t, struct tm *result);

time_t mktime(struct tm *tm);
size_t strftime(char *str, size_t maxsize, const char *format, const struct tm *tm);
char * strptime(const char *str, const char *format, struct tm *tm);

int nanosleep(const struct timespec *requested_time, struct timespec *remaining);

int timespec_get(struct timespec *ts, int base);
void tzset(void);
extern char *tzname[2];
extern long timezong;
extern int daylight;

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_LIBC_TIME

#ifdef __cplusplus
}
#endif

#endif
