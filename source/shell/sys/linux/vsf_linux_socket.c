/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
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

#include "./vsf_linux_cfg.h"

#if VSF_USE_LINUX == ENABLED && VSF_LINUX_USE_SOCKET == ENABLED

#define __VSF_LINUX_CLASS_INHERIT__
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "./include/unistd.h"
#   include "./include/errno.h"
#   include "./include/sys/socket.h"
#   include "./include/netinet/in.h"
#   include "./include/arpa/inet.h"
#   include "./include/netdb.h"
#else
#   include <unistd.h>
#   include <errno.h>
#   include <sys/socket.h>
#   include <netinet/in.h>
#   include <arpa/inet.h>
#   include <netdb.h>
#endif

#include "lwip/api.h"
#include "lwip/ip.h"

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

typedef struct vsf_linux_socket_priv_t {
    struct netconn          *conn;
    struct {
        struct netbuf       *netbuf;
        struct pbuf         *pbuf;
    } last;
} vsf_linux_socket_priv_t;

typedef union vsf_linux_sockaddr_t {
    struct sockaddr         sa;
    struct sockaddr_in      in;
    struct sockaddr_in6     in6;
} vsf_linux_sockaddr_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

static int __vsf_linux_socket_fcntl(vsf_linux_fd_t *sfd, int cmd, long arg);
static ssize_t __vsf_linux_socket_read(vsf_linux_fd_t *sfd, void *buf, size_t count);
static ssize_t __vsf_linux_socket_write(vsf_linux_fd_t *sfd, const void *buf, size_t count);
static int __vsf_linux_socket_close(vsf_linux_fd_t *sfd);

// APIs not implemented in older lwip
extern struct pbuf * pbuf_free_header(struct pbuf *q, u16_t size);

static void __vsf_linux_socket_lwip_evthandler(struct netconn *conn, enum netconn_evt evt, u16_t len);

/*============================ LOCAL VARIABLES ===============================*/

static const vsf_linux_fd_op_t __vsf_linux_socket_fdop = {
    .priv_size          = sizeof(vsf_linux_socket_priv_t),
    .fn_fcntl           = __vsf_linux_socket_fcntl,
    .fn_read            = __vsf_linux_socket_read,
    .fn_write           = __vsf_linux_socket_write,
    .fn_close           = __vsf_linux_socket_close,
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
//  case SO_REUSEADDR:
//    return SOF_REUSEADDR;
  default:
    LWIP_ASSERT("Unknown socket option", 0);
    return 0;
  }
}

// arpa/inet.h
int inet_aton(const char *cp, struct in_addr *addr)
{
    uint32_t parts[4];
    int num_parts = 0;
    char *endp;

    for (;;) {
        parts[num_parts++] = strtoul(cp, &endp, 0);
        if (cp == endp) {
            return 0;
        }

        if ((*endp != '.') || (num_parts >= 4)) {
            break;
        }
        cp = endp + 1;
    }

    uint32_t val = parts[num_parts - 1];
    switch (num_parts) {
    case 1:     // a        -- 32 bits
        break;
    case 2:     // a.b      -- 8.24 bits
        if (val > 0xFFFFFF) {
            return 0;
        }
        val |= parts[0] << 24;
        break;
    case 3:     // a.b.c    -- 8.8.16 bits
        if (val > 0xFFFF) {
            return 0;
        }
        val |= (parts[0] << 24) | (parts[1] << 16);
        break;
    case 4:     // a.b.c.d  -- 8.8.8.8 bits
        if (val > 0xFF) {
            return 0;
        }
        val |= (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8);
    }
    if (addr) {
        addr->s_addr = htonl(val);
    }
    return 1;
}

in_addr_t inet_addr(const char *cp)
{
    struct in_addr addr;
    if (!inet_aton(cp, &addr)) {
        return INADDR_NONE;
    }
    return addr.s_addr;
}

in_addr_t inet_lnaof(struct in_addr in)
{
    uint32_t val = ntohl(in.s_addr);
    if (IN_CLASSA(val)) {
        return val & IN_CLASSA_HOST;
    } else if (IN_CLASSB(val)) {
        return val & IN_CLASSB_HOST;
    } else {
        return val & IN_CLASSC_HOST;
    }
}

struct in_addr inet_makeaddr(in_addr_t net, in_addr_t lna)
{
    in_addr_t addr;

