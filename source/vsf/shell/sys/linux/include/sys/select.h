#ifndef __SELECT_H__
#define __SELECT_H__

#include <sys/time.h>

#ifndef __WIN__
__vsf_declare_bitmap_ex(fd_set, 1024)
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *execeptfds, struct timeval *timeout);
#endif

#endif
