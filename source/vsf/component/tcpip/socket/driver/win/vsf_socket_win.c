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

#include "component/tcpip/vsf_tcpip_cfg.h"

#if VSF_USE_TCPIP == ENABLED

#define __VSF_SOCKET_CLASS_INHERIT__
#define __VSF_SOCKET_WIN_CLASS_IMPLEMENT
#include "../../vsf_socket.h"

#include "kernel/vsf_kernel.h"

#include <winsock2.h>

#pragma comment (lib, "ws2_32.lib")

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vk_socket_win_local_t {
    vsf_arch_irq_thread_t       irq_thread;
    vsf_arch_irq_request_t      irq_request;

    enum {
        VSF_SOCKET_WIN_REQ_STARTUP,
        VSF_SOCKET_WIN_REQ_GETHOSTBYNAME,
        VSF_SOCKET_WIN_REQ_CLEANUP,
    } request;
    union {
        struct {
            const char          *name;
            vk_netdrv_addr_t    addr;
        } gethostbyname;
    } param;

    vsf_mutex_t                 mutex;
    vsf_eda_t                   *eda;
    int                         ret;
    bool                        is_inited;
    bool                        is_started;
} vk_socket_win_local_t;

/*============================ PROTOTYPES ====================================*/

static vsf_err_t __vk_socket_win_socket(vk_socket_t *socket, int family, int protocol);
static vsf_err_t __vk_socket_win_close(vk_socket_t *socket);
static vsf_err_t __vk_socket_win_bind(vk_socket_t *socket, const vk_socket_addr_t *addr);
static vsf_err_t __vk_socket_win_listen(vk_socket_t *socket, int backlog);
static vsf_err_t __vk_socket_win_connect(vk_socket_t *socket, const vk_socket_addr_t *remote_addr);
static vsf_err_t __vk_socket_win_accept(vk_socket_t *socket, vk_socket_addr_t *remote_addr);
static vsf_err_t __vk_socket_win_send(vk_socket_t *socket, const void *buf, size_t len, int flags,
                                    const vk_socket_addr_t *remote_addr);
static vsf_err_t __vk_socket_win_recv(vk_socket_t *socket, void *buf, size_t len, int flags,
                                    const vk_socket_addr_t *remote_addr);

static vsf_err_t __vk_dns_win_gethostbyname(const char *name, vk_netdrv_addr_t *addr);

/*============================ GLOBAL VARIABLES ==============================*/

const vk_socket_op_t vk_socket_win_op = {
    .feature                = VSF_SOCKET_THREAD,
    .socket                 = __vk_socket_win_socket,
    .close                  = __vk_socket_win_close,
    .bind                   = __vk_socket_win_bind,
    .listen                 = __vk_socket_win_listen,
    .connect                = __vk_socket_win_connect,
    .accept                 = __vk_socket_win_accept,
    .send                   = __vk_socket_win_send,
    .recv                   = __vk_socket_win_recv,

    .protocols              = {
        .dns                = {
            .gethostbyname  = __vk_dns_win_gethostbyname,
        },
    },
};

/*============================ LOCAL VARIABLES ===============================*/

static vk_socket_win_local_t __vk_socket_win;

/*============================ IMPLEMENTATION ================================*/

static void __vk_socket_win_thread(void *arg)
{
    vsf_arch_irq_thread_t *irq_thread = arg;
    vk_socket_win_local_t *socket_win_local = container_of(irq_thread, vk_socket_win_local_t, irq_thread);
    WSADATA wsaData;

    __vsf_arch_irq_set_background(irq_thread);

    while (1) {
        __vsf_arch_irq_request_pend(&socket_win_local->irq_request);

        switch (socket_win_local->request) {
        case VSF_SOCKET_WIN_REQ_STARTUP:
            socket_win_local->ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
            break;
        case VSF_SOCKET_WIN_REQ_GETHOSTBYNAME: {
                struct hostent *hostent = gethostbyname(socket_win_local->param.gethostbyname.name);
                socket_win_local->ret = !hostent;
                if (hostent != NULL) {
                    VSF_TCPIP_ASSERT(hostent->h_addr != NULL);
                    VSF_TCPIP_ASSERT(hostent->h_length <= sizeof(socket_win_local->param.gethostbyname.addr.addr_buf));
                    socket_win_local->param.gethostbyname.addr.size = hostent->h_length;
                    memcpy(socket_win_local->param.gethostbyname.addr.addr_buf, hostent->h_addr, hostent->h_length);
                }
            }
            break;
        case VSF_SOCKET_WIN_REQ_CLEANUP:
            socket_win_local->ret = WSACleanup();
            break;
        }

        __vsf_arch_irq_start(irq_thread);
            vsf_eda_post_evt(socket_win_local->eda, VSF_EVT_USER);
        __vsf_arch_irq_end(irq_thread, false);
    }

    __vsf_arch_irq_fini(irq_thread);
}

