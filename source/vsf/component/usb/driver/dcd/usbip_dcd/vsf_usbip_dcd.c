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

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_DEVICE == ENABLED && VSF_USBD_USE_DCD_USBIP == ENABLED

#define __VSF_EDA_CLASS_INHERIT__
#define __VSF_USBIP_DCD_CLASS_IMPLEMENT_BACKEND__
#define __VSF_USBIP_DCD_CLASS_IMPLEMENT

#include "kernel/vsf_kernel.h"
#include "./vsf_usbip_dcd.h"

#include <Windows.h>

/*============================ MACROS ========================================*/

#define VSF_USBIP_VERSION                       0x0111

/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_USBIP_SERVER_CFG_DEBUG == ENABLED
#   define __vk_usbip_server_trace(...)                                         \
            vsf_trace_debug("usbip_server: " __VA_ARGS__)
#   define __vk_usbip_server_trace_buffer(...)                                  \
            vsf_trace_buffer(VSF_TRACE_DEBUG, __VA_ARGS__, VSF_TRACE_DF_DEFAULT)
#else
#   define __vk_usbip_server_trace(...)
#   define __vk_usbip_server_trace_buffer(...)
#endif

#if VSF_USBIP_SERVER_CFG_DEBUG_TRAFFIC == ENABLED
#   define __vk_usbip_server_trace_rx(__buffer, __size)                         \
        do {                                                                    \
            __vk_usbip_server_trace("recv %d bytes" VSF_TRACE_CFG_LINEEND, __size);\
            __vk_usbip_server_trace_buffer(__buffer, __size);                   \
        } while (0)

#   define __vk_usbip_server_trace_tx(__buffer, __size)                         \
        do {                                                                    \
            __vk_usbip_server_trace("send %d bytes" VSF_TRACE_CFG_LINEEND, __size);\
            __vk_usbip_server_trace_buffer(__buffer, __size);                   \
        } while (0)
#else
#   define __vk_usbip_server_trace_rx(__buffer, __size)
#   define __vk_usbip_server_trace_tx(__buffer, __size)
#endif

#if VSF_USBIP_SERVER_CFG_DEBUG_URB == ENABLED
#   define __vk_usbip_server_trace_urb_submit(__urb)                            \
        do {                                                                    \
            __vk_usbip_server_trace("submit urb%d %s%d %d bytes" VSF_TRACE_CFG_LINEEND,\
                                    (__urb)->req.seqnum,                        \
                                    (__urb)->req.direction ? "IN" : "OUT",      \
                                    (__urb)->req.ep,                            \
                                    (__urb)->req.transfer_length);              \
            if (!(__urb)->req.ep) {                                             \
                __vk_usbip_server_trace_buffer(&((__urb)->req.setup), 8);       \
            }                                                                   \
            if (!(__urb)->req.direction && (__urb)->req.transfer_length) {      \
                __vk_usbip_server_trace_buffer((__urb)->dynmem.buffer, (__urb)->req.transfer_length);\
            }                                                                   \
        } while (0)

#   define __vk_usbip_server_trace_urb_done(__urb)                              \
        do {                                                                    \
            __vk_usbip_server_trace("done urb%d %s%d " VSF_TRACE_CFG_LINEEND,   \
                                    (__urb)->req.seqnum,                        \
                                    (__urb)->req.direction ? "IN" : "OUT",      \
                                    (__urb)->req.ep);                           \
            uint_fast32_t actual_length = be32_to_cpu((__urb)->rep.actual_length);\
            if ((__urb)->req.direction && actual_length) {                      \
                __vk_usbip_server_trace_buffer((__urb)->dynmem.buffer, actual_length);\
            }                                                                   \
        } while (0)

#   define __vk_usbip_server_trace_urb_unlink(__urb)                            \
        do {                                                                    \
            __vk_usbip_server_trace("unlink urb%d %s%d " VSF_TRACE_CFG_LINEEND, \
                                    (__urb)->req.seqnum,                        \
                                    (__urb)->req.direction ? "IN" : "OUT",      \
                                    (__urb)->req.ep);                           \
        } while (0)
#else
#   define __vk_usbip_server_trace_urb_submit(__urb)
#   define __vk_usbip_server_trace_urb_done(__urb)
#   define __vk_usbip_server_trace_urb_unlink(__urb)
#endif

/*============================ TYPES =========================================*/

enum {
    VSF_USBIP_SERVER_EVT                        = VSF_EVT_USER + 0,
    VSF_USBIP_SERVER_EVT_BACKEND_INIT_DONE      = VSF_USBIP_SERVER_EVT + 0,
    VSF_USBIP_SERVER_EVT_BACKEND_CONNECTED      = VSF_USBIP_SERVER_EVT + 1,
    VSF_USBIP_SERVER_EVT_BACKEND_DISCONNECTED   = VSF_USBIP_SERVER_EVT + 2,
    VSF_USBIP_SERVER_EVT_BACKEND_RECV_DONE      = VSF_USBIP_SERVER_EVT + 3,
    VSF_USBIP_SERVER_EVT_BACKEND_SEND_DONE      = VSF_USBIP_SERVER_EVT + 4,

    VSF_USBIP_USBD_EVT                          = VSF_EVT_USER + 0x10,
};

#if VSF_USBIP_DCD_CFG_BACKEND == VSF_USBIP_DCD_CFG_BACKEND_WIN

typedef struct vk_usbip_server_backend_irq_thread_t {
    implement(vsf_arch_irq_thread_t);
    vsf_arch_irq_request_t irq_request;
    vsf_mem_t mem;
} vk_usbip_server_backend_irq_thread_t;

typedef struct vk_usbip_server_backend_t {
    vk_usbip_server_t *server;

    SOCKET listener_socket;
    SOCKET socket;

    vk_usbip_server_backend_irq_thread_t rx;
    vk_usbip_server_backend_irq_thread_t tx;
} vk_usbip_server_backend_t;

#endif

/*============================ PROTOTYPES ====================================*/

static vsf_err_t __vk_usbip_server_done_urb(vk_usbip_server_t *server, vk_usbip_urb_t *urb);

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static vk_usbip_server_t __vk_usbip_server;

#if VSF_USBIP_DCD_CFG_BACKEND == VSF_USBIP_DCD_CFG_BACKEND_WIN
static vk_usbip_server_backend_t __vk_usbip_server_backend;
#endif

/*============================ IMPLEMENTATION ================================*/

imp_vsf_pool(vk_usbip_urb_poll, vk_usbip_urb_t)

#if VSF_USBIP_DCD_CFG_BACKEND == VSF_USBIP_DCD_CFG_BACKEND_WIN

