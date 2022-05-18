#ifndef __VSF_LINUX_STAT_H__
#define __VSF_LINUX_STAT_H__

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
#define fstat                   VSF_LINUX_WRAPPER(fstat)
#define fstatat                 VSF_LINUX_WRAPPER(fstatat)
#define umask                   VSF_LINUX_WRAPPER(umask)
#define chmod                   VSF_LINUX_WRAPPER(chmod)
#define fchmod                  VSF_LINUX_WRAPPER(fchmod)
#endif
#define lstat                   stat

#define VSF_FILE_ATTR_BLK       (VSF_FILE_ATTR_USER << 0)
#define VSF_FILE_ATTR_LNK       (VSF_FILE_ATTR_USER << 1)
#define VSF_FILE_ATTR_EXCL      (VSF_FILE_ATTR_USER << 2)
#define VSF_FILE_ATTR_SOCK      (VSF_FILE_ATTR_USER << 3)

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

// protection bits
#define S_ISUID                 0004000
#define S_ISGID                 0002000
#define S_ISVTX                 0001000

#define S_ISLNK(__MODE) (((__MODE) & S_IFMT) == S_IFLNK)
#define S_ISREG(__MODE) (((__MODE) & S_IFMT) == S_IFREG)
#define S_ISDIR(__MODE) (((__MODE) & S_IFMT) == S_IFDIR)
#define S_ISFIFO(__MODE)(((__MODE) & S_IFMT) == S_IFIFO)
#define S_ISSOCK(__MODE)(((__MODE) & S_IFMT) == S_IFSOCK)

struct stat {
    dev_t           st_dev;
    dev_t           st_rdev;
    ino_t           st_ino;
    mode_t          st_mode;
    nlink_t         st_nlink;
    uid_t           st_uid;
    gid_t           st_gid;
    off_t           st_size;
    blksiz_t        st_blksize;
    blkcnt_t        st_blocks;

    struct timespec st_atim;
    struct timespec st_mtim;
    struct timespec st_ctim;
#define st_atime    st_atim.tv_sec
#define st_mtime    st_mtim.tv_sec
#define st_ctime    st_ctim.tv_sec
};

mode_t umask(mode_t mask);
int stat(const char *pathname, struct stat *buf);
int fstat(int fd, struct stat *buf);
int fstatat(int dirfd, const char *pathname, struct stat *buf, int flags);
int chmod(const char *pathname, mode_t mode);
int fchmod(int fd, mode_t mode);

#ifdef __cplusplus
}
#endif

#endif