vsf_err_t vk_socket_win_init(void)
{
    int ret;

    if (!__vk_socket_win.is_inited) {
        __vk_socket_win.is_inited = true;
        __vsf_arch_irq_request_init(&__vk_socket_win.irq_request);
        __vsf_arch_irq_init(&__vk_socket_win.irq_thread, "socket_win", __vk_socket_win_thread, vsf_arch_prio_0);

        vsf_eda_mutex_init(&__vk_socket_win.mutex);
    }

    VSF_TCPIP_ASSERT(!__vk_socket_win.is_started);

    vsf_thread_mutex_enter(&__vk_socket_win.mutex, -1);
        __vk_socket_win.eda = vsf_eda_get_cur();
        __vk_socket_win.request = VSF_SOCKET_WIN_REQ_STARTUP;
        __vsf_arch_irq_request_send(&__vk_socket_win.irq_request);
        vsf_thread_wfe(VSF_EVT_USER);
        ret = __vk_socket_win.ret;
    vsf_thread_mutex_leave(&__vk_socket_win.mutex);

    if (ret != 0) {
        return VSF_ERR_FAIL;
    }

    __vk_socket_win.is_started = true;
    return VSF_ERR_NONE;
}

vsf_err_t vk_socket_win_fini(void)
{
    VSF_TCPIP_ASSERT(__vk_socket_win.is_started);

    vsf_thread_mutex_enter(&__vk_socket_win.mutex, -1);
        __vk_socket_win.eda = vsf_eda_get_cur();
        __vk_socket_win.request = VSF_SOCKET_WIN_REQ_CLEANUP;
        __vsf_arch_irq_request_send(&__vk_socket_win.irq_request);
        vsf_thread_wfe(VSF_EVT_USER);
    vsf_thread_mutex_leave(&__vk_socket_win.mutex);

    __vk_socket_win.is_started = false;
    return __vk_socket_win.ret != 0 ? VSF_ERR_FAIL : VSF_ERR_NONE;
}

