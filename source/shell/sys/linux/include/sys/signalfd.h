#ifndef __VSF_LINUX_SIGNALFD_H__
#define __VSF_LINUX_SIGNALFD_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../sys/types.h"
#   include "../signal.h"
#else
#   include <sys/types.h>
#   include <signal.h>
#endif
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_CFG_WRAPPER == ENABLED
#define signalfd                VSF_LINUX_WRAPPER(signalfd)
#endif

struct signalfd_siginfo {
    uint32_t                    ssi_signo;
    uint32_t                    ssi_errno;
    uint32_t                    ssi_code;
    uint32_t                    ssi_pid;
    uint32_t                    ssi_uid;
    uint32_t                    ssi_fd;
    uint32_t                    ssi_tid;
};

#if VSF_LINUX_APPLET_USE_SYS_SIGNALFD == ENABLED
typedef struct vsf_linux_sys_signalfd_vplt_t {
    vsf_vplt_info_t info;

    int (*signalfd)(int fd, const sigset_t *mask, int flags);
} vsf_linux_sys_signalfd_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_sys_signalfd_vplt_t vsf_linux_sys_signalfd_vplt;
#   endif
#endif

#if defined(__VSF_APPLET__) && VSF_LINUX_APPLET_USE_SYS_SIGNALFD == ENABLED

#ifndef VSF_LINUX_APPLET_SYS_SIGNALFD_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_SYS_SIGNALFD_VPLT                               \
            ((vsf_linux_sys_signalfd_vplt_t *)(VSF_LINUX_APPLET_VPLT->sys_signalfd))
#   else
#       define VSF_LINUX_APPLET_SYS_SIGNALFD_VPLT                               \
            ((vsf_linux_sys_signalfd_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

static inline int signalfd(int fd, const sigset_t *mask, int flags) {
    return VSF_LINUX_APPLET_SYS_SIGNALFD_VPLT->signalfd(fd, mask, flags);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_SIGNALFD

int signalfd(int fd, const sigset_t *mask, int flags);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_SIGNALFD

#ifdef __cplusplus
}
#endif

#endif      // __VSF_LINUX_SIGNALFD_H__
