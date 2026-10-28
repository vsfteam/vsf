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

#include "../../vsf_mal_cfg.h"

#if VSF_USE_MAL == ENABLED && VSF_MAL_USE_SDMMC_MAL == ENABLED && VSF_HAL_USE_SDIO == ENABLED

#define __VSF_MAL_CLASS_INHERIT__
#define __VSF_SDMMC_MAL_CLASS_IMPLEMENT

#include "../../vsf_mal.h"
#include "./vsf_sdmmc_mal.h"

/*============================ MACROS ========================================*/

#define VSF_EVT_SDMMC_ERROR             (VSF_EVT_USER + 0)
#define VSF_EVT_SDMMC_DONE              (VSF_EVT_USER + 1)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct __vk_sdmmc_mal_rw_local_t {
    uint32_t chunk;
    uint32_t transfer_unit;
    uint32_t max_data_size;
    bool use_bounce;
} __vk_sdmmc_mal_rw_local_t;

/*============================ PROTOTYPES ====================================*/

static uint_fast32_t __vk_sdmmc_mal_blksz(vk_mal_t *mal, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op);
static vsf_mal_capability_t __vk_sdmmc_mal_capability(vk_mal_t *mal);
dcl_vsf_peda_methods(static, __vk_sdmmc_mal_init)
dcl_vsf_peda_methods(static, __vk_sdmmc_mal_fini)
dcl_vsf_peda_methods(static, __vk_sdmmc_mal_read)
dcl_vsf_peda_methods(static, __vk_sdmmc_mal_write)

/*============================ GLOBAL VARIABLES ==============================*/

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-function-type"
#endif

const vk_mal_drv_t vk_sdmmc_mal_drv = {
    .blksz          = __vk_sdmmc_mal_blksz,
    .capability     = __vk_sdmmc_mal_capability,
    .read_local_size= sizeof(__vk_sdmmc_mal_rw_local_t),
    .write_local_size=sizeof(__vk_sdmmc_mal_rw_local_t),
    .init           = (vsf_peda_evthandler_t)vsf_peda_func(__vk_sdmmc_mal_init),
    .fini           = (vsf_peda_evthandler_t)vsf_peda_func(__vk_sdmmc_mal_fini),
    .read           = (vsf_peda_evthandler_t)vsf_peda_func(__vk_sdmmc_mal_read),
    .write          = (vsf_peda_evthandler_t)vsf_peda_func(__vk_sdmmc_mal_write),
};

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

static void __vk_sdmmc_mal_on_timer(vsf_callback_timer_t *timer)
{
    vk_sdmmc_mal_t *pthis = vsf_container_of(timer, vk_sdmmc_mal_t, timer);
    if (pthis->is_probing) {
        vsf_err_t err = vsf_sdio_probe_irqhandler(pthis->sdio, &pthis->use_as__vsf_sdio_probe_t, 0, 0, NULL);
        if (err <= 0) {
            pthis->is_probing = false;
            vsf_eda_post_evt(pthis->eda, (err < 0) ? VSF_EVT_SDMMC_ERROR : VSF_EVT_SDMMC_DONE);
        }
    }
}

