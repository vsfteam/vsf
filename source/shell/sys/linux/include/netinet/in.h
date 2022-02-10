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

#define INADDR_ANY          (in_addr_t)0x00000000
#define INADDR_NONE         (in_addr_t)0xFFFFFFFF
#define INADDR_BROADCAST    (in_addr_t)0xFFFFFFFF
#define INADDR_LOOPBACK     (in_addr_t)0x7F000001

#define IN_CLASSA(a)        ((((long int) (a)) & 0x80000000) == 0)
#define IN_CLASSA_NET       0xff000000
#define IN_CLASSA_NSHIFT    24
#define IN_CLASSA_HOST      (0xffffffff & ~IN_CLASSA_NET)
#define IN_CLASSA_MAX       128

#define IN_CLASSB(a)        ((((long int) (a)) & 0xc0000000) == 0x80000000)
#define IN_CLASSB_NET       0xffff0000
#define IN_CLASSB_NSHIFT    16
#define IN_CLASSB_HOST      (0xffffffff & ~IN_CLASSB_NET)
#define IN_CLASSB_MAX       65536

#define IN_CLASSC(a)        ((((long int) (a)) & 0xe0000000) == 0xc0000000)
#define IN_CLASSC_NET       0xffffff00
#define IN_CLASSC_NSHIFT    8
#define IN_CLASSC_HOST      (0xffffffff & ~IN_CLASSC_NET)

#define IN_CLASSD(a)        ((((long int) (a)) & 0xf0000000) == 0xe0000000)
#define IN_MULTICAST(a)     IN_CLASSD(a)
#define IN_MULTICAST_NET    0xe0000000

#define IN6_IS_ADDR_MULTICAST(a)    (((const uint8_t *) (a))[0] == 0xff)

#define IN6_IS_ADDR_MC_NODELOCAL(a)                                             \
        (IN6_IS_ADDR_MULTICAST(a)                                               \
         && ((((const uint8_t *) (a))[1] & 0xf) == 0x1))
#define IN6_IS_ADDR_MC_LINKLOCAL(a)                                             \
        (IN6_IS_ADDR_MULTICAST(a)                                               \
         && ((((const uint8_t *) (a))[1] & 0xf) == 0x2))
#define IN6_IS_ADDR_MC_SITELOCAL(a)                                             \
        (IN6_IS_ADDR_MULTICAST(a)                                               \
         && ((((const uint8_t *) (a))[1] & 0xf) == 0x5))
#define IN6_IS_ADDR_MC_ORGLOCAL(a)                                              \
        (IN6_IS_ADDR_MULTICAST(a)                                               \
         && ((((const uint8_t *) (a))[1] & 0xf) == 0x8))
#define IN6_IS_ADDR_MC_GLOBAL(a)                                                \
        (IN6_IS_ADDR_MULTICAST(a)                                               \
         && ((((const uint8_t *) (a))[1] & 0xf) == 0xe))

#define IN6_IS_ADDR_UNSPECIFIED(a)                                              \
        (((const uint32_t *) (a))[0] == 0                                       \
         && ((const uint32_t *) (a))[1] == 0                                    \
         && ((const uint32_t *) (a))[2] == 0                                    \
         && ((const uint32_t *) (a))[3] == 0)
#define IN6_IS_ADDR_LOOPBACK(a)                                                 \
        (((const uint32_t *) (a))[0] == 0                                       \
         && ((const uint32_t *) (a))[1] == 0                                    \
         && ((const uint32_t *) (a))[2] == 0                                    \
         && ((const uint32_t *) (a))[3] == htonl (1))
#define IN6_IS_ADDR_LINKLOCAL(a)                                                \
        ((((const uint32_t *) (a))[0] & htonl (0xffc00000))                     \
         == htonl (0xfe800000))
#define IN6_IS_ADDR_SITELOCAL(a)                                                \
        ((((const uint32_t *) (a))[0] & htonl (0xffc00000))                     \
         == htonl (0xfec00000))
#define IN6_IS_ADDR_V4MAPPED(a)                                                 \
        ((((const uint32_t *) (a))[0] == 0)                                     \
         && (((const uint32_t *) (a))[1] == 0)                                  \
         && (((const uint32_t *) (a))[2] == htonl (0xffff)))
#define IN6_IS_ADDR_V4COMPAT(a)                                                 \
        ((((const uint32_t *) (a))[0] == 0)                                     \
         && (((const uint32_t *) (a))[1] == 0)                                  \
         && (((const uint32_t *) (a))[2] == 0)                                  \
         && (ntohl (((const uint32_t *) (a))[3]) > 1))
#define IN6_ARE_ADDR_EQUAL(a,b)                                                 \
        ((((const uint32_t *) (a))[0] == ((const uint32_t *) (b))[0])           \
         && (((const uint32_t *) (a))[1] == ((const uint32_t *) (b))[1])        \
         && (((const uint32_t *) (a))[2] == ((const uint32_t *) (b))[2])        \
         && (((const uint32_t *) (a))[3] == ((const uint32_t *) (b))[3]))

#define INET_ADDRSTRLEN     16
struct in_addr {
    in_addr_t               s_addr;
};
struct sockaddr_in {
    sa_family_t             sin_family;
    in_port_t               sin_port;
    struct in_addr          sin_addr;
#define SIN_ZERO_LEN        8
    char                    sin_zero[SIN_ZERO_LEN];
};

#define INET6_ADDRSTRLEN    46
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

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../arpa/inet.h"
#else
#   include <arpa/inet.h>
#endif

#ifdef __cplusplus
}
#endif
#endif
