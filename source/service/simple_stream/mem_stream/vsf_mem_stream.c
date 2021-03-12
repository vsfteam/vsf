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

#if VSF_USE_SIMPLE_STREAM == ENABLED

#define __VSF_SIMPLE_STREAM_CLASS_INHERIT__
#define __VSF_MEM_STREAM_CLASS_IMPLEMENT
#include "../vsf_simple_stream.h"
#include "./vsf_mem_stream.h"
#include "kernel/vsf_kernel.h"

/*============================ MACROS ========================================*/

#ifndef VSF_MEM_STREAM_CFG_PROTECT_LEVEL
/*! \note   By default, the driver tries to make all APIs interrupt-safe,
 *!
 *!         in the case when you want to disable it,
 *!         please use following macro:
 *!         #define VSF_MEM_STREAM_CFG_PROTECT_LEVEL    none
 *!         
 *!         in the case when you want to use scheduler-safe,
 *!         please use following macro:
 *!         #define VSF_MEM_STREAM_CFG_PROTECT_LEVEL    scheduler
 *!         
 *!         NOTE: This macro should be defined in vsf_usr_cfg.h
 */
#   define VSF_MEM_STREAM_CFG_PROTECT_LEVEL     interrupt
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define __vsf_mem_stream_protect                vsf_protect(VSF_MEM_STREAM_CFG_PROTECT_LEVEL)
#define __vsf_mem_stream_unprotect              vsf_unprotect(VSF_MEM_STREAM_CFG_PROTECT_LEVEL)

/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

static void __vsf_mem_stream_init(vsf_stream_t *stream);
static uint_fast32_t __vsf_mem_stream_write(vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size);
static uint_fast32_t __vsf_mem_stream_read(vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size);
static uint_fast32_t __vsf_mem_stream_get_buff_length(vsf_stream_t *stream);
static uint_fast32_t __vsf_mem_stream_get_data_length(vsf_stream_t *stream);
static uint_fast32_t __vsf_mem_stream_get_avail_length(vsf_stream_t *stream);
static uint_fast32_t __vsf_mem_stream_get_wbuf(vsf_stream_t *stream, uint8_t **ptr);
static uint_fast32_t __vsf_mem_stream_get_rbuf(vsf_stream_t *stream, uint8_t **ptr);

/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/

const vsf_stream_op_t vsf_mem_stream_op = {
    .init               = __vsf_mem_stream_init,
    .fini               = __vsf_mem_stream_init,
    .write              = __vsf_mem_stream_write,
    .read               = __vsf_mem_stream_read,
    .get_buff_length    = __vsf_mem_stream_get_buff_length,
    .get_data_length    = __vsf_mem_stream_get_data_length,
    .get_avail_length   = __vsf_mem_stream_get_avail_length,
    .get_wbuf           = __vsf_mem_stream_get_wbuf,
    .get_rbuf           = __vsf_mem_stream_get_rbuf,
};

/*============================ IMPLEMENTATION ================================*/

static void __vsf_mem_stream_init(vsf_stream_t *stream)
{
    vsf_mem_stream_t *mem_stream = (vsf_mem_stream_t *)stream;

    mem_stream->is_writing = false;
    mem_stream->rpos = mem_stream->wpos = 0;
    mem_stream->data_size = 0;
    if (!mem_stream->align) {
        mem_stream->align = 1;
    }
    VSF_SERVICE_ASSERT(!(mem_stream->align & (mem_stream->align - 1)));
    mem_stream->align -= 1;
}

static uint_fast32_t __vsf_mem_stream_get_buff_length(vsf_stream_t *stream)
{
    vsf_mem_stream_t *mem_stream = (vsf_mem_stream_t *)stream;
    return mem_stream->size;
}

static uint_fast32_t __vsf_mem_stream_get_data_length(vsf_stream_t *stream)
{
    vsf_mem_stream_t *mem_stream = (vsf_mem_stream_t *)stream;
    uint_fast32_t data_size;

    vsf_protect_t orig = __vsf_mem_stream_protect();
        data_size = mem_stream->data_size;
    __vsf_mem_stream_unprotect(orig);
    return data_size;
}

static uint_fast32_t __vsf_mem_stream_get_avail_length(vsf_stream_t *stream)
{
    vsf_mem_stream_t *mem_stream = (vsf_mem_stream_t *)stream;

    return (mem_stream->wpos & mem_stream->align) ? 0 :
        mem_stream->use_as__vsf_mem_t.size - __vsf_mem_stream_get_data_length(stream);
}

