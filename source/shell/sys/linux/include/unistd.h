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

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_CFG_FAKE_API == ENABLED
#define usleep              __vsf_linux_usleep
#define sleep               __vsf_linux_sleep
#define getpid              __vsf_linux_getpid
#define getppid             __vsf_linux_getppid
#define execl               __vsf_linux_execl
#define execv               __vsf_linux_execv
#define system              __vsf_linux_system
#define realpath            __vsf_linux_realpath
#define sysconf             __vsf_linux_sysconf
#define pipe                __vsf_linux_pipe
#define alarm               __vsf_linux_alarm

#if __IS_COMPILER_IAR__
#else
#   define creat            __vsf_linux_creat
#   define open             __vsf_linux_open
#   define access           __vsf_linux_access
#   define unlink           __vsf_linux_unlink
#   define remove           __vsf_linux_remove
#   define mkdir            __vsf_linux_mkdir
#   define close            __vsf_linux_close
#   define lseek            __vsf_linux_lseek
#   define read             __vsf_linux_read
#   define write            __vsf_linux_write
#   define chdir            __vsf_linux_chdir
#   define getcwd           __vsf_linux_getcwd
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
int remove(const char *pathname);
int mkdir(const char *pathname, mode_t mode);

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
