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

#if VSF_HAL_USE_EXTI == ENABLED

/*============================ MACROS ========================================*/

#ifndef VSF_EXTI_CFG_RETAIN_DEFINED_PREFIX
#   define VSF_EXTI_CFG_RETAIN_DEFINED_PREFIX                       DISABLED
#endif

#ifndef VSF_EXTI_CFG_REIMPLEMENT_API_IRQ_CLEAR
#   define VSF_EXTI_CFG_REIMPLEMENT_API_IRQ_CLEAR                 DISABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ IMPLEMENTATION ================================*/

#ifndef VSF_EXTI_CFG_REAL_PREFIX
#   if defined(VSF_EXTI_CFG_IMP_RENAME_DEVICE_PREFIX) && (VSF_EXTI_CFG_IMP_RENAME_DEVICE_PREFIX == ENABLED)
#       define VSF_EXTI_CFG_REAL_PREFIX         VSF_EXTI_CFG_IMP_DEVICE_PREFIX
#   else
#       define VSF_EXTI_CFG_REAL_PREFIX         VSF_EXTI_CFG_IMP_PREFIX
#   endif
#endif

#if defined(VSF_EXTI_CFG_IMP_RENAME_DEVICE_PREFIX) && (VSF_EXTI_CFG_IMP_RENAME_DEVICE_PREFIX == ENABLED)
#   define vsf_real_exti_t                      VSF_MCONNECT(VSF_EXTI_CFG_IMP_DEVICE_PREFIX, _t)
#   define vsf_real_exti_irq_clear              VSF_MCONNECT(VSF_EXTI_CFG_IMP_DEVICE_PREFIX, _irq_clear)
#   define vsf_real_exti_ctrl                   VSF_MCONNECT(VSF_EXTI_CFG_IMP_DEVICE_PREFIX, _ctrl)
#else
#   define vsf_real_exti_t                      VSF_MCONNECT(VSF_EXTI_CFG_REAL_PREFIX, _exti_t)
#   define vsf_real_exti_irq_clear              VSF_MCONNECT(VSF_EXTI_CFG_REAL_PREFIX, _exti_irq_clear)
#   define vsf_real_exti_ctrl                   VSF_MCONNECT(VSF_EXTI_CFG_REAL_PREFIX, _exti_ctrl)
#endif

#ifndef VSF_EXTI_CFG_REIMPLEMENT_API_CTRL
#   define VSF_EXTI_CFG_REIMPLEMENT_API_CTRL                    DISABLED
#endif

#if VSF_EXTI_CFG_REIMPLEMENT_API_IRQ_CLEAR == DISABLED
vsf_exti_channel_mask_t vsf_real_exti_irq_clear(vsf_real_exti_t *exti_ptr, vsf_exti_channel_mask_t channel_mask)
{
    VSF_HAL_ASSERT(NULL != exti_ptr);

    // Default implementation: not supported, trigger assertion
    VSF_HAL_ASSERT(0);

    return 0;
}
#endif

