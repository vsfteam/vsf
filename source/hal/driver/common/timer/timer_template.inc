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

#if VSF_HAL_USE_TIMER == ENABLED

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

#ifndef VSF_TIMER_CFG_REIMPLEMENT_API_CAPABILITY
#   define  VSF_TIMER_CFG_REIMPLEMENT_API_CAPABILITY                DISABLED
#endif

#ifndef VSF_TIMER_CFG_REIMPLEMENT_API_PWM_SET
#   define  VSF_TIMER_CFG_REIMPLEMENT_API_PWM_SET                   DISABLED
#endif

#ifdef VSF_TIMER_CFG_IMP_REMAP_PREFIX
#   undef VSF_TIMER_CFG_REIMPLEMENT_API_CAPABILITY
#   undef VSF_TIMER_CFG_REIMPLEMENT_API_PWM_SET
#   define VSF_TIMER_CFG_REIMPLEMENT_API_CAPABILITY                 ENABLED
#   define VSF_TIMER_CFG_REIMPLEMENT_API_PWM_SET                    ENABLED
#endif

#if VSF_TIMER_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
#   ifndef VSF_TIMER_CFG_CAPABILITY_IRQ_MASK
#       define VSF_TIMER_CFG_CAPABILITY_IRQ_MASK        VSF_TIMER_IRQ_ALL_BITS_MASK
#   endif
#   ifndef VSF_TIMER_CFG_CAPABILITY_TIMER_BITLEN
#       define VSF_TIMER_CFG_CAPABILITY_TIMER_BITLEN    32
#   endif
#   ifndef VSF_TIMER_CFG_CAPABILITY_SUPPORT_PWM
#       define VSF_TIMER_CFG_CAPABILITY_SUPPORT_PWM     1
#   endif
#   ifndef VSF_TIMER_CFG_CAPABILITY_PWM_CHANNEL_CNT
#       define VSF_TIMER_CFG_CAPABILITY_PWM_CHANNEL_CNT 32
#   endif
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

#define vsf_real_timer_t          VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_t)
#define vsf_real_timer_capability VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_capability)
#define vsf_real_timer_pwm_set    VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_pwm_set)

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_TIMER_CFG_REIMPLEMENT_API_PWM_SET == DISABLED
vsf_err_t vsf_real_timer_pwm_set(vsf_real_timer_t *timer_ptr, uint8_t channel, uint32_t period, uint32_t pulse)
{
    VSF_HAL_ASSERT(NULL != timer_ptr);

    VSF_HAL_ASSERT(0);
    return VSF_ERR_FAIL;
}
#endif

#if VSF_TIMER_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
vsf_timer_capability_t vsf_real_timer_capability(vsf_real_timer_t *timer_ptr)
{
    vsf_timer_capability_t timer_capability = {
        .irq_mask           = VSF_TIMER_CFG_CAPABILITY_IRQ_MASK,
        .timer_bitlen       = VSF_TIMER_CFG_CAPABILITY_TIMER_BITLEN,
        .support_pwm        = VSF_TIMER_CFG_CAPABILITY_SUPPORT_PWM,
        .channel_cnt        = VSF_TIMER_CFG_CAPABILITY_PWM_CHANNEL_CNT,
    };

    return timer_capability;
}
#endif

/*============================ MACROS ========================================*/

#undef VSF_TIMER_CFG_REIMPLEMENT_API_CAPABILITY
#undef VSF_TIMER_CFG_CAPABILITY_IRQ_MASK
#undef VSF_TIMER_CFG_CAPABILITY_TIMER_BITLEN
#undef VSF_TIMER_CFG_CAPABILITY_SUPPORT_PWM
#undef VSF_TIMER_CFG_CAPABILITY_PWM_CHANNEL_CNT
#undef vsf_real_timer_t
#undef vsf_real_timer_capability
#undef vsf_real_timer_pwm_set

/*============================ MACROS ========================================*/

#ifdef VSF_TIMER_CFG_IMP_REMAP_PREFIX

#   define vsf_imp_timer_t                  VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_t)
#   define vsf_imp_timer_init               VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_init)
#   define vsf_imp_timer_fini               VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_fini)
#   define vsf_imp_timer_enable             VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_enable)
#   define vsf_imp_timer_disable            VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_disable)
#   define vsf_imp_timer_capability         VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_capability)
#   define vsf_imp_timer_irq_enable         VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_irq_enable)
#   define vsf_imp_timer_irq_disable        VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_irq_disable)
#   define vsf_imp_timer_channel_config     VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_channel_config)
#   define vsf_imp_timer_channel_start      VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_channel_start)
#   define vsf_imp_timer_channel_stop       VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_channel_stop)
#   define vsf_imp_timer_ctrl               VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_ctrl)

