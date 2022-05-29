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
#   include "../../../include/sys/types.h"
#else
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

#include "hal/arch/vsf_arch.h"
#include "utilities/vsf_utilities.h"

#if defined(__WIN__)
#   include <WinSock2.h>
#   pragma comment (lib, "ws2_32.lib")
#elif defined(__LINUX__) || defined(__linux__)
#   include <unistd.h>
#   include <netdb.h>
#   include <sys/socket.h>
#   include <netinet/in.h>
#   include <netinet/tcp.h>
#   include <fcntl.h>
#   include <errno.h>
#endif

/*============================ MACROS ========================================*/

#if VSF_LINUX_SOCKET_CFG_WRAPPER != ENABLED
#   error VSF_LINUX_SOCKET_CFG_WRAPPER MUST be enabled to avoid conflicts with host socket APIs
#endif

#define VSF_LINUX_WRAPPER(__api)            VSF_SHELL_WRAPPER(vsf_linux, __api)
#define VSF_LINUX_SOCKET_WRAPPER(__api)     VSF_SHELL_WRAPPER(vsf_linux_socket, __api)

#ifdef __WIN__
#   define socket_t                         SOCKET
#   define errno                            WSAGetLastError()
#   define ERRNO_WOULDBLOCK                 WSAEWOULDBLOCK
#   define SHUT_RDWR                        SD_BOTH
#elif defined(__LINUX__) || defined(__linux__)
#   define socket_t                         int
#   define ERRNO_WOULDBLOCK                 EWOULDBLOCK
#   define SOCKET_ERROR                     (-1)
#   define INVALID_SOCKET                   (-1)
#   define closesocket(__sock)              close(__sock)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

// need to sync types/constants below with the real definitions in vsf
// from vsf_linux.h
vsf_class(vsf_linux_trigger_t) {
    public_member(
        implement(vsf_trig_t)
    )
#if VSF_LINUX_CFG_SUPPORT_SIG == ENABLED
    private_member(
        vsf_dlist_node_t node;
        void *pending_process;
        int sig;
    )
#endif
};
extern void vsf_linux_trigger_init(vsf_linux_trigger_t *trig);

// from errno.h
#define VSF_LINUX_ERRNO_EAGAIN          11
extern int * __vsf_linux_errno(void);
#define vsf_linux_errno                 (*__vsf_linux_errno())

// from time.h
#ifndef __SUSECONDS_T
#   define __SUSECONDS_T    long int
#endif
typedef __SUSECONDS_T       suseconds_t;
struct vsf_linux_timeval {
    time_t          tv_sec;
    suseconds_t     tv_usec;
};

// from poll.h
#define VSF_LINUX_POLLIN                (1 << 0)
#define VSF_LINUX_POLLOUT               (1 << 1)
typedef int vsf_linux_nfds_t;
struct vsf_linux_pollfd {
    int fd;
    short events;
    short revents;
};
extern int VSF_LINUX_WRAPPER(poll)(struct vsf_linux_pollfd *fds, vsf_linux_nfds_t nfds, int timeout);

// from sys/socket.h
#define VSF_LINUX_SOCKET_INVALID_SOCKET -1
#define VSF_LINUX_SOCKET_SOCKET_ERROR   -1
#define VSF_LINUX_SOCKET_AF_INET        2
#define VSF_LINUX_SOCKET_SOCK_STREAM    1
#define VSF_LINUX_SOCKET_SOCK_DGRAM     2
enum {
    VSF_LINUX_SOCKET_IPPROTO_TCP        = 6,
    VSF_LINUX_SOCKET_IPPROTO_UDP        = 17,
};
#define VSF_LINUX_SOCKET_SOL_SOCKET     0xFFFF
#define VSF_LINUX_SOCKET_SO_REUSEADDR   2
#define VSF_LINUX_SOCKET_SO_ERROR       4
#define VSF_LINUX_SOCKET_SO_BROADCAST   6
#define VSF_LINUX_SOCKET_SO_RCVBUF      8
#define VSF_LINUX_SOCKET_SO_KEEPALIVE   9
#define VSF_LINUX_SOCKET_SO_LINGER      13
#define VSF_LINUX_SOCKET_SO_RCVTIMEO    20
#define VSF_LINUX_SOCKET_SO_SNDTIMEO    21
#define VSF_LINUX_SOCKET_SO_NONBLOCK    100

typedef uint32_t socklen_t;
typedef uint16_t vsf_linux_socket_sa_family_t;
struct vsf_linux_socket_sockaddr {
    vsf_linux_socket_sa_family_t sa_family;
    char sa_data[14];
};
struct vsf_linux_socket_linger {
    int l_onoff;
    int l_linger;
};

