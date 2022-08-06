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

#if VSF_LINUX_APPLET_USE_SYS_SHM == ENABLED
typedef struct vsf_linux_sys_shm_vplt_t {
    vsf_vplt_info_t info;

    int (*shmget)(key_t key, size_t size, int shmflg);
    void * (*shmat)(int shmid, const void *shmaddr, int shmflg);
    int (*shmdt)(const void *shmaddr);
    int (*shmctl)(int shmid, int cmd, struct shmid_ds *buf);
} vsf_linux_sys_shm_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_sys_shm_vplt_t vsf_linux_sys_shm_vplt;
#   endif
#endif

#if defined(__VSF_APPLET__) && VSF_LINUX_APPLET_USE_SYS_SHM == ENABLED

#ifndef VSF_LINUX_APPLET_SYS_SHM_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_SYS_SHM_VPLT                                    \
            ((vsf_linux_sys_shm_vplt_t *)(VSF_LINUX_APPLET_VPLT->sys_shm))
#   else
#       define VSF_LINUX_APPLET_SYS_SHM_VPLT                                    \
            ((vsf_linux_sys_shm_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

static inline int shmget(key_t key, size_t size, int shmflg) {
    return VSF_LINUX_APPLET_SYS_SHM_VPLT->shmget(key, size, shmflg);
}
static inline void * shmat(int shmid, const void *shmaddr, int shmflg) {
    return VSF_LINUX_APPLET_SYS_SHM_VPLT->shmat(shmid, shmaddr, shmflg);
}
static inline int shmdt(const void *shmaddr) {
    return VSF_LINUX_APPLET_SYS_SHM_VPLT->shmdt(shmaddr);
}
static inline int shmctl(int shmid, int cmd, struct shmid_ds *buf) {
    return VSF_LINUX_APPLET_SYS_SHM_VPLT->shmctl(shmid, cmd, buf);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_SHM

int shmget(key_t key, size_t size, int shmflg);
void * shmat(int shmid, const void *shmaddr, int shmflg);
int shmdt(const void *shmaddr);
int shmctl(int shmid, int cmd, struct shmid_ds *buf);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_SHM

#ifdef __cplusplus
}
#endif

#endif      // VSF_LINUX_CFG_SHM_NUM
#endif      // __VSF_LINUX_SHM_H__
