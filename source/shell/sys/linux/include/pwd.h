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

struct passwd {
    char *pw_name;
    char *pw_passwd;
    uid_t pw_uid;
    gid_t pw_gid;
    char *pw_gecos;
    char *pw_dir;
    char *pw_shell;
};

#if VSF_LINUX_APPLET_USE_PWD == ENABLED
typedef struct vsf_linux_pwd_vplt_t {
    vsf_vplt_info_t info;

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(getpwuid);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(getpwnam);
} vsf_linux_pwd_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_pwd_vplt_t vsf_linux_pwd_vplt;
#   endif
#endif

#if defined(__VSF_APPLET__) && defined(__VSF_APPLET_LIB__) && VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_APPLET_USE_PWD == ENABLED

#ifndef VSF_LINUX_APPLET_PWD_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_PWD_VPLT                                        \
            ((vsf_linux_pwd_vplt_t *)(VSF_LINUX_APPLET_VPLT->pwd_vplt))
#   else
#       define VSF_LINUX_APPLET_PWD_VPLT                                        \
            ((vsf_linux_pwd_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_PWD_ENTRY(__NAME)                                      \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_PWD_VPLT, __NAME)
#define VSF_LINUX_APPLET_PWD_IMP(...)                                           \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_PWD_VPLT, __VA_ARGS__)

VSF_LINUX_APPLET_PWD_IMP(getpwuid, struct passwd *, uid_t uid) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PWD_ENTRY(getpwuid)(uid);
}
VSF_LINUX_APPLET_PWD_IMP(getpwnam, struct passwd *, const char *name) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_PWD_ENTRY(getpwnam)(name);
}

#else

struct passwd * getpwuid(uid_t uid);
struct passwd * getpwnam(const char *name);

#endif

static inline int getpwuid_r(uid_t uid, struct passwd *pwd,
        char *buffer, size_t bufsize, struct passwd **result)
{
    return 0;
}

#ifdef __cplusplus
}
#endif

#endif
