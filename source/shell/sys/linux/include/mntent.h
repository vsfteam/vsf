#ifndef __VSF_LINUX_MNTENT_H__
#define __VSF_LINUX_MNTENT_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_LIBC == ENABLED
#   include "./simple_libc/stddef.h"
#else
#   include <stddef.h>
#endif
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_CFG_WRAPPER == ENABLED
#define setmntent           VSF_LINUX_WRAPPER(setmntent)
#define getmntent           VSF_LINUX_WRAPPER(getmntent)
#define addmntent           VSF_LINUX_WRAPPER(addmntent)
#define endmntent           VSF_LINUX_WRAPPER(endmntent)
#define hasmntopt           VSF_LINUX_WRAPPER(hasmntopt)

#if defined(_GNU_SOURCE)
#define getmntent_r         VSF_LINUX_WRAPPER(getmntent_r)
#endif
#endif

struct mntent {
    char *mnt_fsname;
    char *mnt_dir;
    char *mnt_type;
    char *mnt_opts;
    int mnt_freq;
    int mnt_passno;
};

#if VSF_LINUX_APPLET_USE_MNTENT == ENABLED
typedef struct vsf_linux_mntent_vplt_t {
    vsf_vplt_info_t info;

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(setmntent);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(getmntent);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(addmntent);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(endmntent);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(hasmntopt);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(getmntent_r);
} vsf_linux_mntent_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_mntent_vplt_t vsf_linux_mntent_vplt;
#   endif
#endif

#if     defined(__VSF_APPLET__) && (defined(__VSF_APPLET_LIB__) || defined(__VSF_APPLET_LINUX_MNTENT_LIB__))\
    &&  VSF_LINUX_APPLET_USE_MNTENT == ENABLED

#ifndef VSF_LINUX_APPLET_MNTENT_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_MNTENT_VPLT                                     \
            ((vsf_linux_mntent_vplt_t *)(VSF_LINUX_APPLET_VPLT->mntent_vplt))
#   else
#       define VSF_LINUX_APPLET_MNTENT_VPLT                                     \
            ((vsf_linux_mntent_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_MNTENT_ENTRY(__NAME)                                   \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_MNTENT_VPLT, __NAME)
#define VSF_LINUX_APPLET_MNTENT_IMP(...)                                        \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_MNTENT_VPLT, __VA_ARGS__)

VSF_LINUX_APPLET_MNTENT_IMP(setmntent, FILE *, const char *filename, const char *type) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_MNTENT_ENTRY(setmntent)(filename, type);
}
VSF_LINUX_APPLET_MNTENT_IMP(getmntent, struct mntent *, FILE *stream) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_MNTENT_ENTRY(getmntent)(stream);
}
VSF_LINUX_APPLET_MNTENT_IMP(addmntent, int, FILE *stream, const struct mntent *mnt) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_MNTENT_ENTRY(addmntent)(stream, mnt);
}
VSF_LINUX_APPLET_MNTENT_IMP(endmntent, int, FILE *stream) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_MNTENT_ENTRY(endmntent)(stream);
}
VSF_LINUX_APPLET_MNTENT_IMP(hasmntopt, char *, const struct mntent *mnt, const char *opt) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_MNTENT_ENTRY(hasmntopt)(mnt, opt);
}
VSF_LINUX_APPLET_MNTENT_IMP(getmntent_r, struct mntent *, FILE *stream, struct mntent *mntbuf, char *buf, int buflen) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_MNTENT_ENTRY(getmntent_r)(stream, mntbuf, buflen);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_MNTENT

FILE * setmntent(const char *filename, const char *type);
struct mntent * getmntent(FILE *stream);
int addmntent(FILE *stream, const struct mntent *mnt);
int endmntent(FILE *stream);
char * hasmntopt(const struct mntent *mnt, const char *opt);

#if defined(_GNU_SOURCE)
struct mntent * getmntent_r(FILE *stream, struct mntent *mntbuf, char *buf, int buflen);
#endif

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_MNTENT

#ifdef __cplusplus
}
#endif

#endif