// from netinet/in.h
typedef uint32_t in_addr_t;
typedef uint16_t in_port_t;
struct vsf_linux_socket_in_addr {
    // s_addr maybe MACRO
    in_addr_t __s_addr;
};
struct vsf_linux_socket_sockaddr_in {
    vsf_linux_socket_sa_family_t sin_family;
    in_port_t sin_port;
    struct vsf_linux_socket_in_addr sin_addr;
    char sin_zero[8];
};

// from netinet/tcp.h
#define VSF_LINUX_SOCKET_TCP_NODELAY    1
#define VSF_LINUX_SOCKET_TCP_KEEPALIVE  100

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

// hostsock private
typedef struct vsf_linux_socket_inet_priv_t {
    implement(vsf_linux_socket_priv_t)

    socket_t hostsock;
    int hdomain;
    int htype;
    int hprotocol;
    int is_nonblock;
    int rcvto;
    int sndto;

    vsf_dlist_node_t node;
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

struct vsf_linux_socket_inet_hostsock_t {
#ifdef __WIN__
    WSADATA wsaData;
#endif

    bool is_inited;

    // socket pair for event notifier to host irq_thread
    union {
        struct {
            socket_t sock_event_listener;
            socket_t sock_event_notifier;
        };
        socket_t pipefd[2];
    };
    struct sockaddr_in *inaddr;

    struct {
        fd_set rfds;
        fd_set wfds;
    } select;

    vsf_dlist_t sock_list;
    vsf_arch_irq_thread_t irq_thread;
} __vsf_linux_hostsock;

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

static void __vsf_linux_socket_inet_irqthread(void *arg)
{
    vsf_arch_irq_thread_t *irq_thread = arg;
    vsf_linux_socket_inet_priv_t *priv;
    vsf_protect_t orig;
    fd_set rfds;
    fd_set wfds;
    int ret, nfds;

    __vsf_arch_irq_set_background(irq_thread);
#ifndef VSF_ARCH_NO_SOCKET_PAIR
    connect(__vsf_linux_hostsock.sock_event_listener, (const struct sockaddr *)__vsf_linux_hostsock.inaddr, sizeof(*__vsf_linux_hostsock.inaddr));
#endif
    FD_ZERO(&__vsf_linux_hostsock.select.rfds);
    FD_ZERO(&__vsf_linux_hostsock.select.wfds);
    FD_SET(__vsf_linux_hostsock.sock_event_listener, &__vsf_linux_hostsock.select.rfds);

    while (true) {
        __vsf_arch_irq_start(irq_thread);
        orig = vsf_protect_sched();
        rfds = __vsf_linux_hostsock.select.rfds;
        wfds = __vsf_linux_hostsock.select.wfds;
        vsf_dlist_peek_head(vsf_linux_socket_inet_priv_t, node, &__vsf_linux_hostsock.sock_list, priv);
        if (NULL == priv) {
            nfds = __vsf_linux_hostsock.sock_event_listener;
        } else {
            nfds = vsf_max(__vsf_linux_hostsock.sock_event_listener, priv->hostsock);
        }
        vsf_unprotect_sched(orig);
        __vsf_arch_irq_end(irq_thread, false);

        ret = select(nfds + 1, &rfds, &wfds, NULL, NULL);
        if (ret < 0) {
            // if fds are not updated before socket is closed, error will be issued here, just ignore
            continue;
        }

        if ((ret >= 1) && FD_ISSET(__vsf_linux_hostsock.sock_event_listener, &rfds)) {
#ifdef VSF_ARCH_NO_SOCKET_PAIR
            read(__vsf_linux_hostsock.sock_event_listener, (char *)&ret, 1);
#else
            recv(__vsf_linux_hostsock.sock_event_listener, (char *)&ret, 1, 0);
#endif
            FD_CLR(__vsf_linux_hostsock.sock_event_listener, &rfds);
            ret--;
        }
        if (ret > 0) {
            __vsf_arch_irq_start(irq_thread);
#ifdef __WIN__
            for (int i = 0; i < rfds.fd_count; i++) {
                priv = NULL;
                orig = vsf_protect_sched();
                __vsf_dlist_foreach_unsafe(vsf_linux_socket_inet_priv_t, node, &__vsf_linux_hostsock.sock_list) {
                    if (_->hostsock == rfds.fd_array[i]) {
                        priv = _;
                        break;
                    }
                }

                if (priv != NULL) {
                    FD_CLR(rfds.fd_array[i], &__vsf_linux_hostsock.select.rfds);
                    vsf_linux_fd_set_status(&priv->use_as__vsf_linux_fd_priv_t, VSF_LINUX_POLLIN, orig);
                } else {
                    vsf_unprotect_sched(orig);
                }
            }
            for (int i = 0; i < wfds.fd_count; i++) {
                priv = NULL;
                orig = vsf_protect_sched();
                __vsf_dlist_foreach_unsafe(vsf_linux_socket_inet_priv_t, node, &__vsf_linux_hostsock.sock_list) {
                    if (_->hostsock == wfds.fd_array[i]) {
                        priv = _;
                        break;
                    }
                }

                if (priv != NULL) {
                    FD_CLR(wfds.fd_array[i], &__vsf_linux_hostsock.select.wfds);
                    vsf_linux_fd_set_status(&priv->use_as__vsf_linux_fd_priv_t, VSF_LINUX_POLLOUT, orig);
                } else {
                    vsf_unprotect_sched(orig);
                }
            }
#elif defined(__LINUX__) || defined(__linux__)
            for (int i = 0; ret > 0 && i <= nfds; i++) {
                priv = NULL;
                orig = vsf_protect_sched();
                __vsf_dlist_foreach_unsafe(vsf_linux_socket_inet_priv_t, node, &__vsf_linux_hostsock.sock_list) {
                    if (_->hostsock == i) {
                        priv = _;
                        break;
                    }
                }

                if (priv != NULL) {
                    if (FD_ISSET(i, &rfds)) {
                        FD_CLR(i, &__vsf_linux_hostsock.select.rfds);
                        vsf_linux_fd_set_status(&priv->use_as__vsf_linux_fd_priv_t, VSF_LINUX_POLLIN, vsf_protect_sched());
                    }
                    if (FD_ISSET(i, &wfds)) {
                        FD_CLR(i, &__vsf_linux_hostsock.select.wfds);
                        vsf_linux_fd_set_status(&priv->use_as__vsf_linux_fd_priv_t, VSF_LINUX_POLLOUT, vsf_protect_sched());
                    }
                    ret--;
                }
                vsf_unprotect_sched(orig);
            }
#endif
            __vsf_arch_irq_end(irq_thread, false);
        }
    }

    __vsf_arch_irq_fini(irq_thread);
}

static void __vsf_linux_hostsock_notify(void)
{
    char buf = 0;
#ifdef VSF_ARCH_NO_SOCKET_PAIR
    ssize_t size = write(__vsf_linux_hostsock.sock_event_notifier, &buf, 1);
#else
    ssize_t size = send(__vsf_linux_hostsock.sock_event_notifier, &buf, 1, 0);
#endif
    VSF_LINUX_ASSERT(size == 1);
}

static void __vsf_linux_hostsock_pend(vsf_linux_socket_inet_priv_t *priv)
{
    vsf_protect_t orig = vsf_protect_sched();
        FD_SET(priv->hostsock, &__vsf_linux_hostsock.select.rfds);
        FD_SET(priv->hostsock, &__vsf_linux_hostsock.select.wfds);
        vsf_linux_fd_clear_status(&priv->use_as__vsf_linux_fd_priv_t, VSF_LINUX_POLLIN | VSF_LINUX_POLLOUT, orig);
    vsf_unprotect_sched(orig);
    __vsf_linux_hostsock_notify();
}

static void __vsf_linux_hostsock_add(vsf_linux_socket_inet_priv_t *priv)
{
    vsf_protect_t orig = vsf_protect_sched();
        vsf_dlist_insert(vsf_linux_socket_inet_priv_t, node, &__vsf_linux_hostsock.sock_list, priv, _->hostsock < priv->hostsock);
    vsf_unprotect_sched(orig);
    __vsf_linux_hostsock_pend(priv);
}

static void __vsf_linux_hostsock_remove(vsf_linux_socket_inet_priv_t *priv)
{
    vsf_protect_t orig = vsf_protect_sched();
        vsf_dlist_remove(vsf_linux_socket_inet_priv_t, node, &__vsf_linux_hostsock.sock_list, priv);
        FD_CLR(priv->hostsock, &__vsf_linux_hostsock.select.rfds);
        FD_CLR(priv->hostsock, &__vsf_linux_hostsock.select.wfds);
    vsf_unprotect_sched(orig);
    __vsf_linux_hostsock_notify();
}

static void __vsf_linux_hostsock_init(void)
{
    if (__vsf_linux_hostsock.is_inited) {
        return;
    }
#ifdef __WIN__
    WSAStartup(MAKEWORD(2, 2), &__vsf_linux_hostsock.wsaData);
#endif

#ifdef VSF_ARCH_NO_SOCKET_PAIR
    if (0 != pipe(__vsf_linux_hostsock.pipefd)) {
        goto __assert_fail;
    }
#else
    struct sockaddr_in inaddr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = htonl(INADDR_LOOPBACK),
        .sin_port = 0,
    };
    int addrlen = sizeof(inaddr);
    socket_t listener;

    __vsf_linux_hostsock.inaddr = &inaddr;
    if (    (INVALID_SOCKET == (listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)))
        ||  (SOCKET_ERROR == bind(listener, (const struct sockaddr *)&inaddr, addrlen))
        ||  (SOCKET_ERROR == getsockname(listener, (struct sockaddr *)&inaddr, &addrlen))
        ||  (addrlen < sizeof(inaddr))
        ||  (SOCKET_ERROR == listen(listener, 1))
        ||  (INVALID_SOCKET == (__vsf_linux_hostsock.sock_event_listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)))) {
        goto __assert_fail;
    }
