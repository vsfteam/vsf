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

#include "component/mal/vsf_mal_cfg.h"

#if VSF_USE_MAL == ENABLED

#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
#   define VSFSTREAM_CLASS_INHERIT
#endif
#define VSF_MAL_IMPLEMENT

// TODO: use dedicated include
#include "vsf.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum {
    VSF_EVT_MAL_READ        = VSF_EVT_USER + 0,
    VSF_EVT_MAL_WRITE       = VSF_EVT_USER + 1,
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_mal_init(vsf_mal_t *pthis)
{
    VSF_MAL_ASSERT((pthis != NULL) && (pthis->drv != NULL) && (pthis->drv->init != NULL));
    return vsf_eda_call_param_eda(pthis->drv->init, pthis);
}

vsf_err_t vsf_mal_fini(vsf_mal_t *pthis)
{
    VSF_MAL_ASSERT((pthis != NULL) && (pthis->drv != NULL) && (pthis->drv->fini != NULL));
    return vsf_eda_call_param_eda(pthis->drv->fini, pthis);
}

uint_fast32_t vsf_mal_blksz(vsf_mal_t *pthis, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op)
{
    VSF_MAL_ASSERT((pthis != NULL) && (pthis->drv != NULL) && (pthis->drv->blksz != NULL));
    return pthis->drv->blksz(pthis, addr, size, op);
}

bool vsf_mal_prepare_buffer(vsf_mal_t *pthis, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op, vsf_mem_t *mem)
{
    VSF_MAL_ASSERT((pthis != NULL) && (pthis->drv != NULL));
    if (pthis->drv->buffer != NULL) {
        return pthis->drv->buffer(pthis, addr, size, op, mem);
    }
    return false;
}

// to erase all, size should be 0
vsf_err_t vsf_mal_erase(vsf_mal_t *pthis, uint_fast64_t addr, uint_fast32_t size)
{
    VSF_MAL_ASSERT((pthis != NULL) && (pthis->drv != NULL) && (pthis->drv->erase != NULL));
    pthis->args.addr = addr;
    pthis->args.size = size;
    return vsf_eda_call_param_eda(pthis->drv->erase, pthis);
}

vsf_err_t vsf_mal_read(vsf_mal_t *pthis, uint_fast64_t addr, uint_fast32_t size, uint8_t *buff)
{
    VSF_MAL_ASSERT((pthis != NULL) && (pthis->drv != NULL) && (pthis->drv->read != NULL));
    pthis->args.addr = addr;
    pthis->args.size = size;
    pthis->args.buff = buff;
    return vsf_eda_call_param_eda(pthis->drv->read, pthis);
}

vsf_err_t vsf_mal_write(vsf_mal_t *pthis, uint_fast64_t addr, uint_fast32_t size, uint8_t *buff)
{
    VSF_MAL_ASSERT((pthis != NULL) && (pthis->drv != NULL) && (pthis->drv->write != NULL));
    pthis->args.addr = addr;
    pthis->args.size = size;
    pthis->args.buff = buff;
    return vsf_eda_call_param_eda(pthis->drv->write, pthis);
}

vsf_err_t vsf_mal_get_result(vsf_mal_t *pthis, uint32_t *size)
{
    VSF_MAL_ASSERT(pthis != NULL);
    if (size != NULL) {
        *size = pthis->result.size;
    }
    return pthis->result.errcode;
}

#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
static void vsf_mal_stream_tx_evthandler(void *param, vsf_stream_evt_t evt)
{
    vsf_mal_t *pthis = (vsf_mal_t *)param;
    vsf_stream_t *stream = pthis->stream.stream;

    switch (evt) {
    case VSF_STREAM_ON_CONNECT:
    case VSF_STREAM_ON_OUT:
        if (pthis->stream.size > 0) {
            pthis->args.size = vsf_stream_get_wbuf(stream, &pthis->args.buff);
            if (pthis->args.size < vsf_mal_blksz(pthis, pthis->stream.addr, 0, VSF_MAL_OP_READ)) {
                break;
            }

            vsf_eda_post_evt(pthis->stream.cur_eda, VSF_EVT_MAL_READ);
        }
        break;
    }
}

