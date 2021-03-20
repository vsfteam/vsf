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

#define __VSF_EDA_CLASS_INHERIT__
#define __VSF_USBIP_DCD_CLASS_IMPLEMENT_BACKEND__
#include "../vsf_usbip_dcd.h"

#if     VSF_USE_USB_DEVICE == ENABLED && VSF_USBD_USE_DCD_USBIP == ENABLED      \
    &&  VSF_USBIP_DCD_CFG_BACKEND == VSF_USBIP_DCD_CFG_BACKEND_LWIP

#include "lwip/opt.h"
#include "lwip/tcpip.h"
#include "lwip/tcp.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vk_usbip_server_lwip_t {
    vk_usbip_server_t *server;

    struct tcp_pcb *listener_pcb;
    struct tcp_pcb *work_pcb;

    struct pbuf *pbuf_rx;
    vsf_mem_t mem_rx;
} vk_usbip_server_lwip_t;

/*============================ PROTOTYPES ====================================*/

extern vsf_err_t __vk_usbip_server_done_urb(vk_usbip_server_t *server, vk_usbip_urb_t *urb);

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static NO_INIT vk_usbip_server_lwip_t __vk_usbip_server_lwip;

/*============================ IMPLEMENTATION ================================*/

static void __vk_usbip_server_lwip_save_pbuf(vk_usbip_server_lwip_t *backend, struct pbuf *p)
{
    if (backend->pbuf_rx != NULL) {
        pbuf_cat(backend->pbuf_rx, p);
    } else {
        backend->pbuf_rx = p;
    }
}

static uint_fast32_t __vk_usbip_server_lwip_read(vk_usbip_server_lwip_t *backend, void *buffer, uint_fast32_t size)
{
    uint_fast32_t read_len = 0;
    while ((size > 0) && (backend->pbuf_rx != NULL)) {
        u16_t len = size;
        len = pbuf_copy_partial(backend->pbuf_rx, buffer, len, 0);
        size -= len;
        read_len += len;
        buffer = (uint8_t *)buffer + len;

        backend->pbuf_rx = pbuf_free_header(backend->pbuf_rx, len);
    }
    return read_len;
}

static void __vk_usbip_server_lwip_reset(vk_usbip_server_lwip_t *backend)
{
    backend->work_pcb = NULL;
    backend->mem_rx.size = 0;
    if (backend->pbuf_rx != NULL) {
        pbuf_free(backend->pbuf_rx);
        backend->pbuf_rx = NULL;
    }
}

static err_t __vk_usbip_server_lwip_on_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    vk_usbip_server_lwip_t *backend = (vk_usbip_server_lwip_t *)arg;
    vk_usbip_server_t *server = backend->server;
    bool is_to_send_evt = false;

    if (err != ERR_OK) {
        server->err = VSF_ERR_FAIL;
        return err;
    }
    if (NULL == p) {
        // closed
        tcp_close(backend->work_pcb);
        __vk_usbip_server_lwip_reset(backend);
        vsf_eda_post_evt(&server->teda.use_as__vsf_eda_t, VSF_USBIP_SERVER_EVT_BACKEND_DISCONNECTED);
        return ERR_OK;
    }

    tcp_recved(tpcb, p->tot_len);
    vsf_protect_t orig = vsf_protect_sched();
        __vk_usbip_server_lwip_save_pbuf(backend, p);
        if (backend->mem_rx.size > 0) {
            backend->mem_rx.size -= __vk_usbip_server_lwip_read(backend, backend->mem_rx.buffer, backend->mem_rx.size);
            is_to_send_evt = 0 == backend->mem_rx.size;
        }
    vsf_unprotect_sched(orig);

    if (is_to_send_evt) {
        vsf_eda_post_evt(&server->teda.use_as__vsf_eda_t, VSF_USBIP_SERVER_EVT_BACKEND_RECV_DONE);
    }
    return ERR_OK;
}

static err_t __vk_usbip_server_lwip_on_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
    vk_usbip_server_lwip_t *backend = (vk_usbip_server_lwip_t *)arg;
    vk_usbip_server_t *server = backend->server;

    if (err != ERR_OK) {
        server->err = VSF_ERR_FAIL;
        return err;
    }

    backend->work_pcb = newpcb;
    tcp_arg(newpcb, backend);
    tcp_accepted(newpcb);
    tcp_recv(newpcb, __vk_usbip_server_lwip_on_recv);
    vsf_eda_post_evt(&server->teda.use_as__vsf_eda_t, VSF_USBIP_SERVER_EVT_BACKEND_CONNECTED);
    return ERR_OK;
}

