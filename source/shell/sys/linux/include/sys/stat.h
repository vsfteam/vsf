#ifndef __VSF_LINUX_SYS_STAT_H__
#define __VSF_LINUX_SYS_STAT_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../sys/types.h"
#   include "../simple_libc/time.h"
#else
#   include <sys/types.h>
#   include <time.h>
#endif

// for fs constants
#include "component/fs/vsf_fs_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_CFG_WRAPPER == ENABLED
#define stat                    VSF_LINUX_WRAPPER(stat)
#define lstat                   VSF_LINUX_WRAPPER(lstat)
#define fstat                   VSF_LINUX_WRAPPER(fstat)
#define fstatat                 VSF_LINUX_WRAPPER(fstatat)
#define umask                   VSF_LINUX_WRAPPER(umask)
#define chmod                   VSF_LINUX_WRAPPER(chmod)
#define fchmod                  VSF_LINUX_WRAPPER(fchmod)
#define futimens                VSF_LINUX_WRAPPER(futimens)
#define utimensat               VSF_LINUX_WRAPPER(utimensat)
#define mkdir                   VSF_LINUX_WRAPPER(mkdir)
#define mkfifo                  VSF_LINUX_WRAPPER(mkfifo)
#define mkfifoat                VSF_LINUX_WRAPPER(mkfifoat)
#define mknod                   VSF_LINUX_WRAPPER(mknod)
#define mknodat                 VSF_LINUX_WRAPPER(mknodat)
#endif

// syscalls

#define __NR_stat               stat
#define __NR_fstat              fstat
#define __NR_fstatat            fstatat
#define __NR_umask              umask
#define __NR_chmod              chmod
#define __NR_fchmod             fchmod
#define __NR_fchmodat           fchmodat
#define __NR_futimesat          futimesat
#define __NR_utimensat          utimensat

// TODO: assert extension attr is fit in vk_file_attr_t
#define VSF_FILE_ATTR_CHR       (VSF_FILE_ATTR_USER << 0)
#define VSF_FILE_ATTR_BLK       (VSF_FILE_ATTR_USER << 1)
#define VSF_FILE_ATTR_EXCL      (VSF_FILE_ATTR_USER << 3)
#define VSF_FILE_ATTR_SOCK      (VSF_FILE_ATTR_USER << 4)
#define VSF_FILE_ATTR_TTY       (VSF_FILE_ATTR_USER << 5)
#define VSF_FILE_ATTR_FIFO      (VSF_FILE_ATTR_USER << 6)
// internal use, indicating priv of sfd is shared, do not free
//  normally, vsf_linux_fs_bind_fdpriv is used to do binding for __VSF_FILE_ATTR_SHARE_PRIV
#define __VSF_FILE_ATTR_SHARE_PRIV      (VSF_FILE_ATTR_USER << 16)

#define UTIME_NOW	            ((1l << 30) - 1l)
#define UTIME_OMIT	            ((1l << 30) - 2l)

#define S_IFDIR                 0040000
#define S_IFREG                 0100000
#define S_IFLNK                 0120000
#define S_IFIFO                 0010000
#define S_IFCHR                 0020000
#define S_IFSOCK                0140000
#define S_IFBLK                 0060000
#define S_IFMT                  0170000

#define __S_IR                  04
#define __S_IW                  02
#define __S_IX                  01
#define S_IROTH                 (__S_IR << 0)
#define S_IWOTH                 (__S_IW << 0)
#define S_IXOTH                 (__S_IX << 0)
#define S_IRWXO                 (S_IROTH | S_IWOTH | S_IXOTH)
#define S_IRGRP                 (__S_IR << 3)
#define S_IWGRP                 (__S_IW << 3)
#define S_IXGRP                 (__S_IX << 3)
#define S_IRWXG                 (S_IRGRP | S_IWGRP | S_IXGRP)
#define S_IRUSR                 (__S_IR << 6)
#define S_IWUSR                 (__S_IW << 6)
#define S_IXUSR                 (__S_IX << 6)
#define S_IRWXU                 (S_IRUSR | S_IWUSR | S_IXUSR)

