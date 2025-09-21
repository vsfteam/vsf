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

#if VSF_HAL_USE_PWM == ENABLED

/*============================ INCLUDES ======================================*/

/*============================ MACROS ========================================*/

#ifndef VSF_PWM_CFG_REIMPLEMENT_API_GET_CONFIGURATION
#   define VSF_PWM_CFG_REIMPLEMENT_API_GET_CONFIGURATION        DISABLED
#endif

#ifndef VSF_PWM_CFG_REIMPLEMENT_API_CAPABILITY
#   define  VSF_PWM_CFG_REIMPLEMENT_API_CAPABILITY              DISABLED
#endif

#ifdef VSF_PWM_CFG_IMP_REMAP_PREFIX
#   undef VSF_PWM_CFG_REIMPLEMENT_API_CAPABILITY
#   define VSF_PWM_CFG_REIMPLEMENT_API_CAPABILITY               ENABLED
#   undef VSF_PWM_CFG_REIMPLEMENT_API_GET_CONFIGURATION
#   define VSF_PWM_CFG_REIMPLEMENT_API_GET_CONFIGURATION        ENABLED
#endif

#if VSF_PWM_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
#   ifndef VSF_PWM_CFG_CAPABILITY_MIN_FREQ
#       define VSF_PWM_CFG_CAPABILITY_MIN_FREQ      (1ul * 1000)
#   endif
#   ifndef VSF_PWM_CFG_CAPABILITY_MAX_FREQ
#       define VSF_PWM_CFG_CAPABILITY_MAX_FREQ      (10ul * 1000 * 1000)
#   endif
#endif

#define vsf_real_pwm_t          VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_t)
#define vsf_real_pwm_set        VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_set)
#define vsf_real_pwm_get_freq   VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_get_freq)
#define vsf_real_pwm_capability VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_capability)
#define vsf_real_pwm_get_configuration      VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_get_configuration)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_PWM_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
vsf_pwm_capability_t vsf_real_pwm_capability(vsf_real_pwm_t *pwm_ptr)
{
    vsf_pwm_capability_t pwm_capability = {
        .max_freq = VSF_PWM_CFG_CAPABILITY_MAX_FREQ,
        .min_freq = VSF_PWM_CFG_CAPABILITY_MIN_FREQ,
    };

    return pwm_capability;
}
#endif

#if VSF_PWM_CFG_REIMPLEMENT_API_GET_CONFIGURATION == DISABLED
vsf_err_t vsf_real_pwm_get_configuration(vsf_real_pwm_t *pwm_ptr, vsf_pwm_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(NULL != pwm_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);
    VSF_HAL_ASSERT(0); // Default implementation: not supported, trigger assertion
    return VSF_ERR_NOT_SUPPORT;
}
#endif

/*============================ MACROS ========================================*/

#undef VSF_PWM_CFG_REIMPLEMENT_API_CAPABILITY
#undef VSF_PWM_CFG_REIMPLEMENT_API_GET_CONFIGURATION
#undef VSF_PWM_CFG_CAPABILITY_MAX_FREQ
#undef VSF_PWM_CFG_CAPABILITY_MIN_FREQ
#undef vsf_real_pwm_t
#undef vsf_real_pwm_capability
#undef vsf_real_pwm_get_configuration

/*============================ MACROS ========================================*/

#ifdef VSF_PWM_CFG_IMP_REMAP_PREFIX

#   define vsf_imp_pwm_t            VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_t)
#   define vsf_imp_pwm_init         VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_init)
#   define vsf_imp_pwm_fini         VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_fini)
#   define vsf_imp_pwm_enable       VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_enable)
#   define vsf_imp_pwm_disable      VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_disable)
#   define vsf_imp_pwm_capability   VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_capability)
#   define vsf_imp_pwm_set          VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_set)
#   define vsf_imp_pwm_get_freq     VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_get_freq)
#   define vsf_imp_pwm_get_configuration    VSF_MCONNECT(VSF_PWM_CFG_IMP_PREFIX, _pwm_get_configuration)

#   define vsf_remap_pwm_t          VSF_MCONNECT(VSF_PWM_CFG_IMP_REMAP_PREFIX, _pwm_t)
#   define vsf_remap_pwm_init       VSF_MCONNECT(VSF_PWM_CFG_IMP_REMAP_PREFIX, _pwm_init)
#   define vsf_remap_pwm_fini       VSF_MCONNECT(VSF_PWM_CFG_IMP_REMAP_PREFIX, _pwm_fini)
#   define vsf_remap_pwm_enable     VSF_MCONNECT(VSF_PWM_CFG_IMP_REMAP_PREFIX, _pwm_enable)
#   define vsf_remap_pwm_disable    VSF_MCONNECT(VSF_PWM_CFG_IMP_REMAP_PREFIX, _pwm_disable)
#   define vsf_remap_pwm_capability VSF_MCONNECT(VSF_PWM_CFG_IMP_REMAP_PREFIX, _pwm_capability)
#   define vsf_remap_pwm_set        VSF_MCONNECT(VSF_PWM_CFG_IMP_REMAP_PREFIX, _pwm_set)
#   define vsf_remap_pwm_get_freq   VSF_MCONNECT(VSF_PWM_CFG_IMP_REMAP_PREFIX, _pwm_get_freq)
#   define vsf_remap_pwm_get_configuration  VSF_MCONNECT(VSF_PWM_CFG_IMP_REMAP_PREFIX, _pwm_get_configuration)

