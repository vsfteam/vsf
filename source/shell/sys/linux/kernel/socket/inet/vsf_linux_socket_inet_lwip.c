/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

/*============================ INCLUDES ======================================*/

#include "shell/sys/linux/vsf_linux_cfg.h"

#if     VSF_USE_LINUX == ENABLED && VSF_LINUX_USE_SOCKET == ENABLED             \
    &&  VSF_LINUX_SOCKET_USE_INET == ENABLED && VSF_USE_LWIP == ENABLED

#define __VSF_LINUX_FS_CLASS_INHERIT__
#define __VSF_LINUX_SOCKET_CLASS_INHERIT__
#define __VSF_LINUX_CLASS_INHERIT__
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../../../include/unistd.h"
#   include "../../../include/errno.h"
#   include "../../../include/sys/socket.h"
#   include "../../../include/sys/time.h"
#   include "../../../include/net/if.h"
#   include "../../../include/netinet/in.h"
#   include "../../../include/netinet/tcp.h"
#   include "../../../include/arpa/inet.h"
#   include "../../../include/ifaddrs.h"
#   include "../../../include/poll.h"
#   include "../../../include/fcntl.h"
#   if VSF_LINUX_SOCKET_USE_NETLINK == ENABLED
#       include "../../../include/linux/netlink.h"
#       include "../../../include/linux/rtnetlink.h"
#       include "../../../include/linux/if_addr.h"
#   endif
#else
#   include <unistd.h>
#   include <errno.h>
#   include <sys/socket.h>
#   include <sys/time.h>
#   include <net/if.h>
#   include <netinet/in.h>
#   include <netinet/tcp.h>
#   include <arpa/inet.h>
#   include <ifaddrs.h>
#   include <poll.h>
#   include <fcntl.h>
#   if VSF_LINUX_SOCKET_USE_NETLINK == ENABLED
#       include <linux/netlink.h>
#       include <linux/rtnetlink.h>
#       include <linux/if_addr.h>
#   endif
#endif
#include "../vsf_linux_socket.h"

#include "lwip/tcpip.h"
// avoid to include lwip/inet.h for conflictions
//#include "lwip/inet.h"
#include "lwip/api.h"
#include "lwip/ip.h"
#include "lwip/tcp.h"
#include "lwip/udp.h"
#include "lwip/dns.h"
#include "lwip/mld6.h"
#include "lwip/igmp.h"
#if LWIP_RAW
#   include "lwip/raw.h"
#endif

/*============================ MACROS ========================================*/

#if VSF_USE_LWIP != ENABLED
#   error current socket layer uses lwip as backend, please enable VSF_USE_LWIP
#endif

#if !LWIP_IPV4
#   error ipv4 is a MUST, please enable LWIP_IPV4
#endif

#if !LWIP_SO_RCVBUF
#   error LWIP_SO_RCVBUF MUST be enabled for SO_RCVBUF control in setsockopt/getsockopt
#endif

#if VSF_LINUX_SOCKET_USE_NETLINK == ENABLED && !LWIP_NETIF_EXT_STATUS_CALLBACK
#   error LWIP_NETIF_EXT_STATUS_CALLBACK MUST be enabled to support netlink socket
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#if LWIP_IPV4
#   define inet_addr_from_ip4addr(target_inaddr, source_ipaddr)                 \
        ((target_inaddr)->s_addr = ip4_addr_get_u32(source_ipaddr))
#   define inet_addr_to_ip4addr(target_ipaddr, source_inaddr)                   \
        (ip4_addr_set_u32(target_ipaddr, (source_inaddr)->s_addr))
#   define inet_addr_to_ip4addr_p(target_ip4addr_p, source_inaddr)              \
        ((target_ip4addr_p) = (ip4_addr_t*)&((source_inaddr)->s_addr))
#endif

#if LWIP_IPV6
#   define inet6_addr_from_ip6addr(target_in6addr, source_ip6addr)              \
        {                                                                       \
            (target_in6addr)->s6_addr32[0] = (source_ip6addr)->addr[0];         \
            (target_in6addr)->s6_addr32[1] = (source_ip6addr)->addr[1];         \
            (target_in6addr)->s6_addr32[2] = (source_ip6addr)->addr[2];         \
            (target_in6addr)->s6_addr32[3] = (source_ip6addr)->addr[3];         \
        }
#   define inet6_addr_to_ip6addr(target_ip6addr, source_in6addr)                \
        {                                                                       \
            (target_ip6addr)->addr[0] = (source_in6addr)->s6_addr32[0];         \
            (target_ip6addr)->addr[1] = (source_in6addr)->s6_addr32[1];         \
            (target_ip6addr)->addr[2] = (source_in6addr)->s6_addr32[2];         \
            (target_ip6addr)->addr[3] = (source_in6addr)->s6_addr32[3];         \
            ip6_addr_clear_zone(target_ip6addr);                                \
        }
#endif /* LWIP_IPV6 */

/*============================ TYPES =========================================*/

typedef struct vsf_linux_socket_group_t {
    vsf_dlist_node_t        node;
    sa_family_t             family;
    union {
#if LWIP_IPV6
        struct {
            ip6_addr_t      multi_addr;
            struct netif    *netif;
        } ip6;
#endif
#if LWIP_IPV4
        struct {
            ip4_addr_t      multi_addr;
            ip4_addr_t      if_addr;
        } ip4;
#endif
    };
} vsf_linux_socket_group_t;

typedef struct vsf_linux_socket_inet_priv_t {
    implement(vsf_linux_socket_priv_t)

    struct netconn          *conn;
    vsf_dlist_t             group_list;
    struct {
        struct netbuf       *netbuf;
        struct pbuf         *pbuf;
    } last;
    uint16_t                rxcnt;
} vsf_linux_socket_inet_priv_t;

typedef union vsf_linux_sockaddr_t {
    struct sockaddr         sa;
    struct sockaddr_in      in;
    struct sockaddr_in6     in6;
} vsf_linux_sockaddr_t;

#if VSF_LINUX_SOCKET_USE_NETLINK == ENABLED
typedef struct vsf_linux_socket_netlink_msg_t {
    vsf_slist_node_t node;

    struct nlmsghdr hdr;
} vsf_linux_socket_netlink_msg_t;

typedef struct vsf_linux_socket_netlink_priv_t {
    implement(vsf_linux_socket_priv_t)

    vsf_slist_queue_t msg_list;
    vsf_dlist_node_t node;
    uint32_t groups;
    uint32_t seq;
} vsf_linux_socket_netlink_priv_t;
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

int __vsf_linux_socket_inet_fcntl(vsf_linux_fd_t *sfd, int cmd, uintptr_t arg);
static int __vsf_linux_lwip_socket_inet_fcntl(vsf_linux_fd_t *sfd, int cmd, uintptr_t arg);
static ssize_t __vsf_linux_socket_inet_read(vsf_linux_fd_t *sfd, void *buf, size_t count);
static ssize_t __vsf_linux_socket_inet_write(vsf_linux_fd_t *sfd, const void *buf, size_t count);
static int __vsf_linux_socket_inet_close(vsf_linux_fd_t *sfd);

// APIs not implemented in older lwip
extern struct pbuf * pbuf_free_header(struct pbuf *q, u16_t size);

static void __vsf_linux_socket_inet_lwip_evthandler(struct netconn *conn, enum netconn_evt evt, u16_t len);

static int __vsf_linux_socket_inet_init(vsf_linux_fd_t *sfd);
static int __vsf_linux_socket_inet_fini(vsf_linux_socket_priv_t *socket_priv, int how);
static int __vsf_linux_socket_inet_connect(vsf_linux_socket_priv_t *socket_priv, const struct sockaddr *addr, socklen_t addrlen);
static int __vsf_linux_socket_inet_listen(vsf_linux_socket_priv_t *socket_priv, int backlog);
static int __vsf_linux_socket_inet_accept(vsf_linux_socket_priv_t *socket_priv, struct sockaddr *addr, socklen_t *addr_len);
static int __vsf_linux_socket_inet_bind(vsf_linux_socket_priv_t *socket_priv, const struct sockaddr *addr, socklen_t addrlen);
static int __vsf_linux_socket_inet_getsockopt(vsf_linux_socket_priv_t *socket_priv, int level, int optname, void *optval, socklen_t *optlen);
static int __vsf_linux_socket_inet_setsockopt(vsf_linux_socket_priv_t *socket_priv, int level, int optname,const void *optval, socklen_t optlen);
static int __vsf_linux_socket_inet_getpeername(vsf_linux_socket_priv_t *socket_priv, struct sockaddr *addr, socklen_t *addrlen);
static int __vsf_linux_socket_inet_getsockname(vsf_linux_socket_priv_t *socket_priv, struct sockaddr *addr, socklen_t *addrlen);