#pragma comment(lib,"WS2_32.lib")

static void __vk_usbip_server_backend_thread_rx(void *arg)
{
    vk_usbip_server_backend_irq_thread_t *irq_thread = (vk_usbip_server_backend_irq_thread_t *)arg;
    vsf_arch_irq_request_t *irq_request = &irq_thread->irq_request;

    vk_usbip_server_backend_t *backend = container_of(irq_thread, vk_usbip_server_backend_t, rx);
    vk_usbip_server_t *server = backend->server;

    __vsf_arch_irq_set_background(&irq_thread->use_as__vsf_arch_irq_thread_t);

    WSADATA wsaData;
    WORD sockVersion = MAKEWORD(2, 2);
    server->err = 0 == WSAStartup(sockVersion, &wsaData) ?
            VSF_ERR_NONE : VSF_ERR_FAIL;
    if (VSF_ERR_NONE == server->err) {
        backend->listener_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (INVALID_SOCKET == backend->listener_socket) {
            server->err = VSF_ERR_FAIL;
        }
    }
    if (VSF_ERR_NONE == server->err) {
        SOCKADDR_IN sin = {
            .sin_family             = AF_INET,
            .sin_port               = htons(server->port),
            .sin_addr.S_un.S_addr   = INADDR_ANY,
        };
        if (SOCKET_ERROR == bind(backend->listener_socket, (LPSOCKADDR)&sin, sizeof(sin))) {
            server->err = VSF_ERR_FAIL;
        }
    }
    if (VSF_ERR_NONE == server->err) {
        if (SOCKET_ERROR == listen(backend->listener_socket, 1)) {
            server->err = VSF_ERR_FAIL;
        }
    }

    __vsf_arch_irq_start(&irq_thread->use_as__vsf_arch_irq_thread_t);
        vsf_eda_post_evt(&server->teda.use_as__vsf_eda_t, VSF_USBIP_SERVER_EVT_BACKEND_INIT_DONE);
    __vsf_arch_irq_end(&irq_thread->use_as__vsf_arch_irq_thread_t, false);

    while (VSF_ERR_NONE == server->err) {
        SOCKADDR_IN remote;
        int len = sizeof(remote);
        backend->socket = accept(backend->listener_socket, (SOCKADDR *)&remote, &len);
        if (INVALID_SOCKET == backend->socket) {
            server->err = VSF_ERR_FAIL;
        } else {
            __vsf_arch_irq_start(&irq_thread->use_as__vsf_arch_irq_thread_t);
                vsf_eda_post_evt(&server->teda.use_as__vsf_eda_t, VSF_USBIP_SERVER_EVT_BACKEND_CONNECTED);
            __vsf_arch_irq_end(&irq_thread->use_as__vsf_arch_irq_thread_t, false);
        }

        while (VSF_ERR_NONE == server->err) {
            __vsf_arch_irq_request_pend(irq_request);

            if (NULL == irq_thread->mem.buffer) {
            close_session:
                closesocket(backend->socket);
                break;
            } else {
                int want_size = irq_thread->mem.size;
                char *buffer = (char *)irq_thread->mem.buffer;
                int ret;
                while (want_size > 0) {
                    ret = recv(backend->socket, buffer, want_size, 0);
                    if (ret <= 0) {
                        goto close_session;
                    }
                    want_size -= ret;
                    buffer += ret;
                }
            }

            __vsf_arch_irq_start(&irq_thread->use_as__vsf_arch_irq_thread_t);
                if (irq_thread->mem.size > 0) {
                    __vk_usbip_server_trace_rx(irq_thread->mem.buffer, irq_thread->mem.size);
                }
                vsf_eda_post_evt(&server->teda.use_as__vsf_eda_t, VSF_USBIP_SERVER_EVT_BACKEND_RECV_DONE);
            __vsf_arch_irq_end(&irq_thread->use_as__vsf_arch_irq_thread_t, false);
        }

        __vsf_arch_irq_start(&irq_thread->use_as__vsf_arch_irq_thread_t);
            vsf_eda_post_evt(&server->teda.use_as__vsf_eda_t, VSF_USBIP_SERVER_EVT_BACKEND_DISCONNECTED);
        __vsf_arch_irq_end(&irq_thread->use_as__vsf_arch_irq_thread_t, false);
    }

    WSACleanup();
}

static void __vk_usbip_server_backend_thread_tx(void *arg)
{
    vk_usbip_server_backend_irq_thread_t *irq_thread = (vk_usbip_server_backend_irq_thread_t *)arg;
    vsf_arch_irq_request_t *irq_request = &irq_thread->irq_request;

    vk_usbip_server_backend_t *backend = container_of(irq_thread, vk_usbip_server_backend_t, tx);
    vk_usbip_server_t *server = backend->server;

    __vsf_arch_irq_set_background(&irq_thread->use_as__vsf_arch_irq_thread_t);
    while (VSF_ERR_NONE == server->err) {
        __vsf_arch_irq_request_pend(irq_request);

        if (NULL == irq_thread->mem.buffer) {
            vk_usbip_urb_t *urb = NULL;
            uint_fast32_t actual_length;

            while (1) {
                __vsf_arch_irq_start(&irq_thread->use_as__vsf_arch_irq_thread_t);
                    vsf_dlist_remove_head(vk_usbip_urb_t, urb_node_ep, &server->urb_done_list, urb);
                __vsf_arch_irq_end(&irq_thread->use_as__vsf_arch_irq_thread_t, false);

                if (NULL == urb) {
                    break;
                }

                send(backend->socket, (char *)&urb->rep, 48, 0);
                if (urb->is_unlinked) {
                    actual_length = 0;
                } else if (urb->req.direction) {
                    actual_length = be32_to_cpu(urb->rep.actual_length);
                } else {
                    actual_length = 0;
                }
                if (actual_length > 0) {
                    send(backend->socket, (char *)urb->dynmem.buffer, actual_length, 0);
                }

                __vsf_arch_irq_start(&irq_thread->use_as__vsf_arch_irq_thread_t);
                    __vk_usbip_server_trace_tx(&urb->rep, 48);
                    if (actual_length > 0) {
                        __vk_usbip_server_trace_tx(urb->dynmem.buffer, actual_length);
                    }

                    __vk_usbip_server_done_urb(server, urb);
                __vsf_arch_irq_end(&irq_thread->use_as__vsf_arch_irq_thread_t, false);
            }

            irq_thread->mem.size = 0;
        } else {
            if (irq_thread->mem.size != send(backend->socket, (char *)irq_thread->mem.buffer, irq_thread->mem.size, 0)) {
                server->err = VSF_ERR_FAIL;
            }
        }

        __vsf_arch_irq_start(&irq_thread->use_as__vsf_arch_irq_thread_t);
            if (irq_thread->mem.buffer != NULL) {
                if ((VSF_ERR_NONE == server->err) && (irq_thread->mem.size > 0)) {
                    __vk_usbip_server_trace_tx(irq_thread->mem.buffer, irq_thread->mem.size);
                }
                irq_thread->mem.buffer = NULL;
                vsf_eda_post_evt(&server->teda.use_as__vsf_eda_t, VSF_USBIP_SERVER_EVT_BACKEND_SEND_DONE);
            }
        __vsf_arch_irq_end(&irq_thread->use_as__vsf_arch_irq_thread_t, false);
    }
}

