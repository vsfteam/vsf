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

enum {
    IPC_PRIVATE             = 0,
    IPC_CREAT               = 1,
    IPC_EXCL                = 2,
    IPC_NOEAIT              = 3,
    IPC_RMID                = 4,
    IPC_SET                 = 5,
    IPC_STAT                = 6,
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

struct shmid_ds {
    struct ipc_perm         shm_perm;
    int                     shm_segsz;
    time_t                  shm_atime;
    time_t                  shm_dtime;
    time_t                  shm_ctime;
    unsigned short          shm_cpid;
    unsigned short          shm_lpid;
    short                   shm_nattch;
};

#define ftok                __vsf_linux_ftok

key_t ftok(const char *pathname, int id);

#ifdef __cplusplus
}
#endif

#endif