#if VSF_LINUX_SOCKET_USE_NETLINK == ENABLED
static int __vsf_linux_socket_netlink_init(vsf_linux_fd_t *sfd);
static int __vsf_linux_socket_netlink_fini(vsf_linux_socket_priv_t *socket_priv, int how);
static ssize_t __vsf_linux_socket_netlink_read(vsf_linux_fd_t *sfd, void *buf, size_t count);
static int __vsf_linux_socket_netlink_bind(vsf_linux_socket_priv_t *socket_priv, const struct sockaddr *addr, socklen_t addrlen);
#endif

/*============================ LOCAL VARIABLES ===============================*/

#if VSF_LINUX_SOCKET_USE_NETLINK == ENABLED
static bool __vsf_linux_netlink_is_callback_installed = false;
static vsf_dlist_t __vsf_linux_netlink_priv_list = { 0 };
static netif_ext_callback_t __vsf_linux_netlink_lwip_netif_cb;
#endif

/*============================ GLOBAL VARIABLES ==============================*/

const vsf_linux_socket_op_t vsf_linux_socket_inet_op = {
    .fdop               = {
        .priv_size      = sizeof(vsf_linux_socket_inet_priv_t),
        .fn_fcntl       = __vsf_linux_lwip_socket_inet_fcntl,
        .fn_read        = __vsf_linux_socket_inet_read,
        .fn_write       = __vsf_linux_socket_inet_write,
        .fn_close       = __vsf_linux_socket_inet_close,
    },

    .fn_init            = __vsf_linux_socket_inet_init,
    .fn_fini            = __vsf_linux_socket_inet_fini,
    .fn_connect         = __vsf_linux_socket_inet_connect,
    .fn_listen          = __vsf_linux_socket_inet_listen,
    .fn_accept          = __vsf_linux_socket_inet_accept,
    .fn_bind            = __vsf_linux_socket_inet_bind,
    .fn_getsockopt      = __vsf_linux_socket_inet_getsockopt,
    .fn_setsockopt      = __vsf_linux_socket_inet_setsockopt,
    .fn_getpeername     = __vsf_linux_socket_inet_getpeername,
    .fn_getsockname     = __vsf_linux_socket_inet_getsockname,
};

#if VSF_LINUX_SOCKET_USE_NETLINK == ENABLED
const vsf_linux_socket_op_t vsf_linux_socket_netlink_op = {
    .fdop               = {
        .priv_size      = sizeof(vsf_linux_socket_netlink_priv_t),
        .fn_read        = __vsf_linux_socket_netlink_read,
    },

    .fn_init            = __vsf_linux_socket_netlink_init,
    .fn_fini            = __vsf_linux_socket_netlink_fini,
    .fn_bind            = __vsf_linux_socket_netlink_bind,
};
#endif

/*============================ IMPLEMENTATION ================================*/

#if __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#endif

// helper
static void __sockaddr_to_ipaddr_port(const struct sockaddr *sockaddr, ip_addr_t *ipaddr, u16_t *port)
{
#if LWIP_IPV6
    if (AF_INET6 == sockaddr->sa_family) {
        const struct sockaddr_in6 *sockaddr_in6 = (const struct sockaddr_in6 *)sockaddr;

        ip_2_ip6(ipaddr)->addr[0] = sockaddr_in6->sin6_addr.s6_addr[0];
        ip_2_ip6(ipaddr)->addr[1] = sockaddr_in6->sin6_addr.s6_addr[1];
        ip_2_ip6(ipaddr)->addr[2] = sockaddr_in6->sin6_addr.s6_addr[2];
        ip_2_ip6(ipaddr)->addr[3] = sockaddr_in6->sin6_addr.s6_addr[3];

#   if LWIP_IPV6_SCOPES
        ip6_addr_clear_zone(ip_2_ip6(ipaddr));
        if (ip6_addr_has_scope(ip_2_ip6(ipaddr), IP6_UNKNOWN)) {
            ip6_addr_set_zone(ip_2_ip6(ipaddr), (u8_t)(sockaddr_in6->sin6_scope_id));
        }
#   endif

        *port = lwip_ntohs(sockaddr_in6->sin6_port);
        ipaddr->type = IPADDR_TYPE_V6;
    } else
#endif
    {
        const struct sockaddr_in * sockaddr_in = (const struct sockaddr_in *)sockaddr;

        ip4_addr_set_u32(ip_2_ip4(ipaddr), sockaddr_in->sin_addr.s_addr);
        *port = lwip_ntohs(sockaddr_in->sin_port);
#if LWIP_IPV6
        ipaddr->type = IPADDR_TYPE_V4;
#endif
    }
}

static void __ipaddr_port_to_sockaddr(struct sockaddr *sockaddr, ip_addr_t *ipaddr, u16_t port)
{
#if LWIP_IPV6
    if (IP_IS_ANY_TYPE_VAL(*ipaddr) || IP_IS_V6_VAL(*ipaddr)) {
        struct sockaddr_in6 *sockaddr_in6 = (struct sockaddr_in6 *)sockaddr;

        sockaddr_in6->sin6_family = AF_INET6;
        sockaddr_in6->sin6_port = lwip_htons((port));
        sockaddr_in6->sin6_flowinfo = 0;
        sockaddr_in6->sin6_addr.s6_addr[0] = ip_2_ip6(ipaddr)->addr[0];
        sockaddr_in6->sin6_addr.s6_addr[1] = ip_2_ip6(ipaddr)->addr[1];
        sockaddr_in6->sin6_addr.s6_addr[2] = ip_2_ip6(ipaddr)->addr[2];
        sockaddr_in6->sin6_addr.s6_addr[3] = ip_2_ip6(ipaddr)->addr[3];
#   if LWIP_IPV6_SCOPES
        sockaddr_in6->sin6_scope_id = ip6_addr_zone(ip_2_ip6(ipaddr));
#   endif
    } else
#endif
    {
        struct sockaddr_in * sockaddr_in = (struct sockaddr_in *)sockaddr;

        sockaddr_in->sin_family = AF_INET;
        sockaddr_in->sin_port = lwip_htons((port));
        sockaddr_in->sin_addr.s_addr = ip4_addr_get_u32(ip_2_ip4(ipaddr));
        memset(sockaddr_in->sin_zero, 0, SIN_ZERO_LEN);
    }
}

static int __netconn_return(err_t err)
{
    switch (err) {
    case ERR_OK:                                    break;
    case ERR_MEM:           errno = ENOMEM;         break;
    case ERR_BUF:           errno = ENOBUFS;        break;
    case ERR_TIMEOUT:       errno = ETIMEDOUT;      break;
    case ERR_USE:           errno = EADDRINUSE;     break;
    case ERR_INPROGRESS:    errno = EINPROGRESS;    break;
    case ERR_WOULDBLOCK:    errno = EAGAIN;         break;
    case ERR_RST:           errno = ENETRESET;      break;
    case ERR_ABRT:          errno = ECONNABORTED;   break;
    default:                errno = EIO;            break;
    }
    return err == ERR_OK ? 0 : SOCKET_ERROR;
}

#if __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic pop
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

// copied from lwip_socket
static int lwip_sockopt_to_ipopt(int optname)
{
  /* Map SO_* values to our internal SOF_* values
   * We should not rely on #defines in socket.h
   * being in sync with ip.h.
   */
  switch (optname) {
  case SO_BROADCAST:
    return SOF_BROADCAST;
  case SO_KEEPALIVE:
    return SOF_KEEPALIVE;
  case SO_REUSEADDR:
    return SOF_REUSEADDR;
  default:
    LWIP_ASSERT("Unknown socket option", 0);
    return 0;
  }
}

