#ifndef __VSF_LINUX_VERSION_H__
#define __VSF_LINUX_VERSION_H__

#ifdef __cplusplus
extern "C" {
#endif

#define KERNEL_VERSION(__MAJOR, __MINOR, __PATCH)       (((__MAJOR) << 16) + ((__MINOR) << 8) + (__PATCH))

#define LINUX_VERSION_CODE                              KERNEL_VERSION(0, 0, 0)

#ifdef __cplusplus
}
#endif

#endif
