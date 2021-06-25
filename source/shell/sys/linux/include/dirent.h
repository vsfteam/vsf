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
#define readdir         VSF_LINUX_WRAPPER(readdir)
#define closedir        VSF_LINUX_WRAPPER(closedir)
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
struct dirent * readdir(DIR *dir);
int closedir(DIR *dir);

#ifdef __cplusplus
}
#endif

#endif