#endif

    __vsf_arch_irq_init(&__vsf_linux_hostsock.irq_thread, "hostsock_irqthread", __vsf_linux_socket_inet_irqthread, vsf_arch_prio_0);

#ifndef VSF_ARCH_NO_SOCKET_PAIR
    if (INVALID_SOCKET == (__vsf_linux_hostsock.sock_event_notifier = accept(listener, NULL, NULL))) {
        closesocket(listener);
        goto __assert_fail;
    }
    closesocket(listener);
#endif

    __vsf_linux_hostsock.is_inited = true;
    return;
__assert_fail:
    VSF_LINUX_ASSERT(false);
}

static void __vsf_linux_sockaddr2host(const struct vsf_linux_socket_sockaddr *sockaddr,
                    struct sockaddr *hsockaddr)
{
    struct vsf_linux_socket_sockaddr_in *sockaddr_in = (struct vsf_linux_socket_sockaddr_in *)sockaddr;
    struct sockaddr_in *hsockaddr_in = (struct sockaddr_in *)hsockaddr;

    switch (sockaddr_in->sin_family) {
    case 0:
    case VSF_LINUX_SOCKET_AF_INET:  hsockaddr_in->sin_family = AF_INET; break;
    default:                        VSF_LINUX_ASSERT(false);
    }
    hsockaddr_in->sin_port = sockaddr_in->sin_port;
    hsockaddr_in->sin_addr.s_addr = sockaddr_in->sin_addr.__s_addr;
}

