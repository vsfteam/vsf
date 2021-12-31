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

#define __VSF_USBIP_DCD_CLASS_IMPLEMENT_BACKEND__
#include "../vsf_usbip_dcd.h"

#if     VSF_USE_USB_DEVICE == ENABLED && VSF_USBD_USE_DCD_USBIP == ENABLED      \
    &&  VSF_USBIP_DCD_CFG_BACKEND == VSF_USBIP_DCD_CFG_BACKEND_WIN

#include <Windows.h>

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

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
    vsf_dlist_t urb_done_list;
} vk_usbip_server_backend_t;

/*============================ PROTOTYPES ====================================*/

extern vsf_err_t __vk_usbip_server_done_urb(vk_usbip_server_t *server, vk_usbip_urb_t *urb);

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static vk_usbip_server_backend_t __vk_usbip_server_backend;

/*============================ IMPLEMENTATION ================================*/

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
                    vsf_dlist_remove_head(vk_usbip_urb_t, urb_node_ep, &backend->urb_done_list, urb);
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

void __vk_usbip_server_backend_init(vk_usbip_server_t *server)
{
    __vk_usbip_server_backend.server = server;

    __vsf_arch_irq_request_init(&__vk_usbip_server_backend.rx.irq_request);
    __vsf_arch_irq_init(&__vk_usbip_server_backend.rx.use_as__vsf_arch_irq_thread_t,
        "usbip_server_rx", __vk_usbip_server_backend_thread_rx, VSF_USBD_CFG_HW_PRIORITY);

    __vsf_arch_irq_request_init(&__vk_usbip_server_backend.tx.irq_request);
    __vsf_arch_irq_init(&__vk_usbip_server_backend.tx.use_as__vsf_arch_irq_thread_t,
        "usbip_server_tx", __vk_usbip_server_backend_thread_tx, VSF_USBD_CFG_HW_PRIORITY);
}

void __vk_usbip_server_backend_close(void)
{
    __vk_usbip_server_backend.rx.mem.buffer = NULL;
    __vsf_arch_irq_request_send(&__vk_usbip_server_backend.rx.irq_request);
}

void __vk_usbip_server_backend_recv(uint8_t *buff, uint_fast32_t size)
{
    __vk_usbip_server_backend.rx.mem.buffer = buff;
    __vk_usbip_server_backend.rx.mem.size = size;
    __vsf_arch_irq_request_send(&__vk_usbip_server_backend.rx.irq_request);
}

void __vk_usbip_server_backend_send(uint8_t *buff, uint_fast32_t size)
{
    __vk_usbip_server_backend.tx.mem.buffer = buff;
    __vk_usbip_server_backend.tx.mem.size = size;
    __vsf_arch_irq_request_send(&__vk_usbip_server_backend.tx.irq_request);
}

void __vk_usbip_server_backend_send_urb(vk_usbip_urb_t *urb)
{
    vk_usbip_server_backend_t *backend = &__vk_usbip_server_backend;
    vsf_protect_t orig = vsf_protect_int();
        vsf_dlist_add_to_tail(vk_usbip_urb_t, urb_node_ep, &backend->urb_done_list, urb);
    vsf_unprotect_int(orig);

    __vk_usbip_server_backend.tx.mem.buffer = NULL;
    __vsf_arch_irq_request_send(&__vk_usbip_server_backend.tx.irq_request);
}

#endif      // VSF_USE_USB_DEVICE && VSF_USBD_USE_DCD_USBIP && VSF_USBIP_DCD_CFG_BACKEND_WIN
