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

enum {
    _SC_PAGESIZE,
    _SC_OPEN_MAX,
    _SC_CLK_TCK,
};

// workaround while using lwip, which will check iovec MACRO and define iovec if not defined
#define iovec iovec
struct iovec {
    void  *iov_base;
    size_t iov_len;
};

#define F_OK                (1 << 0)
#define R_OK                (1 << 1)
#define W_OK                (1 << 2)
#define X_OK                (1 << 3)

#if VSF_LINUX_APPLET_USE_UNISTD == ENABLED
typedef struct vsf_linux_unistd_vplt_t {
    vsf_vplt_info_t info;

    int * (*__vsf_linux_errno)(void);

    int (*usleep)(int micro_seconds);
    unsigned (*sleep)(unsigned seconds);
    unsigned int (*alarm)(unsigned int seconds);
    useconds_t (*ualarm)(useconds_t usecs, useconds_t interval);

    pid_t (*getpid)(void);
    pid_t (*getppid)(void);

    int (*__execl_va)(const char *pathname, const char *arg, va_list ap);
    int (*execl)(const char *pathname, const char *arg, ...);
    int (*__execlp_va)(const char *pathname, const char *arg, va_list ap);
    int (*execlp)(const char *file, const char *arg, ...);
    int (*execv)(const char *pathname, char * const * argv);
    int (*execve)(const char *pathname, char * const * argv, char * const * envp);
    int (*execvp)(const char *file, char * const * argv);
    int (*execvpe)(const char *file, char * const * argv, char * const * envp);
    int (*daemon)(int nochdir, int noclose);

    long (*sysconf)(int name);
    char * (*realpath)(const char *path, char *resolved_path);
    int (*pipe)(int pipefd[2]);

    int (*creat)(const char *pathname, mode_t mode);
    int (*__open_va)(const char *pathname, int flags, va_list ap);
    int (*open)(const char *pathname, int flags, ...);
    int (*__openat_va)(int dirfd, const char *pathname, int flags, va_list ap);
    int (*openat)(int dirfd, const char *pathname, int flags, ...);
    int (*access)(const char *pathname, int mode);
    int (*unlink)(const char *pathname);
    int (*unlinkat)(int dirfd, const char *pathname, int flags);
    int (*link)(const char *oldpath, const char *newpath);
    int (*mkdir)(const char *pathname, mode_t mode);
    int (*mkdirat)(int dirfd, const char *pathname, mode_t mode);
    int (*rmdir)(const char *pathname);
    int (*dup)(int oldfd);
    int (*dup2)(int oldfd, int newfd);

    int (*chroot)(const char *path);
    int (*chdir)(const char *pathname);
    char * (*getcwd)(char *buffer, size_t maxlen);

    int (*close)(int fd);
    off_t (*lseek)(int fd, off_t offset, int whence);
    ssize_t (*read)(int fd, void *buf, size_t count);
    ssize_t (*write)(int fd, const void *buf, size_t count);
    ssize_t (*readv)(int fd, const struct iovec *iov, int iovcnt);
    ssize_t (*writev)(int fd, const struct iovec *iov, int iovcnt);
    ssize_t (*pread)(int fd, void *buf, size_t count, off_t offset);
    ssize_t (*pwrite)(int fd, const void *buf, size_t count, off_t offset);
    ssize_t (*preadv)(int fd, const struct iovec *iov, int iovcnt, off_t offset);
    ssize_t (*pwritev)(int fd, const struct iovec *iov, int iovcnt, off_t offset);
    int (*fsync)(int fd);
    int (*fdatasync)(int fd);

    int (*isatty)(int fd);
    size_t (*getpagesize)(void);
    int (*symlink)(const char *target, const char *linkpath);
    int (*ftruncate)(int fd, off_t length);
    ssize_t (*readlink)(const char *pathname, char *buf, size_t bufsiz);
    pid_t (*tcgetpgrp)(int fd);
    int (*tcsetpgrp)(int fd, pid_t pgrp);
    char * (*getpass)(const char *prompt);
    int (*gethostname)(char *name, size_t len);
    int (*sethostname)(const char *name, size_t len);

    int (*chown)(const char *pathname, uid_t owner, gid_t group);
    int (*fchown)(int fd, uid_t owner, gid_t group);
    int (*lchown)(const char *pathname, uid_t owner, gid_t group);
    int (*fchownat)(int dirfd, const char *pathname, uid_t owner, gid_t group, int flags);

    int (*getentropy)(void *buffer, size_t length);
} vsf_linux_unistd_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_unistd_vplt_t vsf_linux_unistd_vplt;
#   endif
#endif

