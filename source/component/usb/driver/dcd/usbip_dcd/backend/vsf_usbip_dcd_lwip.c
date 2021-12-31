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

#define __VSF_EDA_CLASS_INHERIT__
#define __VSF_USBIP_DCD_CLASS_IMPLEMENT_BACKEND__
#include "../vsf_usbip_dcd.h"

#if     VSF_USE_USB_DEVICE == ENABLED && VSF_USBD_USE_DCD_USBIP == ENABLED      \
    &&  VSF_USBIP_DCD_CFG_BACKEND == VSF_USBIP_DCD_CFG_BACKEND_LWIP

#include "lwip/tcpip.h"
#include "lwip/tcp.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vk_usbip_server_lwip_t {
    vk_usbip_server_t *server;
    sys_thread_t thread;

    struct tcp_pcb *listener_pcb;
    struct tcp_pcb *work_pcb;

    struct pbuf *pbuf_rx;
    vsf_mem_t mem_rx;

    vsf_dlist_t urb_list;
    u16_t to_send_len;
    u16_t sent_len;
    bool is_to_exit;
} vk_usbip_server_lwip_t;

/*============================ PROTOTYPES ====================================*/

extern vsf_err_t __vk_usbip_server_done_urb(vk_usbip_server_t *server, vk_usbip_urb_t *urb);

// extern pbuf_free_header, which is maybe not implemented in older lwip
extern struct pbuf * pbuf_free_header(struct pbuf *q, u16_t size);

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
    backend->sent_len = 0;
    if (backend->pbuf_rx != NULL) {
        pbuf_free(backend->pbuf_rx);
        backend->pbuf_rx = NULL;
    }

    vk_usbip_urb_t *urb;
    while (!vsf_dlist_is_empty(&backend->urb_list)) {
        vsf_dlist_remove_head(vk_usbip_urb_t, urb_node_ep, &backend->urb_list, urb);
        __vk_usbip_server_done_urb(backend->server, urb);
    }
}

static err_t __vk_usbip_server_lwip_on_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
    vk_usbip_server_lwip_t *backend = (vk_usbip_server_lwip_t *)arg;
    vk_usbip_urb_t *urb;

    backend->sent_len += len;

    while (1) {
        vsf_dlist_peek_head(vk_usbip_urb_t, urb_node_ep, &backend->urb_list, urb);
        if (urb != NULL) {
            u16_t urb_len = 48 + (urb->req.direction ? be32_to_cpu(urb->rep.actual_length) : 0);
            if (backend->sent_len >= urb_len) {
                backend->sent_len -= urb_len;
                vsf_dlist_remove_head(vk_usbip_urb_t, urb_node_ep, &backend->urb_list, urb);
                __vk_usbip_server_done_urb(backend->server, urb);
                continue;
            }
        } else if (backend->to_send_len > 0) {
            VSF_USB_ASSERT(backend->sent_len <= backend->to_send_len);
            if (backend->sent_len == backend->to_send_len) {
                backend->sent_len = 0;
                backend->to_send_len = 0;
                vsf_eda_post_evt(&backend->server->teda.use_as__vsf_eda_t, VSF_USBIP_SERVER_EVT_BACKEND_SEND_DONE);
            }
        }
        break;
    }
    return ERR_OK;
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

static void __vk_usbip_server_lwip_on_err(void *arg, err_t err)
{
    vk_usbip_server_lwip_t *backend = (vk_usbip_server_lwip_t *)arg;
    vk_usbip_server_t *server = backend->server;

    __vk_usbip_server_lwip_reset(backend);
    vsf_eda_post_evt(&server->teda.use_as__vsf_eda_t, VSF_USBIP_SERVER_EVT_BACKEND_DISCONNECTED);
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
    tcp_sent(newpcb, __vk_usbip_server_lwip_on_sent);
    tcp_err(newpcb, __vk_usbip_server_lwip_on_err);
    vsf_eda_post_evt(&server->teda.use_as__vsf_eda_t, VSF_USBIP_SERVER_EVT_BACKEND_CONNECTED);
    return ERR_OK;
}

