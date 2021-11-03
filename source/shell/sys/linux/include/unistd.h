#ifndef __VSF_LINUX_UNISTD_H__
#define __VSF_LINUX_UNISTD_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "./simple_libc/stddef.h"
#   include "./linux/limits.h"
#   include "./sys/select.h"

// avoid compiler.h in vsf.h include c headers
#   define __USE_LOCAL_LIBC__
#   define __USE_LOCAL_STDIO__
#else
#   include <stddef.h>
#   include <linux/limits.h>
#   include <sys/select.h>
#endif

#include "vsf.h"
#include "../vsf_linux.h"
#include "../port/busybox/busybox.h"
#include "shell/sys/linux/vsf_linux_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_CFG_WRAPPER == ENABLED
#define usleep              VSF_LINUX_WRAPPER(usleep)
#define sleep               VSF_LINUX_WRAPPER(sleep)
#define getpid              VSF_LINUX_WRAPPER(getpid)
#define getppid             VSF_LINUX_WRAPPER(getppid)
#define execl               VSF_LINUX_WRAPPER(execl)
#define execv               VSF_LINUX_WRAPPER(execv)
#define realpath            VSF_LINUX_WRAPPER(realpath)
#define sysconf             VSF_LINUX_WRAPPER(sysconf)
#define pipe                VSF_LINUX_WRAPPER(pipe)
#define alarm               VSF_LINUX_WRAPPER(alarm)

#if __IS_COMPILER_IAR__
#else
#   define creat            VSF_LINUX_WRAPPER(creat)
#   define open             VSF_LINUX_WRAPPER(open)
#   define access           VSF_LINUX_WRAPPER(access)
#   define unlink           VSF_LINUX_WRAPPER(unlink)
#   define link             VSF_LINUX_WRAPPER(link)
#   define remove           VSF_LINUX_WRAPPER(remove)
#   define mkdir            VSF_LINUX_WRAPPER(mkdir)
#   define rmdir            VSF_LINUX_WRAPPER(rmdir)
#   define close            VSF_LINUX_WRAPPER(close)
#   define lseek            VSF_LINUX_WRAPPER(lseek)
#   define read             VSF_LINUX_WRAPPER(read)
#   define write            VSF_LINUX_WRAPPER(write)
#   define chdir            VSF_LINUX_WRAPPER(chdir)
#   define getcwd           VSF_LINUX_WRAPPER(getcwd)
#endif
#endif

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

void usleep(int micro_seconds);
unsigned sleep(unsigned seconds);

unsigned int alarm(unsigned int seconds);

pid_t getpid(void);
pid_t getppid(void);

#if defined(__WIN__) && defined(__CPU_X64__)
intptr_t execl(const char *pathname, const char *arg, ...);
intptr_t execv(const char *pathname, char const* const* argv);
#else
int execl(const char *pathname, const char *arg, ...);
int execv(const char *pathname, char const * const * argv);
#endif

int system(const char *cmd);
enum {
    _SC_PAGESIZE,
};
long sysconf(int name);
char *realpath(const char *path, char *resolved_path);
int pipe(int pipefd[2]);

int creat(const char *pathname, mode_t mode);
int open(const char *pathname, int flags, ...);
#define F_OK            (1 << 0)
#define R_OK            (1 << 1)
#define W_OK            (1 << 2)
#define X_OK            (1 << 3)
int access(const char *pathname, int mode);
int unlink(const char *pathname);
int link(const char *oldpath, const char *newpath);
int remove(const char *pathname);
int mkdir(const char *pathname, mode_t mode);
int rmdir(const char *pathname);

int chdir(const char *pathname);
char * getcwd(char *buffer, size_t maxlen);

int close(int fd);
off_t lseek(int fd, off_t offset, int whence);
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);

#ifdef __cplusplus
}
#endif

#endif
