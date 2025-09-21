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

#if VSF_HAL_USE_ADC == ENABLED

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

#define vsf_real_adc_t                                          VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t)
#define vsf_real_adc_capability                                 VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_capability)
#define vsf_real_adc_get_configuration                          VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_get_configuration)

#ifndef VSF_ADC_CFG_REIMPLEMENT_API_CAPABILITY
#   define  VSF_ADC_CFG_REIMPLEMENT_API_CAPABILITY              DISABLED
#endif

#ifndef VSF_ADC_CFG_REIMPLEMENT_API_GET_CONFIGURATION
#   define VSF_ADC_CFG_REIMPLEMENT_API_GET_CONFIGURATION        DISABLED
#endif

#ifdef VSF_ADC_CFG_IMP_REMAP_PREFIX
#   undef VSF_ADC_CFG_REIMPLEMENT_API_CAPABILITY
#   define VSF_ADC_CFG_REIMPLEMENT_API_CAPABILITY               ENABLED
#   undef VSF_ADC_CFG_REIMPLEMENT_API_GET_CONFIGURATION
#   define VSF_ADC_CFG_REIMPLEMENT_API_GET_CONFIGURATION        ENABLED
#endif

#if VSF_ADC_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
#   ifndef VSF_ADC_CFG_CAPABILITY_IRQ_MASK
#       define VSF_ADC_CFG_CAPABILITY_IRQ_MASK                  VSF_ADC_IRQ_ALL_BITS_MASK
#   endif
#   ifndef VSF_ADC_CFG_CAPABILITY_MAX_DATA_BITS
#       define VSF_ADC_CFG_CAPABILITY_MAX_DATA_BITS             12
#   endif
#   ifndef VSF_ADC_CFG_CAPABILITY_CHANNEL_COUNT
#       define VSF_ADC_CFG_CAPABILITY_CHANNEL_COUNT             4
#   endif
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_ADC_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
vsf_adc_capability_t vsf_real_adc_capability(vsf_real_adc_t *adc_ptr)
{
    vsf_adc_capability_t adc_capability = {
        .irq_mask       = VSF_ADC_CFG_CAPABILITY_IRQ_MASK,
        .max_data_bits  = VSF_ADC_CFG_CAPABILITY_MAX_DATA_BITS,
        .channel_count  = VSF_ADC_CFG_CAPABILITY_CHANNEL_COUNT,
    };

    return adc_capability;
}
#endif

#if VSF_ADC_CFG_REIMPLEMENT_API_GET_CONFIGURATION == DISABLED
vsf_err_t vsf_real_adc_get_configuration(vsf_real_adc_t *adc_ptr, vsf_adc_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(NULL != adc_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);

    // Default implementation: not supported, trigger assertion
    VSF_HAL_ASSERT(0);

    return VSF_ERR_NOT_SUPPORT;
}
#endif

/*============================ MACROS ========================================*/

#undef VSF_ADC_CFG_REIMPLEMENT_API_CAPABILITY
#undef VSF_ADC_CFG_REIMPLEMENT_API_GET_CONFIGURATION
#undef VSF_ADC_CFG_CAPABILITY_IRQ_MASK
#undef VSF_ADC_CFG_CAPABILITY_MAX_DATA_BITS
#undef VSF_ADC_CFG_CAPABILITY_CHANNEL_COUNT

#undef vsf_real_adc_t
#undef vsf_real_adc_capability
#undef vsf_real_adc_get_configuration

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/
/*============================ MACROS ========================================*/

#ifdef VSF_ADC_CFG_IMP_REMAP_PREFIX
#   define vsf_imp_adc_t                        VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t)
#   define vsf_imp_adc_init                     VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_init)
#   define vsf_imp_adc_fini                     VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_fini)
#   define vsf_imp_adc_enable                   VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_enable)
#   define vsf_imp_adc_disable                  VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_disable)
#   define vsf_imp_adc_status                   VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_status)
#   define vsf_imp_adc_capability               VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_capability)
#   define vsf_imp_adc_irq_enable               VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_irq_enable)
#   define vsf_imp_adc_irq_disable              VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_irq_disable)
#   define vsf_imp_adc_channel_request_once     VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_channel_request_once)
#   define vsf_imp_adc_channel_config           VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_channel_config)
#   define vsf_imp_adc_channel_request          VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_channel_request)
#   define vsf_imp_adc_get_configuration        VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_get_configuration)

