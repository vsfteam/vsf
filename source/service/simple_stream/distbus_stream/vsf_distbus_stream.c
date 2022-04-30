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
#include "service/vsf_service_cfg.h"

#if VSF_USE_SIMPLE_STREAM == ENABLED && VSF_USE_DISTBUS == ENABLED

#define __VSF_SIMPLE_STREAM_CLASS_INHERIT__
#define __VSF_DISTBUS_CLASS_INHERIT__
#define __VSF_DISTBUS_STREAM_CLASS_IMPLEMENT
#include "../vsf_simple_stream.h"
#include "./vsf_distbus_stream.h"

#include "hal/arch/vsf_arch.h"

#if VSF_DISTBUS_CFG_DEBUG == ENABLED
#   include "service/trace/vsf_trace.h"
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

static void __vsf_distbus_stream_init(vsf_stream_t *stream);
static uint_fast32_t __vsf_distbus_stream_write(vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size);
static uint_fast32_t __vsf_distbus_stream_read(vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size);
static uint_fast32_t __vsf_distbus_stream_get_buff_length(vsf_stream_t *stream);
static uint_fast32_t __vsf_distbus_stream_get_data_length(vsf_stream_t *stream);
static uint_fast32_t __vsf_distbus_stream_get_avail_length(vsf_stream_t *stream);
static uint_fast32_t __vsf_distbus_stream_get_wbuf(vsf_stream_t *stream, uint8_t **ptr);
static uint_fast32_t __vsf_distbus_stream_get_rbuf(vsf_stream_t *stream, uint8_t **ptr);

/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/

const vsf_stream_op_t vsf_distbus_stream_op = {
    .init               = __vsf_distbus_stream_init,
    .fini               = __vsf_distbus_stream_init,
    .write              = __vsf_distbus_stream_write,
    .read               = __vsf_distbus_stream_read,
    .get_buff_length    = __vsf_distbus_stream_get_buff_length,
    .get_data_length    = __vsf_distbus_stream_get_data_length,
    .get_avail_length   = __vsf_distbus_stream_get_avail_length,
    .get_wbuf           = __vsf_distbus_stream_get_wbuf,
    .get_rbuf           = __vsf_distbus_stream_get_rbuf,
};

/*============================ IMPLEMENTATION ================================*/

bool __vsf_distbus_stream_msghandler(vsf_distbus_t *bus,
                        vsf_distbus_service_t *service, vsf_distbus_msg_t *msg)
{
    vsf_distbus_stream_t *distbus_stream =
        container_of(service, vsf_distbus_stream_t, use_as__vsf_distbus_service_t);
    bool retain_msg = false;
    uint8_t *data = (uint8_t *)&msg->header + sizeof(msg->header);

    switch (msg->header.addr) {
    case VSF_DISTBUS_STREAM_CMD_BUF_SIZE:
        distbus_stream->buf_size = get_unaligned_le32(data);
        __vsf_distbus_trace("distbus_stream: buf_size %d" VSF_TRACE_CFG_LINEEND, distbus_stream->buf_size);
        vsf_stream_connect_rx(&distbus_stream->use_as__vsf_stream_t);
        break;
    case VSF_DISTBUS_STREAM_CMD_DATA: {
            VSF_SERVICE_ASSERT(distbus_stream->is_tx);
            vsf_protect_t orig = vsf_protect_int();
                vsf_slist_queue_enqueue(vsf_distbus_msg_t, node, &distbus_stream->msgq, msg);
                distbus_stream->data_size += msg->header.datalen;
            vsf_unprotect_int(orig);
            retain_msg = true;
            __vsf_stream_on_write(&distbus_stream->use_as__vsf_stream_t);
        }
        break;
    case VSF_DISTBUS_STREAM_CMD_SIZE: {
            VSF_SERVICE_ASSERT(!distbus_stream->is_tx);
            uint32_t size = get_unaligned_le32(data);
            VSF_SERVICE_ASSERT(distbus_stream->data_size >= size);
            vsf_protect_t orig = vsf_protect_int();
                distbus_stream->data_size -= size;
            vsf_unprotect_int(orig);
            __vsf_stream_on_read(&distbus_stream->use_as__vsf_stream_t);
        }
        break;
    default:
        VSF_SERVICE_ASSERT(false);
        break;
    }
    return retain_msg;
}

