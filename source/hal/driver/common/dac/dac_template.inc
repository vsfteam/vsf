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

#if VSF_HAL_USE_DAC == ENABLED

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/


#define vsf_real_dac_t                                          VSF_MCONNECT(VSF_DAC_CFG_IMP_PREFIX, _dac_t)
#define vsf_real_dac_capability                                 VSF_MCONNECT(VSF_DAC_CFG_IMP_PREFIX, _dac_capability)
#define vsf_real_dac_get_configuration                          VSF_MCONNECT(VSF_DAC_CFG_IMP_PREFIX, _dac_get_configuration)

#ifndef VSF_DAC_CFG_REIMPLEMENT_API_CAPABILITY
#   define  VSF_DAC_CFG_REIMPLEMENT_API_CAPABILITY              DISABLED
#endif

#ifndef VSF_DAC_CFG_REIMPLEMENT_API_GET_CONFIGURATION
#   define VSF_DAC_CFG_REIMPLEMENT_API_GET_CONFIGURATION        DISABLED
#endif

#ifdef VSF_DAC_CFG_IMP_REMAP_PREFIX
#   undef VSF_DAC_CFG_REIMPLEMENT_API_CAPABILITY
#   define VSF_DAC_CFG_REIMPLEMENT_API_CAPABILITY               ENABLED
#   undef VSF_DAC_CFG_REIMPLEMENT_API_GET_CONFIGURATION
#   define VSF_DAC_CFG_REIMPLEMENT_API_GET_CONFIGURATION        ENABLED
#endif

#if VSF_DAC_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
#   ifndef VSF_DAC_CFG_CAPABILITY_IRQ_MASK
#       define VSF_DAC_CFG_CAPABILITY_IRQ_MASK                  VSF_DAC_IRQ_ALL_BITS_MASK
#   endif
#   ifndef VSF_DAC_CFG_CAPABILITY_MAX_RESOLUTION_BITS
#       define VSF_DAC_CFG_CAPABILITY_MAX_RESOLUTION_BITS       12
#   endif
#   ifndef VSF_DAC_CFG_CAPABILITY_MIN_RESOLUTION_BITS
#       define VSF_DAC_CFG_CAPABILITY_MIN_RESOLUTION_BITS       8
#   endif
#   ifndef VSF_DAC_CFG_CAPABILITY_CHANNEL_COUNT
#       define VSF_DAC_CFG_CAPABILITY_CHANNEL_COUNT             4
#   endif
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_DAC_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
vsf_dac_capability_t vsf_real_dac_capability(vsf_real_dac_t *dac_ptr)
{
    vsf_dac_capability_t dac_capability = {
        .irq_mask               = VSF_DAC_CFG_CAPABILITY_IRQ_MASK,
        .max_resolution_bits    = VSF_DAC_CFG_CAPABILITY_MAX_RESOLUTION_BITS,
        .min_resolution_bits    = VSF_DAC_CFG_CAPABILITY_MIN_RESOLUTION_BITS,
        .channel_count          = VSF_DAC_CFG_CAPABILITY_CHANNEL_COUNT,
    };

    return dac_capability;
}
#endif

#if VSF_DAC_CFG_REIMPLEMENT_API_GET_CONFIGURATION == DISABLED
vsf_err_t vsf_real_dac_get_configuration(vsf_real_dac_t *dac_ptr, vsf_dac_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(NULL != dac_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);

    // Default implementation: not supported, trigger assertion
    VSF_HAL_ASSERT(0);

    return VSF_ERR_NOT_SUPPORT;
}
#endif

/*============================ MACROS ========================================*/

#undef VSF_DAC_CFG_REIMPLEMENT_API_CAPABILITY
#undef VSF_DAC_CFG_REIMPLEMENT_API_GET_CONFIGURATION
#undef VSF_DAC_CFG_CAPABILITY_IRQ_MASK
#undef VSF_DAC_CFG_CAPABILITY_MAX_RESOLUTION_BITS
#undef VSF_DAC_CFG_CAPABILITY_MIN_RESOLUTION_BITS
#undef VSF_DAC_CFG_CAPABILITY_CHANNEL_COUNT

