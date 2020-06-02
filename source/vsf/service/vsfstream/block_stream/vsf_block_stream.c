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

#if VSF_USE_SERVICE_VSFSTREAM == ENABLED

#define VSFSTREAM_BLOCK_CLASS_IMPLEMENT
#include "../vsfstream.h"
#include "./vsf_block_stream.h"
#include "kernel/vsf_kernel.h"

/*============================ MACROS ========================================*/

#ifndef VSF_BLOCK_STREAM_CFG_PROTECT_LEVEL
/*! \note   By default, the driver tries to make all APIs interrupt-safe,
 *!
 *!         in the case when you want to disable it,
 *!         please use following macro:
 *!         #define VSF_BLOCK_STREAM_CFG_PROTECT_LEVEL      none
 *!         
 *!         in the case when you want to use scheduler-safe,
 *!         please use following macro:
 *!         #define VSF_BLOCK_STREAM_CFG_PROTECT_LEVEL      scheduler
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

static void __vsf_block_stream_init(vsf_stream_t *stream)
{
    vsf_block_stream_t *block_stream = (vsf_block_stream_t *)stream;
    uint_fast8_t block_num = block_stream->nSize / block_stream->block_size;
    VSF_SERVICE_ASSERT(block_stream->nSize >= block_num * (sizeof(*block_stream->block_size_arr) + block_stream->block_size));

    block_stream->state = 0;
    block_stream->block_size_arr = (uint32_t *)block_stream->pchBuffer;
    block_stream->pchBuffer = (uint8_t *)&block_stream->block_size_arr[block_num];
    block_stream->nSize -= block_num * 4;
}

static uint_fast32_t __vsf_block_stream_get_buff_length(vsf_stream_t *stream)
{
    vsf_block_stream_t *block_stream = (vsf_block_stream_t *)stream;
    return block_stream->nSize;
}

static uint_fast32_t __vsf_block_stream_get_data_length(vsf_stream_t *stream)
{
    vsf_block_stream_t *block_stream = (vsf_block_stream_t *)stream;
    uint_fast32_t data_size;

    vsf_protect_t orig = __vsf_block_stream_protect();
        data_size = (block_stream->rblock_num > 0) ? block_stream->block_size_arr[block_stream->rblock_idx] : 0;
    __vsf_block_stream_unprotect(orig);
    return data_size;
}

static uint_fast32_t __vsf_block_stream_get_avail_length(vsf_stream_t *stream)
{
    vsf_block_stream_t *block_stream = (vsf_block_stream_t *)stream;
    uint_fast32_t data_size;
    uint_fast8_t block_num = block_stream->nSize / block_stream->block_size;

    vsf_protect_t orig = __vsf_block_stream_protect();
        data_size = (block_stream->rblock_num < block_num) ? block_stream->block_size : 0;
    __vsf_block_stream_unprotect(orig);
    return data_size;
}

static uint_fast32_t __vsf_block_stream_get_wbuf(vsf_stream_t *stream, uint8_t **ptr)
{
    vsf_block_stream_t *block_stream = (vsf_block_stream_t *)stream;
    uint_fast8_t block_num = block_stream->nSize / block_stream->block_size;
    uint8_t *buffer;

    vsf_protect_t orig = __vsf_block_stream_protect();
    if (block_stream->rblock_num < block_num) {
        buffer = &block_stream->pchBuffer[block_stream->block_size * block_stream->wblock_idx];
        __vsf_block_stream_unprotect(orig);
        if (ptr != NULL) {
            *ptr = buffer;
        }
        return block_stream->block_size;
    }
    __vsf_block_stream_unprotect(orig);
    return 0;
}

static uint_fast32_t __vsf_block_stream_get_rbuf(vsf_stream_t *stream, uint8_t **ptr)
{
    vsf_block_stream_t *block_stream = (vsf_block_stream_t *)stream;
    uint_fast32_t size = 0;
    uint8_t *buffer;

    vsf_protect_t orig = __vsf_block_stream_protect();
    if (block_stream->rblock_num > 0) {
        buffer = &block_stream->pchBuffer[block_stream->block_size * block_stream->rblock_idx];
        size = block_stream->block_size_arr[block_stream->rblock_idx];
        __vsf_block_stream_unprotect(orig);
        if (ptr != NULL) {
            *ptr = buffer;
        }
        return size;
    }
    __vsf_block_stream_unprotect(orig);
    return 0;
}

static uint_fast32_t __vsf_block_stream_write(vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size)
{
    vsf_block_stream_t *block_stream = (vsf_block_stream_t *)stream;
    uint_fast8_t block_num = block_stream->nSize / block_stream->block_size;

    vsf_protect_t orig = __vsf_block_stream_protect();
        VSF_SERVICE_ASSERT(block_num > block_stream->rblock_num);
        VSF_SERVICE_ASSERT(!buf || (buf == &block_stream->pchBuffer[block_stream->wblock_idx * block_stream->block_size]));
        VSF_SERVICE_ASSERT(size <= block_stream->block_size);
        block_stream->block_size_arr[block_stream->wblock_idx] = size;
        if (++block_stream->wblock_idx >= block_num) {
            block_stream->wblock_idx = 0;
        }
        block_stream->rblock_num++;
    __vsf_block_stream_unprotect(orig);
    return size;
}

static uint_fast32_t __vsf_block_stream_read(vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size)
{
    vsf_block_stream_t *block_stream = (vsf_block_stream_t *)stream;
    uint_fast8_t block_num = block_stream->nSize / block_stream->block_size;

    vsf_protect_t orig = __vsf_block_stream_protect();
        VSF_SERVICE_ASSERT(block_stream->rblock_num > 0);
        VSF_SERVICE_ASSERT(!buf || (buf == &block_stream->pchBuffer[block_stream->rblock_idx * block_stream->block_size]));
        VSF_SERVICE_ASSERT(size <= block_stream->block_size_arr[block_stream->rblock_idx]);
        size = block_stream->block_size_arr[block_stream->rblock_idx];
        if (++block_stream->rblock_idx >= block_num) {
            block_stream->rblock_idx = 0;
        }
        block_stream->rblock_num--;
    __vsf_block_stream_unprotect(orig);
    return size;
}

#endif      // VSF_USE_SERVICE_VSFSTREAM
