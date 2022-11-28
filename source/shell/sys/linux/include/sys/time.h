#ifndef __VSF_LINUX_SYS_TIME_H__
#define __VSF_LINUX_SYS_TIME_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_TIME == ENABLED
#   include "./types.h"
#else
#   include <sys/types.h>
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

struct timespec {
    time_t          tv_sec;
    long            tv_nsec;
};

struct itimerval {
    struct timeval  it_value;
    struct timeval  it_interval;
};

struct itimerspec {
    struct timespec it_interval;
    struct timespec it_value;
};

#if VSF_LINUX_APPLET_USE_SYS_TIME == ENABLED
typedef struct vsf_linux_sys_time_vplt_t {
    vsf_vplt_info_t info;

    int (*gettimeofday)(struct timeval * tv, struct timezone * tz);
    int (*getitimer)(int which, struct itimerval *curr_value);
    int (*setitimer)(int which, const struct itimerval *new_value, struct itimerval *old_value);

    int (*futimes)(int fd, const struct timeval tv[2]);
    int (*utimes)(const char *filename, const struct timeval times[2]);
} vsf_linux_sys_time_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_sys_time_vplt_t vsf_linux_sys_time_vplt;
#   endif
#endif

#if defined(__VSF_APPLET__) && VSF_LINUX_APPLET_USE_SYS_TIME == ENABLED

#ifndef VSF_LINUX_APPLET_SYS_TIME_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_SYS_TIME_VPLT                                    \
            ((vsf_linux_sys_time_vplt_t *)(VSF_LINUX_APPLET_VPLT->sys_time))
#   else
#       define VSF_LINUX_APPLET_SYS_TIME_VPLT                                    \
            ((vsf_linux_sys_time_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

static inline int gettimeofday(struct timeval * tv, struct timezone * tz) {
    return VSF_LINUX_APPLET_SYS_TIME_VPLT->gettimeofday(tv, tz);
}
static inline int getitimer(int which, struct itimerval *curr_value) {
    return VSF_LINUX_APPLET_SYS_TIME_VPLT->getitimer(which, curr_value);
}
static inline int setitimer(int which, const struct itimerval *new_value, struct itimerval *old_value) {
    return VSF_LINUX_APPLET_SYS_TIME_VPLT->setitimer(which, new_value, old_value);
}

static inline int futimes(int fd, const struct timeval tv[2]) {
    return VSF_LINUX_APPLET_SYS_TIME_VPLT->futimes(fd, tv);
}
static inline int utimes(const char *filename, const struct timeval times[2]) {
    return VSF_LINUX_APPLET_SYS_TIME_VPLT->utimes(filename, times);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_TIME

int gettimeofday(struct timeval * tv, struct timezone * tz);
int getitimer(int which, struct itimerval *curr_value);
int setitimer(int which, const struct itimerval *new_value, struct itimerval *old_value);

int futimes(int fd, const struct timeval tv[2]);
int utimes(const char *filename, const struct timeval times[2]);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_TIME

#ifdef __cplusplus
}
#endif

#endif
