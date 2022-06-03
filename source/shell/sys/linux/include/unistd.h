#ifndef __VSF_LINUX_UNISTD_H__
#define __VSF_LINUX_UNISTD_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "./simple_libc/stddef.h"
#   include "./linux/limits.h"
#   include "./sys/types.h"
#   include "./sys/select.h"
#   include "./pwd.h"

// avoid compiler.h in vsf.h include c headers
#   define __USE_LOCAL_LIBC__
#   define __USE_LOCAL_STDIO__
#else
#   include <stddef.h>
#   include <linux/limits.h>
#   include <sys/types.h>
#   include <sys/select.h>
#   include <pwd.h>
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
#define execlp              VSF_LINUX_WRAPPER(execlp)
#define execv               VSF_LINUX_WRAPPER(execv)
#define execve              VSF_LINUX_WRAPPER(execve)
#define execvp              VSF_LINUX_WRAPPER(execvp)
#define execvpe             VSF_LINUX_WRAPPER(execvpe)
#define realpath            VSF_LINUX_WRAPPER(realpath)
#define sysconf             VSF_LINUX_WRAPPER(sysconf)
#define pipe                VSF_LINUX_WRAPPER(pipe)
#define alarm               VSF_LINUX_WRAPPER(alarm)
#define ualarm              VSF_LINUX_WRAPPER(ualarm)

#define isatty              VSF_LINUX_WRAPPER(isatty)
#define symlink             VSF_LINUX_WRAPPER(symlink)
#define getpagesize         VSF_LINUX_WRAPPER(getpagesize)
#define ftruncate           VSF_LINUX_WRAPPER(ftruncate)
#define readlink            VSF_LINUX_WRAPPER(readlink)
#define tcgetpgrp           VSF_LINUX_WRAPPER(tcgetpgrp)
#define tcsetpgrp           VSF_LINUX_WRAPPER(tcsetpgrp)
#define getpass             VSF_LINUX_WRAPPER(getpass)
#define gethostname         VSF_LINUX_WRAPPER(gethostname)
#define sethostname         VSF_LINUX_WRAPPER(sethostname)

#define getentropy          VSF_LINUX_WRAPPER(getentropy)

#if __IS_COMPILER_IAR__
#else
#   define creat            VSF_LINUX_WRAPPER(creat)
#   define open             VSF_LINUX_WRAPPER(open)
#   define openat           VSF_LINUX_WRAPPER(openat)
#   define access           VSF_LINUX_WRAPPER(access)
#   define unlink           VSF_LINUX_WRAPPER(unlink)
#   define unlinkat         VSF_LINUX_WRAPPER(unlinkat)
#   define link             VSF_LINUX_WRAPPER(link)
#   define remove           VSF_LINUX_WRAPPER(remove)
#   define mkdir            VSF_LINUX_WRAPPER(mkdir)
#   define mkdirat          VSF_LINUX_WRAPPER(mkdirat)
#   define rmdir            VSF_LINUX_WRAPPER(rmdir)
#   define close            VSF_LINUX_WRAPPER(close)
#   define lseek            VSF_LINUX_WRAPPER(lseek)
#   define read             VSF_LINUX_WRAPPER(read)
#   define write            VSF_LINUX_WRAPPER(write)
#   define readv            VSF_LINUX_WRAPPER(readv)
#   define writev           VSF_LINUX_WRAPPER(writev)
#   define pread            VSF_LINUX_WRAPPER(pread)
#   define pwrite           VSF_LINUX_WRAPPER(pwrite)
#   define preadv           VSF_LINUX_WRAPPER(preadv)
#   define pwritev          VSF_LINUX_WRAPPER(pwritev)
#   define chdir            VSF_LINUX_WRAPPER(chdir)
#   define getcwd           VSF_LINUX_WRAPPER(getcwd)
#   define fsync            VSF_LINUX_WRAPPER(fsync)
#   define fdatasync        VSF_LINUX_WRAPPER(fdatasync)
#endif
#endif

#define STDIN_FILENO        0
#define STDOUT_FILENO       1
#define STDERR_FILENO       2

