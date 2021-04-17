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

#include "service/vsf_service_cfg.h"

#if VSF_USE_DISTBUS == ENABLED

#define __VSF_DISTBUS_CLASS_IMPLEMENT
#include "hal/arch/vsf_arch.h"
#include "./vsf_distbus.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

static void __vsf_distbus_on_sent(void *p);

/*============================ IMPLEMENTATION ================================*/

vsf_distbus_msg_t * vsf_distbus_alloc_msg(vsf_distbus_t *distbus, uint_fast32_t size, uint8_t **buf)
{
    VSF_SERVICE_ASSERT((distbus != NULL) && (size > 0));
    size += sizeof(vsf_distbus_msg_t);
    vsf_distbus_msg_t *msg = distbus->op.mem.alloc_msg(size);
    if (msg != NULL) {
        msg->buffer_size = size;
        if (buf != NULL) {
            *buf = (uint8_t *)&msg->header + sizeof(msg->header);
        }
    }
    return msg;
}

void vsf_distbus_free_msg(vsf_distbus_t *distbus, vsf_distbus_msg_t *msg)
{
    VSF_SERVICE_ASSERT((distbus != NULL) && (msg != NULL));
    distbus->op.mem.free_msg(msg);
}

static uint16_t __vsf_distbus_hash(uint8_t *buffer, uint_fast32_t size)
{
    uint16_t hash = 0;
    for (uint_fast32_t i = 0; i < size; i++) {
        hash += *buffer++;
    }
    return hash;
}

static bool __vsf_distbus_send_msg(vsf_distbus_t *distbus, vsf_distbus_msg_t *msg)
{
    VSF_SERVICE_ASSERT((distbus != NULL) && (msg != NULL));
    uint_fast32_t size = msg->header.datalen + sizeof(msg->header);
    return distbus->op.bus.send((uint8_t *)&msg->header, size, distbus, __vsf_distbus_on_sent);
}

static void __vsf_distbus_on_sent(void *p)
{
    vsf_distbus_t *distbus = (vsf_distbus_t *)p;
    VSF_SERVICE_ASSERT((distbus != NULL) && (distbus->msg_tx != NULL));

    vsf_distbus_free_msg(distbus, distbus->msg_tx);
    vsf_protect_t orig = vsf_protect_int();
        vsf_slist_remove_from_head(vsf_distbus_msg_t, node, &distbus->msg_tx_list, distbus->msg_tx);
    vsf_unprotect_int(orig);

    if (distbus->msg_tx != NULL) {
        __vsf_distbus_send_msg(distbus, distbus->msg_tx);
    }
}

void vsf_distbus_send_msg(vsf_distbus_t *distbus, vsf_distbus_service_t *service, vsf_distbus_msg_t *msg)
{
    VSF_SERVICE_ASSERT((distbus != NULL) && (service != NULL) && (msg != NULL));

    msg->header.addr += service->addr_start;
    msg->header.hash_data = __vsf_distbus_hash((uint8_t *)&msg[1], msg->header.datalen);
    msg->header.hash_header = 0;
    msg->header.hash_header = __vsf_distbus_hash((uint8_t *)&msg->header, sizeof(msg->header));

    bool is_to_send_now;
    vsf_protect_t orig = vsf_protect_int();
        is_to_send_now = (NULL == distbus->msg_tx);
        if (is_to_send_now) {
            distbus->msg_tx = msg;
        } else {
            vsf_slist_append(vsf_distbus_msg_t, node, &distbus->msg_tx_list, msg);
        }
    vsf_unprotect_int(orig);

    if (is_to_send_now) {
        if (__vsf_distbus_send_msg(distbus, msg)) {
            __vsf_distbus_on_sent(distbus);
        }
    }
}

