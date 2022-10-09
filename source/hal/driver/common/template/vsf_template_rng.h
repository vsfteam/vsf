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

#ifndef __HAL_DRIVER_RNG_INTERFACE_H__
#define __HAL_DRIVER_RNG_INTERFACE_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_template_hal_driver.h"
#include "hal/arch/vsf_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

// application code can redefine it
#ifndef VSF_RNG_CFG_PREFIX
#   if defined(VSF_HW_RNG_COUNT) && (VSF_HW_RNG_COUNT != 0)
#       define VSF_RNG_CFG_PREFIX               vsf_hw
#   else
#       define VSF_RNG_CFG_PREFIX               vsf
#   endif
#endif

// multi-class support enabled by default for maximum availability.
#ifndef VSF_RNG_CFG_MULTI_CLASS
#   define VSF_RNG_CFG_MULTI_CLASS              ENABLED
#endif

#ifndef VSF_RNG_CFG_FUNCTION_RENAME
#   define VSF_RNG_CFG_FUNCTION_RENAME          ENABLED
#endif

#ifndef VSF_RNG_CFG_REIMPLEMENT_CAPABILITY
#   define VSF_RNG_CFG_REIMPLEMENT_CAPABILITY   DISABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_RNG_APIS(__prefix_name)                                                                                              \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,        rng, init,             VSF_MCONNECT(__prefix_name, _rng_t) *rng_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,             rng, fini,             VSF_MCONNECT(__prefix_name, _rng_t) *rng_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_rng_capability_t, rng, capability,       VSF_MCONNECT(__prefix_name, _rng_t) *rng_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,        rng, generate_request, VSF_MCONNECT(__prefix_name, _rng_t) *rng_ptr, \
                                uint32_t *buffer, uint32_t num, void *param, vsf_rng_on_ready_callback_t * on_ready_cb)

/*============================ TYPES =========================================*/

typedef struct vsf_rng_t vsf_rng_t;
typedef void vsf_rng_on_ready_callback_t(void *param, uint32_t *buffer, uint32_t num);

#if VSF_RNG_CFG_REIMPLEMENT_CAPABILITY == DISABLED
typedef struct vsf_rng_capability_t {
    inherit(vsf_peripheral_capability_t)
} vsf_rng_capability_t;
#endif

typedef struct vsf_rng_op_t {
#undef  __VSF_HAL_TEMPLATE_API
#define __VSF_HAL_TEMPLATE_API VSF_HAL_TEMPLATE_API_FP

    VSF_RNG_APIS(vsf)
} vsf_rng_op_t;

#if VSF_RNG_CFG_MULTI_CLASS == ENABLED
struct vsf_rng_t  {
    const vsf_rng_op_t * op;
};
#endif

/*============================ PROTOTYPES ====================================*/
/*============================ INCLUDES ======================================*/

/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_RNG_CFG_FUNCTION_RENAME == ENABLED
#   define __vsf_rng_t                          VSF_MCONNECT(VSF_RNG_CFG_PREFIX, _rng_t)
#   define vsf_rng_init(__RNG, ...)             VSF_MCONNECT(VSF_RNG_CFG_PREFIX, _rng_init)             ((__vsf_rng_t *)__RNG, ##__VA_ARGS__)
#   define vsf_rng_fini(__RNG)                  VSF_MCONNECT(VSF_RNG_CFG_PREFIX, _rng_fini)             ((__vsf_rng_t *)__RNG)
#   define vsf_rng_capability(__RNG)            VSF_MCONNECT(VSF_RNG_CFG_PREFIX, _rng_capability)       ((__vsf_rng_t *)__RNG)
#   define vsf_rng_generate_request(__RNG, ...) VSF_MCONNECT(VSF_RNG_CFG_PREFIX, _rng_generate_request) ((__vsf_rng_t *)__RNG, ##__VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif  /*__HAL_DRIVER_RNG_INTERFACE_H__*/