#define S_IREAD                 S_IRUSR
#define S_IWRITE                S_IWUSR
#define S_IEXEC                 S_IXUSR

// protection bits
#define S_ISUID                 0004000
#define S_ISGID                 0002000
#define S_ISVTX                 0001000

#define S_ISLNK(__MODE)         (((__MODE) & S_IFMT) == S_IFLNK)
#define S_ISREG(__MODE)         (((__MODE) & S_IFMT) == S_IFREG)
#define S_ISDIR(__MODE)         (((__MODE) & S_IFMT) == S_IFDIR)
#define S_ISFIFO(__MODE)        (((__MODE) & S_IFMT) == S_IFIFO)
#define S_ISSOCK(__MODE)        (((__MODE) & S_IFMT) == S_IFSOCK)
#define S_ISCHR(__MODE)         (((__MODE) & S_IFMT) == S_IFCHR)
#define S_ISBLK(__MODE)         (((__MODE) & S_IFMT) == S_IFBLK)

// struct stat is actually stat64
struct stat {
    dev_t           st_dev;
    dev_t           st_rdev;
    ino64_t         st_ino;
    mode_t          st_mode;
    nlink_t         st_nlink;
    uid_t           st_uid;
    gid_t           st_gid;
    off64_t         st_size;
    blksiz_t        st_blksize;
    blkcnt64_t      st_blocks;
    mode_t          st_attr;

    struct timespec st_atim;
    struct timespec st_mtim;
    struct timespec st_ctim;
#define st_atime    st_atim.tv_sec
#define st_mtime    st_mtim.tv_sec
#define st_ctime    st_ctim.tv_sec
};

struct stat64 {
    dev_t           st_dev;
    dev_t           st_rdev;
    ino64_t         st_ino;
    mode_t          st_mode;
    nlink_t         st_nlink;
    uid_t           st_uid;
    gid_t           st_gid;
    off64_t         st_size;
    blksiz_t        st_blksize;
    blkcnt64_t      st_blocks;
    mode_t          st_attr;

    struct timespec st_atim;
    struct timespec st_mtim;
    struct timespec st_ctim;
#define st_atime    st_atim.tv_sec
#define st_mtime    st_mtim.tv_sec
#define st_ctime    st_ctim.tv_sec
};

#if VSF_LINUX_APPLET_USE_SYS_STAT == ENABLED
typedef struct vsf_linux_sys_stat_vplt_t {
    vsf_vplt_info_t info;

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(umask);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(stat);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fstat);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fstatat);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(chmod);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fchmod);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(mkdir);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(mkfifo);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(mkfifoat);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(mknod);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(mknodat);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(lstat);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(futimens);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(utimensat);
} vsf_linux_sys_stat_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_sys_stat_vplt_t vsf_linux_sys_stat_vplt;
#   endif
#endif

#if     defined(__VSF_APPLET__) && (defined(__VSF_APPLET_LIB__) || defined(__VSF_APPLET_LINUX_SYS_STAT_LIB__))\
    &&  VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_APPLET_USE_SYS_STAT == ENABLED

#ifndef VSF_LINUX_APPLET_SYS_STAT_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_SYS_STAT_VPLT                                   \
            ((vsf_linux_sys_stat_vplt_t *)(VSF_LINUX_APPLET_VPLT->sys_stat_vplt))
