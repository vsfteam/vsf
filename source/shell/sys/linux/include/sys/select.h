#ifndef __VSF_LINUX_SELECT_H__
#define __VSF_LINUX_SELECT_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../sys/time.h"
#else
#   include <sys/time.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_CFG_FAKE_API == ENABLED
#   define select               __vsf_linux_select
#endif

#define FD_ZERO(set)            vsf_bitmap_reset(*(set), 1024)
#define FD_SET(fd, set)         vsf_bitmap_set(*(set), (fd))
#define FD_CLR(fd, set)         vsf_bitmap_clear(*(set), (fd))
#define FD_ISSET(fd, set)       vsf_bitmap_get(*(set), (fd))

__vsf_declare_bitmap_ex(fd_set, 1024)
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *execeptfds, struct timeval *timeout);

#ifdef __cplusplus
}
#endif

#endif
