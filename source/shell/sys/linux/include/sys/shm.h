#ifndef __VSF_LINUX_SHM_H__
#define __VSF_LINUX_SHM_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

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

#define shmget              __vsf_linux_shmget
#define shmat               __vsf_linux_shmat
#define shmdt               __vsf_linux_shmdt
#define shmctl              __vsf_linux_shmctl

int shmget(key_t key, size_t size, int shmflg);
void * shmat(int shmid, const void *shmaddr, int shmflg);
int shmdt(const void *shmaddr);
int shmctl(int shmid, int cmd, struct shmid_ds *buf);

#ifdef __cplusplus
}
#endif

#endif
