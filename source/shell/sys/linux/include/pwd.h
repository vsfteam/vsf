#ifndef __VSF_LINUX_PWD_H__
#define __VSF_LINUX_PWD_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "./sys/types.h"
#else
#   include <sys/types.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define getpwuid(__uid)     ((struct passwd *)&__vsf_default_passwd)
#define getpwnam(__name)    ((struct passwd *)&__vsf_default_passwd)

struct passwd {
    char *pw_name;
    char *pw_passwd;
    uid_t pw_uid;
    gid_t pw_gid;
    char *pw_gecos;
    char *pw_dir;
    char *pw_shell;
};

extern const struct passwd __vsf_default_passwd;

#ifdef __cplusplus
}
#endif

#endif