#undef vsf_real_dac_t
#undef vsf_real_dac_capability
#undef vsf_real_dac_get_configuration

/*============================ MACROS ========================================*/

#ifdef VSF_DAC_CFG_IMP_REMAP_PREFIX
#   define vsf_imp_dac_t                                    VSF_MCONNECT(VSF_DAC_CFG_IMP_PREFIX, _dac_t)
#   define vsf_imp_dac_init(__DAC)                          VSF_MCONNECT(VSF_DAC_CFG_IMP_PREFIX, _dac_init)
#   define vsf_imp_dac_fini(__DAC)                          VSF_MCONNECT(VSF_DAC_CFG_IMP_PREFIX, _dac_fini)
#   define vsf_imp_dac_enable(__DAC)                        VSF_MCONNECT(VSF_DAC_CFG_IMP_PREFIX, _dac_enable)
#   define vsf_imp_dac_disable(__DAC)                       VSF_MCONNECT(VSF_DAC_CFG_IMP_PREFIX, _dac_disable)
#   define vsf_imp_dac_status(__DAC)                        VSF_MCONNECT(VSF_DAC_CFG_IMP_PREFIX, _dac_status)
#   define vsf_imp_dac_capability(__DAC)                    VSF_MCONNECT(VSF_DAC_CFG_IMP_PREFIX, _dac_capability)
#   define vsf_imp_dac_irq_enable(__DAC, ...)               VSF_MCONNECT(VSF_DAC_CFG_IMP_PREFIX, _dac_irq_enable)
#   define vsf_imp_dac_irq_disable(__DAC, ...)              VSF_MCONNECT(VSF_DAC_CFG_IMP_PREFIX, _dac_irq_disable)
#   define vsf_imp_dac_channel_request_once(__DAC, ...)     VSF_MCONNECT(VSF_DAC_CFG_IMP_PREFIX, _dac_channel_request_once)
#   define vsf_imp_dac_channel_config(__DAC, ...)           VSF_MCONNECT(VSF_DAC_CFG_IMP_PREFIX, _dac_channel_config)
#   define vsf_imp_dac_channel_request(__DAC, ...)          VSF_MCONNECT(VSF_DAC_CFG_IMP_PREFIX, _dac_channel_request)
#   define vsf_imp_dac_get_configuration                    VSF_MCONNECT(VSF_DAC_CFG_IMP_PREFIX, _dac_get_configuration)

#   define vsf_remap_dac_t                                  VSF_MCONNECT(VSF_DAC_CFG_IMP_REMAP_PREFIX, _dac_t)
#   define vsf_remap_dac_init(__DAC)                        VSF_MCONNECT(VSF_DAC_CFG_IMP_REMAP_PREFIX, _dac_init)
#   define vsf_remap_dac_fini(__DAC)                        VSF_MCONNECT(VSF_DAC_CFG_IMP_REMAP_PREFIX, _dac_fini)
#   define vsf_remap_dac_enable(__DAC)                      VSF_MCONNECT(VSF_DAC_CFG_IMP_REMAP_PREFIX, _dac_enable)
#   define vsf_remap_dac_disable(__DAC)                     VSF_MCONNECT(VSF_DAC_CFG_IMP_REMAP_PREFIX, _dac_disable)
#   define vsf_remap_dac_status(__DAC)                      VSF_MCONNECT(VSF_DAC_CFG_IMP_REMAP_PREFIX, _dac_status)
#   define vsf_remap_dac_capability(__DAC)                  VSF_MCONNECT(VSF_DAC_CFG_IMP_REMAP_PREFIX, _dac_capability)
#   define vsf_remap_dac_irq_enable(__DAC, ...)             VSF_MCONNECT(VSF_DAC_CFG_IMP_REMAP_PREFIX, _dac_irq_enable)
#   define vsf_remap_dac_irq_disable(__DAC, ...)            VSF_MCONNECT(VSF_DAC_CFG_IMP_REMAP_PREFIX, _dac_irq_disable)
#   define vsf_remap_dac_channel_request_once(__DAC, ...)   VSF_MCONNECT(VSF_DAC_CFG_IMP_REMAP_PREFIX, _dac_channel_request_once)
#   define vsf_remap_dac_channel_config(__DAC, ...)         VSF_MCONNECT(VSF_DAC_CFG_IMP_REMAP_PREFIX, _dac_channel_config)
#   define vsf_remap_dac_channel_request(__DAC, ...)        VSF_MCONNECT(VSF_DAC_CFG_IMP_REMAP_PREFIX, _dac_channel_request)
#   define vsf_remap_dac_get_configuration                  VSF_MCONNECT(VSF_DAC_CFG_IMP_REMAP_PREFIX, _dac_get_configuration)

