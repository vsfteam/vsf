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
#   error VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL is needed to use mal
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

/*============================ PROTOTYPES ====================================*/

#if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
static uint_fast32_t __vk_reentrant_mal_blksz(vk_mal_t *mal, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op);
static bool __vk_reentrant_mal_buffer(vk_mal_t *mal, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op, vsf_mem_t *mem);
static uint_fast16_t __vk_reentrant_mal_alignment(vk_mal_t *mal);
dcl_vsf_peda_methods(static, __vk_reentrant_mal_init)
dcl_vsf_peda_methods(static, __vk_reentrant_mal_fini)
dcl_vsf_peda_methods(static, __vk_reentrant_mal_read)
dcl_vsf_peda_methods(static, __vk_reentrant_mal_write)
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-function-type"
#endif

const vk_mal_drv_t vk_reentrant_mal_drv = {
    .blksz          = __vk_reentrant_mal_blksz,
    .buffer         = __vk_reentrant_mal_buffer,
//    .alignment      = __vk_reentrant_mal_alignment,
    .init           = (vsf_peda_evthandler_t)vsf_peda_func(__vk_reentrant_mal_init),
    .fini           = (vsf_peda_evthandler_t)vsf_peda_func(__vk_reentrant_mal_fini),
    .read           = (vsf_peda_evthandler_t)vsf_peda_func(__vk_reentrant_mal_read),
    .write          = (vsf_peda_evthandler_t)vsf_peda_func(__vk_reentrant_mal_write),
};

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vk_mal_init(vk_mal_t *pthis)
{
    vsf_err_t err;
    VSF_MAL_ASSERT((pthis != NULL) && (pthis->drv != NULL) && (pthis->drv->init != NULL));
    __vsf_component_call_peda_ifs(vk_mal_init, err, pthis->drv->init, pthis->drv->init_local_size, pthis);
    return err;
}

vsf_err_t vk_mal_fini(vk_mal_t *pthis)
{
    vsf_err_t err;
    VSF_MAL_ASSERT((pthis != NULL) && (pthis->drv != NULL) && (pthis->drv->fini != NULL));
    __vsf_component_call_peda_ifs(vk_mal_fini, err, pthis->drv->fini, pthis->drv->fini_local_size, pthis);
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
    mem->buffer = NULL;
    mem->size = 0;
    return false;
}

uint_fast16_t vk_mal_alignment(vk_mal_t *pthis)
{
    VSF_MAL_ASSERT((pthis != NULL) && (pthis->drv != NULL));
    if (pthis->drv->alignment != NULL) {
        return pthis->drv->alignment(pthis);
    }
    return 1;
}

// to erase all, size should be 0
vsf_err_t vk_mal_erase(vk_mal_t *pthis, uint_fast64_t addr, uint_fast32_t size)
{
    vsf_err_t err;
    VSF_MAL_ASSERT((pthis != NULL) && (pthis->drv != NULL) && (pthis->drv->erase != NULL));
    __vsf_component_call_peda_ifs(vk_mal_erase, err, pthis->drv->erase, pthis->drv->erase_local_size, pthis,
        .addr       = addr,
        .size       = size,
    );
    return err;
}

