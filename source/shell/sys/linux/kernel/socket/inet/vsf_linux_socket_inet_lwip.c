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
#   include "../../../include/netinet/in.h"
#   include "../../../include/netinet/tcp.h"
#   include "../../../include/arpa/inet.h"
#   include "../../../include/ifaddrs.h"
#   include "../../../include/poll.h"
#else
#   include <unistd.h>
#   include <errno.h>
#   include <sys/socket.h>
#   include <sys/time.h>
#   include <netinet/in.h>
#   include <netinet/tcp.h>
#   include <arpa/inet.h>
#   include <ifaddrs.h>
#   include <poll.h>
#endif
#include "../vsf_linux_socket.h"

#include "lwip/tcpip.h"
#include "lwip/api.h"
#include "lwip/ip.h"
#include "lwip/tcp.h"
#include "lwip/dns.h"

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

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_linux_socket_inet_priv_t {
    implement(vsf_linux_socket_priv_t)

    struct netconn          *conn;
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

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

int __vsf_linux_socket_inet_fcntl(vsf_linux_fd_t *sfd, int cmd, long arg);
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

/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/

const vsf_linux_socket_op_t vsf_linux_socket_inet_op = {
    .fdop               = {
        .priv_size      = sizeof(vsf_linux_socket_inet_priv_t),
        .fn_fcntl       = __vsf_linux_socket_inet_fcntl,
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
        ip6_addr_clear_zone(ip_2_ip6(ipaddr));

        if (ip6_addr_has_scope(ip_2_ip6(ipaddr), IP6_UNKNOWN)) {
            ip6_addr_set_zone(ip_2_ip6(ipaddr), (u8_t)(sockaddr_in6->sin6_scope_id));
        }
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
        sockaddr_in6->sin6_scope_id = ip6_addr_zone(ip_2_ip6(ipaddr));
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

    switch (socket_priv->type) {
    case SOCK_DGRAM:
        if (socket_priv->protocol != IPPROTO_UDP) {
            return INVALID_SOCKET;
        }
        conn_type = NETCONN_UDP;
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
    conn = netconn_new_with_callback(conn_type, __vsf_linux_socket_inet_lwip_evthandler);
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
            if (NETCONNTYPE_GROUP(conn->type) != NETCONN_UDP) {
                return ENOPROTOOPT;
            }
            // fall through
        case SO_KEEPALIVE:
            optname = lwip_sockopt_to_ipopt(optname);
            if (*(const int *)optval) {
                ip_set_option(conn->pcb.ip, optname);
            } else {
                ip_reset_option(conn->pcb.ip, optname);
            }
            break;
        case SO_RCVBUF:
            netconn_set_recvbufsize(conn, *(const int *)optval);
            break;
        case SO_REUSEADDR:
            if (    (optname == SO_BROADCAST)
                &&  (NETCONNTYPE_GROUP(conn->type) != NETCONN_UDP)) {
                return ENOPROTOOPT;
            }

            optname = lwip_sockopt_to_ipopt(optname);
            if (*(const int *)optval) {
                ip_set_option(conn->pcb.ip, optname);
            } else {
                ip_reset_option(conn->pcb.ip, optname);
            }
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
        default:
            // TODO: add support
            VSF_LINUX_ASSERT(false);
            break;
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
        case TCP_KEEPALIVE:
            conn->pcb.tcp->keep_idle = (u32_t)(*(const int *)optval);
            break;
        default:
            VSF_LINUX_ASSERT(false);
            break;
        }
        break;
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
            if (NETCONNTYPE_GROUP(conn->type) != NETCONN_UDP) {
                return ENOPROTOOPT;
            }
            // fall through
        case SO_KEEPALIVE:
            optname = lwip_sockopt_to_ipopt(optname);
            *(int *)optval = ip_get_option(conn->pcb.ip, optname);
            break;
        case SO_RCVBUF:
            *(int *)optval = netconn_get_recvbufsize(conn);
            break;
        case SO_REUSEADDR:
            if (    (optname == SO_BROADCAST)
                &&  (NETCONNTYPE_GROUP(conn->type) != NETCONN_UDP)) {
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
        default:
            VSF_LINUX_ASSERT(false);
            break;
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

    if (NETCONNTYPE_GROUP(netconn_type(conn)) == NETCONN_TCP) {
        size_t written = 0;
        err_t err = netconn_write_partly(conn, buffer, size, NETCONN_COPY, &written);
        int sockerr = __netconn_return(err);
        return sockerr ? sockerr : written;
    } else if (NETCONNTYPE_GROUP(netconn_type(conn)) == NETCONN_UDP) {
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
        if (NETCONNTYPE_GROUP(netconn_type(conn)) != NETCONN_RAW) {
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
    u16_t len = 0, pos = 0;

    struct pbuf *pbuf = priv->last.pbuf;
    if (NULL == pbuf) {
        err_t err = ERR_OK;

    recv_next:
        if (NETCONNTYPE_GROUP(netconn_type(conn)) == NETCONN_UDP) {
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
        } else if (NETCONNTYPE_GROUP(netconn_type(conn)) == NETCONN_TCP) {
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

    if (NETCONNTYPE_GROUP(netconn_type(conn)) == NETCONN_UDP) {
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
static ssize_t __vsf_linux_socket_inet_read(vsf_linux_fd_t *sfd, void *buf, size_t count)
{
    vsf_linux_socket_inet_priv_t *priv = (vsf_linux_socket_inet_priv_t *)sfd->priv;
    return __vsf_linux_socket_inet_recv(priv, buf, count, 0, NULL, NULL);
}

static ssize_t __vsf_linux_socket_inet_write(vsf_linux_fd_t *sfd, const void *buf, size_t count)
{
    vsf_linux_socket_inet_priv_t *priv = (vsf_linux_socket_inet_priv_t *)sfd->priv;
    return __vsf_linux_socket_inet_send(priv, buf, count, 0, NULL, 0);
}

static int __vsf_linux_socket_inet_close(vsf_linux_fd_t *sfd)
{
    vsf_linux_socket_inet_priv_t *priv = (vsf_linux_socket_inet_priv_t *)sfd->priv;
    struct netconn *conn = priv->conn;

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

// ifaddrs.h
int getifaddrs(struct ifaddrs **ifaddrs)
{
    return 0;
}

void freeifaddrs(struct ifaddrs *ifaddrs)
{
}

// netdb.h
// none thread safty
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
            ||  (IPADDR_TYPE_V6 == ipaddr.type)
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