#   define VSF_PWM_CFG_IMP_REMAP_FUNCTIONS                                                       \
        vsf_err_t vsf_imp_pwm_init(vsf_imp_pwm_t *pwm_ptr, vsf_pwm_cfg_t *cfg_ptr) \
        {                                                                          \
            VSF_HAL_ASSERT(pwm_ptr != NULL);                                       \
            return vsf_remap_pwm_init(pwm_ptr, cfg_ptr);                           \
        }                                                                          \
        void vsf_imp_pwm_fini(vsf_imp_pwm_t *pwm_ptr)                              \
        {                                                                          \
            VSF_HAL_ASSERT(pwm_ptr != NULL);                                       \
            vsf_remap_pwm_fini(pwm_ptr);                                           \
        }                                                                          \
        fsm_rt_t vsf_imp_pwm_enable(vsf_imp_pwm_t *pwm_ptr)                        \
        {                                                                          \
            VSF_HAL_ASSERT(pwm_ptr != NULL);                                       \
            return vsf_remap_pwm_enable(pwm_ptr);                                  \
        }                                                                          \
        fsm_rt_t vsf_imp_pwm_disable(vsf_imp_pwm_t *pwm_ptr)                       \
        {                                                                          \
            VSF_HAL_ASSERT(pwm_ptr != NULL);                                       \
            return vsf_remap_pwm_disable(pwm_ptr);                                 \
        }                                                                          \
        vsf_pwm_capability_t vsf_imp_pwm_capability(vsf_imp_pwm_t *pwm_ptr)        \
        {                                                                          \
            VSF_HAL_ASSERT(pwm_ptr != NULL);                                       \
            return vsf_remap_pwm_capability(pwm_ptr);                              \
        }                                                                          \
        vsf_err_t vsf_imp_pwm_set(vsf_imp_pwm_t *pwm_ptr, uint8_t channel,         \
                                  uint32_t period, uint32_t pulse)                 \
        {                                                                          \
            VSF_HAL_ASSERT(pwm_ptr != NULL);                                       \
            return vsf_remap_pwm_set(pwm_ptr, channel, period, pulse);             \
        }                                                                          \
        uint32_t vsf_imp_pwm_get_freq(vsf_imp_pwm_t *pwm_ptr)                      \
        {                                                                          \
            VSF_HAL_ASSERT(pwm_ptr != NULL);                                       \
            return vsf_remap_pwm_get_freq(pwm_ptr);                                \
        }                                                                          \
                                                                                   \
        vsf_err_t vsf_imp_pwm_get_configuration(vsf_imp_pwm_t *pwm_ptr, vsf_pwm_cfg_t *cfg_ptr) \
        {                                                                          \
            VSF_HAL_ASSERT(pwm_ptr != NULL);                                       \
            return vsf_remap_pwm_get_configuration(pwm_ptr, cfg_ptr);              \
        }
#endif


/*============================ GLOBAL VARIABLES ==============================*/

#define VSF_HAL_TEMPLATE_IMP_NAME                   _pwm
#define VSF_HAL_TEMPLATE_IMP_UPCASE_NAME            _PWM

#if !defined(VSF_PWM_CFG_IMP_PREFIX) && !defined(VSF_PWM_CFG_IMP_DEVICE_PREFIX)
#   error "Please define VSF_PWM_CFG_IMP_PREFIX in pwm driver"
#endif

#if !defined(VSF_PWM_CFG_IMP_UPCASE_PREFIX) && !defined(VSF_PWM_CFG_IMP_DEVICE_UPCASE_PREFIX)
#   error "Please define VSF_PWM_CFG_IMP_UPCASE_PREFIX in pwm driver"
#endif

#ifndef VSF_PWM_CFG_IMP_COUNT_MASK_PREFIX
#   define VSF_PWM_CFG_IMP_COUNT_MASK_PREFIX        VSF_PWM_CFG_IMP_UPCASE_PREFIX
#endif

#ifdef VSF_PWM_CFG_IMP_REMAP_FUNCTIONS
#   define VSF_HAL_CFG_IMP_REMAP_FUNCTIONS          VSF_PWM_CFG_IMP_REMAP_FUNCTIONS
#endif

#include "hal/driver/common/template/vsf_template_instance_implementation.h"

#undef VSF_PWM_CFG_IMP_PREFIX
#undef VSF_PWM_CFG_IMP_COUNT_MASK_PREFIX
#undef VSF_PWM_CFG_IMP_UPCASE_PREFIX
#undef VSF_PWM_CFG_IMP_DEVICE_PREFIX
#undef VSF_PWM_CFG_IMP_DEVICE_UPCASE_PREFIX
#undef VSF_PWM_CFG_IMP_LV0
#undef VSF_PWM_CFG_IMP_REMAP_FUNCTIONS
#undef VSF_PWM_CFG_IMP_HAS_OP
#undef VSF_PWM_CFG_IMP_EXTERN_OP

#undef vsf_imp_pwm_t
#undef vsf_imp_pwm_init
#undef vsf_imp_pwm_fini
#undef vsf_imp_pwm_enable
#undef vsf_imp_pwm_disable
#undef vsf_imp_pwm_capability
#undef vsf_imp_pwm_set
#undef vsf_imp_pwm_get_freq
#undef vsf_imp_pwm_get_configuration

#undef vsf_remap_pwm_t
#undef vsf_remap_pwm_init
#undef vsf_remap_pwm_fini
#undef vsf_remap_pwm_enable
#undef vsf_remap_pwm_disable
#undef vsf_remap_pwm_capability
#undef vsf_remap_pwm_set
#undef vsf_remap_pwm_get_freq
#undef vsf_remap_pwm_get_configuration

#endif  /* VSF_HAL_USE_PWM */