	if (net < 128) {
		addr = (net << IN_CLASSA_NSHIFT) | (lna & IN_CLASSA_HOST);
    } else if (net < 0x10000) {
		addr = (net << IN_CLASSB_NSHIFT) | (lna & IN_CLASSB_HOST);
    } else if (net < 0x1000000L) {
		addr = (net << IN_CLASSC_NSHIFT) | (lna & IN_CLASSC_HOST);
    } else {
		addr = net | lna;
    }
	addr = htonl(addr);
	return (*(struct in_addr *)&addr);
}

in_addr_t inet_netof(struct in_addr in)
{
    uint32_t val = ntohl(in.s_addr);
    if (IN_CLASSA(val)) {
        return (val & IN_CLASSA_NET) >> IN_CLASSA_NSHIFT;
    } else if (IN_CLASSB(val)) {
        return (val & IN_CLASSB_NET) >> IN_CLASSB_NSHIFT;
    } else {
        return (val & IN_CLASSC_NET) >> IN_CLASSC_NSHIFT;
    }
}

char * inet_ntoa(struct in_addr in)
{
	static char __inet_ntoa_buf[16];
	unsigned char *a = (void *)&in;
	snprintf(__inet_ntoa_buf, sizeof(__inet_ntoa_buf), "%d.%d.%d.%d", a[0], a[1], a[2], a[3]);
	return __inet_ntoa_buf;
}

in_addr_t inet_network(const char *cp)
{
    return ntohl(inet_addr(cp));
}

const char * inet_ntop(int af, const void *src, char *dst, socklen_t size)
{
    switch (af) {
    case AF_INET:
        strlcpy(dst, inet_ntoa(*(struct in_addr *)src), size);
        break;
    case AF_INET6:
        // TODO: add ipv6 support
        return NULL;
    }
    return dst;
}