#   define vsf_remap_adc_t                      VSF_MCONNECT(VSF_ADC_CFG_IMP_REMAP_PREFIX, _adc_t)
#   define vsf_remap_adc_init                   VSF_MCONNECT(VSF_ADC_CFG_IMP_REMAP_PREFIX, _adc_init)
#   define vsf_remap_adc_fini                   VSF_MCONNECT(VSF_ADC_CFG_IMP_REMAP_PREFIX, _adc_fini)
#   define vsf_remap_adc_enable                 VSF_MCONNECT(VSF_ADC_CFG_IMP_REMAP_PREFIX, _adc_enable)
#   define vsf_remap_adc_disable                VSF_MCONNECT(VSF_ADC_CFG_IMP_REMAP_PREFIX, _adc_disable)
#   define vsf_remap_adc_status                 VSF_MCONNECT(VSF_ADC_CFG_IMP_REMAP_PREFIX, _adc_status)
#   define vsf_remap_adc_capability             VSF_MCONNECT(VSF_ADC_CFG_IMP_REMAP_PREFIX, _adc_capability)
#   define vsf_remap_adc_irq_enable             VSF_MCONNECT(VSF_ADC_CFG_IMP_REMAP_PREFIX, _adc_irq_enable)
#   define vsf_remap_adc_irq_disable            VSF_MCONNECT(VSF_ADC_CFG_IMP_REMAP_PREFIX, _adc_irq_disable)
#   define vsf_remap_adc_channel_request_once   VSF_MCONNECT(VSF_ADC_CFG_IMP_REMAP_PREFIX, _adc_channel_request_once)
#   define vsf_remap_adc_channel_config         VSF_MCONNECT(VSF_ADC_CFG_IMP_REMAP_PREFIX, _adc_channel_config)
#   define vsf_remap_adc_channel_request        VSF_MCONNECT(VSF_ADC_CFG_IMP_REMAP_PREFIX, _adc_channel_request)
#   define vsf_remap_adc_get_configuration      VSF_MCONNECT(VSF_ADC_CFG_IMP_REMAP_PREFIX, _adc_get_configuration)

#   define VSF_ADC_CFG_IMP_REMAP_FUNCTIONS                                                            \
        vsf_err_t vsf_imp_adc_init(vsf_imp_adc_t *adc, vsf_adc_cfg_t *cfg)              \
        {                                                                               \
            VSF_HAL_ASSERT(adc != NULL);                                                \
            return vsf_remap_adc_init((vsf_remap_adc_t *)adc, cfg);                     \
        }                                                                               \
                                                                                        \
        void vsf_imp_adc_fini(vsf_imp_adc_t *adc)                                       \
        {                                                                               \
            VSF_HAL_ASSERT(adc != NULL);                                                \
            vsf_remap_adc_fini((vsf_remap_adc_t *)adc);                                 \
        }                                                                               \
                                                                                        \
        vsf_adc_status_t vsf_imp_adc_status(vsf_imp_adc_t *adc)                         \
        {                                                                               \
            VSF_HAL_ASSERT(adc != NULL);                                                \
            return vsf_remap_adc_status((vsf_remap_adc_t *)adc);                        \
        }                                                                               \
                                                                                        \
        vsf_adc_capability_t vsf_imp_adc_capability(vsf_imp_adc_t *adc)                 \
        {                                                                               \
            VSF_HAL_ASSERT(adc != NULL);                                                \
            return vsf_remap_adc_capability((vsf_remap_adc_t *)adc);                    \
        }                                                                               \
                                                                                        \
        fsm_rt_t vsf_imp_adc_enable(vsf_imp_adc_t *adc)                                 \
        {                                                                               \
            VSF_HAL_ASSERT(adc != NULL);                                                \
            return vsf_remap_adc_enable((vsf_remap_adc_t *)adc);                        \
        }                                                                               \
                                                                                        \
        fsm_rt_t vsf_imp_adc_disable(vsf_imp_adc_t *adc)                                \
        {                                                                               \
            VSF_HAL_ASSERT(adc != NULL);                                                \
            return vsf_remap_adc_disable((vsf_remap_adc_t *)adc);                       \
        }                                                                               \
                                                                                        \
        void vsf_imp_adc_irq_enable(vsf_imp_adc_t *adc, vsf_adc_irq_mask_t irq_mask)    \
        {                                                                               \
            VSF_HAL_ASSERT(adc != NULL);                                                \
            vsf_remap_adc_irq_enable((vsf_remap_adc_t *)adc, irq_mask);                 \
        }                                                                               \
                                                                                        \
        void vsf_imp_adc_irq_disable(vsf_imp_adc_t *adc, vsf_adc_irq_mask_t irq_mask)   \
        {                                                                               \
            VSF_HAL_ASSERT(adc != NULL);                                                \
            vsf_remap_adc_irq_disable((vsf_remap_adc_t *)adc, irq_mask);                \
        }                                                                               \
                                                                                        \
        vsf_err_t vsf_imp_adc_channel_request_once(vsf_imp_adc_t *adc,                  \
                vsf_adc_channel_cfg_t *channel_cfg, void *buffer)                       \
        {                                                                               \
            VSF_HAL_ASSERT(adc != NULL);                                                \
            return vsf_remap_adc_channel_request_once((vsf_remap_adc_t *)adc,           \
                                                      channel_cfg, buffer);             \
        }                                                                               \
                                                                                        \
        vsf_err_t vsf_imp_adc_channel_config(vsf_imp_adc_t *adc,                        \
                vsf_adc_channel_cfg_t *channel_cfgs, uint32_t channel_cfgs_cnt)         \
        {                                                                               \
            VSF_HAL_ASSERT(adc != NULL);                                                \
            return vsf_remap_adc_channel_config((vsf_remap_adc_t *)adc,                 \
                                                channel_cfgs, channel_cfgs_cnt);        \
        }                                                                               \
                                                                                        \
        vsf_err_t vsf_imp_adc_channel_request(vsf_imp_adc_t *adc,                       \
                void *buffer, uint_fast32_t count)                                      \
        {                                                                               \
            VSF_HAL_ASSERT(adc != NULL);                                                \
            return vsf_remap_adc_channel_request((vsf_remap_adc_t *)adc, buffer, count);\
        }                                                                               \
        vsf_err_t vsf_imp_adc_get_configuration(vsf_imp_adc_t *adc, vsf_adc_cfg_t *cfg) \
        {                                                                               \
            VSF_HAL_ASSERT(adc != NULL);                                                \
            return vsf_remap_adc_get_configuration((vsf_remap_adc_t *)adc, cfg);        \
        }
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#define VSF_HAL_TEMPLATE_IMP_NAME                   _adc
#define VSF_HAL_TEMPLATE_IMP_UPCASE_NAME            _ADC