static void __vsf_distbus_stream_init(vsf_stream_t *stream)
{
    vsf_distbus_stream_t *distbus_stream = (vsf_distbus_stream_t *)stream;
    distbus_stream->data_size = 0;
    if (distbus_stream->is_tx) {
        uint8_t *ptr;
        vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(distbus_stream->distbus, sizeof(uint32_t), &ptr);
        VSF_SERVICE_ASSERT(msg != NULL);
        put_unaligned_le32(distbus_stream->buf_size, ptr);

        vsf_slist_queue_init(&distbus_stream->msgq);
        msg->header.addr = VSF_DISTBUS_STREAM_CMD_BUF_SIZE;
        vsf_distbus_send_msg(distbus_stream->distbus, &distbus_stream->use_as__vsf_distbus_service_t, msg);

        vsf_stream_connect_tx(&distbus_stream->use_as__vsf_stream_t);
    } else {
        distbus_stream->msg = NULL;
    }
}

static uint_fast32_t __vsf_distbus_stream_get_buff_length(vsf_stream_t *stream)
{
    vsf_distbus_stream_t *distbus_stream = (vsf_distbus_stream_t *)stream;
    return distbus_stream->buf_size ? distbus_stream->buf_size : distbus_stream->mtu;
}

static uint_fast32_t __vsf_distbus_stream_get_data_length(vsf_stream_t *stream)
{
    vsf_distbus_stream_t *distbus_stream = (vsf_distbus_stream_t *)stream;
    uint_fast32_t data_size;

    vsf_protect_t orig = vsf_protect_int();
        data_size = distbus_stream->data_size;
    vsf_unprotect_int(orig);
    return data_size;
}

static uint_fast32_t __vsf_distbus_stream_get_avail_length(vsf_stream_t *stream)
{
    vsf_distbus_stream_t *distbus_stream = (vsf_distbus_stream_t *)stream;
    return distbus_stream->buf_size ?
                distbus_stream->buf_size - __vsf_distbus_stream_get_data_length(stream)
            :   distbus_stream->mtu;
}

static uint_fast32_t __vsf_distbus_stream_get_wbuf(vsf_stream_t *stream, uint8_t **ptr)
{
    vsf_distbus_stream_t *distbus_stream = (vsf_distbus_stream_t *)stream;
    VSF_SERVICE_ASSERT(!distbus_stream->is_tx);
    uint_fast32_t avail_length = __vsf_distbus_stream_get_avail_length(stream);

    if (avail_length > 0) {
        VSF_SERVICE_ASSERT(NULL == distbus_stream->msg);
        avail_length = vsf_min(avail_length, distbus_stream->mtu);
        distbus_stream->msg = vsf_distbus_alloc_msg(distbus_stream->distbus, avail_length, ptr);
        return (NULL == distbus_stream->msg) ? 0 : avail_length;
    }
    return avail_length;
}

static uint_fast32_t __vsf_distbus_stream_get_rbuf(vsf_stream_t *stream, uint8_t **ptr)
{
    vsf_distbus_stream_t *distbus_stream = (vsf_distbus_stream_t *)stream;
    VSF_SERVICE_ASSERT(distbus_stream->is_tx);

    if (vsf_slist_queue_is_empty(&distbus_stream->msgq)) {
        return 0;
    }

    vsf_distbus_msg_t *msg;
    vsf_protect_t orig = vsf_protect_int();
        vsf_slist_queue_peek(vsf_distbus_msg_t, node, &distbus_stream->msgq, msg);
    vsf_unprotect_int(orig);
    if (ptr != NULL) {
        *ptr = (uint8_t *)&msg->header + msg->pos;
    }
    return msg->header.datalen - (msg->pos - sizeof(msg->header));
}