void __vk_usbip_server_backend_init(vk_usbip_server_t *server)
{
    vk_usbip_server_lwip_t *backend = &__vk_usbip_server_lwip;

    memset(backend, 0, sizeof(*backend));
    backend->server = server;

    LOCK_TCPIP_CORE();
    if (    (NULL == (backend->listener_pcb = tcp_new()))
        ||  (ERR_OK != tcp_bind(backend->listener_pcb, IP_ADDR_ANY, server->port))
        ||  (NULL == (backend->listener_pcb = tcp_listen_with_backlog(backend->listener_pcb, 1)))) {

        if (backend->listener_pcb != NULL) {
            tcp_close(backend->listener_pcb);
            backend->listener_pcb = NULL;
        }
        server->err = VSF_ERR_FAIL;
    } else {
        tcp_arg(backend->listener_pcb, backend);
        tcp_accept(backend->listener_pcb, __vk_usbip_server_lwip_on_accept);
    }
    UNLOCK_TCPIP_CORE();
    vsf_eda_post_evt(&server->teda.use_as__vsf_eda_t, VSF_USBIP_SERVER_EVT_BACKEND_INIT_DONE);
}

void __vk_usbip_server_backend_close(void)
{
    vk_usbip_server_lwip_t *backend = &__vk_usbip_server_lwip;
    LOCK_TCPIP_CORE();
    tcp_close(backend->work_pcb);
    UNLOCK_TCPIP_CORE();
    backend->work_pcb = NULL;
}

void __vk_usbip_server_backend_recv(uint8_t *buff, uint_fast32_t size)
{
    vk_usbip_server_lwip_t *backend = &__vk_usbip_server_lwip;

    VSF_USB_ASSERT(NULL != backend->work_pcb);
    VSF_USB_ASSERT(0 == backend->mem_rx.size);

    vsf_protect_t orig = vsf_protect_sched();
    uint_fast32_t read_len = __vk_usbip_server_lwip_read(backend, buff, size);
    if (read_len >= size) {
        vsf_unprotect_sched(orig);
        vsf_eda_post_evt(&backend->server->teda.use_as__vsf_eda_t, VSF_USBIP_SERVER_EVT_BACKEND_RECV_DONE);
    } else {
        backend->mem_rx.size = size - read_len;
        backend->mem_rx.buffer = buff + read_len;
        vsf_unprotect_sched(orig);
    }
}

void __vk_usbip_server_backend_send(uint8_t *buff, uint_fast32_t size)
{
    vk_usbip_server_lwip_t *backend = &__vk_usbip_server_lwip;
    err_t err;

    VSF_USB_ASSERT(NULL != backend->work_pcb);
    LOCK_TCPIP_CORE();
    err = tcp_write(backend->work_pcb, buff, size, 1);
    err += tcp_output(backend->work_pcb);
    UNLOCK_TCPIP_CORE();
    VSF_USB_ASSERT(ERR_OK == err);
    vsf_eda_post_evt(&backend->server->teda.use_as__vsf_eda_t, VSF_USBIP_SERVER_EVT_BACKEND_SEND_DONE);
}

void __vk_usbip_server_backend_send_urb(vk_usbip_urb_t *urb)
{
    vk_usbip_server_lwip_t *backend = &__vk_usbip_server_lwip;
    vk_usbip_server_t *server = backend->server;
    err_t err;
    VSF_USB_ASSERT(NULL != backend->work_pcb);

    uint_fast32_t actual_length;
    LOCK_TCPIP_CORE();
    err = tcp_write(backend->work_pcb, (char *)&urb->rep, 48, 1);
    UNLOCK_TCPIP_CORE();
    VSF_USB_ASSERT(ERR_OK == err);
    if (urb->is_unlinked) {
        actual_length = 0;
    } else if (urb->req.direction) {
        actual_length = be32_to_cpu(urb->rep.actual_length);
    } else {
        actual_length = 0;
    }
    if (actual_length > 0) {
        LOCK_TCPIP_CORE();
        err = tcp_write(backend->work_pcb, (char *)urb->dynmem.buffer, actual_length, 1);
        UNLOCK_TCPIP_CORE();
        VSF_USB_ASSERT(ERR_OK == err);
    }
    LOCK_TCPIP_CORE();
    err = tcp_output(backend->work_pcb);
    UNLOCK_TCPIP_CORE();
    VSF_USB_ASSERT(ERR_OK == err);

    __vk_usbip_server_done_urb(server, urb);
}

#endif      // VSF_USE_USB_DEVICE && VSF_USBD_USE_DCD_USBIP && VSF_USBIP_DCD_CFG_BACKEND_LWIP