#   define vsf_remap_timer_t                VSF_MCONNECT(VSF_TIMER_CFG_IMP_REMAP_PREFIX, _timer_t)
#   define vsf_remap_timer_init             VSF_MCONNECT(VSF_TIMER_CFG_IMP_REMAP_PREFIX, _timer_init)
#   define vsf_remap_timer_fini             VSF_MCONNECT(VSF_TIMER_CFG_IMP_REMAP_PREFIX, _timer_fini)
#   define vsf_remap_timer_enable           VSF_MCONNECT(VSF_TIMER_CFG_IMP_REMAP_PREFIX, _timer_enable)
#   define vsf_remap_timer_disable          VSF_MCONNECT(VSF_TIMER_CFG_IMP_REMAP_PREFIX, _timer_disable)
#   define vsf_remap_timer_capability       VSF_MCONNECT(VSF_TIMER_CFG_IMP_REMAP_PREFIX, _timer_capability)
#   define vsf_remap_timer_irq_enable       VSF_MCONNECT(VSF_TIMER_CFG_IMP_REMAP_PREFIX, _timer_irq_enable)
#   define vsf_remap_timer_irq_disable      VSF_MCONNECT(VSF_TIMER_CFG_IMP_REMAP_PREFIX, _timer_irq_disable)
#   define vsf_remap_timer_channel_config   VSF_MCONNECT(VSF_TIMER_CFG_IMP_REMAP_PREFIX, _timer_channel_config)
#   define vsf_remap_timer_channel_start    VSF_MCONNECT(VSF_TIMER_CFG_IMP_REMAP_PREFIX, _timer_channel_start)
#   define vsf_remap_timer_channel_stop     VSF_MCONNECT(VSF_TIMER_CFG_IMP_REMAP_PREFIX, _timer_channel_stop)
#   define vsf_remap_timer_ctrl             VSF_MCONNECT(VSF_TIMER_CFG_IMP_REMAP_PREFIX, _timer_ctrl)

#   define VSF_TIMER_CFG_IMP_REMAP_FUNCTIONS                                                                          \
        vsf_err_t vsf_imp_timer_init(vsf_imp_timer_t *timer_ptr, vsf_timer_cfg_t *cfg_ptr)                            \
        {                                                                                                             \
            VSF_HAL_ASSERT(timer_ptr != NULL);                                                                        \
            return vsf_remap_timer_init(timer_ptr, cfg_ptr);                                                          \
        }                                                                                                             \
        void vsf_imp_timer_fini(vsf_imp_timer_t *timer_ptr)                                                           \
        {                                                                                                             \
            VSF_HAL_ASSERT(timer_ptr != NULL);                                                                        \
            vsf_remap_timer_fini(timer_ptr);                                                                          \
        }                                                                                                             \
        fsm_rt_t vsf_imp_timer_enable(vsf_imp_timer_t *timer_ptr)                                                     \
        {                                                                                                             \
            VSF_HAL_ASSERT(timer_ptr != NULL);                                                                        \
            return vsf_remap_timer_enable(timer_ptr);                                                                 \
        }                                                                                                             \
        fsm_rt_t vsf_imp_timer_disable(vsf_imp_timer_t *timer_ptr)                                                    \
        {                                                                                                             \
            VSF_HAL_ASSERT(timer_ptr != NULL);                                                                        \
            return vsf_remap_timer_disable(timer_ptr);                                                                \
        }                                                                                                             \
        void vsf_imp_timer_irq_enable(vsf_imp_timer_t *timer_ptr, vsf_timer_irq_mask_t irq_mask)                      \
        {                                                                                                             \
            VSF_HAL_ASSERT(timer_ptr != NULL);                                                                        \
            vsf_remap_timer_irq_enable(timer_ptr, irq_mask);                                                          \
        }                                                                                                             \
        void vsf_imp_timer_irq_disable(vsf_imp_timer_t *timer_ptr, vsf_timer_irq_mask_t irq_mask)                     \
        {                                                                                                             \
            VSF_HAL_ASSERT(timer_ptr != NULL);                                                                        \
            vsf_remap_timer_irq_disable(timer_ptr, irq_mask);                                                         \
        }                                                                                                             \
        vsf_timer_capability_t vsf_imp_timer_capability(vsf_imp_timer_t *timer_ptr)                                   \
        {                                                                                                             \
            VSF_HAL_ASSERT(timer_ptr != NULL);                                                                        \
            return vsf_remap_timer_capability(timer_ptr);                                                             \
        }                                                                                                             \
        vsf_err_t vsf_imp_timer_pwm_set(vsf_imp_timer_t *timer_ptr, uint8_t channel, uint32_t period, uint32_t pulse) \
        {                                                                                                             \
            VSF_HAL_ASSERT(NULL != timer_ptr);                                                                        \
            return vsf_remap_timer_pwm_set(timer_ptr, channel, period, pulse);                                        \
        }                                                                                                             \
        vsf_err_t vsf_imp_timer_channel_config(vsf_imp_timer_t *timer_ptr, uint8_t channel,                           \
            vsf_timer_channel_cfg_t *cfg_ptr)                                                                         \
        {                                                                                                             \
            VSF_HAL_ASSERT(NULL != timer_ptr);                                                                        \
            return vsf_remap_timer_channel_config(timer_ptr, channel, cfg_ptr);                                       \
        }                                                                                                             \
        fsm_rt_t vsf_imp_timer_channel_start(vsf_imp_timer_t *timer_ptr, uint8_t channel)                             \
        {                                                                                                             \
            VSF_HAL_ASSERT(NULL != timer_ptr);                                                                        \
            return vsf_remap_timer_channel_start(timer_ptr, channel);                                                 \
        }                                                                                                             \
        fsm_rt_t vsf_imp_timer_channel_stop(vsf_imp_timer_t *timer_ptr, uint8_t channel)                              \
        {                                                                                                             \
            VSF_HAL_ASSERT(NULL != timer_ptr);                                                                        \
            return vsf_remap_timer_channel_stop(timer_ptr, channel);                                                  \
        }                                                                                                             \
        void vsf_imp_timer_ctrl(vsf_imp_timer_t *timer_ptr, uint_fast32_t ctrl_bit, bool level)                       \
        {                                                                                                             \
            VSF_HAL_ASSERT(NULL != timer_ptr);                                                                        \
            vsf_remap_timer_ctrl(timer_ptr, ctrl_bit, level);                                                         \
        }
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#define VSF_HAL_TEMPLATE_IMP_NAME                   _timer
#define VSF_HAL_TEMPLATE_IMP_UPCASE_NAME            _TIMER

