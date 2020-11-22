#ifndef __POLL_H__
#define __POLL_H__

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

#define poll            __vsf_linux_poll
#define ppoll           __vsf_linux_ppoll

typedef int nfds_t;

#define POLLIN          (1 << 0)
#define POLLOUT         (1 << 1)

struct pollfd {
    int fd;
    short events;
    short revents;
};

int poll(struct pollfd *fds, nfds_t nfds, int timeout);
int ppoll(struct pollfd *fds, nfds_t nfds, const struct timespec *timeout_ts, const sigset_t *sigmask);

#ifdef __cplusplus
}
#endif

#endif
