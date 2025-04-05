/*****************************************************************************
 *   Cop->right(C)2009-2019 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a cop-> of the License at                                  *
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

#if VSF_HAL_USE_TIMER == ENABLED

/*\note For IPCore drivers, define __VSF_HAL_${TIMER_IP}_TIMER_CLASS_IMPLEMENT
 * before including vsf_hal.h. For peripheral drivers, if IPCore driver is used,
 * define __VSF_HAL_${TIMER_IP}_TIMER_CLASS_INHERIT__ before including vsf_hal.h
 */

// IPCore
#   define __VSF_HAL_$ {TIMER_IP} _TIMER_CLASS_IMPLEMENT
// IPCore end
// HW using ${TIMER_IP} IPCore driver
#   define __VSF_HAL_$ {TIMER_IP} _TIMER_CLASS_INHERIT__
// HW end

#   include "hal/vsf_hal.h"

/*============================ MACROS ========================================*/

/*\note VSF_HW_TIMER_CFG_MULTI_CLASS is only for drivers for specified device(hw
 * drivers). For other drivers, please define
 * VSF_${TIMER_IP}_TIMER_CFG_MULTI_CLASS in header file.
 */

// HW
#   ifndef VSF_HW_TIMER_CFG_MULTI_CLASS
#      define VSF_HW_TIMER_CFG_MULTI_CLASS VSF_TIMER_CFG_MULTI_CLASS
#   endif
// HW end

// HW
#   define VSF_TIMER_CFG_IMP_PREFIX         vsf_hw
#   define VSF_TIMER_CFG_IMP_UPCASE_PREFIX  VSF_HW
// HW end
// IPCore
#   define VSF_TIMER_CFG_IMP_PREFIX         vsf_${timer_ip}
#   define VSF_TIMER_CFG_IMP_UPCASE_PREFIX  VSF_${TIMER_IP}
// IPCore end

/*============================ TYPES =========================================*/

// HW
typedef struct VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_t) {
#   if VSF_HW_TIMER_CFG_MULTI_CLASS == ENABLED
    vsf_timer_t vsf_timer;
#   endif
    void           *reg;
    vsf_timer_isr_t isr;
} VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_t);
// HW end

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*============================ IMPLEMENTATION ================================*/

/*\note Implementation below is for hw timer only, because there is no
 * requirements on the APIs of IPCore drivers. Usage of VSF_MCONNECT is not a
 * requirement, but for convenience only,
 */

// HW
vsf_err_t VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_init)(
    VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_t) * timer_ptr,
    vsf_timer_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT((NULL != timer_ptr) && (NULL != cfg_ptr));
    // configure according to cfg_ptr
    timer_ptr->isr = cfg_ptr->isr;
    // configure interrupt according to cfg_ptr->isr
    return VSF_ERR_NONE;
}

void VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_fini)(
    VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_t) * timer_ptr)
{
    VSF_HAL_ASSERT(timer_ptr != NULL);
}

fsm_rt_t VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_enable)(
    VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_t) * timer_ptr)
{
    VSF_HAL_ASSERT(timer_ptr != NULL);

    return fsm_rt_cpl;
}

fsm_rt_t VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_disable)(
    VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_t) * timer_ptr)
{
    VSF_HAL_ASSERT(timer_ptr != NULL);

    return fsm_rt_cpl;
}

void VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_irq_enable)(
    VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_t) * timer_ptr,
    vsf_timer_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(timer_ptr != NULL);
}

void VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_irq_disable)(
    VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_t) * timer_ptr,
    vsf_timer_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(timer_ptr != NULL);
}

vsf_timer_capability_t VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX,
                                    _timer_capability)(
    VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_t) * timer_ptr)
{
    VSF_HAL_ASSERT(timer_ptr != NULL);

    return (vsf_timer_capability_t){
        .irq_mask               = VSF_TIMER_IRQ_MASK_OVERFLOW,
        .timer_bitlen           = 32,
        .support_pwm            = 1,
        .support_input_capture  = 1,
        .support_output_compare = 1,
        .support_one_pulse      = 1,
        .channel_cnt            = 4,
    };
}

vsf_err_t VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_set_period)(
    VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_t) * timer_ptr,
    uint32_t period)
{
    VSF_HAL_ASSERT(timer_ptr != NULL);

    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_ctrl)(
    VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_t) * timer_ptr,
    vsf_timer_ctrl_t ctrl, void *param)
{
    VSF_HAL_ASSERT(timer_ptr != NULL);

    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_channel_config)(
    VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_t) * timer_ptr,
    uint8_t channel, vsf_timer_channel_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT((timer_ptr != NULL) && (cfg_ptr != NULL));

    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_channel_start)(
    VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_t) * timer_ptr,
    uint8_t channel)
{
    VSF_HAL_ASSERT(timer_ptr != NULL);

    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_channel_stop)(
    VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_t) * timer_ptr,
    uint8_t channel)
{
    VSF_HAL_ASSERT(timer_ptr != NULL);

    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_channel_request_start)(
    VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_t) * timer_ptr,
    uint8_t channel, vsf_timer_channel_request_t *request_ptr)
{
    VSF_HAL_ASSERT((timer_ptr != NULL) && (request_ptr != NULL));

    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_channel_request_stop)(
    VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_t) * timer_ptr,
    uint8_t channel)
{
    VSF_HAL_ASSERT(timer_ptr != NULL);

    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_channel_ctrl)(
    VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_t) * timer_ptr,
    uint8_t channel, vsf_timer_channel_ctrl_t ctrl, void *param)
{
    VSF_HAL_ASSERT(timer_ptr != NULL);

    return VSF_ERR_NONE;
}

static void VSF_MCONNECT(__, VSF_TIMER_CFG_IMP_PREFIX, _timer_irqhandler)(
    VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_t) * timer_ptr)
{
    VSF_HAL_ASSERT(NULL != timer_ptr);

    vsf_timer_irq_mask_t irq_mask = GET_IRQ_MASK(timer_ptr);
    vsf_timer_isr_t     *isr_ptr  = &timer_ptr->isr;
    if ((irq_mask != 0) && (isr_ptr->handler_fn != NULL)) {
        isr_ptr->handler_fn(isr_ptr->target_ptr, (vsf_timer_t *)timer_ptr,
                            irq_mask);
    }
}
// HW end

/*============================ MACROFIED FUNCTIONS ===========================*/

/*\note Instantiation below is for hw timer only.
 *      Usage of VSF_MCONNECT is not a requirement, but for convenience only,
 */

// HW
#   define VSF_TIMER_CFG_REIMPLEMENT_API_CAPABILITY ENABLED
#   define VSF_TIMER_CFG_IMP_LV0(__IDX, __HAL_OP)                              \
       VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_t)                        \
       VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer, __IDX) = {               \
           .reg = VSF_MCONNECT(VSF_TIMER_CFG_IMP_UPCASE_PREFIX, _TIMER, __IDX, \
                               _REG),                                          \
           __HAL_OP};                                                          \
       VSF_CAL_ROOT void VSF_MCONNECT(VSF_TIMER_CFG_IMP_UPCASE_PREFIX, _TIMER, \
                                      __IDX, _IRQHandler)(void)                \
       {                                                                       \
           uintptr_t ctx = vsf_hal_irq_enter();                                \
           VSF_MCONNECT(__, VSF_TIMER_CFG_IMP_PREFIX, _timer_irqhandler)(      \
               &VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer, __IDX));        \
           vsf_hal_irq_leave(ctx);                                             \
       }
#   include "hal/driver/common/timer/timer_template.inc"
// HW end

#endif /* VSF_HAL_USE_TIMER */
