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

int getifaddrs(struct ifaddrs **ifa);
void freeifaddrs(struct ifaddrs *ifa);

#ifdef __cplusplus
}
#endif

#endif