static void __vsf_linux_sockaddr2vsf(const struct sockaddr *hsockaddr,
                    struct vsf_linux_socket_sockaddr *sockaddr)
{
    struct vsf_linux_socket_sockaddr_in *sockaddr_in = (struct vsf_linux_socket_sockaddr_in *)sockaddr;
    struct sockaddr_in *hsockaddr_in = (struct sockaddr_in *)hsockaddr;

    switch (hsockaddr_in->sin_family) {
    case 0:
    case AF_INET:   sockaddr_in->sin_family = VSF_LINUX_SOCKET_AF_INET; break;
    default:        VSF_LINUX_ASSERT(false);
    }
    sockaddr_in->sin_port = hsockaddr_in->sin_port;
    sockaddr_in->sin_addr.__s_addr = hsockaddr_in->sin_addr.s_addr;
}

static unsigned long __vsf_linux_timeval_to_ms(const struct vsf_linux_timeval *t)
{
    return t->tv_sec * 1000 + t->tv_usec / 1000;
}

static void __vsf_linux_ms_to_timeval(struct vsf_linux_timeval *t, unsigned long ms)
{
    t->tv_sec = ms / 1000;
    t->tv_usec = (ms % 1000) * 1000;
}

static int __vsf_linux_sockflag2host(int flags)
{
    return 0;
}

