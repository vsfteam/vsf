#ifndef __VSF_LINUX_UNISTD_H__
#define __VSF_LINUX_UNISTD_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "./simple_libc/stddef.h"
//#   include "./linux/limits.h"
#   include "./sys/types.h"
#   include "./sys/select.h"
#   include "./linux/bitops.h"
#   include "./pwd.h"

// avoid compiler.h in vsf.h include c headers
#   define __USE_LOCAL_LIBC__
#   define __USE_LOCAL_STDIO__
#else
#   include <stddef.h>
// avoid to include <linux/xxx.h>, because it will introduce kernel structures,
//  which will maybe conflict with user structures with the same name.
//  include <linux/limits.h> in source file for PATH_MAX
//#   include <linux/limits.h>
#   include <sys/types.h>
#   include <sys/select.h>
#   include <linux/bitops.h>
#   include <pwd.h>
#endif

#define __SIMPLE_LIBC_SETJMP_VPLT_ONLY__
#include <setjmp/setjmp.h>

// avoid to include vsf.h, include necessary headers only
//#include "vsf.h"

#include "../vsf_linux.h"
#include "../port/busybox/busybox.h"
#include "shell/sys/linux/vsf_linux_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__WIN__) && defined(__CPU_X64__)
#   define exec_ret_t           intptr_t
#else
#   define exec_ret_t           int
#endif

#if VSF_LINUX_CFG_WRAPPER == ENABLED
#define confstr             VSF_LINUX_WRAPPER(confstr)

#define usleep              VSF_LINUX_WRAPPER(usleep)
#define sleep               VSF_LINUX_WRAPPER(sleep)
#define pause               VSF_LINUX_WRAPPER(pause)

#define getlogin            VSF_LINUX_WRAPPER(getlogin)
#define getlogin_r          VSF_LINUX_WRAPPER(getlogin_r)

#define setgid              VSF_LINUX_WRAPPER(setgid)
#define getgid              VSF_LINUX_WRAPPER(getgid)
#define setegid             VSF_LINUX_WRAPPER(setegid)
#define getegid             VSF_LINUX_WRAPPER(getegid)
#define setuid              VSF_LINUX_WRAPPER(setuid)
#define getuid              VSF_LINUX_WRAPPER(getuid)
#define seteuid             VSF_LINUX_WRAPPER(seteuid)
#define geteuid             VSF_LINUX_WRAPPER(geteuid)
#define getpid              VSF_LINUX_WRAPPER(getpid)
#define getppid             VSF_LINUX_WRAPPER(getppid)
#define gettid              VSF_LINUX_WRAPPER(gettid)
#define setsid              VSF_LINUX_WRAPPER(setsid)
#define getsid              VSF_LINUX_WRAPPER(getsid)
#define setpgid             VSF_LINUX_WRAPPER(setpgid)
#define getpgid             VSF_LINUX_WRAPPER(getpgid)
#define setpgrp             VSF_LINUX_WRAPPER(setpgrp)
#define getpgrp             VSF_LINUX_WRAPPER(getpgrp)
#define setresuid           VSF_LINUX_WRAPPER(setresuid)
#define setresgid           VSF_LINUX_WRAPPER(setresgid)

#define execl               VSF_LINUX_WRAPPER(execl)
#define execlp              VSF_LINUX_WRAPPER(execlp)
#define execv               VSF_LINUX_WRAPPER(execv)
#define execve              VSF_LINUX_WRAPPER(execve)
#define execvp              VSF_LINUX_WRAPPER(execvp)
#define execvpe             VSF_LINUX_WRAPPER(execvpe)
#define sysconf             VSF_LINUX_WRAPPER(sysconf)
#define pathconf            VSF_LINUX_WRAPPER(pathconf)
#define fpathconf           VSF_LINUX_WRAPPER(fpathconf)
#define pipe                VSF_LINUX_WRAPPER(pipe)
#define pipe2               VSF_LINUX_WRAPPER(pipe2)
#define alarm               VSF_LINUX_WRAPPER(alarm)
#define ualarm              VSF_LINUX_WRAPPER(ualarm)
#define fork                VSF_LINUX_WRAPPER(fork)
#define _exit               VSF_LINUX_WRAPPER(_exit)
#define acct                VSF_LINUX_WRAPPER(acct)

