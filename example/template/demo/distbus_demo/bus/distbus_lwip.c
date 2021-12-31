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

#include "vsf.h"

#if     VSF_USE_DISTBUS == ENABLED &&  APP_USE_DISTBUS_DEMO == ENABLED          \
    &&  APP_DISTBUS_DEMO_CFG_LWIP == ENABLED

#include "lwip/tcpip.h"
#include "lwip/tcp.h"

/*============================ MACROS ========================================*/

#ifndef APP_DISTBUS_DEMO_CFG_LWIP_PORT
#   define APP_DISTBUS_DEMO_CFG_LWIP_PORT           7086
#endif

#ifdef __WIN__
// windows require a minial stack size of 8K(4K page + 4K guardian)
#   define __APP_DISTBUS_DEMO_CFG_STACK_SZIE        8192
#else
#   define __APP_DISTBUS_DEMO_CFG_STACK_SZIE        1024
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum {
    VSF_DISTBUS_LWIP_EVT_SEND = VSF_EVT_USER + 0,
};

typedef struct __user_distbus_lwip_t {
    sys_thread_t thread;

    struct tcp_pcb *listener_pcb;
    struct tcp_pcb *work_pcb;
    struct pbuf *pbuf_rx;

    void *on_inited_param;
    void (*on_inited)(void *p);

    void *buffer_to_send;
    uint32_t size_to_send;
    void *on_sent_param;
    void (*on_sent)(void *p);

    void *buffer_to_recv;
    uint32_t size_to_recv;
    void *on_recv_param;
    void (*on_recv)(void *p);

    bool is_to_exit;
} __user_distbus_lwip_t;

/*============================ PROTOTYPES ====================================*/

// extern pbuf_free_header, which is maybe not implemented in older lwip
extern struct pbuf * pbuf_free_header(struct pbuf *q, u16_t size);
extern bool __user_distbus_is_master(void);

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static __user_distbus_lwip_t __user_distbus_lwip;

/*============================ IMPLEMENTATION ================================*/

static void __user_distbus_lwip_reset(__user_distbus_lwip_t *distbus_lwip)
{
    distbus_lwip->work_pcb = NULL;
    distbus_lwip->size_to_recv = 0;
    distbus_lwip->size_to_send = 0;
    if (distbus_lwip->pbuf_rx != NULL) {
        pbuf_free(distbus_lwip->pbuf_rx);
        distbus_lwip->pbuf_rx = NULL;
    }
}

static void __user_distbus_lwip_save_pbuf(__user_distbus_lwip_t *distbus_lwip, struct pbuf *p)
{
    if (distbus_lwip->pbuf_rx != NULL) {
        pbuf_cat(distbus_lwip->pbuf_rx, p);
    } else {
        distbus_lwip->pbuf_rx = p;
    }
}

static uint_fast32_t __user_distbus_lwip_read(__user_distbus_lwip_t *distbus_lwip, void *buffer, uint_fast32_t size)
{
    uint_fast32_t read_len = 0;
    while ((size > 0) && (distbus_lwip->pbuf_rx != NULL)) {
        u16_t len = size;
        len = pbuf_copy_partial(distbus_lwip->pbuf_rx, buffer, len, 0);
        size -= len;
        read_len += len;
        buffer = (uint8_t *)buffer + len;

        distbus_lwip->pbuf_rx = pbuf_free_header(distbus_lwip->pbuf_rx, len);
    }
    return read_len;
}

static err_t __user_distbus_lwip_on_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    __user_distbus_lwip_t *distbus_lwip = (__user_distbus_lwip_t *)arg;
    bool is_to_notify = false;

    if (err != ERR_OK) {
        return err;
    }
    if (NULL == p) {
        // closed
        tcp_close(distbus_lwip->work_pcb);
        __user_distbus_lwip_reset(distbus_lwip);
        return ERR_OK;
    }

    tcp_recved(tpcb, p->tot_len);
    vsf_protect_t orig = vsf_protect_int();
        __user_distbus_lwip_save_pbuf(distbus_lwip, p);
        if (distbus_lwip->size_to_recv > 0) {
            distbus_lwip->size_to_recv -= __user_distbus_lwip_read(distbus_lwip,
                        distbus_lwip->buffer_to_recv, distbus_lwip->size_to_recv);
            is_to_notify = 0 == distbus_lwip->size_to_recv;
        }
    vsf_unprotect_int(orig);

    if (is_to_notify) {
        void (*on_recv)(void *p) = distbus_lwip->on_recv;
        if (on_recv != NULL) {
            on_recv(distbus_lwip->on_recv_param);
        }
    }
    return ERR_OK;
}

static err_t __user_distbus_lwip_on_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
    __user_distbus_lwip_t *distbus_lwip = (__user_distbus_lwip_t *)arg;
    return ERR_OK;
}

static void __user_distbus_lwip_on_err(void *arg, err_t err)
{
    __user_distbus_lwip_t *distbus_lwip = (__user_distbus_lwip_t *)arg;
    __user_distbus_lwip_reset(distbus_lwip);
}

static err_t __user_distbus_lwip_on_connected_or_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
    __user_distbus_lwip_t *distbus_lwip = (__user_distbus_lwip_t *)arg;

    if (err != ERR_OK) {
        return err;
    }

    distbus_lwip->work_pcb = newpcb;
    tcp_arg(newpcb, distbus_lwip);
    if (!__user_distbus_is_master()) {
        tcp_accepted(newpcb);
    }
    tcp_recv(newpcb, __user_distbus_lwip_on_recv);
    tcp_err(newpcb, __user_distbus_lwip_on_err);
    tcp_sent(newpcb, __user_distbus_lwip_on_sent);

    if (distbus_lwip->on_inited != NULL) {
        distbus_lwip->on_inited(distbus_lwip->on_inited_param);
    }
    return ERR_OK;
}