static void __vk_usbip_server_backend_init(vk_usbip_server_t *server)
{
    __vk_usbip_server_backend.server = server;

    __vsf_arch_irq_request_init(&__vk_usbip_server_backend.rx.irq_request);
    __vsf_arch_irq_init(&__vk_usbip_server_backend.rx.use_as__vsf_arch_irq_thread_t,
        "usbip_server_rx", __vk_usbip_server_backend_thread_rx, VSF_USBD_CFG_HW_PRIORITY);

    __vsf_arch_irq_request_init(&__vk_usbip_server_backend.tx.irq_request);
    __vsf_arch_irq_init(&__vk_usbip_server_backend.tx.use_as__vsf_arch_irq_thread_t,
        "usbip_server_tx", __vk_usbip_server_backend_thread_tx, VSF_USBD_CFG_HW_PRIORITY);
}

static void __vk_usbip_server_backend_close(void)
{
    __vk_usbip_server_backend.rx.mem.buffer = NULL;
    __vsf_arch_irq_request_send(&__vk_usbip_server_backend.rx.irq_request);
}

static void __vk_usbip_server_backend_recv(uint8_t *buff, uint_fast32_t size)
{
    __vk_usbip_server_backend.rx.mem.buffer = buff;
    __vk_usbip_server_backend.rx.mem.size = size;
    __vsf_arch_irq_request_send(&__vk_usbip_server_backend.rx.irq_request);
}

static void __vk_usbip_server_backend_send(uint8_t *buff, uint_fast32_t size)
{
    __vk_usbip_server_backend.tx.mem.buffer = buff;
    __vk_usbip_server_backend.tx.mem.size = size;
    __vsf_arch_irq_request_send(&__vk_usbip_server_backend.tx.irq_request);
}

static void __vk_usbip_server_backend_send_urb(vk_usbip_urb_t *urb)
{
    __vk_usbip_server_backend.tx.mem.buffer = NULL;
    __vsf_arch_irq_request_send(&__vk_usbip_server_backend.tx.irq_request);
}
#elif VSF_USBIP_DCD_CFG_BACKEND == VSF_USBIP_DCD_CFG_BACKEND_VSF
// TODO: use vsf tcp stream as backend
#elif VSF_USBIP_DCD_CFG_BACKEND == VSF_USBIP_DCD_CFG_BACKEND_LIB
extern void __vk_usbip_server_backend_init(vk_usbip_server_t *server);
extern void __vk_usbip_server_backend_close(void);
extern void __vk_usbip_server_backend_recv(uint8_t *buff, uint_fast32_t size);
extern void __vk_usbip_server_backend_send(uint8_t *buff, uint_fast32_t size);
#endif

static void __vk_usbip_usbd_notify(vk_usbip_dcd_t *usbd, usb_evt_t evt, uint_fast8_t value)
{
    if (usbd->callback.evt_handler != NULL) {
        usbd->callback.evt_handler(usbd->callback.param, evt, value);
    }
}

static vk_usbip_dcd_ep_t * __vk_usbip_usbd_get_ep(vk_usbip_dcd_t *usbd, uint_fast8_t ep)
{
    vk_usbip_dcd_ep_t *dcd_ep;
    if ((ep & USB_DIR_MASK) == USB_DIR_IN) {
        ep &= ~USB_DIR_MASK;
        dcd_ep = usbd->ep_in;
    } else {
        dcd_ep = usbd->ep_out;
    }
    VSF_USB_ASSERT(ep < 16);
    return &dcd_ep[ep];
}

static vk_usbip_dcd_ep_t * __vk_usbip_usbd_get_ep_by_urb(vk_usbip_dcd_t *usbd, vk_usbip_urb_t *urb)
{
    uint_fast8_t ep = urb->req.ep & (urb->req.direction ? 0 : USB_DIR_MASK);
    return __vk_usbip_usbd_get_ep(usbd, ep);
}

static vk_usbip_urb_t * __vk_usbip_usbd_peek_urb(vk_usbip_dcd_ep_t *dcd_ep)
{
    vk_usbip_urb_t *urb = NULL;
    vsf_dlist_peek_head(vk_usbip_urb_t, urb_node_ep, &dcd_ep->urb_list, urb);
    return urb;
}

static void __vk_usbip_usbd_transfer_done(vk_usbip_dcd_t *usbd, vk_usbip_dcd_ep_t *dcd_ep)
{
    uint_fast8_t ep = dcd_ep->ep & 0x0F;

    dcd_ep->transfer.pending = false;
    if ((dcd_ep->ep & USB_DIR_MASK) == USB_DIR_IN) {
        __vk_usbip_usbd_notify(usbd, USB_ON_IN, ep);
    } else {
        __vk_usbip_usbd_notify(usbd, USB_ON_OUT, ep);
    }
}

static bool __vk_usbip_usbd_is_short(vk_usbip_dcd_ep_t *dcd_ep, uint_fast32_t transfer_size)
{
    uint_fast32_t short_size = transfer_size % dcd_ep->size;
    return !transfer_size || ((short_size > 0) && (short_size < dcd_ep->size));
}

static void __vk_usbip_usbd_transfer_send(vk_usbip_dcd_t *usbd, vk_usbip_dcd_ep_t *dcd_ep)
{
    vk_usbip_urb_t *urb = __vk_usbip_usbd_peek_urb(dcd_ep);
    if (urb != NULL) {
        uint_fast32_t real_size = min(urb->req.transfer_length, dcd_ep->transfer.mem.size);

        memcpy(urb->mem.buffer, dcd_ep->transfer.mem.buffer, real_size);
        dcd_ep->transfer.mem.buffer += real_size;
        dcd_ep->transfer.mem.size -= real_size;
        urb->mem.buffer += real_size;
        urb->mem.size -= real_size;
        urb->rep.actual_length += real_size;

        if ((!urb->mem.size || __vk_usbip_usbd_is_short(dcd_ep, real_size)) && (urb->req.ep != 0)) {
            vsf_dlist_remove(vk_usbip_urb_t, urb_node_ep, &dcd_ep->urb_list, urb);
            vsf_eda_post_msg(&__vk_usbip_server.teda.use_as__vsf_eda_t, urb);
        }

        if (!dcd_ep->transfer.mem.size) {
            __vk_usbip_usbd_transfer_done(usbd, dcd_ep);
        }
    }
}