#define isatty              VSF_LINUX_WRAPPER(isatty)
#define symlink             VSF_LINUX_WRAPPER(symlink)
#define getpagesize         VSF_LINUX_WRAPPER(getpagesize)
#define ftruncate           VSF_LINUX_WRAPPER(ftruncate)
#define truncate            VSF_LINUX_WRAPPER(truncate)
#define ftruncate64         VSF_LINUX_WRAPPER(ftruncate64)
#define truncate64          VSF_LINUX_WRAPPER(truncate64)
#define readlink            VSF_LINUX_WRAPPER(readlink)
#define tcgetpgrp           VSF_LINUX_WRAPPER(tcgetpgrp)
#define tcsetpgrp           VSF_LINUX_WRAPPER(tcsetpgrp)
#define getpass             VSF_LINUX_WRAPPER(getpass)
#define gethostname         VSF_LINUX_WRAPPER(gethostname)
#define sethostname         VSF_LINUX_WRAPPER(sethostname)

#define getentropy          VSF_LINUX_WRAPPER(getentropy)

#define gethostid           VSF_LINUX_WRAPPER(gethostid)
#define sethostid           VSF_LINUX_WRAPPER(sethostid)

#if __IS_COMPILER_IAR__
#else
#   define access           VSF_LINUX_WRAPPER(access)
#   define unlink           VSF_LINUX_WRAPPER(unlink)
#   define unlinkat         VSF_LINUX_WRAPPER(unlinkat)
#   define link             VSF_LINUX_WRAPPER(link)
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
#   define fchdir           VSF_LINUX_WRAPPER(fchdir)
#   define getcwd           VSF_LINUX_WRAPPER(getcwd)
#   define fsync            VSF_LINUX_WRAPPER(fsync)
#   define fdatasync        VSF_LINUX_WRAPPER(fdatasync)
#endif
#elif defined(__WIN__) && !defined(__VSF_APPLET__)
#define _exit               VSF_LINUX_WRAPPER(_exit)
#endif

// syscalls

#define __NR_close          close
#define __NR_dup            dup
#define __NR_dup2           dup2
#define __NR_dup3           dup3
#define __NR_access         access
#define __NR_write          write
#define __NR_writev         writev
#define __NR_read           read
#define __NR_readv          readv
#define __NR_truncate       truncate
#define __NR_rtruncate      ftruncate
#define __NR_faccessat      faccessat
#define __NR_unlink         unlink
#define __NR_unlinkat       unlinkat
#define __NR_sync           sync
#define __NR_fsync          fsync
#define __NR_chdir          chdir
#define __NR_fchdir         fchdir
#define __NR_chown          chown
#define __NR_fchown         fchown
#define __NR_fchownat       fchownat
#define __NR_chroot         chroot
#define __NR_pipe           pipe
#define __NR_pipe2          pipe2
#define __NR_alarm          alarm
#define __NR_execve         execve
#define __NR_wait4          wait4
#define __NR_gettid         gettid
#define SYS_gettid          __NR_gettid

#define __NR_preadv(__fd, __vec, __vlen, __pos_l, __pos_h)                      \
                            preadv64((__fd), (__vec), (__vlen), ((off64_t)(__pos_l) << 32) + (__pos_h))
#define __NR_pwritev(__fd, __vec, __vlen, __pos_l, __pos_h)                     \
                            pwritev64((__fd), (__vec), (__vlen), ((off64_t)(__pos_l) << 32) + (__pos_h))

#define _POSIX_VERSION      200112L

#define STDIN_FILENO        0
#define STDOUT_FILENO       1
#define STDERR_FILENO       2

#define environ             (vsf_linux_get_cur_process()->__environ)

// confstr