#   else
#       define VSF_LINUX_APPLET_SYS_STAT_VPLT                                   \
            ((vsf_linux_sys_stat_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_SYS_STAT_ENTRY(__NAME)                                 \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_SYS_STAT_VPLT, __NAME)
#define VSF_LINUX_APPLET_SYS_STAT_IMP(...)                                      \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_SYS_STAT_VPLT, __VA_ARGS__)

VSF_LINUX_APPLET_SYS_STAT_IMP(umask, mode_t, mode_t mask) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_STAT_ENTRY(umask)(mask);
}
VSF_LINUX_APPLET_SYS_STAT_IMP(stat, int, const char *pathname, struct stat *buf) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_STAT_ENTRY(stat)(pathname, buf);
}
VSF_LINUX_APPLET_SYS_STAT_IMP(lstat, int, const char *pathname, struct stat *buf) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_STAT_ENTRY(lstat)(pathname, buf);
}
VSF_LINUX_APPLET_SYS_STAT_IMP(fstat, int, int fd, struct stat *buf) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_STAT_ENTRY(fstat)(fd, buf);
}
VSF_LINUX_APPLET_SYS_STAT_IMP(fstatat, int, int dirfd, const char *pathname, struct stat *buf, int flags) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_STAT_ENTRY(fstatat)(dirfd, pathname, buf, flags);
}
VSF_LINUX_APPLET_SYS_STAT_IMP(chmod, int, const char *pathname, mode_t mode) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_STAT_ENTRY(chmod)(pathname, mode);
}
VSF_LINUX_APPLET_SYS_STAT_IMP(fchmod, int, int fd, mode_t mode) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_STAT_ENTRY(fchmod)(fd, mode);
}
VSF_LINUX_APPLET_SYS_STAT_IMP(mkdir, int, const char *pathname, mode_t mode) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_STAT_ENTRY(mkdir)(pathname, mode);
}
VSF_LINUX_APPLET_SYS_STAT_IMP(mkfifo, int, const char *pathname, mode_t mode) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_STAT_ENTRY(mkfifo)(pathname, mode);
}
VSF_LINUX_APPLET_SYS_STAT_IMP(mkfifoat, int, int dirfd, const char *pathname, mode_t mode) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_STAT_ENTRY(mkfifoat)(dirfd, pathname, mode);
}
VSF_LINUX_APPLET_SYS_STAT_IMP(mknod, int, const char *pathname, mode_t mode, dev_t dev) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_STAT_ENTRY(mknod)(pathname, mode, dev);
}
VSF_LINUX_APPLET_SYS_STAT_IMP(mknodat, int, int dirfd, const char *pathname, mode_t mode, dev_t dev) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_STAT_ENTRY(mknodat)(dirfd, pathname, mode, dev);
}
VSF_LINUX_APPLET_SYS_STAT_IMP(futimens, int, int fd, const struct timespec times[2]) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_STAT_ENTRY(futimens)(fd, times);
}
VSF_LINUX_APPLET_SYS_STAT_IMP(utimensat, int, int dirfd, const char *pathname, const struct timespec times[2], int flags) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_STAT_ENTRY(utimensat)(dirfd, pathname, times, flags);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_STAT

mode_t umask(mode_t mask);
int stat(const char *pathname, struct stat *buf);
int lstat(const char *pathname, struct stat *buf);
int fstat(int fd, struct stat *buf);
int fstatat(int dirfd, const char *pathname, struct stat *buf, int flags);
int futimens(int fd, const struct timespec times[2]);
int utimensat(int dirfd, const char *pathname, const struct timespec times[2], int flags);
int chmod(const char *pathname, mode_t mode);
int fchmod(int fd, mode_t mode);
int mkdir(const char *pathname, mode_t mode);
int mkfifo(const char *pathname, mode_t mode);
int mkfifoat(int dirfd, const char *pathname, mode_t mode);
int mknod(const char *pathname, mode_t mode, dev_t dev);
int mknodat(int dirfd, const char *pathname, mode_t mode, dev_t dev);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_STAT

static inline int stat64(const char *pathname, struct stat64 *buf)
{
    return stat(pathname, (struct stat *)buf);
}
static inline int lstat64(const char *pathname, struct stat64 *buf)
{
    return lstat(pathname, (struct stat *)buf);
}
static inline int fstat64(int fd, struct stat64 *buf)
{
    return fstat(fd, (struct stat *)buf);
}

#ifdef __cplusplus
}
#endif

#endif
