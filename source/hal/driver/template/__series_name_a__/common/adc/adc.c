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

#if VSF_HAL_USE_ADC == ENABLED

/*\note For IPCore drivers, define __VSF_HAL_${ADC_IP}_ADC_CLASS_IMPLEMENT before including vsf_hal.h.
 *      For peripheral drivers, if IPCore driver is used, define __VSF_HAL_${ADC_IP}_ADC_CLASS_INHERIT__ before including vsf_hal.h
 */

// IPCore
#define __VSF_HAL_${ADC_IP}_ADC_CLASS_IMPLEMENT
// IPCore end
// HW using ${ADC_IP} IPCore driver
#define __VSF_HAL_${ADC_IP}_ADC_CLASS_INHERIT__
// HW end

#include "hal/vsf_hal.h"

// HW
// for vendor headers
#define __VSF_HAL_SHOW_VENDOR_INFO__
#include "hal/driver/vendor_driver.h"
// HW end

/*============================ MACROS ========================================*/

/*\note VSF_HW_ADC_CFG_MULTI_CLASS is only for drivers for specified device(hw drivers).
 *      For other drivers, please define VSF_${ADC_IP}_ADC_CFG_MULTI_CLASS in header file.
 */

// HW
#ifndef VSF_HW_ADC_CFG_MULTI_CLASS
#   define VSF_HW_ADC_CFG_MULTI_CLASS           VSF_ADC_CFG_MULTI_CLASS
#endif
// HW end

// HW
#define VSF_ADC_CFG_IMP_PREFIX                  vsf_hw
#define VSF_ADC_CFG_IMP_UPCASE_PREFIX           VSF_HW
// HW end
// IPCore
#define VSF_ADC_CFG_IMP_PREFIX                  vsf_${adc_ip}
#define VSF_ADC_CFG_IMP_UPCASE_PREFIX           VSF_${ADC_IP}
// IPCore end

/*============================ TYPES =========================================*/

// HW
typedef struct VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t) {
#if VSF_HW_ADC_CFG_MULTI_CLASS == ENABLED
    vsf_adc_t               vsf_adc;
#endif
    void                    *reg;
    vsf_adc_isr_t           isr;
} VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t);
// HW end

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/*\note Implementation below is for hw adc only, because there is no requirements
 *          on the APIs of IPCore drivers.
 *      Usage of VSF_MCONNECT is not a requirement, but for convenience only,
 */

// HW
vsf_err_t VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_init)(
    VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t) *adc_ptr,
    vsf_adc_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT((NULL != adc_ptr) && (NULL != cfg_ptr));
    // configure according to cfg_ptr
    adc_ptr->isr = cfg_ptr->isr;
    // configure interrupt according to cfg_ptr->isr
    return VSF_ERR_NONE;
}

void VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_fini)(
    VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t) *adc_ptr
) {
    VSF_HAL_ASSERT(adc_ptr != NULL);
}

vsf_err_t VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_get_configuration)(
    VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t) *adc_ptr,
    vsf_adc_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT(NULL != adc_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);

    // TODO: Implement hardware-specific configuration reading
    // Read current ADC configuration from hardware registers

    // Template implementation returns default configuration
    cfg_ptr->isr = adc_ptr->isr;
    // Add other configuration fields as needed

    return VSF_ERR_NONE;
}

fsm_rt_t VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_enable)(
    VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t) *adc_ptr
) {
    VSF_HAL_ASSERT(NULL != adc_ptr);
    return fsm_rt_cpl;
}

fsm_rt_t VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_disable)(
    VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t) *adc_ptr
) {
    VSF_HAL_ASSERT(NULL != adc_ptr);
    return fsm_rt_cpl;
}

void VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_irq_enable)(
    VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t) *adc_ptr,
    vsf_adc_irq_mask_t irq_mask
) {
    VSF_HAL_ASSERT(NULL != adc_ptr);
}

void VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_irq_disable)(
    VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t) *adc_ptr,
    vsf_adc_irq_mask_t irq_mask
) {
    VSF_HAL_ASSERT(NULL != adc_ptr);
}

