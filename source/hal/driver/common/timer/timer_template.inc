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

#ifndef VSF_TIMER_CFG_REIMPLEMENT_API_GET_CONFIGURATION
#   define VSF_TIMER_CFG_REIMPLEMENT_API_GET_CONFIGURATION        DISABLED
#endif
#   define  VSF_TIMER_CFG_REIMPLEMENT_API_CAPABILITY                DISABLED
#endif

#ifndef VSF_TIMER_CFG_REIMPLEMENT_API_IRQ_CLEAR
#   define VSF_TIMER_CFG_REIMPLEMENT_API_IRQ_CLEAR                DISABLED
#endif

#ifndef VSF_TIMER_CFG_REIMPLEMENT_API_CTRL
#   define  VSF_TIMER_CFG_REIMPLEMENT_API_CTRL                      DISABLED
#endif

#ifndef VSF_TIMER_CFG_REIMPLEMENT_API_STATUS
#   define  VSF_TIMER_CFG_REIMPLEMENT_API_STATUS                    DISABLED
#endif


#ifndef VSF_TIMER_CFG_REIMPLEMENT_API_CHANNEL
#   define  VSF_TIMER_CFG_REIMPLEMENT_API_CHANNEL                   DISABLED
#endif

#ifndef VSF_TIMER_CFG_REIMPLEMENT_API_CHANNEL_CONFIG
#   define  VSF_TIMER_CFG_REIMPLEMENT_API_CHANNEL_CONFIG            VSF_TIMER_CFG_REIMPLEMENT_API_CHANNEL
#endif

#ifndef VSF_TIMER_CFG_REIMPLEMENT_API_CHANNEL_START
#   define  VSF_TIMER_CFG_REIMPLEMENT_API_CHANNEL_START             VSF_TIMER_CFG_REIMPLEMENT_API_CHANNEL
#endif

#ifndef VSF_TIMER_CFG_REIMPLEMENT_API_CHANNEL_STOP
#   define  VSF_TIMER_CFG_REIMPLEMENT_API_CHANNEL_STOP              VSF_TIMER_CFG_REIMPLEMENT_API_CHANNEL
#endif

#ifndef VSF_TIMER_CFG_REIMPLEMENT_API_CHANNEL_REQUEST_START
#   define  VSF_TIMER_CFG_REIMPLEMENT_API_CHANNEL_REQUEST_START     VSF_TIMER_CFG_REIMPLEMENT_API_CHANNEL
#endif

#ifndef VSF_TIMER_CFG_REIMPLEMENT_API_CHANNEL_REQUEST_STOP
#   define  VSF_TIMER_CFG_REIMPLEMENT_API_CHANNEL_REQUEST_STOP      VSF_TIMER_CFG_REIMPLEMENT_API_CHANNEL
#endif

#ifndef VSF_TIMER_CFG_REIMPLEMENT_API_CHANNEL_CTRL
#   define  VSF_TIMER_CFG_REIMPLEMENT_API_CHANNEL_CTRL              VSF_TIMER_CFG_REIMPLEMENT_API_CHANNEL
#endif