vsf_err_t vk_mal_read(vk_mal_t *pthis, uint_fast64_t addr, uint_fast32_t size, uint8_t *buff)
{
    vsf_err_t err;
    VSF_MAL_ASSERT((pthis != NULL) && (pthis->drv != NULL) && (pthis->drv->read != NULL));
    __vsf_component_call_peda_ifs(vk_mal_read, err, pthis->drv->read, pthis->drv->read_local_size, pthis,
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
    __vsf_component_call_peda_ifs(vk_mal_write, err, pthis->drv->write, pthis->drv->write_local_size, pthis,
        .addr       = addr,
        .size       = size,
        .buff       = buff,
    );
    return err;
}

/*******************************************************************************
* reentrant mal                                                                *
*******************************************************************************/

#if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
static uint_fast32_t __vk_reentrant_mal_blksz(vk_mal_t *mal, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op)
{
    vk_reentrant_mal_t *pthis = (vk_reentrant_mal_t *)mal;
    return vk_mal_blksz(pthis->mal, addr + pthis->offset, size, op);
}

static bool __vk_reentrant_mal_buffer(vk_mal_t *mal, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op, vsf_mem_t *mem)
{
    vk_reentrant_mal_t *pthis = (vk_reentrant_mal_t *)mal;
    return vk_mal_prepare_buffer(pthis->mal, addr + pthis->offset, size, op, mem);
}

static uint_fast16_t __vk_reentrant_mal_alignment(vk_mal_t *mal)
{
    vk_reentrant_mal_t *pthis = (vk_reentrant_mal_t *)mal;
    return vk_mal_alignment(pthis->mal);
}

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#endif

__vsf_component_peda_ifs_entry(__vk_reentrant_mal_init, vk_mal_init)
{
    vsf_peda_begin();
    vsf_eda_return(VSF_ERR_NONE);
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_reentrant_mal_fini, vk_mal_fini)
{
    vsf_peda_begin();
    vsf_eda_return(VSF_ERR_NONE);
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_reentrant_mal_read, vk_mal_read)
{
    vsf_peda_begin();
    vk_reentrant_mal_t *pthis = (vk_reentrant_mal_t *)&vsf_this;
    vsf_err_t err;

    switch (evt) {
    case VSF_EVT_INIT:
        if (pthis->mutex != NULL) {
            err = vsf_eda_mutex_enter(pthis->mutex);
            if (err < 0) {
                vsf_eda_return(err);
            } else if (err != VSF_ERR_NONE) {
                break;
            }
        }
        // fall througn
    case VSF_EVT_SYNC:
        err = vk_mal_read(pthis->mal, vsf_local.addr + pthis->offset, vsf_local.size, vsf_local.buff);
        if (err != VSF_ERR_NONE) {
            vsf_eda_return(err);
        }
        break;
    case VSF_EVT_RETURN:
        if (pthis->mutex != NULL) {
            vsf_eda_mutex_leave(pthis->mutex);
        }
        vsf_eda_return(vsf_eda_get_return_value());
        break;
    }
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_reentrant_mal_write, vk_mal_write)
{
    vsf_peda_begin();
    vk_reentrant_mal_t *pthis = (vk_reentrant_mal_t *)&vsf_this;
    vsf_err_t err;

    switch (evt) {
    case VSF_EVT_INIT:
        if (pthis->mutex != NULL) {
            err = vsf_eda_mutex_enter(pthis->mutex);
            if (err < 0) {
                vsf_eda_return(err);
            } else if (err != VSF_ERR_NONE) {
                break;
            }
        }
        // fall througn
    case VSF_EVT_SYNC:
        err = vk_mal_write(pthis->mal, vsf_local.addr + pthis->offset, vsf_local.size, vsf_local.buff);
        if (err != VSF_ERR_NONE) {
            vsf_eda_return(err);
        }
        break;
    case VSF_EVT_RETURN:
        if (pthis->mutex != NULL) {
            vsf_eda_mutex_leave(pthis->mutex);
        }
        vsf_eda_return(vsf_eda_get_return_value());
        break;
    }
    vsf_peda_end();
}
#endif

/*******************************************************************************
* mal stream                                                                   *
*******************************************************************************/

#if VSF_USE_SIMPLE_STREAM == ENABLED
static void __vk_mal_stream_tx_evthandler(vsf_stream_t *stream, void *param, vsf_stream_evt_t evt)
{
    vk_mal_stream_t *pthis = (vk_mal_stream_t *)param;
    vk_mal_t *mal = pthis->mal;
    uint32_t block_size, bufsize;
    vsf_protect_t orig;
    uint8_t *buffer;

    switch (evt) {
    case VSF_STREAM_ON_CONNECT:
    case VSF_STREAM_ON_OUT:
        block_size = vk_mal_blksz(mal, pthis->addr, 0, VSF_MAL_OP_READ);
        orig = vsf_protect_int();
        bufsize = vsf_stream_get_wbuf(stream, &buffer);
        if (pthis->size && !pthis->cur_size && (bufsize >= block_size)) {
            bufsize = vsf_min(bufsize, pthis->size);
            bufsize &= ~(block_size - 1);
            pthis->cur_size = bufsize;
            vsf_unprotect_int(orig);
            pthis->cur_buff = buffer;
            vsf_eda_post_evt(pthis->cur_eda, VSF_EVT_MAL_READ);
        } else {
            vsf_unprotect_int(orig);
        }
        break;
    }
}

static void __vk_mal_stream_rx_evthandler(vsf_stream_t *stream, void *param, vsf_stream_evt_t evt)
{
    vk_mal_stream_t *pthis = (vk_mal_stream_t *)param;
    vk_mal_t *mal = pthis->mal;
    uint32_t block_size, bufsize;
    vsf_protect_t orig;
    uint8_t *buffer;

    switch (evt) {
    case VSF_STREAM_ON_CONNECT:
    case VSF_STREAM_ON_IN:
        block_size = vk_mal_blksz(mal, pthis->addr, 0, VSF_MAL_OP_WRITE);
        orig = vsf_protect_int();
        bufsize = vsf_stream_get_rbuf(stream, &buffer);
        if (pthis->size && !pthis->cur_size && (bufsize >= block_size)) {
            bufsize = vsf_min(bufsize, pthis->size);
            bufsize &= ~(block_size - 1);
            pthis->cur_size = bufsize;
            vsf_unprotect_int(orig);
            pthis->cur_buff = buffer;
            vsf_eda_post_evt(pthis->cur_eda, VSF_EVT_MAL_WRITE);
        } else {
            vsf_unprotect_int(orig);
        }
        break;
    }
}

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
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
        pthis->cur_size = 0;
        pthis->rw_size = 0;
        pthis->cur_eda = vsf_eda_get_cur();
        VSF_MAL_ASSERT(pthis->cur_eda != NULL);
        stream->tx.param = pthis;
        stream->tx.evthandler = __vk_mal_stream_tx_evthandler;
        vsf_stream_connect_tx(stream);
        break;
    case VSF_EVT_RETURN: {
            int32_t result = (int32_t)vsf_eda_get_return_value();
            if (result > 0) {
                pthis->size -= result;
                pthis->addr += result;
                pthis->cur_buff += result;
                pthis->rw_size += result;
                vsf_stream_write(stream, NULL, result);
            }
            if ((result <= 0) || !pthis->size) {
                vsf_stream_disconnect_tx(stream);
                vsf_eda_return(pthis->rw_size);
                break;
            }

            uint32_t block_size_mask = ~(vk_mal_blksz(mal, pthis->addr, 0, VSF_MAL_OP_READ) - 1);
            bool is_cur_size_zero;
            vsf_protect_t orig = vsf_protect_int();
                pthis->cur_size = vsf_stream_get_wbuf(stream, &pthis->cur_buff);
                pthis->cur_size = vsf_min(pthis->cur_size, pthis->size);
                pthis->cur_size &= block_size_mask;
                is_cur_size_zero = 0 == pthis->cur_size;
            vsf_unprotect_int(orig);
            if (is_cur_size_zero) {
                break;
            }
        }
        // fall through
    case VSF_EVT_MAL_READ:
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
        pthis->cur_size = 0;
        pthis->rw_size = 0;
        pthis->cur_eda = vsf_eda_get_cur();
        VSF_MAL_ASSERT(pthis->cur_eda != NULL);
        stream->rx.param = pthis;
        stream->rx.evthandler = __vk_mal_stream_rx_evthandler;
        vsf_stream_connect_rx(stream);
        break;
    case VSF_EVT_RETURN: {
            int32_t result = (int32_t)vsf_eda_get_return_value();
            if (result > 0) {
                pthis->size -= result;
                pthis->addr += result;
                pthis->cur_buff += result;
                pthis->rw_size += result;
                vsf_stream_read(stream, NULL, result);
            }
            if ((result <= 0) || !pthis->size) {
                vsf_stream_disconnect_rx(stream);
                vsf_eda_return(pthis->rw_size);
                break;
            }

            uint32_t block_size_mask = ~(vk_mal_blksz(mal, pthis->addr, 0, VSF_MAL_OP_WRITE) - 1);
            bool is_cur_size_zero;
            vsf_protect_t orig = vsf_protect_int();
                pthis->cur_size = vsf_stream_get_rbuf(stream, &pthis->cur_buff);
                pthis->cur_size = vsf_min(pthis->cur_size, pthis->size);
                pthis->cur_size &= block_size_mask;
                is_cur_size_zero = 0 == pthis->cur_size;
            vsf_unprotect_int(orig);
            if (is_cur_size_zero) {
                break;
            }
        }
        // fall through
    case VSF_EVT_MAL_WRITE:
        vk_mal_write(mal, pthis->addr, pthis->cur_size, pthis->cur_buff);
        break;
    }
    vsf_peda_end();
}

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
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