#if defined(__VSF_APPLET__) && VSF_LINUX_APPLET_USE_UNISTD == ENABLED

#ifndef VSF_LINUX_APPLET_UNISTD_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_UNISTD_VPLT                                     \
            ((vsf_linux_unistd_vplt_t *)(VSF_LINUX_APPLET_VPLT->unistd))
#   else
#       define VSF_LINUX_APPLET_UNISTD_VPLT                                     \
            ((vsf_linux_unistd_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

static inline int * __vsf_linux_errno(void) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->__vsf_linux_errno();
}

static inline int usleep(int micro_seconds) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->usleep(micro_seconds);
}
static inline unsigned sleep(unsigned seconds) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->sleep(seconds);
}
static inline unsigned int alarm(unsigned int seconds) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->alarm(seconds);
}
static inline useconds_t ualarm(useconds_t usecs, useconds_t interval) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->ualarm(usecs, interval);
}

static inline pid_t getpid(void) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->getpid();
}
static inline pid_t getppid(void) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->getppid();
}

static inline int execl(const char *pathname, const char *arg, ...) {
    int ret;

    va_list ap;
    va_start(ap, arg);
        ret = VSF_LINUX_APPLET_UNISTD_VPLT->__execl_va(pathname, arg, ap);
    va_end(ap);
    return ret;
}
static inline int execlp(const char *file, const char *arg, ...) {
    int ret;

    va_list ap;
    va_start(ap, arg);
        ret = VSF_LINUX_APPLET_UNISTD_VPLT->__execlp_va(file, arg, ap);
    va_end(ap);
    return ret;
}
static inline int execv(const char *pathname, char * const * argv) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->execv(pathname, argv);
}
static inline int execve(const char *pathname, char * const * argv, char * const * envp) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->execve(pathname, argv, envp);
}
static inline int execvp(const char *file, char * const * argv) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->execvp(file, argv);
}
static inline int execvpe(const char *file, char * const * argv, char * const * envp) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->execvpe(file, argv, envp);
}
static inline int daemon(int nochdir, int noclose) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->daemon(nochdir, noclose);
}

static inline long sysconf(int name) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->sysconf(name);
}
static inline char *realpath(const char *path, char *resolved_path) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->realpath(path, resolved_path);
}
static inline int pipe(int pipefd[2]) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->pipe(pipefd);
}

static inline int creat(const char *pathname, mode_t mode) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->creat(pathname, mode);
}
static inline int open(const char *pathname, int flags, ...) {
    int ret;

    va_list ap;
    va_start(ap, flags);
        ret = VSF_LINUX_APPLET_UNISTD_VPLT->__open_va(pathname, flags, ap);
    va_end(ap);
    return ret;
}
static inline int openat(int dirfd, const char *pathname, int flags, ...) {
    int ret;

    va_list ap;
    va_start(ap, flags);
        ret = VSF_LINUX_APPLET_UNISTD_VPLT->__openat_va(dirfd, pathname, flags, ap);
    va_end(ap);
    return ret;
}
static inline int access(const char *pathname, int mode) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->access(pathname, mode);
}
static inline int unlink(const char *pathname) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->unlink(pathname);
}
static inline int unlinkat(int dirfd, const char *pathname, int flags) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->unlinkat(dirfd, pathname, flags);
}
static inline int link(const char *oldpath, const char *newpath) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->link(oldpath, newpath);
}
static inline int mkdir(const char *pathname, mode_t mode) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->mkdir(pathname, mode);
}
static inline int mkdirat(int dirfd, const char *pathname, mode_t mode) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->mkdirat(dirfd, pathname, mode);
}
static inline int rmdir(const char *pathname) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->rmdir(pathname);
}
static inline int dup(int oldfd) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->dup(oldfd);
}
static inline int dup2(int oldfd, int newfd) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->dup2(oldfd, newfd);
}

