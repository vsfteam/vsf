#ifndef __VSF_LINUX_FCNTL_H__
#define __VSF_LINUX_FCNTL_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "./sys/types.h"
#else
#   include <sys/types.h>
#endif
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_CFG_WRAPPER == ENABLED
#define fcntl           VSF_LINUX_WRAPPER(fcntl)
#endif

// syscalls

#define __NR_fcntl      fcntl

#define O_RDONLY        0x0000
#define O_WRONLY        0x0001
#define O_RDWR          0x0002
#define O_ACCMODE       0x0003

#define O_NONBLOCK      0x0004
#define O_APPEND        0x0008
#define O_DIRECTORY     0x0100
#define O_CREAT         0x0200
#define O_TRUNC         0x0400
#define O_EXCL          0x0800
#define O_CLOEXEC       0x1000
#define O_SYNC          0x2000
#define O_ASYNC         0x4000
#define O_LARGEFILE     0

#define O_NOCTTY        0x2000
#define O_NDELAY        O_NONBLOCK

#define F_DUPFD         0
#define F_GETFD         1
#define F_SETFD         2
#define F_GETFL         3
#define F_SETFL         4
#define F_RDLCK         5
#define F_WRLCK         6
#define F_UNLCK         7
#define F_GETLK         8
#define F_SETLK         9
#define F_SETLKW        10
#define F_GETOWN        11
#define F_SETOWN        12
#define F_GETSIG        13
#define F_SETSIG        14
#define F_USER          16
// for ioctrl
#define F_IO            16

#define FD_CLOEXEC      1
// __FD_READALL is used internally to indicate to read all data
#define __FD_READALL    2

struct flock {
    off_t   l_start;
    off_t   l_len;
    pid_t   l_pid;
    short   l_type;
    short   l_whence;
};

#if VSF_LINUX_APPLET_USE_FCNTL == ENABLED
typedef struct vsf_linux_fcntl_vplt_t {
    vsf_vplt_info_t info;

    int (*__fcntl_va)(int fd, int cmd, va_list ap);
    int (*fcntl)(int fd, int cmd, ...);
} vsf_linux_fcntl_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_fcntl_vplt_t vsf_linux_fcntl_vplt;
#   endif
#endif

#if defined(__VSF_APPLET__) && VSF_LINUX_APPLET_USE_FCNTL == ENABLED

#ifndef VSF_LINUX_APPLET_FCNTL_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_FCNTL_VPLT                                      \
            ((vsf_linux_fcntl_vplt_t *)(VSF_LINUX_APPLET_VPLT->fcntl))
#   else
#       define VSF_LINUX_APPLET_FCNTL_VPLT                                      \
            ((vsf_linux_fcntl_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

static inline int fcntl(int fd, int cmd, ...) {
    int ret;

    va_list ap;
    va_start(ap, cmd);
        ret = VSF_LINUX_APPLET_FCNTL_VPLT->__fcntl_va(fd, cmd, ap);
    va_end(ap);
    return ret;
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_FCNTL

int fcntl(int fd, int cmd, ...);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_FCNTL

#ifdef __cplusplus
}
#endif

#endif
