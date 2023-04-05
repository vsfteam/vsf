#ifndef __VSF_LINUX_IFADDRS_H__
#define __VSF_LINUX_IFADDRS_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../sys/types.h"
#   include "../sys/socket.h"
#else
#   include <sys/types.h>
#   include <sys/socket.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

enum net_device_flags {
    IFF_UP                  = 1 << 0,
    IFF_BROADCAST           = 1 << 1,
    IFF_DEBUG               = 1 << 2,
    IFF_LOOPBACK            = 1 << 3,
    IFF_RUNNING             = 1 << 6,
};

struct ifaddrs {
    struct ifaddrs          *ifa_next;
    char                    *ifa_name;
    unsigned int            ifa_flags;

    struct sockaddr         *ifa_addr;
    struct sockaddr         *ifa_netmask;
    union {
        struct sockaddr     *ifu_broadaddr;
        struct sockaddr     *ifs_dstaddr;
    } ifa_ifu;
#ifndef ifa_broadaddr
#   define ifa_broadaddr    ifa_ifu.ifu_broadaddr
#endif
#ifndef ifa_dstaddr
#   define ifa_dstaddr      ifa_ifu.ifu_dstaddr
#endif

    void *ifa_data;
};

#if VSF_LINUX_CFG_WRAPPER == ENABLED
#define getifaddrs          VSF_LINUX_WRAPPER(getifaddrs)
#define freeifaddrs         VSF_LINUX_WRAPPER(freeifaddrs)
#endif

#if VSF_LINUX_APPLET_USE_IFADDRS == ENABLED
typedef struct vsf_linux_ifaddrs_vplt_t {
    vsf_vplt_info_t info;

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(getifaddrs);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(freeifaddrs);
} vsf_linux_ifaddrs_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_ifaddrs_vplt_t vsf_linux_ifaddrs_vplt;
#   endif
#endif

#if defined(__VSF_APPLET__) && VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_APPLET_USE_IFADDRS == ENABLED

#ifndef VSF_LINUX_APPLET_IFADDRS_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_IFADDRS_VPLT                                    \
            ((vsf_linux_ifaddrs_vplt_t *)(VSF_LINUX_APPLET_VPLT->ifaddrs_vplt))
#   else
#       define VSF_LINUX_APPLET_IFADDRS_VPLT                                    \
            ((vsf_linux_ifaddrs_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_IFADDRS_ENTRY(__NAME)                                  \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_IFADDRS_VPLT, __NAME)
#define VSF_LINUX_APPLET_IFADDRS_IMP(...)                                       \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_IFADDRS_VPLT, __VA_ARGS__)

VSF_LINUX_APPLET_IFADDRS_IMP(getifaddrs, int, struct ifaddrs **ifa) {
    return VSF_LINUX_APPLET_IFADDRS_ENTRY(getifaddrs)(ifa);
}
VSF_LINUX_APPLET_IFADDRS_IMP(freeifaddrs, void, struct ifaddrs *ifa) {
    VSF_LINUX_APPLET_IFADDRS_ENTRY(freeifaddrs)(ifa);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_IFADDRS

int getifaddrs(struct ifaddrs **ifa);
void freeifaddrs(struct ifaddrs *ifa);

#endif      // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_IFADDRS

#ifdef __cplusplus
}
#endif

#endif