static void __vk_usbip_usbd_transfer_recv(vk_usbip_dcd_t *usbd, vk_usbip_dcd_ep_t *dcd_ep)
{
    vk_usbip_urb_t *urb = __vk_usbip_usbd_peek_urb(dcd_ep);
    if (urb != NULL) {
        uint_fast32_t real_size = min(urb->req.transfer_length, dcd_ep->transfer.mem.size);

        memcpy(dcd_ep->transfer.mem.buffer, urb->mem.buffer, real_size);
        dcd_ep->transfer.mem.buffer += real_size;
        dcd_ep->transfer.mem.size -= real_size;
        urb->mem.buffer += real_size;
        urb->mem.size -= real_size;
        urb->rep.actual_length += real_size;

        if ((!urb->mem.size) && (urb->req.ep != 0)) {
            vsf_dlist_remove(vk_usbip_urb_t, urb_node_ep, &dcd_ep->urb_list, urb);
            vsf_eda_post_msg(&__vk_usbip_server.teda.use_as__vsf_eda_t, urb);
        }

        // TODO: check urb->req.transfer_flags for URB_ZERO_PACKET
        if (!dcd_ep->transfer.mem.size || __vk_usbip_usbd_is_short(dcd_ep, real_size)) {
            __vk_usbip_usbd_transfer_done(usbd, dcd_ep);
        }
    }
}

static vsf_err_t __vk_usbip_server_commit_urb(vk_usbip_server_t *server, vk_usbip_dcd_ep_t *dcd_ep)
{
    vk_usbip_dcd_t *usbd = server->usbd;
    vk_usbip_urb_t *urb = NULL;
next:
    vsf_dlist_peek_head(vk_usbip_urb_t, urb_node_ep, &dcd_ep->urb_list, urb);
    if (urb != NULL) {
        vsf_protect_t orig = vsf_protect_int();
        if (!urb->is_unlinked) {
            urb->state = VSF_USBIP_URB_COMITTED;
            vsf_unprotect_int(orig);
        } else {
            vsf_dlist_add_to_tail(vk_usbip_urb_t, urb_node_ep, &server->urb_done_list, urb);
            vsf_unprotect_int(orig);

            __vk_usbip_server_backend_send_urb(urb);
            goto next;
        }

        if (0 == urb->req.ep) {
            usbd->setup = urb->req.setup;
            __vk_usbip_usbd_notify(usbd, USB_ON_SETUP, 0);
        } else if (dcd_ep->transfer.pending) {
            // TODO: __vk_usbip_server_commit_urb maybe called in interrupt
            //      make sure it's safe to call __vk_usbip_usbd_transfer_send/recv
            if (urb->req.direction) {
                __vk_usbip_usbd_transfer_send(usbd, dcd_ep);
            } else {
                __vk_usbip_usbd_transfer_recv(usbd, dcd_ep);
            }
        }
        return VSF_ERR_NONE;
    }
    return VSF_ERR_FAIL;
}

static vsf_err_t __vk_usbip_server_done_urb(vk_usbip_server_t *server, vk_usbip_urb_t *urb)
{
    if (urb->is_unlinked) {
        __vk_usbip_server_trace_urb_unlink(urb);
    } else {
        __vk_usbip_server_trace_urb_done(urb);
    }

    vsf_protect_t orig = vsf_protect_int();
        vsf_dlist_add_to_tail(vk_usbip_urb_t, urb_node_ep, &server->urb_free_list, urb);
    vsf_unprotect_int(orig);

    vk_usbip_dcd_ep_t *dcd_ep = __vk_usbip_usbd_get_ep_by_urb(server->usbd, urb);
    return __vk_usbip_server_commit_urb(server, dcd_ep);
}

static vsf_err_t __vk_usbip_server_submit_urb(vk_usbip_server_t *server, vk_usbip_urb_t *urb)
{
    uint_fast8_t ep = urb->req.ep;
    vk_usbip_dcd_ep_t *dcd_ep;
    bool is_to_commit;

    memcpy(&urb->rep, &urb->req, sizeof(urb->rep));
    urb->rep.actual_length = 0;
    urb->rep.error_count = 0;
    urb->rep.status = 0;
    vsf_dlist_init_node(vk_usbip_urb_t, urb_node, urb);
    vsf_dlist_init_node(vk_usbip_urb_t, urb_node_ep, urb);
    if (urb->req.direction) {
        dcd_ep = &server->usbd->ep_in[ep];
    } else {
        dcd_ep = &server->usbd->ep_out[ep];
    }
    is_to_commit = vsf_dlist_is_empty(&dcd_ep->urb_list);
    vsf_dlist_add_to_tail(vk_usbip_urb_t, urb_node, &server->urb_list, urb);
    vsf_dlist_add_to_tail(vk_usbip_urb_t, urb_node_ep, &dcd_ep->urb_list, urb);
    urb->state = VSF_USBIP_URB_QUEUED;

    __vk_usbip_server_trace_urb_submit(urb);
    if (is_to_commit) {
        __vk_usbip_server_commit_urb(server, dcd_ep);
    }
    return VSF_ERR_NONE;
}

static void __vk_usbip_server_control_msg(vk_usbip_server_t *server, struct usb_ctrlrequest_t *req, uint8_t *buffer)
{
    vk_usbip_urb_t *urb = VSF_POOL_ALLOC(vk_usbip_urb_poll, &server->urb_pool);
    VSF_USB_ASSERT(urb != NULL);

    memset(urb, 0, sizeof(*urb));
    urb->state = VSF_USBIP_URB_IDLE;
    urb->req.direction = (req->bRequestType & USB_DIR_MASK) == USB_DIR_IN ? 1 : 0;
    //urb->ep = 0;
    urb->req.transfer_length = req->wLength;
    urb->req.setup = *req;
    urb->mem.buffer = buffer;
    urb->mem.size = urb->req.transfer_length;
    __vk_usbip_server_submit_urb(server, urb);
}