static void __vk_sdmmc_mal_irqhandler(void *target, vsf_sdio_t *sdio,
    vsf_sdio_irq_mask_t irq_mask, vsf_sdio_reqsts_t status, uint32_t resp[4])
{
    vk_sdmmc_mal_t *pthis = target;
    vsf_eda_t *eda;
    vsf_evt_t evt;

    if (pthis->is_probing) {
        vsf_err_t err = vsf_sdio_probe_irqhandler(sdio, &pthis->use_as__vsf_sdio_probe_t, irq_mask, status, resp);
        if (err > 0) {
            if (pthis->delay_ms > 0) {
                vsf_callback_timer_add_ms(&pthis->timer, pthis->delay_ms);
            }
        } else {
            pthis->is_probing = false;
            evt = (err < 0) ? VSF_EVT_SDMMC_ERROR : VSF_EVT_SDMMC_DONE;
            goto request_done;
        }
    } else if (pthis->is_stopping) {
        pthis->is_stopping = false;
        evt = !(irq_mask & SDIO_IRQ_MASK_HOST_RESP_DONE) ? VSF_EVT_SDMMC_ERROR : VSF_EVT_SDMMC_DONE;
        goto request_done;
    } else {
        if (status & SDIO_REQSTS_ERR_MASK) {
            vsf_trace_error("sdmmc: sdio failed with 0x%x" VSF_TRACE_CFG_LINEEND, status);
            if (status & SDIO_REQSTS_ERR_RESP_NONE) {
                vsf_trace_error("sdmmc: sdio resp not received" VSF_TRACE_CFG_LINEEND);
            }
            if (status & SDIO_REQSTS_ERR_RESP_CRC) {
                vsf_trace_error("sdmmc: sdio resp crc failed" VSF_TRACE_CFG_LINEEND);
            }
            if (status & SDIO_REQSTS_ERR_DATA_CRC) {
                vsf_trace_error("sdmmc: sdio data crc failed" VSF_TRACE_CFG_LINEEND);
            }
            evt = VSF_EVT_SDMMC_ERROR;
            goto request_done;
        }
        if (irq_mask & SDIO_IRQ_MASK_HOST_DATA_DONE) {
            if (pthis->is_to_stop) {
                pthis->is_stopping = true;
                if (VSF_ERR_NONE != vsf_sdio_host_request(pthis->sdio, &(vsf_sdio_req_t){
                        .cmd                = MMC_STOP_TRANSMISSION,
                        .op                 = MMC_STOP_TRANSMISSION_OP,
                    })) {
                    pthis->is_stopping = false;
                    evt = VSF_EVT_SDMMC_ERROR;
                    goto request_done;
                }
                return;
            }
            evt = VSF_EVT_SDMMC_DONE;
            goto request_done;
        }
    }
    return;
request_done:
    pthis->is_to_stop = false;
    if ((eda = pthis->eda) != NULL) {
        pthis->eda = NULL;
        vsf_eda_post_evt(eda, evt);
    }
    return;
}

static uint_fast32_t __vk_sdmmc_mal_blksz(vk_mal_t *mal, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op)
{
    vk_sdmmc_mal_t *pthis = (vk_sdmmc_mal_t *)mal;
    return 1 << pthis->csd.sd_v2.READ_BL_LEN;
}

typedef struct __vk_sdmmc_mal_buffer_cap_t {
    uint32_t max_data_size;
    uint16_t ptr_alignment;
    uint16_t size_alignment;
} __vk_sdmmc_mal_buffer_cap_t;

static __vk_sdmmc_mal_buffer_cap_t __vk_sdmmc_mal_get_buffer_cap(
        vk_sdmmc_mal_t *pthis, bool is_write)
{
    vsf_sdio_capability_t cap = vsf_sdio_capability(pthis->sdio);
    uint16_t cache_alignment = is_write
        ? cap.write_cache_alignment : cap.read_cache_alignment;
    __vk_sdmmc_mal_buffer_cap_t result = {
        .max_data_size  = cap.max_data_size,
        .ptr_alignment  = cap.data_ptr_alignment ? cap.data_ptr_alignment : 1,
        .size_alignment = cap.data_size_alignment ? cap.data_size_alignment : 1,
    };

    if (cache_alignment > result.ptr_alignment) {
        result.ptr_alignment = cache_alignment;
    }
    if (cache_alignment > result.size_alignment) {
        result.size_alignment = cache_alignment;
    }
    VSF_MAL_ASSERT(!(result.ptr_alignment & (result.ptr_alignment - 1)));
    return result;
}

static bool __vk_sdmmc_mal_buffer_is_direct(uint8_t *buffer, uint32_t size,
        __vk_sdmmc_mal_buffer_cap_t cap)
{
    return !((uintptr_t)buffer % cap.ptr_alignment)
        && !(size % cap.size_alignment);
}

static uint32_t __vk_sdmmc_mal_gcd(uint32_t a, uint32_t b)
{
    while (b != 0) {
        uint32_t tmp = a % b;
        a = b;
        b = tmp;
    }
    return a;
}

static bool __vk_sdmmc_mal_get_transfer_unit(uint32_t block_size,
        __vk_sdmmc_mal_buffer_cap_t cap, uint32_t *transfer_unit)
{
    uint32_t gcd;

    if ((0 == block_size) || (0 == cap.size_alignment)) {
        return false;
    }
    gcd = __vk_sdmmc_mal_gcd(block_size, cap.size_alignment);
    block_size /= gcd;
    if (block_size > ((uint32_t)-1) / cap.size_alignment) {
        return false;
    }
    *transfer_unit = block_size * cap.size_alignment;
    return true;
}

