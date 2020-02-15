#ifndef __UNISTD_H__
#define __UNISTD_H__

#include "vsf.h"
#include "../vsf_linux.h"
#include <linux/limits.h>
#include <sys/types.h>

#define STDIN_FILENO        0
#define STDOUT_FILENO       1
#define STDERR_FILENO       2

enum {
    DT_UNKNOWN,
    DT_REG,
    DT_DIR,
    DT_FIFO,
    DT_SOCK,
    DT_CHR,
    DT_BLK,
    DT_LNK,
    DT_EXE,
};

ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, void *buf, size_t count);

void usleep(int micro_seconds);
unsigned sleep(unsigned seconds);

pid_t getpid(void);
pid_t getppid(void);

int execl(const char *pathname, const char *arg, ...);
int execv(const char *pathname, char const* const* argv);

int creat(const char *pathname, mode_t mode);
int open(const char *pathname, int flags, ...);
int access(const char *pathname, int mode);
int unlink(const char *pathname);
int mkdir(const char* pathname, mode_t mode);

int close(int fd);
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, void *buf, size_t count);

#endif
