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

// fsdata if filesystem is not specified
typedef struct vsf_linux_fsdata_auto_t {
    vk_mal_t *mal;
    vsf_mutex_t mutex;
} vsf_linux_fsdata_auto_t;

#if VSF_LINUX_APPLET_USE_SYS_MOUNT == ENABLED
typedef struct vsf_linux_sys_mount_vplt_t {
    vsf_vplt_info_t info;
} vsf_linux_sys_mount_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_sys_mount_vplt_t vsf_linux_sys_mount_vplt;
#   endif
#endif

#if defined(__VSF_APPLET__) && VSF_LINUX_APPLET_USE_SYS_MOUNT == ENABLED

#ifndef VSF_LINUX_APPLET_SYS_MOUNT_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_SYS_MOUNT_VPLT                                  \
            ((vsf_linux_sys_mount_vplt_t *)(VSF_LINUX_APPLET_VPLT->sys_mount))
#   else
#       define VSF_LINUX_APPLET_SYS_MOUNT_VPLT                                  \
            ((vsf_linux_sys_mount_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_MOUNT

// TODO: implement standard mount after device mapping is implemented
//int mount(const char *source, const char *target,
//   const char *filesystemtype, unsigned long mountflags, const void *data);
int mount(const char *source, const char *target,
   const vk_fs_op_t *filesystem, unsigned long mountflags, const void *data);
int umount(const char *target);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_MOUNT

#ifdef __cplusplus
}
#endif

#endif
