#ifndef __VSF_LINUX_SYS_UTSNAME_H__
#define __VSF_LINUX_SYS_UTSNAME_H__

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_CFG_WRAPPER == ENABLED
#define uname                   VSF_LINUX_WRAPPER(uname)
#endif

#ifndef VSF_LINUX_SYSNAME
#   define VSF_LINUX_SYSNAME    "Linux"
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

#define SYS_NMLN                65
struct utsname {
    char sysname[SYS_NMLN];
    char nodename[SYS_NMLN];
    char release[SYS_NMLN];
    char version[SYS_NMLN];
    char machine[SYS_NMLN];
};

#if VSF_LINUX_APPLET_USE_SYS_UTSNAME == ENABLED
typedef struct vsf_linux_sys_utsname_vplt_t {
    vsf_vplt_info_t info;

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(uname);
} vsf_linux_sys_utsname_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_sys_utsname_vplt_t vsf_linux_sys_utsname_vplt;
#   endif
#endif

#if defined(__VSF_APPLET__) && defined(__VSF_APPLET_LIB__) && VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_APPLET_USE_SYS_UTSNAME == ENABLED

#ifndef VSF_LINUX_APPLET_SYS_UTSNAME_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_SYS_UTSNAME_VPLT                                \
            ((vsf_linux_sys_utsname_vplt_t *)(VSF_LINUX_APPLET_VPLT->sys_utsname_vplt))
#   else
#       define VSF_LINUX_APPLET_SYS_UTSNAME_VPLT                                \
            ((vsf_linux_sys_utsname_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_SYS_UTSNAME_ENTRY(__NAME)                              \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_SYS_UTSNAME_VPLT, __NAME)
#define VSF_LINUX_APPLET_SYS_UTSNAME_IMP(...)                                   \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_SYS_UTSNAME_VPLT, __VA_ARGS__)

VSF_LINUX_APPLET_SYS_UTSNAME_IMP(uname, int, struct utsname *name) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SYS_UTSNAME_ENTRY(uname)(name);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_UTSNAME

extern int uname(struct utsname *name);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_SYS_UTSNAME

#ifdef __cplusplus
}
#endif

#endif
