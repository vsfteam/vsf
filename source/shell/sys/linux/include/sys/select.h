#ifndef __VSF_LINUX_SELECT_H__
#define __VSF_LINUX_SELECT_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../sys/time.h"
#else
#   include <sys/time.h>
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
#endif

#ifndef FD_SETSIZE
#   define FD_SETSIZE           1024
#endif
#define FD_ZERO(set)            vsf_bitmap_reset(*(set), FD_SETSIZE)
#define FD_SET(fd, set)         vsf_bitmap_set(*(set), (fd))
#define FD_CLR(fd, set)         vsf_bitmap_clear(*(set), (fd))
#define FD_ISSET(fd, set)       vsf_bitmap_get(*(set), (fd))

__vsf_declare_bitmap_ex(fd_set, FD_SETSIZE)
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *execeptfds, struct timeval *timeout);

#ifdef __cplusplus
}
#endif

#endif
