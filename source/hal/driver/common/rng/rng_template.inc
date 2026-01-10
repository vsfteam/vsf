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

#if VSF_HAL_USE_RNG == ENABLED

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

#if defined(VSF_RNG_CFG_IMP_RENAME_DEVICE_PREFIX) && (VSF_RNG_CFG_IMP_RENAME_DEVICE_PREFIX == ENABLED)
#   define vsf_real_rng_t                      VSF_MCONNECT(VSF_RNG_CFG_IMP_DEVICE_PREFIX, _t)
#   define vsf_real_rng_capability             VSF_MCONNECT(VSF_RNG_CFG_IMP_DEVICE_PREFIX, _capability)
#   define vsf_real_rng_ctrl                   VSF_MCONNECT(VSF_RNG_CFG_IMP_DEVICE_PREFIX, _ctrl)
#else
#   define vsf_real_rng_t                      VSF_MCONNECT(VSF_RNG_CFG_IMP_PREFIX, _rng_t)
#   define vsf_real_rng_capability             VSF_MCONNECT(VSF_RNG_CFG_IMP_PREFIX, _rng_capability)
#   define vsf_real_rng_ctrl                   VSF_MCONNECT(VSF_RNG_CFG_IMP_PREFIX, _rng_ctrl)
#endif

#ifndef VSF_RNG_CFG_REIMPLEMENT_API_CAPABILITY
#   define  VSF_RNG_CFG_REIMPLEMENT_API_CAPABILITY              DISABLED
#endif

#ifndef VSF_RNG_CFG_REIMPLEMENT_API_CTRL
#   define  VSF_RNG_CFG_REIMPLEMENT_API_CTRL                    DISABLED
#endif

#ifdef VSF_RNG_CFG_IMP_REMAP_PREFIX
#   undef VSF_RNG_CFG_REIMPLEMENT_API_CAPABILITY
#   define VSF_RNG_CFG_REIMPLEMENT_API_CAPABILITY               ENABLED
#   undef VSF_RNG_CFG_REIMPLEMENT_API_CTRL
#   define VSF_RNG_CFG_REIMPLEMENT_API_CTRL                     ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_RNG_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
vsf_rng_capability_t vsf_real_rng_capability(vsf_real_rng_t *rng_ptr)
{
    vsf_rng_capability_t rng_capability = {
        0,
    };

    return rng_capability;
}
#endif

#if VSF_RNG_CFG_REIMPLEMENT_API_CTRL == DISABLED
vsf_err_t vsf_real_rng_ctrl(vsf_real_rng_t *rng_ptr, vsf_rng_ctrl_t ctrl, void *param)
{
    VSF_HAL_ASSERT(NULL != rng_ptr);

    // Default implementation: not supported, trigger assertion
    VSF_HAL_ASSERT(0);

    return VSF_ERR_NOT_SUPPORT;
}
#endif

/*============================ MACROS ========================================*/

#undef VSF_RNG_CFG_REIMPLEMENT_TYPE_CAPABILITY
#undef VSF_RNG_CFG_REIMPLEMENT_TYPE_CTRL
#undef VSF_RNG_CFG_REIMPLEMENT_API_CAPABILITY
#undef VSF_RNG_CFG_REIMPLEMENT_API_CTRL
#undef vsf_real_rng_t
#undef vsf_real_rng_capability
#undef vsf_real_rng_ctrl

/*============================ MACROS ========================================*/

#ifdef VSF_RNG_CFG_IMP_REMAP_PREFIX

#   define vsf_imp_rng_t                    VSF_MCONNECT(VSF_RNG_CFG_IMP_PREFIX, _rng_t)
#   define vsf_imp_rng_init                 VSF_MCONNECT(VSF_RNG_CFG_IMP_PREFIX, _rng_init)
#   define vsf_imp_rng_fini                 VSF_MCONNECT(VSF_RNG_CFG_IMP_PREFIX, _rng_fini)
#   define vsf_imp_rng_capability           VSF_MCONNECT(VSF_RNG_CFG_IMP_PREFIX, _rng_capability)
#   define vsf_imp_rng_generate_request     VSF_MCONNECT(VSF_RNG_CFG_IMP_PREFIX, _rng_generate_request)
#   define vsf_imp_rng_ctrl                 VSF_MCONNECT(VSF_RNG_CFG_IMP_PREFIX, _rng_ctrl)