static void vsf_mal_read_stream_do(uintptr_t target, vsf_evt_t evt)
{
    vsf_mal_t *pthis = (vsf_mal_t *)target;
    vsf_stream_t *stream = pthis->stream.stream;

    switch (evt) {
    case VSF_EVT_INIT:
        pthis->stream.rw_size = 0;
        pthis->stream.cur_eda = vsf_eda_get_cur();
        stream->tx.param = pthis;
        stream->tx.evthandler = vsf_mal_stream_tx_evthandler;
        vsf_stream_connect_tx(stream);
        break;
    case VSF_EVT_RETURN:
        if (VSF_ERR_NONE == pthis->result.errcode) {
            pthis->stream.size -= pthis->result.size;
            pthis->stream.addr += pthis->result.size;
            pthis->stream.rw_size += pthis->result.size;
            vsf_stream_write(stream, NULL, pthis->result.size);
        }
        if ((pthis->result.errcode != VSF_ERR_NONE) || !pthis->stream.size) {
            pthis->result.size = pthis->stream.rw_size;
            vsf_stream_disconnect_tx(stream);
            vsf_eda_return();
        }
        break;
    case VSF_EVT_MAL_READ:
        pthis->args.size = min(pthis->args.size, pthis->stream.size);
        vsf_mal_read(pthis, pthis->stream.addr, pthis->args.size, pthis->args.buff);
        break;
    }
}

vsf_err_t vsf_mal_read_stream(vsf_mal_t *pthis, uint_fast64_t addr, uint_fast32_t size, vsf_stream_t *stream)
{
    VSF_MAL_ASSERT(pthis != NULL);
    pthis->stream.addr = addr;
    pthis->stream.size = size;
    pthis->stream.stream = stream;
    return vsf_eda_call_param_eda(vsf_mal_read_stream_do, pthis);
}

static void vsf_mal_stream_rx_evthandler(void *param, vsf_stream_evt_t evt)
{
    vsf_mal_t *pthis = (vsf_mal_t *)param;
    vsf_stream_t *stream = pthis->stream.stream;

    switch (evt) {
    case VSF_STREAM_ON_CONNECT:
    case VSF_STREAM_ON_IN:
        if (pthis->stream.size > 0) {
            pthis->args.size = vsf_stream_get_rbuf(stream, &pthis->args.buff);
            if (pthis->args.size < vsf_mal_blksz(pthis, pthis->stream.addr, 0, VSF_MAL_OP_WRITE)) {
                break;
            }

            vsf_eda_post_evt(pthis->stream.cur_eda, VSF_EVT_MAL_WRITE);
        }
        break;
    }
}

static void vsf_mal_write_stream_do(uintptr_t target, vsf_evt_t evt)
{
    vsf_mal_t *pthis = (vsf_mal_t *)target;
    vsf_stream_t *stream = pthis->stream.stream;

    switch (evt) {
    case VSF_EVT_INIT:
        pthis->stream.rw_size = 0;
        pthis->stream.cur_eda = vsf_eda_get_cur();
        stream->rx.param = pthis;
        stream->rx.evthandler = vsf_mal_stream_rx_evthandler;
        vsf_stream_connect_rx(stream);
        break;
    case VSF_EVT_RETURN:
        if (VSF_ERR_NONE == pthis->result.errcode) {
            pthis->stream.size -= pthis->result.size;
            pthis->stream.addr += pthis->result.size;
            pthis->stream.rw_size += pthis->result.size;
            vsf_stream_read(stream, NULL, pthis->result.size);
        }
        if ((pthis->result.errcode != VSF_ERR_NONE) || !pthis->stream.size) {
            pthis->result.size = pthis->stream.rw_size;
            vsf_stream_disconnect_rx(stream);
            vsf_eda_return();
        }
        break;
    case VSF_EVT_MAL_WRITE:
        pthis->args.size = min(pthis->args.size, pthis->stream.size);
        vsf_mal_write(pthis, pthis->stream.addr, pthis->args.size, pthis->args.buff);
        break;
    }
}

vsf_err_t vsf_mal_write_stream(vsf_mal_t *pthis, uint_fast64_t addr, uint_fast32_t size, vsf_stream_t *stream)
{
    VSF_MAL_ASSERT(pthis != NULL);
    pthis->stream.addr = addr;
    pthis->stream.size = size;
    pthis->stream.stream = stream;
    return vsf_eda_call_param_eda(vsf_mal_write_stream_do, pthis);
}
#endif

#endif
