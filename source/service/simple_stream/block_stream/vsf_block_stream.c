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

#if VSF_USE_SIMPLE_STREAM == ENABLED && VSF_USE_FIFO == ENABLED

#define __VSF_SIMPLE_STREAM_CLASS_INHERIT__
#define __VSF_BLOCK_STREAM_CLASS_IMPLEMENT
#include "../vsf_simple_stream.h"
#include "./vsf_block_stream.h"
#include "kernel/vsf_kernel.h"

/*============================ MACROS ========================================*/

#ifndef VSF_BLOCK_STREAM_CFG_PROTECT_LEVEL
/*! \note   By default, the driver tries to make all APIs interrupt-safe,
 *!
 *!         in the case when you want to disable it,
 *!         please use following macro:
 *!         #define VSF_BLOCK_STREAM_CFG_PROTECT_LEVEL  none
 *!
 *!         in the case when you want to use scheduler-safe,
 *!         please use following macro:
 *!         #define VSF_BLOCK_STREAM_CFG_PROTECT_LEVEL  scheduler
 *!
 *!         NOTE: This macro should be defined in vsf_usr_cfg.h
 */
#   define VSF_BLOCK_STREAM_CFG_PROTECT_LEVEL   interrupt
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define __vsf_block_stream_protect              vsf_protect(VSF_BLOCK_STREAM_CFG_PROTECT_LEVEL)
#define __vsf_block_stream_unprotect            vsf_unprotect(VSF_BLOCK_STREAM_CFG_PROTECT_LEVEL)

/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

static void __vsf_block_stream_init(vsf_stream_t *stream);
static uint_fast32_t __vsf_block_stream_write(vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size);
static uint_fast32_t __vsf_block_stream_read(vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size);
static uint_fast32_t __vsf_block_stream_get_buff_length(vsf_stream_t *stream);
static uint_fast32_t __vsf_block_stream_get_data_length(vsf_stream_t *stream);
static uint_fast32_t __vsf_block_stream_get_avail_length(vsf_stream_t *stream);
static uint_fast32_t __vsf_block_stream_get_wbuf(vsf_stream_t *stream, uint8_t **ptr);
static uint_fast32_t __vsf_block_stream_get_rbuf(vsf_stream_t *stream, uint8_t **ptr);

/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/

const vsf_stream_op_t vsf_block_stream_op = {
    .init               = __vsf_block_stream_init,
    .fini               = __vsf_block_stream_init,
    .write              = __vsf_block_stream_write,
    .read               = __vsf_block_stream_read,
    .get_buff_length    = __vsf_block_stream_get_buff_length,
    .get_data_length    = __vsf_block_stream_get_data_length,
    .get_avail_length   = __vsf_block_stream_get_avail_length,
    .get_wbuf           = __vsf_block_stream_get_wbuf,
    .get_rbuf           = __vsf_block_stream_get_rbuf,
};

/*============================ IMPLEMENTATION ================================*/

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#endif

static void __vsf_block_stream_init(vsf_stream_t *stream)
{
    __vsf_block_stream_t *block_stream = (__vsf_block_stream_t *)stream;
    block_stream->data_size = 0;
    vsf_fifo_init((vsf_fifo_t *)&block_stream->__fifo, block_stream->block_num);
}

static uint_fast32_t __vsf_block_stream_get_buff_length(vsf_stream_t *stream)
{
    __vsf_block_stream_t *block_stream = (__vsf_block_stream_t *)stream;
    return block_stream->block_num * block_stream->block_size;
}

static uint_fast32_t __vsf_block_stream_get_data_length(vsf_stream_t *stream)
{
    __vsf_block_stream_t *block_stream = (__vsf_block_stream_t *)stream;
    return block_stream->data_size;
}

static uint_fast32_t __vsf_block_stream_get_avail_length(vsf_stream_t *stream)
{
    __vsf_block_stream_t *block_stream = (__vsf_block_stream_t *)stream;
    return (block_stream->block_num - vsf_fifo_get_number((vsf_fifo_t *)&block_stream->__fifo)) * block_stream->block_size;
}

static uint_fast32_t __vsf_block_stream_get_wbuf(vsf_stream_t *stream, uint8_t **ptr)
{
    __vsf_block_stream_t *block_stream = (__vsf_block_stream_t *)stream;
    vsf_block_stream_item_t *item = (vsf_block_stream_item_t *)vsf_fifo_get_head((vsf_fifo_t *)&block_stream->__fifo,
                        block_stream->block_size + sizeof(vsf_block_stream_item_t));

    if (item != NULL) {
        if (ptr != NULL) {
            *ptr = (uint8_t *)&item[1];
        }
        return block_stream->block_size;
    }
    return 0;
}

static uint_fast32_t __vsf_block_stream_get_rbuf(vsf_stream_t *stream, uint8_t **ptr)
{
    __vsf_block_stream_t *block_stream = (__vsf_block_stream_t *)stream;
    vsf_block_stream_item_t *item = (vsf_block_stream_item_t *)vsf_fifo_get_tail((vsf_fifo_t *)&block_stream->__fifo,
                        block_stream->block_size + sizeof(vsf_block_stream_item_t));

    if (item != NULL) {
        if (ptr != NULL) {
            *ptr = (uint8_t *)&item[1] + item->pos;
        }
        return item->size - item->pos;
    }
    return 0;
}

static uint_fast32_t __vsf_block_stream_write(vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size)
{
    __vsf_block_stream_t *block_stream = (__vsf_block_stream_t *)stream;
    vsf_block_stream_item_t *item = (vsf_block_stream_item_t *)vsf_fifo_get_head((vsf_fifo_t *)&block_stream->__fifo,
                        block_stream->block_size + sizeof(vsf_block_stream_item_t));

    VSF_SERVICE_ASSERT(size <= block_stream->block_size);
    if (item != NULL) {
        if (buf != NULL) {
            memcpy(&item[1], buf, size);
        }
        item->size = size;
        item->pos = 0;
        vsf_fifo_push((vsf_fifo_t *)&block_stream->__fifo, (uintptr_t)NULL, block_stream->block_size + sizeof(*item));

        vsf_protect_t orig = __vsf_block_stream_protect();
        block_stream->data_size += size;
        __vsf_block_stream_unprotect(orig);
        return size;
    }
    return 0;
}

static uint_fast32_t __vsf_block_stream_read(vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size)
{
    __vsf_block_stream_t *block_stream = (__vsf_block_stream_t *)stream;
    vsf_block_stream_item_t *item = (vsf_block_stream_item_t *)vsf_fifo_get_tail((vsf_fifo_t *)&block_stream->__fifo,
                        block_stream->block_size + sizeof(vsf_block_stream_item_t));

    VSF_SERVICE_ASSERT(size <= block_stream->block_size);
    if (item != NULL) {
        size = vsf_min(size, item->size - item->pos);
        if (buf != NULL) {
            memcpy(buf, (char *)&item[1] + item->pos, size);
        }

        item->pos += size;
        VSF_SERVICE_ASSERT(item->pos <= item->size);
        if (item->pos == item->size) {
            vsf_fifo_pop((vsf_fifo_t *)&block_stream->__fifo, (uintptr_t)NULL, block_stream->block_size + sizeof(*item));
        }

        vsf_protect_t orig = __vsf_block_stream_protect();
        block_stream->data_size -= size;
        __vsf_block_stream_unprotect(orig);
        return size;
    }
    return 0;
}

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic pop
#endif

#endif      // VSF_USE_SIMPLE_STREAM
