#ifndef __VSF_LINUX_SYS_SHM_H__
#define __VSF_LINUX_SYS_SHM_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_SUPPORT_SHM == ENABLED

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

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(shmget);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(shmat);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(shmdt);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(shmctl);
} vsf_linux_sys_shm_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_sys_shm_vplt_t vsf_linux_sys_shm_vplt;
#   endif
#endif

#if     defined(__VSF_APPLET__) && (defined(__VSF_APPLET_LIB__) || defined(__VSF_APPLET_LINUX_SYS_SHM_LIB__))\
    &&  VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_APPLET_USE_SYS_SHM == ENABLED

#ifndef VSF_LINUX_APPLET_SYS_SHM_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_SYS_SHM_VPLT                                    \
            ((vsf_linux_sys_shm_vplt_t *)(VSF_LINUX_APPLET_VPLT->sys_shm_vplt))
#   else
#       define VSF_LINUX_APPLET_SYS_SHM_VPLT                                    \
            ((vsf_linux_sys_shm_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_SYS_SHM_ENTRY(__NAME)                                  \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_SYS_SHM_VPLT, __NAME)
#define VSF_LINUX_APPLET_SYS_SHM_IMP(...)                                       \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_SYS_SHM_VPLT, __VA_ARGS__)

VSF_LINUX_APPLET_SYS_SHM_IMP(shmget, int, key_t key, size_t size, int shmflg) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_SHM_ENTRY(shmget)(key, size, shmflg);
}
VSF_LINUX_APPLET_SYS_SHM_IMP(shmat, void *, int shmid, const void *shmaddr, int shmflg) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_SHM_ENTRY(shmat)(shmid, shmaddr, shmflg);
}
VSF_LINUX_APPLET_SYS_SHM_IMP(shmdt, int, const void *shmaddr) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_SHM_ENTRY(shmdt)(shmaddr);
}
VSF_LINUX_APPLET_SYS_SHM_IMP(shmctl, int, int shmid, int cmd, struct shmid_ds *buf) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_SHM_ENTRY(shmctl)(shmid, cmd, buf);
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

#endif      // VSF_LINUX_CFG_SUPPORT_SHM
#endif      // __VSF_LINUX_SYS_SHM_H__
