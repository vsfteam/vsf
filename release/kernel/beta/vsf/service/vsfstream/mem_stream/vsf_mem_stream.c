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

#define VSFSTREAM_MEM_CLASS_IMPLEMENT
#include "../vsfstream.h"
#include "./vsf_mem_stream.h"
#include "kernel/vsf_kernel.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

static void vsf_mem_stream_init(vsf_stream_t *stream);
static uint_fast32_t vsf_mem_stream_write(vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size);
static uint_fast32_t vsf_mem_stream_read(vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size);
static uint_fast32_t vsf_mem_stream_get_data_length(vsf_stream_t *stream);
static uint_fast32_t vsf_mem_stream_get_avail_length(vsf_stream_t *stream);
static uint_fast32_t vsf_mem_stream_get_wbuf(vsf_stream_t *stream, uint8_t **ptr);
static uint_fast32_t vsf_mem_stream_get_rbuf(vsf_stream_t *stream, uint8_t **ptr);

/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/

const vsf_stream_op_t vsf_mem_stream_op = {
    .init               = vsf_mem_stream_init,
    .fini               = vsf_mem_stream_init,
    .write              = vsf_mem_stream_write,
    .read               = vsf_mem_stream_read,
    .get_data_length    = vsf_mem_stream_get_data_length,
    .get_avail_length   = vsf_mem_stream_get_avail_length,
    .get_wbuf           = vsf_mem_stream_get_wbuf,
    .get_rbuf           = vsf_mem_stream_get_rbuf,
};

/*============================ IMPLEMENTATION ================================*/

static void vsf_mem_stream_init(vsf_stream_t *stream)
{
    vsf_mem_stream_t *mem_stream = (vsf_mem_stream_t *)stream;
    mem_stream->rpos = mem_stream->wpos = 0;
    mem_stream->data_size = 0;
    mem_stream->is_writing = false;
    if (!mem_stream->align) {
        mem_stream->align = 1;
    }
    VSF_SERVICE_ASSERT(!(mem_stream->align & (mem_stream->align - 1)));
    mem_stream->align -= 1;
}

static uint_fast32_t vsf_mem_stream_get_data_length(vsf_stream_t *stream)
{
    vsf_mem_stream_t *mem_stream = (vsf_mem_stream_t *)stream;
    uint_fast32_t data_size;

    vsf_protect_t orig = vsf_protect_sched();
        data_size = mem_stream->data_size;
    vsf_unprotect_sched(orig);
    return data_size;
}

static uint_fast32_t vsf_mem_stream_get_avail_length(vsf_stream_t *stream)
{
    vsf_mem_stream_t *mem_stream = (vsf_mem_stream_t *)stream;
    if (mem_stream->wpos & mem_stream->align) {
        return 0;
    } else {
        return mem_stream->use_as__vsf_mem_t.s32_size - vsf_mem_stream_get_data_length(stream);
    }
}

static uint_fast32_t vsf_mem_stream_get_wbuf(vsf_stream_t *stream, uint8_t **ptr)
{
    vsf_mem_stream_t *mem_stream = (vsf_mem_stream_t *)stream;
    uint_fast32_t avail_len = vsf_mem_stream_get_avail_length(stream);
    uint_fast32_t wlen = mem_stream->use_as__vsf_mem_t.s32_size - mem_stream->wpos;
    uint8_t *p = (avail_len > 0) ?
        mem_stream->use_as__vsf_mem_t.buffer_ptr + mem_stream->wpos : NULL;

    if (ptr != NULL) {
        *ptr = p;
        mem_stream->is_writing = true;
    }
    return min(wlen, avail_len);
}

static uint_fast32_t vsf_mem_stream_get_rbuf(vsf_stream_t *stream, uint8_t **ptr)
{
    vsf_mem_stream_t *mem_stream = (vsf_mem_stream_t *)stream;
    uint_fast32_t data_len = vsf_mem_stream_get_data_length(stream);
    uint_fast32_t rlen = mem_stream->use_as__vsf_mem_t.s32_size - mem_stream->rpos;
    uint8_t *p = (data_len > 0) ?
        mem_stream->use_as__vsf_mem_t.buffer_ptr + mem_stream->rpos : NULL;

    if (ptr != NULL) {
        *ptr = p;
    }
    return min(rlen, data_len);
}

static uint_fast32_t vsf_mem_stream_write(vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size)
{
    vsf_mem_stream_t *mem_stream = (vsf_mem_stream_t *)stream;
    uint_fast32_t avail_len = vsf_mem_stream_get_avail_length(stream);
    uint_fast32_t wsize = min(avail_len, size);

    VSF_SERVICE_ASSERT(!(mem_stream->wpos & mem_stream->align));
    if ((buf != NULL) && (buf != &mem_stream->use_as__vsf_mem_t.buffer_ptr[mem_stream->wpos])) {
        memcpy(mem_stream->use_as__vsf_mem_t.buffer_ptr + mem_stream->wpos, buf, wsize);
    }

    vsf_protect_t orig = vsf_protect_sched();
        mem_stream->data_size += wsize;
    vsf_unprotect_sched(orig);

    mem_stream->wpos += wsize;
    if (mem_stream->wpos >= mem_stream->use_as__vsf_mem_t.s32_size) {
        mem_stream->wpos = 0;
    }
    mem_stream->is_writing = false;
    return wsize;
}

static uint_fast32_t vsf_mem_stream_read(vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size)
{
    vsf_mem_stream_t *mem_stream = (vsf_mem_stream_t *)stream;
    uint_fast32_t data_len = vsf_mem_stream_get_data_length(stream);
    uint_fast32_t rsize = min(data_len, size);

    VSF_SERVICE_ASSERT(!(mem_stream->rpos & mem_stream->align));
    if (size < data_len) {
        rsize &= ~mem_stream->align;
    }
    if ((buf != NULL) && (buf != &mem_stream->use_as__vsf_mem_t.buffer_ptr[mem_stream->rpos])) {
        memcpy(buf, mem_stream->use_as__vsf_mem_t.buffer_ptr + mem_stream->rpos, rsize);
    }

    vsf_protect_t orig = vsf_protect_sched();
        mem_stream->data_size -= rsize;
    vsf_unprotect_sched(orig);

    if ((size < data_len) || mem_stream->is_writing) {
        mem_stream->rpos += rsize;
    } else {
        mem_stream->rpos = mem_stream->wpos = 0;
    }
    return rsize;
}

#endif      // VSF_USE_SERVICE_VSFSTREAM
