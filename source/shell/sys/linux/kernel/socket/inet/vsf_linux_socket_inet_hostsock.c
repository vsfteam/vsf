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
    &&  VSF_LINUX_SOCKET_USE_INET == ENABLED                                    \
    && (defined(__WIN__) || defined(__LINUX__) || defined(__linux__))

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../../../include/errno.h"
#   include "../../../include/sys/types.h"
#else
#   include <errno.h>
#   include <sys/types.h>
#endif

#define __VSF_LINUX_SOCKET_CLASS_IMPLEMENT
#include "utilities/ooc_class.h"

struct dirent {
    long d_ino;
    off_t d_off;
    unsigned short d_reclen;
    unsigned char d_type;
    char *d_name;
};

#define __VSF_LINUX_FS_CLASS_INHERIT__
#include "../../fs/vsf_linux_fs.h"

#if defined(__WIN__)
#   include <WinSock2.h>
#   pragma comment (lib, "ws2_32.lib")
#endif

/*============================ MACROS ========================================*/

#if VSF_LINUX_SOCKET_CFG_WRAPPER != ENABLED
#   error VSF_LINUX_SOCKET_CFG_WRAPPER MUST be enabled to avoid conflicts with WinSock APIs
#endif

#define VSF_LINUX_SOCKET_WRAPPER(__api)     VSF_SHELL_WRAPPER(vsf_linux_socket, __api)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

// need to sync types/constants below with the real definitions in vsf
// from socket.h
enum {
    VSF_LINUX_SOCKET_IPPROTO_TCP         = 6,
    VSF_LINUX_SOCKET_IPPROTO_UDP         = 17,
};
#define VSF_LINUX_SOCKET_SOL_SOCKET      0xFFFF
enum {
    VSF_LINUX_SOCKET_SO_DEBUG            = 1,
    VSF_LINUX_SOCKET_SO_REUSEADDR,
    VSF_LINUX_SOCKET_SO_ACCEPTCONN,
    VSF_LINUX_SOCKET_SO_KEEPALIVE,
    VSF_LINUX_SOCKET_SO_BROADCAST,

    VSF_LINUX_SOCKET_SO_TYPE,
    VSF_LINUX_SOCKET_SO_ERROR,
    VSF_LINUX_SOCKET_SO_SNDBUF,
    VSF_LINUX_SOCKET_SO_RCVBUF,
    VSF_LINUX_SOCKET_SO_RCVTIMEO,
    VSF_LINUX_SOCKET_SO_SNDTIMEO,
    VSF_LINUX_SOCKET_SO_NONBLOCK,
};
typedef uint32_t socklen_t;
typedef uint16_t vsf_linux_socket_sa_family_t;
struct vsf_linux_socket_sockaddr {
    vsf_linux_socket_sa_family_t sa_family;
    char sa_data[14];
};

// from netinet/in.h
typedef uint32_t in_addr_t;

// from netinet/tcp.h
#define VSF_LINUX_SOCKET_TCP_NODELAY    1
#define VSF_LINUX_SOCKET_TCP_KEEPALIVE  2

// from vsf_linux_socket.h
vsf_dcl_class(vsf_linux_socket_priv_t)

typedef struct vsf_linux_socket_op_t {
    vsf_linux_fd_op_t fdop;

    int (*fn_init)(vsf_linux_fd_t *sfd);
    int (*fn_fini)(vsf_linux_socket_priv_t *priv, int how);
    int (*fn_connect)(vsf_linux_socket_priv_t *priv, const struct vsf_linux_socket_sockaddr *addr, socklen_t addrlen);
    int (*fn_listen)(vsf_linux_socket_priv_t *priv, int backlog);
    int (*fn_accept)(vsf_linux_socket_priv_t *priv, struct vsf_linux_socket_sockaddr *addr, socklen_t *addr_len);
    int (*fn_bind)(vsf_linux_socket_priv_t *priv, const struct vsf_linux_socket_sockaddr *addr, socklen_t addrlen);

    int (*fn_getsockopt)(vsf_linux_socket_priv_t *priv, int level, int optname, void *optval, socklen_t *optlen);
    int (*fn_setsockopt)(vsf_linux_socket_priv_t *priv, int level, int optname,const void *optval, socklen_t optlen);

    int (*fn_getpeername)(vsf_linux_socket_priv_t *socket_priv, struct vsf_linux_socket_sockaddr *addr, socklen_t *addrlen);
    int (*fn_getsockname)(vsf_linux_socket_priv_t *socket_priv, struct vsf_linux_socket_sockaddr *addr, socklen_t *addrlen);
} vsf_linux_socket_op_t;

vsf_class(vsf_linux_socket_priv_t) {
    public_member(
        implement(vsf_linux_fd_priv_t)

        int domain;
        int type;
        int protocol;
        const vsf_linux_socket_op_t *sockop;
    )
};

