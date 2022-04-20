#ifndef __SIMPLE_LIBC_TIME_H__
#define __SIMPLE_LIBC_TIME_H__

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_LIBC == ENABLED
#   include "./stddef.h"
#else
#   include <stddef.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_LIBC_CFG_WRAPPER == ENABLED
#define clock_gettime       VSF_LINUX_LIBC_WRAPPER(clock_gettime)
#define clock_getres        VSF_LINUX_LIBC_WRAPPER(clock_getres)
#define nanosleep           VSF_LINUX_LIBC_WRAPPER(nanosleep)
#define clock               VSF_LINUX_LIBC_WRAPPER(clock)
#endif

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
    time_t  tv_sec;
    long    tv_nsec;
};

clock_t clock(void);
int clock_gettime(clockid_t clk_id, struct timespec *tp);
int clock_getres(clockid_t clk_id, struct timespec *res);

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

#ifdef __cplusplus
}
#endif

#endif
