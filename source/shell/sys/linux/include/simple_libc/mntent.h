#ifndef __SIMPLE_LIBC_MNTENT_H__
#define __SIMPLE_LIBC_MNTENT_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_LIBC == ENABLED
#   include "./stddef.h"
#else
#   include <stddef.h>
#endif
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_LIBC_CFG_WRAPPER == ENABLED
#define setmntent           VSF_LINUX_LIBC_WRAPPER(setmntent)
#define getmntent           VSF_LINUX_LIBC_WRAPPER(getmntent)
#define addmntent           VSF_LINUX_LIBC_WRAPPER(addmntent)
#define endmntent           VSF_LINUX_LIBC_WRAPPER(endmntent)
#define hasmntopt           VSF_LINUX_LIBC_WRAPPER(hasmntopt)

#if defined(_GNU_SOURCE)
#define getmntent_r         VSF_LINUX_LIBC_WRAPPER(getmntent_r)
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

#if VSF_LINUX_APPLET_USE_LIBC_MNTENT == ENABLED
typedef struct vsf_linux_libc_mntent_vplt_t {
    vsf_vplt_info_t info;

    FILE * (*setmntent)(const char *filename, const char *type);
    struct mntent * (*getmntent)(FILE *stream);
    int (*addmntent)(FILE *stream, const struct mntent *mnt);
    int (*endmntent)(FILE *stream);
    char * (*hasmntopt)(const struct mntent *mnt, const char *opt);

    struct mntent * (*getmntent_r)(FILE *stream, struct mntent *mntbuf, char *buf, int buflen);
} vsf_linux_libc_mntent_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_libc_mntent_vplt_t vsf_linux_libc_mntent_vplt;
#   endif
#endif

#if defined(__VSF_APPLET__) && VSF_LINUX_APPLET_USE_LIBC_MNTENT == ENABLED

#ifndef VSF_LINUX_APPLET_LIBC_MNTENT_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_LIBC_MNTENT_VPLT                                \
            ((vsf_linux_libc_mntent_vplt_t *)(VSF_LINUX_APPLET_VPLT->libc_mntent))
#   else
#       define VSF_LINUX_APPLET_LIBC_MNTENT_VPLT                                \
            ((vsf_linux_libc_mntent_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

static inline FILE * setmntent(const char *filename, const char *type) {
    return VSF_LINUX_APPLET_LIBC_MNTENT_VPLT->setmntent(filename, type);
}
static inline struct mntent * getmntent(FILE *stream) {
    return VSF_LINUX_APPLET_LIBC_MNTENT_VPLT->getmntent(filename);
}
static inline int addmntent(FILE *stream, const struct mntent *mnt) {
    return VSF_LINUX_APPLET_LIBC_MNTENT_VPLT->addmntent(filename, mnt);
}
static inline int endmntent(FILE *stream) {
    return VSF_LINUX_APPLET_LIBC_MNTENT_VPLT->endmntent(stream);
}
static inline char * hasmntopt(const struct mntent *mnt, const char *opt) {
    return VSF_LINUX_APPLET_LIBC_MNTENT_VPLT->hasmntopt(mnt, opt);
}

static inline struct mntent * getmntent_r(FILE *stream, struct mntent *mntbuf, char *buf, int buflen) {
    return VSF_LINUX_APPLET_LIBC_MNTENT_VPLT->getmntent_r(stream, mntbuf, buf, buflen);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_LIBC_MNTENT

FILE * setmntent(const char *filename, const char *type);
struct mntent * getmntent(FILE *stream);
int addmntent(FILE *stream, const struct mntent *mnt);
int endmntent(FILE *stream);
char * hasmntopt(const struct mntent *mnt, const char *opt);

#if defined(_GNU_SOURCE)
struct mntent * getmntent_r(FILE *stream, struct mntent *mntbuf, char *buf, int buflen);
#endif

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_LIBC_MNTENT

#ifdef __cplusplus
}
#endif

#endif
