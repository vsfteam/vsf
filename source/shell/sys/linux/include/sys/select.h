#ifndef __VSF_LINUX_SELECT_H__
#define __VSF_LINUX_SELECT_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../sys/time.h"
#   include "../signal.h"
#else
#   include <sys/time.h>
#   include <signal.h>
#endif
// for bitmap
#include "utilities/vsf_utilities.h"

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_CFG_WRAPPER == ENABLED
#   define select               VSF_LINUX_WRAPPER(select)
#elif defined(__WIN__)
#   define select               VSF_LINUX_WRAPPER(select)
#   define pselect              VSF_LINUX_WRAPPER(pselect)
#endif

// syscalls

#define __NR_select             select

#ifndef FD_SETSIZE
#   define FD_SETSIZE           1024
#endif
#define FD_ZERO(set)            vsf_bitmap_reset(*(set), FD_SETSIZE)
#define FD_SET(fd, set)         vsf_bitmap_set(*(set), (fd))
#define FD_CLR(fd, set)         vsf_bitmap_clear(*(set), (fd))
#define FD_ISSET(fd, set)       vsf_bitmap_get(*(set), (fd))

__vsf_declare_bitmap_ex(fd_set, FD_SETSIZE)

#if VSF_LINUX_APPLET_USE_SYS_SELECT == ENABLED
typedef struct vsf_linux_sys_select_vplt_t {
    vsf_vplt_info_t info;

    int (*select)(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
        struct timeval *timeout);
    int (*pselect)(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
        const struct timespec *timeout_ts, const sigset_t *sigmask);
} vsf_linux_sys_select_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_sys_select_vplt_t vsf_linux_sys_select_vplt;
#   endif
#endif

#if defined(__VSF_APPLET__) && VSF_LINUX_APPLET_USE_SYS_SELECT == ENABLED

#ifndef VSF_LINUX_APPLET_SYS_SELECT_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_SYS_SELECT_VPLT                                 \
            ((vsf_linux_sys_select_vplt_t *)(VSF_LINUX_APPLET_VPLT->sys_select_vplt))
#   else
#       define VSF_LINUX_APPLET_SYS_SELECT_VPLT                                 \
            ((vsf_linux_sys_select_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

static inline int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
        struct timeval *timeout) {
    return VSF_LINUX_APPLET_SYS_SELECT_VPLT->select(nfds, readfds, writefds, exceptfds, timeout);
}
static inline int pselect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
        const struct timespec *timeout_ts, const sigset_t *sigmask) {
    return VSF_LINUX_APPLET_SYS_SELECT_VPLT->pselect(nfds, readfds, writefds, exceptfds, timeout_ts, sigmask);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_SELECT

int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
        struct timeval *timeout);
int pselect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
        const struct timespec *timeout_ts, const sigset_t *sigmask);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_SELECT

#ifdef __cplusplus
}
#endif

#endif