static int __vsf_linux_socket_inet_init(vsf_linux_fd_t *sfd)
{
    vsf_linux_socket_priv_t *socket_priv = (vsf_linux_socket_priv_t *)sfd->priv;
    vsf_linux_socket_inet_priv_t *priv = (vsf_linux_socket_inet_priv_t *)socket_priv;

    __vsf_linux_hostsock_init();

    switch (socket_priv->domain) {
    case VSF_LINUX_SOCKET_AF_INET:      priv->hdomain = AF_INET;        break;
    default:                            goto assert_fail;
    }
    switch (socket_priv->type) {
    case VSF_LINUX_SOCKET_SOCK_STREAM:  priv->htype = SOCK_STREAM;      break;
    case VSF_LINUX_SOCKET_SOCK_DGRAM:   priv->htype = SOCK_DGRAM;       break;
    default:                            goto assert_fail;
    }
    switch (socket_priv->protocol) {
    case 0:                             priv->hprotocol = 0;            break;
    case VSF_LINUX_SOCKET_IPPROTO_TCP:  priv->hprotocol = IPPROTO_TCP;  break;
    case VSF_LINUX_SOCKET_IPPROTO_UDP:  priv->hprotocol = IPPROTO_UDP;  break;
    default:                            goto assert_fail;
    }

    priv->hostsock = socket(priv->hdomain, priv->htype, priv->hprotocol);
    if (INVALID_SOCKET == priv->hostsock) {
        return -1;
    }

#ifdef __WIN__
    u_long optval_ulong = 1;
    ioctlsocket(priv->hostsock, FIONBIO, &optval_ulong);
#elif defined(__LINUX__) || defined(__linux__)
    fcntl(priv->hostsock, F_SETFL, fcntl(priv->hostsock, F_GETFL) | O_NONBLOCK);
#endif
    priv->rcvto = priv->sndto = -1;
    __vsf_linux_hostsock_add(priv);
    return 0;
assert_fail:
    VSF_LINUX_ASSERT(false);
    return VSF_LINUX_SOCKET_INVALID_SOCKET;
}

static int __vsf_linux_socket_inet_fini(vsf_linux_socket_priv_t *socket_priv, int how)
{
    vsf_linux_socket_inet_priv_t *priv = (vsf_linux_socket_inet_priv_t *)socket_priv;
    shutdown(priv->hostsock, SHUT_RDWR);
    return 0;
}

static int __vsf_linux_socket_inet_setsockopt(vsf_linux_socket_priv_t *socket_priv,
                    int level, int optname, const void *optval, socklen_t optlen)
{
    vsf_linux_socket_inet_priv_t *priv = (vsf_linux_socket_inet_priv_t *)socket_priv;
    int ret;
    union {
        struct linger linger;
    } tmpbuf;

    switch (level) {
    case VSF_LINUX_SOCKET_SOL_SOCKET:
        level = SOL_SOCKET;
        switch (optname) {
        case VSF_LINUX_SOCKET_SO_BROADCAST:     optname = SO_BROADCAST;     break;
        case VSF_LINUX_SOCKET_SO_KEEPALIVE:     optname = SO_KEEPALIVE;     break;
        case VSF_LINUX_SOCKET_SO_RCVBUF:        optname = SO_RCVBUF;        break;
        case VSF_LINUX_SOCKET_SO_REUSEADDR:     optname = SO_REUSEADDR;     break;
        case VSF_LINUX_SOCKET_SO_RCVTIMEO: {
                unsigned long ms = __vsf_linux_timeval_to_ms((const struct vsf_linux_timeval *)optval);
                priv->rcvto = ms;
                optname = SO_RCVTIMEO;
                ret = setsockopt(priv->hostsock, level, optname, (const char *)&ms, sizeof(ms));
                goto __return;
            }
            break;
        case VSF_LINUX_SOCKET_SO_SNDTIMEO: {
                unsigned long ms = __vsf_linux_timeval_to_ms((const struct vsf_linux_timeval *)optval);
                priv->sndto = ms;
                optname = SO_SNDTIMEO;
                ret = setsockopt(priv->hostsock, level, optname, (const char *)&ms, sizeof(ms));
                goto __return;
            }
            break;
        case VSF_LINUX_SOCKET_SO_NONBLOCK: {
                priv->is_nonblock = *(int *)optval;
                ret = 0;
                goto __return;
            }
            break;
        case VSF_LINUX_SOCKET_SO_LINGER: {
                const struct vsf_linux_socket_linger *vsf_linux_linger = optval;

                tmpbuf.linger.l_onoff = vsf_linux_linger->l_onoff;
                tmpbuf.linger.l_linger = vsf_linux_linger->l_linger;
                optval = &tmpbuf.linger;
                optlen = sizeof(tmpbuf.linger);
                optname = SO_LINGER;
            }
            break;
        default:                                VSF_LINUX_ASSERT(false);    break;
        }
        break;
    case VSF_LINUX_SOCKET_IPPROTO_TCP:
        level = IPPROTO_TCP;
        switch (optname) {
        case VSF_LINUX_SOCKET_TCP_NODELAY:      optname = TCP_NODELAY;      break;
        case VSF_LINUX_SOCKET_TCP_KEEPALIVE:    optname = SO_KEEPALIVE;     break;
        default:                                VSF_LINUX_ASSERT(false);    break;
        }
        break;
    default:
        VSF_LINUX_ASSERT(false);
        break;
    }
    ret = setsockopt(priv->hostsock, level, optname, optval, optlen);
__return:
    return SOCKET_ERROR == ret ? VSF_LINUX_SOCKET_SOCKET_ERROR : ret;
}

