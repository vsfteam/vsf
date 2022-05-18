#ifndef __VSF_LINUX_IOCTL_H__
#define __VSF_LINUX_IOCTL_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../sys/types.h"
#   include "../fcntl.h"
#else
#   include <sys/types.h>
#   include <fcntl.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct winsize {
    unsigned short ws_row;
    unsigned short ws_col;
    unsigned short ws_xpixel;
    unsigned short ws_ypixel;
};

enum {
    TIOCGWINSZ = F_IO,
};

//int ioctl(int fd, unsigned long request, ...);
#define ioctl       fcntl

#ifdef __cplusplus
}
#endif

#endif      // __VSF_LINUX_IOCTL_H__
