#ifndef __VSF_LINUX_SHM_H__
#define __VSF_LINUX_SHM_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_SHM_NUM > 0

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../sys/types.h"
#   include "../sys/ipc.h"
#   include "../simple_libc/time.h"
#else
#   include <sys/types.h>
#   include <sys/ipc.h>
#   include <time.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

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

#if VSF_LINUX_CFG_WRAPPER == ENABLED
#define shmget              VSF_LINUX_WRAPPER(shmget)
#define shmat               VSF_LINUX_WRAPPER(shmat)
#define shmdt               VSF_LINUX_WRAPPER(shmdt)
#define shmctl              VSF_LINUX_WRAPPER(shmctl)
#endif

int shmget(key_t key, size_t size, int shmflg);
void * shmat(int shmid, const void *shmaddr, int shmflg);
int shmdt(const void *shmaddr);
int shmctl(int shmid, int cmd, struct shmid_ds *buf);

#ifdef __cplusplus
}
#endif

#endif      // VSF_LINUX_CFG_SHM_NUM
#endif      // __VSF_LINUX_SHM_H__
