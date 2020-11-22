#ifndef __FCNTL_H__
#define __FCNTL_H__

#ifdef __cplusplus
extern "C" {
#endif

#define fcntl           __vsf_linux_fcntl

#define O_RDONLY        0x0000
#define O_WRONLY        0x0001
#define O_RDWR          0x0002
#define O_ACCMODE       0x0003

#define O_NONBLOCK      0x0004
#define O_APPEND        0x0008
#define O_CREAT         0x0200
#define O_TRUNC         0x0400
#define O_EXCL          0x0800

int fcntl(int fd, int cmd, ...);

#ifdef __cplusplus
}
#endif

#endif
