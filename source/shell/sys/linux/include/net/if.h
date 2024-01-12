#ifndef __VSF_LINUX_NET_IF_H__
#define __VSF_LINUX_NET_IF_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#define IFNAMSIZ                16
#define	IFALIASZ                256

#if VSF_LINUX_APPLET_USE_NET_IF == ENABLED
typedef struct vsf_linux_net_if_vplt_t {
    vsf_vplt_info_t info;

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(if_nametoindex);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(if_indextoname);
} vsf_linux_net_if_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_net_if_vplt_t vsf_linux_net_if_vplt;
#   endif
#endif

#if     defined(__VSF_APPLET__) && (defined(__VSF_APPLET_LIB__) || defined(__VSF_APPLET_LINUX_NET_IF_LIB__))\
    &&  VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_APPLET_USE_NET_IF == ENABLED

#ifndef VSF_LINUX_APPLET_NET_IF_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_NET_IF_VPLT                                     \
            ((vsf_linux_net_if_vplt_t *)(VSF_LINUX_APPLET_VPLT->net_if_vplt))
#   else
#       define VSF_LINUX_APPLET_NET_IF_VPLT                                     \
            ((vsf_linux_net_if_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_NET_IF_ENTRY(__NAME)                                   \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_NET_IF_VPLT, __NAME)
#define VSF_LINUX_APPLET_NET_IF_IMP(...)                                        \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_NET_IF_VPLT, __VA_ARGS__)

VSF_LINUX_APPLET_NET_IF_IMP(if_nametoindex, unsigned int, const char *ifname) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_NET_IF_ENTRY(if_nametoindex)(ifname);
}
VSF_LINUX_APPLET_NET_IF_IMP(if_indextoname, char *, unsigned int ifindex, char *ifname) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_NET_IF_ENTRY(if_indextoname)(ifindex, ifname);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_NET_IF

unsigned int if_nametoindex(const char *ifname);
char * if_indextoname(unsigned int ifindex, char *ifname);

#endif

#endif      // __VSF_LINUX_NET_IF_H__
