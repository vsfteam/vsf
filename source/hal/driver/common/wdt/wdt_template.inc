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

#if VSF_HAL_USE_WDT == ENABLED

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

#define vsf_real_wdt_t          VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_t)
#define vsf_real_wdt_capability VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_capability)

#ifndef VSF_WDT_CFG_REIMPLEMENT_API_CAPABILITY
#   define  VSF_WDT_CFG_REIMPLEMENT_API_CAPABILITY              DISABLED
#endif

#ifdef VSF_WDT_CFG_IMP_REMAP_PREFIX
#   undef VSF_WDT_CFG_REIMPLEMENT_API_CAPABILITY
#   define VSF_WDT_CFG_REIMPLEMENT_API_CAPABILITY                ENABLED
#endif

#if VSF_WDT_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
#   ifndef VSF_WDT_CFG_CAPABILITY_SUPPORT_EARLY_WAKEUP
#       define  VSF_WDT_CFG_CAPABILITY_SUPPORT_EARLY_WAKEUP             1
#   endif
#   ifndef VSF_WDT_CFG_CAPABILITY_SUPPORT_RESET_NONE
#       define  VSF_WDT_CFG_CAPABILITY_SUPPORT_RESET_NONE               1
#   endif
#   ifndef VSF_WDT_CFG_CAPABILITY_support_reset_cpu
#       define  VSF_WDT_CFG_CAPABILITY_support_reset_cpu               1
#   endif
#   ifndef VSF_WDT_CFG_CAPABILITY_SUPPORT_RESET_SOC
#       define  VSF_WDT_CFG_CAPABILITY_SUPPORT_RESET_SOC                1
#   endif
#   ifndef VSF_WDT_CFG_CAPABILITY_SUPPORT_DISABLE
#       define  VSF_WDT_CFG_CAPABILITY_SUPPORT_DISABLE                  1
#   endif
#   ifndef VSF_WDT_CFG_CAPABILITY_MAX_TIMEOUT_MS
#       define  VSF_WDT_CFG_CAPABILITY_MAX_TIMEOUT_MS                   (1000 * 1000)
#   endif
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_WDT_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
vsf_wdt_capability_t vsf_real_wdt_capability(vsf_real_wdt_t *wdt_ptr)
{
    vsf_wdt_capability_t wdt_capability = {
        .support_early_wakeup   = VSF_WDT_CFG_CAPABILITY_SUPPORT_EARLY_WAKEUP,
        .support_reset_none     = VSF_WDT_CFG_CAPABILITY_SUPPORT_RESET_NONE,
        .support_reset_cpu     = VSF_WDT_CFG_CAPABILITY_support_reset_cpu,
        .support_reset_soc      = VSF_WDT_CFG_CAPABILITY_SUPPORT_RESET_SOC,
        .support_disable        = VSF_WDT_CFG_CAPABILITY_SUPPORT_DISABLE,
        .max_timeout_ms         = VSF_WDT_CFG_CAPABILITY_MAX_TIMEOUT_MS,
    };

    return wdt_capability;
}
#endif

/*============================ MACROS ========================================*/

#undef VSF_WDT_CFG_REIMPLEMENT_API_CAPABILITY
#undef VSF_WDT_CFG_CAPABILITY_SUPPORT_EARLY_WAKEUP
#undef VSF_WDT_CFG_CAPABILITY_SUPPORT_RESET_NONE
#undef VSF_WDT_CFG_CAPABILITY_support_reset_cpu
#undef VSF_WDT_CFG_CAPABILITY_SUPPORT_RESET_SOC
#undef VSF_WDT_CFG_CAPABILITY_SUPPORT_DISABLE
#undef VSF_WDT_CFG_CAPABILITY_MAX_TIMEOUT_MS

#undef vsf_real_wdt_t
#undef vsf_real_wdt_capability

/*============================ MACROS ========================================*/

#ifdef VSF_WDT_CFG_IMP_REMAP_PREFIX

#   define vsf_imp_wdt_t            VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_t)
#   define vsf_imp_wdt_init         VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_init)
#   define vsf_imp_wdt_fini         VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_fini)
#   define vsf_imp_wdt_enable       VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_enable)
#   define vsf_imp_wdt_disable      VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_disable)
#   define vsf_imp_wdt_capability   VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_capability)
#   define vsf_imp_wdt_feed         VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_feed)