static bool __vk_sdmmc_mal_can_bounce(vk_sdmmc_mal_t *pthis)
{
    if (pthis->bounce_buffer != NULL) {
        return true;
    }
#if (VSF_SDMMC_MAL_CFG_BOUNCE_BUFFER_SIZE > 0) && (VSF_USE_HEAP == ENABLED)
    return true;
#else
    return false;
#endif
}

static vsf_err_t __vk_sdmmc_mal_ensure_bounce(vk_sdmmc_mal_t *pthis,
        uint32_t transfer_unit)
{
    __vk_sdmmc_mal_buffer_cap_t read_cap = __vk_sdmmc_mal_get_buffer_cap(pthis, false);
    __vk_sdmmc_mal_buffer_cap_t write_cap = __vk_sdmmc_mal_get_buffer_cap(pthis, true);
    uint16_t alignment = vsf_max(read_cap.ptr_alignment, write_cap.ptr_alignment);

    VSF_MAL_ASSERT(transfer_unit > 0);
    if (pthis->bounce_buffer != NULL) {
        if (pthis->bounce_buffer_size < transfer_unit) {
            return VSF_ERR_PROVIDED_RESOURCE_NOT_SUFFICIENT;
        }
        if ((uintptr_t)pthis->bounce_buffer & (alignment - 1)) {
            return VSF_ERR_PROVIDED_RESOURCE_NOT_ALIGNED;
        }
        return VSF_ERR_NONE;
    }

#if (VSF_SDMMC_MAL_CFG_BOUNCE_BUFFER_SIZE > 0) && (VSF_USE_HEAP == ENABLED)
    if (VSF_SDMMC_MAL_CFG_BOUNCE_BUFFER_SIZE < transfer_unit) {
        return VSF_ERR_PROVIDED_RESOURCE_NOT_SUFFICIENT;
    }
    alignment = vsf_max(alignment, 4);
    uint8_t *buffer = vsf_heap_malloc_aligned(
        VSF_SDMMC_MAL_CFG_BOUNCE_BUFFER_SIZE, alignment);
    if (NULL == buffer) {
        return VSF_ERR_NOT_ENOUGH_RESOURCES;
    }
    pthis->bounce_buffer = buffer;
    pthis->bounce_buffer_size = VSF_SDMMC_MAL_CFG_BOUNCE_BUFFER_SIZE;
    pthis->bounce_is_owned = true;
    return VSF_ERR_NONE;
#else
    return VSF_ERR_NOT_ENOUGH_RESOURCES;
#endif
}

static uint32_t __vk_sdmmc_mal_chunk_size(vk_sdmmc_mal_t *pthis,
        uint32_t remaining, uint32_t transfer_unit, uint32_t max_data_size,
        bool use_bounce)
{
    uint32_t chunk = remaining;
    if (use_bounce && (chunk > pthis->bounce_buffer_size)) {
        chunk = pthis->bounce_buffer_size;
    }
    if (max_data_size && (chunk > max_data_size)) {
        chunk = max_data_size;
    }
    return chunk - chunk % transfer_unit;
}

static vsf_mal_capability_t __vk_sdmmc_mal_capability(vk_mal_t *mal)
{
    vk_sdmmc_mal_t *pthis = (vk_sdmmc_mal_t *)mal;
    __vk_sdmmc_mal_buffer_cap_t read_cap = __vk_sdmmc_mal_get_buffer_cap(pthis, false);
    __vk_sdmmc_mal_buffer_cap_t write_cap = __vk_sdmmc_mal_get_buffer_cap(pthis, true);
    bool can_bounce = __vk_sdmmc_mal_can_bounce(pthis);

    return (vsf_mal_capability_t){
        .data_size_alignment     = write_cap.size_alignment,
        .read_data_ptr_alignment = can_bounce ? 1 : read_cap.ptr_alignment,
        .write_data_ptr_alignment = can_bounce ? 1 : write_cap.ptr_alignment,
    };
}

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#endif