static void __vk_usbip_server_get_descriptor(vk_usbip_server_t *server,
    uint_fast8_t type, uint_fast8_t index, uint_fast16_t size, uint8_t *buffer)
{
    struct usb_ctrlrequest_t req = {
        .bRequestType    =  USB_RECIP_DEVICE | USB_DIR_IN,
        .bRequest        =  USB_REQ_GET_DESCRIPTOR,
        .wValue          =  (type << 8) + index,
        .wIndex          =  index,
        .wLength         =  size,
    };
    __vk_usbip_server_control_msg(server, &req, buffer);
}

static void __vk_usbip_server_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vk_usbip_server_t *server = container_of(eda, vk_usbip_server_t, teda);
    vk_usbip_dcd_t *usbd = server->usbd;
    vk_usbip_urb_t *urb;
    vsf_protect_t orig;

    switch (evt) {
    case VSF_EVT_INIT:
        VSF_POOL_PREPARE(vk_usbip_urb_poll, &server->urb_pool);
        __vk_usbip_server_backend_init(server);
        // fall through
    case VSF_EVT_TIMER:
        do {
            orig = vsf_protect_int();
                vsf_dlist_remove_head(vk_usbip_urb_t, urb_node_ep, &server->urb_free_list, urb);
            vsf_unprotect_int(orig);
            if (urb != NULL) {
                vsf_dlist_remove(vk_usbip_urb_t, urb_node, &server->urb_list, urb);
                if (urb->dynmem.buffer != NULL) {
                    vsf_heap_free(urb->dynmem.buffer);
                }
                VSF_POOL_FREE(vk_usbip_urb_poll, &__vk_usbip_server.urb_pool, urb);
            }
        } while (urb != NULL);
        vsf_teda_set_timer_ms(100);
        break;
    case VSF_USBIP_SERVER_EVT_BACKEND_INIT_DONE:
        if (server->err != VSF_ERR_NONE) {
            __vk_usbip_server_trace("fail to init usbip_server backend" VSF_TRACE_CFG_LINEEND);
            break;
        }
        server->is_connected = false;

        __vk_usbip_usbd_notify(server->usbd, USB_ON_RESET, 0);
        break;
    case VSF_USBIP_SERVER_EVT_BACKEND_CONNECTED:
        __vk_usbip_server_trace("client connected" VSF_TRACE_CFG_LINEEND);
        server->is_connected = true;
    recv_command:
        server->req_state = VSF_USBIP_SERVER_REQ_COMMAND;
        __vk_usbip_server_backend_recv((uint8_t *)&server->req.cmd32, 4);
        break;
    case VSF_USBIP_SERVER_EVT_BACKEND_DISCONNECTED:
        __vk_usbip_server_trace("client disconnected" VSF_TRACE_CFG_LINEEND);
        server->is_connected = false;
        break;
    case VSF_USBIP_SERVER_EVT_BACKEND_RECV_DONE:
        if (server->is_connected && (VSF_ERR_NONE == server->err)) {
            switch (server->req_state) {
            case VSF_USBIP_SERVER_REQ_COMMAND:
                switch (be32_to_cpu(server->req.cmd32)) {
                case USBIP_CMD_SUBMIT:
                    server->command = USBIP_CMD_SUBMIT;
                    __vk_usbip_server_backend_recv((uint8_t *)&server->req.submit + 4, 44);
                    break;
                case USBIP_CMD_UNLINK:
                    server->command = USBIP_CMD_UNLINK;
                    __vk_usbip_server_backend_recv((uint8_t *)&server->req.unlink + 4, 44);
                    break;
                default:
                    switch (be16_to_cpu(server->req.cmd16)) {
                    case USBIP_REQ_DEVLIST:
                        server->command = USBIP_REQ_DEVLIST;
                        __vk_usbip_server_backend_recv((uint8_t *)&server->req.devlist + 4, sizeof(server->req.devlist) - 4);
                        break;
                    case USBIP_REQ_IMPORT:
                        server->command = USBIP_REQ_IMPORT;
                        __vk_usbip_server_backend_recv((uint8_t *)&server->req.import + 4, sizeof(server->req.import) - 4);
                        break;
                    default:
                        __vk_usbip_server_trace("unknown command" VSF_TRACE_CFG_LINEEND);
                        server->command = USBIP_CMD_INVALID;
                        break;
                    }
                }
                if (server->command != USBIP_CMD_INVALID) {
                    server->req_state = VSF_USBIP_SERVER_REQ_PARAM;
                }
                break;
            case VSF_USBIP_SERVER_REQ_PARAM:
                switch (server->command) {
                case USBIP_CMD_SUBMIT:
                    __vk_usbip_server_trace("recv USBIP_CMD_SUBMIT" VSF_TRACE_CFG_LINEEND);
                    urb = VSF_POOL_ALLOC(vk_usbip_urb_poll, &server->urb_pool);
                    VSF_USB_ASSERT(urb != NULL);

                    memset(urb, 0, sizeof(*urb));
                    urb->req = server->req.submit;
                    urb->req.seqnum = be32_to_cpu(urb->req.seqnum);
                    urb->req.ep = be32_to_cpu(urb->req.ep);
                    urb->req.transfer_length = be32_to_cpu(urb->req.transfer_length);
                    if (urb->req.transfer_length) {
                        urb->dynmem.size = urb->req.transfer_length;
                        urb->dynmem.buffer = vsf_heap_malloc(urb->dynmem.size);
                        VSF_USB_ASSERT(urb->dynmem.buffer != NULL);
                    }
                    urb->mem = urb->dynmem;
                    if (!urb->req.direction && urb->req.transfer_length) {
                        server->cur_urb = urb;
                        server->req_state = VSF_USBIP_SERVER_REQ_DATA;
                        __vk_usbip_server_backend_recv(urb->mem.buffer, urb->mem.size);
                        break;
                    }
                    __vk_usbip_server_submit_urb(server, urb);
                    goto recv_command;
                case USBIP_CMD_UNLINK:
                    __vk_usbip_server_trace("recv USBIP_CMD_UNLINK" VSF_TRACE_CFG_LINEEND);
                    server->req.unlink.seqnum_to_unlink = be32_to_cpu(server->req.unlink.seqnum_to_unlink);
                    urb = NULL;
                    __vsf_dlist_foreach_unsafe(vk_usbip_urb_t, urb_node, &server->urb_list) {
                        if (server->req.unlink.seqnum_to_unlink == _->req.seqnum) {
                            urb = _;
                            break;
                        }
                    }
                    VSF_USB_ASSERT(urb != NULL);

                    bool is_to_commit;
                    bool is_to_dequeue;
                    orig = vsf_protect_int();
                        urb->is_unlinked = true;
                        is_to_dequeue = urb->state >= VSF_USBIP_URB_QUEUED && urb->state < VSF_USBIP_URB_DONE;
                        is_to_commit = urb->state == VSF_USBIP_URB_COMITTED;
                    vsf_unprotect_int(orig);

                    if (is_to_dequeue) {
                        uint_fast32_t status = urb->rep.status;
                        vk_usbip_dcd_ep_t *dcd_ep = __vk_usbip_usbd_get_ep_by_urb(usbd, urb);
                        vsf_dlist_remove(vk_usbip_urb_t, urb_node_ep, &dcd_ep->urb_list, urb);

                        urb->unlink.command = cpu_to_be32(USBIP_RET_UNLINK);
                        urb->unlink.seqnum = server->req.unlink.seqnum;
                        urb->unlink.ep = cpu_to_be32(urb->req.ep);
                        urb->unlink.status = cpu_to_be32(status);

                        orig = vsf_protect_int();
                            vsf_dlist_add_to_tail(vk_usbip_urb_t, urb_node_ep, &server->urb_done_list, urb);
                        vsf_unprotect_int(orig);
                        __vk_usbip_server_backend_send_urb(urb);

                        if (is_to_commit) {
                            dcd_ep->transfer.mem.size = 0;
                            __vk_usbip_server_commit_urb(server, dcd_ep);
                        }
                    }
                    goto recv_command;
                case USBIP_REQ_DEVLIST:
                    __vk_usbip_server_trace("recv USBIP_REQ_DEVLIST" VSF_TRACE_CFG_LINEEND);
                    server->reply = USBIP_REP_DEVLIST;
                    server->rep.common.version_bcd = cpu_to_be16(VSF_USBIP_VERSION);
                    server->rep.common.code = cpu_to_be16(USBIP_REP_DEVLIST & ~0x8000);
                    server->rep.common.status = 0;
                    if (usbd->is_connected) {
                        server->rep.devlist.devnum = cpu_to_be32(1);
                        server->rep_state = VSF_USBIP_SERVER_REP_REPLY;
                        __vk_usbip_server_backend_send((uint8_t *)&server->rep.devlist, sizeof(server->rep.devlist));
                    } else {
                        server->rep.devlist.devnum = 0;
                        server->rep_state = VSF_USBIP_SERVER_REP_DONE;
                        __vk_usbip_server_backend_send((uint8_t *)&server->rep.devlist, sizeof(server->rep.devlist));
                    }
                    goto recv_command;
                case USBIP_REQ_IMPORT:
                    __vk_usbip_server_trace("recv USBIP_REQ_IMPORT" VSF_TRACE_CFG_LINEEND);
                    server->reply = USBIP_REP_IMPORT;
                    server->rep.common.version_bcd = cpu_to_be16(VSF_USBIP_VERSION);
                    server->rep.common.code = cpu_to_be16(USBIP_REP_IMPORT & ~0x8000);
                    if (!strcmp(server->req.import.busid, strrchr(VSF_USBIP_DCD_CFG_PATH, '/') + 1)) {
                        server->rep.common.status = 0;
                        server->rep_state = VSF_USBIP_SERVER_REP_REPLY;
                        __vk_usbip_server_backend_send((uint8_t *)&server->rep.import, sizeof(server->rep.import));
                    } else {
                        server->rep.common.status = 1;
                        server->rep_state = VSF_USBIP_SERVER_REP_DONE;
                        __vk_usbip_server_backend_send((uint8_t *)&server->rep.common, sizeof(server->rep.common));
                    }
                    goto recv_command;
                default:
                    VSF_USB_ASSERT(false);
                    break;
                }
                break;
            case VSF_USBIP_SERVER_REQ_DATA:
                switch (server->command) {
                case USBIP_CMD_SUBMIT:
                    urb = server->cur_urb;
                    __vk_usbip_server_submit_urb(server, urb);
                    goto recv_command;
                }
                break;
            }
        }
        break;
    case VSF_USBIP_SERVER_EVT_BACKEND_SEND_DONE:
        if (VSF_USBIP_SERVER_REP_DONE == server->rep_state) {
            server->reply = USBIP_CMD_INVALID;
            break;
        }

        switch (server->reply) {
        case USBIP_RET_SUBMIT:
            VSF_USB_ASSERT(VSF_USBIP_SERVER_REP_REPLY == server->rep_state);

            break;
        case USBIP_RET_UNLINK:
            break;
        case USBIP_REP_IMPORT:
            switch (server->rep_state) {
            case VSF_USBIP_SERVER_REP_REPLY:
                goto send_path;
            case VSF_USBIP_SERVER_REP_IMPORT_PATH:
                goto send_busid;
            case VSF_USBIP_SERVER_REP_IMPORT_BUSID:
                server->rep_state = VSF_USBIP_SERVER_REP_DONE;
                __vk_usbip_server_backend_send((uint8_t *)&server->dev, sizeof(server->dev) - sizeof(server->dev.ifs));
                break;
            default:
                VSF_USB_ASSERT(false);
            }
            break;
        case USBIP_REP_DEVLIST:
            switch (server->rep_state) {
            case VSF_USBIP_SERVER_REP_REPLY:
            send_path:
                memset(&server->rep, 0, 256);
                strcpy((char *)&server->rep, VSF_USBIP_DCD_CFG_PATH);
                server->rep_state = VSF_USBIP_SERVER_REP_DEVLIST_PATH;
                __vk_usbip_server_backend_send((uint8_t *)&server->rep, 256);
                break;
            case VSF_USBIP_SERVER_REP_DEVLIST_PATH:
            send_busid:
                memset(&server->rep, 0, 32);
                strcpy((char *)&server->rep, strrchr(VSF_USBIP_DCD_CFG_PATH, '/') + 1);
                server->rep_state = VSF_USBIP_SERVER_REP_DEVLIST_BUSID;
                __vk_usbip_server_backend_send((uint8_t *)&server->rep, 32);
                break;
            case VSF_USBIP_SERVER_REP_DEVLIST_BUSID:
                server->rep_state = VSF_USBIP_SERVER_REP_DEVLIST_DEV;
                __vk_usbip_server_get_descriptor(server, USB_DT_DEVICE, 0, USB_DT_DEVICE_SIZE, (uint8_t *)&server->rep);
                break;
            default:
                VSF_USB_ASSERT(false);
            }
            break;
        default:
            VSF_USB_ASSERT(false);
        }
        break;
    case VSF_EVT_MESSAGE: {
            vk_usbip_urb_t *urb = vsf_eda_get_cur_msg();
            VSF_USB_ASSERT(urb != NULL);

            if ((server->rep_state != VSF_USBIP_SERVER_REP_DONE) && (USBIP_REP_DEVLIST == server->reply)) {
                VSF_USB_ASSERT(0 == urb->rep.status);

                union {
                    void *ptr;
                    struct usb_descriptor_header_t *header;
                    struct usb_device_desc_t *dev;
                    struct usb_config_desc_t *cfg;
                    struct usb_interface_desc_t *ifs;
                } desc;
                desc.ptr = (void *)&server->rep;
                vk_usbip_rep_dev_t *dev = &server->dev;

                // remove from server->urb_list, because urb will not be put into done_list and free_list
                vsf_dlist_remove(vk_usbip_urb_t, urb_node, &server->urb_list, urb);

                switch (server->rep_state) {
                case VSF_USBIP_SERVER_REP_DEVLIST_DEV:
                    dev->busnum = 0;
                    dev->devnum = 0;
                    dev->speed = cpu_to_be32(USB_SPEED_HIGH);
                    dev->idVendor = cpu_to_be16(desc.dev->idVendor);
                    dev->idProduct = cpu_to_be16(desc.dev->idProduct);
                    dev->bcdDevice = cpu_to_be16(desc.dev->bcdDevice);
                    dev->bDeviceClass = desc.dev->bDeviceClass;
                    dev->bDeviceSubClass = desc.dev->bDeviceSubClass;
                    dev->bDeviceProtocol = desc.dev->bDeviceProtocol;
                    dev->bNumConfigurations = desc.dev->bNumConfigurations;

                    // get configure descriptor index 0
                    urb->req.transfer_length = USB_DT_CONFIG_SIZE;
                    urb->req.setup.wValue = cpu_to_le16((USB_DT_CONFIG << 8) + 0);
                    urb->req.setup.wLength = cpu_to_le16(urb->req.transfer_length);
                    urb->mem.buffer = (uint8_t *)&server->rep;
                    urb->mem.size = urb->req.transfer_length;

                    server->rep_state = VSF_USBIP_SERVER_REP_DEVLIST_CFG;
                    __vk_usbip_server_submit_urb(server, urb);
                    break;
                case VSF_USBIP_SERVER_REP_DEVLIST_CFG:
                    if (urb->rep.actual_length < desc.cfg->wTotalLength) {
                        VSF_USB_ASSERT(sizeof(server->rep) >= desc.cfg->wTotalLength);
                        // get full configure descriptor index 0
                        urb->req.transfer_length = desc.cfg->wTotalLength;
                        urb->req.setup.wValue = cpu_to_le16((USB_DT_CONFIG << 8) + 0);
                        urb->req.setup.wLength = cpu_to_le16(urb->req.transfer_length);
                        urb->mem.buffer = (uint8_t *)&server->rep;
                        urb->mem.size = urb->req.transfer_length;
                        __vk_usbip_server_submit_urb(server, urb);
                    } else {
                        VSF_USB_ASSERT(urb->rep.actual_length == desc.cfg->wTotalLength);
                        dev->bConfigurationValue = desc.cfg->bConfigurationValue;
                        dev->bNumInterfaces = desc.cfg->bNumInterfaces;
                        VSF_USB_ASSERT(dev->bNumInterfaces <= dimof(dev->ifs));

                        for (uint8_t i = 0; i < dev->bNumInterfaces; i++) {
                            while (desc.header->bDescriptorType != USB_DT_INTERFACE) {
                                desc.header = (struct usb_descriptor_header_t *)((uint8_t *)desc.header + desc.header->bLength);
                            }
                            dev->ifs[i].bInterfaceClass = desc.ifs->bInterfaceClass;
                            dev->ifs[i].bInterfaceSubClass = desc.ifs->bInterfaceSubClass;
                            dev->ifs[i].bInterfaceProtocol = desc.ifs->bInterfaceProtocol;
                            dev->ifs[i].padding = 0;
                            desc.header = (struct usb_descriptor_header_t *)((uint8_t *)desc.header + desc.header->bLength);
                        }

                        VSF_POOL_FREE(vk_usbip_urb_poll, &server->urb_pool, urb);
                        server->rep_state = VSF_USBIP_SERVER_REP_DONE;
                        __vk_usbip_server_backend_send((uint8_t *)dev, sizeof(*dev) - sizeof(dev->ifs) + dev->bNumInterfaces * sizeof(dev->ifs[0]));
                    }
                    break;
                default:
                    VSF_USB_ASSERT(false);
                }
            } else {
                urb->rep.command = cpu_to_be32(USBIP_RET_SUBMIT);
                urb->rep.seqnum = cpu_to_be32(urb->rep.seqnum);
                urb->rep.ep = cpu_to_be32(urb->rep.ep);
                urb->rep.status = cpu_to_be32(urb->rep.status);
                urb->rep.actual_length = cpu_to_be32(urb->rep.actual_length);
                urb->rep.error_count = cpu_to_be32(urb->rep.error_count);
                server->rep_state = VSF_USBIP_SERVER_REP_REPLY;
                orig = vsf_protect_int();
                    vsf_dlist_add_to_tail(vk_usbip_urb_t, urb_node_ep, &server->urb_done_list, urb);
                vsf_unprotect_int(orig);
                __vk_usbip_server_backend_send_urb(urb);
                break;
            }
        }
        break;
    default:
        VSF_USB_ASSERT(false);
    }
}

