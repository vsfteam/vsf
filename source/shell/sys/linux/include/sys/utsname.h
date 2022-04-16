#ifndef __VSF_LINUX_UTSNAME_H__
#define __VSF_LINUX_UTSNAME_H__

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_CFG_WRAPPER == ENABLED
#define uname                   VSF_LINUX_WRAPPER(uname)
#endif

#ifndef VSF_LINUX_SYSNAME
#   define VSF_LINUX_SYSNAME    "vsf"
#endif
#ifndef VSF_LINUX_RELEASE
#   define VSF_LINUX_RELEASE    "0.1.0"
#endif
#ifndef VSF_LINUX_VERSION
#   define VSF_LINUX_VERSION    "unknown"
#endif
#ifndef VSF_LINUX_MACHINE
#   define VSF_LINUX_MACHINE    "unknown"
#endif
#ifndef VSF_LINUX_NODENAME
#   define VSF_LINUX_NODENAME   "unknown"
#endif
struct utsname {
    char sysname[sizeof(VSF_LINUX_SYSNAME)];
    char nodename[sizeof(VSF_LINUX_NODENAME)];
    char release[sizeof(VSF_LINUX_RELEASE)];
    char version[sizeof(VSF_LINUX_VERSION)];
    char machine[sizeof(VSF_LINUX_MACHINE)];
};

extern int uname(struct utsname *name);

#ifdef __cplusplus
}
#endif

#endif
