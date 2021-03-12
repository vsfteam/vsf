#ifndef __STAT_H__
#define __STAT_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../sys/types.h"
#else
#   include <sys/types.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define stat            __vsf_linux_stat

#define S_IFDIR         1

// TODO:
#define S_ISREG(__MODE) !((__MODE) & VSF_FILE_ATTR_DIRECTORY)
#define S_ISDIR(__MODE) ((__MODE) & VSF_FILE_ATTR_DIRECTORY)

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