static err_t __user_distbus_lwip_on_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
    return __user_distbus_lwip_on_connected_or_accept(arg, newpcb, err);
}

static err_t __user_distbus_lwip_on_connected(void *arg, struct tcp_pcb *newpcb, err_t err)
{
    return __user_distbus_lwip_on_connected_or_accept(arg, newpcb, err);
}

static void __user_distbus_lwip_thread(void *param)
{
    __user_distbus_lwip_t *distbus_lwip = (__user_distbus_lwip_t *)param;

    LOCK_TCPIP_CORE();
        if (__user_distbus_is_master()) {
            struct tcp_pcb *tpcb = tcp_new();
            ip_addr_t ipaddr;
            // TODO: use configurable ipaddr
            IP_ADDR4(&ipaddr, 192, 168, 88, 1);
            if (    (NULL != tpcb)
                &&  (ERR_OK == tcp_connect(tpcb, &ipaddr, APP_DISTBUS_DEMO_CFG_LWIP_PORT,
                                            __user_distbus_lwip_on_connected))) {

                tcp_arg(tpcb, distbus_lwip);
            }
        } else {
            if (    (NULL == (distbus_lwip->listener_pcb = tcp_new()))
                ||  (ERR_OK != tcp_bind(distbus_lwip->listener_pcb, IP_ADDR_ANY, APP_DISTBUS_DEMO_CFG_LWIP_PORT))
                ||  (NULL == (distbus_lwip->listener_pcb = tcp_listen_with_backlog(distbus_lwip->listener_pcb, 1)))) {

                if (distbus_lwip->listener_pcb != NULL) {
                    tcp_close(distbus_lwip->listener_pcb);
                    distbus_lwip->listener_pcb = NULL;
                }
            } else {
                tcp_arg(distbus_lwip->listener_pcb, distbus_lwip);
                tcp_accept(distbus_lwip->listener_pcb, __user_distbus_lwip_on_accept);
            }
        }
    UNLOCK_TCPIP_CORE();

    vsf_evt_t evt;
    err_t err;
    while (1) {
        evt = vsf_thread_wait();
        if (distbus_lwip->is_to_exit) {
            break;
        }

        switch (evt) {
        case VSF_DISTBUS_LWIP_EVT_SEND:
            if (distbus_lwip->size_to_send > 0) {
                uint8_t *buffer = distbus_lwip->buffer_to_send;
                uint32_t size = distbus_lwip->size_to_send;
                void (*on_sent)(void *p) = distbus_lwip->on_sent;

                LOCK_TCPIP_CORE();
                    err = tcp_write(distbus_lwip->work_pcb, buffer, size, TCP_WRITE_FLAG_COPY);
                    VSF_ASSERT(ERR_OK == err);
                    if (ERR_OK == err) {
                        err = tcp_output(distbus_lwip->work_pcb);
                        VSF_ASSERT(ERR_OK == err);
                    }
                UNLOCK_TCPIP_CORE();

                distbus_lwip->size_to_send = 0;
                if (on_sent != NULL) {
                    on_sent(distbus_lwip->on_sent_param);
                }
            }
            break;
        }
    }

    LOCK_TCPIP_CORE();
        tcp_close(distbus_lwip->work_pcb);
    UNLOCK_TCPIP_CORE();
    distbus_lwip->work_pcb = NULL;
}

bool __user_distbus_init(void *p, void (*on_inited)(void *p))
{
    __user_distbus_lwip_t *distbus_lwip = &__user_distbus_lwip;

    distbus_lwip->is_to_exit = false;
    distbus_lwip->on_inited_param = p;
    distbus_lwip->on_inited = on_inited;
    distbus_lwip->thread = sys_thread_new("distbus_lwip",
                __user_distbus_lwip_thread, &__user_distbus_lwip,
                __APP_DISTBUS_DEMO_CFG_STACK_SZIE, TCPIP_THREAD_PRIO);
    return false;
}

bool __user_distbus_send(uint8_t *buffer, uint_fast32_t size, void *p, void (*on_sent)(void *p))
{
    __user_distbus_lwip_t *distbus_lwip = &__user_distbus_lwip;
    if (NULL == distbus_lwip->work_pcb) {
        return false;
    }
    VSF_ASSERT(0 == distbus_lwip->size_to_send);

    distbus_lwip->on_sent_param = p;
    distbus_lwip->on_sent = on_sent;
    distbus_lwip->size_to_send = size;
    distbus_lwip->buffer_to_send = buffer;
    vsf_eda_post_evt((vsf_eda_t *)distbus_lwip->thread, VSF_DISTBUS_LWIP_EVT_SEND);
    return false;
}

bool __user_distbus_recv(uint8_t *buffer, uint_fast32_t size, void *p, void (*on_recv)(void *p))
{
    __user_distbus_lwip_t *distbus_lwip = &__user_distbus_lwip;
    VSF_ASSERT(0 == distbus_lwip->size_to_recv);

    vsf_protect_t orig = vsf_protect_int();
    uint_fast32_t read_len = __user_distbus_lwip_read(distbus_lwip, buffer, size);
    if (read_len >= size) {
        vsf_unprotect_int(orig);
        return true;
    } else {
        distbus_lwip->on_recv_param = p;
        distbus_lwip->on_recv = on_recv;
        distbus_lwip->size_to_recv = size - read_len;
        distbus_lwip->buffer_to_recv = buffer + read_len;
        vsf_unprotect_int(orig);

        return false;
    }
}

#endif