#ifdef VSF_TIMER_CFG_IMP_REMAP_PREFIX
#   undef VSF_TIMER_CFG_REIMPLEMENT_API_CAPABILITY
#   undef VSF_TIMER_CFG_REIMPLEMENT_API_CTRL
#   undef VSF_TIMER_CFG_REIMPLEMENT_API_STATUS
#   undef VSF_TIMER_CFG_REIMPLEMENT_API_CHANNEL_CONFIG
#   undef VSF_TIMER_CFG_REIMPLEMENT_API_CHANNEL_START
#   undef VSF_TIMER_CFG_REIMPLEMENT_API_CHANNEL_STOP
#   undef VSF_TIMER_CFG_REIMPLEMENT_API_CHANNEL_REQUEST_START
#   undef VSF_TIMER_CFG_REIMPLEMENT_API_CHANNEL_REQUEST_STOP
#   undef VSF_TIMER_CFG_REIMPLEMENT_API_CHANNEL_CTRL
#   define VSF_TIMER_CFG_REIMPLEMENT_API_CAPABILITY                 ENABLED
#   undef VSF_TIMER_CFG_REIMPLEMENT_API_GET_CONFIGURATION
#   define VSF_TIMER_CFG_REIMPLEMENT_API_GET_CONFIGURATION        ENABLED
#   define VSF_TIMER_CFG_REIMPLEMENT_API_CTRL                       ENABLED
#   define VSF_TIMER_CFG_REIMPLEMENT_API_STATUS                 ENABLED
#   define VSF_TIMER_CFG_REIMPLEMENT_API_CHANNEL_CONFIG             ENABLED
#   define VSF_TIMER_CFG_REIMPLEMENT_API_CHANNEL_START              ENABLED
#   define VSF_TIMER_CFG_REIMPLEMENT_API_CHANNEL_STOP               ENABLED
#   define VSF_TIMER_CFG_REIMPLEMENT_API_CHANNEL_REQUEST_START      ENABLED
#   define VSF_TIMER_CFG_REIMPLEMENT_API_CHANNEL_REQUEST_STOP       ENABLED
#   define VSF_TIMER_CFG_REIMPLEMENT_API_CHANNEL_CTRL               ENABLED
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

#define vsf_real_timer_t                        VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_t)
#define vsf_real_timer_capability               VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_capability)
#define vsf_real_timer_get_configuration      VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_get_configuration)
#define vsf_real_timer_irq_clear                 VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_irq_clear)
#define vsf_real_timer_ctrl                     VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_ctrl)
#define vsf_real_timer_status                   VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_status)
#define vsf_real_timer_channel_config           VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_channel_config)
#define vsf_real_timer_channel_start            VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_channel_start)
#define vsf_real_timer_channel_stop             VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_channel_stop)
#define vsf_real_timer_channel_request_start    VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_channel_request_start)
#define vsf_real_timer_channel_request_stop     VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_channel_request_stop)
#define vsf_real_timer_channel_ctrl             VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_channel_ctrl)

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

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

#if VSF_TIMER_CFG_REIMPLEMENT_API_CTRL == DISABLED
vsf_err_t vsf_real_timer_ctrl(vsf_real_timer_t *timer_ptr, vsf_timer_ctrl_t ctrl, void *param)
{
    VSF_HAL_ASSERT(NULL != timer_ptr);

    return VSF_ERR_NOT_SUPPORT;
}
#endif

#if VSF_TIMER_CFG_REIMPLEMENT_API_STATUS == DISABLED
vsf_timer_status_t vsf_real_timer_status(vsf_real_timer_t *timer_ptr)
{
    VSF_HAL_ASSERT(NULL != timer_ptr);
    VSF_HAL_ASSERT(0);

    return (vsf_timer_status_t){
        .value = 0,
    };
}
#endif

#if VSF_TIMER_CFG_REIMPLEMENT_API_CHANNEL_CONFIG == DISABLED
vsf_err_t vsf_real_timer_channel_config(vsf_real_timer_t *timer_ptr, uint8_t channel, vsf_timer_channel_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(NULL != timer_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);

    return VSF_ERR_NOT_SUPPORT;
}
#endif

#if VSF_TIMER_CFG_REIMPLEMENT_API_CHANNEL_START == DISABLED
vsf_err_t vsf_real_timer_channel_start(vsf_real_timer_t *timer_ptr, uint8_t channel)
{
    VSF_HAL_ASSERT(NULL != timer_ptr);

    return VSF_ERR_NOT_SUPPORT;
}
#endif

#if VSF_TIMER_CFG_REIMPLEMENT_API_CHANNEL_STOP == DISABLED
vsf_err_t vsf_real_timer_channel_stop(vsf_real_timer_t *timer_ptr, uint8_t channel)
{
    VSF_HAL_ASSERT(NULL != timer_ptr);

    return VSF_ERR_NOT_SUPPORT;
}
#endif

