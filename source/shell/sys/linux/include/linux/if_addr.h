#ifndef __VSF_LINUX_IFADDR_H__
#define __VSF_LINUX_IFADDR_H__

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../sys/types.h"
#   include "./netlink.h"
#else
#   include <sys/types.h>
#   include <linux/netlink.h>
#endif

struct ifaddrmsg {
    uint8_t                 ifa_family;
    uint8_t                 ifa_prefixlen;
    uint8_t                 ifa_flags;
    uint8_t                 ifa_scope;
    uint32_t                ifa_index;
};

enum {
    IFA_UNSPEC,
};

#endif      // __VSF_LINUX_IFADDR_H__