static void __vk_socket_win_socket_tx_thread(void *arg)
{
    vsf_arch_irq_thread_t *irq_thread = arg;
    vk_socket_win_t *socket_win = container_of(irq_thread, vk_socket_win_t, tx);

    __vsf_arch_irq_set_background(irq_thread);

    while (!socket_win->tx.is_to_exit) {
        __vsf_arch_irq_request_pend(&socket_win->tx.irq_request);

        switch (socket_win->tx.request) {
        case VSF_SOCKET_WIN_BIND:
            switch (socket_win->family) {
            case AF_INET: {
                    const vk_socket_addr_t *addr = socket_win->tx.args.bind.addr;
                    struct sockaddr_in sockaddr = { 0 };
                    sockaddr.sin_family = PF_INET;
                    sockaddr.sin_addr.s_addr = *(uint32_t *)addr->addr.addr_buf;
                    sockaddr.sin_port = addr->port;
                    socket_win->tx.ret = bind(socket_win->socket, (const struct sockaddr*)&sockaddr, sizeof(sockaddr));
                }
                break;
            default:
                VSF_TCPIP_ASSERT(false);
                socket_win->tx.ret = SOCKET_ERROR;
            }
            break;
        case VSF_SOCKET_WIN_LISTEN:
            socket_win->tx.ret = listen(socket_win->socket, socket_win->tx.args.listen.backlog);
            break;
        case VSF_SOCKET_WIN_CONNECT:
            switch (socket_win->family) {
            case AF_INET: {
                    const vk_socket_addr_t *remote_addr = socket_win->tx.args.connect.remote_addr;
                    struct sockaddr_in sockaddr = { 0 };
                    sockaddr.sin_family = PF_INET;
                    sockaddr.sin_addr.s_addr = *(uint32_t *)remote_addr->addr.addr_buf;
                    sockaddr.sin_port = htons(remote_addr->port);
                    socket_win->tx.ret = connect(socket_win->socket, (const struct sockaddr*)&sockaddr, sizeof(sockaddr));
                }
                break;
            default:
                VSF_TCPIP_ASSERT(false);
                socket_win->tx.ret = SOCKET_ERROR;
            }
            break;
        case VSF_SOCKET_WIN_ACCEPT:
            switch (socket_win->family) {
            case AF_INET: {
                    vk_socket_addr_t *remote_addr = socket_win->tx.args.accept.remote_addr;
                    struct sockaddr_in sockaddr = { 0 };
                    int socklen = sizeof(sockaddr);
                    socket_win->tx.ret = accept(socket_win->socket, (struct sockaddr*)&sockaddr, &socklen);
                    if (socket_win->tx.ret != SOCKET_ERROR) {
                        remote_addr->addr.size = 4;
                        remote_addr->addr.addr32 = sockaddr.sin_addr.s_addr;
                        remote_addr->port = sockaddr.sin_port;
                    }
                }
                break;
            default:
                VSF_TCPIP_ASSERT(false);
                socket_win->tx.ret = SOCKET_ERROR;
            }
            break;
        case VSF_SOCKET_WIN_SEND: {
                const void *buf = socket_win->tx.args.send.buf;
                size_t len = socket_win->tx.args.send.len;
                int flags = socket_win->tx.args.send.flags;
                const vk_socket_addr_t *remote_addr = socket_win->tx.args.send.remote_addr;
                struct sockaddr_in sockaddr = { 0 };

                switch (socket_win->family) {
                case AF_INET:
                    switch (socket_win->protocol) {
                    case IPPROTO_UDP:
                        VSF_TCPIP_ASSERT(remote_addr != NULL);
                        sockaddr.sin_family = PF_INET;
                        sockaddr.sin_addr.s_addr = *(uint32_t *)remote_addr->addr.addr_buf;
                        sockaddr.sin_port = remote_addr->port;
                        socket_win->tx.ret = sendto(socket_win->socket, buf, len, flags, (const struct sockaddr*)&sockaddr, sizeof(sockaddr));
                        break;
                    case IPPROTO_TCP:
                        VSF_TCPIP_ASSERT(NULL == remote_addr);
                        socket_win->tx.ret = send(socket_win->socket, buf, len, flags);
                        break;
                    default:
                        VSF_TCPIP_ASSERT(false);
                        socket_win->tx.ret = SOCKET_ERROR;
                    }
                    break;
                default:
                    VSF_TCPIP_ASSERT(false);
                    socket_win->tx.ret = SOCKET_ERROR;
                }
            }
            break;
        case VSF_SOCKET_WIN_CLOSE:
            socket_win->tx.ret = closesocket(socket_win->socket);
            socket_win->tx.is_to_exit = true;
            break;
        }

        __vsf_arch_irq_start(irq_thread);
            vsf_eda_post_evt(socket_win->tx.eda, VSF_EVT_USER);
        __vsf_arch_irq_end(irq_thread, false);

        socket_win->tx.is_busy = false;
    }
}

