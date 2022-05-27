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

#ifndef __HAL_DRIVER_TIMER_INTERFACE_H__
#define __HAL_DRIVER_TIMER_INTERFACE_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_template_hal_driver.h"
#include "hal/arch/vsf_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_TIMER_CFG_MULTI_CLASS
#   define VSF_TIMER_CFG_MULTI_CLASS              DISABLED
#endif

// Turn off multi class support for the current implementation
// when the VSF_TIMER_CFG_MULTI_CLASS is enabled
#ifndef VSF_TIMER_CFG_IMPLEMENT_OP
#   if VSF_TIMER_CFG_MULTI_CLASS == ENABLED
#       define VSF_TIMER_CFG_IMPLEMENT_OP         ENABLED
#   else
#       define VSF_TIMER_CFG_IMPLEMENT_OP         DISABLED
#   endif
#endif

// VSF_TIMER_CFG_PREFIX: use for macro vsf_timer_{init, enable, ...}
#ifndef VSF_TIMER_CFG_PREFIX
#   if VSF_TIMER_CFG_MULTI_CLASS == ENABLED
#       define VSF_TIMER_CFG_PREFIX               vsf
#   elif defined(VSF_HW_TIMER_COUNT) && (VSF_HW_TIMER_COUNT != 0)
#       define VSF_TIMER_CFG_PREFIX               vsf_hw
#   endif
#endif

#ifndef VSF_TIMER_CFG_FUNCTION_RENAME
#   define VSF_TIMER_CFG_FUNCTION_RENAME           ENABLED
#endif

#ifndef VSF_TIMER_CFG_REIMPLEMENT_CAPABILITY
#   define VSF_TIMER_CFG_REIMPLEMENT_CAPABILITY   DISABLED
#endif

#ifndef VSF_TIMER_CFG_REIMPLEMENT_IRQ_TYPE
#   define VSF_TIMER_CFG_REIMPLEMENT_IRQ_TYPE      DISABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_TIMER_APIS(__prefix_name) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,          timer, init,        VSF_MCONNECT(__prefix_name, _timer_t) *timer_ptr, timer_cfg_t *cfg_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,           timer, enable,      VSF_MCONNECT(__prefix_name, _timer_t) *timer_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,           timer, disable,     VSF_MCONNECT(__prefix_name, _timer_t) *timer_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,               timer, irq_enable,  VSF_MCONNECT(__prefix_name, _timer_t) *timer_ptr, em_timer_irq_mask_t irq_mask) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,               timer, irq_disable, VSF_MCONNECT(__prefix_name, _timer_t) *timer_ptr, em_timer_irq_mask_t irq_mask) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, timer_capability_t, timer, capability,  VSF_MCONNECT(__prefix_name, _timer_t) *timer_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,          timer, pwm_set,     VSF_MCONNECT(__prefix_name, _timer_t) *timer_ptr,  uint8_t channel, uint32_t period, uint32_t pulse)

/*============================ TYPES =========================================*/

// TODO: Add more feature support, for example match interrupt

#if VSF_TIMER_CFG_REIMPLEMENT_CHANNEL_FEATURE == DISABLED
typedef enum timer_mode_t{
    TIMER_MODE_ONESHOT,
    TIMER_MODE_CONTINUES,

    TIMER_MODE_PWM,
} timer_mode_t;
#endif

#if VSF_TIMER_CFG_REIMPLEMENT_IRQ_TYPE == DISABLED
typedef enum em_timer_irq_mask_t{
    VSF_TIMER_IRQ_MASK_OVERFLOW = (1 << 0),
} em_timer_irq_mask_t;
#endif

typedef struct vsf_timer_t vsf_timer_t;

typedef void vsf_timer_isr_handler_t(void *target_ptr,
                                     vsf_timer_t *timer_ptr,
                                     em_timer_irq_mask_t irq_mask);

typedef struct vsf_timer_isr_t {
    vsf_timer_isr_handler_t *handler_fn;
    void *target_ptr;
    vsf_arch_prio_t prio;
} vsf_timer_isr_t;

//! timer configuration
typedef struct timer_cfg_t {
    timer_mode_t mode;

    uint32_t max_count;
    union {
        uint32_t freq;
        uint32_t min_freq;
    };

    vsf_timer_isr_t isr;
} timer_cfg_t;


#if VSF_TIMER_CFG_REIMPLEMENT_CAPABILITY == DISABLED
typedef struct timer_capability_t {
    inherit(peripheral_capability_t)
} timer_capability_t;
#endif

typedef struct vsf_timer_op_t {
#undef __VSF_HAL_TEMPLATE_API
#define __VSF_HAL_TEMPLATE_API VSF_HAL_TEMPLATE_API_FP

    VSF_TIMER_APIS(vsf)
} vsf_timer_op_t;

#if VSF_TIMER_CFG_MULTI_CLASS == ENABLED
struct vsf_timer_t  {
    const vsf_timer_op_t * op;
};
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vsf_timer_init(vsf_timer_t *timer_ptr, timer_cfg_t *cfg_ptr);
extern fsm_rt_t vsf_timer_enable(vsf_timer_t *timer_ptr);
extern fsm_rt_t vsf_timer_disable(vsf_timer_t *timer_ptr);
extern timer_capability_t vsf_timer_capability(vsf_timer_t *timer_ptr);
extern void vsf_timer_irq_enable(vsf_timer_t *timer_ptr, em_timer_irq_mask_t irq_mask);
extern void vsf_timer_irq_disable(vsf_timer_t *timer_ptr, em_timer_irq_mask_t irq_mask);
extern vsf_err_t vsf_timer_pwm_set(vsf_timer_t *timer_ptr, uint8_t channel, uint32_t period, uint32_t pulse);

/**
 * set timer date time
 *
 * @param[in] timer_ptr timer instance
 * @param[in] timer data time
 */
extern vsf_err_t vsf_timer_set(vsf_timer_t *timer_ptr, uint32_t count);

/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_TIMER_CFG_FUNCTION_RENAME == ENABLED
#   define vsf_timer_init(__TIME, ...)                                          \
        VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_init)        ((VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_t) *)__TIME, ##__VA_ARGS__)
#   define vsf_timer_enable(__TIME)                                             \
        VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_enable)      ((VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_t) *)__TIME)
#   define vsf_timer_disable(__TIME)                                            \
        VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_disable)     ((VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_t) *)__TIME)
#   define vsf_timer_capability(__TIME)                                         \
        VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_capability)  ((VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_t) *)__TIME)
#   define vsf_timer_irq_enable(__TIME, ...)                                    \
        VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_irq_enable)  ((VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_t) *)__TIME, ##__VA_ARGS__)
#   define vsf_timer_irq_disable(__TIME, ...)                                   \
        VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_irq_disable) ((VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_t) *)__TIME, ##__VA_ARGS__)
#   define vsf_timer_match_set(__TIME, ...)                                     \
        VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_match_set)   ((VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_t) *)__TIME, ##__VA_ARGS__)
#   define vsf_timer_pwm_set(__TIME, ...)                                       \
        VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_pwm_set)     ((VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_t) *)__TIME, ##__VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif  /*__HAL_DRIVER_TIMER_INTERFACE_H__*/