// socket
static int __vsf_linux_socket_inet_init(vsf_linux_fd_t *sfd)
{
    vsf_linux_socket_priv_t *socket_priv = (vsf_linux_socket_priv_t *)sfd->priv;
    vsf_linux_socket_inet_priv_t *priv = (vsf_linux_socket_inet_priv_t *)socket_priv;
    struct netconn *conn;
    enum netconn_type conn_type;
    bool use_protocol = false;

    switch (socket_priv->type) {
    case SOCK_DGRAM:
        if (socket_priv->protocol != IPPROTO_UDP) {
            return INVALID_SOCKET;
        }
        conn_type = NETCONN_UDP;
        break;
    case SOCK_RAW:
        conn_type = NETCONN_RAW;
        use_protocol = true;
        break;
    case SOCK_STREAM:
        if (socket_priv->protocol != IPPROTO_TCP) {
            // some user code will call socket to create STREAM sock with protocol 0
//            return INVALID_SOCKET;
        }
        conn_type = NETCONN_TCP;
        break;
    default:
        return INVALID_SOCKET;
    }

    if (use_protocol) {
        conn = netconn_new_with_proto_and_callback(conn_type, (u8_t)socket_priv->protocol,
                __vsf_linux_socket_inet_lwip_evthandler);
    } else {
        conn = netconn_new_with_callback(conn_type, __vsf_linux_socket_inet_lwip_evthandler);
    }
    if (NULL == conn) {
        return INVALID_SOCKET;
    }
#if LWIP_NETBUF_RECVINFO
    else {
        conn->flags &= ~NETCONN_FLAG_PKTINFO;
    }
#endif

    priv->conn = conn;
    VSF_LINUX_ASSERT(sizeof(conn->socket) >= sizeof(vsf_linux_fd_t *));
    conn->socket = (int)sfd;
    return 0;
}

static int __vsf_linux_socket_inet_fini(vsf_linux_socket_priv_t *socket_priv, int how)
{
    vsf_linux_socket_inet_priv_t *priv = (vsf_linux_socket_inet_priv_t *)socket_priv;
    struct netconn *conn = priv->conn;

    VSF_LINUX_ASSERT(NETCONNTYPE_GROUP(netconn_type(conn)) == NETCONN_TCP);
    VSF_LINUX_ASSERT((how & SHUT_RDWR) != 0);

    err_t err = netconn_shutdown(conn, how & SHUT_RD, how & SHUT_WR);
    return __netconn_return(err);
}

static long __vsf_linux_timeval_to_ms(const struct timeval *t)
{
    return t->tv_sec * 1000 + t->tv_usec / 1000;
}

static int __vsf_linux_socket_inet_setsockopt(vsf_linux_socket_priv_t *socket_priv, int level, int optname, const void *optval, socklen_t optlen)
{
    vsf_linux_socket_inet_priv_t *priv = (vsf_linux_socket_inet_priv_t *)socket_priv;
    struct netconn *conn = priv->conn;

    switch (level) {
    case SOL_SOCKET:
        switch (optname) {
        case SO_BROADCAST:
            if (NETCONNTYPE_GROUP(netconn_type(conn)) != NETCONN_UDP) {
                return ENOPROTOOPT;
            }
            // fall through
        case SO_KEEPALIVE:
        case SO_REUSEADDR:
            optname = lwip_sockopt_to_ipopt(optname);
            if (*(const int *)optval) {
                ip_set_option(conn->pcb.ip, optname);
            } else {
                ip_reset_option(conn->pcb.ip, optname);
            }
            break;
        case SO_SNDBUF:
            // ignore without error
            break;
        case SO_RCVBUF:
            netconn_set_recvbufsize(conn, *(const int *)optval);
            break;
#if LWIP_SO_RCVTIMEO
        case SO_RCVTIMEO: {
                long ms = __vsf_linux_timeval_to_ms((const struct timeval *)optval);
                netconn_set_recvtimeout(conn, (u32_t)ms);
            }
            break;
#endif
#if LWIP_SO_SNDTIMEO
        case SO_SNDTIMEO: {
                long ms = __vsf_linux_timeval_to_ms((const struct timeval *)optval);
                netconn_set_sendtimeout(conn, ms);
            }
            break;
#endif
        case SO_NONBLOCK:
            netconn_set_nonblocking(conn, *(const int *)optval);
            break;
#if LWIP_SO_LINGER
        case SO_LINGER: {
                const struct linger *linger = optval;
                if (linger->l_onoff) {
                    VSF_LINUX_ASSERT(linger->l_linger >= 0);
                    conn->linger = (s16_t)linger->l_linger;
                } else {
                    conn->linger = -1;
                }
            }
            break;
#endif
#if LWIP_UDP
        case SO_NO_CHECK:
#if LWIP_UDPLITE
            if ((udp_flags(conn->pcb.udp) & UDP_FLAGS_UDPLITE) != 0) {
                return EAFNOSUPPORT;
            }
#endif
            if (*(const int*)optval) {
                udp_setflags(conn->pcb.udp, udp_flags(conn->pcb.udp) | UDP_FLAGS_NOCHKSUM);
            } else {
                udp_setflags(conn->pcb.udp, udp_flags(conn->pcb.udp) & ~UDP_FLAGS_NOCHKSUM);
            }
            break;
#endif
// some old version of lwip does not support bind APIs, lwip supports these APIs, enable VSF_LINUX_CFG_SOCKET_BIND
#if VSF_LINUX_CFG_SOCKET_BIND == ENABLED
        case SO_BINDTODEVICE: {
                const struct ifreq *iface;
                struct netif *n = NULL;

                iface = (const struct ifreq *)optval;
                if (iface->ifr_name[0] != 0) {
                    n = netif_find(iface->ifr_name);
                    if (n == NULL) {
                        return ENODEV;
                    }
                }

                switch (NETCONNTYPE_GROUP(netconn_type(conn))) {
#if LWIP_TCP
                case NETCONN_TCP:
                    tcp_bind_netif(conn->pcb.tcp, n);
                    break;
#endif
#if LWIP_UDP
                case NETCONN_UDP:
                    udp_bind_netif(conn->pcb.udp, n);
                    break;
#endif
#if LWIP_RAW
                case NETCONN_RAW:
                    raw_bind_netif(conn->pcb.raw, n);
                    break;
#endif
                default:
                    LWIP_ASSERT("Unhandled netconn type in SO_BINDTODEVICE", 0);
                    break;
                }
            }
            break;
#endif
        default:
            // TODO: add support
            VSF_LINUX_ASSERT(false);
            break;
        }
        break;
    case IPPROTO_IP:
        switch (optname) {
        case IP_TTL:
            conn->pcb.ip->ttl = (u8_t)(*(const int*)optval);
            break;
        case IP_TOS:
            conn->pcb.ip->tos = (u8_t)(*(const int*)optval);
            break;
#if LWIP_MULTICAST_TX_OPTIONS
        case IP_MULTICAST_TTL:
            udp_set_multicast_ttl(conn->pcb.udp, (u8_t)(*(const u8_t*)optval));
            break;
        case IP_MULTICAST_IF: {
                ip4_addr_t if_addr;
                inet_addr_to_ip4addr(&if_addr, (const struct in_addr*)optval);
                udp_set_multicast_netif_addr(conn->pcb.udp, &if_addr);
            }
            break;
        case IP_MULTICAST_LOOP:
            if (*(const u8_t*)optval) {
                udp_setflags(conn->pcb.udp, udp_flags(conn->pcb.udp) | UDP_FLAGS_MULTICAST_LOOP);
            } else {
                udp_setflags(conn->pcb.udp, udp_flags(conn->pcb.udp) & ~UDP_FLAGS_MULTICAST_LOOP);
            }
            break;
#endif
#if LWIP_IGMP
        case IP_ADD_MEMBERSHIP:
        case IP_DROP_MEMBERSHIP: {
            err_t igmp_err;
            const struct ip_mreq *imr = (const struct ip_mreq *)optval;
            vsf_linux_socket_group_t *group = NULL;
            ip4_addr_t if_addr;
            ip4_addr_t multi_addr;

            inet_addr_to_ip4addr(&if_addr, &imr->imr_interface);
            inet_addr_to_ip4addr(&multi_addr, &imr->imr_multiaddr);
            if (optname == IP_ADD_MEMBERSHIP) {
                igmp_err = igmp_joingroup(&if_addr, &multi_addr);
                if (igmp_err != ERR_OK) {
                    return EADDRNOTAVAIL;
                }

                group = vsf_heap_malloc(sizeof(*group));
                if (NULL == group) {
                    return ENOMEM;
                }

                group->family = AF_INET;
                ip4_addr_copy(group->ip4.if_addr, if_addr);
                ip4_addr_copy(group->ip4.multi_addr, multi_addr);
                vsf_dlist_add_to_head(vsf_linux_socket_group_t, node, &priv->group_list, group);
            } else {
                __vsf_dlist_foreach_unsafe(vsf_linux_socket_group_t, node, &priv->group_list) {
                    if (    ip4_addr_cmp(&group->ip4.if_addr, &if_addr)
                        &&  ip4_addr_cmp(&group->ip4.multi_addr, &multi_addr)) {
                        group = _;
                    }
                }
                if (group != NULL) {
                    vsf_dlist_remove(vsf_linux_socket_group_t, node, &priv->group_list, group);
                    igmp_err = igmp_leavegroup(&if_addr, &multi_addr);
                    if (igmp_err != ERR_OK) {
                        return EADDRNOTAVAIL;
                    }
                }
            }
        }
        break;
#endif
        }
        break;
    case IPPROTO_TCP:
        switch (optname) {
        case TCP_NODELAY:
            if (*(const int *)optval) {
                tcp_nagle_disable(conn->pcb.tcp);
            } else {
                tcp_nagle_enable(conn->pcb.tcp);
            }
            break;
        case TCP_MAXSEG:
            // TODO: add support
            break;
        case TCP_KEEPALIVE:
            conn->pcb.tcp->keep_idle = (u32_t)(*(const int *)optval);
            break;
        default:
            VSF_LINUX_ASSERT(false);
            break;
        }
        break;
#if LWIP_IPV6
    case IPPROTO_IPV6:
        switch (optname) {
        case IPV6_V6ONLY:
            if (*(const int *)optval) {
                netconn_set_ipv6only(conn, 1);
            } else {
                netconn_set_ipv6only(conn, 0);
            }
            break;
#   if LWIP_IPV6_MLD
        case IPV6_JOIN_GROUP:
        case IPV6_LEAVE_GROUP: {
            err_t mld6_err;
            struct netif *netif;
            ip6_addr_t multi_addr;
            vsf_linux_socket_group_t *group = NULL;
            const struct ipv6_mreq *imr = (const struct ipv6_mreq *)optval;

            inet6_addr_to_ip6addr(&multi_addr, &imr->ipv6mr_multiaddr);
            netif = netif_get_by_index((u8_t)imr->ipv6mr_interface);
            if (netif == NULL) {
                return EADDRNOTAVAIL;
            }

            if (optname == IPV6_JOIN_GROUP) {
                mld6_err = mld6_joingroup_netif(netif, &multi_addr);
                if (mld6_err != ERR_OK) {
                    return EADDRNOTAVAIL;
                }

                group = vsf_heap_malloc(sizeof(*group));
                if (NULL == group) {
                    return ENOMEM;
                }

                group->family = AF_INET6;
                group->ip6.netif = netif;
                ip6_addr_copy(group->ip6.multi_addr, multi_addr);
                vsf_dlist_add_to_head(vsf_linux_socket_group_t, node, &priv->group_list, group);
            } else {
                __vsf_dlist_foreach_unsafe(vsf_linux_socket_group_t, node, &priv->group_list) {
                    if ((_->ip6.netif == netif) && ip6_addr_cmp(&group->ip6.multi_addr, &multi_addr)) {
                        group = _;
                    }
                }
                if (group != NULL) {
                    vsf_dlist_remove(vsf_linux_socket_group_t, node, &priv->group_list, group);
                    mld6_err = mld6_leavegroup_netif(netif, &multi_addr);
                    if (mld6_err != ERR_OK) {
                        return EADDRNOTAVAIL;
                    }
                }
            }
        }
        break;
#   endif
        default:
            VSF_LINUX_ASSERT(false);
            break;
        }
        break;
#endif
    default:
        // TODO: add support
        VSF_LINUX_ASSERT(false);
        break;
    }
    return 0;
}

