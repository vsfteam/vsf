#ifndef __VSF_LINUX_GRP_H__
#define __VSF_LINUX_GRP_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "./sys/types.h"
#else
#   include <sys/types.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct group {
    char *gr_name;
    char *gr_passwd;
    gid_t gr_gid;
    char **gr_mem;
};

#if 1
#   define getgrnam(__name)         (NULL)
#   define getgrgid(__gid)          (NULL)
#   define getgrnam_r(...)          (-1)
#   define getgrgid_r(...)          (-1)
#else
struct group * getgrnam(const char *name);
struct group * getgrgid(gid_t gid);
int getgrnam_r(const char *name, struct group *grp,
          char *buf, size_t buflen, struct group **result);
int getgrgid_r(gid_t gid, struct group *grp,
          char *buf, size_t buflen, struct group **result);
#endif

#ifdef __cplusplus
}
#endif

#endif