// socket
int setsockopt(int socket, int level, int optname, const void *optval, socklen_t optlen)
{
    vsf_linux_fd_t *sfd = vsf_linux_get_fd(socket);
    if (NULL == sfd) {
        return SOCKET_ERROR;
    }

    vsf_linux_socket_priv_t *priv = (vsf_linux_socket_priv_t *)sfd->priv;
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
        default:
            // TODO: add support
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

int getsockopt(int socket, int level, int optname, void *optval, socklen_t *optlen)
{
    vsf_linux_fd_t *sfd = vsf_linux_get_fd(socket);
    if (NULL == sfd) {
        return SOCKET_ERROR;
    }

    vsf_linux_socket_priv_t *priv = (vsf_linux_socket_priv_t *)sfd->priv;
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
        default:
            // TODO: add support
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

int getpeername(int socket, struct sockaddr *addr, socklen_t *addrlen)
{
    vsf_linux_fd_t *sfd = vsf_linux_get_fd(socket);
    if (NULL == sfd) {
        return SOCKET_ERROR;
    }

    vsf_linux_socket_priv_t *priv = (vsf_linux_socket_priv_t *)sfd->priv;
    struct netconn *conn = priv->conn;

    ip_addr_t localaddr;
    u16_t port;
    netconn_peer(conn, &localaddr, &port);
    __ipaddr_port_to_sockaddr(addr, &localaddr, port);
    return 0;
}

int getsockname(int socket, struct sockaddr *addr, socklen_t *addrlen)
{
    vsf_linux_fd_t *sfd = vsf_linux_get_fd(socket);
    if (NULL == sfd) {
        return SOCKET_ERROR;
    }

    vsf_linux_socket_priv_t *priv = (vsf_linux_socket_priv_t *)sfd->priv;
    struct netconn *conn = priv->conn;

    ip_addr_t localaddr;
    u16_t port;
    netconn_addr(conn, &localaddr, &port);
    __ipaddr_port_to_sockaddr(addr, &localaddr, port);
    return 0;
}

int accept(int socket, struct sockaddr *addr, socklen_t *addrlen)
{
    vsf_linux_fd_t *sfd = vsf_linux_get_fd(socket);
    if (NULL == sfd) {
        return SOCKET_ERROR;
    }

    vsf_linux_socket_priv_t *priv = (vsf_linux_socket_priv_t *)sfd->priv;
    struct netconn *conn = priv->conn;

    VSF_LINUX_ASSERT(NETCONNTYPE_GROUP(netconn_type(conn)) == NETCONN_TCP);

    struct netconn *newconn;
    err_t err = netconn_accept(conn, &newconn);
    if (err != ERR_OK) {
        return SOCKET_ERROR;
    }

    int newsock = vsf_linux_create_fd(&sfd, &__vsf_linux_socket_fdop);
    if (newsock < 0) {
        netconn_delete(newconn);
        return INVALID_SOCKET;
    }

    priv = (vsf_linux_socket_priv_t *)sfd->priv;
    priv->conn = newconn;

    if ((addr != NULL) && (addrlen != NULL)) {
        ip_addr_t naddr;
        uint16_t port = 0;
        err = netconn_peer(newconn, &naddr, &port);
        if (ERR_OK != err) {
            vsf_linux_delete_fd(newsock);
            return SOCKET_ERROR;
        }

        // TODO: get addr and addrlen if addrlen is large enough
//        __ipaddr_port_to_sockaddr(, &naddr, port);
//        if (*addrlen >
    }

    newconn->socket = (int)sfd;
    newconn->callback = __vsf_linux_socket_lwip_evthandler;

    // maybe callback is already issued(and ignored becase of invalid newconn->socket),
    //  so try trigger if recv data available
    vsf_protect_t orig = vsf_protect_sched();
    int recv_avail;
    SYS_ARCH_GET(newconn->recv_avail, recv_avail);
    if (recv_avail > 0) {
        vsf_linux_fd_rx_trigger(sfd, orig);
    } else {
        vsf_unprotect_sched(orig);
    }
    return newsock;
}

int bind(int socket, const struct sockaddr *addr, socklen_t addrlen)
{
    vsf_linux_fd_t *sfd = vsf_linux_get_fd(socket);
    if (NULL == sfd) {
        return SOCKET_ERROR;
    }

    vsf_linux_socket_priv_t *priv = (vsf_linux_socket_priv_t *)sfd->priv;
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
    return (ERR_OK == err) ? 0 : SOCKET_ERROR;
}

int connect(int socket, const struct sockaddr *addr, socklen_t addrlen)
{
    vsf_linux_fd_t *sfd = vsf_linux_get_fd(socket);
    if (NULL == sfd) {
        return SOCKET_ERROR;
    }

    vsf_linux_socket_priv_t *priv = (vsf_linux_socket_priv_t *)sfd->priv;
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
    return (ERR_OK == err) ? 0 : SOCKET_ERROR;
}

int listen(int socket, int backlog)
{
    vsf_linux_fd_t *sfd = vsf_linux_get_fd(socket);
    if (NULL == sfd) {
        return SOCKET_ERROR;
    }

    vsf_linux_socket_priv_t *priv = (vsf_linux_socket_priv_t *)sfd->priv;
    struct netconn *conn = priv->conn;

    VSF_LINUX_ASSERT(NETCONNTYPE_GROUP(netconn_type(conn)) == NETCONN_TCP);

    if (backlog < 0) {
        backlog = 0;
    } else if (backlog > 0xFF) {
        backlog = 0xFF;
    }

    err_t err = netconn_listen_with_backlog(conn, (u8_t)backlog);
    return (ERR_OK == err) ? 0 : SOCKET_ERROR;
}

ssize_t recv(int socket, void *buffer, size_t size, int flags)
{
    return recvfrom(socket, buffer, size, flags, NULL, NULL);
}

ssize_t send(int socket, const void *buffer, size_t size, int flags)
{
    vsf_linux_fd_t *sfd = vsf_linux_get_fd(socket);
    if (NULL == sfd) {
        return SOCKET_ERROR;
    }

    vsf_linux_socket_priv_t *priv = (vsf_linux_socket_priv_t *)sfd->priv;
    struct netconn *conn = priv->conn;

    if (NETCONNTYPE_GROUP(netconn_type(conn)) == NETCONN_TCP) {
        size_t written = 0;
        err_t err = netconn_write_partly(conn, buffer, size, NETCONN_COPY, &written);
        return (ERR_OK == err) ? (ssize_t)written : SOCKET_ERROR;
    } else if (NETCONNTYPE_GROUP(netconn_type(conn)) == NETCONN_UDP) {
        return sendto(socket, buffer, size, flags, NULL, 0);
    }
    return SOCKET_ERROR;
}

ssize_t recvfrom(int socket, void *buffer, size_t size, int flags,
                    struct sockaddr *src_addr, socklen_t *addrlen)
{
    vsf_linux_fd_t *sfd = vsf_linux_get_fd(socket);
    if (NULL == sfd) {
        return SOCKET_ERROR;
    }

    vsf_linux_socket_priv_t *priv = (vsf_linux_socket_priv_t *)sfd->priv;
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
            }
        }

        if (err != ERR_OK) {
            return 0 == len ? -1 : len;
        }
        if (priv->last.netbuf != NULL) {
            priv->last.pbuf = priv->last.netbuf->p;
        }
        pbuf = priv->last.pbuf;
    }


    u16_t curlen = min(pbuf->tot_len, size);
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
        pos = 0;
    }
    if ((flags & MSG_WAITALL) && (size > 0)) {
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
            priv->last.pbuf = NULL;
        }
    }

    return len;
}

