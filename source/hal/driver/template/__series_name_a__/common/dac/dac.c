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

#if VSF_HAL_USE_DAC == ENABLED

/*\note For IPCore drivers, define __VSF_HAL_${DAC_IP}_DAC_CLASS_IMPLEMENT before including vsf_hal.h.
 *      For peripheral drivers, if IPCore driver is used, define __VSF_HAL_${DAC_IP}_DAC_CLASS_INHERIT__ before including vsf_hal.h
 */

// IPCore
#define __VSF_HAL_${DAC_IP}_DAC_CLASS_IMPLEMENT
// IPCore end
// HW using ${DAC_IP} IPCore driver
#define __VSF_HAL_${DAC_IP}_DAC_CLASS_INHERIT__
// HW end

#include "hal/vsf_hal.h"

// HW
// for vendor headers
#define __VSF_HAL_SHOW_VENDOR_INFO__
#include "hal/driver/vendor_driver.h"
// HW end

/*============================ MACROS ========================================*/

/*\note VSF_HW_DAC_CFG_MULTI_CLASS is only for drivers for specified device(hw drivers).
 *      For other drivers, please define VSF_${DAC_IP}_DAC_CFG_MULTI_CLASS in header file.
 */

// HW
#ifndef VSF_HW_DAC_CFG_MULTI_CLASS
#   define VSF_HW_DAC_CFG_MULTI_CLASS           VSF_DAC_CFG_MULTI_CLASS
#endif
// HW end

// HW
#define VSF_DAC_CFG_IMP_PREFIX                  vsf_hw
#define VSF_DAC_CFG_IMP_UPCASE_PREFIX           VSF_HW
// HW end
// IPCore
#define VSF_DAC_CFG_IMP_PREFIX                  vsf_${dac_ip}
#define VSF_DAC_CFG_IMP_UPCASE_PREFIX           VSF_${DAC_IP}
// IPCore end

/*============================ TYPES =========================================*/

// HW
typedef struct VSF_MCONNECT(VSF_DAC_CFG_IMP_PREFIX, _dac_t) {
#if VSF_HW_DAC_CFG_MULTI_CLASS == ENABLED
    vsf_dac_t               vsf_dac;
#endif
    void                    *reg;
    vsf_dac_isr_t           isr;
} VSF_MCONNECT(VSF_DAC_CFG_IMP_PREFIX, _dac_t);
// HW end


/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*============================ IMPLEMENTATION ================================*/

/*\note Implementation below is for hw dac only, because there is no requirements
 *          on the APIs of IPCore drivers.
 *      Usage of VSF_MCONNECT is not a requirement, but for convenience only,
 */

// HW
vsf_err_t VSF_MCONNECT(VSF_DAC_CFG_IMP_PREFIX, _dac_init)(
    VSF_MCONNECT(VSF_DAC_CFG_IMP_PREFIX, _dac_t) *dac_ptr,
    vsf_dac_cfg_t* cfg_ptr
) {
    VSF_HAL_ASSERT((NULL != dac_ptr) && (NULL != cfg_ptr));
    // configure according to cfg_ptr
    dac_ptr->isr = cfg_ptr->isr;
    // configure interrupt according to cfg_ptr->isr
    return VSF_ERR_NONE;
}

void VSF_MCONNECT(VSF_DAC_CFG_IMP_PREFIX, _dac_fini)(
    VSF_MCONNECT(VSF_DAC_CFG_IMP_PREFIX, _dac_t) *dac_ptr
) {
    VSF_HAL_ASSERT(dac_ptr != NULL);
}

fsm_rt_t VSF_MCONNECT(VSF_DAC_CFG_IMP_PREFIX, _dac_enable)(
    VSF_MCONNECT(VSF_DAC_CFG_IMP_PREFIX, _dac_t) *dac_ptr
) {
    VSF_HAL_ASSERT(dac_ptr != NULL);

    return fsm_rt_cpl;
}

fsm_rt_t VSF_MCONNECT(VSF_DAC_CFG_IMP_PREFIX, _dac_disable)(
    VSF_MCONNECT(VSF_DAC_CFG_IMP_PREFIX, _dac_t) *dac_ptr
) {
    VSF_HAL_ASSERT(dac_ptr != NULL);

    return fsm_rt_cpl;
}

void VSF_MCONNECT(VSF_DAC_CFG_IMP_PREFIX, _dac_irq_enable)(
    VSF_MCONNECT(VSF_DAC_CFG_IMP_PREFIX, _dac_t) *dac_ptr,
    vsf_dac_irq_mask_t irq_mask
) {
    VSF_HAL_ASSERT(dac_ptr != NULL);
}

void VSF_MCONNECT(VSF_DAC_CFG_IMP_PREFIX, _dac_irq_disable)(
    VSF_MCONNECT(VSF_DAC_CFG_IMP_PREFIX, _dac_t) *dac_ptr,
    vsf_dac_irq_mask_t irq_mask
) {
    VSF_HAL_ASSERT(dac_ptr != NULL);
}