static vsf_err_t __vk_usbip_server_init(vk_usbip_server_t *server, vk_usbip_dcd_t *usbd, usb_dc_cfg_t *cfg)
{
    if (NULL == server->usbd) {
        server->usbd = usbd;
        if ((usbd->param != NULL) && (usbd->param->port != 0)) {
            server->port = usbd->param->port;
        } else {
            server->port = 3240;
        }

        server->teda.fn.evthandler = __vk_usbip_server_evthandler;
#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE == ENABLED
        server->teda.on_terminate = NULL;

#endif
        return vsf_teda_init(&server->teda, VSF_USBD_CFG_EDA_PRIORITY, false);
    }
    return VSF_ERR_NONE;
}

vsf_err_t vk_usbip_usbd_init(vk_usbip_dcd_t *usbd, usb_dc_cfg_t *cfg)
{
    VSF_USB_ASSERT((usbd != NULL) && (cfg != NULL));

    usbd->is_connected = false;
    usbd->frame_number = 0;
    usbd->callback.evt_handler = cfg->evt_handler;
    usbd->callback.param = cfg->param;
    return __vk_usbip_server_init(&__vk_usbip_server, usbd, cfg);
}

void vk_usbip_usbd_fini(vk_usbip_dcd_t *usbd)
{
    // TODO:
}

