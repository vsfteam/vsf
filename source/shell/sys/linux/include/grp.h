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

#define setgid(__uid)               (0)
#define getgid()                    ((gid_t)0)
#define getegid()                   ((gid_t)0)
#define initgroups(__user, __gid)   (0)

#ifdef __cplusplus
}
#endif

#endif
