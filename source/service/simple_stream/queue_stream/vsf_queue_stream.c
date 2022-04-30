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

#if VSF_USE_SIMPLE_STREAM == ENABLED

#define __VSF_SIMPLE_STREAM_CLASS_INHERIT__
#define __VSF_QUEUE_STREAM_CLASS_IMPLEMENT
#include "../vsf_simple_stream.h"
#include "./vsf_queue_stream.h"
#include "kernel/vsf_kernel.h"

/*============================ MACROS ========================================*/

// \note    Queue_stream can use be used in interrupt, because heap is used for buffer allocation.
#define __VSF_QUEUE_STREAM_CFG_PROTECT_LEVEL    scheduler

/*============================ MACROFIED FUNCTIONS ===========================*/

#define __vsf_queue_stream_protect              vsf_protect(__VSF_QUEUE_STREAM_CFG_PROTECT_LEVEL)
#define __vsf_queue_stream_unprotect            vsf_unprotect(__VSF_QUEUE_STREAM_CFG_PROTECT_LEVEL)

/*============================ TYPES =========================================*/

typedef struct vsf_queue_stream_buffer_node_t {
    vsf_slist_node_t buffer_node;
    uint32_t size;
    uint32_t pos;
} vsf_queue_stream_buffer_node_t;

/*============================ PROTOTYPES ====================================*/

static void __vsf_queue_stream_init(vsf_stream_t *stream);
static uint_fast32_t __vsf_queue_stream_write(vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size);
static uint_fast32_t __vsf_queue_stream_read(vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size);
static uint_fast32_t __vsf_queue_stream_get_buff_length(vsf_stream_t *stream);
static uint_fast32_t __vsf_queue_stream_get_data_length(vsf_stream_t *stream);
static uint_fast32_t __vsf_queue_stream_get_avail_length(vsf_stream_t *stream);
static uint_fast32_t __vsf_queue_stream_get_rbuf(vsf_stream_t *stream, uint8_t **ptr);

/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/

const vsf_stream_op_t vsf_queue_stream_op = {
    .init               = __vsf_queue_stream_init,
    .fini               = __vsf_queue_stream_init,
    .write              = __vsf_queue_stream_write,
    .read               = __vsf_queue_stream_read,
    .get_buff_length    = __vsf_queue_stream_get_buff_length,
    .get_data_length    = __vsf_queue_stream_get_data_length,
    .get_avail_length   = __vsf_queue_stream_get_avail_length,
    .get_wbuf           = NULL,
    .get_rbuf           = __vsf_queue_stream_get_rbuf,
};

/*============================ IMPLEMENTATION ================================*/

static void __vsf_queue_stream_init(vsf_stream_t *stream)
{
    vsf_queue_stream_t *queue_stream = (vsf_queue_stream_t *)stream;
    VSF_SERVICE_ASSERT(queue_stream->max_buffer_size != 0);
    VSF_SERVICE_ASSERT(queue_stream->max_entry_num != 0);
    vsf_slist_queue_init(&queue_stream->buffer_queue);
    queue_stream->size = 0;
    queue_stream->entry_num = 0;
}

static uint_fast32_t __vsf_queue_stream_get_buff_length(vsf_stream_t *stream)
{
    vsf_queue_stream_t *queue_stream = (vsf_queue_stream_t *)stream;
    return queue_stream->max_buffer_size >= 0 ? queue_stream->max_buffer_size : 0xFFFFFFFF;
}

static uint_fast32_t __vsf_queue_stream_get_data_length(vsf_stream_t *stream)
{
    vsf_queue_stream_t *queue_stream = (vsf_queue_stream_t *)stream;
    uint_fast32_t data_size;

    vsf_protect_t orig = __vsf_queue_stream_protect();
        data_size = queue_stream->size;
    __vsf_queue_stream_unprotect(orig);
    return data_size;
}