#ifndef VSF_TIMER_CFG_IMP_PREFIX
#   error "Please define VSF_TIMER_CFG_IMP_PREFIX in timer driver"
#endif

#ifndef VSF_TIMER_CFG_IMP_UPCASE_PREFIX
#   error "Please define VSF_TIMER_CFG_IMP_UPCASE_PREFIX in timer driver"
#endif

#ifndef VSF_TIMER_CFG_IMP_COUNT_MASK_PREFIX
#   define VSF_TIMER_CFG_IMP_COUNT_MASK_PREFIX      VSF_TIMER_CFG_IMP_UPCASE_PREFIX
#endif

#ifdef VSF_TIMER_CFG_IMP_REMAP_FUNCTIONS
#   define VSF_HAL_CFG_IMP_REMAP_FUNCTIONS          VSF_TIMER_CFG_IMP_REMAP_FUNCTIONS
#endif

#include "hal/driver/common/template/vsf_template_instance_implementation.h"

#undef VSF_TIMER_CFG_IMP_PREFIX
#undef VSF_TIMER_CFG_IMP_COUNT_MASK_PREFIX
#undef VSF_TIMER_CFG_IMP_UPCASE_PREFIX
#undef VSF_TIMER_CFG_IMP_LV0
#undef VSF_TIMER_CFG_IMP_REMAP_FUNCTIONS
#undef VSF_TIMER_CFG_IMP_HAS_OP
#undef VSF_TIMER_CFG_IMP_EXTERN_OP

#undef vsf_imp_timer_t
#undef vsf_imp_timer_init
#undef vsf_imp_timer_fini
#undef vsf_imp_timer_enable
#undef vsf_imp_timer_disable
#undef vsf_imp_timer_capability
#undef vsf_imp_timer_irq_enable
#undef vsf_imp_timer_irq_disable
#undef vsf_imp_timer_channel_config
#undef vsf_imp_timer_channel_start
#undef vsf_imp_timer_channel_stop
#undef vsf_imp_timer_ctrl

#undef vsf_remap_timer_t
#undef vsf_remap_timer_init
#undef vsf_remap_timer_fini
#undef vsf_remap_timer_enable
#undef vsf_remap_timer_disable
#undef vsf_remap_timer_capability
#undef vsf_remap_timer_irq_enable
#undef vsf_remap_timer_irq_disable
#undef vsf_remap_timer_channel_config
#undef vsf_remap_timer_channel_start
#undef vsf_remap_timer_channel_stop
#undef vsf_remap_timer_ctrl

#endif  /* VSF_HAL_USE_TIEMR */