#if !defined(VSF_ADC_CFG_IMP_PREFIX) && !defined(VSF_ADC_CFG_IMP_DEVICE_PREFIX)
#   error "Please define VSF_ADC_CFG_IMP_PREFIX in adc driver"
#endif

#if !defined(VSF_ADC_CFG_IMP_UPCASE_PREFIX) && !defined(VSF_ADC_CFG_IMP_DEVICE_UPCASE_PREFIX)
#   error "Please define VSF_ADC_CFG_IMP_UPCASE_PREFIX in adc driver"
#endif

#ifndef VSF_ADC_CFG_IMP_COUNT_MASK_PREFIX
#   define VSF_ADC_CFG_IMP_COUNT_MASK_PREFIX        VSF_ADC_CFG_IMP_UPCASE_PREFIX
#endif

#ifdef VSF_ADC_CFG_IMP_REMAP_FUNCTIONS
#   define VSF_HAL_CFG_IMP_REMAP_FUNCTIONS          VSF_ADC_CFG_IMP_REMAP_FUNCTIONS
#endif

#include "hal/driver/common/template/vsf_template_instance_implementation.h"

#undef VSF_ADC_CFG_IMP_PREFIX
#undef VSF_ADC_CFG_IMP_COUNT_MASK_PREFIX
#undef VSF_ADC_CFG_IMP_UPCASE_PREFIX
#undef VSF_ADC_CFG_IMP_DEVICE_PREFIX
#undef VSF_ADC_CFG_IMP_DEVICE_UPCASE_PREFIX
#undef VSF_ADC_CFG_IMP_REMAP_PREFIX
#undef VSF_ADC_CFG_IMP_LV0
#undef VSF_ADC_CFG_IMP_REMAP_FUNCTIONS
#undef VSF_ADC_CFG_IMP_HAS_OP
#undef VSF_ADC_CFG_IMP_EXTERN_OP

#undef vsf_imp_adc_t
#undef vsf_imp_adc_init
#undef vsf_imp_adc_fini
#undef vsf_imp_adc_enable
#undef vsf_imp_adc_disable
#undef vsf_imp_adc_status
#undef vsf_imp_adc_capability
#undef vsf_imp_adc_irq_enable
#undef vsf_imp_adc_irq_disable
#undef vsf_imp_adc_channel_request_once
#undef vsf_imp_adc_channel_config
#undef vsf_imp_adc_channel_request
#undef vsf_imp_adc_get_configuration

#undef vsf_remap_adc_t
#undef vsf_remap_adc_init
#undef vsf_remap_adc_fini
#undef vsf_remap_adc_enable
#undef vsf_remap_adc_disable
#undef vsf_remap_adc_status
#undef vsf_remap_adc_capability
#undef vsf_remap_adc_irq_enable
#undef vsf_remap_adc_irq_disable
#undef vsf_remap_adc_channel_request_once
#undef vsf_remap_adc_channel_config
#undef vsf_remap_adc_channel_request
#undef vsf_remap_adc_get_configuration

#endif  /* VSF_HAL_USE_ADC */
