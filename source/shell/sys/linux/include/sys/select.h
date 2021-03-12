#ifndef __SELECT_H__
#define __SELECT_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../sys/time.h"
#else
#   include <sys/time.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __WIN__
#   define select               __vsf_linux_select
#endif

#ifndef __WIN__
__vsf_declare_bitmap_ex(fd_set, 1024)
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *execeptfds, struct timeval *timeout);
#endif

#ifdef __cplusplus
}
#endif

#endif
