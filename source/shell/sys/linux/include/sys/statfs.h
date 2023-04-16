#ifndef __VSF_LINUX_SYS_STATFS_H__
#define __VSF_LINUX_SYS_STATFS_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../sys/types.h"
#else
#   include <sys/types.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_CFG_WRAPPER == ENABLED
#define statfs              VSF_LINUX_WRAPPER(statfs)
#define fstatfs             VSF_LINUX_WRAPPER(fstatfs)
#endif

typedef int                 fsid_t;

struct statfs {
    short                   f_type;
    short                   f_bsize;
    fsblkcnt_t              f_blocks;
    fsblkcnt_t              f_bfree;
    fsblkcnt_t              f_bavail;

    fsblkcnt_t              f_files;
    fsblkcnt_t              f_ffree;
    fsid_t                  f_fsid;
    short                   f_namelen;
    short                   f_frsize;
    short                   f_flags;
};

#if VSF_LINUX_APPLET_USE_SYS_STATFS == ENABLED
typedef struct vsf_linux_sys_statfs_vplt_t {
    vsf_vplt_info_t info;

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(statfs);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fstatfs);
} vsf_linux_sys_statfs_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_sys_statfs_vplt_t vsf_linux_sys_statfs_vplt;
#   endif
#endif

#if defined(__VSF_APPLET__) && VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_APPLET_USE_SYS_STATFS == ENABLED

#ifndef VSF_LINUX_APPLET_SYS_STATFS_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_SYS_STATFS_VPLT                                 \
            ((vsf_linux_sys_statfs_vplt_t *)(VSF_LINUX_APPLET_VPLT->sys_statfs_vplt))
#   else
#       define VSF_LINUX_APPLET_SYS_STATFS_VPLT                                 \
            ((vsf_linux_sys_statfs_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_SYS_STATFS_ENTRY(__NAME)                               \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_SYS_STATFS_VPLT, __NAME)
#define VSF_LINUX_APPLET_SYS_STATFS_IMP(...)                                    \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_SYS_STATFS_VPLT, __VA_ARGS__)

VSF_LINUX_APPLET_SYS_STATFS_IMP(statfs, int, const char *path, struct statfs *buf) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_STATFS_ENTRY(statfs)(path, buf);
}
VSF_LINUX_APPLET_SYS_STATFS_IMP(fstatfs, int, int fd, struct statfs *buf) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_STATFS_ENTRY(fstatfs)(fd, buf);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_STATFS

int statfs(const char *path, struct statfs *buf);
int fstatfs(int fd, struct statfs *buf);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_STATFS

#ifdef __cplusplus
}
#endif

#endif      // __VSF_LINUX_SYS_STATFS_H__
