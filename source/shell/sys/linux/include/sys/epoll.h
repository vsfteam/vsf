#ifndef __VSF_LINUX_EPOLL_H__
#define __VSF_LINUX_EPOLL_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../sys/types.h"
#   include "../fcntl.h"
#   include "../poll.h"
#else
#   include <sys/types.h>
#   include <fcntl.h>
#   include <poll.h>
#endif
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define EPOLL_CLOEXEC       O_CLOEXEC

#define EPOLL_CTL_ADD       0
#define EPOLL_CTL_MOD       1
#define EPOLL_CTL_DEL       2

#define EPOLLIN             POLLIN
#define EPOLLOUT            POLLOUT
#define EPOLLHUP            POLLHUP
#define EPOLLERR            POLLERR
#define EPOLLPRI            POLLPRI

#define EPOLLET             (1 << 16)
#define EPOLLONESHOT        (1 << 17)
#define EPOLLWAKEUP         (1 << 18)
#define EPOLLEXCLUSIVE      (1 << 19)

typedef union epoll_data {
    void        *ptr;
    int          fd;
    uint32_t     u32;
    uint64_t     u64;
} epoll_data_t;

struct epoll_event {
    uint32_t     events;
    epoll_data_t data;
} PACKED;

#if VSF_LINUX_CFG_WRAPPER == ENABLED
#define epoll_create        VSF_LINUX_WRAPPER(epoll_create)
#define epoll_create1       VSF_LINUX_WRAPPER(epoll_create1)
#define epoll_ctl           VSF_LINUX_WRAPPER(epoll_ctl)
#define epoll_wait          VSF_LINUX_WRAPPER(epoll_wait)
#define epoll_pwait         VSF_LINUX_WRAPPER(epoll_pwait)
#define epoll_pwait2        VSF_LINUX_WRAPPER(epoll_pwait2)
#endif

// syscalls

#define __NR_epoll_create   epoll_create
#define __NR_epoll_create1  epoll_create1
#define __NR_epoll_ctl      epoll_ctl
#define __NR_epoll_wait     epoll_wait
#define __NR_epoll_pwait(__epfd, __events, __maxevents, __timeout, __set, __set_size)\
                            epoll_pwait((__epfd), (__events), (__maxevents), (__timeout), (__set))

#if VSF_LINUX_APPLET_USE_SYS_EPOLL == ENABLED
typedef struct vsf_linux_sys_epoll_vplt_t {
    vsf_vplt_info_t info;

    int (*epoll_create)(int size);
    int (*epoll_create1)(int flags);
    int (*epoll_ctl)(int epfd, int op, int fd, struct epoll_event *event);
    int (*epoll_wait)(int epfd, struct epoll_event *events,
                    int maxevents, int timeout);
    int (*epoll_pwait)(int epfd, struct epoll_event *events,
                    int maxevents, int timeout,
                    const sigset_t *sigmask);
    int (*epoll_pwait2)(int epfd, struct epoll_event *events,
                    int maxevents, const struct timespec *timeout,
                    const sigset_t *sigmask);
} vsf_linux_sys_epoll_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_sys_epoll_vplt_t vsf_linux_sys_epoll_vplt;
#   endif
#endif

#if defined(__VSF_APPLET__) && VSF_LINUX_APPLET_USE_SYS_EPOLL == ENABLED

#ifndef VSF_LINUX_APPLET_SYS_EPOLL_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_SYS_EPOLL_VPLT                                  \
            ((vsf_linux_sys_epoll_vplt_t *)(VSF_LINUX_APPLET_VPLT->sys_epoll_vplt))
#   else
#       define VSF_LINUX_APPLET_SYS_EPOLL_VPLT                                  \
            ((vsf_linux_sys_epoll_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

static inline int epoll_create(int size) {
    return VSF_LINUX_APPLET_SYS_EPOLL_VPLT->epoll_create(size);
}
static inline int epoll_create1(int flags) {
    return VSF_LINUX_APPLET_SYS_EPOLL_VPLT->epoll_create1(flags);
}
static inline int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event) {
    return VSF_LINUX_APPLET_SYS_EPOLL_VPLT->epoll_ctl(epfd, op, fd, event);
}
static inline int epoll_wait(int epfd, struct epoll_event *events,
                        int maxevents, int timeout) {
    return VSF_LINUX_APPLET_SYS_EPOLL_VPLT->epoll_wait(epfd, events, maxevents, timeout);
}
static inline int epoll_pwait(int epfd, struct epoll_event *events,
                    int maxevents, int timeout,
                    const sigset_t *sigmask) {
    return VSF_LINUX_APPLET_SYS_EPOLL_VPLT->epoll_pwait(epfd, events, maxevents, timeout, sigmask);
}
static inline int epoll_pwait2(int epfd, struct epoll_event *events,
                    int maxevents, const struct timespec *timeout,
                    const sigset_t *sigmask) {
    return VSF_LINUX_APPLET_SYS_EPOLL_VPLT->epoll_pwait2(epfd, events, maxevents, timeout, sigmask);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_EPOLL

int epoll_create(int size);
int epoll_create1(int flags);
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
int epoll_wait(int epfd, struct epoll_event *events,
                        int maxevents, int timeout);
int epoll_pwait(int epfd, struct epoll_event *events,
                    int maxevents, int timeout,
                    const sigset_t *sigmask);
int epoll_pwait2(int epfd, struct epoll_event *events,
                    int maxevents, const struct timespec *timeout,
                    const sigset_t *sigmask);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_EPOLL

#ifdef __cplusplus
}
#endif

#endif
