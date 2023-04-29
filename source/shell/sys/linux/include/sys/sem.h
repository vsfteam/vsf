#ifndef __VSF_LINUX_SYS_SEM_H__
#define __VSF_LINUX_SYS_SEM_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "./types.h"
#   include "./ipc.h"
#else
#   include <sys/types.h>
#   include <sys/ipc.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

// semop flags
#define SEM_UNDO            (1 << 0)

// semctl cmd
#define GETPID              (F_USER + 0)
#define GETVAL              (F_USER + 1)
#define GETALL              (F_USER + 2)
#define GETNCNT             (F_USER + 3)
#define GETZCNT             (F_USER + 4)
#define SETVAL              (F_USER + 5)
#define SETALL              (F_USER + 6)

struct semid_ds {
    struct ipc_perm         sem_perm;
};

struct sembuf {
    unsigned short          sem_num;
    short                   sem_op;
    short                   sem_flg;
};

#if VSF_LINUX_CFG_WRAPPER == ENABLED
#define semget              VSF_LINUX_WRAPPER(semget)
#define semctl              VSF_LINUX_WRAPPER(semctl)
#define semop               VSF_LINUX_WRAPPER(semop)
#define semtimedop          VSF_LINUX_WRAPPER(semtimedop)
#endif

#if VSF_LINUX_APPLET_USE_SYS_SEM == ENABLED
typedef struct vsf_linux_sys_sem_vplt_t {
    vsf_vplt_info_t info;
} vsf_linux_sys_sem_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_sys_sem_vplt_t vsf_linux_sys_sem_vplt;
#   endif
#endif

#if defined(__VSF_APPLET__) && defined(__VSF_APPLET_LIB__) && VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_APPLET_USE_SYS_SEM == ENABLED

#ifndef VSF_LINUX_APPLET_SYS_SEM_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_SYS_SEM_VPLT                                    \
            ((vsf_linux_sys_sem_vplt_t *)(VSF_LINUX_APPLET_VPLT->sys_sem_vplt))
#   else
#       define VSF_LINUX_APPLET_SYS_SEM_VPLT                                    \
            ((vsf_linux_sys_sem_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_SYS_SEM_ENTRY(__NAME)                                  \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_SYS_SEM_VPLT, __NAME)
#define VSF_LINUX_APPLET_SYS_SEM_IMP(...)                                       \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_SYS_SEM_VPLT, __VA_ARGS__)

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_SEM

int semctl(int semid, int semnum, int cmd, ...);
int semget(key_t key, int nsems, int semflg);
int semop(int semid, struct sembuf *sops, size_t nsops);
int semtimedop(int semid, struct sembuf *sops, size_t nsops,
                    const struct timespec *timeout);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_SEM

#ifdef __cplusplus
}
#endif

#endif      // __VSF_LINUX_SEM_H__
