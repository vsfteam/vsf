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

#if VSF_USE_SIMPLE_STREAM == ENABLED
#   define __VSF_SIMPLE_STREAM_CLASS_INHERIT__
#   include "service/vsf_service.h"
#endif

#define __VSF_MAL_CLASS_IMPLEMENT
#include "./vsf_mal.h"

/*============================ MACROS ========================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL != ENABLED
#   error VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL is needed to use scsi
#endif

#if VSF_KERNEL_USE_SIMPLE_SHELL != ENABLED
#   error VSF_KERNEL_USE_SIMPLE_SHELL must be enabled
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

#if VSF_USE_SIMPLE_STREAM == ENABLED
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
    vsf_err_t err;
    VSF_MAL_ASSERT((pthis != NULL) && (pthis->drv != NULL) && (pthis->drv->init != NULL));
    __vsf_component_call_peda_ifs(vk_mal_init, err, pthis->drv->init, 0, pthis);
    return err;
}

vsf_err_t vk_mal_fini(vk_mal_t *pthis)
{
    vsf_err_t err;
    VSF_MAL_ASSERT((pthis != NULL) && (pthis->drv != NULL) && (pthis->drv->fini != NULL));
    __vsf_component_call_peda_ifs(vk_mal_fini, err, pthis->drv->fini, 0, pthis);
    return err;
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
    vsf_err_t err;
    VSF_MAL_ASSERT((pthis != NULL) && (pthis->drv != NULL) && (pthis->drv->erase != NULL));
    __vsf_component_call_peda_ifs(vk_mal_erase, err, pthis->drv->erase, 0, pthis,
        .addr       = addr,
        .size       = size,
    );
    return err;
}

vsf_err_t vk_mal_read(vk_mal_t *pthis, uint_fast64_t addr, uint_fast32_t size, uint8_t *buff)
{
    vsf_err_t err;
    VSF_MAL_ASSERT((pthis != NULL) && (pthis->drv != NULL) && (pthis->drv->read != NULL));
    __vsf_component_call_peda_ifs(vk_mal_read, err, pthis->drv->read, 0, pthis,
        .addr       = addr,
        .size       = size,
        .buff       = buff,
    );
    return err;
}

vsf_err_t vk_mal_write(vk_mal_t *pthis, uint_fast64_t addr, uint_fast32_t size, uint8_t *buff)
{
    vsf_err_t err;
    VSF_MAL_ASSERT((pthis != NULL) && (pthis->drv != NULL) && (pthis->drv->write != NULL));
    __vsf_component_call_peda_ifs(vk_mal_write, err, pthis->drv->write, 0, pthis,
        .addr       = addr,
        .size       = size,
        .buff       = buff,
    );
    return err;
}

#if VSF_USE_SIMPLE_STREAM == ENABLED
static void __vk_mal_stream_tx_evthandler(void *param, vsf_stream_evt_t evt)
{
    vk_mal_stream_t *pthis = (vk_mal_stream_t *)param;
    vk_mal_t *mal = pthis->mal;
    vsf_stream_t *stream = pthis->stream;

    switch (evt) {
    case VSF_STREAM_ON_CONNECT:
    case VSF_STREAM_ON_OUT:
        if (pthis->size > 0) {
            pthis->cur_size = vsf_stream_get_wbuf(stream, &pthis->cur_buff);
            if (pthis->cur_size < vk_mal_blksz(mal, pthis->addr, 0, VSF_MAL_OP_READ)) {
                break;
            }

            vsf_eda_post_evt(pthis->cur_eda, VSF_EVT_MAL_READ);
        }
        break;
    }
}

static void __vk_mal_stream_rx_evthandler(void *param, vsf_stream_evt_t evt)
{
    vk_mal_stream_t *pthis = (vk_mal_stream_t *)param;
    vk_mal_t *mal = pthis->mal;
    vsf_stream_t *stream = pthis->stream;

    switch (evt) {
    case VSF_STREAM_ON_CONNECT:
    case VSF_STREAM_ON_IN:
        if (pthis->size > 0) {
            pthis->cur_size = vsf_stream_get_rbuf(stream, &pthis->cur_buff);
            if (pthis->cur_size < vk_mal_blksz(mal, pthis->addr, 0, VSF_MAL_OP_WRITE)) {
                break;
            }

            vsf_eda_post_evt(pthis->cur_eda, VSF_EVT_MAL_WRITE);
        }
        break;
    }
}

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#elif   __IS_COMPILER_LLVM__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#endif

__vsf_component_peda_private_entry(__vk_mal_read_stream)
{
    vsf_peda_begin();
    vk_mal_stream_t *pthis = (vk_mal_stream_t *)&vsf_this;
    vk_mal_t *mal = pthis->mal;
    vsf_stream_t *stream = pthis->stream;

    switch (evt) {
    case VSF_EVT_INIT:
        pthis->rw_size = 0;
        pthis->cur_eda = vsf_eda_get_cur();
        stream->tx.param = pthis;
        stream->tx.evthandler = __vk_mal_stream_tx_evthandler;
        vsf_stream_connect_tx(stream);
        break;
    case VSF_EVT_RETURN: {
            int32_t result = (int32_t)vsf_eda_get_return_value();
            if (result > 0) {
                pthis->size -= result;
                pthis->addr += result;
                pthis->rw_size += result;
                vsf_stream_write(stream, NULL, result);
            }
            if ((result <= 0) || !pthis->size) {
                vsf_stream_disconnect_tx(stream);
                vsf_eda_return(pthis->rw_size);
            }
        }
        break;
    case VSF_EVT_MAL_READ:
        pthis->cur_size = min(pthis->cur_size, pthis->size);
        vk_mal_read(mal, pthis->addr, pthis->cur_size, pthis->cur_buff);
        break;
    }
    vsf_peda_end();
}

__vsf_component_peda_private_entry(__vk_mal_write_stream)
{
    vsf_peda_begin();
    vk_mal_stream_t *pthis = (vk_mal_stream_t *)&vsf_this;
    vk_mal_t *mal = pthis->mal;
    vsf_stream_t *stream = pthis->stream;

    switch (evt) {
    case VSF_EVT_INIT:
        pthis->rw_size = 0;
        pthis->cur_eda = vsf_eda_get_cur();
        stream->rx.param = pthis;
        stream->rx.evthandler = __vk_mal_stream_rx_evthandler;
        vsf_stream_connect_rx(stream);
        break;
    case VSF_EVT_RETURN: {
            int32_t result = (int32_t)vsf_eda_get_return_value();
            if (result > 0) {
                pthis->size -= result;
                pthis->addr += result;
                pthis->rw_size += result;
                vsf_stream_read(stream, NULL, result);
            }
            if ((result <= 0) || !pthis->size) {
                vsf_stream_disconnect_rx(stream);
                vsf_eda_return(pthis->rw_size);
            }
        }
        break;
    case VSF_EVT_MAL_WRITE:
        pthis->cur_size = min(pthis->cur_size, pthis->size);
        vk_mal_write(mal, pthis->addr, pthis->cur_size, pthis->cur_buff);
        break;
    }
    vsf_peda_end();
}

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#elif   __IS_COMPILER_LLVM__
#   pragma clang diagnostic pop
#endif

vsf_err_t vk_mal_read_stream(vk_mal_stream_t *pthis, uint_fast64_t addr, uint_fast32_t size, vsf_stream_t *stream)
{
    vsf_err_t err;
    VSF_MAL_ASSERT(pthis != NULL);
    pthis->addr = addr;
    pthis->size = size;
    pthis->stream = stream;
    __vsf_component_call_peda(__vk_mal_read_stream, err, pthis)
    return err;
}

vsf_err_t vk_mal_write_stream(vk_mal_stream_t *pthis, uint_fast64_t addr, uint_fast32_t size, vsf_stream_t *stream)
{
    vsf_err_t err;
    VSF_MAL_ASSERT(pthis != NULL);
    pthis->addr = addr;
    pthis->size = size;
    pthis->stream = stream;
    __vsf_component_call_peda(__vk_mal_write_stream, err, pthis)
    return err;
}
#endif

#endif
