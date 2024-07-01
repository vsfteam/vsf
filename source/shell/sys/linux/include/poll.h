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

// syscalls

#define __NR_poll       poll
#define __NR_ppoll      ppoll

typedef int nfds_t;

#define POLLIN          (1 << 0)
#define POLLPRI         (1 << 1)
#define POLLOUT         (1 << 2)
#define POLLERR         (1 << 3)
#define POLLHUP         (1 << 4)
#define POLLNVAL        (1 << 5)

#define POLLRDNORM      (1 << 6)
#define POLLRDBAND      (1 << 7)
#define POLLWRNORM      (1 << 8)
#define POLLWRBAND      (1 << 9)
#define POLLRDHUP       (1 << 10)

typedef struct vsf_linux_fd_priv_callback_t vsf_linux_fd_priv_callback_t;
struct pollfd {
    int fd;
    short events;
    short revents;

    // private
    short events_triggered;
    vsf_linux_fd_priv_callback_t *callback;
    void *trig;
};

#if VSF_LINUX_APPLET_USE_POLL == ENABLED
typedef struct vsf_linux_poll_vplt_t {
    vsf_vplt_info_t info;

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(poll);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(ppoll);
} vsf_linux_poll_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_poll_vplt_t vsf_linux_poll_vplt;
#   endif
#endif

#if     defined(__VSF_APPLET__) && (defined(__VSF_APPLET_LIB__) || defined(__VSF_APPLET_LINUX_POLL_LIB__))\
    &&  VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_APPLET_USE_POLL == ENABLED

#ifndef VSF_LINUX_APPLET_POLL_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_POLL_VPLT                                       \
            ((vsf_linux_poll_vplt_t *)(VSF_LINUX_APPLET_VPLT->poll_vplt))
#   else
#       define VSF_LINUX_APPLET_POLL_VPLT                                       \
            ((vsf_linux_poll_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_POLL_ENTRY(__NAME)                                     \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_POLL_VPLT, __NAME)
#define VSF_LINUX_APPLET_POLL_IMP(...)                                          \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_POLL_VPLT, __VA_ARGS__)

VSF_LINUX_APPLET_POLL_IMP(poll, int, struct pollfd *fds, nfds_t nfds, int timeout) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_POLL_ENTRY(poll)(fds, nfds, timeout);
}
VSF_LINUX_APPLET_POLL_IMP(ppoll, int, struct pollfd *fds, nfds_t nfds, const struct timespec *timeout_ts, const sigset_t *sigmask) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_POLL_ENTRY(ppoll)(fds, nfds, timeout_ts, sigmask);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_POLL

int poll(struct pollfd *fds, nfds_t nfds, int timeout);
int ppoll(struct pollfd *fds, nfds_t nfds, const struct timespec *timeout_ts, const sigset_t *sigmask);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_POLL

#ifdef __cplusplus
}
#endif

#endif
