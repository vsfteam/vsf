#ifndef __SIMPLE_LIBC_GLOB_H__
#define __SIMPLE_LIBC_GLOB_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_LIBC == ENABLED
#   include "./stddef.h"
#else
#   include <stddef.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_LIBC_CFG_WRAPPER == ENABLED
#define glob                VSF_LINUX_LIBC_WRAPPER(glob)
#define globfree            VSF_LINUX_LIBC_WRAPPER(globfree)
#endif

// flags

#define GLOB_ERR            (1 << 0)
#define GLOB_MARK           (1 << 1)
#define GLOB_NOSORT         (1 << 2)
#define GLOB_DOOFFS         (1 << 3)
#define GLOB_NOCHECK        (1 << 4)
#define GLOB_APPEND         (1 << 5)
#define GLOB_NOESCAPE       (1 << 6)
#define GLOB_PERIOD         (1 << 7)
#define GLOB_MAGCHAR        (1 << 8)
#define GLOB_ALTDIRFUNC     (1 << 9)
#define GLOB_BRACE          (1 << 10)
#define GLOB_NOMAGIC        (1 << 11)
#define GLOB_TILDE          (1 << 12)
#define GLOB_ONLYDIR        (1 << 13)
#define GLOB_TILDE_CHECK    (1 << 14)

// error

#define GLOB_NOSPACE        1
#define GLOB_ABORTED        2
#define GLOB_NOMATCH        3
#define GLOB_NOSYS          4

typedef struct {
    size_t  gl_pathc;
    char  **gl_pathv;
    size_t  gl_offs;
    int     gl_flags;

    void (*gl_closedir)(void *);
    struct dirent *(*gl_readdir)(void *);
    void *(*gl_opendir)(const char *);
    int (*gl_lstat)(const char *, struct stat *);
    int (*gl_stat)(const char *, struct stat *);
} glob_t;

#if VSF_LINUX_APPLET_USE_LIBC_GLOB == ENABLED
typedef struct vsf_linux_libc_glob_vplt_t {
    vsf_vplt_info_t info;

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(glob);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(globfree);
} vsf_linux_libc_glob_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_libc_glob_vplt_t vsf_linux_libc_glob_vplt;
#   endif
#endif

#if defined(__VSF_APPLET__) && VSF_LINUX_APPLET_USE_LIBC_GLOB == ENABLED

#ifndef VSF_LINUX_APPLET_LIBC_GLOB_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_LIBC_GLOB_VPLT                                  \
            ((vsf_linux_libc_glob_vplt_t *)(VSF_LINUX_APPLET_VPLT->libc_glob))
#   else
#       define VSF_LINUX_APPLET_LIBC_GLOB_VPLT                                  \
            ((vsf_linux_libc_glob_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_LIBC_GLOB_ENTRY(__NAME)                                \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_LIBC_GLOB_VPLT, __NAME)
#define VSF_LINUX_APPLET_LIBC_GLOB_IMP(...)                                     \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_LIBC_GLOB_VPLT, __VA_ARGS__)

VSF_LINUX_APPLET_LIBC_GLOB_IMP(glob, int, const char *path, int flags, int (*errfunc)(const char *path, int err), glob_t *g) {
    return VSF_LINUX_APPLET_LIBC_GLOB_ENTRY(glob)(path, flags, errfunc, g);
}
VSF_LINUX_APPLET_LIBC_GLOB_IMP(globfree, int, glob_t *g) {
    return VSF_LINUX_APPLET_LIBC_GLOB_ENTRY(globfree)(g);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_LIBC_GLOB

int glob(const char *path, int flags, int (*errfunc)(const char *path, int err), glob_t *g);
void globfree(glob_t *g);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_LIBC_GLOB

#ifdef __cplusplus
}
#endif

#endif
