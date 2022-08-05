#ifndef __VSF_LINUX_POLL_H__
#define __VSF_LINUX_POLL_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "./signal.h"
#else
#   include <signal.h>
#endif
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED & VSF_LINUX_USE_SIMPLE_TIME == ENABLED
#   include "./simple_libc/time.h"
#else
#   include <time.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_CFG_WRAPPER == ENABLED
#define poll            VSF_LINUX_WRAPPER(poll)
#define ppoll           VSF_LINUX_WRAPPER(ppoll)
#endif

typedef int nfds_t;

#define POLLIN          (1 << 0)
#define POLLOUT         (1 << 1)
#define POLLPRI         (1 << 2)
#define POLLHUP         (1 << 3)
#define POLLERR         (1 << 4)
#define POLLNVAL        (1 << 5)

struct pollfd {
    int fd;
    short events;
    short revents;

    // private
    short events_triggered;
    void *trig;
};

#if VSF_LINUX_APPLET_USE_POLL == ENABLED
typedef struct vsf_linux_poll_vplt_t {
    vsf_vplt_info_t info;
} vsf_linux_poll_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_poll_vplt_t vsf_linux_poll_vplt;
#   endif
#endif

#if defined(__VSF_APPLET__) && VSF_LINUX_APPLET_USE_POLL == ENABLED

#ifndef VSF_LINUX_APPLET_POLL_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_POLL_VPLT                                       \
            ((vsf_linux_poll_vplt_t *)(VSF_LINUX_APPLET_VPLT->poll))
#   else
#       define VSF_LINUX_APPLET_POLL_VPLT                                       \
            ((vsf_linux_poll_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_POLL

int poll(struct pollfd *fds, nfds_t nfds, int timeout);
int ppoll(struct pollfd *fds, nfds_t nfds, const struct timespec *timeout_ts, const sigset_t *sigmask);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_POLL

#ifdef __cplusplus
}
#endif

#endif
