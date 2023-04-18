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

#if VSF_LINUX_CFG_WRAPPER == ENABLED
#   define getgroups        VSF_LINUX_WRAPPER(getgroups)
#   define setgroups        VSF_LINUX_WRAPPER(setgroups)
#   define getgrnam         VSF_LINUX_WRAPPER(getgrnam)
#   define getgrgid         VSF_LINUX_WRAPPER(getgrgid)
#   define getgrnam_r       VSF_LINUX_WRAPPER(getgrnam_r)
#   define getgrgid_r       VSF_LINUX_WRAPPER(getgrgid_r)
#endif

struct group {
    char *gr_name;
    char *gr_passwd;
    gid_t gr_gid;
    char **gr_mem;
};

#if VSF_LINUX_APPLET_USE_GRP == ENABLED
typedef struct vsf_linux_grp_vplt_t {
    vsf_vplt_info_t info;

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(getgroups);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(setgroups);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(getgrnam);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(getgrgid);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(getgrnam_r);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(getgrgid_r);
} vsf_linux_grp_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_grp_vplt_t vsf_linux_grp_vplt;
#   endif
#endif

#if defined(__VSF_APPLET__) && VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_APPLET_USE_GRP == ENABLED

#ifndef VSF_LINUX_APPLET_GRP_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_GRP_VPLT                                        \
            ((vsf_linux_grp_vplt_t *)(VSF_LINUX_APPLET_VPLT->grp_vplt))
#   else
#       define VSF_LINUX_APPLET_GRP_VPLT                                        \
            ((vsf_linux_grp_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_GRP_ENTRY(__NAME)                                      \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_GRP_VPLT, __NAME)
#define VSF_LINUX_APPLET_GRP_IMP(...)                                           \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_GRP_VPLT, __VA_ARGS__)

VSF_LINUX_APPLET_GRP_IMP(getgroups, int, size_t size, gid_t list[]) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_GRP_ENTRY(getgroups)(size, list);
}
VSF_LINUX_APPLET_GRP_IMP(setgroups, int, size_t size, const gid_t *list) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_GRP_ENTRY(setgroups)(size, list);
}
VSF_LINUX_APPLET_GRP_IMP(getgrnam, struct group *, const char *name) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_GRP_ENTRY(getgrnam)(name);
}
VSF_LINUX_APPLET_GRP_IMP(getgrgid, struct group *, gid_t gid) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_GRP_ENTRY(getgrgid)(gid);
}
VSF_LINUX_APPLET_GRP_IMP(getgrnam_r, int, const char *name, struct group *grp, char *buf, size_t buflen, struct group **result) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_GRP_ENTRY(getgrnam_r)(name, grp, buf, buflen, result);
}
VSF_LINUX_APPLET_GRP_IMP(getgrgid_r, int, gid_t gid, struct group *grp, char *buf, size_t buflen, struct group **result) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_GRP_ENTRY(getgrgid_r)(gid, grp, buf, buflen, result);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_GRP

int getgroups(size_t size, gid_t list[]);
int setgroups(size_t size, const gid_t *list);
struct group * getgrnam(const char *name);
struct group * getgrgid(gid_t gid);
int getgrnam_r(const char *name, struct group *grp,
          char *buf, size_t buflen, struct group **result);
int getgrgid_r(gid_t gid, struct group *grp,
          char *buf, size_t buflen, struct group **result);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_GRP

#ifdef __cplusplus
}
#endif

#endif