#   define vsf_remap_wdt_t          VSF_MCONNECT(VSF_WDT_CFG_IMP_REMAP_PREFIX, _wdt_t)
#   define vsf_remap_wdt_init       VSF_MCONNECT(VSF_WDT_CFG_IMP_REMAP_PREFIX, _wdt_init)
#   define vsf_remap_wdt_fini       VSF_MCONNECT(VSF_WDT_CFG_IMP_REMAP_PREFIX, _wdt_fini)
#   define vsf_remap_wdt_enable     VSF_MCONNECT(VSF_WDT_CFG_IMP_REMAP_PREFIX, _wdt_enable)
#   define vsf_remap_wdt_disable    VSF_MCONNECT(VSF_WDT_CFG_IMP_REMAP_PREFIX, _wdt_disable)
#   define vsf_remap_wdt_capability VSF_MCONNECT(VSF_WDT_CFG_IMP_REMAP_PREFIX, _wdt_capability)
#   define vsf_remap_wdt_feed       VSF_MCONNECT(VSF_WDT_CFG_IMP_REMAP_PREFIX, _wdt_feed)

#   define VSF_WDT_CFG_IMP_REMAP_FUNCTIONS                                                        \
        vsf_err_t vsf_imp_wdt_init(vsf_imp_wdt_t *wdt_ptr, vsf_wdt_cfg_t *cfg_ptr)  \
        {                                                                           \
            VSF_HAL_ASSERT(wdt_ptr != NULL);                                        \
            return vsf_remap_wdt_init(wdt_ptr, cfg_ptr);                            \
        }                                                                           \
        void vsf_imp_wdt_fini(vsf_imp_wdt_t *wdt_ptr)                               \
        {                                                                           \
            VSF_HAL_ASSERT(wdt_ptr != NULL);                                        \
            vsf_remap_wdt_fini(wdt_ptr);                                            \
        }                                                                           \
        fsm_rt_t vsf_imp_wdt_enable(vsf_imp_wdt_t *wdt_ptr)                         \
        {                                                                           \
            VSF_HAL_ASSERT(wdt_ptr != NULL);                                        \
            return vsf_remap_wdt_enable(wdt_ptr);                                   \
        }                                                                           \
        fsm_rt_t vsf_imp_wdt_disable(vsf_imp_wdt_t *wdt_ptr)                        \
        {                                                                           \
            VSF_HAL_ASSERT(wdt_ptr != NULL);                                        \
            return vsf_remap_wdt_disable(wdt_ptr);                                  \
        }                                                                           \
        vsf_wdt_capability_t vsf_imp_wdt_capability(vsf_imp_wdt_t *wdt_ptr)         \
        {                                                                           \
            VSF_HAL_ASSERT(wdt_ptr != NULL);                                        \
            return vsf_remap_wdt_capability(wdt_ptr);                               \
        }                                                                           \
        void vsf_imp_wdt_feed(vsf_imp_wdt_t *wdt_ptr)                               \
        {                                                                           \
            VSF_HAL_ASSERT(wdt_ptr != NULL);                                        \
            vsf_remap_wdt_feed(wdt_ptr);                                            \
        }
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#define VSF_HAL_TEMPLATE_IMP_NAME                   _wdt
#define VSF_HAL_TEMPLATE_IMP_UPCASE_NAME            _WDT

#ifndef VSF_WDT_CFG_IMP_PREFIX
#   error "Please define VSF_WDT_CFG_IMP_PREFIX in wdt driver"
#endif

#ifndef VSF_WDT_CFG_IMP_UPCASE_PREFIX
#   error "Please define VSF_WDT_CFG_IMP_UPCASE_PREFIX in wdt driver"
#endif

#ifndef VSF_WDT_CFG_IMP_COUNT_MASK_PREFIX
#   define VSF_WDT_CFG_IMP_COUNT_MASK_PREFIX        VSF_WDT_CFG_IMP_UPCASE_PREFIX
#endif

#ifdef VSF_WDT_CFG_IMP_REMAP_FUNCTIONS
#   define VSF_HAL_CFG_IMP_REMAP_FUNCTIONS          VSF_WDT_CFG_IMP_REMAP_FUNCTIONS
#endif

#include "hal/driver/common/template/vsf_template_instance_implementation.h"

#undef VSF_WDT_CFG_IMP_PREFIX
#undef VSF_WDT_CFG_IMP_COUNT_MASK_PREFIX
#undef VSF_WDT_CFG_IMP_UPCASE_PREFIX
#undef VSF_WDT_CFG_IMP_LV0
#undef VSF_WDT_CFG_IMP_REMAP_FUNCTIONS
#undef VSF_WDT_CFG_IMP_HAS_OP
#undef VSF_WDT_CFG_IMP_EXTERN_OP

#undef vsf_imp_wdt_t
#undef vsf_imp_wdt_init
#undef vsf_imp_wdt_fini
#undef vsf_imp_wdt_enable
#undef vsf_imp_wdt_disable
#undef vsf_imp_wdt_capability
#undef vsf_imp_wdt_feed

#undef vsf_remap_wdt_t
#undef vsf_remap_wdt_init
#undef vsf_remap_wdt_fini
#undef vsf_remap_wdt_enable
#undef vsf_remap_wdt_disable
#undef vsf_remap_wdt_capability
#undef vsf_remap_wdt_feed
#endif  /* VSF_HAL_USE_WDT */
