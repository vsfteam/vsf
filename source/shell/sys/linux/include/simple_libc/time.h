#ifndef __SIMPLE_LIBC_TIME_H__
#define __SIMPLE_LIBC_TIME_H__

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
#include <stddef.h>
// for time_t
#   include <sys/types.h>
// for sigevent
#   include <signal.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_LIBC_CFG_WRAPPER == ENABLED
#define clock_gettime       VSF_LINUX_LIBC_WRAPPER(clock_gettime)
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

#if VSF_LINUX_APPLET_USE_LIBC_TIME == ENABLED
typedef struct vsf_linux_libc_time_vplt_t {
    vsf_vplt_info_t info;

    clock_t (*clock)(void);
    int (*clock_gettime)(clockid_t clk_id, struct timespec *tp);
    int (*clock_getres)(clockid_t clk_id, struct timespec *res);
    int (*clock_nanosleep)(clockid_t clockid, int flags, const struct timespec *request,
                    struct timespec *remain);
    time_t (*time)(time_t *t);
    double (*difftime)(time_t time1, time_t time2);
    char * (*asctime)(const struct tm *tm);
    char * (*asctime_r)(const struct tm *tm, char *buf);
    char * (*ctime)(const time_t *t);
    char * (*ctime_r)(const time_t *t, char *buf);
    struct tm * (*gmtime)(const time_t *t);
    struct tm * (*gmtime_r)(const time_t *t, struct tm *result);
    struct tm * (*localtime)(const time_t *t);
    struct tm * (*localtime_r)(const time_t *t, struct tm *result);
    time_t (*mktime)(struct tm *tm);
    size_t (*strftime)(char *str, size_t maxsize, const char *format, const struct tm *tm);
    int (*nanosleep)(const struct timespec *requested_time, struct timespec *remaining);

    int (*timer_create)(clockid_t clockid, struct sigevent *sevp, timer_t *timerid);
    int (*timer_delete)(timer_t timerid);
    int (*timer_settime)(timer_t timerid, int flags, const struct itimerspec *new_value,
            struct itimerspec *old_value);
    int (*timer_gettime)(timer_t timerid, struct itimerspec *curr_value);
    int (*timer_getoverrun)(timer_t timerid);

    int (*timespec_get)(struct timespec *ts, int base);

} vsf_linux_libc_time_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_libc_time_vplt_t vsf_linux_libc_time_vplt;
#   endif
#endif

#if defined(__VSF_APPLET__) && VSF_LINUX_APPLET_USE_LIBC_TIME == ENABLED

#ifndef VSF_LINUX_APPLET_LIBC_TIME_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_LIBC_TIME_VPLT                                  \
            ((vsf_linux_libc_time_vplt_t *)(VSF_LINUX_APPLET_VPLT->libc_time))
#   else
#       define VSF_LINUX_APPLET_LIBC_TIME_VPLT                                  \
            ((vsf_linux_libc_time_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

static inline clock_t clock(void) {
    return VSF_LINUX_APPLET_LIBC_TIME_VPLT->clock();
}
static inline int clock_gettime(clockid_t clk_id, struct timespec *tp) {
    return VSF_LINUX_APPLET_LIBC_TIME_VPLT->clock_gettime(clk_id, tp);
}
static inline int clock_getres(clockid_t clk_id, struct timespec *res) {
    return VSF_LINUX_APPLET_LIBC_TIME_VPLT->clock_getres(clk_id, res);
}
static inline int clock_nanosleep(clockid_t clockid, int flags,
        const struct timespec *request, struct timespec *remain) {
    return VSF_LINUX_APPLET_LIBC_TIME_VPLT->clock_nanosleep(clk_id, res);
}
static inline time_t time(time_t *t) {
    return VSF_LINUX_APPLET_LIBC_TIME_VPLT->time(t);
}
static inline double difftime(time_t time1, time_t time2) {
    return VSF_LINUX_APPLET_LIBC_TIME_VPLT->difftime(time1, time2);
}
static inline char *asctime(const struct tm *tm) {
    return VSF_LINUX_APPLET_LIBC_TIME_VPLT->asctime(tm);
}
static inline char *asctime_r(const struct tm *tm, char *buf) {
    return VSF_LINUX_APPLET_LIBC_TIME_VPLT->asctime_r(tm, buf);
}
static inline char *ctime(const time_t *t) {
    return VSF_LINUX_APPLET_LIBC_TIME_VPLT->ctime(t);
}
static inline char *ctime_r(const time_t *t, char *buf) {
    return VSF_LINUX_APPLET_LIBC_TIME_VPLT->ctime_r(t, buf);
}
static inline struct tm *gmtime(const time_t *t) {
    return VSF_LINUX_APPLET_LIBC_TIME_VPLT->gmtime(t);
}
static inline struct tm *gmtime_r(const time_t *t, struct tm *result) {
    return VSF_LINUX_APPLET_LIBC_TIME_VPLT->gmtime_r(t, result);
}
static inline struct tm *localtime(const time_t *t) {
    return VSF_LINUX_APPLET_LIBC_TIME_VPLT->localtime(t);
}
static inline struct tm *localtime_r(const time_t *t, struct tm *result) {
    return VSF_LINUX_APPLET_LIBC_TIME_VPLT->localtime_r(t, result);
}
static inline time_t mktime(struct tm *tm) {
    return VSF_LINUX_APPLET_LIBC_TIME_VPLT->mktime(tm);
}
static inline size_t strftime(char *str, size_t maxsize, const char *format, const struct tm *tm) {
    return VSF_LINUX_APPLET_LIBC_TIME_VPLT->strftime(str, maxsize, format, tm);
}
static inline int nanosleep(const struct timespec *requested_time, struct timespec *remaining) {
    return VSF_LINUX_APPLET_LIBC_TIME_VPLT->nanosleep(requested_time, remaining);
}
static inline int timer_create(clockid_t clockid, struct sigevent *sevp, timer_t *timerid) {
    return VSF_LINUX_APPLET_LIBC_TIME_VPLT->timer_create(clockid, sevp, timerid);
}
static inline int timer_delete(timer_t timerid) {
    return VSF_LINUX_APPLET_LIBC_TIME_VPLT->timer_delete(timerid);
}
static inline int timer_settime(timer_t timerid, int flags, const struct itimerspec *new_value,
        struct itimerspec *old_value) {
    return VSF_LINUX_APPLET_LIBC_TIME_VPLT->timer_settime(timerid, flags, new_value, old_value);
}
static inline int timer_gettime(timer_t timerid, struct itimerspec *curr_value) {
    return VSF_LINUX_APPLET_LIBC_TIME_VPLT->timer_gettime(timerid, curr_value);
}
static inline int timespec_get(struct timespec *ts, int base) {
    return VSF_LINUX_APPLET_LIBC_TIME_VPLT->timespec_get(ts, base);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_LIBC_TIME

clock_t clock(void);
int clock_gettime(clockid_t clk_id, struct timespec *tp);
int clock_getres(clockid_t clk_id, struct timespec *res);
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

struct tm *localtime(const time_t *t);
struct tm *localtime_r(const time_t *t, struct tm *result);

time_t mktime(struct tm *tm);
size_t strftime(char *str, size_t maxsize, const char *format, const struct tm *tm);

int nanosleep(const struct timespec *requested_time, struct timespec *remaining);

int timespec_get(struct timespec *ts, int base);
void tzset(void);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_LIBC_TIME

#ifdef __cplusplus
}
#endif

#endif
