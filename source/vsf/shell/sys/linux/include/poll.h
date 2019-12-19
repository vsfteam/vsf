#ifndef __POLL_H__
#define __POLL_H__

#include <signal.h>
#include <time.h>

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

#endif