static void __vsf_linux_ms_to_timeval(struct timeval *t, u32_t ms)
{
    t->tv_sec = ms / 1000;
    t->tv_usec = (ms % 1000) * 1000;
}

static int __vsf_linux_socket_inet_getsockopt(vsf_linux_socket_priv_t *socket_priv, int level, int optname, void *optval, socklen_t *optlen)
{
    vsf_linux_socket_inet_priv_t *priv = (vsf_linux_socket_inet_priv_t *)socket_priv;
    struct netconn *conn = priv->conn;

    switch (level) {
    case SOL_SOCKET:
        switch (optname) {
        case SO_BROADCAST:
            if (NETCONNTYPE_GROUP(netconn_type(conn)) != NETCONN_UDP) {
                return ENOPROTOOPT;
            }
            // fall through
        case SO_KEEPALIVE:
            optname = lwip_sockopt_to_ipopt(optname);
            *(int *)optval = ip_get_option(conn->pcb.ip, optname);
            break;
        case SO_TYPE:
            switch (NETCONNTYPE_GROUP(netconn_type(conn))) {
            case NETCONN_RAW:   *(int *)optval = SOCK_RAW;      break;
            case NETCONN_TCP:   *(int *)optval = SOCK_STREAM;   break;
            case NETCONN_UDP:   *(int *)optval = SOCK_DGRAM;    break;
            default:            VSF_LINUX_ASSERT(false);        break;
            }
            break;
        case SO_RCVBUF:
            *(int *)optval = netconn_get_recvbufsize(conn);
            break;
        case SO_SNDBUF:
            switch (NETCONNTYPE_GROUP(netconn_type(conn))) {
            case NETCONN_TCP:   *(int *)optval = TCP_SND_BUF;   break;
            default:            VSF_LINUX_ASSERT(false);        break;
            }
            break;
        case SO_REUSEADDR:
            if (    (optname == SO_BROADCAST)
                &&  (NETCONNTYPE_GROUP(netconn_type(conn)) != NETCONN_UDP)) {
                return ENOPROTOOPT;
            }

            optname = lwip_sockopt_to_ipopt(optname);

            *(int *)optval = ip_get_option(conn->pcb.ip, optname);
            break;
#if LWIP_SO_RCVTIMEO
        case SO_RCVTIMEO:
            __vsf_linux_ms_to_timeval(optval, netconn_get_recvtimeout(conn));
            break;
#endif
#if LWIP_SO_SNDTIMEO
        case SO_SNDTIMEO:
            __vsf_linux_ms_to_timeval(optval, netconn_get_sendtimeout(conn));
            break;
#endif
        case SO_ERROR:
            *(int *)optval = err_to_errno(netconn_err(conn));
            break;
#if LWIP_SO_LINGER
        case SO_LINGER: {
                s16_t conn_linger;
                struct linger *linger = optval;
                conn_linger = conn->linger;
                if (conn_linger >= 0) {
                    linger->l_onoff = 1;
                    linger->l_linger = (int)conn_linger;
                } else {
                    linger->l_onoff = 0;
                    linger->l_linger = 0;
                }
            }
            break;
#endif
#if LWIP_UDP
        case SO_NO_CHECK:
#if LWIP_UDPLITE
            if ((udp_flags(conn->pcb.udp) & UDP_FLAGS_UDPLITE) != 0) {
                return EAFNOSUPPORT;
            }
#endif
            *(int*)optval = (udp_flags(conn->pcb.udp) & UDP_FLAGS_NOCHKSUM) ? 1 : 0;
            break;
#endif
        default:
            VSF_LINUX_ASSERT(false);
            break;
        }
        break;
    case IPPROTO_IP:
        switch (optname) {
        case IP_TTL:
            *(int*)optval = conn->pcb.ip->ttl;
            break;
        case IP_TOS:
            *(int*)optval = conn->pcb.ip->tos;
            break;
#if LWIP_MULTICAST_TX_OPTIONS
        case IP_MULTICAST_TTL:
            if (NETCONNTYPE_GROUP(netconn_type(conn)) != NETCONN_UDP) {
                return ENOPROTOOPT;
            }
            *(u8_t*)optval = udp_get_multicast_ttl(conn->pcb.udp);
            break;
        case IP_MULTICAST_IF:
            if (NETCONNTYPE_GROUP(netconn_type(conn)) != NETCONN_UDP) {
                return ENOPROTOOPT;
            }
            inet_addr_from_ip4addr((struct in_addr*)optval, udp_get_multicast_netif_addr(conn->pcb.udp));
            break;
        case IP_MULTICAST_LOOP:
            if ((conn->pcb.udp->flags & UDP_FLAGS_MULTICAST_LOOP) != 0) {
                *(u8_t*)optval = 1;
            } else {
                *(u8_t*)optval = 0;
            }
            break;
#endif /* LWIP_MULTICAST_TX_OPTIONS */
        }
        break;
    case IPPROTO_TCP:
        switch (optname) {
        case TCP_NODELAY:
            *(int *)optval = tcp_nagle_disabled(conn->pcb.tcp);
            break;
        case TCP_KEEPALIVE:
            *(int *)optval = (int)conn->pcb.tcp->keep_idle;
            break;
        default:
            VSF_LINUX_ASSERT(false);
            break;
        }
        break;
#if LWIP_IPV6
    case IPPROTO_IPV6:
      switch (optname) {
        case IPV6_V6ONLY:
          *(int *)optval = (netconn_get_ipv6only(conn) ? 1 : 0);
          break;
        default:
          VSF_LINUX_ASSERT(false);
          break;
      }
      break;
#endif
    default:
        // TODO: add support
        VSF_LINUX_ASSERT(false);
        break;
    }
    return 0;
}