static uint_fast32_t __vsf_queue_stream_get_avail_length(vsf_stream_t *stream)
{
    vsf_queue_stream_t *queue_stream = (vsf_queue_stream_t *)stream;
    uint_fast32_t data_size, entry_num;

    vsf_protect_t orig = __vsf_queue_stream_protect();
        data_size = queue_stream->size;
        entry_num = queue_stream->entry_num;
    __vsf_queue_stream_unprotect(orig);

    if (entry_num >= queue_stream->max_entry_num) {
        return 0;
    }
    if (queue_stream->max_buffer_size < 0) {
        return 0xFFFFFFFF - data_size;
    }
    return queue_stream->max_buffer_size - data_size;
}

static uint_fast32_t __vsf_queue_stream_get_rbuf(vsf_stream_t *stream, uint8_t **ptr)
{
    vsf_queue_stream_t *queue_stream = (vsf_queue_stream_t *)stream;
    vsf_queue_stream_buffer_node_t *buffer_node;
    vsf_slist_queue_peek(vsf_queue_stream_buffer_node_t, buffer_node, &queue_stream->buffer_queue, buffer_node);

    if (NULL == buffer_node) {
        return 0;
    }

    if (ptr != NULL) {
        *ptr = (uint8_t *)&buffer_node[1] + buffer_node->pos;
    }
    return buffer_node->size - buffer_node->pos;
}

static uint_fast32_t __vsf_queue_stream_write(vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size)
{
    vsf_queue_stream_t *queue_stream = (vsf_queue_stream_t *)stream;
    vsf_queue_stream_buffer_node_t *buffer_node;
    uint_fast32_t avail_len = __vsf_queue_stream_get_avail_length(stream);
    uint_fast32_t wsize = vsf_min(avail_len, size);

    buffer_node = vsf_heap_malloc(sizeof(*buffer_node) + size);
    if (NULL == buffer_node) {
        return 0;
    }
    vsf_slist_init_node(vsf_queue_stream_buffer_node_t, buffer_node, buffer_node);
    buffer_node->pos = 0;
    buffer_node->size = size;
    memcpy(&buffer_node[1], buf, size);

    vsf_protect_t orig = __vsf_queue_stream_protect();
        queue_stream->size += size;
        queue_stream->entry_num++;
        vsf_slist_queue_enqueue(vsf_queue_stream_buffer_node_t, buffer_node, &queue_stream->buffer_queue, buffer_node);
    __vsf_queue_stream_unprotect(orig);

    return wsize;
}

static uint_fast32_t __vsf_queue_stream_read(vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size)
{
    vsf_queue_stream_t *queue_stream = (vsf_queue_stream_t *)stream;
    vsf_queue_stream_buffer_node_t *buffer_node;
    uint_fast32_t rsize = 0, cur_size, remain_size = size;

    while (remain_size > 0) {
        vsf_slist_queue_peek(vsf_queue_stream_buffer_node_t, buffer_node, &queue_stream->buffer_queue, buffer_node);
        if (NULL == buffer_node) {
            break;
        }

        cur_size = buffer_node->size - buffer_node->pos;
        cur_size = vsf_min(cur_size, remain_size);
        if (buf != NULL) {
            memcpy(buf, (uint8_t *)&buffer_node[1] + buffer_node->pos, cur_size);
            buf += cur_size;
        }
        buffer_node->pos += cur_size;
        rsize += cur_size;
        remain_size -= cur_size;

        if (buffer_node->pos < buffer_node->size) {
            buffer_node = NULL;
        }

        vsf_protect_t orig = __vsf_queue_stream_protect();
            queue_stream->size -= cur_size;
            if (buffer_node != NULL) {
                queue_stream->entry_num--;
                vsf_slist_queue_dequeue(vsf_queue_stream_buffer_node_t, buffer_node, &queue_stream->buffer_queue, buffer_node);
            }
        __vsf_queue_stream_unprotect(orig);

        if (buffer_node != NULL) {
            vsf_heap_free(buffer_node);
        }
    }

    return rsize;
}

#endif      // VSF_USE_SIMPLE_STREAM
