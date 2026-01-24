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
#define sched_getcpu                VSF_LINUX_WRAPPER(sched_getcpu)
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

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(sched_get_priority_max);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(sched_get_priority_min);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(sched_getparam);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(sched_getscheduler);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(sched_setparam);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(sched_setscheduler);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(sched_yield);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(sched_getcpu);
} vsf_linux_sched_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_sched_vplt_t vsf_linux_sched_vplt;
#   endif
#endif

#if     defined(__VSF_APPLET__) && (defined(__VSF_APPLET_LIB__) || defined(__VSF_APPLET_LINUX_SCHED_LIB__))\
    &&  VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_APPLET_USE_SCHED == ENABLED

#ifndef VSF_LINUX_APPLET_SCHED_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_SCHED_VPLT                                      \
            ((vsf_linux_sched_vplt_t *)(VSF_LINUX_APPLET_VPLT->sched_vplt))
#   else
#       define VSF_LINUX_APPLET_SCHED_VPLT                                      \
            ((vsf_linux_sched_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_SCHED_ENTRY(__NAME)                                    \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_SCHED_VPLT, __NAME)
#define VSF_LINUX_APPLET_SCHED_IMP(...)                                         \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_SCHED_VPLT, __VA_ARGS__)

VSF_LINUX_APPLET_SCHED_IMP(sched_get_priority_max, int, int policy) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SCHED_ENTRY(sched_get_priority_max)(policy);
}
VSF_LINUX_APPLET_SCHED_IMP(sched_get_priority_min, int, int policy) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SCHED_ENTRY(sched_get_priority_min)(policy);
}
VSF_LINUX_APPLET_SCHED_IMP(sched_getparam, int, pid_t pid, struct sched_param *param) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SCHED_ENTRY(sched_getparam)(pid, param);
}
VSF_LINUX_APPLET_SCHED_IMP(sched_getscheduler, int, pid_t pid) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SCHED_ENTRY(sched_getscheduler)(pid);
}
VSF_LINUX_APPLET_SCHED_IMP(sched_setparam, int, pid_t pid, const struct sched_param *param) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SCHED_ENTRY(sched_setparam)(pid, param);
}
VSF_LINUX_APPLET_SCHED_IMP(sched_setscheduler, int, pid_t pid, int policy, const struct sched_param *param) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SCHED_ENTRY(sched_setscheduler)(pid, policy, param);
}
VSF_LINUX_APPLET_SCHED_IMP(sched_yield, int, void) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SCHED_ENTRY(sched_yield)();
}
VSF_LINUX_APPLET_SCHED_IMP(sched_getcpu, int, void) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SCHED_ENTRY(sched_getcpu)();
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SCHED

int sched_get_priority_max(int policy);
int sched_get_priority_min(int policy);
int sched_getparam(pid_t pid, struct sched_param *param);
int sched_getscheduler(pid_t pid);
int sched_setparam(pid_t pid, const struct sched_param *param);
int sched_setscheduler(pid_t pid, int policy, const struct sched_param *param);
int sched_yield(void);
int sched_getcpu(void);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SCHED

#ifdef __cplusplus
}
#endif

#endif
