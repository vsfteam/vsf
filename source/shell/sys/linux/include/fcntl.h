#ifndef __VSF_LINUX_FCNTL_H__
#define __VSF_LINUX_FCNTL_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "./sys/types.h"
#else
#   include <sys/types.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_CFG_WRAPPER == ENABLED
#define fcntl           VSF_LINUX_WRAPPER(fcntl)
#endif

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
// for ioctrl
#define F_IO            16

#define FD_CLOEXEC      1

struct flock {
    off_t   l_start;
    off_t   l_len;
    pid_t   l_pid;
    short   l_type;
    short   l_whence;
};

int fcntl(int fd, int cmd, ...);

#ifdef __cplusplus
}
#endif

#endif
