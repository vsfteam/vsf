#ifndef __VSF_LINUX_SYS_XATTR_H__
#define __VSF_LINUX_SYS_XATTR_H__

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
#define removexattr             VSF_LINUX_WRAPPER(removexattr)
#define lremovexattr            VSF_LINUX_WRAPPER(lremovexattr)
#define fremovexattr            VSF_LINUX_WRAPPER(fremovexattr)
#define setxattr                VSF_LINUX_WRAPPER(setxattr)
#define lsetxattr               VSF_LINUX_WRAPPER(lsetxattr)
#define fsetxattr               VSF_LINUX_WRAPPER(fsetxattr)
#define getxattr                VSF_LINUX_WRAPPER(getxattr)
#define lgetxattr               VSF_LINUX_WRAPPER(lgetxattr)
#define fgetxattr               VSF_LINUX_WRAPPER(fgetxattr)
#endif

#if VSF_LINUX_APPLET_USE_SYS_XATTR == ENABLED
typedef struct vsf_linux_sys_xattr_vplt_t {
    vsf_vplt_info_t info;

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(removexattr);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(lremovexattr);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fremovexattr);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(setxattr);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(lsetxattr);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fsetxattr);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(getxattr);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(lgetxattr);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fgetxattr);
} vsf_linux_sys_xattr_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_sys_xattr_vplt_t vsf_linux_sys_xattr_vplt;
#   endif
#endif

#if     defined(__VSF_APPLET__) && (defined(__VSF_APPLET_LIB__) || defined(__VSF_APPLET_LINUX_SYS_XATTR_LIB__))\
    &&  VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_APPLET_USE_SYS_XATTR == ENABLED

#ifndef VSF_LINUX_APPLET_SYS_XATTR_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_SYS_XATTR_VPLT                                  \
            ((vsf_linux_sys_xattr_vplt_t *)(VSF_LINUX_APPLET_VPLT->sys_xattr_vplt))
#   else
#       define VSF_LINUX_APPLET_SYS_XATTR_VPLT                                  \
            ((vsf_linux_sys_xattr_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_SYS_XATTR_ENTRY(__NAME)                                \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_SYS_XATTR_VPLT, __NAME)
#define VSF_LINUX_APPLET_SYS_XATTR_IMP(...)                                     \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_SYS_XATTR_VPLT, __VA_ARGS__)

VSF_LINUX_APPLET_SYS_XATTR_IMP(removexattr, int, const char *path, const char *name) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_XATTR_ENTRY(removexattr)(path, name);
}
VSF_LINUX_APPLET_SYS_XATTR_IMP(lremovexattr, int, const char *path, const char *name) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_XATTR_ENTRY(lremovexattr)(path, name);
}
VSF_LINUX_APPLET_SYS_XATTR_IMP(fremovexattr, int, int fd, const char *name) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_XATTR_ENTRY(fremovexattr)(fd, name);
}

VSF_LINUX_APPLET_SYS_XATTR_IMP(setxattr, int, const char *path, const char *name, const void *value, size_t size, int flags) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_XATTR_ENTRY(setxattr)(path, name, value, size, flags);
}
VSF_LINUX_APPLET_SYS_XATTR_IMP(lsetxattr, int, const char *path, const char *name, const void *value, size_t size, int flags) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_XATTR_ENTRY(lsetxattr)(path, name, value, size, flags);
}
VSF_LINUX_APPLET_SYS_XATTR_IMP(fsetxattr, int, int fd, const char *name, const void *value, size_t size, int flags) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_XATTR_ENTRY(fsetxattr)(fd, name, value, size, flags);
}

VSF_LINUX_APPLET_SYS_XATTR_IMP(removexattr, ssize_t, const char *path, const char *name, void *value, size_t size) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_XATTR_ENTRY(removexattr)(path, name, value, size);
}
VSF_LINUX_APPLET_SYS_XATTR_IMP(lremovexattr, ssize_t, const char *path, const char *name, void *value, size_t size) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_XATTR_ENTRY(lremovexattr)(path, name, value, size);
}
VSF_LINUX_APPLET_SYS_XATTR_IMP(fremovexattr, ssize_t, int fd, const char *name, void *value, size_t size) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_XATTR_ENTRY(fremovexattr)(fd, name, value, size);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_XATTR

int removexattr(const char *path, const char *name);
int lremovexattr(const char *path, const char *name);
int fremovexattr(int fd, const char *name);

int setxattr(const char *path, const char *name, const void *value, size_t size, int flags);
int lsetxattr(const char *path, const char *name, const void *value, size_t size, int flags);
int fsetxattr(int fd, const char *name, const void *value, size_t size, int flags);

ssize_t getxattr(const char *path, const char *name, void *value, size_t size);
ssize_t lgetxattr(const char *path, const char *name, void *value, size_t size);
ssize_t fgetxattr(int fd, const char *name, void *value, size_t size);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_XATTR

#ifdef __cplusplus
}
#endif

#endif
