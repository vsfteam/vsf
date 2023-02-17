#ifndef __VSF_LINUX_SYS_FILE_H__
#define __VSF_LINUX_SYS_FILE_H__

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
#define flock               VSF_LINUX_WRAPPER(flock)
#endif

#define LOCK_SH             (1 << 0)
#define LOCK_EX             (1 << 1)
#define LOCK_UN             (1 << 2)

#if VSF_LINUX_APPLET_USE_SYS_FILE == ENABLED
typedef struct vsf_linux_sys_file_vplt_t {
    vsf_vplt_info_t info;

    int (*flock)(int fd, int operation);
} vsf_linux_sys_file_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_sys_file_vplt_t vsf_linux_sys_file_vplt;
#   endif
#endif

#if defined(__VSF_APPLET__) && VSF_LINUX_APPLET_USE_SYS_FILE == ENABLED

#ifndef VSF_LINUX_APPLET_SYS_FILE_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_SYS_FILE_VPLT                                   \
            ((vsf_linux_sys_file_vplt_t *)(VSF_LINUX_APPLET_VPLT->sys_file))
#   else
#       define VSF_LINUX_APPLET_SYS_FILE_VPLT                                   \
            ((vsf_linux_sys_file_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

static inline int flock(int fd, int operation) {
    return VSF_LINUX_APPLET_SYS_FILE_VPLT->flock(fd, operation);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_FILE

int flock(int fd, int operation);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_FILE

#ifdef __cplusplus
}
#endif

#endif      // __VSF_LINUX_SYS_FILE_H__