enum {
    _CS_PATH,
#define _CS_PATH                    _CS_PATH
    _CS_GNU_LIBC_VERSION,
#define _CS_GNU_LIBC_VERSION        _CS_GNU_LIBC_VERSION
    _CS_GNU_LIBPTHREAD_VERSION,
#define _CS_GNU_LIBPTHREAD_VERSION  _CS_GNU_LIBPTHREAD_VERSION
};

enum {
    DT_UNKNOWN,
#define DT_UNKNOWN                  DT_UNKNOWN
    DT_REG,
#define DT_REG                      DT_REG
    DT_DIR,
#define DT_DIR                      DT_DIR
    DT_FIFO,
#define DT_FIFO                     DT_FIFO
    DT_SOCK,
#define DT_SOCK                     DT_SOCK
    DT_CHR,
#define DT_CHR                      DT_CHR
    DT_BLK,
#define DT_BLK                      DT_BLK
    DT_LNK,
#define DT_LNK                      DT_LNK
    DT_EXE,
#define DT_EXE                      DT_EXE
};

enum {
    _SC_PHYS_PAGES,
    _SC_PAGESIZE,
    _SC_OPEN_MAX,
    _SC_CLK_TCK,
    _SC_GETGR_R_SIZE_MAX,
    _SC_SYMLOOP_MAX,
    _SC_NPROCESSORS_ONLN,

    _SC_PAGE_SIZE = _SC_PAGESIZE,
};

enum {
    _PC_LINK_MAX,
    _PC_MAX_CANON,
    _PC_MAX_INPUT,
    _PC_NAME_MAX,
    _PC_PATH_MAX,
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

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(confstr);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(usleep);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(sleep);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pause);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(alarm);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(ualarm);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(getlogin);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(getlogin_r);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(setgid);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(getgid);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(setegid);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(getegid);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(setuid);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(getuid);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(seteuid);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(geteuid);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(getpid);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(getppid);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(gettid);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(setsid);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(getsid);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(setpgid);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(getpgid);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(setpgrp);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(getpgrp);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(setresuid);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(setresgid);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__execl_va);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(execl);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(execle);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__execlp_va);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(execlp);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(execv);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(execve);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(execvp);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(execvpe);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(daemon);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(sysconf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pathconf);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fpathconf);
    // realpath belongs to stdlib.h, but do not remove realpath here for compatibility
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(realpath);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pipe);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pipe2);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(access);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(unlink);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(unlinkat);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(link);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(mkdirat);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(rmdir);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(dup);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(dup2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(dup3);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(chroot);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(chdir);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fchdir);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(getcwd);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(close);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(lseek);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(read);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(write);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(readv);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(writev);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pread);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pwrite);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(preadv);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(pwritev);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(sync);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(syncfs);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fsync);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fdatasync);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(isatty);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(getpagesize);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(symlink);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(ftruncate);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(truncate);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(ftruncate64);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(truncate64);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(readlink);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(tcgetpgrp);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(tcsetpgrp);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(getpass);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(gethostname);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(sethostname);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(chown);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fchown);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(lchown);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fchownat);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(getentropy);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(gethostid);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(sethostid);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(lseek64);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(ttyname);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(ttyname_r);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(_exit);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(acct);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__vsf_linux_vfork_prepare);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(mkdirs);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(getdtablesize);
} vsf_linux_unistd_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_unistd_vplt_t vsf_linux_unistd_vplt;
#   endif
#endif

#if     defined(__VSF_APPLET__) && (defined(__VSF_APPLET_LIB__) || defined(__VSF_APPLET_LINUX_UNISTD_LIB__))\
    &&  VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_APPLET_USE_UNISTD == ENABLED

#ifndef VSF_LINUX_APPLET_UNISTD_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_UNISTD_VPLT                                     \
            ((vsf_linux_unistd_vplt_t *)(VSF_LINUX_APPLET_VPLT->unistd_vplt))