vsf_adc_status_t VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_status)(
    VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t) *adc_ptr
) {
    VSF_HAL_ASSERT(NULL != adc_ptr);
    return (vsf_adc_status_t) {
        .is_busy                = false,
    };
}

vsf_err_t VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_channel_request_once)(
    VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t) *adc_ptr,
    vsf_adc_channel_cfg_t *channel_cfg_ptr,
    void *buffer_ptr
) {
    VSF_HAL_ASSERT((NULL != adc_ptr) && (NULL != buffer_ptr));
    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_channel_config)(
    VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t) *adc_ptr,
    vsf_adc_channel_cfg_t *channel_cfgs_ptr,
    uint32_t channel_cfgs_cnt
) {
    VSF_HAL_ASSERT((NULL != adc_ptr) && (NULL != channel_cfgs_ptr));
    VSF_HAL_ASSERT(0 != channel_cfgs_cnt);
    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_channel_request)(
    VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t) *adc_ptr,
    void *buffer_ptr,
    uint_fast32_t count
) {
    VSF_HAL_ASSERT((NULL != adc_ptr) && (NULL != buffer_ptr) && (count > 0));
    return VSF_ERR_NONE;
}

vsf_adc_capability_t VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_capability)(
    VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t) *adc_ptr
) {
    return (vsf_adc_capability_t) {
        .irq_mask           = VSF_ADC_IRQ_MASK_CPL,
        .max_data_bits      = 8,
        .channel_count      = 1,
    };
}

static vsf_adc_irq_mask_t VSF_MCONNECT(__, VSF_ADC_CFG_IMP_PREFIX, _adc_get_irq_mask)(
    VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t) *adc_ptr
) {
    // implement this function in the device file
    VSF_HAL_ASSERT(0);
    return 0;
}

static void VSF_MCONNECT(__, VSF_ADC_CFG_IMP_PREFIX, _adc_irqhandler)(
    VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t) *adc_ptr
) {
    VSF_HAL_ASSERT(NULL != adc_ptr);

    vsf_adc_irq_mask_t irq_mask = VSF_MCONNECT(__, VSF_ADC_CFG_IMP_PREFIX, _adc_get_irq_mask)(adc_ptr);
    vsf_adc_isr_t *isr_ptr = &adc_ptr->isr;
    if ((irq_mask != 0) && (isr_ptr->handler_fn != NULL)) {
        isr_ptr->handler_fn(isr_ptr->target_ptr, (vsf_adc_t *)adc_ptr, irq_mask);
    }
}
// HW end

/*============================ MACROFIED FUNCTIONS ===========================*/

/*\note Instantiation below is for hw adc only.
 *      Usage of VSF_MCONNECT is not a requirement, but for convenience only,
 */

// HW
#define VSF_ADC_CFG_REIMPLEMENT_API_CAPABILITY        ENABLED
#define VSF_ADC_CFG_REIMPLEMENT_API_GET_CONFIGURATION ENABLED
#define VSF_ADC_CFG_IMP_LV0(__IDX, __HAL_OP)                                    \
    VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t)                                \
        VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc, __IDX) = {                   \
        .reg = VSF_MCONNECT(VSF_ADC_CFG_IMP_UPCASE_PREFIX, _ADC, __IDX, _REG),  \
        __HAL_OP                                                                \
    };                                                                          \
    VSF_CAL_ROOT void VSF_MCONNECT(VSF_ADC_CFG_IMP_UPCASE_PREFIX,               \
                                   _ADC, __IDX, _IRQHandler)(void)              \
    {                                                                           \
        uintptr_t ctx = vsf_hal_irq_enter();                                    \
        VSF_MCONNECT(__, VSF_ADC_CFG_IMP_PREFIX, _adc_irqhandler)(              \
            &VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc, __IDX)                  \
        );                                                                      \
        vsf_hal_irq_leave(ctx);                                                 \
    }
#include "hal/driver/common/adc/adc_template.inc"
// HW end

#endif /* VSF_HAL_USE_ADC */