static uint_fast32_t __vsf_mem_stream_get_wbuf(vsf_stream_t *stream, uint8_t **ptr)
{
    vsf_mem_stream_t *mem_stream = (vsf_mem_stream_t *)stream;
    uint_fast32_t avail_len = __vsf_mem_stream_get_avail_length(stream);
    uint_fast32_t wlen = mem_stream->use_as__vsf_mem_t.size - mem_stream->wpos;
    uint8_t *p = (avail_len > 0) ?
            mem_stream->use_as__vsf_mem_t.buffer + mem_stream->wpos : NULL;

    if (ptr != NULL) {
        *ptr = p;
        mem_stream->is_writing = true;
    }
    return min(wlen, avail_len);
}

static uint_fast32_t __vsf_mem_stream_get_rbuf(vsf_stream_t *stream, uint8_t **ptr)
{
    vsf_mem_stream_t *mem_stream = (vsf_mem_stream_t *)stream;
    uint_fast32_t data_len = __vsf_mem_stream_get_data_length(stream);
    uint_fast32_t rlen = mem_stream->use_as__vsf_mem_t.size - mem_stream->rpos;
    uint8_t *p = (data_len > 0) ?
            mem_stream->use_as__vsf_mem_t.buffer + mem_stream->rpos : NULL;

    if (ptr != NULL) {
        *ptr = p;
    }
    return min(rlen, data_len);
}

static uint_fast32_t __vsf_mem_stream_write(vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size)
{
    vsf_mem_stream_t *mem_stream = (vsf_mem_stream_t *)stream;
    uint_fast32_t avail_len = __vsf_mem_stream_get_avail_length(stream);
    uint_fast32_t wsize = min(avail_len, size);

    VSF_SERVICE_ASSERT(!(mem_stream->wpos & mem_stream->align));
    mem_stream->is_writing = true;
    if ((buf != NULL) && (buf != &mem_stream->use_as__vsf_mem_t.buffer[mem_stream->wpos])) {
        uint_fast32_t totalsize = wsize;
        uint_fast32_t curlen = mem_stream->use_as__vsf_mem_t.size - mem_stream->wpos;

        curlen = min(totalsize, curlen);
        memcpy(mem_stream->use_as__vsf_mem_t.buffer + mem_stream->wpos, buf, curlen);
        totalsize -= curlen;
        buf += curlen;
        if (totalsize > 0) {
            memcpy(mem_stream->use_as__vsf_mem_t.buffer, buf, totalsize);
        }
    }

    vsf_protect_t orig = __vsf_mem_stream_protect();
        mem_stream->data_size += wsize;
        mem_stream->wpos += wsize;
        mem_stream->wpos %= mem_stream->use_as__vsf_mem_t.size;
    __vsf_mem_stream_unprotect(orig);
    mem_stream->is_writing = false;
    return wsize;
}

static uint_fast32_t __vsf_mem_stream_read(vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size)
{
    vsf_mem_stream_t *mem_stream = (vsf_mem_stream_t *)stream;
    uint_fast32_t data_len = __vsf_mem_stream_get_data_length(stream);
    uint_fast32_t rsize = min(data_len, size);

    VSF_SERVICE_ASSERT(!(mem_stream->rpos & mem_stream->align));
    if (size < data_len) {
        rsize &= ~mem_stream->align;
    }
    if ((buf != NULL) && (buf != &mem_stream->use_as__vsf_mem_t.buffer[mem_stream->rpos])) {
        uint_fast32_t totalsize = rsize;
        uint_fast32_t curlen = mem_stream->use_as__vsf_mem_t.size - mem_stream->rpos;

        curlen = min(totalsize, curlen);
        memcpy(buf, mem_stream->use_as__vsf_mem_t.buffer + mem_stream->rpos, curlen);
        totalsize -= curlen;
        buf += curlen;
        if (totalsize > 0) {
            memcpy(buf, mem_stream->use_as__vsf_mem_t.buffer, totalsize);
        }
    }

    vsf_protect_t orig = __vsf_mem_stream_protect();
        mem_stream->data_size -= rsize;
        if ((size < data_len) || mem_stream->is_writing) {
            mem_stream->rpos += rsize;
            mem_stream->rpos %= mem_stream->use_as__vsf_mem_t.size;
        } else {
            mem_stream->rpos = mem_stream->wpos = 0;
        }
    __vsf_mem_stream_unprotect(orig);
    return rsize;
}

#endif      // VSF_USE_SIMPLE_STREAM