#   else
#       define VSF_LINUX_APPLET_UNISTD_VPLT                                     \
            ((vsf_linux_unistd_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_UNISTD_ENTRY(__NAME)                                   \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_UNISTD_VPLT, __NAME)
#define VSF_LINUX_APPLET_UNISTD_IMP(...)                                        \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_UNISTD_VPLT, __VA_ARGS__)

VSF_LINUX_APPLET_UNISTD_IMP(__execl_va, exec_ret_t, const char *pathname, const char *arg, va_list ap, bool has_env) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(__execl_va)(pathname, arg, ap, has_env);
}
VSF_LINUX_APPLET_UNISTD_IMP(__execlp_va, exec_ret_t, const char *pathname, const char *arg, va_list ap) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(__execlp_va)(pathname, arg, ap);
}
VSF_LINUX_APPLET_UNISTD_IMP(confstr, size_t, int name, char *buf, size_t len) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(confstr)(name, buf, len);
}
VSF_LINUX_APPLET_UNISTD_IMP(usleep, int, int micro_seconds) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(usleep)(micro_seconds);
}
VSF_LINUX_APPLET_UNISTD_IMP(sleep, unsigned, unsigned seconds) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(sleep)(seconds);
}
VSF_LINUX_APPLET_UNISTD_IMP(pause, int, void) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(pause)();
}
VSF_LINUX_APPLET_UNISTD_IMP(alarm, unsigned int, unsigned int seconds) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(alarm)(seconds);
}
VSF_LINUX_APPLET_UNISTD_IMP(ualarm, useconds_t, useconds_t usecs, useconds_t interval) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(ualarm)(usecs, interval);
}
VSF_LINUX_APPLET_UNISTD_IMP(getlogin, char *, void) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(getlogin)();
}
VSF_LINUX_APPLET_UNISTD_IMP(getlogin_r, int, char *buf, size_t bufsize) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(getlogin_r)(buf, bufsize);
}
VSF_LINUX_APPLET_UNISTD_IMP(setuid, int, uid_t uid) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(setuid)(uid);
}
VSF_LINUX_APPLET_UNISTD_IMP(getuid, uid_t, void) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(getuid)();
}
VSF_LINUX_APPLET_UNISTD_IMP(seteuid, int, uid_t euid) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(seteuid)(euid);
}
VSF_LINUX_APPLET_UNISTD_IMP(geteuid, uid_t, void) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(geteuid)();
}
VSF_LINUX_APPLET_UNISTD_IMP(setgid, int, gid_t gid) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(setgid)(gid);
}
VSF_LINUX_APPLET_UNISTD_IMP(getgid, gid_t, void) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(getgid)();
}
VSF_LINUX_APPLET_UNISTD_IMP(setegid, int, gid_t egid) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(setegid)(egid);
}
VSF_LINUX_APPLET_UNISTD_IMP(getegid, gid_t, void) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(getegid)();
}
VSF_LINUX_APPLET_UNISTD_IMP(getpid, pid_t, void) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(getpid)();
}
VSF_LINUX_APPLET_UNISTD_IMP(getppid, pid_t, void) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(getppid)();
}
VSF_LINUX_APPLET_UNISTD_IMP(gettid, pid_t, void) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(gettid)();
}
VSF_LINUX_APPLET_UNISTD_IMP(setsid, pid_t, void) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(setsid)();
}
VSF_LINUX_APPLET_UNISTD_IMP(getsid, pid_t, pid_t pid) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(getsid)(pid);
}
VSF_LINUX_APPLET_UNISTD_IMP(setpgid, int, pid_t pid, pid_t pgid) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(setpgid)(pid, pgid);
}
VSF_LINUX_APPLET_UNISTD_IMP(getpgid, pid_t, pid_t pid) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(getpgid)(pid);
}
VSF_LINUX_APPLET_UNISTD_IMP(setpgrp, int, void) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(setpgrp)();
}
VSF_LINUX_APPLET_UNISTD_IMP(getpgrp, pid_t, void) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(getpgrp)();
}
VSF_LINUX_APPLET_UNISTD_IMP(setresuid, int, uid_t ruid, uid_t euid, uid_t suid) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(setresuid)(ruid, euid, suid);
}
VSF_LINUX_APPLET_UNISTD_IMP(setresgid, int, gid_t rgid, gid_t egid, gid_t sgid) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(setresgid)(rgid, egid, sgid);
}
VSF_LINUX_APPLET_UNISTD_IMP(execv, exec_ret_t, const char *pathname, char * const * argv) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(execv)(pathname, argv);
}
VSF_LINUX_APPLET_UNISTD_IMP(execve, exec_ret_t, const char *pathname, char * const * argv, char * const * envp) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(execve)(pathname, argv, envp);
}
VSF_LINUX_APPLET_UNISTD_IMP(execvp, exec_ret_t, const char *file, char * const * argv) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(execvp)(file, argv);
}
VSF_LINUX_APPLET_UNISTD_IMP(execvpe, exec_ret_t, const char *file, char * const * argv, char * const * envp) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(execvpe)(file, argv, envp);
}
VSF_LINUX_APPLET_UNISTD_IMP(daemon, int, int nochdir, int noclose) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(daemon)(nochdir, noclose);
}
VSF_LINUX_APPLET_UNISTD_IMP(sysconf, long, int name) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(sysconf)(name);
}
VSF_LINUX_APPLET_UNISTD_IMP(pathconf, long, const char *path, int name) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(pathconf)(path, name);
}
VSF_LINUX_APPLET_UNISTD_IMP(fpathconf, long, int fd, int name) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(fpathconf)(fd, name);
}
VSF_LINUX_APPLET_UNISTD_IMP(pipe, int, int pipefd[2]) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(pipe)(pipefd);
}
VSF_LINUX_APPLET_UNISTD_IMP(pipe2, int, int pipefd[2], int flags) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(pipe2)(pipefd, flags);
}
VSF_LINUX_APPLET_UNISTD_IMP(access, int, const char *pathname, int mode) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(access)(pathname, mode);
}
VSF_LINUX_APPLET_UNISTD_IMP(unlink, int, const char *pathname) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(unlink)(pathname);
}
VSF_LINUX_APPLET_UNISTD_IMP(unlinkat, int, int dirfd, const char *pathname, int flags) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(unlinkat)(dirfd, pathname, flags);
}
VSF_LINUX_APPLET_UNISTD_IMP(link, int, const char *oldpath, const char *newpath) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(link)(oldpath, newpath);
}
VSF_LINUX_APPLET_UNISTD_IMP(mkdirat, int, int dirfd, const char *pathname, mode_t mode) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(mkdirat)(dirfd, pathname, mode);
}
VSF_LINUX_APPLET_UNISTD_IMP(rmdir, int, const char *pathname) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(rmdir)(pathname);
}
VSF_LINUX_APPLET_UNISTD_IMP(dup, int, int oldfd) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(dup)(oldfd);
}
VSF_LINUX_APPLET_UNISTD_IMP(dup2, int, int oldfd, int newfd) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(dup2)(oldfd, newfd);
}
VSF_LINUX_APPLET_UNISTD_IMP(dup3, int, int oldfd, int newfd, int flags) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(dup3)(oldfd, newfd, flags);
}
VSF_LINUX_APPLET_UNISTD_IMP(chroot, int, const char *path) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(chroot)(path);
}
VSF_LINUX_APPLET_UNISTD_IMP(chdir, int, const char *pathname) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(chdir)(pathname);
}
VSF_LINUX_APPLET_UNISTD_IMP(fchdir, int, int fd) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(fchdir)(fd);
}
VSF_LINUX_APPLET_UNISTD_IMP(getcwd, char *, char *buffer, size_t maxlen) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(getcwd)(buffer, maxlen);
}
VSF_LINUX_APPLET_UNISTD_IMP(close, int, int fd) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(close)(fd);
}
VSF_LINUX_APPLET_UNISTD_IMP(lseek, off_t, int fd, off_t offset, int whence) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(lseek)(fd, offset, whence);
}
VSF_LINUX_APPLET_UNISTD_IMP(read, ssize_t, int fd, void *buf, size_t count) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(read)(fd, buf, count);
}
VSF_LINUX_APPLET_UNISTD_IMP(write, ssize_t, int fd, const void *buf, size_t count) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(write)(fd, buf, count);
}
VSF_LINUX_APPLET_UNISTD_IMP(readv, ssize_t, int fd, const struct iovec *iov, int iovcnt) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(readv)(fd, iov, iovcnt);
}
VSF_LINUX_APPLET_UNISTD_IMP(writev, ssize_t, int fd, const struct iovec *iov, int iovcnt) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(writev)(fd, iov, iovcnt);
}
VSF_LINUX_APPLET_UNISTD_IMP(pread, ssize_t, int fd, void *buf, size_t count, off_t offset) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(pread)(fd, buf, count, offset);
}
VSF_LINUX_APPLET_UNISTD_IMP(pwrite, ssize_t, int fd, const void *buf, size_t count, off_t offset) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(pwrite)(fd, buf, count, offset);
}
VSF_LINUX_APPLET_UNISTD_IMP(preadv, ssize_t, int fd, const struct iovec *iov, int iovcnt, off_t offset) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(preadv)(fd, iov, iovcnt, offset);
}
VSF_LINUX_APPLET_UNISTD_IMP(pwritev, ssize_t, int fd, const struct iovec *iov, int iovcnt, off_t offset) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(pwritev)(fd, iov, iovcnt, offset);
}
VSF_LINUX_APPLET_UNISTD_IMP(sync, void, void) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_LINUX_APPLET_UNISTD_ENTRY(sync)();
}
VSF_LINUX_APPLET_UNISTD_IMP(syncfs, int, int fd) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(syncfs)(fd);
}
VSF_LINUX_APPLET_UNISTD_IMP(fsync, int, int fd) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(fsync)(fd);
}
VSF_LINUX_APPLET_UNISTD_IMP(fdatasync, int, int fd) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(fdatasync)(fd);
}
VSF_LINUX_APPLET_UNISTD_IMP(isatty, int, int fd) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(isatty)(fd);
}
VSF_LINUX_APPLET_UNISTD_IMP(getpagesize, size_t, void) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(getpagesize)();
}
VSF_LINUX_APPLET_UNISTD_IMP(symlink, int, const char *target, const char *linkpath) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(symlink)(target, linkpath);
}
VSF_LINUX_APPLET_UNISTD_IMP(ftruncate, int, int fd, off_t length) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(ftruncate)(fd, length);
}
VSF_LINUX_APPLET_UNISTD_IMP(truncate, int, const char *pathname, off_t length) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(truncate)(pathname, length);
}
VSF_LINUX_APPLET_UNISTD_IMP(ftruncate64, int, int fd, off64_t length) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(ftruncate64)(fd, length);
}
VSF_LINUX_APPLET_UNISTD_IMP(truncate64, int, const char *pathname, off64_t length) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(truncate64)(pathname, length);
}
VSF_LINUX_APPLET_UNISTD_IMP(readlink, ssize_t, const char *pathname, char *buf, size_t bufsiz) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(readlink)(pathname, buf, bufsiz);
}
VSF_LINUX_APPLET_UNISTD_IMP(tcgetpgrp, pid_t, int fd) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(tcgetpgrp)(fd);
}
VSF_LINUX_APPLET_UNISTD_IMP(tcsetpgrp, int, int fd, pid_t pgrp) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(tcsetpgrp)(fd, pgrp);
}
VSF_LINUX_APPLET_UNISTD_IMP(getpass, char *, const char *prompt) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(getpass)(prompt);
}
VSF_LINUX_APPLET_UNISTD_IMP(gethostname, int, char *name, size_t len) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(gethostname)(name, len);
}
VSF_LINUX_APPLET_UNISTD_IMP(sethostname, int, const char *name, size_t len) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(sethostname)(name, len);
}
VSF_LINUX_APPLET_UNISTD_IMP(chown, int, const char *pathname, uid_t owner, gid_t group) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(chown)(pathname, owner, group);
}
VSF_LINUX_APPLET_UNISTD_IMP(fchown, int, int fd, uid_t owner, gid_t group) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(fchown)(fd, owner, group);
}
VSF_LINUX_APPLET_UNISTD_IMP(lchown, int, const char *pathname, uid_t owner, gid_t group) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(lchown)(pathname, owner, group);
}
VSF_LINUX_APPLET_UNISTD_IMP(fchownat, int, int dirfd, const char *pathname, uid_t owner, gid_t group, int flags) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(fchownat)(dirfd, pathname, owner, group, flags);
}
VSF_LINUX_APPLET_UNISTD_IMP(getentropy, int, void *buffer, size_t length) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(getentropy)(buffer, length);
}
VSF_LINUX_APPLET_UNISTD_IMP(gethostid, long, void) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(gethostid)();
}
VSF_LINUX_APPLET_UNISTD_IMP(sethostid, int, long hostid) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(sethostid)(hostid);
}
VSF_LINUX_APPLET_UNISTD_IMP(lseek64, off64_t, int fd, off64_t offset, int whence) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(lseek64)(fd, offset, whence);
}
VSF_LINUX_APPLET_UNISTD_IMP(ttyname, char *, int fd) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(ttyname)(fd);
}
VSF_LINUX_APPLET_UNISTD_IMP(ttyname_r, int, int fd, char *buf, size_t buflen) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(ttyname_r)(fd, buf, buflen);
}
VSF_LINUX_APPLET_UNISTD_IMP(_exit, void, int status) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_LINUX_APPLET_UNISTD_ENTRY(_exit)(status);
}
VSF_LINUX_APPLET_UNISTD_IMP(acct, int, const char *filename) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(acct)(filename);
}
VSF_LINUX_APPLET_UNISTD_IMP(__vsf_linux_vfork_prepare, pid_t, vsf_linux_process_t *parent_process) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(__vsf_linux_vfork_prepare)(parent_process);
}
VSF_LINUX_APPLET_UNISTD_IMP(mkdirs, int, const char *pathname, mode_t mode) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(mkdirs)(pathname, mode);
}
VSF_LINUX_APPLET_UNISTD_IMP(getdtablesize, int, void) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_UNISTD_ENTRY(getdtablesize)();
}

