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

#include "hal/driver/common/template/vsf_template_hal_driver.h"

/*============================ MACROS ========================================*/
#if VSF_HAL_USE_PM == ENABLED

#ifndef VSF_PM_CFG_CS_COUNT
#   define VSF_PM_CFG_CS_COUNT     1
#endif

#ifndef VSF_PM_CFG_REIMPLEMENT_API_CAPABILITY
#   define  VSF_PM_CFG_REIMPLEMENT_API_CAPABILITY               DISABLED
#endif

#ifdef VSF_PM_CFG_IMP_REMAP_PREFIX
#   undef VSF_PM_CFG_REIMPLEMENT_API_CAPABILITY
#   define VSF_PM_CFG_REIMPLEMENT_API_CAPABILITY                ENABLED
#endif

#if defined(VSF_PM_CFG_IMP_RENAME_DEVICE_PREFIX) && (VSF_PM_CFG_IMP_RENAME_DEVICE_PREFIX == ENABLED)
#   define vsf_real_pm_t                      VSF_MCONNECT(VSF_PM_CFG_IMP_DEVICE_PREFIX, _t)
#   define vsf_real_pm_capability             VSF_MCONNECT(VSF_PM_CFG_IMP_DEVICE_PREFIX, _capability)
#else
#   define vsf_real_pm_t                      VSF_MCONNECT(VSF_PM_CFG_IMP_PREFIX, _pm_t)
#   define vsf_real_pm_capability             VSF_MCONNECT(VSF_PM_CFG_IMP_PREFIX, _pm_capability)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_PM_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
vsf_pm_capability_t vsf_real_pm_capability(vsf_real_pm_t *pm_ptr)
{
    vsf_pm_capability_t pm_capability = {
        0,
    };

    return pm_capability;
}
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#define VSF_HAL_TEMPLATE_IMP_NAME                     _pm
#define VSF_HAL_TEMPLATE_IMP_UPCASE_NAME              _PM

#if !defined(VSF_PM_CFG_IMP_PREFIX) && !defined(VSF_PM_CFG_IMP_DEVICE_PREFIX)
#   error "Please define VSF_PM_CFG_IMP_PREFIX in pm driver"
#endif

#if !defined(VSF_PM_CFG_IMP_UPCASE_PREFIX) && !defined(VSF_PM_CFG_IMP_DEVICE_UPCASE_PREFIX)
#   error "Please define VSF_PM_CFG_IMP_UPCASE_PREFIX    in pm driver"
#endif

#include "hal/driver/common/template/vsf_template_instance_implementation.h"

#undef VSF_PM_CFG_IMP_PREFIX
#undef VSF_PM_CFG_IMP_UPCASE_PREFIX
#undef VSF_PM_CFG_IMP_DEVICE_PREFIX
#undef VSF_PM_CFG_IMP_DEVICE_UPCASE_PREFIX
#undef VSF_PM_CFG_IMP_RENAME_DEVICE_PREFIX

#undef VSF_HAL_TEMPLATE_IMP_NAME
#undef VSF_HAL_TEMPLATE_IMP_UPCASE_NAME

/*============================ STATIC ASSERTIONS ==============================*/

/* User-extensible macros for custom mode and IRQ mask values
 * Users can define these macros before including this template to append
 * their custom enum values to the uniqueness checks.
 *
 * Check modes for custom values:
 *   VSF_PM_CFG_MODE_CHECK_UNIQUE - Check mode for custom mode values
 *     Default: VSF_HAL_CHECK_MODE_LOOSE (loose mode, no overlapping bits)
 *     Can be set to: VSF_HAL_CHECK_MODE_STRICT (strict mode, no overlapping bits)
 *   VSF_PM_CFG_IRQ_MASK_CHECK_UNIQUE - Check mode for custom IRQ mask values
 *     Default: VSF_HAL_CHECK_MODE_STRICT (strict mode, no overlapping bits)
 *     Can be set to: VSF_HAL_CHECK_MODE_LOOSE (loose mode, no overlapping bits)
 *
 * Example usage in vendor driver:
 *   #define VSF_PM_CUSTOM_MODE_VALUES           MY_CUSTOM_MODE1, MY_CUSTOM_MODE2
 *   #define VSF_PM_CFG_MODE_CHECK_UNIQUE        VSF_HAL_CHECK_MODE_LOOSE
 *   #define VSF_PM_CUSTOM_IRQ_MASK_VALUES       MY_CUSTOM_IRQ1, MY_CUSTOM_IRQ2
 *   #define VSF_PM_CFG_IRQ_MASK_CHECK_UNIQUE    VSF_HAL_CHECK_MODE_STRICT
 *   #include "hal/driver/common/pm/pm_template.inc"
 */


#undef VSF_PM_CFG_MODE_CHECK_UNIQUE
#undef VSF_PM_CFG_IRQ_MASK_CHECK_UNIQUE
#undef VSF_PM_CUSTOM_MODE_VALUES
#undef VSF_PM_CUSTOM_IRQ_MASK_VALUES

#endif // VSF_HAL_USE_PM == ENABLED
