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

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(sendfile);
} vsf_linux_sys_sendfile_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_sys_sendfile_vplt_t vsf_linux_sys_sendfile_vplt;
#   endif
#endif

#if defined(__VSF_APPLET__) && defined(__VSF_APPLET_LIB__) && VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_APPLET_USE_SYS_SENDFILE == ENABLED

#ifndef VSF_LINUX_APPLET_SYS_SENDFILE_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_SYS_SENDFILE_VPLT                               \
            ((vsf_linux_sys_sendfile_vplt_t *)(VSF_LINUX_APPLET_VPLT->sys_sendfile_vplt))
#   else
#       define VSF_LINUX_APPLET_SYS_SENDFILE_VPLT                               \
            ((vsf_linux_sys_sendfile_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_SYS_SENDFILE_ENTRY(__NAME)                             \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_SYS_SENDFILE_VPLT, __NAME)
#define VSF_LINUX_APPLET_SYS_SENDFILE_IMP(...)                                  \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_SYS_SENDFILE_VPLT, __VA_ARGS__)

VSF_LINUX_APPLET_SYS_SENDFILE_IMP(sendfile, ssize_t, int out_fd, int in_fd, off_t *offset, size_t count) {
    return VSF_LINUX_APPLET_SYS_SENDFILE_ENTRY(sendfile)(out_fd, in_fd, offset, count);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_SENDFILE

ssize_t sendfile(int out_fd, int in_fd, off_t *offset, size_t count);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_SENDFILE

#ifdef __cplusplus
}
#endif

#endif      // __VSF_LINUX_SYS_SENDFILE_H__
