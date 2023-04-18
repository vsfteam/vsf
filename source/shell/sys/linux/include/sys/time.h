#ifndef __VSF_LINUX_SYS_TIME_H__
#define __VSF_LINUX_SYS_TIME_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "./types.h"
#else
#   include <sys/types.h>
#endif

// ugly, but seems no choice, because:
//  libc/time.h can not include timeval, so can not include sys/time.h,
//      or it will conflict with timeval in winsock.h.
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_CFG_WRAPPER == ENABLED
#define getitimer           VSF_LINUX_WRAPPER(getitimer)
#define setitimer           VSF_LINUX_WRAPPER(setitimer)
#define gettimeofday        VSF_LINUX_WRAPPER(gettimeofday)
#define settimeofday        VSF_LINUX_WRAPPER(settimeofday)

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

#if VSF_LINUX_APPLET_USE_SYS_TIME == ENABLED
typedef struct vsf_linux_sys_time_vplt_t {
    vsf_vplt_info_t info;

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(gettimeofday);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(settimeofday);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(getitimer);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(setitimer);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(futimes);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(utimes);
} vsf_linux_sys_time_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_sys_time_vplt_t vsf_linux_sys_time_vplt;
#   endif
#endif

#if defined(__VSF_APPLET__) && VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_APPLET_USE_SYS_TIME == ENABLED

#ifndef VSF_LINUX_APPLET_SYS_TIME_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_SYS_TIME_VPLT                                    \
            ((vsf_linux_sys_time_vplt_t *)(VSF_LINUX_APPLET_VPLT->sys_time_vplt))
#   else
#       define VSF_LINUX_APPLET_SYS_TIME_VPLT                                    \
            ((vsf_linux_sys_time_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_SYS_TIME_ENTRY(__NAME)                                 \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_SYS_TIME_VPLT, __NAME)
#define VSF_LINUX_APPLET_SYS_TIME_IMP(...)                                      \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_SYS_TIME_VPLT, __VA_ARGS__)

VSF_LINUX_APPLET_SYS_TIME_IMP(gettimeofday, int, struct timeval *tv, struct timezone *tz) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_TIME_ENTRY(gettimeofday)(tv, tz);
}
VSF_LINUX_APPLET_SYS_TIME_IMP(settimeofday, int, const struct timeval *tv, const struct timezone *tz) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_TIME_ENTRY(settimeofday)(tv, tz);
}
VSF_LINUX_APPLET_SYS_TIME_IMP(getitimer, int, int which, struct itimerval *curr_value) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_TIME_ENTRY(getitimer)(which, curr_value);
}
VSF_LINUX_APPLET_SYS_TIME_IMP(setitimer, int, int which, const struct itimerval *new_value, struct itimerval *old_value) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_TIME_ENTRY(setitimer)(which, new_value, old_value);
}
VSF_LINUX_APPLET_SYS_TIME_IMP(futimes, int, int fd, const struct timeval tv[2]) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_TIME_ENTRY(futimes)(fd, tv);
}
VSF_LINUX_APPLET_SYS_TIME_IMP(utimes, int, const char *filename, const struct timeval times[2]) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_TIME_ENTRY(utimes)(filename, times);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_TIME

int gettimeofday(struct timeval *tv, struct timezone *tz);
int settimeofday(const struct timeval *tv, const struct timezone *tz);
int getitimer(int which, struct itimerval *curr_value);
int setitimer(int which, const struct itimerval *new_value, struct itimerval *old_value);

int futimes(int fd, const struct timeval tv[2]);
int utimes(const char *filename, const struct timeval times[2]);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_TIME

#ifdef __cplusplus
}
#endif

#endif