static uint_fast32_t __vsf_distbus_stream_write(vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size)
{
    vsf_distbus_stream_t *distbus_stream = (vsf_distbus_stream_t *)stream;
    vsf_distbus_msg_t *msg;

    VSF_SERVICE_ASSERT(!distbus_stream->is_tx);
    if (__vsf_distbus_stream_get_avail_length(stream) < size) {
        if (distbus_stream->msg != NULL) {
            vsf_distbus_free_msg(distbus_stream->distbus, distbus_stream->msg);
            distbus_stream->msg = NULL;
        }
        return 0;
    }

    if (buf != NULL) {
        uint8_t *ptr;
        msg = vsf_distbus_alloc_msg(distbus_stream->distbus, size, &ptr);
        if (msg != NULL) {
            memcpy(ptr, buf, size);
        }
    } else {
        msg = distbus_stream->msg;
        distbus_stream->msg = NULL;
    }
    if (NULL == msg) {
        return 0;
    }

    vsf_protect_t orig = vsf_protect_int();
        distbus_stream->data_size += size;
    vsf_unprotect_int(orig);
    msg->header.datalen = size;
    msg->header.addr = VSF_DISTBUS_STREAM_CMD_DATA;
    vsf_distbus_send_msg(distbus_stream->distbus, &distbus_stream->use_as__vsf_distbus_service_t, msg);
    return size;
}

static uint_fast32_t __vsf_distbus_stream_read(vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size)
{
    vsf_distbus_stream_t *distbus_stream = (vsf_distbus_stream_t *)stream;
    uint_fast32_t realsize = 0, cursize;
    vsf_distbus_msg_t *msg;
    vsf_protect_t orig;

    VSF_SERVICE_ASSERT(distbus_stream->is_tx);
    if (buf != NULL) {
        uint8_t *ptr;

        while (realsize < size) {
            cursize = __vsf_distbus_stream_get_rbuf(stream, &ptr);
            if (0 == cursize) {
                break;
            }

            cursize = vsf_min(cursize, size - realsize);
            memcpy(buf, ptr, cursize);
            realsize += cursize;
            __vsf_distbus_stream_read(stream, NULL, cursize);
        }
    } else {
        uint_fast32_t remainsize = size;
        while (remainsize > 0) {
            orig = vsf_protect_int();
                vsf_slist_queue_peek(vsf_distbus_msg_t, node, &distbus_stream->msgq, msg);
            vsf_unprotect_int(orig);
            if (NULL == msg) {
                break;
            }

            cursize = vsf_min(remainsize, msg->header.datalen);
            remainsize -= cursize;
            msg->pos += cursize;
            realsize += cursize;
            orig = vsf_protect_int();
                distbus_stream->data_size -= size;
            vsf_unprotect_int(orig);

            if (msg->pos == sizeof(msg->header) + msg->header.datalen) {
                orig = vsf_protect_int();
                    vsf_slist_queue_dequeue(vsf_distbus_msg_t, node, &distbus_stream->msgq, msg);
                vsf_unprotect_int(orig);

                vsf_distbus_free_msg(distbus_stream->distbus, msg);
            }
        }

        uint8_t *ptr;
        vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(distbus_stream->distbus, sizeof(uint32_t), &ptr);
        VSF_SERVICE_ASSERT(msg != NULL);
        put_unaligned_le32(realsize, ptr);
        msg->header.addr = VSF_DISTBUS_STREAM_CMD_SIZE;
        vsf_distbus_send_msg(distbus_stream->distbus, &distbus_stream->use_as__vsf_distbus_service_t, msg);
    }
    return realsize;
}

void vsf_distbus_stream_register_service(vsf_distbus_stream_t *distbus_stream)
{
    if (NULL == distbus_stream->handler) {
        distbus_stream->handler = __vsf_distbus_stream_msghandler;
    }
    vsf_distbus_register_service(distbus_stream->distbus, &distbus_stream->use_as__vsf_distbus_service_t);
}

#endif      // VSF_USE_SIMPLE_STREAM && VSF_USE_DISTBUS