static int __vsf_linux_socket_inet_getpeername(vsf_linux_socket_priv_t *socket_priv, struct sockaddr *addr, socklen_t *addrlen)
{
    vsf_linux_socket_inet_priv_t *priv = (vsf_linux_socket_inet_priv_t *)socket_priv;
    struct netconn *conn = priv->conn;

    ip_addr_t localaddr;
    u16_t port;
    netconn_peer(conn, &localaddr, &port);
    __ipaddr_port_to_sockaddr(addr, &localaddr, port);
    return 0;
}

static int __vsf_linux_socket_inet_getsockname(vsf_linux_socket_priv_t *socket_priv, struct sockaddr *addr, socklen_t *addrlen)
{
    vsf_linux_socket_inet_priv_t *priv = (vsf_linux_socket_inet_priv_t *)socket_priv;
    struct netconn *conn = priv->conn;

    ip_addr_t localaddr;
    u16_t port;
    netconn_addr(conn, &localaddr, &port);
    __ipaddr_port_to_sockaddr(addr, &localaddr, port);
    return 0;
}

static int __vsf_linux_socket_inet_accept(vsf_linux_socket_priv_t *socket_priv, struct sockaddr *addr, socklen_t *addrlen)
{
    vsf_linux_socket_inet_priv_t *priv = (vsf_linux_socket_inet_priv_t *)socket_priv;
    struct netconn *conn = priv->conn;

    VSF_LINUX_ASSERT(NETCONNTYPE_GROUP(netconn_type(conn)) == NETCONN_TCP);

    struct netconn *newconn;
    err_t err = netconn_accept(conn, &newconn);
    if (err != ERR_OK) {
        return SOCKET_ERROR;
    }

    int newsock = socket(socket_priv->domain, socket_priv->type, socket_priv->protocol);
    if (newsock < 0) {
        netconn_delete(newconn);
        return INVALID_SOCKET;
    }

    vsf_linux_fd_t *sfd = vsf_linux_fd_get(newsock);
    priv = (vsf_linux_socket_inet_priv_t *)sfd->priv;
    priv->conn = newconn;

    if ((addr != NULL) && (addrlen != NULL)) {
        ip_addr_t naddr;
        uint16_t port = 0;
        err = netconn_peer(newconn, &naddr, &port);
        if (ERR_OK != err) {
            vsf_linux_fd_delete(newsock);
            return SOCKET_ERROR;
        }

        // TODO: get addr and addrlen if addrlen is large enough
//        __ipaddr_port_to_sockaddr(, &naddr, port);
//        if (*addrlen >
    }


    newconn->callback = __vsf_linux_socket_inet_lwip_evthandler;
    LOCK_TCPIP_CORE();
        int rcvplus = -1 -  newconn->socket;
        newconn->socket = (int)sfd;
        while (rcvplus > 0) {
            rcvplus--;
            newconn->callback(newconn, NETCONN_EVT_RCVPLUS, 0);
        }
        newconn->callback(newconn, NETCONN_EVT_SENDPLUS, 0);
    UNLOCK_TCPIP_CORE();
    return newsock;
}

static int __vsf_linux_socket_inet_bind(vsf_linux_socket_priv_t *socket_priv, const struct sockaddr *addr, socklen_t addrlen)
{
    vsf_linux_socket_inet_priv_t *priv = (vsf_linux_socket_inet_priv_t *)socket_priv;
    struct netconn *conn = priv->conn;

    ip_addr_t local_addr;
    uint16_t local_port;
    __sockaddr_to_ipaddr_port(addr, &local_addr, &local_port);
#if LWIP_IPV4 && LWIP_IPV6
    /* Dual-stack: Unmap IPv4 mapped IPv6 addresses */
    if (IP_IS_V6_VAL(local_addr) && ip6_addr_isipv4mappedipv6(ip_2_ip6(&local_addr))) {
        unmap_ipv4_mapped_ipv6(ip_2_ip4(&local_addr), ip_2_ip6(&local_addr));
        IP_SET_TYPE_VAL(local_addr, IPADDR_TYPE_V4);
    }
#endif

    err_t err = netconn_bind(conn, &local_addr, local_port);
    return __netconn_return(err);
}

static int __vsf_linux_socket_inet_connect(vsf_linux_socket_priv_t *socket_priv, const struct sockaddr *addr, socklen_t addrlen)
{
    vsf_linux_socket_inet_priv_t *priv = (vsf_linux_socket_inet_priv_t *)socket_priv;
    struct netconn *conn = priv->conn;

    ip_addr_t remote_addr;
    uint16_t remote_port;
    __sockaddr_to_ipaddr_port(addr, &remote_addr, &remote_port);
#if LWIP_IPV4 && LWIP_IPV6
    /* Dual-stack: Unmap IPv4 mapped IPv6 addresses */
    if (IP_IS_V6_VAL(remote_addr) && ip6_addr_isipv4mappedipv6(ip_2_ip6(&remote_addr))) {
        unmap_ipv4_mapped_ipv6(ip_2_ip4(&remote_addr), ip_2_ip6(&remote_addr));
        IP_SET_TYPE_VAL(remote_addr, IPADDR_TYPE_V4);
    }
#endif
    err_t err = netconn_connect(conn, &remote_addr, remote_port);
    return __netconn_return(err);
}

static int __vsf_linux_socket_inet_listen(vsf_linux_socket_priv_t *socket_priv, int backlog)
{
    vsf_linux_socket_inet_priv_t *priv = (vsf_linux_socket_inet_priv_t *)socket_priv;
    struct netconn *conn = priv->conn;

    VSF_LINUX_ASSERT(NETCONNTYPE_GROUP(netconn_type(conn)) == NETCONN_TCP);

    if (backlog < 0) {
        backlog = 0;
    } else if (backlog > 0xFF) {
        backlog = 0xFF;
    }

    err_t err = netconn_listen_with_backlog(conn, (u8_t)backlog);
    return __netconn_return(err);
}