void vk_usbip_usbd_reset(vk_usbip_dcd_t *usbd, usb_dc_cfg_t *cfg)
{
    memset(usbd->ep, 0, sizeof(usbd->ep));
    usbd->address = 0;
}

void vk_usbip_usbd_connect(vk_usbip_dcd_t *usbd)
{
    if (!usbd->is_connected) {
        usbd->is_connected = true;
        // todo: need notify?
    }
}

void vk_usbip_usbd_disconnect(vk_usbip_dcd_t *usbd)
{
    if (usbd->is_connected) {
        usbd->is_connected = false;
        // todo: need notify?
    }
}

void vk_usbip_usbd_wakeup(vk_usbip_dcd_t *usbd)
{
}

void vk_usbip_usbd_set_address(vk_usbip_dcd_t *usbd, uint_fast8_t addr)
{
    usbd->address = addr;
}

uint_fast8_t vk_usbip_usbd_get_address(vk_usbip_dcd_t *usbd)
{
    return usbd->address;
}

uint_fast16_t vk_usbip_usbd_get_frame_number(vk_usbip_dcd_t *usbd)
{
    return usbd->frame_number;
}

extern uint_fast8_t vk_usbip_usbd_get_mframe_number(vk_usbip_dcd_t *usbd)
{
    return usbd->mframe_number;
}

