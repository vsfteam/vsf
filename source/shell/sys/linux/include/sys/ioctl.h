#ifndef __VSF_LINUX_SYS_IOCTL_H__
#define __VSF_LINUX_SYS_IOCTL_H__

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

#define _IOC_NRSHIFT            0
#define _IOC_NRBITS             8
#define _IOC_NRMASK             ((1 << _IOC_NRBITS) - 1)

#define _IOC_TYPESHIFT          (_IOC_NRSHIFT + _IOC_NRBITS)
#define _IOC_TYPEBITS           8
#define _IOC_TYPEMASK           ((1 << _IOC_TYPEBITS) - 1)

#define _IOC_SIZESHIFT          (_IOC_TYPESHIFT + _IOC_TYPEBITS)
#define _IOC_SIZEBITS           14
#define _IOC_SIZEMASK           ((1 << _IOC_SIZEBITS) - 1)

#define _IOC_DIRSHIFT           (_IOC_SIZESHIFT + _IOC_SIZEBITS)
#define _IOC_DIRBITS            2
#define _IOC_DIRMASK            ((1 << _IOC_DIRBITS) - 1)

#define _IOC_NONE               0
#define _IOC_WRITE              1
#define _IOC_READ               2

#define _IOC(dir, type, nr, size)                                               \
                                (   ((dir) << _IOC_DIRSHIFT)                    \
                                |   ((type) << _IOC_TYPESHIFT)                  \
                                |   ((nr) << _IOC_NRSHIFT)                      \
                                |   ((size) << _IOC_SIZESHIFT)                  \
                                )
#define _IO(type, nr)           _IOC(_IOC_NONE, (type), (nr), 0)
#define _IOR(type, nr, size)    _IOC(_IOC_READ, (type), (nr), sizeof(size))
#define _IOW(type, nr, size)    _IOC(_IOC_WRITE, (type), (nr), sizeof(size))
#define _IOWR(type, nr, size)   _IOC(_IOC_READ | _IOC_WRITE, (type), (nr), sizeof(size))

#define _IOC_DIR(nr)            (((nr) >> _IOC_DIRSHIFT) & _IOC_DIRMASK)
#define _IOC_TYPE(nr)           (((nr) >> _IOC_TYPESHIFT) & _IOC_TYPEMASK)
#define _IOC_NR(nr)             (((nr) >> _IOC_NRSHIFT) & _IOC_NRMASK)
#define _IOC_SIZE(nr)           (((nr) >> _IOC_SIZESHIFT) & _IOC_SIZEMASK)

struct winsize {
    unsigned short ws_row;
    unsigned short ws_col;
    unsigned short ws_xpixel;
    unsigned short ws_ypixel;
};

enum {
    TIOCGWINSZ      = F_IO,
    TIOCGSERIAL     = F_IO + 1,
    TIOCSSERIAL     = F_IO + 2,
    TIOCSCTTY       = F_IO + 3,
    FIONREAD        = F_IO + 4,
};

//int ioctl(int fd, unsigned long request, ...);
#define ioctl       fcntl

#ifdef __cplusplus
}
#endif

#include <asm/ioctls.h>

#endif      // __VSF_LINUX_IOCTL_H__
