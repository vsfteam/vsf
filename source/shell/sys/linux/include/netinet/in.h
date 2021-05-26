#ifndef __VSF_LINUX_NETINET_IN_H__
#define __VSF_LINUX_NETINET_IN_H__

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

// fixme: in_port_t and in_addr_t should be in inttypes.h
typedef uint16_t            in_port_t;
typedef uint32_t            in_addr_t;

#define INADDR_ANY          (uint32_t)0x00000000
#define INADDR_BROADCAST    (uint32_t)0xFFFFFFFF
struct in_addr {
    in_addr_t               s_addr;
};
struct sockaddr_in {
    sa_family_t             sin_family;
    in_port_t               sin_port;
    struct in_addr          sin_addr;
    char                    sin_zero[8];
};

#define IN6ADDR_ANY_INIT    { 0 }
struct in6_addr {
    uint8_t                 s6_addr[16];
};
struct sockaddr_in6 {
    sa_family_t             sin6_family;
    in_port_t               sin6_port;
    uint32_t                sin6_flowinfo;
    struct in6_addr         sin6_addr;
    uint32_t                sin6_scope_id;
};

#ifdef __cplusplus
}
#endif
#endif