static int __vsf_linux_socket_inet_getsockopt(vsf_linux_socket_priv_t *socket_priv,
                    int level, int optname, void *optval, socklen_t *optlen)
{
    vsf_linux_socket_inet_priv_t *priv = (vsf_linux_socket_inet_priv_t *)socket_priv;
    void *orig_optval = optval;
    union {
        struct linger linger;
    } tmpbuf;
    int ret;

    switch (level) {
    case VSF_LINUX_SOCKET_SOL_SOCKET:
        level = SOL_SOCKET;
        switch (optname) {
        case VSF_LINUX_SOCKET_SO_BROADCAST:     optname = SO_BROADCAST;     break;
        case VSF_LINUX_SOCKET_SO_KEEPALIVE:     optname = SO_KEEPALIVE;     break;
        case VSF_LINUX_SOCKET_SO_RCVBUF:        optname = SO_RCVBUF;        break;
        case VSF_LINUX_SOCKET_SO_REUSEADDR:     optname = SO_REUSEADDR;     break;
        case VSF_LINUX_SOCKET_SO_ERROR:         optname = SO_ERROR;         break;
        case VSF_LINUX_SOCKET_SO_RCVTIMEO: {
                unsigned long ms;
                int hoptlen = sizeof(ms);
                optname = SO_RCVTIMEO;
                ret = getsockopt(priv->hostsock, level, optname, (char *)&ms, &hoptlen);
                __vsf_linux_ms_to_timeval(optval, ms);
                goto __return;
            }
            break;
        case VSF_LINUX_SOCKET_SO_SNDTIMEO: {
                unsigned long ms;
                int hoptlen = sizeof(ms);
                optname = SO_SNDTIMEO;
                ret = getsockopt(priv->hostsock, level, optname, (char *)&ms, &hoptlen);
                __vsf_linux_ms_to_timeval(optval, ms);
                goto __return;
            }
            break;
        case VSF_LINUX_SOCKET_SO_NONBLOCK:
            *(int *)optval = priv->is_nonblock;
            ret = 0;
            goto __return;
        case VSF_LINUX_SOCKET_SO_LINGER:
            VSF_LINUX_ASSERT(*optlen >= sizeof(struct vsf_linux_socket_linger));
            optval = &tmpbuf.linger;
            *optlen = sizeof(tmpbuf.linger);
            optname = SO_LINGER;
            break;
        default:                                VSF_LINUX_ASSERT(false);    break;
        }
        break;
    case VSF_LINUX_SOCKET_IPPROTO_TCP:
        level = IPPROTO_TCP;
        switch (optname) {
        case VSF_LINUX_SOCKET_TCP_NODELAY:      optname = TCP_NODELAY;      break;
        case VSF_LINUX_SOCKET_TCP_KEEPALIVE:    optname = SO_KEEPALIVE;     break;
        default:                                VSF_LINUX_ASSERT(false);    break;
        }
        break;
    default:
        VSF_LINUX_ASSERT(false);
        break;
    }
    ret = getsockopt(priv->hostsock, level, optname, optval, (int *)optlen);
    if (!ret) {
        switch (level) {
        case SOL_SOCKET:
            switch (optname) {
            case SO_LINGER: {
                    struct vsf_linux_socket_linger *vsf_linux_linger = orig_optval;
                    vsf_linux_linger->l_onoff = tmpbuf.linger.l_onoff;
                    vsf_linux_linger->l_linger = tmpbuf.linger.l_linger;
                    *optlen = sizeof(struct vsf_linux_socket_linger);
                }
                break;
            }
        }
    }
__return:
    return SOCKET_ERROR == ret ? VSF_LINUX_SOCKET_SOCKET_ERROR : ret;
}

static int __vsf_linux_socket_inet_getpeername(vsf_linux_socket_priv_t *socket_priv,
                    struct vsf_linux_socket_sockaddr *addr, socklen_t *addrlen)
{
    vsf_linux_socket_inet_priv_t *priv = (vsf_linux_socket_inet_priv_t *)socket_priv;
    struct sockaddr hsockaddr = { 0 };
    int hsockaddr_len = sizeof(hsockaddr);
    int ret = getpeername(priv->hostsock, &hsockaddr, &hsockaddr_len);
    __vsf_linux_sockaddr2vsf(&hsockaddr, addr);
    return SOCKET_ERROR == ret ? VSF_LINUX_SOCKET_SOCKET_ERROR : ret;
}