#   define VSF_DAC_CFG_IMP_REMAP_FUNCTIONS                                                                            \
        vsf_err_t vsf_imp_dac_init(vsf_imp_dac_t *dac)                                                  \
        {                                                                                               \
            VSF_HAL_ASSERT(dac != NULL);                                                                \
            return vsf_remap_dac_init((vsf_remap_dac_t *)dac);                                          \
        }                                                                                               \
                                                                                                        \
        void vsf_imp_dac_fini(vsf_imp_dac_t *dac)                                                       \
        {                                                                                               \
            VSF_HAL_ASSERT(dac != NULL);                                                                \
            vsf_remap_dac_fini((vsf_remap_dac_t *)dac);                                                 \
        }                                                                                               \
                                                                                                        \
        vsf_remap_dac_status_t vsf_imp_dac_status(vsf_imp_dac_t *dac)                                   \
        {                                                                                               \
            VSF_HAL_ASSERT(dac != NULL);                                                                \
            return vsf_remap_dac_status((vsf_remap_dac_t *)dac);                                        \
        }                                                                                               \
                                                                                                        \
        vsf_remap_dac_capability_t vsf_imp_dac_capability(vsf_imp_dac_t *dac)                           \
        {                                                                                               \
            VSF_HAL_ASSERT(dac != NULL);                                                                \
            return vsf_remap_dac_capability((vsf_remap_dac_t *)dac);                                    \
        }                                                                                               \
                                                                                                        \
        fsm_rt_t vsf_imp_dac_enable(vsf_imp_dac_t *dac)                                                 \
        {                                                                                               \
            VSF_HAL_ASSERT(dac != NULL);                                                                \
            return vsf_remap_dac_enable((vsf_remap_dac_t *)dac);                                        \
        }                                                                                               \
                                                                                                        \
        fsm_rt_t vsf_imp_dac_disable(vsf_imp_dac_t *dac)                                                \
        {                                                                                               \
            VSF_HAL_ASSERT(dac != NULL);                                                                \
            return vsf_remap_dac_disable((vsf_remap_dac_t *)dac);                                       \
        }                                                                                               \
                                                                                                        \
        void vsf_imp_dac_irq_enable(vsf_imp_dac_t *dac, vsf_remap_dac_irq_mask_t irq_mask)              \
        {                                                                                               \
            VSF_HAL_ASSERT(dac != NULL);                                                                \
            vsf_remap_dac_irq_enable((vsf_remap_dac_t *)dac, irq_mask);                                 \
        }                                                                                               \
                                                                                                        \
        void vsf_imp_dac_irq_disable(vsf_imp_dac_t *dac, vsf_remap_dac_irq_mask_t irq_mask)             \
        {                                                                                               \
            VSF_HAL_ASSERT(dac != NULL);                                                                \
            vsf_remap_dac_irq_disable((vsf_remap_dac_t *)dac, irq_mask);                                \
        }                                                                                               \
                                                                                                        \
        vsf_err_t vsf_imp_dac_channel_request_once(vsf_imp_dac_t *dac,                                  \
                vsf_remap_dac_channel_cfg_t *channel_cfg, uint_fast16_t value)                          \
        {                                                                                               \
            VSF_HAL_ASSERT(dac != NULL);                                                                \
            return vsf_remap_dac_channel_request_once((vsf_remap_dac_t *)dac, channel_cfg, value);      \
        }                                                                                               \
                                                                                                        \
        vsf_err_t vsf_imp_dac_channel_config(vsf_imp_dac_t *dac,                                        \
                vsf_remap_dac_channel_cfg_t *channel_cfgs, uint_fast8_t channel_cfgs_cnt)               \
        {                                                                                               \
            VSF_HAL_ASSERT(dac != NULL);                                                                \
            return vsf_remap_dac_channel_config((vsf_remap_dac_t *)dac, channel_cfgs, channel_cfgs_cnt);\
        }                                                                                               \
                                                                                                        \
        vsf_err_t vsf_imp_dac_channel_request(vsf_imp_dac_t *dac,                                       \
                void *buffer, uint_fast32_t count)                                                      \
        {                                                                                               \
            VSF_HAL_ASSERT(dac != NULL);                                                                \
            return vsf_remap_dac_channel_request((vsf_remap_dac_t *)dac, buffer, count);                \
        }                                                                                               \
                                                                                                        \
        vsf_err_t vsf_imp_dac_get_configuration(vsf_imp_dac_t *dac, vsf_dac_cfg_t *cfg)                \
        {                                                                                               \
            VSF_HAL_ASSERT(dac != NULL);                                                                \
            return vsf_remap_dac_get_configuration((vsf_remap_dac_t *)dac, cfg);                        \
        }
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/
/*============================ MACROS ========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

#define VSF_HAL_TEMPLATE_IMP_NAME                   _dac
#define VSF_HAL_TEMPLATE_IMP_UPCASE_NAME            _DAC

#if !defined(VSF_DAC_CFG_IMP_PREFIX) && !defined(VSF_DAC_CFG_IMP_DEVICE_PREFIX)
#   error "Please define VSF_DAC_CFG_IMP_PREFIX in dac driver"
#endif

#if !defined(VSF_DAC_CFG_IMP_UPCASE_PREFIX) && !defined(VSF_DAC_CFG_IMP_DEVICE_UPCASE_PREFIX)
#   error "Please define VSF_DAC_CFG_IMP_UPCASE_PREFIX in dac driver"
#endif

#ifndef VSF_DAC_CFG_IMP_COUNT_MASK_PREFIX
#   define VSF_DAC_CFG_IMP_COUNT_MASK_PREFIX        VSF_DAC_CFG_IMP_UPCASE_PREFIX
#endif

#ifdef VSF_DAC_CFG_IMP_REMAP_FUNCTIONS
#   define VSF_HAL_CFG_IMP_REMAP_FUNCTIONS          VSF_DAC_CFG_IMP_REMAP_FUNCTIONS
#endif

#include "hal/driver/common/template/vsf_template_instance_implementation.h"

#undef VSF_DAC_CFG_IMP_PREFIX
#undef VSF_DAC_CFG_IMP_COUNT_MASK_PREFIX
#undef VSF_DAC_CFG_IMP_UPCASE_PREFIX
#undef VSF_DAC_CFG_IMP_DEVICE_PREFIX
#undef VSF_DAC_CFG_IMP_DEVICE_UPCASE_PREFIX
#undef VSF_DAC_CFG_IMP_LV0
#undef VSF_DAC_CFG_IMP_REMAP_FUNCTIONS
#undef VSF_DAC_CFG_IMP_HAS_OP
#undef VSF_DAC_CFG_IMP_EXTERN_OP

#undef vsf_imp_dac_t
#undef vsf_imp_dac_init
#undef vsf_imp_dac_fini
#undef vsf_imp_dac_enable
#undef vsf_imp_dac_disable
#undef vsf_imp_dac_status
#undef vsf_imp_dac_capability
#undef vsf_imp_dac_irq_enable
#undef vsf_imp_dac_irq_disable
#undef vsf_imp_dac_channel_request_once
#undef vsf_imp_dac_channel_config
#undef vsf_imp_dac_channel_request
#undef vsf_imp_dac_get_configuration

#undef vsf_remap_dac_t
#undef vsf_remap_dac_init
#undef vsf_remap_dac_fini
#undef vsf_remap_dac_enable
#undef vsf_remap_dac_disable
#undef vsf_remap_dac_status
#undef vsf_remap_dac_capability
#undef vsf_remap_dac_irq_enable
#undef vsf_remap_dac_irq_disable
#undef vsf_remap_dac_channel_request_once
#undef vsf_remap_dac_channel_config
#undef vsf_remap_dac_channel_request
#undef vsf_remap_dac_get_configuration

#endif  /* VSF_HAL_USE_DAC */