#define environ             (vsf_linux_get_cur_process()->__environ)

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

int usleep(int micro_seconds);
unsigned sleep(unsigned seconds);

unsigned int alarm(unsigned int seconds);
useconds_t ualarm(useconds_t usecs, useconds_t interval);

#define setgid(__uid)               (0)
#define getgid()                    ((gid_t)0)
#define getegid()                   ((gid_t)0)
#define initgroups(__user, __gid)   (0)
#define getpgid(__pid)              (__pid)
#define getpgrp()                   ((pid_t)0)
#define setpgrp()                   (0)
#define getuid()                    ((uid_t)0)
#define setuid(__uid)               (0)
#define geteuid()                   ((uid_t)0)
pid_t getpid(void);
pid_t getppid(void);


#if defined(__WIN__) && defined(__CPU_X64__)
#   define exec_ret_t           intptr_t
#else
#   define exec_ret_t           int
#endif
exec_ret_t execl(const char *pathname, const char *arg, ...);
exec_ret_t execlp(const char *file, const char *arg, ...);
exec_ret_t execv(const char *pathname, char * const * argv);
exec_ret_t execve(const char *pathname, char * const * argv, char * const * envp);
exec_ret_t execvp(const char *file, char * const * argv);
exec_ret_t execvpe(const char *file, char * const * argv, char * const * envp);
int daemon(int nochdir, int noclose);

pid_t fork(void);
int system(const char *cmd);
enum {
    _SC_PAGESIZE,
    _SC_OPEN_MAX,
    _SC_CLK_TCK,
};
long sysconf(int name);
char *realpath(const char *path, char *resolved_path);
int pipe(int pipefd[2]);

int creat(const char *pathname, mode_t mode);
int open(const char *pathname, int flags, ...);
int openat(int dirfd, const char *pathname, int flags, ...);
#define F_OK            (1 << 0)
#define R_OK            (1 << 1)
#define W_OK            (1 << 2)
#define X_OK            (1 << 3)
int access(const char *pathname, int mode);
int unlink(const char *pathname);
int unlinkat(int dirfd, const char *pathname, int flags);
int link(const char *oldpath, const char *newpath);
int remove(const char *pathname);
int mkdir(const char *pathname, mode_t mode);
int mkdirat(int dirfd, const char *pathname, mode_t mode);
int mkdirs(const char *pathname, mode_t mode);
int rmdir(const char *pathname);
int dup(int oldfd);
int dup2(int oldfd, int newfd);

int chroot(const char *path);
int chdir(const char *pathname);
char * getcwd(char *buffer, size_t maxlen);

int close(int fd);
off_t lseek(int fd, off_t offset, int whence);
// workaround while using lwip, which will check iovec MACRO and define iovec if not defined
#define iovec iovec
struct iovec {
    void  *iov_base;
    size_t iov_len;
};
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);
ssize_t readv(int fd, const struct iovec *iov, int iovcnt);
ssize_t writev(int fd, const struct iovec *iov, int iovcnt);
ssize_t pread(int fd, void *buf, size_t count, off_t offset);
ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset);
ssize_t preadv(int fd, const struct iovec *iov, int iovcnt, off_t offset);
ssize_t pwritev(int fd, const struct iovec *iov, int iovcnt, off_t offset);
int fsync(int fd);
int fdatasync(int fd);

int isatty(int fd);
size_t getpagesize(void);
int symlink(const char *target, const char *linkpath);
int ftruncate(int fildes, off_t length);
ssize_t readlink(const char *pathname, char *buf, size_t bufsiz);
pid_t tcgetpgrp(int fd);
int tcsetpgrp(int fd, pid_t pgrp);
char * getpass(const char *prompt);
int gethostname(char *name, size_t len);
int sethostname(const char *name, size_t len);

int chown(const char *pathname, uid_t owner, gid_t group);
int fchown(int fd, uid_t owner, gid_t group);
int lchown(const char *pathname, uid_t owner, gid_t group);
int fchownat(int dirfd, const char *pathname, uid_t owner, gid_t group, int flags);

int getentropy(void *buffer, size_t length);

#ifdef __cplusplus
}
#endif

#endif
