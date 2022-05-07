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

DIR * opendir(const char *name);
DIR *fdopendir(int fd);
struct dirent * readdir(DIR *dir);
void rewinddir(DIR *dir);
long telldir(DIR *dir);
void seekdir(DIR *dir, long loc);
int closedir(DIR *dir);
int scandir(const char *dir, struct dirent ***namelist,
              int (*filter)(const struct dirent *),
              int (*compare)(const struct dirent **, const struct dirent **));
int alphasort(const struct dirent **a, const struct dirent **b);

#ifdef __cplusplus
}
#endif

#endif