static void __vk_socket_win_socket_rx_thread(void *arg)
{
    vsf_arch_irq_thread_t *irq_thread = arg;
    vk_socket_win_t *socket_win = container_of(irq_thread, vk_socket_win_t, rx);

    __vsf_arch_irq_set_background(irq_thread);

    while (!socket_win->rx.is_to_exit) {
        __vsf_arch_irq_request_pend(&socket_win->rx.irq_request);

        switch (socket_win->rx.request) {
        case VSF_SOCKET_WIN_RECV: {
                void *buf = socket_win->rx.args.recv.buf;
                size_t len = socket_win->rx.args.recv.len;
                int flags = socket_win->rx.args.recv.flags;
                const vk_socket_addr_t *remote_addr = socket_win->rx.args.recv.remote_addr;
                struct sockaddr_in sockaddr = { 0 };

                switch (socket_win->family) {
                case AF_INET:
                    switch (socket_win->protocol) {
                    case IPPROTO_UDP:
                        VSF_TCPIP_ASSERT(remote_addr != NULL);
                        int socklen = sizeof(sockaddr);
                        sockaddr.sin_family = PF_INET;
                        sockaddr.sin_addr.s_addr = *(uint32_t *)remote_addr->addr.addr_buf;
                        sockaddr.sin_port = remote_addr->port;
                        socket_win->rx.ret = recvfrom(socket_win->socket, buf, len, flags, (struct sockaddr*)&sockaddr, &socklen);
                        break;
                    case IPPROTO_TCP:
                        VSF_TCPIP_ASSERT(NULL == remote_addr);
                        socket_win->rx.ret = recv(socket_win->socket, buf, len, flags);
                        break;
                    default:
                        VSF_TCPIP_ASSERT(false);
                        socket_win->rx.ret = SOCKET_ERROR;
                    }
                    break;
                default:
                    VSF_TCPIP_ASSERT(false);
                    socket_win->rx.ret = SOCKET_ERROR;
                }
            }
            break;
        case VSF_SOCKET_WIN_CLOSE:
            socket_win->rx.is_to_exit = true;
            break;
        }

        __vsf_arch_irq_start(irq_thread);
            vsf_eda_post_evt(socket_win->rx.eda, VSF_EVT_USER);
        __vsf_arch_irq_end(irq_thread, false);

        socket_win->rx.is_busy = false;
    }
}

static vsf_err_t __vk_socket_win_socket(vk_socket_t *s, int family, int protocol)
{
    VSF_TCPIP_ASSERT(__vk_socket_win.is_started);
    vk_socket_win_t *socket_win = (vk_socket_win_t *)s;
    VSF_TCPIP_ASSERT(!socket_win->is_inited);
    VSF_TCPIP_ASSERT(!socket_win->tx.is_busy && !socket_win->rx.is_busy);

    int type;
    switch (protocol) {
    case VSF_SOCKET_IPPROTO_TCP:    type = SOCK_STREAM; break;
    case VSF_SOCKET_IPPROTO_UDP:    type = SOCK_DGRAM;  break;
    default:                        VSF_TCPIP_ASSERT(false); return VSF_ERR_NOT_SUPPORT;
    }

    socket_win->socket = socket(family, type, protocol);
    if (INVALID_SOCKET == socket_win->socket) {
        return VSF_ERR_FAIL;
    }

    socket_win->is_inited = true;
    socket_win->family = family;
    socket_win->protocol = protocol;
    socket_win->tx.is_to_exit = false;
    socket_win->tx.is_busy = false;
    socket_win->rx.is_to_exit = false;
    socket_win->rx.is_busy = false;

    __vsf_arch_irq_request_init(&socket_win->tx.irq_request);
    __vsf_arch_irq_init(&socket_win->tx.irq_thread, "socket_win_tx", __vk_socket_win_socket_tx_thread, vsf_arch_prio_0);
    __vsf_arch_irq_request_init(&socket_win->rx.irq_request);
    __vsf_arch_irq_init(&socket_win->rx.irq_thread, "socket_win_tx", __vk_socket_win_socket_rx_thread, vsf_arch_prio_0);
    return VSF_ERR_NONE;
}

static vsf_err_t __vk_socket_win_close(vk_socket_t *s)
{
    VSF_TCPIP_ASSERT(__vk_socket_win.is_started);
    vk_socket_win_t *socket_win = (vk_socket_win_t *)s;
    VSF_TCPIP_ASSERT(socket_win->is_inited);

    vsf_protect_t orig = vsf_protect_sched();
        VSF_TCPIP_ASSERT(!socket_win->tx.is_busy);
        socket_win->tx.is_busy = true;
    vsf_unprotect_sched(orig);

    socket_win->tx.eda = vsf_eda_get_cur();
    socket_win->tx.request = VSF_SOCKET_WIN_CLOSE;
    __vsf_arch_irq_request_send(&socket_win->tx.irq_request);
    vsf_thread_wfe(VSF_EVT_USER);
    __vsf_arch_irq_fini(&socket_win->tx.irq_thread);

    socket_win->rx.eda = vsf_eda_get_cur();
    socket_win->rx.request = VSF_SOCKET_WIN_CLOSE;
    __vsf_arch_irq_request_send(&socket_win->rx.irq_request);
    vsf_thread_wfe(VSF_EVT_USER);
    __vsf_arch_irq_fini(&socket_win->rx.irq_thread);

    return VSF_ERR_NONE;
}