// winsock2 private
typedef struct vsf_linux_socket_inet_priv_t {
    implement(vsf_linux_socket_priv_t)
    int host_sock;
} vsf_linux_socket_inet_priv_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

int __vsf_linux_socket_inet_fcntl(vsf_linux_fd_t *sfd, int cmd, long arg);
static ssize_t __vsf_linux_socket_inet_read(vsf_linux_fd_t *sfd, void *buf, size_t count);
static ssize_t __vsf_linux_socket_inet_write(vsf_linux_fd_t *sfd, const void *buf, size_t count);
static int __vsf_linux_socket_inet_close(vsf_linux_fd_t *sfd);

static int __vsf_linux_socket_inet_init(vsf_linux_fd_t *sfd);
static int __vsf_linux_socket_inet_fini(vsf_linux_socket_priv_t *socket_priv, int how);
static int __vsf_linux_socket_inet_connect(vsf_linux_socket_priv_t *socket_priv, const struct vsf_linux_socket_sockaddr *addr, socklen_t addrlen);
static int __vsf_linux_socket_inet_listen(vsf_linux_socket_priv_t *socket_priv, int backlog);
static int __vsf_linux_socket_inet_accept(vsf_linux_socket_priv_t *socket_priv, struct vsf_linux_socket_sockaddr *addr, socklen_t *addr_len);
static int __vsf_linux_socket_inet_bind(vsf_linux_socket_priv_t *socket_priv, const struct vsf_linux_socket_sockaddr *addr, socklen_t addrlen);
static int __vsf_linux_socket_inet_getsockopt(vsf_linux_socket_priv_t *socket_priv, int level, int optname, void *optval, socklen_t *optlen);
static int __vsf_linux_socket_inet_setsockopt(vsf_linux_socket_priv_t *socket_priv, int level, int optname,const void *optval, socklen_t optlen);
static int __vsf_linux_socket_inet_getpeername(vsf_linux_socket_priv_t *socket_priv, struct vsf_linux_socket_sockaddr *addr, socklen_t *addrlen);
static int __vsf_linux_socket_inet_getsockname(vsf_linux_socket_priv_t *socket_priv, struct vsf_linux_socket_sockaddr *addr, socklen_t *addrlen);

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

static int __vsf_linux_socket_inet_init(vsf_linux_fd_t *sfd)
{
    vsf_linux_socket_priv_t *socket_priv = (vsf_linux_socket_priv_t *)sfd->priv;
    vsf_linux_socket_inet_priv_t *priv = (vsf_linux_socket_inet_priv_t *)socket_priv;

    switch (socket_priv->type) {
    case SOCK_DGRAM:
        
        break;
    case SOCK_STREAM:
        
        break;
    default:
        return INVALID_SOCKET;
    }

    return 0;
}

static int __vsf_linux_socket_inet_fini(vsf_linux_socket_priv_t *socket_priv, int how)
{
    vsf_linux_socket_inet_priv_t *priv = (vsf_linux_socket_inet_priv_t *)socket_priv;
    return 0;
}

