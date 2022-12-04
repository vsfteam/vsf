#ifndef __VSF_LINUX_WAIT_H__
#define __VSF_LINUX_WAIT_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../sys/types.h"
#   include "../signal.h"
#else
#   include <sys/types.h>
#   include <signal.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_CFG_WRAPPER == ENABLED
#define waitpid                 VSF_LINUX_WRAPPER(waitpid)
#endif

// options of waitpid
#define WEXITED                 1
#define WSTOPPED                2
#define WCONTINUED              3
#define WNOHANG                 4
#define WNOWAIT                 5
#define WUNTRACED               6

// internal usage
#define PID_STATUS_RUNNING      (1 << 0)
#define PID_STATUS_DAEMON       (1 << 1)

#define WIFEXITED(__STATUS)     !((__STATUS) & 0xFF)
#define WEXITSTATUS(__STATUS)   ((__STATUS) >> 8)
// TODO: implement WIFSIGNALED
#define WIFSIGNALED(__STATUS)   0
#define WTERMSIG(__STATUS)      ((__STATUS) & 0x7F)

#if VSF_LINUX_APPLET_USE_SYS_WAIT == ENABLED
typedef struct vsf_linux_sys_wait_vplt_t {
    vsf_vplt_info_t info;

    pid_t (*wait)(int *status);
    pid_t (*waitpid)(pid_t pid, int *status, int options);
    int (*waitid)(idtype_t idtype, id_t id, siginfo_t *infop, int options);
} vsf_linux_sys_wait_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_sys_wait_vplt_t vsf_linux_sys_wait_vplt;
#   endif
#endif

#if defined(__VSF_APPLET__) && VSF_LINUX_APPLET_USE_SYS_WAIT == ENABLED

#ifndef VSF_LINUX_APPLET_SYS_WAIT_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_SYS_WAIT_VPLT                                   \
            ((vsf_linux_sys_wait_vplt_t *)(VSF_LINUX_APPLET_VPLT->sys_wait))
#   else
#       define VSF_LINUX_APPLET_SYS_WAIT_VPLT                                   \
            ((vsf_linux_sys_wait_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

static inline pid_t wait(int *status) {
    return VSF_LINUX_APPLET_SYS_WAIT_VPLT->wait(status);
}
static inline pid_t waitpid(pid_t pid, int *status, int options) {
    return VSF_LINUX_APPLET_SYS_WAIT_VPLT->waitpid(pid, status, options);
}
static inline int waitid(idtype_t idtype, id_t id, siginfo_t *infop, int options) {
    return VSF_LINUX_APPLET_SYS_WAIT_VPLT->waitid(idtype, id, infop, options);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_WAIT

pid_t wait(int *status);
pid_t waitpid(pid_t pid, int *status, int options);
int waitid(idtype_t idtype, id_t id, siginfo_t *infop, int options);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_WAIT

#ifdef __cplusplus
}
#endif

#endif