static ssize_t __vsf_linux_socket_inet_send(vsf_linux_socket_inet_priv_t *priv, const void *buffer, size_t size, int flags,
                    const struct sockaddr *dst_addr, socklen_t addrlen)
{
    struct netconn *conn = priv->conn;
    enum netconn_type type = NETCONNTYPE_GROUP(netconn_type(conn));

    if (type == NETCONN_TCP) {
        size_t written = 0;
        err_t err = netconn_write_partly(conn, buffer, size, NETCONN_COPY, &written);
        if (ERR_WOULDBLOCK == err) {
            // ERR_WOULDBLOCK because of too much data in non-block mode,
            //  send again in block mode as a workaround
            netconn_set_nonblocking(conn, 0);
            err = netconn_write_partly(conn, buffer, size, 0, &written);
            netconn_set_nonblocking(conn, 1);
        }
        int sockerr = __netconn_return(err);
        return sockerr ? sockerr : written;
    } else if ((type == NETCONN_UDP) || (type == NETCONN_RAW)) {
        if (size > LWIP_MIN(0xFFFF, SSIZE_MAX)) {
            return SOCKET_ERROR;
        }

        struct netbuf buf = { 0 };
        uint16_t remote_port;
        if (dst_addr) {
            __sockaddr_to_ipaddr_port(dst_addr, &buf.addr, &remote_port);
        } else {
            remote_port = 0;
            ip_addr_set_any(NETCONNTYPE_ISIPV6(netconn_type(conn)), &buf.addr);
        }
        netbuf_fromport(&buf) = remote_port;

#if LWIP_NETIF_TX_SINGLE_PBUF
        if (NULL == netbuf_alloc(&buf, short_size)) {
            return SOCKET_ERROR;
        }
#   if LWIP_CHECKSUM_ON_COPY
        if (type != NETCONN_RAW) {
            uint16_t chksum = LWIP_CHKSUM_COPY(buf.p->payload, buffer, size);
            netbuf_set_chksum(&buf, chksum);
        } else
#   endif
        {
            memcpy(buf.p->payload, data, size);
        }
#else
        if (ERR_OK != netbuf_ref(&buf, buffer, size)) {
            return SOCKET_ERROR;
        }
#endif

#if LWIP_IPV4 && LWIP_IPV6
        /* Dual-stack: Unmap IPv4 mapped IPv6 addresses */
        if (IP_IS_V6_VAL(buf.addr) && ip6_addr_isipv4mappedipv6(ip_2_ip6(&buf.addr))) {
            unmap_ipv4_mapped_ipv6(ip_2_ip4(&buf.addr), ip_2_ip6(&buf.addr));
            IP_SET_TYPE_VAL(buf.addr, IPADDR_TYPE_V4);
        }
#endif

        err_t err = netconn_send(conn, &buf);
        netbuf_free(&buf);
        int sockerr = __netconn_return(err);
        return sockerr ? sockerr : size;
    }
    return SOCKET_ERROR;
}

#if __IS_COMPILER_IAR__
//! transfer of control bypasses initialization
#   pragma diag_suppress=pe546
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

static ssize_t __vsf_linux_socket_inet_recv(vsf_linux_socket_inet_priv_t *priv, void *buffer, size_t size, int flags,
                    struct sockaddr *src_addr, socklen_t *addrlen)
{
    struct netconn *conn = priv->conn;
    enum netconn_type type = NETCONNTYPE_GROUP(netconn_type(conn));
    u16_t len = 0, pos = 0;

    struct pbuf *pbuf = priv->last.pbuf;
    if (NULL == pbuf) {
        err_t err = ERR_OK;

    recv_next:
        if ((type == NETCONN_UDP) || (type == NETCONN_RAW)) {
            struct netbuf *netbuf;
            err = netconn_recv(conn, &netbuf);
            // for the latest lwip, use netconn_recv_udp_raw_netbuf_flags
//            err = netconn_recv_udp_raw_netbuf_flags(conn, &netbuf, flags);
            if (ERR_OK == err) {
                if (priv->last.netbuf != NULL) {
                    netbuf_chain(priv->last.netbuf, netbuf);
                } else {
                    priv->last.netbuf = netbuf;
                }
            }
        } else if (type == NETCONN_TCP) {
            struct pbuf *pbuf;
            err = netconn_recv_tcp_pbuf(conn, &pbuf);
            // for the latest lwip, use netconn_recv_tcp_pbuf_flags
//            err = netconn_recv_tcp_pbuf_flags(conn, &pbuf, flags);
            if (ERR_OK == err) {
                if (priv->last.pbuf != NULL) {
                    pbuf_chain(priv->last.pbuf, pbuf);
                } else {
                    priv->last.pbuf = pbuf;
                }
            } else if (ERR_CLSD == err) {
                // remote closed, return 0
                return 0;
            } else if (ERR_WOULDBLOCK == err) {
                errno = EAGAIN;
            } else {
                errno = EIO;
            }
        }

        if (err != ERR_OK) {
            return 0 == len ? -1 : len;
        }
        LOCK_TCPIP_CORE();
            conn->callback(conn, NETCONN_EVT_RCVPLUS, 0);
        UNLOCK_TCPIP_CORE();
        if (priv->last.netbuf != NULL) {
            priv->last.pbuf = priv->last.netbuf->p;
        }
        pbuf = priv->last.pbuf;
    }


    u16_t curlen = LWIP_MIN(pbuf->tot_len, size);
    pbuf_copy_partial(pbuf, buffer, curlen, pos);
    len += curlen;
    size -= curlen;
    pos += curlen;
    if (!(flags & MSG_PEEK)) {
        pbuf = pbuf_free_header(pbuf, curlen);
        if (priv->last.netbuf != NULL) {
            priv->last.netbuf->ptr = priv->last.netbuf->p = pbuf;
        }
        priv->last.pbuf = pbuf;
        if (NULL == priv->last.pbuf) {
            LOCK_TCPIP_CORE();
                conn->callback(conn, NETCONN_EVT_RCVMINUS, 0);
            UNLOCK_TCPIP_CORE();
        }
        pos = 0;
    }
    if ((flags & MSG_WAITALL) && (size > 0)) {
        // PE546 in IAR: transfer of control by0passes initialization of: err
        goto recv_next;
    }

    if ((type == NETCONN_UDP) || (type == NETCONN_RAW)) {
        vsf_linux_sockaddr_t addr;
        socklen_t src_addrlen;
        __ipaddr_port_to_sockaddr(&addr.sa, netbuf_fromaddr(priv->last.netbuf), netbuf_fromport(priv->last.netbuf));
        if (AF_INET == addr.sa.sa_family) {
            src_addrlen = sizeof(addr.in);
        } else {
            src_addrlen = sizeof(addr.in6);
        }

        if (addrlen != NULL) {
            *addrlen = src_addrlen;
        }
        if (src_addr != NULL) {
            memcpy(src_addr, &addr, src_addrlen);
        }
        if (!(flags & MSG_PEEK)) {
            VSF_LINUX_ASSERT(priv->last.netbuf != NULL);
            netbuf_delete(priv->last.netbuf);
            priv->last.netbuf = NULL;

            if (priv->last.pbuf != NULL) {
                priv->last.pbuf = NULL;
                LOCK_TCPIP_CORE();
                    conn->callback(conn, NETCONN_EVT_RCVMINUS, 0);
                UNLOCK_TCPIP_CORE();
            }
        }
    }

    return len;
}

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

static void __vsf_linux_socket_inet_lwip_evthandler(struct netconn *conn, enum netconn_evt evt, u16_t len)
{
    VSF_LINUX_ASSERT(conn != NULL);
//    SYS_ARCH_DECL_PROTECT(lev);
    int s = conn->socket;

    // warning: before fd is created, conn can receive up to 256 NETCONN_EVT_RCVPLUS
    if ((s < 0) && (s >= (-1 - 256))) {
        if (evt == NETCONN_EVT_RCVPLUS) {
            conn->socket--;
        }
        return;
    }

    vsf_linux_fd_t *sfd = (vsf_linux_fd_t *)conn->socket;
    if (NULL == sfd) {
        return;
    }

    vsf_linux_socket_inet_priv_t *priv = (vsf_linux_socket_inet_priv_t *)sfd->priv;
    if (sfd != NULL) {
        switch (evt) {
        case NETCONN_EVT_RCVPLUS:
            if (0 == priv->rxcnt++) {
                vsf_linux_fd_set_status(&priv->use_as__vsf_linux_fd_priv_t, POLLIN, vsf_protect_sched());
            }
            break;
        case NETCONN_EVT_RCVMINUS:
            if (0 == --priv->rxcnt) {
                vsf_linux_fd_clear_status(&priv->use_as__vsf_linux_fd_priv_t, POLLIN, vsf_protect_sched());
            }
            break;
        case NETCONN_EVT_SENDPLUS:
            vsf_linux_fd_set_status(&priv->use_as__vsf_linux_fd_priv_t, POLLOUT, vsf_protect_sched());
            break;
        case NETCONN_EVT_SENDMINUS:
            vsf_linux_fd_clear_status(&priv->use_as__vsf_linux_fd_priv_t, POLLOUT, vsf_protect_sched());
            break;
        case NETCONN_EVT_ERROR:
            // TODO: issue POLLERR?
            break;
        }
    }
}

// socket fd

static struct netif * __vsf_linux_lwip_get_netif_by_ifreq(struct ifreq *ifr)
{
    if (ifr->ifr_name[0] != '\0') {
        return netif_find(ifr->ifr_name);
    } else if (ifr->ifr_ifindex > 0) {
        return netif_get_by_index(ifr->ifr_ifindex);
    }
    return NULL;
}

