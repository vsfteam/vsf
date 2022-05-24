#ifndef __VSF_LINUX_SYS_TIME_H__
#define __VSF_LINUX_SYS_TIME_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_TIME == ENABLED
#   include "./types.h"
#   include "../simple_libc/time.h"
#else
#   include <sys/types.h>
#   include <time.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_CFG_WRAPPER == ENABLED
#define getitimer           VSF_LINUX_WRAPPER(getitimer)
#define setitimer           VSF_LINUX_WRAPPER(setitimer)
#define gettimeofday        VSF_LINUX_WRAPPER(gettimeofday)

#define utimes              VSF_LINUX_WRAPPER(utimes)
#endif

#ifndef __SUSECONDS_T
#   define __SUSECONDS_T    long int
#endif
typedef __SUSECONDS_T       suseconds_t;

enum {
    ITIMER_REAL     = 0,
    ITIMER_VIRTUAL  = 1,
    ITIMER_PROF     = 2,
    ITIMER_NUM      = 3,
};

struct timeval {
    time_t          tv_sec;
    suseconds_t     tv_usec;
};

struct timezone {
    int             tz_minuteswest;
    int             tz_dsttime;
};

struct itimerval {
    struct timeval  it_value;
    struct timeval  it_interval;
};

int gettimeofday(struct timeval * tv, struct timezone * tz);
int getitimer(int which, struct itimerval *curr_value);
int setitimer(int which, const struct itimerval *new_value, struct itimerval *old_value);

int futimes(int fd, const struct timeval tv[2]);
int utimes(const char *filename, const struct timeval times[2]);

#ifdef __cplusplus
}
#endif

#endif