#if VSF_EXTI_CFG_REIMPLEMENT_API_CTRL == DISABLED
vsf_err_t vsf_real_exti_ctrl(vsf_real_exti_t *exti_ptr, vsf_exti_ctrl_t ctrl, void *param)
{
    VSF_HAL_ASSERT(NULL != exti_ptr);

    // Default implementation: not supported, trigger assertion
    VSF_HAL_ASSERT(0);

    return VSF_ERR_NOT_SUPPORT;
}
#endif
/*============================ MACROS ========================================*/
/*============================ MACROS ========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

#ifndef VSF_HAL_TEMPLATE_IMP_NAME
#   define VSF_HAL_TEMPLATE_IMP_NAME                _exti
#endif

#ifndef VSF_HAL_TEMPLATE_IMP_UPCASE_NAME
#   define VSF_HAL_TEMPLATE_IMP_UPCASE_NAME         _EXTI
#endif

#ifndef VSF_HAL_TEMPLATE_IMP_COUNT_SUFFIX
#   define VSF_HAL_TEMPLATE_IMP_COUNT_SUFFIX        _COUNT
#endif

#ifndef VSF_HAL_TEMPLATE_IMP_MASK_SUFFIX
#   define VSF_HAL_TEMPLATE_IMP_MASK_SUFFIX         _MASK
#endif

#if !defined(VSF_EXTI_CFG_IMP_PREFIX) && !defined(VSF_EXTI_CFG_IMP_DEVICE_PREFIX)
#   error "Please define VSF_EXTI_CFG_IMP_PREFIX in exti driver"
#endif

#if !defined(VSF_EXTI_CFG_IMP_UPCASE_PREFIX) && !defined(VSF_EXTI_CFG_IMP_DEVICE_UPCASE_PREFIX)
#   error "Please define VSF_EXTI_CFG_IMP_UPCASE_PREFIX in exti driver"
#endif

#ifndef VSF_EXTI_CFG_IMP_COUNT_MASK_PREFIX
#   define VSF_EXTI_CFG_IMP_COUNT_MASK_PREFIX       VSF_EXTI_CFG_IMP_UPCASE_PREFIX
#endif

#ifdef VSF_EXTI_CFG_IMP_REMAP_FUNCTIONS
#   define VSF_HAL_CFG_IMP_REMAP_FUNCTIONS          VSF_EXTI_CFG_IMP_REMAP_FUNCTIONS
#endif

#include "hal/driver/common/template/vsf_template_instance_implementation.h"

#if VSF_EXTI_CFG_RETAIN_DEFINED_PREFIX == DISABLED
#   undef VSF_EXTI_CFG_IMP_PREFIX
#   undef VSF_EXTI_CFG_IMP_UPCASE_PREFIX
#   undef VSF_EXTI_CFG_IMP_DEVICE_PREFIX
#   undef VSF_EXTI_CFG_IMP_DEVICE_UPCASE_PREFIX
#endif

#undef VSF_EXTI_CFG_REIMPLEMENT_API_IRQ_CLEAR
#undef VSF_EXTI_CFG_REIMPLEMENT_API_CTRL
#undef VSF_EXTI_CFG_REIMPLEMENT_TYPE_CTRL
#undef VSF_EXTI_CFG_REAL_PREFIX
#undef vsf_real_exti_t
#undef vsf_real_exti_irq_clear
#undef vsf_real_exti_ctrl

/*============================ STATIC ASSERTIONS ==============================*/

/* User-extensible macros for custom mode and IRQ mask values
 * Users can define these macros before including this template to append
 * their custom enum values to the uniqueness checks.
 *
 * Check modes for custom values:
 *   VSF_EXTI_CFG_MODE_CHECK_UNIQUE - Check mode for custom mode values
 *     Default: VSF_HAL_CHECK_MODE_LOOSE (loose mode, no overlapping bits)
 *     Can be set to: VSF_HAL_CHECK_MODE_STRICT (strict mode, no overlapping bits)
 *   VSF_EXTI_CFG_IRQ_MASK_CHECK_UNIQUE - Check mode for custom IRQ mask values
 *     Default: VSF_HAL_CHECK_MODE_STRICT (strict mode, no overlapping bits)
 *     Can be set to: VSF_HAL_CHECK_MODE_LOOSE (loose mode, no overlapping bits)
 *
 * Example usage in vendor driver:
 *   #define VSF_EXTI_CUSTOM_MODE_VALUES           MY_CUSTOM_MODE1, MY_CUSTOM_MODE2
 *   #define VSF_EXTI_CFG_MODE_CHECK_UNIQUE        VSF_HAL_CHECK_MODE_LOOSE
 *   #define VSF_EXTI_CUSTOM_IRQ_MASK_VALUES       MY_CUSTOM_IRQ1, MY_CUSTOM_IRQ2
 *   #define VSF_EXTI_CFG_IRQ_MASK_CHECK_UNIQUE    VSF_HAL_CHECK_MODE_STRICT
 *   #include "hal/driver/common/exti/exti_template.inc"
 */


#undef VSF_EXTI_CFG_MODE_CHECK_UNIQUE
#undef VSF_EXTI_CFG_IRQ_MASK_CHECK_UNIQUE
#undef VSF_EXTI_CUSTOM_MODE_VALUES
#undef VSF_EXTI_CUSTOM_IRQ_MASK_VALUES

#endif  /* VSF_HAL_USE_EXTI */