static int __vsf_linux_socket_inet_getsockname(vsf_linux_socket_priv_t *socket_priv,
                    struct vsf_linux_socket_sockaddr *addr, socklen_t *addrlen)
{
    vsf_linux_socket_inet_priv_t *priv = (vsf_linux_socket_inet_priv_t *)socket_priv;
    struct sockaddr hsockaddr = { 0 };
    int hsockaddr_len = sizeof(hsockaddr);
    int ret = getsockname(priv->hostsock, &hsockaddr, &hsockaddr_len);
    __vsf_linux_sockaddr2vsf(&hsockaddr, addr);
    return SOCKET_ERROR == ret ? VSF_LINUX_SOCKET_SOCKET_ERROR : ret;
}

static int __vsf_linux_socket_inet_accept(vsf_linux_socket_priv_t *socket_priv,
                    struct vsf_linux_socket_sockaddr *addr, socklen_t *addrlen)
{
    vsf_linux_socket_inet_priv_t *priv = (vsf_linux_socket_inet_priv_t *)socket_priv;
    struct sockaddr hsockaddr;
    int hsockaddr_len = sizeof(hsockaddr);

    if (!priv->is_nonblock) {
        vsf_linux_trigger_t trig;
        vsf_linux_trigger_init(&trig);
        short events = vsf_linux_fd_pend_events(&priv->use_as__vsf_linux_fd_priv_t, VSF_LINUX_POLLIN, &trig, vsf_protect_sched());
        if (!(events & VSF_LINUX_POLLIN)) {
            return VSF_LINUX_SOCKET_INVALID_SOCKET;
        }
    }

    int hnewsock = accept(priv->hostsock, &hsockaddr, &hsockaddr_len);
    if (INVALID_SOCKET == hnewsock) {
        if (errno == ERRNO_WOULDBLOCK) {
            vsf_linux_errno = VSF_LINUX_ERRNO_EAGAIN;
        }
        return VSF_LINUX_SOCKET_INVALID_SOCKET;
    }

    vsf_linux_fd_t *sfd;
    int newsock = vsf_linux_fd_create(&sfd, &socket_priv->sockop->fdop);
    if (newsock < 0) {
        closesocket(hnewsock);
        return VSF_LINUX_SOCKET_INVALID_SOCKET;
    }

    vsf_linux_socket_inet_priv_t *newpriv = (vsf_linux_socket_inet_priv_t *)sfd->priv;
    newpriv->sockop = priv->sockop;
    newpriv->domain = priv->domain;
    newpriv->type = priv->type;
    newpriv->protocol = priv->protocol;
    newpriv->hostsock = hnewsock;
    newpriv->hdomain = priv->hdomain;
    newpriv->htype = priv->htype;
    newpriv->hprotocol = priv->hprotocol;
    __vsf_linux_hostsock_add(newpriv);
    __vsf_linux_hostsock_pend(priv);
    return newsock;
}

static int __vsf_linux_socket_inet_bind(vsf_linux_socket_priv_t *socket_priv,
                    const struct vsf_linux_socket_sockaddr *addr, socklen_t addrlen)
{
    vsf_linux_socket_inet_priv_t *priv = (vsf_linux_socket_inet_priv_t *)socket_priv;
    struct sockaddr hsockaddr = { 0 };
    __vsf_linux_sockaddr2host(addr, &hsockaddr);
    int ret = bind(priv->hostsock, (const struct sockaddr *)&hsockaddr, sizeof(hsockaddr));
    return SOCKET_ERROR == ret ? VSF_LINUX_SOCKET_SOCKET_ERROR : ret;
}

static int __vsf_linux_socket_inet_connect(vsf_linux_socket_priv_t *socket_priv,
                    const struct vsf_linux_socket_sockaddr *addr, socklen_t addrlen)
{
    vsf_linux_socket_inet_priv_t *priv = (vsf_linux_socket_inet_priv_t *)socket_priv;
    struct sockaddr hsockaddr = { 0 };
    __vsf_linux_sockaddr2host(addr, &hsockaddr);
    int ret = connect(priv->hostsock, &hsockaddr, sizeof(hsockaddr));
    if (    (SOCKET_ERROR == ret)
#if defined(__WIN__)
        &&  (errno == ERRNO_WOULDBLOCK)) {
#elif defined(__LINUX__) || defined(__linux__)
        &&  ((errno == ERRNO_WOULDBLOCK) || (errno == EINPROGRESS))) {
#endif
        ret = 0;
        if (!priv->is_nonblock) {
            vsf_linux_trigger_t trig;
            vsf_linux_trigger_init(&trig);
            short events = vsf_linux_fd_pend_events(&priv->use_as__vsf_linux_fd_priv_t, VSF_LINUX_POLLOUT, &trig, vsf_protect_sched());
            if (!(events & VSF_LINUX_POLLOUT)) {
                return VSF_LINUX_SOCKET_INVALID_SOCKET;
            }

            // POLLOUT is triggered for connection, but need pollout for send
            vsf_linux_fd_set_events(&priv->use_as__vsf_linux_fd_priv_t, VSF_LINUX_POLLOUT, vsf_protect_sched());
        } else {
            vsf_linux_errno = VSF_LINUX_ERRNO_EAGAIN;
        }
    }
    return SOCKET_ERROR == ret ? VSF_LINUX_SOCKET_SOCKET_ERROR : ret;
}