__vsf_component_peda_ifs_entry(__vk_sdmmc_mal_init, vk_mal_init)
{
    vsf_peda_begin();

    vk_sdmmc_mal_t *pthis = (vk_sdmmc_mal_t *)&vsf_this;
    switch (evt) {
    case VSF_EVT_INIT:
        pthis->timer.on_timer = __vk_sdmmc_mal_on_timer;
        vsf_callback_timer_init(&pthis->timer);

        pthis->is_to_stop = false;
        pthis->is_stopping = false;
        pthis->is_probing = true;
        pthis->eda = vsf_eda_get_cur();
        VSF_MAL_ASSERT(pthis->eda != NULL);
        vsf_sdio_init(pthis->sdio, &(vsf_sdio_cfg_t) {
            .mode   = SDIO_MODE_HOST,
            .isr    = {
                .prio       = pthis->hw_priority,
                .target_ptr = pthis,
                .handler_fn = __vk_sdmmc_mal_irqhandler,
            },
        });
        if (VSF_ERR_NONE != vsf_sdio_probe_start(pthis->sdio, &pthis->use_as__vsf_sdio_probe_t)) {
            pthis->is_probing = false;
            pthis->eda = NULL;
            vsf_eda_return(VSF_ERR_FAIL);
        }
        break;
    case VSF_EVT_SDMMC_ERROR:
        vsf_eda_return(VSF_ERR_FAIL);
        break;
    case VSF_EVT_SDMMC_DONE:
        pthis->size = pthis->capacity * (1 << pthis->csd.sd_v2.READ_BL_LEN);
        vsf_sdio_irq_enable(pthis->sdio, SDIO_IRQ_MASK_HOST_RESP_DONE | SDIO_IRQ_MASK_HOST_DATA_DONE);
        vsf_eda_return(VSF_ERR_NONE);
        break;
    }

    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_sdmmc_mal_fini, vk_mal_fini)
{
    vsf_peda_begin();
    vk_sdmmc_mal_t *pthis = (vk_sdmmc_mal_t *)&vsf_this;

    VSF_MAL_ASSERT(pthis->eda == NULL);
#if VSF_USE_HEAP == ENABLED
    if (pthis->bounce_is_owned) {
        vsf_heap_free(pthis->bounce_buffer);
        pthis->bounce_buffer = NULL;
        pthis->bounce_buffer_size = 0;
        pthis->bounce_is_owned = false;
    }
#else
    VSF_MAL_ASSERT(!pthis->bounce_is_owned);
#endif
    vsf_eda_return(VSF_ERR_NONE);
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_sdmmc_mal_read, vk_mal_read,
    implement(__vk_sdmmc_mal_rw_local_t)
)
{
    vsf_peda_begin();
    vk_sdmmc_mal_t *pthis = (vk_sdmmc_mal_t *)&vsf_this;
    uint32_t block_size = (1 << pthis->csd.sd_v2.READ_BL_LEN);

#define __SDMMC_ISSUE_RX_CHUNK()                                               \
    do {                                                                       \
        uint32_t __off = vsf_local.rsize;                                      \
        vsf_local.chunk = __vk_sdmmc_mal_chunk_size(pthis,                     \
            vsf_local.size - __off, vsf_local.transfer_unit,                    \
            vsf_local.max_data_size, vsf_local.use_bounce);                     \
        uint32_t __block_num = vsf_local.chunk / block_size;                   \
        uint32_t __block_start = (uint32_t)((vsf_local.addr + __off)           \
                                                / block_size);                  \
        pthis->is_to_stop = __block_num > 1;                                   \
        vsf_err_t __err = vsf_sdio_host_request(pthis->sdio, &(vsf_sdio_req_t){   \
            .cmd            = __block_num > 1 ? MMC_READ_MULTIPLE_BLOCK        \
                                             : MMC_READ_SINGLE_BLOCK,          \
            .arg            = pthis->high_capacity ? __block_start             \
                             : (uint32_t)(vsf_local.addr + __off),             \
            .op             = __block_num > 1 ? MMC_READ_MULTIPLE_BLOCK_OP     \
                                             : MMC_READ_SINGLE_BLOCK_OP,       \
            .block_size_bits= pthis->csd.sd_v2.READ_BL_LEN,                    \
            .buffer         = vsf_local.use_bounce ? pthis->bounce_buffer      \
                             : vsf_local.buff + __off,                         \
            .count          = vsf_local.chunk,                                 \
        });                                                                        \
        if (__err != VSF_ERR_NONE) {                                           \
            pthis->eda = NULL;                                                 \
            pthis->is_to_stop = false;                                         \
            vsf_eda_return(__err);                                             \
        }                                                                      \
    } while (0)

    switch (evt) {
    case VSF_EVT_INIT: {
            if (NULL == vsf_local.buff) {
                vsf_eda_return(VSF_ERR_INVALID_PARAMETER);
                break;
            }
            if (0 == vsf_local.size) {
                vsf_eda_return(0);
                break;
            }
            if ((vsf_local.addr % block_size) || (vsf_local.size % block_size)) {
                vsf_eda_return(VSF_ERR_INVALID_PARAMETER);
                break;
            }
            __vk_sdmmc_mal_buffer_cap_t cap = __vk_sdmmc_mal_get_buffer_cap(pthis, false);
            if (!__vk_sdmmc_mal_get_transfer_unit(block_size, cap,
                    &vsf_local.transfer_unit)) {
                vsf_eda_return(VSF_ERR_INVALID_PARAMETER);
                break;
            }
            vsf_local.use_bounce = !__vk_sdmmc_mal_buffer_is_direct(
                vsf_local.buff, vsf_local.size, cap);
            if (    !vsf_local.use_bounce && cap.max_data_size
                &&  (vsf_local.size > cap.max_data_size)) {
                __vk_sdmmc_mal_buffer_cap_t ptr_cap = {
                    .size_alignment = cap.ptr_alignment,
                };
                if (!__vk_sdmmc_mal_get_transfer_unit(vsf_local.transfer_unit,
                        ptr_cap, &vsf_local.transfer_unit)) {
                    vsf_eda_return(VSF_ERR_INVALID_PARAMETER);
                    break;
                }
            }
            if (    (vsf_local.size % vsf_local.transfer_unit)
                ||  (cap.max_data_size && (cap.max_data_size < vsf_local.transfer_unit))) {
                vsf_eda_return(VSF_ERR_INVALID_PARAMETER);
                break;
            }
            if (vsf_local.use_bounce) {
                vsf_err_t err = __vk_sdmmc_mal_ensure_bounce(pthis, vsf_local.transfer_unit);
                if (err != VSF_ERR_NONE) {
                    vsf_eda_return(err);
                    break;
                }
            }
            vsf_local.max_data_size = cap.max_data_size;
            pthis->eda = vsf_eda_get_cur();
            VSF_MAL_ASSERT(pthis->eda != NULL);
            vsf_local.rsize = 0;
            __SDMMC_ISSUE_RX_CHUNK();
            break;
        }
    case VSF_EVT_SDMMC_ERROR:
        vsf_eda_return(-1);
        break;
    case VSF_EVT_SDMMC_DONE:
        if (vsf_local.use_bounce) {
            memcpy(vsf_local.buff + vsf_local.rsize, pthis->bounce_buffer, vsf_local.chunk);
        }
        vsf_local.rsize += vsf_local.chunk;
        if (vsf_local.rsize >= vsf_local.size) {
            vsf_eda_return(vsf_local.size);
        } else {
            pthis->eda = vsf_eda_get_cur();
            __SDMMC_ISSUE_RX_CHUNK();
        }
        break;
    }
    vsf_peda_end();
#undef __SDMMC_ISSUE_RX_CHUNK
}