#if VSF_TIMER_CFG_REIMPLEMENT_API_CHANNEL_REQUEST_START == DISABLED
vsf_err_t vsf_real_timer_channel_request_start(vsf_real_timer_t *timer_ptr, uint8_t channel, vsf_timer_channel_request_t *request_ptr)
{
    VSF_HAL_ASSERT((NULL != timer_ptr) && (NULL != request_ptr));

    return VSF_ERR_NOT_SUPPORT;
}
#endif

#if VSF_TIMER_CFG_REIMPLEMENT_API_CHANNEL_REQUEST_STOP == DISABLED
vsf_err_t vsf_real_timer_channel_request_stop(vsf_real_timer_t *timer_ptr, uint8_t channel)
{
    VSF_HAL_ASSERT(NULL != timer_ptr);

    return VSF_ERR_NOT_SUPPORT;
}
#endif

#if VSF_TIMER_CFG_REIMPLEMENT_API_CHANNEL_CTRL == DISABLED
vsf_err_t vsf_real_timer_channel_ctrl(vsf_real_timer_t *timer_ptr, uint8_t channel, vsf_timer_channel_ctrl_t ctrl, void *param)
{
    VSF_HAL_ASSERT(NULL != timer_ptr);

    return VSF_ERR_NOT_SUPPORT;
}
#endif

#if VSF_TIMER_CFG_REIMPLEMENT_API_GET_CONFIGURATION == DISABLED
vsf_err_t vsf_real_timer_get_configuration(vsf_real_timer_t *timer_ptr, vsf_timer_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(NULL != timer_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);

    // Default implementation: not supported, trigger assertion
    VSF_HAL_ASSERT(0);

    return VSF_ERR_NOT_SUPPORT;
}
#endif

#if VSF_TIMER_CFG_REIMPLEMENT_API_IRQ_CLEAR == DISABLED
vsf_timer_irq_mask_t vsf_real_timer_irq_clear(vsf_real_timer_t *timer_ptr, vsf_timer_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != timer_ptr);

    // Default implementation: not supported, trigger assertion
    VSF_HAL_ASSERT(0);

    return 0;
}
#endif

/*============================ MACROS ========================================*/

#undef VSF_TIMER_CFG_REIMPLEMENT_TYPE_CAPABILITY
#undef VSF_TIMER_CFG_REIMPLEMENT_TYPE_CFG
#undef VSF_TIMER_CFG_REIMPLEMENT_TYPE_CHANNEL_CFG
#undef VSF_TIMER_CFG_REIMPLEMENT_TYPE_CHANNEL_CTRL
#undef VSF_TIMER_CFG_REIMPLEMENT_TYPE_CHANNEL_MODE
#undef VSF_TIMER_CFG_REIMPLEMENT_TYPE_CHANNEL_REQUEST
#undef VSF_TIMER_CFG_REIMPLEMENT_TYPE_CTRL
#undef VSF_TIMER_CFG_REIMPLEMENT_TYPE_IRQ_MASK
#undef VSF_TIMER_CFG_REIMPLEMENT_TYPE_STATUS
#undef VSF_TIMER_CFG_REIMPLEMENT_API_CAPABILITY
#undef VSF_TIMER_CFG_REIMPLEMENT_API_GET_CONFIGURATION
#undef VSF_TIMER_CFG_CAPABILITY_IRQ_MASK
#undef VSF_TIMER_CFG_CAPABILITY_TIMER_BITLEN
#undef VSF_TIMER_CFG_CAPABILITY_SUPPORT_PWM
#undef VSF_TIMER_CFG_CAPABILITY_PWM_CHANNEL_CNT
#undef vsf_real_timer_t
#undef vsf_real_timer_capability
#undef vsf_real_timer_get_configuration
#undef vsf_real_timer_irq_clear
#undef vsf_real_timer_ctrl
#undef vsf_real_timer_status
#undef vsf_real_timer_channel_config
#undef vsf_real_timer_channel_start
#undef vsf_real_timer_channel_stop
#undef vsf_real_timer_channel_request_start
#undef vsf_real_timer_channel_request_stop
#undef vsf_real_timer_channel_ctrl

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
#   define vsf_imp_timer_irq_clear          VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_irq_clear)
#   define vsf_imp_timer_channel_config     VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_channel_config)
#   define vsf_imp_timer_channel_start      VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_channel_start)
#   define vsf_imp_timer_channel_stop       VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_channel_stop)
#   define vsf_imp_timer_channel_ctrl       VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_channel_ctrl)
#   define vsf_imp_timer_ctrl               VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_ctrl)
#   define vsf_imp_timer_get_configuration  VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_get_configuration)

