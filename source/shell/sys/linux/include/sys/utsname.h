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
struct utsname {
    char sysname[sizeof(VSF_LINUX_SYSNAME)];
    char release[sizeof(VSF_LINUX_RELEASE)];
};

extern int uname(struct utsname *name);

#ifdef __cplusplus
}
#endif

#endif