static int __vsf_linux_socket_inet_setsockopt(vsf_linux_socket_priv_t *socket_priv, int level, int optname, const void *optval, socklen_t optlen)
{
    vsf_linux_socket_inet_priv_t *priv = (vsf_linux_socket_inet_priv_t *)socket_priv;

    switch (level) {
    case VSF_LINUX_SOCKET_SOL_SOCKET:
        switch (optname) {
        case VSF_LINUX_SOCKET_SO_BROADCAST:
            break;
        case VSF_LINUX_SOCKET_SO_KEEPALIVE:
            break;
        case VSF_LINUX_SOCKET_SO_RCVBUF:
            break;
        case VSF_LINUX_SOCKET_SO_REUSEADDR:
            break;
        case VSF_LINUX_SOCKET_SO_RCVTIMEO:
            break;
        case VSF_LINUX_SOCKET_SO_SNDTIMEO:
            break;
        case VSF_LINUX_SOCKET_SO_NONBLOCK:
            break;
        default:
            // TODO: add support
            VSF_LINUX_ASSERT(false);
            break;
        }
        break;
    case VSF_LINUX_SOCKET_IPPROTO_TCP:
        switch (optname) {
        case VSF_LINUX_SOCKET_TCP_NODELAY:
            break;
        case VSF_LINUX_SOCKET_TCP_KEEPALIVE:
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

static int __vsf_linux_socket_inet_getsockopt(vsf_linux_socket_priv_t *socket_priv, int level, int optname, void *optval, socklen_t *optlen)
{
    vsf_linux_socket_inet_priv_t *priv = (vsf_linux_socket_inet_priv_t *)socket_priv;

    switch (level) {
    case VSF_LINUX_SOCKET_SOL_SOCKET:
        switch (optname) {
        case VSF_LINUX_SOCKET_SO_BROADCAST:
            break;
        case VSF_LINUX_SOCKET_SO_KEEPALIVE:
            break;
        case VSF_LINUX_SOCKET_SO_RCVBUF:
            break;
        case VSF_LINUX_SOCKET_SO_REUSEADDR:
            break;
        case VSF_LINUX_SOCKET_SO_RCVTIMEO:
            break;
        case VSF_LINUX_SOCKET_SO_SNDTIMEO:
            break;
        default:
            VSF_LINUX_ASSERT(false);
            break;
        }
        break;
    case VSF_LINUX_SOCKET_IPPROTO_TCP:
        switch (optname) {
        case VSF_LINUX_SOCKET_TCP_NODELAY:
            break;
        case VSF_LINUX_SOCKET_TCP_KEEPALIVE:
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

static int __vsf_linux_socket_inet_getpeername(vsf_linux_socket_priv_t *socket_priv,
                    struct vsf_linux_socket_sockaddr *addr, socklen_t *addrlen)
{
    vsf_linux_socket_inet_priv_t *priv = (vsf_linux_socket_inet_priv_t *)socket_priv;
    return SOCKET_ERROR;
}

static int __vsf_linux_socket_inet_getsockname(vsf_linux_socket_priv_t *socket_priv,
                    struct vsf_linux_socket_sockaddr *addr, socklen_t *addrlen)
{
    vsf_linux_socket_inet_priv_t *priv = (vsf_linux_socket_inet_priv_t *)socket_priv;
    return SOCKET_ERROR;
}

static int __vsf_linux_socket_inet_accept(vsf_linux_socket_priv_t *socket_priv,
                    struct vsf_linux_socket_sockaddr *addr, socklen_t *addrlen)
{
    vsf_linux_socket_inet_priv_t *priv = (vsf_linux_socket_inet_priv_t *)socket_priv;

    int newsock = socket(socket_priv->domain, socket_priv->type, socket_priv->protocol);
    if (newsock < 0) {
        return INVALID_SOCKET;
    }

    vsf_linux_fd_t *sfd = vsf_linux_fd_get(newsock);
    priv = (vsf_linux_socket_inet_priv_t *)sfd->priv;
    return newsock;
}

static int __vsf_linux_socket_inet_bind(vsf_linux_socket_priv_t *socket_priv,
                    const struct vsf_linux_socket_sockaddr *addr, socklen_t addrlen)
{
    vsf_linux_socket_inet_priv_t *priv = (vsf_linux_socket_inet_priv_t *)socket_priv;
    return SOCKET_ERROR;
}

static int __vsf_linux_socket_inet_connect(vsf_linux_socket_priv_t *socket_priv,
                    const struct vsf_linux_socket_sockaddr *addr, socklen_t addrlen)
{
    vsf_linux_socket_inet_priv_t *priv = (vsf_linux_socket_inet_priv_t *)socket_priv;
    return SOCKET_ERROR;
}

static int __vsf_linux_socket_inet_listen(vsf_linux_socket_priv_t *socket_priv, int backlog)
{
    vsf_linux_socket_inet_priv_t *priv = (vsf_linux_socket_inet_priv_t *)socket_priv;
    return SOCKET_ERROR;
}

static ssize_t __vsf_linux_socket_inet_send(vsf_linux_socket_inet_priv_t *priv, const void *buffer, size_t size, int flags,
                    const struct vsf_linux_socket_sockaddr *dst_addr, socklen_t addrlen)
{
    return SOCKET_ERROR;
}

static ssize_t __vsf_linux_socket_inet_recv(vsf_linux_socket_inet_priv_t *priv, void *buffer, size_t size, int flags,
                    struct vsf_linux_socket_sockaddr *src_addr, socklen_t *addrlen)
{
    return SOCKET_ERROR;
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
    return 0;
}

// sendto & recvfrom
ssize_t VSF_LINUX_SOCKET_WRAPPER(sendto)(int sockfd, const void *buffer, size_t size, int flags,
                    const struct vsf_linux_socket_sockaddr *dst_addr, socklen_t addrlen)
{
    vsf_linux_fd_t *sfd = vsf_linux_fd_get(sockfd);
	if (!sfd) {
		return -1;
	}

    return __vsf_linux_socket_inet_send((vsf_linux_socket_inet_priv_t *)sfd->priv,
                    buffer, size, flags, dst_addr, addrlen);
}

ssize_t VSF_LINUX_SOCKET_WRAPPER(recvfrom)(int sockfd, void *buffer, size_t size, int flags,
                    struct vsf_linux_socket_sockaddr *src_addr, socklen_t *addrlen)
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
int __inet_gethostbyname(const char *name, in_addr_t *addr)
{
    return 0;
}

#endif
