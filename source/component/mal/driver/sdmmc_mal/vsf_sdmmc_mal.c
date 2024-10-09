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
                vsf_sdio_host_request(pthis->sdio, &(vsf_sdio_req_t){
                    .cmd                = MMC_STOP_TRANSMISSION,
                    .op                 = MMC_STOP_TRANSMISSION_OP,
                });
                return;
            }
            evt = VSF_EVT_SDMMC_DONE;
            goto request_done;
        }
    }
    return;
request_done:
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

static vsf_mal_capability_t __vk_sdmmc_mal_capability(vk_mal_t *mal)
{
    vk_sdmmc_mal_t *pthis = (vk_sdmmc_mal_t *)mal;
    vsf_sdio_capability_t cap = vsf_sdio_capability(pthis->sdio);
    return (vsf_mal_capability_t){
        .data_ptr_alignment     = !cap.data_ptr_alignment ? 1 : cap.data_ptr_alignment,
        .data_size_alignment    = !cap.data_size_alignment ? 1 : cap.data_size_alignment,
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
        VSF_FS_ASSERT(pthis->eda != NULL);
        vsf_sdio_init(pthis->sdio, &(vsf_sdio_cfg_t) {
            .mode   = SDIO_MODE_HOST,
            .isr    = {
                .prio       = pthis->hw_priority,
                .target_ptr = pthis,
                .handler_fn = __vk_sdmmc_mal_irqhandler,
            },
        });
        if (VSF_ERR_NONE != vsf_sdio_probe_start(pthis->sdio, &pthis->use_as__vsf_sdio_probe_t)) {
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
    vsf_eda_return(VSF_ERR_NONE);
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_sdmmc_mal_read, vk_mal_read)
{
    vsf_peda_begin();
    vk_sdmmc_mal_t *pthis = (vk_sdmmc_mal_t *)&vsf_this;

    uint32_t block_size = (1 << pthis->csd.sd_v2.READ_BL_LEN);
    uint32_t block_start = vsf_local.addr / block_size;
    uint32_t block_num = vsf_local.size / block_size;

    switch (evt) {
    case VSF_EVT_INIT:
        pthis->eda = vsf_eda_get_cur();
        VSF_FS_ASSERT(pthis->eda != NULL);
        pthis->is_to_stop = block_num > 1;
        vsf_sdio_host_request(pthis->sdio, &(vsf_sdio_req_t){
            .cmd                = block_num > 1 ? MMC_READ_MULTIPLE_BLOCK : MMC_READ_SINGLE_BLOCK,
            .arg                = pthis->high_capacity ? block_start : vsf_local.addr,
            .op                 = block_num > 1 ? MMC_READ_MULTIPLE_BLOCK_OP : MMC_READ_SINGLE_BLOCK_OP,
            .block_size_bits    = pthis->csd.sd_v2.READ_BL_LEN,
            .buffer             = vsf_local.buff,
            .count              = vsf_local.size,
        });
        break;
    case VSF_EVT_SDMMC_ERROR:
        vsf_eda_return(-1);
        break;
    case VSF_EVT_SDMMC_DONE:
        vsf_eda_return(vsf_local.size);
        break;
    }
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_sdmmc_mal_write, vk_mal_write)
{
    vsf_peda_begin();
    vk_sdmmc_mal_t *pthis = (vk_sdmmc_mal_t *)&vsf_this;

    uint32_t block_size = (1 << pthis->csd.sd_v2.READ_BL_LEN);
    uint32_t block_start = vsf_local.addr / block_size;
    uint32_t block_num = vsf_local.size / block_size;

    switch (evt) {
    case VSF_EVT_INIT:
        pthis->eda = vsf_eda_get_cur();
        VSF_FS_ASSERT(pthis->eda != NULL);
        vsf_sdio_host_request(pthis->sdio, &(vsf_sdio_req_t){
            .cmd                = block_num > 1 ? MMC_WRITE_MULTIPLE_BLOCK : MMC_WRITE_BLOCK,
            .arg                = pthis->high_capacity ? block_start : vsf_local.addr,
            .op                 = block_num > 1 ? MMC_WRITE_MULTIPLE_BLOCK_OP : MMC_WRITE_BLOCK_OP,
            .block_size_bits    = pthis->csd.sd_v2.READ_BL_LEN,
            .buffer             = vsf_local.buff,
            .count              = vsf_local.size,
        });
        break;
    case VSF_EVT_SDMMC_ERROR:
        vsf_eda_return(-1);
        break;
    case VSF_EVT_SDMMC_DONE:
        vsf_eda_return(vsf_local.size);
        break;
    }
    vsf_peda_end();
}

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic pop
#endif

#endif