VSF_APPLET_VPLT_FUNC_DECORATOR(execl) exec_ret_t execl(const char *pathname, const char *arg, ...) {
    exec_ret_t ret;

    va_list ap;
    va_start(ap, arg);
        ret = VSF_LINUX_APPLET_UNISTD_ENTRY(__execl_va)(pathname, arg, ap, false);
    va_end(ap);
    return ret;
}
VSF_APPLET_VPLT_FUNC_DECORATOR(execle) exec_ret_t execle(const char *pathname, const char *arg, ...) {
    exec_ret_t ret;

    va_list ap;
    va_start(ap, arg);
        ret = VSF_LINUX_APPLET_UNISTD_ENTRY(__execl_va)(pathname, arg, ap, true);
    va_end(ap);
    return ret;
}
VSF_APPLET_VPLT_FUNC_DECORATOR(execlp) exec_ret_t execlp(const char *file, const char *arg, ...) {
    int ret;

    va_list ap;
    va_start(ap, arg);
        ret = VSF_LINUX_APPLET_UNISTD_ENTRY(__execlp_va)(file, arg, ap);
    va_end(ap);
    return ret;
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_UNISTD

size_t confstr(int name, char *buf, size_t len);

int usleep(int micro_seconds);
unsigned sleep(unsigned seconds);
int pause(void);

unsigned int alarm(unsigned int seconds);
useconds_t ualarm(useconds_t usecs, useconds_t interval);

char * getlogin(void);
int getlogin_r(char *buf, size_t bufsize);

pid_t gettid(void);
pid_t getpid(void);
pid_t getppid(void);

int setuid(uid_t uid);
uid_t getuid(void);
int seteuid(uid_t euid);
uid_t geteuid(void);
int setegid(gid_t egid);
gid_t getegid(void);
int setgid(gid_t git);
gid_t getgid(void);
pid_t setsid(void);
pid_t getsid(pid_t pid);
int setpgid(pid_t pid, pid_t pgid);
pid_t getpgid(pid_t pid);
int setpgrp(void);
pid_t getpgrp(void);
int setresuid(uid_t ruid, uid_t euid, uid_t suid);
int setresgid(gid_t rgid, gid_t egid, gid_t sgid);

exec_ret_t execl(const char *pathname, const char *arg, ...);
exec_ret_t execle(const char *pathname, const char *arg, ...);
exec_ret_t execlp(const char *file, const char *arg, ...);
exec_ret_t execv(const char *pathname, char * const * argv);
exec_ret_t execve(const char *pathname, char * const * argv, char * const * envp);
exec_ret_t execvp(const char *file, char * const * argv);
exec_ret_t execvpe(const char *file, char * const * argv, char * const * envp);
int daemon(int nochdir, int noclose);

pid_t fork(void);
void _exit(int status);
long sysconf(int name);
long pathconf(const char *path, int name);
long fpathconf(int fd, int name);
int pipe(int pipefd[2]);
int pipe2(int pipefd[2], int flags);

int access(const char *pathname, int mode);
int unlink(const char *pathname);
int unlinkat(int dirfd, const char *pathname, int flags);
int link(const char *oldpath, const char *newpath);
int mkdirat(int dirfd, const char *pathname, mode_t mode);
int mkdirs(const char *pathname, mode_t mode);
int rmdir(const char *pathname);
int dup(int oldfd);
int dup2(int oldfd, int newfd);
int dup3(int oldfd, int newfd, int flags);

int chroot(const char *path);
int chdir(const char *pathname);
int fchdir(int fd);
char * getcwd(char *buffer, size_t maxlen);

int close(int fd);
off_t lseek(int fd, off_t offset, int whence);
off64_t lseek64(int fd, off64_t offset, int whence);
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);
ssize_t readv(int fd, const struct iovec *iov, int iovcnt);
ssize_t writev(int fd, const struct iovec *iov, int iovcnt);
ssize_t pread(int fd, void *buf, size_t count, off_t offset);
ssize_t preadv(int fd, const struct iovec *iov, int iovcnt, off_t offset);
ssize_t preadv64 (int fd, const struct iovec *vector, int iovcnt, off64_t offset);
ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset);
ssize_t pwritev(int fd, const struct iovec *iov, int iovcnt, off_t offset);
ssize_t pwritev64 (int fd, const struct iovec *vector, int iovcnt, off64_t offset);

