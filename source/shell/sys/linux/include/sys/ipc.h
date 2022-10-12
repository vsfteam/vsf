#ifndef __VSF_LINUX_IPC_H__
#define __VSF_LINUX_IPC_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../sys/types.h"
#else
#   include <sys/types.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define IPC_PRIVATE         ((key_t)0)
enum {
    IPC_CREAT               = 1 << 12,
    IPC_EXCL                = 2 << 12,
    IPC_NOWAIT              = 4 << 12,
};
enum {
    IPC_RMID,
    IPC_SET,
    IPC_STAT,
};

struct ipc_perm {
    key_t                   key;
    uid_t                   uid;
    gid_t                   gid;
    uid_t                   cuid;
    gid_t                   cgid;
    unsigned short          mode;
    unsigned short          seq;
};

#if VSF_LINUX_CFG_WRAPPER == ENABLED
#define ftok                VSF_LINUX_WRAPPER(ftok)
#endif

#if VSF_LINUX_APPLET_USE_SYS_IPC == ENABLED
typedef struct vsf_linux_sys_ipc_vplt_t {
    vsf_vplt_info_t info;
} vsf_linux_sys_ipc_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_sys_ipc_vplt_t vsf_linux_sys_ipc_vplt;
#   endif
#endif

#if defined(__VSF_APPLET__) && VSF_LINUX_APPLET_USE_SYS_IPC == ENABLED

#ifndef VSF_LINUX_APPLET_SYS_IPC_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_SYS_IPC_VPLT                                    \
            ((vsf_linux_sys_ipc_vplt_t *)(VSF_LINUX_APPLET_VPLT->sys_ipc))
#   else
#       define VSF_LINUX_APPLET_SYS_IPC_VPLT                                    \
            ((vsf_linux_sys_ipc_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_IPC

key_t ftok(const char *pathname, int id);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_IPC

#ifdef __cplusplus
}
#endif

#endif
