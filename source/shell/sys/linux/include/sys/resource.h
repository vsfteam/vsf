#ifndef __VSF_LINUX_SYS_RESOURCE_H__
#define __VSF_LINUX_SYS_RESOURCE_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "./types.h"
#   include "./time.h"
#else
#   include <sys/types.h>
#   include <sys/time.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define RLIMIT_CPU          0
#define RLIMIT_FSIZE        1
#define RLIMIT_DATA         2
#define RLIMIT_STACK        3
#define RLIMIT_CORE         4

#define RLIMIT_RSS          5
#define RLIMIT_NPROC        6
#define RLIMIT_NOFILE       7
#define RLIMIT_MEMLOCK      8
#define RLIMIT_LOCKS        10
#define RLIMIT_SIGPENDING   11
#define RLIMIT_MSGQUEUE     12
#define RLIMIT_NICE         13
#define RLIMIT_RTPRIO       14
#define RLIMIT_RTTIME       15
#define RLIM_NLIMITS        16

#define RLIM_INFINITY       (~0UL)

enum {
    RUSAGE_SELF             = 0,
    RUSAGE_CHILDREN,
    RUSAGE_THREAD,
};
struct rusage {
    struct timeval          ru_utime;
    struct timeval          ru_stime;
};

static inline int getrusage(int who, struct rusage *usage)
{
    return 0;
}

typedef unsigned long       rlim_t;
struct rlimit {
    rlim_t                  rlim_cur;
    rlim_t                  rlim_max;
};

static inline int getrlimit(int resource, struct rlimit *rlptr)
{
    if (rlptr != NULL) {
        rlptr->rlim_cur = 100;
        rlptr->rlim_max = 100;
    }
    return 0;
}

static inline int setrlimit(int resource, struct rlimit *rlptr)
{
    return 0;
}

#define PRIO_PROCESS        0
#define PRIO_PGRP           1
#define PRIO_USER           2

#if VSF_LINUX_APPLET_USE_SYS_RESOURCE == ENABLED
typedef struct vsf_linux_sys_resource_vplt_t {
    vsf_vplt_info_t info;

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(getpriority);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(setpriority);
} vsf_linux_sys_resource_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_sys_resource_vplt_t vsf_linux_sys_resource_vplt;
#   endif
#endif

#if     defined(__VSF_APPLET__) && (defined(__VSF_APPLET_LIB__) || defined(__VSF_APPLET_LINUX_SYS_RESOURCE_LIB__))\
    &&  VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_APPLET_USE_SYS_RESOURCE == ENABLED

#ifndef VSF_LINUX_APPLET_SYS_RESOURCE_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_SYS_RESOURCE_VPLT                               \
            ((vsf_linux_sys_resource_vplt_t *)(VSF_LINUX_APPLET_VPLT->sys_resource_vplt))
#   else
#       define VSF_LINUX_APPLET_SYS_RESOURCE_VPLT                               \
            ((vsf_linux_sys_resource_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_SYS_RESOURCE_ENTRY(__NAME)                             \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_SYS_RESOURCE_VPLT, __NAME)
#define VSF_LINUX_APPLET_SYS_RESOURCE_IMP(...)                                  \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_SYS_RESOURCE_VPLT, __VA_ARGS__)

VSF_LINUX_APPLET_SYS_RESOURCE_IMP(getpriority, int, int which, id_t who) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_RESOURCE_ENTRY(getpriority)(which, who);
}
VSF_LINUX_APPLET_SYS_RESOURCE_IMP(setpriority, int, int which, id_t who, int prio) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_RESOURCE_ENTRY(setpriority)(which, who, prio);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_RESOURCE

int getpriority(int which, id_t who);
int setpriority(int which, id_t who, int prio);

#endif

#ifdef __cplusplus
}
#endif

#endif