__vsf_component_peda_ifs_entry(__vk_sdmmc_mal_write, vk_mal_write,
    implement(__vk_sdmmc_mal_rw_local_t)
)
{
    vsf_peda_begin();
    vk_sdmmc_mal_t *pthis = (vk_sdmmc_mal_t *)&vsf_this;
    uint32_t block_size = (1 << pthis->csd.sd_v2.READ_BL_LEN);

#define __SDMMC_ISSUE_TX_CHUNK()                                               \
    do {                                                                       \
        uint32_t __off = vsf_local.wsize;                                      \
        vsf_local.chunk = __vk_sdmmc_mal_chunk_size(pthis,                     \
            vsf_local.size - __off, vsf_local.transfer_unit,                    \
            vsf_local.max_data_size, vsf_local.use_bounce);                     \
        uint32_t __block_num = vsf_local.chunk / block_size;                   \
        uint32_t __block_start = (uint32_t)((vsf_local.addr + __off)           \
                                                / block_size);                  \
        if (vsf_local.use_bounce) {                                            \
            memcpy(pthis->bounce_buffer, vsf_local.buff + __off,               \
                vsf_local.chunk);                                              \
        }                                                                      \
        pthis->is_to_stop = __block_num > 1;                                   \
        vsf_err_t __err = vsf_sdio_host_request(pthis->sdio, &(vsf_sdio_req_t){   \
            .cmd            = __block_num > 1 ? MMC_WRITE_MULTIPLE_BLOCK       \
                                             : MMC_WRITE_BLOCK,                \
            .arg            = pthis->high_capacity ? __block_start             \
                             : (uint32_t)(vsf_local.addr + __off),             \
            .op             = __block_num > 1 ? MMC_WRITE_MULTIPLE_BLOCK_OP    \
                                             : MMC_WRITE_BLOCK_OP,             \
            .block_size_bits= pthis->csd.sd_v2.READ_BL_LEN,                    \
            .buffer         = vsf_local.use_bounce ? pthis->bounce_buffer      \
                             : vsf_local.buff + __off,                         \
            .count          = vsf_local.chunk,                                 \
        });                                                                        \
        if (__err != VSF_ERR_NONE) {                                           \
            pthis->eda = NULL;                                                 \
            pthis->is_to_stop = false;                                         \
            vsf_eda_return(__err);                                             \
        }                                                                      \
    } while (0)

    switch (evt) {
    case VSF_EVT_INIT: {
            if (NULL == vsf_local.buff) {
                vsf_eda_return(VSF_ERR_INVALID_PARAMETER);
                break;
            }
            if (0 == vsf_local.size) {
                vsf_eda_return(0);
                break;
            }
            if ((vsf_local.addr % block_size) || (vsf_local.size % block_size)) {
                vsf_eda_return(VSF_ERR_INVALID_PARAMETER);
                break;
            }
            __vk_sdmmc_mal_buffer_cap_t cap = __vk_sdmmc_mal_get_buffer_cap(pthis, true);
            if (!__vk_sdmmc_mal_get_transfer_unit(block_size, cap,
                    &vsf_local.transfer_unit)) {
                vsf_eda_return(VSF_ERR_INVALID_PARAMETER);
                break;
            }
            vsf_local.use_bounce = !__vk_sdmmc_mal_buffer_is_direct(
                vsf_local.buff, vsf_local.size, cap);
            if (    !vsf_local.use_bounce && cap.max_data_size
                &&  (vsf_local.size > cap.max_data_size)) {
                __vk_sdmmc_mal_buffer_cap_t ptr_cap = {
                    .size_alignment = cap.ptr_alignment,
                };
                if (!__vk_sdmmc_mal_get_transfer_unit(vsf_local.transfer_unit,
                        ptr_cap, &vsf_local.transfer_unit)) {
                    vsf_eda_return(VSF_ERR_INVALID_PARAMETER);
                    break;
                }
            }
            if (    (vsf_local.size % vsf_local.transfer_unit)
                ||  (cap.max_data_size && (cap.max_data_size < vsf_local.transfer_unit))) {
                vsf_eda_return(VSF_ERR_INVALID_PARAMETER);
                break;
            }
            if (vsf_local.use_bounce) {
                vsf_err_t err = __vk_sdmmc_mal_ensure_bounce(pthis, vsf_local.transfer_unit);
                if (err != VSF_ERR_NONE) {
                    vsf_eda_return(err);
                    break;
                }
            }
            vsf_local.max_data_size = cap.max_data_size;
            pthis->eda = vsf_eda_get_cur();
            VSF_MAL_ASSERT(pthis->eda != NULL);
            vsf_local.wsize = 0;
            __SDMMC_ISSUE_TX_CHUNK();
            break;
        }
    case VSF_EVT_SDMMC_ERROR:
        vsf_eda_return(-1);
        break;
    case VSF_EVT_SDMMC_DONE:
        vsf_local.wsize += vsf_local.chunk;
        if (vsf_local.wsize >= vsf_local.size) {
            vsf_eda_return(vsf_local.size);
        } else {
            pthis->eda = vsf_eda_get_cur();
            __SDMMC_ISSUE_TX_CHUNK();
        }
        break;
    }
    vsf_peda_end();
#undef __SDMMC_ISSUE_TX_CHUNK
}

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic pop
#endif

#endif
