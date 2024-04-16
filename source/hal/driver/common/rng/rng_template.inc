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

#define vsf_real_rng_t          VSF_MCONNECT(VSF_RNG_CFG_IMP_PREFIX, _rng_t)
#define vsf_real_rng_capability VSF_MCONNECT(VSF_RNG_CFG_IMP_PREFIX, _rng_capability)

#ifndef VSF_RNG_CFG_REIMPLEMENT_API_CAPABILITY
#   define  VSF_RNG_CFG_REIMPLEMENT_API_CAPABILITY              DISABLED
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

/*============================ MACROS ========================================*/

#undef VSF_RNG_CFG_REIMPLEMENT_API_CAPABILITY
#undef vsf_real_rng_t
#undef vsf_real_rng_capability

/*============================ MACROS ========================================*/

#ifdef VSF_RNG_CFG_IMP_REMAP_PREFIX

#   define vsf_imp_rng_t                    VSF_MCONNECT(VSF_RNG_CFG_IMP_PREFIX, _rng_t)
#   define vsf_imp_rng_init                 VSF_MCONNECT(VSF_RNG_CFG_IMP_PREFIX, _rng_init)
#   define vsf_imp_rng_fini                 VSF_MCONNECT(VSF_RNG_CFG_IMP_PREFIX, _rng_fini)
#   define vsf_imp_rng_capability           VSF_MCONNECT(VSF_RNG_CFG_IMP_PREFIX, _rng_capability)
#   define vsf_imp_rng_generate_request     VSF_MCONNECT(VSF_RNG_CFG_IMP_PREFIX, _rng_generate_request)

#   define vsf_remap_rng_t                  VSF_MCONNECT(VSF_RNG_CFG_IMP_REMAP_PREFIX, _rng_t)
#   define vsf_remap_rng_init               VSF_MCONNECT(VSF_RNG_CFG_IMP_REMAP_PREFIX, _rng_init)
#   define vsf_remap_rng_fini               VSF_MCONNECT(VSF_RNG_CFG_IMP_REMAP_PREFIX, _rng_fini)
#   define vsf_remap_rng_capability         VSF_MCONNECT(VSF_RNG_CFG_IMP_REMAP_PREFIX, _rng_capability)
#   define vsf_remap_rng_generate_request   VSF_MCONNECT(VSF_RNG_CFG_IMP_REMAP_PREFIX, _rng_generate_request)

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
                                               vsf_imp_rng_on_ready_callback_t * on_ready_cb)  \
        {                                                                                      \
            VSF_HAL_ASSERT(rng_ptr != NULL);                                                   \
            return vsf_remap_rng_generate_request(rng_ptr, buffer, num, param, on_ready_cb);   \
        }                                                                                      \
        vsf_rng_capability_t vsf_imp_rng_capability(vsf_imp_rng_t *rng_ptr)                    \
        {                                                                                      \
            VSF_HAL_ASSERT(rng_ptr != NULL);                                                   \
            return vsf_remap_rng_capability(rng_ptr);                                          \
        }
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#define VSF_HAL_TEMPLATE_IMP_NAME                   _rng
#define VSF_HAL_TEMPLATE_IMP_UPCASE_NAME            _RNG

#ifndef VSF_RNG_CFG_IMP_PREFIX
#   error "Please define VSF_RNG_CFG_IMP_PREFIX in rng driver"
#endif

#ifndef VSF_RNG_CFG_IMP_UPCASE_PREFIX
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
#undef VSF_RNG_CFG_IMP_LV0

#undef vsf_imp_rng_t
#undef vsf_imp_rng_init
#undef vsf_imp_rng_fini
#undef vsf_imp_rng_capability
#undef vsf_imp_rng_generate_request

#undef vsf_remap_rng_t
#undef vsf_remap_rng_init
#undef vsf_remap_rng_fini
#undef vsf_remap_rng_capability
#undef vsf_remap_rng_generate_request

#endif  /* VSF_HAL_USE_RNG */
