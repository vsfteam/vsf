#ifndef __VSF_LINUX_DIRENT_H__
#define __VSF_LINUX_DIRENT_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "./sys/types.h"
#else
#   include <sys/types.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_CFG_WRAPPER == ENABLED
#define opendir         VSF_LINUX_WRAPPER(opendir)
#define fdopendir       VSF_LINUX_WRAPPER(fdopendir)
#define readdir         VSF_LINUX_WRAPPER(readdir)
#define readdir64       VSF_LINUX_WRAPPER(readdir64)
#define readdir_r       VSF_LINUX_WRAPPER(readdir_r)
#define readdir64_r     VSF_LINUX_WRAPPER(readdir64_r)
#define rewinddir       VSF_LINUX_WRAPPER(rewinddir)
#define telldir         VSF_LINUX_WRAPPER(telldir)
#define seekdir         VSF_LINUX_WRAPPER(seekdir)
#define closedir        VSF_LINUX_WRAPPER(closedir)
#define scandir         VSF_LINUX_WRAPPER(scandir)
#define alphasort       VSF_LINUX_WRAPPER(alphasort)
#endif

struct dirent {
    ino_t d_ino;
    off_t d_off;
    unsigned short d_reclen;
    unsigned char d_type;
    char d_name[256];
};
struct dirent64 {
    ino64_t d_ino;
    off64_t d_off;
    unsigned short d_reclen;
    unsigned char d_type;
    char d_name[256];
};
typedef struct vsf_linux_fd_t DIR;

#if VSF_LINUX_APPLET_USE_DIRENT == ENABLED
typedef struct vsf_linux_dirent_vplt_t {
    vsf_vplt_info_t info;

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(opendir);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(fdopendir);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(readdir);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(readdir64);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(readdir_r);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(readdir64_r);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(rewinddir);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(telldir);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(seekdir);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(closedir);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(scandir);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(alphasort);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(versionsort);
} vsf_linux_dirent_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_dirent_vplt_t vsf_linux_dirent_vplt;
#   endif
#endif

#if     defined(__VSF_APPLET__) && (defined(__VSF_APPLET_LIB__) || defined(__VSF_APPLET_LINUX_DIRENT_LIB__))\
    &&  VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_APPLET_USE_DIRENT == ENABLED

#ifndef VSF_LINUX_APPLET_DIRENT_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_DIRENT_VPLT                                     \
            ((vsf_linux_dirent_vplt_t *)(VSF_LINUX_APPLET_VPLT->dirent_vplt))
#   else
#       define VSF_LINUX_APPLET_DIRENT_VPLT                                     \
            ((vsf_linux_dirent_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_DIRENT_ENTRY(__NAME)                                   \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_DIRENT_VPLT, __NAME)
#define VSF_LINUX_APPLET_DIRENT_IMP(...)                                        \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_DIRENT_VPLT, __VA_ARGS__)

VSF_LINUX_APPLET_DIRENT_IMP(opendir, DIR *, const char *name) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_DIRENT_ENTRY(opendir)(name);
}
VSF_LINUX_APPLET_DIRENT_IMP(fdopendir, DIR *, int fd) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_DIRENT_ENTRY(fdopendir)(fd);
}
VSF_LINUX_APPLET_DIRENT_IMP(readdir, struct dirent *, DIR *dir) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_DIRENT_ENTRY(readdir)(dir);
}
VSF_LINUX_APPLET_DIRENT_IMP(readdir64, struct dirent64 *, DIR *dir) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_DIRENT_ENTRY(readdir64)(dir);
}
VSF_LINUX_APPLET_DIRENT_IMP(readdir_r, int, DIR *dirp, struct dirent *entry, struct dirent **result) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_DIRENT_ENTRY(readdir_r)(dirp, entry, result);
}
VSF_LINUX_APPLET_DIRENT_IMP(readdir64_r, int, DIR *dirp, struct dirent64 *entry, struct dirent64 **result) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_DIRENT_ENTRY(readdir64_r)(dirp, entry, result);
}
VSF_LINUX_APPLET_DIRENT_IMP(rewinddir, void, DIR *dir) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_LINUX_APPLET_DIRENT_ENTRY(rewinddir)(dir);
}
VSF_LINUX_APPLET_DIRENT_IMP(telldir, long, DIR *dir) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_DIRENT_ENTRY(telldir)(dir);
}
VSF_LINUX_APPLET_DIRENT_IMP(seekdir, void, DIR *dir, long loc) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_LINUX_APPLET_DIRENT_ENTRY(seekdir)(dir, loc);
}
VSF_LINUX_APPLET_DIRENT_IMP(closedir, int, DIR *dir) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_DIRENT_ENTRY(closedir)(dir);
}
VSF_LINUX_APPLET_DIRENT_IMP(scandir, int, const char *dir, struct dirent ***namelist, int (*filter)(const struct dirent *), int (*compare)(const struct dirent **, const struct dirent **)) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_DIRENT_ENTRY(scandir)(dir, namelist, filter, compare);
}
VSF_LINUX_APPLET_DIRENT_IMP(alphasort, int, const struct dirent **a, const struct dirent **b) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_DIRENT_ENTRY(alphasort)(a, b);
}
VSF_LINUX_APPLET_DIRENT_IMP(versionsort, int, const struct dirent **a, const struct dirent **b) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_DIRENT_ENTRY(versionsort)(a, b);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_DIRENT

DIR * opendir(const char *name);
DIR * fdopendir(int fd);
struct dirent * readdir(DIR *dir);
struct dirent64 * readdir64(DIR *dir);
int readdir_r(DIR *dirp, struct dirent *entry, struct dirent **result);
int readdir64_r(DIR *dirp, struct dirent64 *entry, struct dirent64 **result);
void rewinddir(DIR *dir);
long telldir(DIR *dir);
void seekdir(DIR *dir, long loc);
int closedir(DIR *dir);
int scandir(const char *dir, struct dirent ***namelist,
              int (*filter)(const struct dirent *),
              int (*compare)(const struct dirent **, const struct dirent **));
int alphasort(const struct dirent **a, const struct dirent **b);
int versionsort(const struct dirent **a, const struct dirent **b);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_DIRENT

#ifdef __cplusplus
}
#endif

#endif