ssize_t sendto(int socket, const void *buffer, size_t size, int flags,
                    const struct sockaddr *dst_addr, socklen_t addrlen)
{
    vsf_linux_fd_t *sfd = vsf_linux_get_fd(socket);
    if (NULL == sfd) {
        return SOCKET_ERROR;
    }

    vsf_linux_socket_priv_t *priv = (vsf_linux_socket_priv_t *)sfd->priv;
    struct netconn *conn = priv->conn;

    VSF_LINUX_ASSERT(NETCONNTYPE_GROUP(netconn_type(conn)) == NETCONN_UDP);

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
    return (ERR_OK == err) ? size : SOCKET_ERROR;
}

int shutdown(int socket, int how)
{
    vsf_linux_fd_t *sfd = vsf_linux_get_fd(socket);
    if (NULL == sfd) {
        return SOCKET_ERROR;
    }

    vsf_linux_socket_priv_t *priv = (vsf_linux_socket_priv_t *)sfd->priv;
    struct netconn *conn = priv->conn;

    VSF_LINUX_ASSERT(NETCONNTYPE_GROUP(netconn_type(conn)) == NETCONN_TCP);
    VSF_LINUX_ASSERT((how & SHUT_RDWR) != 0);

    err_t err = netconn_shutdown(conn, how & SHUT_RD, how & SHUT_WR);
    return (ERR_OK == err) ? 0 : -1;
}

static void __vsf_linux_socket_lwip_evthandler(struct netconn *conn, enum netconn_evt evt, u16_t len)
{
    if (-1 == conn->socket) {
        return;
    }

    vsf_linux_fd_t *sfd = (vsf_linux_fd_t *)conn->socket;
    if (sfd != NULL) {
        switch (evt) {
        case NETCONN_EVT_RCVPLUS:
            vsf_linux_fd_rx_trigger(sfd, vsf_protect_sched());
            break;
        case NETCONN_EVT_RCVMINUS:
            vsf_linux_fd_rx_untrigger(sfd, vsf_protect_sched());
            break;
        case NETCONN_EVT_SENDPLUS:
            vsf_linux_fd_tx_trigger(sfd, vsf_protect_sched());
            break;
        case NETCONN_EVT_SENDMINUS:
            vsf_linux_fd_tx_untrigger(sfd, vsf_protect_sched());
            break;
        }
    }
}

// socket fd
int __vsf_linux_socket_fcntl(vsf_linux_fd_t *sfd, int cmd, long arg)
{
    return 0;
}

static ssize_t __vsf_linux_socket_read(vsf_linux_fd_t *sfd, void *buf, size_t count)
{
    vsf_linux_socket_priv_t *priv = (vsf_linux_socket_priv_t *)sfd->priv;
    struct netconn *conn = priv->conn;

    VSF_LINUX_ASSERT(NETCONNTYPE_GROUP(netconn_type(conn)) == NETCONN_TCP);
    return recv(sfd->fd, buf, count, 0);
}

static ssize_t __vsf_linux_socket_write(vsf_linux_fd_t *sfd, const void *buf, size_t count)
{
    vsf_linux_socket_priv_t *priv = (vsf_linux_socket_priv_t *)sfd->priv;
    struct netconn *conn = priv->conn;

    VSF_LINUX_ASSERT(NETCONNTYPE_GROUP(netconn_type(conn)) == NETCONN_TCP);
    return send(sfd->fd, buf, count, 0);
}

static int __vsf_linux_socket_close(vsf_linux_fd_t *sfd)
{
    vsf_linux_socket_priv_t *priv = (vsf_linux_socket_priv_t *)sfd->priv;
    struct netconn *conn = priv->conn;

    conn->socket = 0;
    // for the latest lwip, use netconn_prepare_delete
//    netconn_prepare_delete(conn);
    netconn_delete(conn);
    return 0;
}