static short __vsf_linux_lwip_netif_flags_to_ifr_flags(struct netif *netif)
{
    short flags = 0;
    if (netif->flags & NETIF_FLAG_UP) {
        flags |= IFF_UP;
    }
    if (netif->flags & NETIF_FLAG_BROADCAST) {
        flags |= IFF_BROADCAST;
    }
    if ((netif->name[0] == 'l') && (netif->name[1] == 'o')) {
        flags |= IFF_LOOPBACK;
    }
    return flags;
}

static int __vsf_linux_lwip_socket_inet_fcntl(vsf_linux_fd_t *sfd, int cmd, uintptr_t arg)
{
    struct netif *netif;

    union {
        uintptr_t arg;
        struct ifreq *ifr;
        struct ifconf *ifconf;
    } u;
    u.arg = arg;

    switch (cmd) {
    case F_SETFL:
        return __vsf_linux_socket_inet_fcntl(sfd, cmd, arg);
    case SIOCGIFNAME:
        netif = __vsf_linux_lwip_get_netif_by_ifreq(u.ifr);
        if (NULL == netif) {
            break;
        }

        u.ifr->ifr_name[0] = netif->name[0];
        u.ifr->ifr_name[1] = netif->name[1];
        u.ifr->ifr_name[2] = '\0';
        return 0;
        break;
    case SIOCGIFCONF:
        // TODO: add support
        break;
    case SIOCGIFFLAGS:
        netif = __vsf_linux_lwip_get_netif_by_ifreq(u.ifr);
        if (NULL == netif) {
            break;
        }

        u.ifr->ifr_flags = __vsf_linux_lwip_netif_flags_to_ifr_flags(netif);
        return 0;
    case SIOCGIFADDR:
        netif = __vsf_linux_lwip_get_netif_by_ifreq(u.ifr);
        if (NULL == netif) {
            break;
        }

        __ipaddr_port_to_sockaddr(&u.ifr->ifr_addr, &netif->ip_addr, 0);
        return 0;
    case SIOCGIFHWADDR:
        netif = __vsf_linux_lwip_get_netif_by_ifreq(u.ifr);
        if (NULL == netif) {
            break;
        }

        memcpy(u.ifr->ifr_hwaddr.sa_data, netif->hwaddr, NETIF_MAX_HWADDR_LEN);
        return 0;
    case SIOCSIFADDR:
    case SIOCSIFFLAGS:
        // not supported
        break;
    }
    return -1;
}

static ssize_t __vsf_linux_socket_inet_read(vsf_linux_fd_t *sfd, void *buf, size_t count)
{
    vsf_linux_socket_inet_priv_t *priv = (vsf_linux_socket_inet_priv_t *)sfd->priv;
    return __vsf_linux_socket_inet_recv(priv, buf, count, sfd->cur_rdflags, NULL, NULL);
}

static ssize_t __vsf_linux_socket_inet_write(vsf_linux_fd_t *sfd, const void *buf, size_t count)
{
    vsf_linux_socket_inet_priv_t *priv = (vsf_linux_socket_inet_priv_t *)sfd->priv;
    return __vsf_linux_socket_inet_send(priv, buf, count, sfd->cur_wrflags, NULL, 0);
}

static int __vsf_linux_socket_inet_close(vsf_linux_fd_t *sfd)
{
    vsf_linux_socket_inet_priv_t *priv = (vsf_linux_socket_inet_priv_t *)sfd->priv;
    struct netconn *conn = priv->conn;

    if (!vsf_dlist_is_empty(&priv->group_list)) {
        ip_addr_t multi_addr, if_addr;

        __vsf_dlist_foreach_next_unsafe(vsf_linux_socket_group_t, node, &priv->group_list) {
            if (AF_INET6 == _->family) {
                ip_addr_copy_from_ip6(multi_addr, _->ip6.multi_addr);
                netconn_join_leave_group_netif(conn, &multi_addr, netif_get_index(_->ip6.netif), NETCONN_LEAVE);
            } else {
                ip_addr_copy_from_ip4(multi_addr, _->ip4.multi_addr);
                ip_addr_copy_from_ip4(if_addr, _->ip4.if_addr);
                netconn_join_leave_group(conn, &multi_addr, &if_addr, NETCONN_LEAVE);
            }
            vsf_heap_free(_);
        }
    }

    conn->socket = 0;
    // for the latest lwip, use netconn_prepare_delete
//    netconn_prepare_delete(conn);
    netconn_delete(conn);
    return 0;
}

// sendto & recvfrom
ssize_t sendto(int sockfd, const void *buffer, size_t size, int flags,
                    const struct sockaddr *dst_addr, socklen_t addrlen)
{
    vsf_linux_fd_t *sfd = vsf_linux_fd_get(sockfd);
    if (!sfd) {
        return -1;
    }

    return __vsf_linux_socket_inet_send((vsf_linux_socket_inet_priv_t *)sfd->priv,
                    buffer, size, flags, dst_addr, addrlen);
}

ssize_t recvfrom(int sockfd, void *buffer, size_t size, int flags,
                    struct sockaddr *src_addr, socklen_t *addrlen)
{
    vsf_linux_fd_t *sfd = vsf_linux_fd_get(sockfd);
    if (!sfd) {
        return -1;
    }

    return __vsf_linux_socket_inet_recv((vsf_linux_socket_inet_priv_t *)sfd->priv,
                    buffer, size, flags, src_addr, addrlen);
}

// netlink

#if VSF_LINUX_SOCKET_USE_NETLINK == ENABLED

static vsf_linux_socket_netlink_msg_t * __vsf_linux_socket_lwip_new_msg(vsf_linux_socket_netlink_priv_t *priv, int msg_size)
{
    int size = NLMSG_ALIGN(sizeof(vsf_linux_socket_netlink_msg_t)) + msg_size;
    vsf_linux_socket_netlink_msg_t *msg = vsf_heap_malloc(size);
    if (NULL == msg) {
        vsf_trace_error("fail to allocate netlink message" VSF_TRACE_CFG_LINEEND);
        return NULL;
    }

    vsf_slist_init_node(vsf_linux_socket_netlink_msg_t, node, msg);
    msg->hdr.nlmsg_len = size;
    msg->hdr.nlmsg_flags = 0;
    msg->hdr.nlmsg_seq = priv->seq++;
    msg->hdr.nlmsg_pid = 0;
    return msg;
}

static void __vsf_linux_socket_lwip_netif_callback(
        struct netif *netif, netif_nsc_reason_t reason, const netif_ext_callback_args_t *args)
{
    vsf_linux_socket_netlink_msg_t *msg = NULL;