static inline int chroot(const char *path) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->chroot(path);
}
static inline int chdir(const char *pathname) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->chdir(pathname);
}
static inline char * getcwd(char *getcwd, size_t maxlen) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->getcwd(getcwd, maxlen);
}

static inline int close(int fd) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->close(fd);
}
static inline off_t lseek(int fd, off_t offset, int whence) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->lseek(fd, offset, whence);
}
static inline ssize_t read(int fd, void *buf, size_t count) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->read(fd, buf, count);
}
static inline ssize_t write(int fd, const void *buf, size_t count) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->write(fd, buf, count);
}
static inline ssize_t readv(int fd, const struct iovec *iov, int iovcnt) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->readv(fd, iov, iovcnt);
}
static inline ssize_t writev(int fd, const struct iovec *iov, int iovcnt) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->writev(fd, iov, iovcnt);
}
static inline ssize_t pread(int fd, void *buf, size_t count, off_t offset) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->pread(fd, buf, count, offset);
}
static inline ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->pwrite(fd, buf, count, offset);
}
static inline ssize_t preadv(int fd, const struct iovec *iov, int iovcnt, off_t offset) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->preadv(fd, iov, iovcnt, offset);
}
static inline ssize_t pwritev(int fd, const struct iovec *iov, int iovcnt, off_t offset) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->pwritev(fd, iov, iovcnt, offset);
}
static inline int fsync(int fd) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->fsync(fd);
}
static inline int fdatasync(int fd) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->fdatasync(fd);
}

static inline int isatty(int fd) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->isatty(fd);
}
static inline size_t getpagesize(void) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->getpagesize();
}
static inline int symlink(const char *target, const char *linkpath) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->symlink(target, linkpath);
}
static inline int ftruncate(int fd, off_t length) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->ftruncate(fd, length);
}
static inline ssize_t readlink(const char *pathname, char *buf, size_t bufsiz) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->readlink(pathname, buf, bufsiz);
}
static inline pid_t tcgetpgrp(int fd) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->tcgetpgrp(fd);
}
static inline int tcsetpgrp(int fd, pid_t pgrp) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->tcsetpgrp(fd, pgrp);
}
static inline char * getpass(const char *prompt) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->getpass(prompt);
}
static inline int gethostname(char *name, size_t len) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->gethostname(name, len);
}
static inline int sethostname(const char *name, size_t len) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->sethostname(name, len);
}

static inline int chown(const char *pathname, uid_t owner, gid_t group) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->chown(pathname, owner, group);
}
static inline int fchown(int fd, uid_t owner, gid_t group) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->fchown(fd, owner, group);
}
static inline int lchown(const char *pathname, uid_t owner, gid_t group) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->lchown(pathname, owner, group);
}
static inline int fchownat(int dirfd, const char *pathname, uid_t owner, gid_t group, int flags) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->fchownat(dirfd, pathname, owner, group, flags);
}

static inline int getentropy(void *buffer, size_t length) {
    return VSF_LINUX_APPLET_UNISTD_VPLT->getentropy(buffer, length);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_UNISTD

int usleep(int micro_seconds);
unsigned sleep(unsigned seconds);

unsigned int alarm(unsigned int seconds);
useconds_t ualarm(useconds_t usecs, useconds_t interval);

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
long sysconf(int name);
char *realpath(const char *path, char *resolved_path);
int pipe(int pipefd[2]);

int creat(const char *pathname, mode_t mode);
int __open_va(const char *pathname, int flags, va_list ap);
int open(const char *pathname, int flags, ...);
int __openat_va(int dirfd, const char *pathname, int flags, va_list ap);
int openat(int dirfd, const char *pathname, int flags, ...);
int access(const char *pathname, int mode);
int unlink(const char *pathname);
int unlinkat(int dirfd, const char *pathname, int flags);
int link(const char *oldpath, const char *newpath);
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
int ftruncate(int fd, off_t length);
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

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_UNISTD

#ifdef __cplusplus
}
#endif

#endif