int socket(int domain, int type, int protocol)
{
    struct netconn *conn;
    enum netconn_type conn_type;

    switch (type) {
    case SOCK_DGRAM:
        if (protocol != IPPROTO_UDP) {
            return INVALID_SOCKET;
        }
        conn_type = NETCONN_UDP;
        break;
    case SOCK_STREAM:
        if (protocol != IPPROTO_TCP) {
            // some user code will call socket to create STREAM sock with protocol 0
//            return INVALID_SOCKET;
        }
        conn_type = NETCONN_TCP;
        break;
    default:
        return INVALID_SOCKET;
    }
    conn = netconn_new_with_callback(conn_type, __vsf_linux_socket_lwip_evthandler);
    if (NULL == conn) {
        return INVALID_SOCKET;
    }
#if LWIP_NETBUF_RECVINFO
    else {
        conn->flags &= ~NETCONN_FLAG_PKTINFO;
    }
#endif

    vsf_linux_fd_t *sfd;
    int fd = vsf_linux_create_fd(&sfd, &__vsf_linux_socket_fdop);
    if (fd < 0) {
        netconn_delete(conn);
        return INVALID_SOCKET;
    }

    vsf_linux_socket_priv_t *priv = (vsf_linux_socket_priv_t *)sfd->priv;
    priv->conn = conn;
    VSF_LINUX_ASSERT(sizeof(conn->socket) >= sizeof(sfd));
    conn->socket = (int)sfd;
    return fd;
}

// netdb
const char * gai_strerror(int errcode)
{
    return (const char *)"unknown error";
}

struct hostent * gethostbyaddr(const void *addr, size_t len, int type)
{
    return NULL;
}

struct hostent * gethostbyname(const char *name)
{
    return NULL;
}

int getnameinfo(const struct sockaddr *addr, socklen_t addrlen,
                        char *host, socklen_t hostlen,
                        char *serv, socklen_t servlen, int flags)
{
    return -1;
}

int getaddrinfo(const char *name, const char *service, const struct addrinfo *hints,
                        struct addrinfo **pai)
{
    static const struct addrinfo __default_hints = {
        .ai_flags = AI_V4MAPPED | AI_ADDRCONFIG,
        .ai_family = PF_UNSPEC,
        .ai_socktype = 0,
        .ai_protocol = 0,
        .ai_addrlen = 0,
        .ai_addr = NULL,
        .ai_canonname = NULL,
        .ai_next = NULL,
    };

    if (name != NULL && name[0] == '*' && name[1] == 0) {
        name = NULL;
    }
    if (service != NULL && service[0] == '*' && service[1] == 0) {
        service = NULL;
    }
    if (name == NULL && service == NULL) {
        return EAI_NONAME;
    }
    if (hints == NULL) {
        hints = &__default_hints;
    }
    if (hints->ai_flags
        & ~(    AI_PASSIVE | AI_CANONNAME | AI_NUMERICHOST | AI_ADDRCONFIG
            |   AI_V4MAPPED | AI_NUMERICSERV | AI_ALL)) {
        return EAI_BADFLAGS;
    }
    if ((hints->ai_flags & AI_CANONNAME) && name == NULL) {
        return EAI_BADFLAGS;
    }

    // TODO: re-implement
    struct in_addr addr;
    if (!inet_aton(name, &addr)) {
        return EAI_NONAME;
    }

    struct __addrinfo {
        struct addrinfo info;
        union {
            struct sockaddr sa;
            struct sockaddr_in sa_in;
            struct sockaddr_in6 sa_in6;
        };
    };
    struct __addrinfo *ai = malloc(sizeof(struct __addrinfo));
    if (NULL == ai) {
        return EAI_MEMORY;
    }
    memset(ai, 0, sizeof(struct __addrinfo));

    ai->info.ai_family      = AF_INET;
    ai->info.ai_addr        = &ai->sa;
    ai->sa_in.sin_family    = AF_INET;
    ai->sa_in.sin_addr      = addr;
    ai->info.ai_addrlen     = sizeof(ai->sa_in);
    *pai = &ai->info;
    return 0;
}

void freeaddrinfo(struct addrinfo *ai)
{
    struct addrinfo *p;
    while (ai != NULL) {
        p = ai;
        ai = ai->ai_next;

        if (p->ai_canonname != NULL) {
            free(p->ai_canonname);
        }
        free(p);
    }
}

#endif
