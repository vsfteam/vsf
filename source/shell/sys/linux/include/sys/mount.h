#ifndef __VSF_LINUX_MOUNT_H__
#define __VSF_LINUX_MOUNT_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_CFG_WRAPPER == ENABLED
#define mount               VSF_LINUX_WRAPPER(mount)
#define umount              VSF_LINUX_WRAPPER(umount)
#endif

// TODO: implement standard mount after device mapping is implemented
//int mount(const char *source, const char *target,
//   const char *filesystemtype, unsigned long mountflags, const void *data);
int mount(const char *source, const char *target,
   const vk_fs_op_t *filesystem, unsigned long mountflags, const void *data);
int umount(const char *target);

#ifdef __cplusplus
}
#endif

#endif
