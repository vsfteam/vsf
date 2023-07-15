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

#if VSF_LINUX_APPLET_USE_SYS_STATVFS == ENABLED
typedef struct vsf_linux_sys_statvfs_vplt_t {
    vsf_vplt_info_t info;

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fstatvfs);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(statvfs);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fstatvfs64);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(statvfs64);
} vsf_linux_sys_statvfs_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_sys_statvfs_vplt_t vsf_linux_sys_statvfs_vplt;
#   endif
#endif

#if     defined(__VSF_APPLET__) && (defined(__VSF_APPLET_LIB__) || defined(__VSF_APPLET_LINUX_SYS_STATVFS_LIB__))\
    &&  VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_APPLET_USE_SYS_STATVFS == ENABLED

#ifndef VSF_LINUX_APPLET_SYS_STATVFS_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_SYS_STATVFS_VPLT                                \
            ((vsf_linux_sys_statvfs_vplt_t *)(VSF_LINUX_APPLET_VPLT->sys_statvfs_vplt))
#   else
#       define VSF_LINUX_APPLET_SYS_STATVFS_VPLT                                \
            ((vsf_linux_sys_statvfs_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_SYS_STATVFS_ENTRY(__NAME)                              \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_SYS_STATVFS_VPLT, __NAME)
#define VSF_LINUX_APPLET_SYS_STATVFS_IMP(...)                                   \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_SYS_STATVFS_VPLT, __VA_ARGS__)

VSF_LINUX_APPLET_SYS_STATVFS_IMP(fstatvfs, int, int fd, struct statvfs *buf) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_STATVFS_ENTRY(fstatvfs)(fd, buf);
}
VSF_LINUX_APPLET_SYS_STATVFS_IMP(statvfs, int, const char *path, struct statvfs *buf) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_STATVFS_ENTRY(statvfs)(path, buf);
}
VSF_LINUX_APPLET_SYS_STATVFS_IMP(fstatvfs64, int, int fd, struct statvfs64 *buf) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_STATVFS_ENTRY(fstatvfs64)(fd, buf);
}
VSF_LINUX_APPLET_SYS_STATVFS_IMP(statvfs64, int, const char *path, struct statvfs64 *buf) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_STATVFS_ENTRY(statvfs64)(path, buf);
}

#else

int fstatvfs(int fd, struct statvfs *buf);
int statvfs(const char *path, struct statvfs *buf);

int fstatvfs64(int fd, struct statvfs64 *buf);
int statvfs64(const char *path, struct statvfs64 *buf);

#endif

#ifdef __cplusplus
}
#endif

#endif
