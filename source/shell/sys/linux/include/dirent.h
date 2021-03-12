#ifndef __DIRENT_H__
#define __DIRENT_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "./sys/types.h"
#else
#   include <sys/types.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define opendir         __vsf_linux_opendir
#define readdir         __vsf_linux_readdir
#define closedir        __vsf_linux_closedir

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