static void __vk_usbip_server_backend_thread(void *param)
{
    vk_usbip_server_lwip_t *backend = (vk_usbip_server_lwip_t *)param;
    vk_usbip_server_t *server = backend->server;

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

    vsf_evt_t evt;
    err_t err;
    while (1) {
        evt = vsf_thread_wait();
        if (backend->is_to_exit) {
            break;
        }

        switch (evt) {
        case VSF_EVT_MESSAGE:
            if (backend->to_send_len > 0) {
                char *buff = vsf_eda_get_cur_msg();
                LOCK_TCPIP_CORE();
                    err = tcp_write(backend->work_pcb, buff, backend->to_send_len, 0);
                    err += tcp_output(backend->work_pcb);
                UNLOCK_TCPIP_CORE();
                VSF_USB_ASSERT(ERR_OK == err);
            } else {
                vk_usbip_urb_t *urb = vsf_eda_get_cur_msg();
                uint_fast32_t actual_length = (urb->is_unlinked || !urb->req.direction) ?
                                                0 : be32_to_cpu(urb->rep.actual_length);
                LOCK_TCPIP_CORE();
                    vsf_dlist_add_to_tail(vk_usbip_urb_t, urb_node_ep, &backend->urb_list, urb);
                    err = tcp_write(backend->work_pcb, (char *)&urb->rep, 48, 0);
                    if (actual_length > 0) {
                        err += tcp_write(backend->work_pcb, (char *)urb->dynmem.buffer, actual_length, 0);
                    }
                    err += tcp_output(backend->work_pcb);
                UNLOCK_TCPIP_CORE();
                VSF_USB_ASSERT(ERR_OK == err);
            }
            break;
        }
    }

    LOCK_TCPIP_CORE();
        tcp_close(backend->work_pcb);
    UNLOCK_TCPIP_CORE();
    backend->work_pcb = NULL;
}

void __vk_usbip_server_backend_init(vk_usbip_server_t *server)
{
    vk_usbip_server_lwip_t *backend = &__vk_usbip_server_lwip;

    memset(backend, 0, sizeof(*backend));
    backend->server = server;
    backend->thread = sys_thread_new("usbip_lwip", __vk_usbip_server_backend_thread, backend, 1024, TCPIP_THREAD_PRIO);
}

void __vk_usbip_server_backend_close(void)
{
    vk_usbip_server_lwip_t *backend = &__vk_usbip_server_lwip;
    backend->is_to_exit = true;
    vsf_eda_post_evt((vsf_eda_t *)backend->thread, VSF_EVT_USER);
}

void __vk_usbip_server_backend_recv(uint8_t *buff, uint_fast32_t size)
{
    vk_usbip_server_lwip_t *backend = &__vk_usbip_server_lwip;
    if (NULL == backend->work_pcb) {
        return;
    }
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
    if (NULL == backend->work_pcb) {
        return;
    }
    VSF_USB_ASSERT(vsf_dlist_is_empty(&backend->urb_list));
    VSF_USB_ASSERT(0 == backend->to_send_len);

    backend->to_send_len = size;
    vsf_eda_post_msg((vsf_eda_t *)backend->thread, buff);
}

void __vk_usbip_server_backend_send_urb(vk_usbip_urb_t *urb)
{
    vk_usbip_server_lwip_t *backend = &__vk_usbip_server_lwip;
    if (NULL == backend->work_pcb) {
        return;
    }
    VSF_USB_ASSERT(0 == backend->to_send_len);

    vsf_eda_post_msg((vsf_eda_t *)backend->thread, urb);
}

#endif      // VSF_USE_USB_DEVICE && VSF_USBD_USE_DCD_USBIP && VSF_USBIP_DCD_CFG_BACKEND_LWIP
