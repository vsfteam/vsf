#ifndef __SIMPLE_LIBC_TIME_H__
#define __SIMPLE_LIBC_TIME_H__

#ifdef __cplusplus
extern "C" {
#endif

#define clock_gettime       __vsf_linux_clock_gettime
#define nanosleep           __vsf_linux_nanosleep
#define clock               __vsf_linux_clock

typedef long long   time_t;
typedef long        clock_t;

#define CLOCKS_PER_SEC ((clock_t)1000000)

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
    long    tv_sec;
    long    tv_nsec;
};

typedef enum {
    CLOCK_MONOTONIC,
    CLOCK_REALTIME,
} clockid_t;

clock_t clock(void);
int clock_gettime(clockid_t clk_id, struct timespec *tp);

time_t time(time_t *t);
char *asctime(const struct tm *tm);
char *ctime(const time_t *t);
double difftime(time_t time1, time_t time2);
struct tm *gmtime(const time_t *t);
struct tm *localtime(const time_t *t);
time_t mktime(struct tm *tm);
size_t strftime(char *str, size_t maxsize, const char *format, const struct tm *tm);

int nanosleep(const struct timespec *requested_time, struct timespec *remaining);

#ifdef __cplusplus
}
#endif

#endif
