/*****************************************************************************
 *   Cop->right(C)2009-2019 by VSF Team                                      *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a cop-> of the License at                                 *
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

#if VSF_HAL_USE_I2S == ENABLED

/*\note For IPCore drivers, define __VSF_HAL_${I2S_IP}_I2S_CLASS_IMPLEMENT before including vsf_hal.h.
 *      For peripheral drivers, if IPCore driver is used, define __VSF_HAL_${I2S_IP}_I2S_CLASS_INHERIT__ before including vsf_hal.h
 */

// IPCore
#define __VSF_HAL_${I2S_IP}_I2S_CLASS_IMPLEMENT
// IPCore end
// HW using ${I2S_IP} IPCore driver
#define __VSF_HAL_${I2S_IP}_I2S_CLASS_INHERIT__
// HW end

#include "hal/vsf_hal.h"

#include "../../__device.h"

// HW
// for vendor headers
#define __VSF_HAL_SHOW_VENDOR_INFO__
#include "hal/driver/vendor_driver.h"
// HW end

/*============================ MACROS ========================================*/

/*\note VSF_HW_I2S_CFG_MULTI_CLASS is only for drivers for specified device(hw drivers).
 *      For other drivers, please define VSF_${I2S_IP}_I2S_CFG_MULTI_CLASS in header file.
 */

// HW
#ifndef VSF_HW_I2S_CFG_MULTI_CLASS
#   define VSF_HW_I2S_CFG_MULTI_CLASS           VSF_I2S_CFG_MULTI_CLASS
#endif
// HW end

// HW
#define VSF_I2S_CFG_IMP_PREFIX                  vsf_hw
#define VSF_I2S_CFG_IMP_UPCASE_PREFIX           VSF_HW
// HW end
// IPCore
#define VSF_I2S_CFG_IMP_PREFIX                  vsf_${i2s_ip}
#define VSF_I2S_CFG_IMP_UPCASE_PREFIX           VSF_${I2S_IP}
// IPCore end

/*============================ TYPES =========================================*/

// HW
typedef struct VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_t) {
#if VSF_HW_I2S_CFG_MULTI_CLASS == ENABLED
    vsf_i2s_t               vsf_i2s;
#endif
    void                    *reg;
    vsf_i2s_isr_t           isr;
} VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_t);
// HW end


/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/*\note Implementation below is for hw i2s only, because there is no requirements
 *          on the APIs of IPCore drivers.
 *      Usage of VSF_MCONNECT is not a requirement, but for convenience only,
 */

// HW
vsf_err_t VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_init)(
    VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_t) *i2s_ptr,
    vsf_i2s_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT((NULL != i2s_ptr) && (NULL != cfg_ptr));
    // configure according to cfg_ptr
    i2s_ptr->isr = cfg_ptr->isr;
    // configure interrupt according to cfg_ptr->isr
    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_tx_init)(
    VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_t) *i2s_ptr,
    vsf_i2s_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT(i2s_ptr != NULL);

    return VSF_ERR_NONE;
}

void VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_tx_fini)(
    VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_t) *i2s_ptr
) {
    VSF_HAL_ASSERT(i2s_ptr != NULL);
}

vsf_err_t VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_tx_start)(
    VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_t) *i2s_ptr
) {
    VSF_HAL_ASSERT(i2s_ptr != NULL);

    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_rx_init)(
    VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_t) *i2s_ptr,
    vsf_i2s_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT(i2s_ptr != NULL);

    return VSF_ERR_NONE;
}

void VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_rx_fini)(
    VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_t) *i2s_ptr
) {
    VSF_HAL_ASSERT(i2s_ptr != NULL);
}

vsf_err_t VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_rx_start)(
    VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_t) *i2s_ptr
) {
    VSF_HAL_ASSERT(i2s_ptr != NULL);

    return VSF_ERR_NONE;
}

fsm_rt_t VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_enable)(
    VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_t) *i2s_ptr
) {
    VSF_HAL_ASSERT(i2s_ptr != NULL);

    return fsm_rt_cpl;
}

fsm_rt_t VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_disable)(
    VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_t) *i2s_ptr
) {
    VSF_HAL_ASSERT(i2s_ptr != NULL);

    return fsm_rt_cpl;
}

vsf_i2s_status_t VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_status)(
    VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_t) *i2s_ptr
) {
    VSF_HAL_ASSERT(i2s_ptr != NULL);

    return (vsf_i2s_status_t) {
        .value = 0,
    };
}

vsf_i2s_capability_t VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_capability)(
    VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_t) *i2s_ptr
) {
    VSF_HAL_ASSERT(i2s_ptr != NULL);

    return (vsf_i2s_capability_t) {
        .i2s_capability.is_src_supported = true,
        .i2s_capability.is_dbuffer_supported = true,
    };
}

static vsf_i2s_irq_mask_t VSF_MCONNECT(__, VSF_I2S_CFG_IMP_PREFIX, _i2s_get_irq_mask)(
    VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_t) *i2s_ptr
) {
    // implement this function in the device file
    VSF_HAL_ASSERT(0);
    return 0;
}

static void VSF_MCONNECT(__, VSF_I2S_CFG_IMP_PREFIX, _i2s_irqhandler)(
    VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_t) *i2s_ptr
) {
    VSF_HAL_ASSERT(NULL != i2s_ptr);

    vsf_i2s_irq_mask_t irq_mask = VSF_MCONNECT(__, VSF_I2S_CFG_IMP_PREFIX, _i2s_get_irq_mask)(i2s_ptr);
    vsf_i2s_isr_t *isr_ptr = &i2s_ptr->isr;
    if ((irq_mask != 0) && (isr_ptr->handler_fn != NULL)) {
        isr_ptr->handler_fn(isr_ptr->target_ptr, (vsf_i2s_t *)i2s_ptr, irq_mask);
    }
}
// HW end

/*============================ MACROFIED FUNCTIONS ===========================*/

/*\note Instantiation below is for hw i2s only.
 *      Usage of VSF_MCONNECT is not a requirement, but for convenience only,
 */

// HW
#define VSF_I2S_CFG_REIMPLEMENT_API_CAPABILITY             ENABLED
#define VSF_I2S_CFG_IMP_LV0(__IDX, __HAL_OP)                                    \
    VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s_t)                                \
        VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s, __IDX) = {                   \
        .reg                = VSF_MCONNECT(VSF_I2S_CFG_IMP_UPCASE_PREFIX, _I2S, __IDX,_REG),\
        __HAL_OP                                                                \
    };                                                                          \
    VSF_CAL_ROOT void VSF_MCONNECT(VSF_I2S_CFG_IMP_UPCASE_PREFIX, _I2S, __IDX, _IRQHandler)(void)\
    {                                                                           \
        uintptr_t ctx = vsf_hal_irq_enter();                                    \
        VSF_MCONNECT(__, VSF_I2S_CFG_IMP_PREFIX, _i2s_irqhandler)(              \
            &VSF_MCONNECT(VSF_I2S_CFG_IMP_PREFIX, _i2s, __IDX)                  \
        );                                                                      \
        vsf_hal_irq_leave(ctx);                                                 \
    }
#include "hal/driver/common/i2s/i2s_template.inc"
// HW end

#endif /* VSF_HAL_USE_I2S */
