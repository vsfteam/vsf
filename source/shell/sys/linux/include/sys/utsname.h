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

#if VSF_LINUX_APPLET_USE_SYS_UTSNAME == ENABLED
typedef struct vsf_linux_sys_utsname_vplt_t {
    vsf_vplt_info_t info;
} vsf_linux_sys_utsname_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_sys_utsname_vplt_t vsf_linux_sys_utsname_vplt;
#   endif
#endif

#if defined(__VSF_APPLET__) && VSF_LINUX_APPLET_USE_SYS_UTSNAME == ENABLED

#ifndef VSF_LINUX_APPLET_SYS_UTSNAME_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_SYS_UTSNAME_VPLT                                \
            ((vsf_linux_sys_utsname_vplt_t *)(VSF_LINUX_APPLET_VPLT->sys_utsname))
#   else
#       define VSF_LINUX_APPLET_SYS_UTSNAME_VPLT                                \
            ((vsf_linux_sys_utsname_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_UTSNAME

extern int uname(struct utsname *name);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_UTSNAME

#ifdef __cplusplus
}
#endif

#endif
