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

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_DMA == ENABLED

/*\note For IPCore drivers, define __VSF_HAL_${DMA_IP}_DMA_CLASS_IMPLEMENT before including vsf_hal.h.
 *      For peripheral drivers, if IPCore driver is used, define __VSF_HAL_${DMA_IP}_DMA_CLASS_INHERIT__ before including vsf_hal.h
 */

// IPCore
#define __VSF_HAL_${DMA_IP}_DMA_CLASS_IMPLEMENT
// IPCore end
// HW using ${DMA_IP} IPCore driver
#define __VSF_HAL_${DMA_IP}_DMA_CLASS_INHERIT__
// HW end

#include "hal/vsf_hal.h"

// HW
// for vendor headers
#define __VSF_HAL_SHOW_VENDOR_INFO__
#include "hal/driver/vendor_driver.h"
// HW end

/*============================ MACROS ========================================*/

/*\note VSF_HW_DMA_CFG_MULTI_CLASS is only for drivers for specified device(hw drivers).
 *      For other drivers, please define VSF_${DMA_IP}_DMA_CFG_MULTI_CLASS in header file.
 */

// HW
#ifndef VSF_HW_DMA_CFG_MULTI_CLASS
#   define VSF_HW_DMA_CFG_MULTI_CLASS           VSF_DMA_CFG_MULTI_CLASS
#endif
// HW end

// HW
#define VSF_DMA_CFG_IMP_PREFIX                  vsf_hw
#define VSF_DMA_CFG_IMP_UPCASE_PREFIX           VSF_HW
// HW end
// IPCore
#define VSF_DMA_CFG_IMP_PREFIX                  vsf_${dma_ip}
#define VSF_DMA_CFG_IMP_UPCASE_PREFIX           VSF_${DMA_IP}
// IPCore end

/*============================ TYPES =========================================*/

// HW
typedef struct VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) {
#if VSF_HW_DMA_CFG_MULTI_CLASS == ENABLED
    vsf_dma_t               vsf_dma;
#endif
    void                    *reg;
    vsf_dma_isr_t           isr;
} VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t);
// HW end


/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*============================ IMPLEMENTATION ================================*/

/*\note Implementation below is for hw dma only, because there is no requirements
 *          on the APIs of IPCore drivers.
 *      Usage of VSF_MCONNECT is not a requirement, but for convenience only,
 */

// HW
vsf_err_t VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_init)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr
) {
    VSF_HAL_ASSERT(NULL != dma_ptr);
    return VSF_ERR_NONE;
}

void VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_fini)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr
) {
    VSF_HAL_ASSERT(dma_ptr != NULL);
}

vsf_dma_capability_t VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_capability)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr
) {
    VSF_HAL_ASSERT(dma_ptr != NULL);
    return (vsf_dma_capability_t) {
        .irq_mask          = 0xff,
        .max_request_count = 8,
        .channel_count     = 16
    };
}

int8_t VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_request)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr,
    void *filter_param
) {
    return 0;
}

void VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_release)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr,
    int8_t channel
) {
    VSF_HAL_ASSERT(dma_ptr != NULL);
}

vsf_err_t VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_config)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr,
    int8_t channel,
                                 vsf_dma_channel_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT(dma_ptr != NULL);

    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_start)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr,
    int8_t channel,
    uint32_t src_address,
    uint32_t dst_address,
    uint32_t count
) {
    VSF_HAL_ASSERT(dma_ptr != NULL);

    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_cancel)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr,
    int8_t channel
) {
    VSF_HAL_ASSERT(dma_ptr != NULL);

    return VSF_ERR_NONE;
}

uint32_t VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_get_transferred_count)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr,
    int8_t channel
) {
    VSF_HAL_ASSERT(dma_ptr != NULL);
    return 0;
}

vsf_dma_channel_status_t VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_status)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr,
    int8_t channel)
{
    VSF_HAL_ASSERT(dma_ptr != NULL);

    return (vsf_dma_channel_status_t) {
        .is_busy = 1,
    };
}

static vsf_dma_irq_mask_t VSF_MCONNECT(__, VSF_DMA_CFG_IMP_PREFIX, _dma_get_irq_mask)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr
) {
    // implement this function in the device file
    VSF_HAL_ASSERT(0);
    return 0;
}

static int8_t VSF_MCONNECT(__, VSF_DMA_CFG_IMP_PREFIX, _dma_get_irq_channel)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr
) {
    // implement this function in the device file
    VSF_HAL_ASSERT(0);
    return 0;
}

static void VSF_MCONNECT(__, VSF_DMA_CFG_IMP_PREFIX, _dma_irqhandler)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr
) {
    VSF_HAL_ASSERT(NULL != dma_ptr);

    vsf_dma_irq_mask_t irq_mask = VSF_MCONNECT(__, VSF_DMA_CFG_IMP_PREFIX, _dma_get_irq_mask)(dma_ptr);
    int8_t channel = VSF_MCONNECT(__, VSF_DMA_CFG_IMP_PREFIX, _dma_get_irq_channel)(dma_ptr);
    vsf_dma_isr_t *isr_ptr = &dma_ptr->isr;
    if ((irq_mask != 0) && (isr_ptr->handler_fn != NULL)) {
        isr_ptr->handler_fn(isr_ptr->target_ptr, (vsf_dma_t *)dma_ptr, channel, irq_mask);
    }
}
// HW end

/*============================ MACROFIED FUNCTIONS ===========================*/

/*\note Instantiation below is for hw dma only.
 *      Usage of VSF_MCONNECT is not a requirement, but for convenience only,
 */

// HW
#define VSF_DMA_CFG_REIMPLEMENT_API_CAPABILITY        ENABLED
#define VSF_DMA_CFG_IMP_LV0(__IDX, __HAL_OP)                                    \
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t)                                \
        VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma, __IDX) = {                   \
        .reg                = VSF_MCONNECT(VSF_DMA_CFG_IMP_UPCASE_PREFIX, _DMA, __IDX, _REG),\
        __HAL_OP                                                                \
    };                                                                          \
    VSF_CAL_ROOT void VSF_MCONNECT(VSF_DMA_CFG_IMP_UPCASE_PREFIX, _DMA, __IDX, _IRQHandler)(void)\
    {                                                                           \
        uintptr_t ctx = vsf_hal_irq_enter();                                    \
        VSF_MCONNECT(__, VSF_DMA_CFG_IMP_PREFIX, _dma_irqhandler)(              \
            &VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma, __IDX)                  \
        );                                                                      \
        vsf_hal_irq_leave(ctx);                                                 \
    }
#include "hal/driver/common/dma/dma_template.inc"
// HW end

#endif /* VSF_HAL_USE_DMA */

