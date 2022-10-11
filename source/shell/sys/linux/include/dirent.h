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
#define rewinddir       VSF_LINUX_WRAPPER(rewinddir)
#define telldir         VSF_LINUX_WRAPPER(telldir)
#define seekdir         VSF_LINUX_WRAPPER(seekdir)
#define closedir        VSF_LINUX_WRAPPER(closedir)
#define scandir         VSF_LINUX_WRAPPER(scandir)
#define alphasort       VSF_LINUX_WRAPPER(alphasort)
#endif

struct dirent {
    long d_ino;
    off_t d_off;
    unsigned short d_reclen;
    unsigned char d_type;
    char *d_name;
};
typedef struct vsf_linux_fd_t DIR;

#if VSF_LINUX_APPLET_USE_DIRENT == ENABLED
typedef struct vsf_linux_dirent_vplt_t {
    vsf_vplt_info_t info;

    DIR * (*opendir)(const char *name);
    DIR * (*fdopendir)(int fd);
    struct dirent * (*readdir)(DIR *dir);
    void (*rewinddir)(DIR *dir);
    long (*telldir)(DIR *dir);
    void (*seekdir)(DIR *dir, long loc);
    int (*closedir)(DIR *dir);
    int (*scandir)(const char *dir, struct dirent ***namelist,
              int (*filter)(const struct dirent *),
              int (*compare)(const struct dirent **, const struct dirent **));
    int (*alphasort)(const struct dirent **a, const struct dirent **b);
    int (*versionsort)(const struct dirent **a, const struct dirent **b);
} vsf_linux_dirent_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_dirent_vplt_t vsf_linux_dirent_vplt;
#   endif
#endif

#if defined(__VSF_APPLET__) && VSF_LINUX_APPLET_USE_DIRENT == ENABLED

#ifndef VSF_LINUX_APPLET_DIRENT_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_DIRENT_VPLT                                     \
            ((vsf_linux_dirent_vplt_t *)(VSF_LINUX_APPLET_VPLT->dirent))
#   else
#       define VSF_LINUX_APPLET_DIRENT_VPLT                                     \
            ((vsf_linux_dirent_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

static inline DIR * opendir(const char *name) {
    return VSF_LINUX_APPLET_DIRENT_VPLT->opendir(name);
}
static inline DIR * fdopendir(int fd) {
    return VSF_LINUX_APPLET_DIRENT_VPLT->fdopendir(fd);
}
static inline struct dirent * readdir(DIR *dir) {
    return VSF_LINUX_APPLET_DIRENT_VPLT->readdir(dir);
}
static inline void rewinddir(DIR *dir) {
    VSF_LINUX_APPLET_DIRENT_VPLT->rewinddir(dir);
}
static inline long telldir(DIR *dir) {
    return VSF_LINUX_APPLET_DIRENT_VPLT->telldir(dir);
}
static inline void seekdir(DIR *dir, long loc) {
    VSF_LINUX_APPLET_DIRENT_VPLT->seekdir(dir, loc);
}
static inline int closedir(DIR *dir) {
    return VSF_LINUX_APPLET_DIRENT_VPLT->closedir(dir);
}
static inline int scandir(const char *dir, struct dirent ***namelist,
              int (*filter)(const struct dirent *),
              int (*compare)(const struct dirent **, const struct dirent **)) {
    return VSF_LINUX_APPLET_DIRENT_VPLT->scandir(dir, namelist, filter, compare);
}
static inline int alphasort(const struct dirent **a, const struct dirent **b) {
    return VSF_LINUX_APPLET_DIRENT_VPLT->alphasort(a, b);
}
static inline int versionsort(const struct dirent **a, const struct dirent **b) {
    return VSF_LINUX_APPLET_DIRENT_VPLT->versionsort(a, b);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_DIRENT

DIR * opendir(const char *name);
DIR * fdopendir(int fd);
struct dirent * readdir(DIR *dir);
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
