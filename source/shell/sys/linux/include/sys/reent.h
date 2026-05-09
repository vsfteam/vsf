#ifndef __VSF_LINUX_SYS_REENT_H__
#define __VSF_LINUX_SYS_REENT_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

struct _reent {
    int _errno;
};

#define __errno_r(r)  ((r)->_errno)

extern struct _reent *__vsf_linux_getreent(void);
#define __getreent()  __vsf_linux_getreent()

#if VSF_LINUX_APPLET_USE_REENT == ENABLED
typedef struct vsf_linux_reent_vplt_t {
    vsf_vplt_info_t info;

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__vsf_linux_getreent);
} vsf_linux_reent_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_reent_vplt_t vsf_linux_reent_vplt;
#   endif
#endif

#if     defined(__VSF_APPLET__) && (defined(__VSF_APPLET_LIB__) || defined(__VSF_APPLET_LINUX_REENT_LIB__))\
    &&  VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_APPLET_USE_REENT == ENABLED

#ifndef VSF_LINUX_APPLET_REENT_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_REENT_VPLT                                      \
            ((vsf_linux_reent_vplt_t *)(VSF_LINUX_APPLET_VPLT->reent_vplt))
#   else
#       define VSF_LINUX_APPLET_REENT_VPLT                                      \
            ((vsf_linux_reent_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_REENT_ENTRY(__NAME)                                    \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_REENT_VPLT, __NAME)
#define VSF_LINUX_APPLET_REENT_IMP(...)                                         \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_REENT_VPLT, __VA_ARGS__)

VSF_LINUX_APPLET_REENT_IMP(__vsf_linux_getreent, struct _reent *, void) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_REENT_ENTRY(__vsf_linux_getreent)();
}

#else

extern struct _reent *__vsf_linux_getreent(void);

#endif

#ifdef __cplusplus
}
#endif

#endif  // __VSF_LINUX_SYS_REENT_H__
