#ifndef __VSF_LINUX_STATVFS_H__
#define __VSF_LINUX_STATVFS_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../sys/types.h"
#else
#   include <sys/types.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define ST_RDONLY       (1 << 0)
#define ST_NOSUID       (1 << 1)
#define ST_NODEV        (1 << 2)
#define ST_NOEXEC       (1 << 3)
#define ST_SYNCHRONOUS  (1 << 4)
#define ST_VALID        (1 << 5)
#define ST_MANDLOCK     (1 << 6)
#define ST_NOATIME      (1 << 10)
#define ST_NODIRATIME   (1 << 11)
#define ST_REAATIME     (1 << 12)
#define ST_NOSYMFOLLOW  (1 << 13)

struct statvfs {
    unsigned long   f_bsize;
    unsigned long   f_frsize;
    fsblkcnt_t      f_blocks;
    fsblkcnt_t      f_bfree;
    fsblkcnt_t      f_bavail;
    fsfilcnt_t      f_files;
    fsfilcnt_t      f_ffree;
    fsfilcnt_t      f_favail;
    unsigned long   f_fsid;
    unsigned long   f_flag;
    unsigned long   f_namemax;
};

struct statvfs64 {
    unsigned long   f_bsize;
    unsigned long   f_frsize;
    fsblkcnt64_t    f_blocks;
    fsblkcnt64_t    f_bfree;
    fsblkcnt64_t    f_bavail;
    fsfilcnt64_t    f_files;
    fsfilcnt64_t    f_ffree;
    fsfilcnt64_t    f_favail;
    unsigned long   f_fsid;
    unsigned long   f_flag;
    unsigned long   f_namemax;
};

int fstatvfs(int fd, struct statvfs *buf);
int statvfs(const char *path, struct statvfs *buf);

int fstatvfs64(int fd, struct statvfs64 *buf);
int statvfs64(const char *path, struct statvfs64 *buf);

#ifdef __cplusplus
}
#endif

#endif