static int __vsf_linux_socket_inet_listen(vsf_linux_socket_priv_t *socket_priv, int backlog)
{
    vsf_linux_socket_inet_priv_t *priv = (vsf_linux_socket_inet_priv_t *)socket_priv;
    int ret = listen(priv->hostsock, backlog);
    return SOCKET_ERROR == ret ? VSF_LINUX_SOCKET_SOCKET_ERROR : ret;
}

static ssize_t __vsf_linux_socket_inet_send(vsf_linux_socket_inet_priv_t *priv, const void *buffer, size_t size, int flags,
                    const struct vsf_linux_socket_sockaddr *dst_addr, socklen_t addrlen)
{
    int ret;

    if (!priv->is_nonblock && (priv->hprotocol != IPPROTO_UDP)) {
        vsf_linux_trigger_t trig;
        vsf_linux_trigger_init(&trig);
        short events = vsf_linux_fd_pend_events(&priv->use_as__vsf_linux_fd_priv_t, VSF_LINUX_POLLOUT, &trig, vsf_protect_sched());
        if (!(events & VSF_LINUX_POLLOUT)) {
            return VSF_LINUX_SOCKET_INVALID_SOCKET;
        }
    }

    if (dst_addr != NULL) {
        struct sockaddr hsockaddr = { 0 };
        __vsf_linux_sockaddr2host(dst_addr, &hsockaddr);
        ret = sendto(priv->hostsock, buffer, size, __vsf_linux_sockflag2host(flags), &hsockaddr, sizeof(hsockaddr));
    } else {
        ret = send(priv->hostsock, buffer, size, __vsf_linux_sockflag2host(flags));
    }
    if ((SOCKET_ERROR == ret) && (errno == ERRNO_WOULDBLOCK)) {
        vsf_linux_errno = VSF_LINUX_ERRNO_EAGAIN;
    }
    __vsf_linux_hostsock_pend(priv);
    return SOCKET_ERROR == ret ? VSF_LINUX_SOCKET_SOCKET_ERROR : ret;
}

static ssize_t __vsf_linux_socket_inet_recv(vsf_linux_socket_inet_priv_t *priv, void *buffer, size_t size, int flags,
                    struct vsf_linux_socket_sockaddr *src_addr, socklen_t *addrlen)
{
    int ret;

    if (!priv->is_nonblock) {
        vsf_linux_trigger_t trig;
        vsf_linux_trigger_init(&trig);
        short events = vsf_linux_fd_pend_events(&priv->use_as__vsf_linux_fd_priv_t, VSF_LINUX_POLLIN, &trig, vsf_protect_sched());
        if (!(events & VSF_LINUX_POLLIN)) {
            return VSF_LINUX_SOCKET_INVALID_SOCKET;
        }
    }

    if (src_addr != NULL) {
        struct sockaddr hsockaddr = { 0 };
        int hsockaddr_len = sizeof(hsockaddr);
        ret = recvfrom(priv->hostsock, buffer, size, __vsf_linux_sockflag2host(flags), &hsockaddr, &hsockaddr_len);
        __vsf_linux_sockaddr2vsf(&hsockaddr, src_addr);
    } else {
        ret = recv(priv->hostsock, buffer, size, __vsf_linux_sockflag2host(flags));
    }
    if ((SOCKET_ERROR == ret) && (errno == ERRNO_WOULDBLOCK)) {
        vsf_linux_errno = VSF_LINUX_ERRNO_EAGAIN;
    }
    __vsf_linux_hostsock_pend(priv);
    return SOCKET_ERROR == ret ? VSF_LINUX_SOCKET_SOCKET_ERROR : ret;
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
    __vsf_linux_hostsock_remove(priv);
    closesocket(priv->hostsock);
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

// netdb.h
// none thread safty
int __inet_gethostbyname(const char *name, in_addr_t *addr)
{
    __vsf_linux_hostsock_init();

    struct hostent *host = gethostbyname(name);
    if (NULL == host) {
        return -1;
    }
    if (addr != NULL) {
        *addr = *(in_addr_t *)host->h_addr;
    }
    return 0;
}

#endif
