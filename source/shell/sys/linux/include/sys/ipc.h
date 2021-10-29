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
    IPC_NOEAIT              = 4 << 12,
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

key_t ftok(const char *pathname, int id);

#ifdef __cplusplus
}
#endif

#endif
