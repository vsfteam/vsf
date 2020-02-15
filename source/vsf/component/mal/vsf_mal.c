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

#include "./vsf_mal_cfg.h"

#if VSF_USE_MAL == ENABLED

#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
#   define VSFSTREAM_CLASS_INHERIT
#endif
#define VSF_MAL_IMPLEMENT

// TODO: use dedicated include
#include "vsf.h"

/*============================ MACROS ========================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL != ENABLED
#   error VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL is needed to use scsi
#endif

#if VSF_USE_KERNEL_SIMPLE_SHELL != ENABLED
#   error VSF_USE_KERNEL_SIMPLE_SHELL must be enabled
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
enum {
    VSF_EVT_MAL_READ        = VSF_EVT_USER + 0,
    VSF_EVT_MAL_WRITE       = VSF_EVT_USER + 1,
};
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vk_mal_init(vk_mal_t *pthis)
{
    VSF_MAL_ASSERT((pthis != NULL) && (pthis->drv != NULL) && (pthis->drv->init != NULL));
    return __vsf_call_eda((uintptr_t)pthis->drv->init, (uintptr_t)pthis);
}

vsf_err_t vk_mal_fini(vk_mal_t *pthis)
{
    VSF_MAL_ASSERT((pthis != NULL) && (pthis->drv != NULL) && (pthis->drv->fini != NULL));
    return __vsf_call_eda((uintptr_t)pthis->drv->fini, (uintptr_t)pthis);
}

uint_fast32_t vk_mal_blksz(vk_mal_t *pthis, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op)
{
    VSF_MAL_ASSERT((pthis != NULL) && (pthis->drv != NULL) && (pthis->drv->blksz != NULL));
    return pthis->drv->blksz(pthis, addr, size, op);
}

bool vk_mal_prepare_buffer(vk_mal_t *pthis, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op, vsf_mem_t *mem)
{
    VSF_MAL_ASSERT((pthis != NULL) && (pthis->drv != NULL));
    if (pthis->drv->buffer != NULL) {
        return pthis->drv->buffer(pthis, addr, size, op, mem);
    }
    return false;
}

// to erase all, size should be 0
vsf_err_t vk_mal_erase(vk_mal_t *pthis, uint_fast64_t addr, uint_fast32_t size)
{
    VSF_MAL_ASSERT((pthis != NULL) && (pthis->drv != NULL) && (pthis->drv->erase != NULL));
    pthis->args.addr = addr;
    pthis->args.size = size;
    return __vsf_call_eda((uintptr_t)pthis->drv->erase, (uintptr_t)pthis);
}

vsf_err_t vk_mal_read(vk_mal_t *pthis, uint_fast64_t addr, uint_fast32_t size, uint8_t *buff)
{
    VSF_MAL_ASSERT((pthis != NULL) && (pthis->drv != NULL) && (pthis->drv->read != NULL));
    pthis->args.addr = addr;
    pthis->args.size = size;
    pthis->args.buff = buff;
    return __vsf_call_eda((uintptr_t)pthis->drv->read, (uintptr_t)pthis);
}

vsf_err_t vk_mal_write(vk_mal_t *pthis, uint_fast64_t addr, uint_fast32_t size, uint8_t *buff)
{
    VSF_MAL_ASSERT((pthis != NULL) && (pthis->drv != NULL) && (pthis->drv->write != NULL));
    pthis->args.addr = addr;
    pthis->args.size = size;
    pthis->args.buff = buff;
    return __vsf_call_eda((uintptr_t)pthis->drv->write, (uintptr_t)pthis);
}

vsf_err_t vk_mal_get_result(vk_mal_t *pthis, uint32_t *size)
{
    VSF_MAL_ASSERT(pthis != NULL);
    if (size != NULL) {
        *size = pthis->result.size;
    }
    return pthis->result.errcode;
}

#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
static void __vk_mal_stream_tx_evthandler(void *param, vsf_stream_evt_t evt)
{
    vk_mal_stream_t *pthis = (vk_mal_stream_t *)param;
    vk_mal_t *mal = pthis->mal;
    vsf_stream_t *stream = pthis->stream.stream;

    switch (evt) {
    case VSF_STREAM_ON_CONNECT:
    case VSF_STREAM_ON_OUT:
        if (pthis->stream.size > 0) {
            mal->args.size = vsf_stream_get_wbuf(stream, &mal->args.buff);
            if (mal->args.size < vk_mal_blksz(mal, pthis->stream.addr, 0, VSF_MAL_OP_READ)) {
                break;
            }

            vsf_eda_post_evt(pthis->stream.cur_eda, VSF_EVT_MAL_READ);
        }
        break;
    }
}

static void __vk_mal_read_stream_do(uintptr_t target, vsf_evt_t evt)
{
    vk_mal_stream_t *pthis = (vk_mal_stream_t *)target;
    vk_mal_t *mal = pthis->mal;
    vsf_stream_t *stream = pthis->stream.stream;

    switch (evt) {
    case VSF_EVT_INIT:
        pthis->stream.rw_size = 0;
        pthis->stream.cur_eda = vsf_eda_get_cur();
        stream->tx.param = pthis;
        stream->tx.evthandler = __vk_mal_stream_tx_evthandler;
        vsf_stream_connect_tx(stream);
        break;
    case VSF_EVT_RETURN:
        if (VSF_ERR_NONE == mal->result.errcode) {
            pthis->stream.size -= mal->result.size;
            pthis->stream.addr += mal->result.size;
            pthis->stream.rw_size += mal->result.size;
            vsf_stream_write(stream, NULL, mal->result.size);
        }
        if ((mal->result.errcode != VSF_ERR_NONE) || !pthis->stream.size) {
            vsf_stream_disconnect_tx(stream);
            vsf_eda_return();
        }
        break;
    case VSF_EVT_MAL_READ:
        mal->args.size = min(mal->args.size, pthis->stream.size);
        vk_mal_read(mal, pthis->stream.addr, mal->args.size, mal->args.buff);
        break;
    }
}

vsf_err_t vk_mal_read_stream(vk_mal_stream_t *pthis, uint_fast64_t addr, uint_fast32_t size, vsf_stream_t *stream)
{
    VSF_MAL_ASSERT(pthis != NULL);
    pthis->stream.addr = addr;
    pthis->stream.size = size;
    pthis->stream.stream = stream;
    return __vsf_call_eda((uintptr_t)__vk_mal_read_stream_do, (uintptr_t)pthis);
}

static void __vk_mal_stream_rx_evthandler(void *param, vsf_stream_evt_t evt)
{
    vk_mal_stream_t *pthis = (vk_mal_stream_t *)param;
    vk_mal_t *mal = pthis->mal;
    vsf_stream_t *stream = pthis->stream.stream;

    switch (evt) {
    case VSF_STREAM_ON_CONNECT:
    case VSF_STREAM_ON_IN:
        if (pthis->stream.size > 0) {
            mal->args.size = vsf_stream_get_rbuf(stream, &mal->args.buff);
            if (mal->args.size < vk_mal_blksz(mal, pthis->stream.addr, 0, VSF_MAL_OP_WRITE)) {
                break;
            }

            vsf_eda_post_evt(pthis->stream.cur_eda, VSF_EVT_MAL_WRITE);
        }
        break;
    }
}

static void __vk_mal_write_stream_do(uintptr_t target, vsf_evt_t evt)
{
    vk_mal_stream_t *pthis = (vk_mal_stream_t *)target;
    vk_mal_t *mal = pthis->mal;
    vsf_stream_t *stream = pthis->stream.stream;

    switch (evt) {
    case VSF_EVT_INIT:
        pthis->stream.rw_size = 0;
        pthis->stream.cur_eda = vsf_eda_get_cur();
        stream->rx.param = pthis;
        stream->rx.evthandler = __vk_mal_stream_rx_evthandler;
        vsf_stream_connect_rx(stream);
        break;
    case VSF_EVT_RETURN:
        if (VSF_ERR_NONE == mal->result.errcode) {
            pthis->stream.size -= mal->result.size;
            pthis->stream.addr += mal->result.size;
            pthis->stream.rw_size += mal->result.size;
            vsf_stream_read(stream, NULL, mal->result.size);
        }
        if ((mal->result.errcode != VSF_ERR_NONE) || !pthis->stream.size) {
            vsf_stream_disconnect_rx(stream);
            vsf_eda_return();
        }
        break;
    case VSF_EVT_MAL_WRITE:
        mal->args.size = min(mal->args.size, pthis->stream.size);
        vk_mal_write(mal, pthis->stream.addr, mal->args.size, mal->args.buff);
        break;
    }
}

vsf_err_t vk_mal_write_stream(vk_mal_stream_t *pthis, uint_fast64_t addr, uint_fast32_t size, vsf_stream_t *stream)
{
    VSF_MAL_ASSERT(pthis != NULL);
    pthis->stream.addr = addr;
    pthis->stream.size = size;
    pthis->stream.stream = stream;
    return __vsf_call_eda((uintptr_t)__vk_mal_write_stream_do, (uintptr_t)pthis);
}
#endif

#endif
