#ifndef __VSF_LINUX_RESOLV_H__
#define __VSF_LINUX_RESOLV_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_LIBC == ENABLED
#   include "./simple_libc/stddef.h"
#else
#   include <stddef.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_CFG_WRAPPER == ENABLED
#define __b64_ntop          VSF_LINUX_WRAPPER(__b64_ntop)
#define __b64_pton          VSF_LINUX_WRAPPER(__b64_pton)
#endif

#define b64_ntop            __b64_ntop
#define b64_pton            __b64_pton

struct res_sym {
    int number;
    char *name;
    char *humanname;
};

#if VSF_LINUX_APPLET_USE_RESOLV == ENABLED
typedef struct vsf_linux_resolv_vplt_t {
    vsf_vplt_info_t info;

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__b64_ntop);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(__b64_pton);
} vsf_linux_resolv_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_resolv_vplt_t vsf_linux_resolv_vplt;
#   endif
#endif

#if     defined(__VSF_APPLET__) && (defined(__VSF_APPLET_LIB__) || defined(__VSF_APPLET_LINUX_RESOLV_LIB__))\
    &&  VSF_LINUX_APPLET_USE_RESOLV == ENABLED

#ifndef VSF_LINUX_APPLET_RESOLV_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_RESOLV_VPLT                                     \
            ((vsf_linux_resolv_vplt_t *)(VSF_LINUX_APPLET_VPLT->resolv_vplt))
#   else
#       define VSF_LINUX_APPLET_RESOLV_VPLT                                     \
            ((vsf_linux_resolv_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_RESOLV_ENTRY(__NAME)                                   \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_RESOLV_VPLT, __NAME)
#define VSF_LINUX_APPLET_RESOLV_IMP(...)                                        \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_RESOLV_VPLT, __VA_ARGS__)

VSF_LINUX_APPLET_RESOLV_IMP(__b64_ntop, int, const unsigned char *src, int srclen, char *dst, size_t dstlen) {
    return VSF_LINUX_APPLET_RESOLV_ENTRY(__b64_ntop)(src, srclen, dst, dstlen);
}
VSF_LINUX_APPLET_RESOLV_IMP(__b64_pton, int, char *src, unsigned char *dst, size_t dstlen) {
    return VSF_LINUX_APPLET_RESOLV_ENTRY(__b64_pton)(src, dst, dstlen);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_RESOLV

int b64_ntop(const unsigned char *src, int srclen, char *dst, size_t dstlen);
int b64_pton(char *src, unsigned char *dst, size_t dstlen);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_RESOLV

#ifdef __cplusplus
}
#endif

#endif
