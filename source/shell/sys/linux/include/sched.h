#ifndef __VSF_LINUX_SCHED_H__
#define __VSF_LINUX_SCHED_H__

#include "shell/sys/linux/vsf_linux_cfg.h"
#include "kernel/vsf_kernel.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "./sys/types.h"
#else
#   include <sys/types.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_CFG_WRAPPER == ENABLED
#define sched_get_priority_max      VSF_LINUX_WRAPPER(sched_get_priority_max)
#define sched_get_priority_min      VSF_LINUX_WRAPPER(sched_get_priority_min)
#define sched_getparam              VSF_LINUX_WRAPPER(sched_getparam)
#define sched_getscheduler          VSF_LINUX_WRAPPER(sched_getscheduler)
#define sched_setparam              VSF_LINUX_WRAPPER(sched_setparam)
#define sched_yield                 VSF_LINUX_WRAPPER(sched_yield)
#endif

enum {
    SCHED_OTHER,
    SCHED_BATCH,
    SCHED_IDLE,
    SCHED_FIFO,
    SCHED_RR,
};
struct sched_param {
    int                             sched_priority;
};

#if VSF_LINUX_APPLET_USE_SCHED == ENABLED
typedef struct vsf_linux_sched_vplt_t {
    vsf_vplt_info_t info;
} vsf_linux_sched_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_sched_vplt_t vsf_linux_sched_vplt;
#   endif
#endif

#if defined(__VSF_APPLET__) && VSF_LINUX_APPLET_USE_SCHED == ENABLED

#ifndef VSF_LINUX_APPLET_SCHED_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_SCHED_VPLT                                      \
            ((vsf_linux_sched_vplt_t *)(VSF_LINUX_APPLET_VPLT->sched))
#   else
#       define VSF_LINUX_APPLET_SCHED_VPLT                                      \
            ((vsf_linux_sched_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SCHED

int sched_get_priority_max(int policy);
int sched_get_priority_min(int policy);
int sched_getparam(pid_t pid, struct sched_param *param);
int sched_getscheduler(pid_t pid);
int sched_setparam(pid_t pid, const struct sched_param *param);
int sched_setscheduler(pid_t pid, int policy, const struct sched_param *param);
int sched_yield(void);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SCHED

#ifdef __cplusplus
}
#endif

#endif