#   define vsf_remap_rng_t                  VSF_MCONNECT(VSF_RNG_CFG_IMP_REMAP_PREFIX, _rng_t)
#   define vsf_remap_rng_init               VSF_MCONNECT(VSF_RNG_CFG_IMP_REMAP_PREFIX, _rng_init)
#   define vsf_remap_rng_fini               VSF_MCONNECT(VSF_RNG_CFG_IMP_REMAP_PREFIX, _rng_fini)
#   define vsf_remap_rng_capability         VSF_MCONNECT(VSF_RNG_CFG_IMP_REMAP_PREFIX, _rng_capability)
#   define vsf_remap_rng_generate_request   VSF_MCONNECT(VSF_RNG_CFG_IMP_REMAP_PREFIX, _rng_generate_request)
#   define vsf_remap_rng_ctrl               VSF_MCONNECT(VSF_RNG_CFG_IMP_REMAP_PREFIX, _rng_ctrl)

#   define VSF_RNG_CFG_IMP_REMAP_FUNCTIONS                                                                   \
        vsf_err_t vsf_imp_rng_init(vsf_imp_rng_t *rng_ptr)                                     \
        {                                                                                      \
            VSF_HAL_ASSERT(rng_ptr != NULL);                                                   \
            return vsf_remap_rng_init(rng_ptr);                                                \
        }                                                                                      \
        void vsf_imp_rng_fini(vsf_imp_rng_t *rng_ptr)                                          \
        {                                                                                      \
            VSF_HAL_ASSERT(rng_ptr != NULL);                                                   \
            vsf_remap_rng_fini(rng_ptr);                                                       \
        }                                                                                      \
        vsf_err_t vsf_imp_rng_generate_request(vsf_imp_rng_t *rng_ptr,                         \
                                               uint32_t *buffer, uint32_t num, void *param,    \
                                               vsf_rng_on_ready_callback_t * on_ready_cb)      \
        {                                                                                      \
            VSF_HAL_ASSERT(rng_ptr != NULL);                                                   \
            return vsf_remap_rng_generate_request(rng_ptr, buffer, num, param, on_ready_cb);   \
        }                                                                                      \
        vsf_rng_capability_t vsf_imp_rng_capability(vsf_imp_rng_t *rng_ptr)                    \
        {                                                                                      \
            VSF_HAL_ASSERT(rng_ptr != NULL);                                                   \
            return vsf_remap_rng_capability(rng_ptr);                                          \
        }                                                                                      \
        vsf_err_t vsf_imp_rng_ctrl(vsf_imp_rng_t *rng_ptr, vsf_rng_ctrl_t ctrl, void *param)   \
        {                                                                                      \
            VSF_HAL_ASSERT(rng_ptr != NULL);                                                   \
            return vsf_remap_rng_ctrl(rng_ptr, ctrl, param);                                   \
        }
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#define VSF_HAL_TEMPLATE_IMP_NAME                   _rng
#define VSF_HAL_TEMPLATE_IMP_UPCASE_NAME            _RNG

#if !defined(VSF_RNG_CFG_IMP_PREFIX) && !defined(VSF_RNG_CFG_IMP_DEVICE_PREFIX)
#   error "Please define VSF_RNG_CFG_IMP_PREFIX in rng driver"
#endif

#if !defined(VSF_RNG_CFG_IMP_UPCASE_PREFIX) && !defined(VSF_RNG_CFG_IMP_DEVICE_UPCASE_PREFIX)
#   error "Please define VSF_RNG_CFG_IMP_UPCASE_PREFIX in rng driver"
#endif

#ifndef VSF_RNG_CFG_IMP_COUNT_MASK_PREFIX
#   define VSF_RNG_CFG_IMP_COUNT_MASK_PREFIX        VSF_RNG_CFG_IMP_UPCASE_PREFIX
#endif

#ifdef VSF_RNG_CFG_IMP_REMAP_FUNCTIONS
#   define VSF_HAL_CFG_IMP_REMAP_FUNCTIONS          VSF_RNG_CFG_IMP_REMAP_FUNCTIONS
#endif

#include "hal/driver/common/template/vsf_template_instance_implementation.h"

