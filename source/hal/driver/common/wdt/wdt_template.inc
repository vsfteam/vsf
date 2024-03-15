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

#if VSF_WDT_CFG_REIMPLEMENT_API_CAPABILITY == ENABLED
#   ifndef VSF_WDT_CFG_CAPABILITY_SUPPORT_EARLY_WAKEUP
#       define  VSF_WDT_CFG_CAPABILITY_SUPPORT_EARLY_WAKEUP             1
#   endif
#   ifndef VSF_WDT_CFG_CAPABILITY_SUPPORT_RESET_NONE
#       define  VSF_WDT_CFG_CAPABILITY_SUPPORT_RESET_NONE               1
#   endif
#   ifndef VSF_WDT_CFG_CAPABILITY_SUPPORT_RESET_CORE
#       define  VSF_WDT_CFG_CAPABILITY_SUPPORT_RESET_CORE               1
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

#if VSF_WDT_CFG_REIMPLEMENT_API_CAPABILITY == ENABLED
vsf_wdt_capability_t vsf_real_wdt_capability(vsf_real_wdt_t *wdt_ptr)
{
    vsf_wdt_capability_t wdt_capability = {
        .support_early_wakeup   = VSF_WDT_CFG_CAPABILITY_SUPPORT_EARLY_WAKEUP,
        .support_reset_none     = VSF_WDT_CFG_CAPABILITY_SUPPORT_RESET_NONE,
        .support_reset_core     = VSF_WDT_CFG_CAPABILITY_SUPPORT_RESET_CORE,
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
#undef VSF_WDT_CFG_CAPABILITY_SUPPORT_RESET_CORE
#undef VSF_WDT_CFG_CAPABILITY_SUPPORT_RESET_SOC
#undef VSF_WDT_CFG_CAPABILITY_SUPPORT_DISABLE
#undef VSF_WDT_CFG_CAPABILITY_MAX_TIMEOUT_MS

#undef vsf_real_wdt_t
#undef vsf_real_wdt_capability

/*============================ MACROS ========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

#define VSF_HAL_TEMPLATE_IMP_NAME                     _wdt
#define VSF_HAL_TEMPLATE_IMP_UPCASE_NAME              _WDT

#ifndef VSF_WDT_CFG_IMP_PREFIX
#   error "Please define VSF_WDT_CFG_IMP_PREFIX in wdt driver"
#endif

#ifndef VSF_WDT_CFG_IMP_UPCASE_PREFIX
#   error "Please define VSF_WDT_CFG_IMP_UPCASE_PREFIX in wdt driver"
#endif

#ifndef VSF_WDT_CFG_IMP_INSTANCE_PREFIX
#   define VSF_WDT_CFG_IMP_INSTANCE_PREFIX         VSF_WDT_CFG_IMP_PREFIX
#endif

#include "hal/driver/common/template/vsf_template_instance_implementation.h"

#undef VSF_WDT_CFG_IMP_PREFIX
#undef VSF_WDT_CFG_IMP_INSTANCE_PREFIX
#undef VSF_WDT_CFG_IMP_UPCASE_PREFIX
#undef VSF_WDT_CFG_IMP_LV0

#endif  /* VSF_HAL_USE_WDT */
