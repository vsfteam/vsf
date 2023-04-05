#ifndef __VSF_LINUX_SEMAPHORE_H__
#define __VSF_LINUX_SEMAPHORE_H__

#include "shell/sys/linux/vsf_linux_cfg.h"
#include "kernel/vsf_kernel.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_TIME == ENABLED
#   include "./simple_libc/time.h"
#else
#   include <time.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_CFG_WRAPPER == ENABLED
#define sem_init            VSF_LINUX_WRAPPER(sem_init)
#define sem_destroy         VSF_LINUX_WRAPPER(sem_destroy)
#define sem_wait            VSF_LINUX_WRAPPER(sem_wait)
#define sem_trywait         VSF_LINUX_WRAPPER(sem_trywait)
#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
#   define sem_timedwait    VSF_LINUX_WRAPPER(sem_timedwait)
#endif
#define sem_post            VSF_LINUX_WRAPPER(sem_post)
#define sem_getvalue        VSF_LINUX_WRAPPER(sem_getvalue)
#endif

#define SEM_FAILED          ((sem_t *)NULL)

typedef vsf_sem_t sem_t;

#if VSF_LINUX_APPLET_USE_SEMAPHORE == ENABLED
typedef struct vsf_linux_semaphore_vplt_t {
    vsf_vplt_info_t info;

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(sem_init);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(sem_destroy);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(sem_wait);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(sem_trywait);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(sem_timedwait);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(sem_post);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(sem_getvalue);
} vsf_linux_semaphore_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_semaphore_vplt_t vsf_linux_semaphore_vplt;
#   endif
#endif

#if defined(__VSF_APPLET__) && VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_APPLET_USE_SEMAPHORE == ENABLED

#ifndef VSF_LINUX_APPLET_SEMAPHORE_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_SEMAPHORE_VPLT                                  \
            ((vsf_linux_semaphore_vplt_t *)(VSF_LINUX_APPLET_VPLT->semaphore_vplt))
#   else
#       define VSF_LINUX_APPLET_SEMAPHORE_VPLT                                  \
            ((vsf_linux_semaphore_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_SEMAPHORE_ENTRY(__NAME)                                \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_SEMAPHORE_VPLT, __NAME)
#define VSF_LINUX_APPLET_SEMAPHORE_IMP(...)                                     \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_SEMAPHORE_VPLT, __VA_ARGS__)

VSF_LINUX_APPLET_SEMAPHORE_IMP(sem_init, int, sem_t *sem, int pshared, unsigned int value) {
    return VSF_LINUX_APPLET_SEMAPHORE_ENTRY(sem_init)(sem, pshared, value);
}
VSF_LINUX_APPLET_SEMAPHORE_IMP(sem_destroy, int, sem_t *sem) {
    return VSF_LINUX_APPLET_SEMAPHORE_ENTRY(sem_destroy)(sem);
}
VSF_LINUX_APPLET_SEMAPHORE_IMP(sem_wait, int, sem_t *sem) {
    return VSF_LINUX_APPLET_SEMAPHORE_ENTRY(sem_wait)(sem);
}
VSF_LINUX_APPLET_SEMAPHORE_IMP(sem_trywait, int, sem_t *sem) {
    return VSF_LINUX_APPLET_SEMAPHORE_ENTRY(sem_trywait)(sem);
}
VSF_LINUX_APPLET_SEMAPHORE_IMP(sem_timedwait, int, sem_t *sem, const struct timespec *abs_timeout) {
    return VSF_LINUX_APPLET_SEMAPHORE_ENTRY(sem_timedwait)(sem, abs_timeout);
}
VSF_LINUX_APPLET_SEMAPHORE_IMP(sem_post, int, sem_t *sem) {
    return VSF_LINUX_APPLET_SEMAPHORE_ENTRY(sem_post)(sem);
}
VSF_LINUX_APPLET_SEMAPHORE_IMP(sem_getvalue, int, sem_t *sem, int *value) {
    return VSF_LINUX_APPLET_SEMAPHORE_ENTRY(sem_getvalue)(sem, value);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SEMAPHORE

int sem_init(sem_t *sem, int pshared, unsigned int value);
int sem_destroy(sem_t *sem);
int sem_wait(sem_t *sem);
int sem_trywait(sem_t *sem);
#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout);
#endif
int sem_post(sem_t *sem);
int sem_getvalue(sem_t *sem, int *value);

sem_t * sem_open(const char *name, int oflag, mode_t mode, unsigned int value);
int sem_close(sem_t *sem);
int sem_unlink(const char *name);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SEMAPHORE

#ifdef __cplusplus
}
#endif

#endif