#   define vsf_remap_timer_t                VSF_MCONNECT(VSF_TIMER_CFG_IMP_REMAP_PREFIX, _timer_t)
#   define vsf_remap_timer_init             VSF_MCONNECT(VSF_TIMER_CFG_IMP_REMAP_PREFIX, _timer_init)
#   define vsf_remap_timer_fini             VSF_MCONNECT(VSF_TIMER_CFG_IMP_REMAP_PREFIX, _timer_fini)
#   define vsf_remap_timer_enable           VSF_MCONNECT(VSF_TIMER_CFG_IMP_REMAP_PREFIX, _timer_enable)
#   define vsf_remap_timer_disable          VSF_MCONNECT(VSF_TIMER_CFG_IMP_REMAP_PREFIX, _timer_disable)
#   define vsf_remap_timer_capability       VSF_MCONNECT(VSF_TIMER_CFG_IMP_REMAP_PREFIX, _timer_capability)
#   define vsf_remap_timer_irq_enable       VSF_MCONNECT(VSF_TIMER_CFG_IMP_REMAP_PREFIX, _timer_irq_enable)
#   define vsf_remap_timer_irq_disable      VSF_MCONNECT(VSF_TIMER_CFG_IMP_REMAP_PREFIX, _timer_irq_disable)
#   define vsf_remap_timer_irq_clear        VSF_MCONNECT(VSF_TIMER_CFG_IMP_REMAP_PREFIX, _timer_irq_clear)
#   define vsf_remap_timer_channel_config   VSF_MCONNECT(VSF_TIMER_CFG_IMP_REMAP_PREFIX, _timer_channel_config)
#   define vsf_remap_timer_channel_start    VSF_MCONNECT(VSF_TIMER_CFG_IMP_REMAP_PREFIX, _timer_channel_start)
#   define vsf_remap_timer_channel_stop     VSF_MCONNECT(VSF_TIMER_CFG_IMP_REMAP_PREFIX, _timer_channel_stop)
#   define vsf_remap_timer_channel_ctrl     VSF_MCONNECT(VSF_TIMER_CFG_IMP_REMAP_PREFIX, _timer_channel_ctrl)
#   define vsf_remap_timer_ctrl             VSF_MCONNECT(VSF_TIMER_CFG_IMP_REMAP_PREFIX, _timer_ctrl)
#   define vsf_remap_timer_get_configuration VSF_MCONNECT(VSF_TIMER_CFG_IMP_REMAP_PREFIX, _timer_get_configuration)

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
        vsf_timer_irq_mask_t vsf_imp_timer_irq_clear(vsf_imp_timer_t *timer_ptr, vsf_timer_irq_mask_t irq_mask)        \
        {                                                                                                             \
            VSF_HAL_ASSERT(timer_ptr != NULL);                                                                        \
            return vsf_remap_timer_irq_clear(timer_ptr, irq_mask);                                                     \
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
        vsf_err_t vsf_imp_timer_channel_ctrl(vsf_imp_timer_t *timer_ptr, uint8_t channel,                             \
                                             vsf_timer_channel_ctrl_t ctrl, void *param)                              \
        {                                                                                                             \
            VSF_HAL_ASSERT(NULL != timer_ptr);                                                                        \
            return vsf_remap_timer_channel_ctrl(timer_ptr, channel, ctrl, param);                                     \
        }                                                                                                             \
        void vsf_imp_timer_ctrl(vsf_imp_timer_t *timer_ptr, uint_fast32_t ctrl_bit, bool level)                       \
        {                                                                                                             \
            VSF_HAL_ASSERT(NULL != timer_ptr);                                                                        \
            vsf_remap_timer_ctrl(timer_ptr, ctrl_bit, level);                                                         \
        }                                                                                                             \
                                                                                                                      \
        vsf_err_t vsf_imp_timer_get_configuration(vsf_imp_timer_t *timer_ptr, vsf_timer_cfg_t *cfg_ptr)               \
        {                                                                                                             \
            VSF_HAL_ASSERT(timer_ptr != NULL);                                                                        \
            return vsf_remap_timer_get_configuration(timer_ptr, cfg_ptr);                                             \
        }
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#define VSF_HAL_TEMPLATE_IMP_NAME                   _timer
#define VSF_HAL_TEMPLATE_IMP_UPCASE_NAME            _TIMER

#if !defined(VSF_TIMER_CFG_IMP_PREFIX) && !defined(VSF_TIMER_CFG_IMP_DEVICE_PREFIX)
#   error "Please define VSF_TIMER_CFG_IMP_PREFIX in timer driver"
#endif

#if !defined(VSF_TIMER_CFG_IMP_UPCASE_PREFIX) && !defined(VSF_TIMER_CFG_IMP_DEVICE_UPCASE_PREFIX)
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
#undef VSF_TIMER_CFG_IMP_DEVICE_PREFIX
#undef VSF_TIMER_CFG_IMP_DEVICE_UPCASE_PREFIX
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
#undef vsf_imp_timer_channel_ctrl
#undef vsf_imp_timer_ctrl
#undef vsf_imp_timer_get_configuration

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
#undef vsf_remap_timer_channel_ctrl
#undef vsf_remap_timer_ctrl
#undef vsf_remap_timer_get_configuration

#undef VSF_HAL_TEMPLATE_IMP_NAME
#undef VSF_HAL_TEMPLATE_IMP_UPCASE_NAME

/*============================ STATIC ASSERTIONS ==============================*/

/* User-extensible macros for custom mode and IRQ mask values
 * Users can define these macros before including this template to append
 * their custom enum values to the uniqueness checks.
 *
 * Check modes for custom values:
 *   VSF_TIMER_CFG_MODE_CHECK_UNIQUE - Check mode for custom mode values
 *     Default: VSF_HAL_CHECK_MODE_LOOSE (loose mode, no overlapping bits)
 *     Can be set to: VSF_HAL_CHECK_MODE_STRICT (strict mode, no overlapping bits)
 *   VSF_TIMER_CFG_IRQ_MASK_CHECK_UNIQUE - Check mode for custom IRQ mask values
 *     Default: VSF_HAL_CHECK_MODE_STRICT (strict mode, no overlapping bits)
 *     Can be set to: VSF_HAL_CHECK_MODE_LOOSE (loose mode, no overlapping bits)
 *
 * Example usage in vendor driver:
 *   #define VSF_TIMER_CUSTOM_MODE_VALUES           MY_CUSTOM_MODE1, MY_CUSTOM_MODE2
 *   #define VSF_TIMER_CFG_MODE_CHECK_UNIQUE        VSF_HAL_CHECK_MODE_LOOSE
 *   #define VSF_TIMER_CUSTOM_IRQ_MASK_VALUES       MY_CUSTOM_IRQ1, MY_CUSTOM_IRQ2
 *   #define VSF_TIMER_CFG_IRQ_MASK_CHECK_UNIQUE    VSF_HAL_CHECK_MODE_STRICT
 *   #include "hal/driver/common/timer/timer_template.inc"
 */


#ifdef VSF_TIMER_CFG_MODE_CHECK_UNIQUE
/* Default mode lists - can be redefined by users before including this file */
#ifndef VSF_TIMER_CHECK_UNIQUE_CHANNEL_MODE_MODES
#   define VSF_TIMER_CHECK_UNIQUE_CHANNEL_MODE_MODES \
        VSF_TIMER_CHANNEL_MODE_BASE
#endif

#ifndef VSF_TIMER_CHECK_UNIQUE_BASE_COUNTER_MODES
#   define VSF_TIMER_CHECK_UNIQUE_BASE_COUNTER_MODES \
        VSF_TIMER_BASE_ONESHOT, \
        VSF_TIMER_BASE_CONTINUES
#endif

/* ==================== GROUP INTERNAL UNIQUENESS CHECKS ==================== */
/* Check uniqueness within each functional group */

/* Check channel mode uniqueness - all channel modes should have different values
 * Mandatory: VSF_TIMER_CHANNEL_MODE_BASE
 * Standard Optional: VSF_TIMER_CHANNEL_MODE_PWM, VSF_TIMER_CHANNEL_MODE_OUTPUT_COMPARE,
 *                    VSF_TIMER_CHANNEL_MODE_INPUT_CAPTURE, VSF_TIMER_CHANNEL_MODE_ENCODER,
 *                    VSF_TIMER_CHANNEL_MODE_ONE_PULSE, VSF_TIMER_CHANNEL_MODE_HALL_SENSOR
 */
VSF_HAL_CHECK_UNIQUE_CUSTOM(
    VSF_TIMER_CFG_MODE_CHECK_UNIQUE,
    VSF_TIMER_CHANNEL_MODE_BASE
#if __VSF_TIMER_CFG_SUPPORT_STANDARD_OPTIONAL
#ifdef VSF_TIMER_CHANNEL_MODE_PWM
    , VSF_TIMER_CHANNEL_MODE_PWM
#endif
#ifdef VSF_TIMER_CHANNEL_MODE_OUTPUT_COMPARE
    , VSF_TIMER_CHANNEL_MODE_OUTPUT_COMPARE
#endif
#ifdef VSF_TIMER_CHANNEL_MODE_INPUT_CAPTURE
    , VSF_TIMER_CHANNEL_MODE_INPUT_CAPTURE
#endif
#ifdef VSF_TIMER_CHANNEL_MODE_ENCODER
    , VSF_TIMER_CHANNEL_MODE_ENCODER
#endif
#ifdef VSF_TIMER_CHANNEL_MODE_ONE_PULSE
    , VSF_TIMER_CHANNEL_MODE_ONE_PULSE
#endif
#ifdef VSF_TIMER_CHANNEL_MODE_HALL_SENSOR
    , VSF_TIMER_CHANNEL_MODE_HALL_SENSOR
#endif
#endif
);

/* Check base counter mode uniqueness - all base counter modes should have different values
 * Mandatory: VSF_TIMER_BASE_ONESHOT, VSF_TIMER_BASE_CONTINUES
 */
VSF_HAL_CHECK_UNIQUE_CUSTOM(
    VSF_TIMER_CFG_MODE_CHECK_UNIQUE,
    VSF_TIMER_BASE_ONESHOT,
    VSF_TIMER_BASE_CONTINUES
);

#ifdef VSF_TIMER_CUSTOM_MODE_VALUES
/* Check uniqueness among custom mode values using user-specified check mode */
VSF_HAL_CHECK_UNIQUE_CUSTOM(VSF_TIMER_CFG_MODE_CHECK_UNIQUE, VSF_TIMER_CUSTOM_MODE_VALUES);
#endif

VSF_HAL_CHECK_UNIQUE_CUSTOM(
    VSF_TIMER_CFG_MODE_CHECK_UNIQUE,
    /* Mandatory modes - always included */
    VSF_TIMER_CHANNEL_MODE_BASE,
    VSF_TIMER_BASE_ONESHOT, VSF_TIMER_BASE_CONTINUES
    /* Standard Optional modes - only included when __VSF_TIMER_CFG_SUPPORT_STANDARD_OPTIONAL is enabled */
#if __VSF_TIMER_CFG_SUPPORT_STANDARD_OPTIONAL
#ifdef VSF_TIMER_CHANNEL_MODE_PWM
    , VSF_TIMER_CHANNEL_MODE_PWM
#endif
#ifdef VSF_TIMER_CHANNEL_MODE_OUTPUT_COMPARE
    , VSF_TIMER_CHANNEL_MODE_OUTPUT_COMPARE
#endif
#ifdef VSF_TIMER_CHANNEL_MODE_INPUT_CAPTURE
    , VSF_TIMER_CHANNEL_MODE_INPUT_CAPTURE
#endif
#ifdef VSF_TIMER_CHANNEL_MODE_ENCODER
    , VSF_TIMER_CHANNEL_MODE_ENCODER
#endif
#ifdef VSF_TIMER_CHANNEL_MODE_ONE_PULSE
    , VSF_TIMER_CHANNEL_MODE_ONE_PULSE
#endif
#ifdef VSF_TIMER_CHANNEL_MODE_HALL_SENSOR
    , VSF_TIMER_CHANNEL_MODE_HALL_SENSOR
#endif
#ifdef VSF_TIMER_BASE_COUNTER_UP
    , VSF_TIMER_BASE_COUNTER_UP
#endif
#ifdef VSF_TIMER_BASE_COUNTER_DOWN
    , VSF_TIMER_BASE_COUNTER_DOWN
#endif
#ifdef VSF_TIMER_BASE_COUNTER_CENTER_ALIGNED1
    , VSF_TIMER_BASE_COUNTER_CENTER_ALIGNED1
#endif
#ifdef VSF_TIMER_BASE_COUNTER_CENTER_ALIGNED2
    , VSF_TIMER_BASE_COUNTER_CENTER_ALIGNED2
#endif
#ifdef VSF_TIMER_BASE_COUNTER_CENTER_ALIGNED3
    , VSF_TIMER_BASE_COUNTER_CENTER_ALIGNED3
#endif
#endif
    /* User-defined modes - appended by user configuration */
#ifdef VSF_TIMER_CUSTOM_MODE_VALUES
    , VSF_TIMER_CUSTOM_MODE_VALUES
#endif
);

#undef VSF_TIMER_CHECK_UNIQUE_CHANNEL_MODE_MODES
#undef VSF_TIMER_CHECK_UNIQUE_BASE_COUNTER_MODES
#endif  /* VSF_TIMER_CFG_MODE_CHECK_UNIQUE */

/* ==================== IRQ MASK UNIQUENESS CHECKS ==================== */

#ifdef VSF_TIMER_CFG_IRQ_MASK_CHECK_UNIQUE

#ifdef VSF_TIMER_CUSTOM_IRQ_MASK_VALUES
/* Check uniqueness among custom IRQ mask values using user-specified check mode */
VSF_HAL_CHECK_UNIQUE_CUSTOM(VSF_TIMER_CFG_IRQ_MASK_CHECK_UNIQUE, VSF_TIMER_CUSTOM_IRQ_MASK_VALUES);
#endif

/* Check uniqueness within vsf_timer_irq_mask_t enum */

/* Check IRQ mask uniqueness - all IRQ mask bits should have different values
 * Mandatory: VSF_TIMER_IRQ_MASK_OVERFLOW
 */
VSF_HAL_CHECK_UNIQUE_CUSTOM(
    VSF_TIMER_CFG_IRQ_MASK_CHECK_UNIQUE,
    /* Mandatory IRQ masks - always included */
    VSF_TIMER_IRQ_MASK_OVERFLOW
    /* User-defined IRQ masks - appended by user configuration */
#ifdef VSF_TIMER_CUSTOM_IRQ_MASK_VALUES
    , VSF_TIMER_CUSTOM_IRQ_MASK_VALUES
#endif
);
#endif  /* VSF_TIMER_CFG_IRQ_MASK_CHECK_UNIQUE */

/* ==================== CTRL UNIQUENESS CHECKS ==================== */

#ifdef VSF_TIMER_CFG_CTRL_CHECK_UNIQUE

#ifdef VSF_TIMER_CUSTOM_CTRL_VALUES
/* Check uniqueness among custom ctrl values using user-specified check mode */
VSF_HAL_CHECK_UNIQUE_CUSTOM(VSF_TIMER_CFG_CTRL_CHECK_UNIQUE, VSF_TIMER_CUSTOM_CTRL_VALUES);
#endif

/* Check ctrl uniqueness - all ctrl values should have different values
 * Placeholder: __VSF_TIMER_CTRL_DUMMY
 */
VSF_HAL_CHECK_UNIQUE_CUSTOM(
    VSF_TIMER_CFG_CTRL_CHECK_UNIQUE,
    /* Placeholder ctrl - always included */
    __VSF_TIMER_CTRL_DUMMY
    /* User-defined ctrls - appended by user configuration */
#ifdef VSF_TIMER_CUSTOM_CTRL_VALUES
    , VSF_TIMER_CUSTOM_CTRL_VALUES
#endif
);
#endif  /* VSF_TIMER_CFG_CTRL_CHECK_UNIQUE */

/* ==================== CHANNEL CTRL UNIQUENESS CHECKS ==================== */

#ifdef VSF_TIMER_CFG_CHANNEL_CTRL_CHECK_UNIQUE

#ifdef VSF_TIMER_CUSTOM_CHANNEL_CTRL_VALUES
/* Check uniqueness among custom channel ctrl values using user-specified check mode */
VSF_HAL_CHECK_UNIQUE_CUSTOM(VSF_TIMER_CFG_CHANNEL_CTRL_CHECK_UNIQUE, VSF_TIMER_CUSTOM_CHANNEL_CTRL_VALUES);
#endif

/* Check channel ctrl uniqueness - all channel ctrl values should have different values
 * Placeholder: __VSF_TIMER_CHANNEL_CTRL_DUMMY
 */
VSF_HAL_CHECK_UNIQUE_CUSTOM(
    VSF_TIMER_CFG_CHANNEL_CTRL_CHECK_UNIQUE,
    /* Placeholder channel ctrl - always included */
    __VSF_TIMER_CHANNEL_CTRL_DUMMY
    /* User-defined channel ctrls - appended by user configuration */
#ifdef VSF_TIMER_CUSTOM_CHANNEL_CTRL_VALUES
    , VSF_TIMER_CUSTOM_CHANNEL_CTRL_VALUES
#endif
);
#endif  /* VSF_TIMER_CFG_CHANNEL_CTRL_CHECK_UNIQUE */

#undef VSF_TIMER_CFG_MODE_CHECK_UNIQUE
#undef VSF_TIMER_CFG_IRQ_MASK_CHECK_UNIQUE
#undef VSF_TIMER_CFG_CTRL_CHECK_UNIQUE
#undef VSF_TIMER_CFG_CHANNEL_CTRL_CHECK_UNIQUE
#undef VSF_TIMER_CUSTOM_MODE_VALUES
#undef VSF_TIMER_CUSTOM_IRQ_MASK_VALUES
#undef VSF_TIMER_CUSTOM_CTRL_VALUES
#undef VSF_TIMER_CUSTOM_CHANNEL_CTRL_VALUES

#endif  /* VSF_HAL_USE_TIEMR */
