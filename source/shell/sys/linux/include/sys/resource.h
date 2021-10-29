#ifndef __VSF_LINUX_RESOURCE_H__
#define __VSF_LINUX_RESOURCE_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../sys/types.h"
#else
#   include <sys/types.h>
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

#ifdef __cplusplus
}
#endif

#endif