vsf_dac_status_t VSF_MCONNECT(VSF_DAC_CFG_IMP_PREFIX, _dac_status)(
    VSF_MCONNECT(VSF_DAC_CFG_IMP_PREFIX, _dac_t) *dac_ptr
) {
    VSF_HAL_ASSERT(NULL != dac_ptr);
    return (vsf_dac_status_t) {
        .is_busy = false,
    };
}

vsf_dac_capability_t VSF_MCONNECT(VSF_DAC_CFG_IMP_PREFIX, _dac_capability)(
    VSF_MCONNECT(VSF_DAC_CFG_IMP_PREFIX, _dac_t) *dac_ptr
) {
    return (vsf_dac_capability_t) {
        .max_resolution_bits = 8,
        .min_resolution_bits = 4,
        .channel_count       = 4,
    };
}

vsf_err_t VSF_MCONNECT(VSF_DAC_CFG_IMP_PREFIX, _dac_channel_request_once)(
    VSF_MCONNECT(VSF_DAC_CFG_IMP_PREFIX, _dac_t) *dac_ptr,
    vsf_dac_channel_cfg_t *channel_cfg,
    uint_fast16_t value
) {
    VSF_HAL_ASSERT(dac_ptr != NULL);
    VSF_HAL_ASSERT(channel_cfg != NULL);

    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_DAC_CFG_IMP_PREFIX, _dac_channel_config)(
    VSF_MCONNECT(VSF_DAC_CFG_IMP_PREFIX, _dac_t) *dac_ptr,
    vsf_dac_channel_cfg_t *channel_cfgs_ptr,
    uint_fast8_t channel_cfgs_cnt
) {
    VSF_HAL_ASSERT(dac_ptr != NULL);
    VSF_HAL_ASSERT(channel_cfgs_ptr != NULL);
    VSF_HAL_ASSERT(channel_cfgs_cnt != 0);

    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_DAC_CFG_IMP_PREFIX, _dac_channel_request)(
    VSF_MCONNECT(VSF_DAC_CFG_IMP_PREFIX, _dac_t) *dac_ptr,
    void *buffer_ptr,
    uint_fast32_t count
) {
    VSF_HAL_ASSERT(dac_ptr != NULL);
    VSF_HAL_ASSERT(buffer_ptr != NULL);
    VSF_HAL_ASSERT(count != 0);

    return VSF_ERR_NONE;
}

static vsf_dac_irq_mask_t VSF_MCONNECT(__, VSF_DAC_CFG_IMP_PREFIX, _dac_get_irq_mask)(
    VSF_MCONNECT(VSF_DAC_CFG_IMP_PREFIX, _dac_t) *dac_ptr
) {
    // implement this function in the device file
    VSF_HAL_ASSERT(0);
    return 0;
}

static void VSF_MCONNECT(__, VSF_DAC_CFG_IMP_PREFIX, _dac_irqhandler)(
    VSF_MCONNECT(VSF_DAC_CFG_IMP_PREFIX, _dac_t) *dac_ptr
) {
    VSF_HAL_ASSERT(NULL != dac_ptr);

    vsf_dac_irq_mask_t irq_mask = VSF_MCONNECT(__, VSF_DAC_CFG_IMP_PREFIX, _dac_get_irq_mask)(dac_ptr);
    vsf_dac_isr_t *isr_ptr = &dac_ptr->isr;
    if ((irq_mask != 0) && (isr_ptr->handler_fn != NULL)) {
        isr_ptr->handler_fn(isr_ptr->target_ptr, (vsf_dac_t *)dac_ptr, irq_mask);
    }
}
// HW end

/*============================ MACROFIED FUNCTIONS ===========================*/

/*\note Instantiation below is for hw dac only.
 *      Usage of VSF_MCONNECT is not a requirement, but for convenience only,
 */

// HW
#define VSF_DAC_CFG_REIMPLEMENT_API_CAPABILITY        ENABLED
#define VSF_DAC_CFG_IMP_LV0(__IDX, __HAL_OP)                                    \
    VSF_MCONNECT(VSF_DAC_CFG_IMP_PREFIX, _dac_t)                                \
        VSF_MCONNECT(VSF_DAC_CFG_IMP_PREFIX, _dac, __IDX) = {                   \
        .reg                = VSF_MCONNECT(VSF_DAC_CFG_IMP_UPCASE_PREFIX, _DAC, __IDX,_REG),\
        __HAL_OP                                                                \
    };                                                                          \
    VSF_CAL_ROOT void VSF_MCONNECT(VSF_DAC_CFG_IMP_UPCASE_PREFIX, _DAC, __IDX, _IRQHandler)(void)\
    {                                                                           \
        uintptr_t ctx = vsf_hal_irq_enter();                                    \
        VSF_MCONNECT(__, VSF_DAC_CFG_IMP_PREFIX, _dac_irqhandler)(              \
            &VSF_MCONNECT(VSF_DAC_CFG_IMP_PREFIX, _dac, __IDX)                  \
        );                                                                      \
        vsf_hal_irq_leave(ctx);                                                 \
    }
#include "hal/driver/common/dac/dac_template.inc"
// HW end

#endif /* VSF_HAL_USE_DAC */
