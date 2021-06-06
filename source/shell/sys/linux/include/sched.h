#ifndef __VSF_LINUX_SCHED_H__
#define __VSF_LINUX_SCHED_H__

#include "shell/sys/linux/vsf_linux_cfg.h"
#include "kernel/vsf_kernel.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_TIME == ENABLED
#else
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_CFG_FAKE_API == ENABLED
#define sched_get_priority_max      __vsf_linux_sched_get_priority_max
#define sched_get_priority_min      __vsf_linux_sched_get_priority_min
#define sched_getparam              __vsf_linux_sched_getparam
#define sched_getscheduler          __vsf_linux_sched_getscheduler
#define sched_setparam              __vsf_linux_sched_setparam
#define sched_yield                 __vsf_linux_sched_yield
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

int sched_get_priority_max(int);
int sched_get_priority_min(int);
int sched_getparam(pid_t pid, struct sched_param *param);
int sched_getscheduler(pid_t pid);
int sched_setparam(pid_t pid, const struct sched_param *param);
int sched_setscheduler(pid_t pid, int policy, const struct sched_param *param);
int sched_yield(void);

#ifdef __cplusplus
}
#endif

#endif