static void __vsf_distbus_on_recv(void *p)
{
    vsf_distbus_t *distbus = (vsf_distbus_t *)p;
    VSF_SERVICE_ASSERT((distbus != NULL) && (distbus->msg_rx != NULL));
    vsf_distbus_msg_t *msg = distbus->msg_rx;

    if (0 == msg->pos) {
        // header receviced
        uint_fast16_t hash = msg->header.hash_header;
        msg->header.hash_header = 0;
        if (hash != __vsf_distbus_hash((uint8_t *)&msg->header, sizeof(msg->header))) {
            goto on_hash_error;
        }

        msg->pos = sizeof(msg->header);
        if (distbus->op.bus.recv((uint8_t *)&msg->header + msg->pos, msg->header.datalen, distbus, __vsf_distbus_on_recv)) {
            __vsf_distbus_on_recv(distbus);
        }
    } else if (sizeof(msg->header) == msg->pos) {
        if (msg->header.hash_data != __vsf_distbus_hash((uint8_t *)&msg->header + msg->pos, msg->header.datalen)) {
        on_hash_error:
            if (distbus->op.on_error != NULL) {
                distbus->op.on_error(distbus);
            }

            vsf_distbus_free_msg(distbus, msg);
            distbus->msg_rx = NULL;
            return;
        }

        __vsf_slist_foreach_unsafe(vsf_distbus_service_t, node, &distbus->service_list) {
            if ((msg->header.addr >= _->addr_start) && (msg->header.addr < (_->addr_start + _->info->addr_range))) {
                msg->header.addr -= _->addr_start;
                if (_->info->handler(distbus, _, msg)) {
                    msg = vsf_distbus_alloc_msg(distbus, distbus->mtu, NULL);
                    VSF_SERVICE_ASSERT(msg != NULL);
                    distbus->msg_rx = msg;
                }
            }
        }

        msg->pos = 0;
        if (distbus->op.bus.recv((uint8_t *)&msg->header, sizeof(msg->header), distbus, __vsf_distbus_on_recv)) {
            __vsf_distbus_on_recv(distbus);
        }
    } else {
        VSF_SERVICE_ASSERT(false);
    }
}

vsf_err_t vsf_distbus_init(vsf_distbus_t *distbus)
{
    VSF_SERVICE_ASSERT(distbus != NULL);
    VSF_SERVICE_ASSERT(distbus->op.mem.alloc_msg != NULL);
    VSF_SERVICE_ASSERT(distbus->op.mem.free_msg != NULL);
    VSF_SERVICE_ASSERT(distbus->op.bus.recv != NULL);
    VSF_SERVICE_ASSERT(distbus->op.bus.send != NULL);

    distbus->cur_addr = 0;

    vsf_slist_init(&distbus->msg_tx_list);
    distbus->msg_tx = distbus->msg_rx = NULL;

    distbus->mtu = 0;
    __vsf_slist_foreach_unsafe(vsf_distbus_service_t, node, &distbus->service_list) {
        VSF_SERVICE_ASSERT((_->info != NULL) && (_->info->mtu > 0));
        distbus->mtu += _->info->mtu;
    }

    if (distbus->op.bus.init != NULL) {
        distbus->op.bus.init();
    }

    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(distbus, distbus->mtu, NULL);
    VSF_SERVICE_ASSERT(msg != NULL);

    distbus->msg_rx = msg;
    msg->pos = 0;
    if (distbus->op.bus.recv((uint8_t *)&msg->header, sizeof(msg->header), distbus, __vsf_distbus_on_recv)) {
        __vsf_distbus_on_recv(distbus);
    }
    return VSF_ERR_NONE;
}

void vsf_distbus_register_service(vsf_distbus_t *distbus, vsf_distbus_service_t *service)
{
    if (!vsf_slist_is_in(vsf_distbus_service_t, node, &distbus->service_list, service)) {
        service->addr_start = distbus->cur_addr;
        distbus->cur_addr += service->info->addr_range;

        vsf_slist_append(vsf_distbus_service_t, node, &distbus->service_list, service);
    }
}

#endif      // VSF_USE_DISTBUS