static vsf_err_t __vk_socket_win_bind(vk_socket_t *s, const vk_socket_addr_t *addr)
{
    VSF_TCPIP_ASSERT(__vk_socket_win.is_started);
    vk_socket_win_t *socket_win = (vk_socket_win_t *)s;
    VSF_TCPIP_ASSERT(socket_win->is_inited);

    vsf_protect_t orig = vsf_protect_sched();
        VSF_TCPIP_ASSERT(!socket_win->tx.is_busy);
        socket_win->tx.is_busy = true;
    vsf_unprotect_sched(orig);

    socket_win->tx.eda = vsf_eda_get_cur();
    socket_win->tx.request = VSF_SOCKET_WIN_BIND;
    socket_win->tx.args.bind.addr = addr;
    __vsf_arch_irq_request_send(&socket_win->tx.irq_request);
    vsf_thread_wfe(VSF_EVT_USER);
    return SOCKET_ERROR == socket_win->tx.ret ? VSF_ERR_FAIL : VSF_ERR_NONE;
}

static vsf_err_t __vk_socket_win_listen(vk_socket_t *s, int backlog)
{
    VSF_TCPIP_ASSERT(__vk_socket_win.is_started);
    vk_socket_win_t *socket_win = (vk_socket_win_t *)s;
    VSF_TCPIP_ASSERT(socket_win->is_inited);

    vsf_protect_t orig = vsf_protect_sched();
        VSF_TCPIP_ASSERT(!socket_win->tx.is_busy);
        socket_win->tx.is_busy = true;
    vsf_unprotect_sched(orig);

    socket_win->tx.eda = vsf_eda_get_cur();
    socket_win->tx.request = VSF_SOCKET_WIN_LISTEN;
    socket_win->tx.args.listen.backlog = backlog;
    __vsf_arch_irq_request_send(&socket_win->tx.irq_request);
    vsf_thread_wfe(VSF_EVT_USER);
    return SOCKET_ERROR == socket_win->tx.ret ? VSF_ERR_FAIL : VSF_ERR_NONE;
}

static vsf_err_t __vk_socket_win_connect(vk_socket_t *s, const vk_socket_addr_t *remote_addr)
{
    VSF_TCPIP_ASSERT(__vk_socket_win.is_started);
    vk_socket_win_t *socket_win = (vk_socket_win_t *)s;
    VSF_TCPIP_ASSERT(socket_win->is_inited);

    vsf_protect_t orig = vsf_protect_sched();
        VSF_TCPIP_ASSERT(!socket_win->tx.is_busy);
        socket_win->tx.is_busy = true;
    vsf_unprotect_sched(orig);

    socket_win->tx.eda = vsf_eda_get_cur();
    socket_win->tx.request = VSF_SOCKET_WIN_CONNECT;
    socket_win->tx.args.connect.remote_addr = remote_addr;
    __vsf_arch_irq_request_send(&socket_win->tx.irq_request);
    vsf_thread_wfe(VSF_EVT_USER);
    return SOCKET_ERROR == socket_win->tx.ret ? VSF_ERR_FAIL : VSF_ERR_NONE;
}

