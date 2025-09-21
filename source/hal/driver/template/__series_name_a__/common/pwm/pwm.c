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

#if VSF_HAL_USE_PWM == ENABLED

/*\note For IPCore drivers, define __VSF_HAL_${PWM_IP}_PWM_CLASS_IMPLEMENT before including vsf_hal.h.
 *      For peripheral drivers, if IPCore driver is used, define __VSF_HAL_${PWM_IP}_PWM_CLASS_INHERIT__ before including vsf_hal.h
 */

// IPCore
#define __VSF_HAL_${PWM_IP}_PWM_CLASS_IMPLEMENT
// IPCore end
// HW using ${PWM_IP} IPCore driver
#define __VSF_HAL_${PWM_IP}_PWM_CLASS_INHERIT__
// HW end

#include "hal/vsf_hal.h"

// HW
// for vendor headers
#define __VSF_HAL_SHOW_VENDOR_INFO__
#include "hal/driver/vendor_driver.h"
// HW end

/*============================ MACROS ========================================*/

/*\note VSF_HW_PWM_CFG_MULTI_CLASS is only for drivers for specified device(hw drivers).
 *      For other drivers, please define VSF_${PWM_IP}_PWM_CFG_MULTI_CLASS in header file.
 */

// HW
#ifndef VSF_HW_PWM_CFG_MULTI_CLASS
#   define VSF_HW_PWM_CFG_MULTI_CLASS           VSF_PWM_CFG_MULTI_CLASS
#endif
// HW end

// HW
#define VSF_PWM_CFG_IMP_PREFIX                  vsf_hw
#define VSF_PWM_CFG_IMP_UPCASE_PREFIX           VSF_HW
// HW end
// IPCore
#define VSF_PWM_CFG_IMP_PREFIX                  vsf_${pwm_ip}
#define VSF_PWM_CFG_IMP_UPCASE_PREFIX           VSF_${PWM_IP}
// IPCore end

/*============================ TYPES =========================================*/

// HW
typedef struct VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_t) {
#if VSF_HW_PWM_CFG_MULTI_CLASS == ENABLED
    vsf_pwm_t               vsf_pwm;
#endif
    void                    *reg;
} VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_t);
// HW end


/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*============================ IMPLEMENTATION ================================*/

/*\note Implementation below is for hw pwm only, because there is no requirements
 *          on the APIs of IPCore drivers.
 *      Usage of VSF_MCONNECT is not a requirement, but for convenience only,
 */

// HW
vsf_err_t VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_init)(
    VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_t) *pwm_ptr,
    vsf_pwm_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT((NULL != pwm_ptr) && (NULL != cfg_ptr));
    return VSF_ERR_NONE;
}

void VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_fini)(
    VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_t) *pwm_ptr
) {
    VSF_HAL_ASSERT(pwm_ptr != NULL);
}

vsf_err_t VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_get_configuration)(
    VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_t) *pwm_ptr,
    vsf_pwm_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT(NULL != pwm_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);

    // TODO: Implement hardware-specific configuration reading
    // Read current PWM configuration from hardware registers

    // Template implementation returns default configuration
    cfg_ptr->isr = pwm_ptr->isr;
    // Add other configuration fields as needed

    return VSF_ERR_NONE;
}

fsm_rt_t VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_enable)(
    VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_t) *pwm_ptr
) {
    VSF_HAL_ASSERT(pwm_ptr != NULL);

    return fsm_rt_cpl;
}

fsm_rt_t VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_disable)(
    VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_t) *pwm_ptr
) {
    VSF_HAL_ASSERT(pwm_ptr != NULL);

    return fsm_rt_cpl;
}

vsf_pwm_capability_t VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_capability)(
    VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_t) *pwm_ptr
) {
    VSF_HAL_ASSERT(pwm_ptr != NULL);

    return (vsf_pwm_capability_t) {
        .max_freq = 100 * 1000 * 1000,
        .min_freq = 100,
    };
}

vsf_err_t VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_set)(
    VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_t) *pwm_ptr,
    uint8_t channel,
    uint32_t period,
    uint32_t pulse
) {
    VSF_HAL_ASSERT(pwm_ptr != NULL);

    return VSF_ERR_NONE;
}

uint32_t VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_get_freq)(
    VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_t) *pwm_ptr
) {
    VSF_HAL_ASSERT(pwm_ptr != NULL);

    return 0;
}
// HW end

/*============================ MACROFIED FUNCTIONS ===========================*/

/*\note Instantiation below is for hw pwm only.
 *      Usage of VSF_MCONNECT is not a requirement, but for convenience only,
 */

// HW
#define VSF_PWM_CFG_REIMPLEMENT_API_CAPABILITY             ENABLED
#define VSF_PWM_CFG_REIMPLEMENT_API_GET_CONFIGURATION ENABLED
#define VSF_PWM_CFG_IMP_LV0(__IDX, __HAL_OP)                                    \
    VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_t)                                \
        VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm, __IDX) = {                   \
        .reg                = VSF_MCONNECT(VSF_PWM_CFG_IMP_UPCASE_PREFIX, _PWM, __IDX, _REG),\
        __HAL_OP                                                                \
    };                                                                          \
    VSF_CAL_ROOT void VSF_MCONNECT(VSF_PWM_CFG_IMP_UPCASE_PREFIX, _PWM, __IDX, _IRQHandler)(void)\
    {                                                                           \
        uintptr_t ctx = vsf_hal_irq_enter();                                    \
        VSF_MCONNECT(__, VSF_PWM_CFG_IMP_PREFIX, _pwm_irqhandler)(              \
            &VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm, __IDX)                  \
        );                                                                      \
        vsf_hal_irq_leave(ctx);                                                 \
    }
#include "hal/driver/common/pwm/pwm_template.inc"
// HW end

#endif /* VSF_HAL_USE_PWM */