    __vsf_dlist_foreach_unsafe(vsf_linux_socket_netlink_priv_t, node, &__vsf_linux_netlink_priv_list) {
        switch (reason) {
        case LWIP_NSC_NETIF_ADDED:
            if (_->groups & RTMGRP_LINK) {
                msg = __vsf_linux_socket_lwip_new_msg(_, sizeof(struct ifinfomsg));
                if (NULL == msg) {
                    break;
                }

                msg->hdr.nlmsg_type = RTM_NEWLINK;

                struct ifinfomsg *ifinfo = (struct ifinfomsg *)((uint8_t *)msg + NLMSG_ALIGN(sizeof(*msg)));
                ifinfo->ifi_family = AF_UNSPEC;
                ifinfo->ifi_type = 0;
                ifinfo->ifi_index = netif_get_index(netif);
                ifinfo->ifi_flags = __vsf_linux_lwip_netif_flags_to_ifr_flags(netif);
                // ifi_change is reserved for future use and should be always set to 0xFFFFFFFF
                ifinfo->ifi_change = 0xFFFFFFFF;
            }
            break;
        case LWIP_NSC_NETIF_REMOVED:
            if (_->groups & RTMGRP_LINK) {
                msg = __vsf_linux_socket_lwip_new_msg(_, sizeof(struct ifinfomsg));
                if (NULL == msg) {
                    break;
                }

                msg->hdr.nlmsg_type = RTM_DELLINK;

                struct ifinfomsg *ifinfo = (struct ifinfomsg *)((uint8_t *)msg + NLMSG_ALIGN(sizeof(*msg)));
                ifinfo->ifi_family = AF_UNSPEC;
                ifinfo->ifi_type = 0;
                ifinfo->ifi_index = netif_get_index(netif);
                ifinfo->ifi_flags = __vsf_linux_lwip_netif_flags_to_ifr_flags(netif);
                // ifi_change is reserved for future use and should be always set to 0xFFFFFFFF
                ifinfo->ifi_change = 0xFFFFFFFF;
            }
            break;
        case LWIP_NSC_IPV4_ADDRESS_CHANGED:
            if (_->groups & RTMGRP_IPV4_IFADDR) {
                msg = __vsf_linux_socket_lwip_new_msg(_, sizeof(struct ifaddrmsg));
                if (NULL == msg) {
                    break;
                }

#if LWIP_IPV4 && LWIP_IPV6
                msg->hdr.nlmsg_type = ip4_addr_isany(&netif->ip_addr.u_addr.ip4) ? RTM_DELADDR : RTM_NEWADDR;
#else
                msg->hdr.nlmsg_type = ip4_addr_isany(&netif->ip_addr) ? RTM_DELADDR : RTM_NEWADDR;
#endif

                struct ifaddrmsg *ifaddr = (struct ifaddrmsg *)((uint8_t *)msg + NLMSG_ALIGN(sizeof(*msg)));
#if LWIP_IPV4 && LWIP_IPV6
                ifaddr->ifa_family = IP_IS_V6_VAL(netif->ip_addr) ? AF_INET6 : AF_INET;
#else
                ifaddr->ifa_family = AF_INET;
#endif
                ifaddr->ifa_prefixlen = 0;
                ifaddr->ifa_flags = 0;
                ifaddr->ifa_scope = 0;
                ifaddr->ifa_index = netif_get_index(netif);
            }
            break;
        }

        if (msg != NULL) {
            vsf_protect_t orig = vsf_protect_sched();
            vsf_slist_queue_enqueue(vsf_linux_socket_netlink_msg_t, node, &_->msg_list, msg);
            vsf_linux_fd_set_status(&_->use_as__vsf_linux_fd_priv_t, POLLIN, orig);
        }
    }
}

static int __vsf_linux_socket_netlink_init(vsf_linux_fd_t *sfd)
{
    vsf_linux_socket_priv_t *socket_priv = (vsf_linux_socket_priv_t *)sfd->priv;
    vsf_linux_socket_netlink_priv_t *priv = (vsf_linux_socket_netlink_priv_t *)socket_priv;

    vsf_slist_init(&priv->msg_list);
    switch (socket_priv->type) {
    case SOCK_RAW:
        switch (socket_priv->protocol) {
        case NETLINK_ROUTE:
            if (!__vsf_linux_netlink_is_callback_installed) {
                __vsf_linux_netlink_is_callback_installed = true;
                netif_add_ext_callback(&__vsf_linux_netlink_lwip_netif_cb, __vsf_linux_socket_lwip_netif_callback);
            }

            priv->seq = 0;
            vsf_dlist_add_to_head(vsf_linux_socket_netlink_priv_t, node, &__vsf_linux_netlink_priv_list, priv);
            return 0;
        }
        break;
    }
    return -1;
}

static int __vsf_linux_socket_netlink_fini(vsf_linux_socket_priv_t *socket_priv, int how)
{
    vsf_linux_socket_netlink_priv_t *priv = (vsf_linux_socket_netlink_priv_t *)socket_priv;

    switch (socket_priv->type) {
    case SOCK_RAW:
        switch (socket_priv->protocol) {
        case NETLINK_ROUTE:
            vsf_dlist_remove(vsf_linux_socket_netlink_priv_t, node, &__vsf_linux_netlink_priv_list, priv);

            vsf_linux_socket_netlink_msg_t *msg;
            while (!vsf_slist_queue_is_empty(&priv->msg_list)) {
                vsf_slist_queue_dequeue(vsf_linux_socket_netlink_msg_t, node, &priv->msg_list, msg);
                vsf_heap_free(msg);
            }
            return 0;
        }
        break;
    }
    return -1;
}

static int __vsf_linux_socket_netlink_bind(vsf_linux_socket_priv_t *socket_priv, const struct sockaddr *addr, socklen_t addrlen)
{
    struct sockaddr_nl *addr_nl = (struct sockaddr_nl *) addr;
    vsf_linux_socket_netlink_priv_t *priv = (vsf_linux_socket_netlink_priv_t *)socket_priv;
    priv->groups = addr_nl->nl_groups;
    return 0;
}

static ssize_t __vsf_linux_socket_netlink_read(vsf_linux_fd_t *sfd, void *buf, size_t count)
{
    vsf_linux_socket_priv_t *socket_priv = (vsf_linux_socket_priv_t *)sfd->priv;
    vsf_linux_socket_netlink_priv_t *priv = (vsf_linux_socket_netlink_priv_t *)socket_priv;
    bool is_block = vsf_linux_fd_is_block(sfd);
    vsf_linux_socket_netlink_msg_t *msg;
    vsf_protect_t orig;
    size_t count_remain = count, curlen;

    vsf_linux_trigger_t trig;
    vsf_linux_trigger_init(&trig);

again:
    orig = vsf_protect_sched();
    if (vsf_slist_queue_is_empty(&priv->msg_list)) {
        if (is_block) {
            if (!vsf_linux_fd_pend_events(&priv->use_as__vsf_linux_fd_priv_t, POLLIN, &trig, orig)) {
                // triggered by signal
                return -1;
            }
            goto again;
        } else {
            vsf_unprotect_sched(orig);
            errno = EAGAIN;
            return -1;
        }
    }

    while (count_remain > 0) {
        vsf_slist_queue_peek(vsf_linux_socket_netlink_msg_t, node, &priv->msg_list, msg);
        if (NULL == msg) {
            break;
        }

        curlen = msg->hdr.nlmsg_len;
        if (curlen > count_remain) {
            break;
        }

        vsf_slist_queue_dequeue(vsf_linux_socket_netlink_msg_t, node, &priv->msg_list, msg);
        vsf_unprotect_sched(orig);

        memcpy(buf, &msg->hdr, curlen);
        buf = (void *)((uint8_t *)buf + curlen);
        count_remain -= curlen;

        vsf_heap_free(msg);

        orig = vsf_protect_sched();
        if (vsf_slist_queue_is_empty(&priv->msg_list)) {
            break;
        }

        // alignment fix
        curlen &= NLMSG_ALIGNTO - 1;
        if (curlen) {
            curlen = NLMSG_ALIGNTO - curlen;
            buf = (void *)((uint8_t *)buf + curlen);
            count_remain -= curlen;
        }
    }
    vsf_unprotect_sched(orig);

    return count - count_remain;
}

#endif

// ifaddrs.h
int getifaddrs(struct ifaddrs **ifaddrs)
{
    return 0;
}

void freeifaddrs(struct ifaddrs *ifaddrs)
{
}

// netdb.h
// none thread safety
typedef struct gethostbyname_param_t {
    ip_addr_t ipaddr;
    vsf_eda_t *eda;
} gethostbyname_param_t;
static void __inet_dns_on_found(const char *name, const ip_addr_t *ipaddr, void *callback_arg)
{
    gethostbyname_param_t *param = callback_arg;
    if (ipaddr != NULL) {
        param->ipaddr = *ipaddr;
        vsf_eda_post_evt(param->eda, VSF_EVT_USER);
    } else {
        vsf_eda_post_evt(param->eda, VSF_EVT_USER + 1);
    }
}

int __inet_gethostbyname(const char *name, in_addr_t *addr)
{
    gethostbyname_param_t param = {
        .eda    = vsf_eda_get_cur(),
    };
    VSF_LINUX_ASSERT(param.eda != NULL);
    err_t err;

    LOCK_TCPIP_CORE();
        err = dns_gethostbyname(name, &param.ipaddr, __inet_dns_on_found, &param);
    UNLOCK_TCPIP_CORE();

    if (ERR_ARG == err) {
        return -1;
    } else if (ERR_OK != err) {
        vsf_evt_t evt = vsf_thread_wait();
        if (    (evt != VSF_EVT_USER)
#if LWIP_IPV4 && LWIP_IPV6
            ||  (IPADDR_TYPE_V6 == param.ipaddr.type)
#endif
        ) {
            return -1;
        }
    }

    if (addr != NULL) {
#if LWIP_IPV4 && LWIP_IPV6
        *addr = param.ipaddr.u_addr.ip4.addr;
#elif LWIP_IPV4
        *addr = param.ipaddr.addr;
#endif
    }
    return 0;
}

#endif