static vsf_err_t __vk_socket_win_accept(vk_socket_t *s, vk_socket_addr_t *remote_addr)
{
    VSF_TCPIP_ASSERT(__vk_socket_win.is_started);
    vk_socket_win_t *socket_win = (vk_socket_win_t *)s;
    VSF_TCPIP_ASSERT(socket_win->is_inited);

    vsf_protect_t orig = vsf_protect_sched();
        VSF_TCPIP_ASSERT(!socket_win->tx.is_busy);
        socket_win->tx.is_busy = true;
    vsf_unprotect_sched(orig);

    socket_win->tx.eda = vsf_eda_get_cur();
    socket_win->tx.request = VSF_SOCKET_WIN_ACCEPT;
    socket_win->tx.args.accept.remote_addr = remote_addr;
    __vsf_arch_irq_request_send(&socket_win->tx.irq_request);
    vsf_thread_wfe(VSF_EVT_USER);
    return SOCKET_ERROR == socket_win->tx.ret ? VSF_ERR_FAIL : VSF_ERR_NONE;
}

static vsf_err_t __vk_socket_win_send(vk_socket_t *s, const void *buf, size_t len, int flags,
                                    const vk_socket_addr_t *remote_addr)
{
    VSF_TCPIP_ASSERT(__vk_socket_win.is_started);
    vk_socket_win_t *socket_win = (vk_socket_win_t *)s;
    VSF_TCPIP_ASSERT(socket_win->is_inited);

    vsf_protect_t orig = vsf_protect_sched();
        VSF_TCPIP_ASSERT(!socket_win->tx.is_busy);
        socket_win->tx.is_busy = true;
    vsf_unprotect_sched(orig);

    socket_win->tx.eda = vsf_eda_get_cur();
    socket_win->tx.request = VSF_SOCKET_WIN_SEND;
    socket_win->tx.args.send.buf = (void *)buf;
    socket_win->tx.args.send.len = len;
    socket_win->tx.args.send.flags = flags;
    socket_win->tx.args.send.remote_addr = remote_addr;
    __vsf_arch_irq_request_send(&socket_win->tx.irq_request);
    vsf_thread_wfe(VSF_EVT_USER);
    return SOCKET_ERROR == socket_win->tx.ret ? VSF_ERR_FAIL : VSF_ERR_NONE;
}

static vsf_err_t __vk_socket_win_recv(vk_socket_t *s, void *buf, size_t len, int flags,
                                    const vk_socket_addr_t *remote_addr)
{
    VSF_TCPIP_ASSERT(__vk_socket_win.is_started);
    vk_socket_win_t *socket_win = (vk_socket_win_t *)s;
    VSF_TCPIP_ASSERT(socket_win->is_inited);

    vsf_protect_t orig = vsf_protect_sched();
        VSF_TCPIP_ASSERT(!socket_win->rx.is_busy);
        socket_win->rx.is_busy = true;
    vsf_unprotect_sched(orig);

    socket_win->rx.eda = vsf_eda_get_cur();
    socket_win->rx.request = VSF_SOCKET_WIN_RECV;
    socket_win->rx.args.recv.buf = buf;
    socket_win->rx.args.recv.len = len;
    socket_win->rx.args.recv.flags = flags;
    socket_win->rx.args.recv.remote_addr = remote_addr;
    __vsf_arch_irq_request_send(&socket_win->rx.irq_request);
    vsf_thread_wfe(VSF_EVT_USER);
    return SOCKET_ERROR == socket_win->rx.ret ? VSF_ERR_FAIL : VSF_ERR_NONE;
}

static vsf_err_t __vk_dns_win_gethostbyname(const char *name, vk_netdrv_addr_t *addr)
{
    int ret;

    VSF_TCPIP_ASSERT(__vk_socket_win.is_started);

    vsf_thread_mutex_enter(&__vk_socket_win.mutex, -1);
        __vk_socket_win.eda = vsf_eda_get_cur();
        __vk_socket_win.request = VSF_SOCKET_WIN_REQ_GETHOSTBYNAME;
        __vk_socket_win.param.gethostbyname.name = name;
        __vsf_arch_irq_request_send(&__vk_socket_win.irq_request);
        vsf_thread_wfe(VSF_EVT_USER);
        ret = __vk_socket_win.ret;
        if (addr != NULL) {
            *addr = __vk_socket_win.param.gethostbyname.addr;
        }
    vsf_thread_mutex_leave(&__vk_socket_win.mutex);
    return SOCKET_ERROR == ret ? VSF_ERR_FAIL : VSF_ERR_NONE;
}

#endif      // VSF_USE_TCPIP
