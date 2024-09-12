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

enum {
    TIOCGWINSZ      = F_IO,
    TIOCGSERIAL     = F_IO + 1,
    TIOCSSERIAL     = F_IO + 2,
    TIOCSCTTY       = F_IO + 3,
    FIONREAD        = F_IO + 4,
    TIOCSERCONFIG   = F_IO + 5,
    TIOCSWINSZ      = F_IO + 6,
    TIOCCONS        = F_IO + 7,
};

#if VSF_LINUX_APPLET_USE_SYS_IOCTL == ENABLED
typedef struct vsf_linux_sys_ioctl_vplt_t {
    vsf_vplt_info_t info;

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(ioctl);
} vsf_linux_sys_ioctl_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_sys_ioctl_vplt_t vsf_linux_sys_ioctl_vplt;
#   endif
#endif

#if     defined(__VSF_APPLET__) && (defined(__VSF_APPLET_LIB__) || defined(__VSF_APPLET_LINUX_SYS_IOCTL_LIB__))\
    &&  VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_APPLET_USE_SYS_IOCTL == ENABLED

#ifndef VSF_LINUX_APPLET_SYS_IOCTL_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_SYS_IOCTL_VPLT                                  \
            ((vsf_linux_sys_ioctl_vplt_t *)(VSF_LINUX_APPLET_VPLT->sys_ioctl_vplt))
#   else
#       define VSF_LINUX_APPLET_SYS_IOCTL_VPLT                                  \
            ((vsf_linux_sys_ioctl_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_SYS_IOCTL_ENTRY(__NAME)                                \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_SYS_IOCTL_VPLT, __NAME)
#define VSF_LINUX_APPLET_SYS_IOCTL_IMP(...)                                     \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_SYS_IOCTL_VPLT, __VA_ARGS__)

VSF_APPLET_VPLT_FUNC_DECORATOR(ioctl) int ioctl(int fd, unsigned long request, ...) {
    int result;
    va_list ap;
    va_start(ap, request);
        result = __fcntl_va(fd, request, ap);
    va_end(ap);
    return result;
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_SEM

int ioctl(int fd, unsigned long request, ...);

#endif

#ifdef __cplusplus
}
#endif

#include <asm/ioctls.h>

#endif      // __VSF_LINUX_IOCTL_H__
