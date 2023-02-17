#ifndef __VSF_LINUX_SYS_SENDFILE_H__
#define __VSF_LINUX_SYS_SENDFILE_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../sys/types.h"
#else
#   include <sys/types.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_CFG_WRAPPER == ENABLED
#define sendfile            VSF_LINUX_WRAPPER(sendfile)
#endif

#if VSF_LINUX_APPLET_USE_SYS_SENDFILE == ENABLED
typedef struct vsf_linux_sys_sendfile_vplt_t {
    vsf_vplt_info_t info;

    ssize_t (*sendfile)(int out_fd, int in_fd, off_t *offset, size_t count);
} vsf_linux_sys_sendfile_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_sys_sendfile_vplt_t vsf_linux_sys_sendfile_vplt;
#   endif
#endif

#if defined(__VSF_APPLET__) && VSF_LINUX_APPLET_USE_SYS_SENDFILE == ENABLED

#ifndef VSF_LINUX_APPLET_SYS_SENDFILE_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_SYS_SENDFILE_VPLT                               \
            ((vsf_linux_sys_sendfile_vplt_t *)(VSF_LINUX_APPLET_VPLT->sys_sendfile))
#   else
#       define VSF_LINUX_APPLET_SYS_SENDFILE_VPLT                               \
            ((vsf_linux_sys_sendfile_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

static inline ssize_t sendfile(int out_fd, int in_fd, off_t *offset, size_t count) {
    return VSF_LINUX_APPLET_SYS_SENDFILE_VPLT->sendfile(out_fd, in_fd, offset, count);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_SENDFILE

ssize_t sendfile(int out_fd, int in_fd, off_t *offset, size_t count);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_SENDFILE

#ifdef __cplusplus
}
#endif

#endif      // __VSF_LINUX_SYS_SENDFILE_H__