#undef VSF_RNG_CFG_IMP_PREFIX
#undef VSF_RNG_CFG_IMP_COUNT_MASK_PREFIX
#undef VSF_RNG_CFG_IMP_UPCASE_PREFIX
#undef VSF_RNG_CFG_IMP_DEVICE_PREFIX
#undef VSF_RNG_CFG_IMP_DEVICE_UPCASE_PREFIX
#undef VSF_RNG_CFG_IMP_LV0

#undef vsf_imp_rng_t
#undef vsf_imp_rng_init
#undef vsf_imp_rng_fini
#undef vsf_imp_rng_capability
#undef vsf_imp_rng_generate_request
#undef vsf_imp_rng_ctrl

#undef vsf_remap_rng_t
#undef vsf_remap_rng_init
#undef vsf_remap_rng_fini
#undef vsf_remap_rng_capability
#undef vsf_remap_rng_generate_request
#undef vsf_remap_rng_ctrl

#undef VSF_HAL_TEMPLATE_IMP_NAME
#undef VSF_HAL_TEMPLATE_IMP_UPCASE_NAME

/*============================ STATIC ASSERTIONS ==============================*/

/* User-extensible macros for custom mode and IRQ mask values
 * Users can define these macros before including this template to append
 * their custom enum values to the uniqueness checks.
 *
 * Check modes for custom values:
 *   VSF_RNG_CFG_MODE_CHECK_UNIQUE - Check mode for custom mode values
 *     Default: VSF_HAL_CHECK_MODE_LOOSE (loose mode, no overlapping bits)
 *     Can be set to: VSF_HAL_CHECK_MODE_STRICT (strict mode, no overlapping bits)
 *   VSF_RNG_CFG_IRQ_MASK_CHECK_UNIQUE - Check mode for custom IRQ mask values
 *     Default: VSF_HAL_CHECK_MODE_STRICT (strict mode, no overlapping bits)
 *     Can be set to: VSF_HAL_CHECK_MODE_LOOSE (loose mode, no overlapping bits)
 *
 * Example usage in vendor driver:
 *   #define VSF_RNG_CUSTOM_MODE_VALUES           MY_CUSTOM_MODE1, MY_CUSTOM_MODE2
 *   #define VSF_RNG_CFG_MODE_CHECK_UNIQUE        VSF_HAL_CHECK_MODE_LOOSE
 *   #define VSF_RNG_CUSTOM_IRQ_MASK_VALUES       MY_CUSTOM_IRQ1, MY_CUSTOM_IRQ2
 *   #define VSF_RNG_CFG_IRQ_MASK_CHECK_UNIQUE    VSF_HAL_CHECK_MODE_STRICT
 *   #include "hal/driver/common/rng/rng_template.inc"
 */


#ifdef VSF_RNG_CFG_MODE_CHECK_UNIQUE
/* Default mode lists - can be redefined by users before including this file */
/* Note: RNG typically doesn't have mode enums, but custom modes can be added */

#ifdef VSF_RNG_CUSTOM_MODE_VALUES
/* Check uniqueness among custom mode values using user-specified check mode */
VSF_HAL_CHECK_UNIQUE_CUSTOM(VSF_RNG_CFG_MODE_CHECK_UNIQUE, VSF_RNG_CUSTOM_MODE_VALUES);
#endif
#endif  /* VSF_RNG_CFG_MODE_CHECK_UNIQUE */

/* ==================== IRQ MASK UNIQUENESS CHECKS ==================== */

#ifdef VSF_RNG_CFG_IRQ_MASK_CHECK_UNIQUE

#ifdef VSF_RNG_CUSTOM_IRQ_MASK_VALUES
/* Check uniqueness among custom IRQ mask values using user-specified check mode */
VSF_HAL_CHECK_UNIQUE_CUSTOM(VSF_RNG_CFG_IRQ_MASK_CHECK_UNIQUE, VSF_RNG_CUSTOM_IRQ_MASK_VALUES);
#endif

/* Note: RNG typically doesn't have a standard IRQ mask enum, but custom IRQ masks can be added */
#endif  /* VSF_RNG_CFG_IRQ_MASK_CHECK_UNIQUE */

#undef VSF_RNG_CFG_MODE_CHECK_UNIQUE
#undef VSF_RNG_CFG_IRQ_MASK_CHECK_UNIQUE
#undef VSF_RNG_CUSTOM_MODE_VALUES
#undef VSF_RNG_CUSTOM_IRQ_MASK_VALUES

#endif  /* VSF_HAL_USE_RNG */