void sync(void);
int syncfs(int fd);
int fsync(int fd);
int fdatasync(int fd);

int isatty(int fd);
char * ttyname(int fd);
int ttyname_r(int fd, char *buf, size_t buflen);
size_t getpagesize(void);
int symlink(const char *target, const char *linkpath);
int truncate(const char *pathname, off_t length);
int ftruncate(int fd, off_t length);
int truncate64(const char *pathname, off64_t length);
int ftruncate64(int fd, off64_t length);
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

long gethostid(void);
int sethostid(long hostid);

int getdtablesize(void);

int acct(const char *filename);

#if VSF_LINUX_USE_VFORK == ENABLED
pid_t __vsf_linux_vfork_prepare(vsf_linux_process_t *parent_process);
#endif

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_UNISTD

#if VSF_LINUX_USE_VFORK == ENABLED
#   define vfork()          ({                                                  \
    vsf_linux_process_t *parent_process = vsf_linux_get_cur_process();          \
    pid_t result = (pid_t)-1, child_pid = __vsf_linux_vfork_prepare(parent_process);\
    if (child_pid >= 0) {                                                       \
        result = !setjmp(parent_process->__vfork_jmpbuf) ? (pid_t)0 : child_pid;\
    }                                                                           \
    result;                                                                     \
})
#else
#   define vfork()          -1
#endif

#ifdef __cplusplus
}
#endif

#endif
