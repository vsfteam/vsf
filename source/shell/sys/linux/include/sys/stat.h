#ifndef __STAT_H__
#define __STAT_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../sys/types.h"
#   include "../time.h"
#else
#   include <sys/types.h>
#   include <time.h>
#endif

// for fs constants
#include "component/vsf_component.h"

#ifdef __cplusplus
extern "C" {
#endif

#define stat            __vsf_linux_stat

#define S_IFDIR         VSF_FILE_ATTR_DIRECTORY
#define S_IFREG         VSF_FILE_ATTR_USER
#define S_IFMT          (S_IFDIR | S_IFREG)

// TODO:
#define S_ISREG(__MODE) !((__MODE) & S_IFDIR)
#define S_ISDIR(__MODE) ((__MODE) & S_IFDIR)

struct stat {
    mode_t      st_mode;
    off_t       st_size;
    time_t      st_atime;
    time_t      st_mtime;
    time_t      st_ctime;
};

int stat(const char *pathname, struct stat *buf);

#ifdef __cplusplus
}
#endif

#endif