void vk_usbip_usbd_get_setup(vk_usbip_dcd_t *usbd, uint8_t *buffer)
{
    memcpy(buffer, (uint8_t *)&usbd->setup, sizeof(usbd->setup));
}

void vk_usbip_usbd_status_stage(vk_usbip_dcd_t *usbd, bool is_in)
{
    vk_usbip_dcd_ep_t *dcd_ep = is_in ? &usbd->ep_out[0] : &usbd->ep_in[0];
    vk_usbip_urb_t *urb = NULL;

    vsf_dlist_remove_head(vk_usbip_urb_t, urb_node_ep, &dcd_ep->urb_list, urb);
    VSF_USB_ASSERT(urb != NULL);

    __vk_usbip_usbd_notify(usbd, USB_ON_STATUS, 0);
    vsf_eda_post_msg(&__vk_usbip_server.teda.use_as__vsf_eda_t, urb);
}

uint_fast8_t vk_usbip_usbd_ep_get_feature(vk_usbip_dcd_t *usbd, uint_fast8_t ep, uint_fast8_t feature)
{
    return USB_DC_FEATURE_TRANSFER;
}

vsf_err_t vk_usbip_usbd_ep_add(vk_usbip_dcd_t *usbd, uint_fast8_t ep, usb_ep_type_t type, uint_fast16_t size)
{
    vk_usbip_dcd_ep_t *dcd_ep = __vk_usbip_usbd_get_ep(usbd, ep);
    dcd_ep->ep = ep;
    dcd_ep->type = type;
    dcd_ep->size = size;
    return VSF_ERR_NONE;
}

uint_fast16_t vk_usbip_usbd_ep_get_size(vk_usbip_dcd_t *usbd, uint_fast8_t ep)
{
    vk_usbip_dcd_ep_t *dcd_ep = __vk_usbip_usbd_get_ep(usbd, ep);
    return dcd_ep->size;
}

vsf_err_t vk_usbip_usbd_ep_set_stall(vk_usbip_dcd_t *usbd, uint_fast8_t ep)
{
    vk_usbip_dcd_ep_t *dcd_ep = __vk_usbip_usbd_get_ep(usbd, ep);
    dcd_ep->is_stalled = true;
    return VSF_ERR_NONE;
}

bool vk_usbip_usbd_ep_is_stalled(vk_usbip_dcd_t *usbd, uint_fast8_t ep)
{
    vk_usbip_dcd_ep_t *dcd_ep = __vk_usbip_usbd_get_ep(usbd, ep);
    return dcd_ep->is_stalled;
}

vsf_err_t vk_usbip_usbd_ep_clear_stall(vk_usbip_dcd_t *usbd, uint_fast8_t ep)
{
    vk_usbip_dcd_ep_t *dcd_ep = __vk_usbip_usbd_get_ep(usbd, ep);
    dcd_ep->is_stalled = false;
    return VSF_ERR_NONE;
}

uint_fast32_t vk_usbip_usbd_ep_get_data_size(vk_usbip_dcd_t *usbd, uint_fast8_t ep)
{
    vk_usbip_dcd_ep_t *dcd_ep = __vk_usbip_usbd_get_ep(usbd, ep);
    return dcd_ep->transfer.size - dcd_ep->transfer.mem.size;
}

vsf_err_t vk_usbip_usbd_ep_transaction_read_buffer(vk_usbip_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size)
{
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t vk_usbip_usbd_ep_transaction_enable_out(vk_usbip_dcd_t *usbd, uint_fast8_t ep)
{
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t vk_usbip_usbd_ep_transaction_set_data_size(vk_usbip_dcd_t *usbd, uint_fast8_t ep, uint_fast16_t size)
{
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t vk_usbip_usbd_ep_transaction_write_buffer(vk_usbip_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size)
{
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t vk_usbip_usbd_ep_transfer_recv(vk_usbip_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast32_t size)
{
    vk_usbip_dcd_ep_t *dcd_ep = __vk_usbip_usbd_get_ep(usbd, ep | USB_DIR_OUT);
    dcd_ep->transfer.mem.buffer = buffer;
    dcd_ep->transfer.mem.size = size;
    dcd_ep->transfer.size = size;
    dcd_ep->transfer.pending = true;

    __vk_usbip_usbd_transfer_recv(usbd, dcd_ep);
    return VSF_ERR_NONE;
}

vsf_err_t vk_usbip_usbd_ep_transfer_send(vk_usbip_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast32_t size, bool zlp)
{
    vk_usbip_dcd_ep_t *dcd_ep = __vk_usbip_usbd_get_ep(usbd, ep | USB_DIR_IN);
    dcd_ep->transfer.mem.buffer = buffer;
    dcd_ep->transfer.mem.size = size;
    dcd_ep->transfer.size = size;
    dcd_ep->transfer.zlp = zlp;
    dcd_ep->transfer.pending = true;

    __vk_usbip_usbd_transfer_send(usbd, dcd_ep);
    return VSF_ERR_NONE;
}

// TODO: call in irq_thread
void vk_usbip_usbd_irq(vk_usbip_dcd_t *usbd)
{
    
}

#endif
