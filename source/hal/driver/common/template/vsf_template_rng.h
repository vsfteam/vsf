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

#ifndef __VSF_TEMPLATE_RNG_H__
#define __VSF_TEMPLATE_RNG_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_template_hal_driver.h"
#include "hal/arch/vsf_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

// multi-class support enabled by default for maximum availability.
#ifndef VSF_RNG_CFG_MULTI_CLASS
#   define VSF_RNG_CFG_MULTI_CLASS              ENABLED
#endif

#if defined(VSF_HW_RNG_COUNT) && !defined(VSF_HW_RNG_MASK)
#   define VSF_HW_RNG_MASK                      VSF_HAL_COUNT_TO_MASK(VSF_HW_RNG_COUNT)
#endif

#if defined(VSF_HW_RNG_MASK) && !defined(VSF_HW_RNG_COUNT)
#   define VSF_HW_RNG_COUNT                     VSF_HAL_MASK_TO_COUNT(VSF_HW_RNG_MASK)
#endif

// application code can redefine it
#ifndef VSF_RNG_CFG_PREFIX
#   if VSF_RNG_CFG_MULTI_CLASS == ENABLED
#       define VSF_RNG_CFG_PREFIX               vsf
#   elif defined(VSF_HW_RNG_COUNT) && (VSF_HW_RNG_COUNT != 0)
#       define VSF_RNG_CFG_PREFIX               vsf_hw
#   else
#       define VSF_RNG_CFG_PREFIX               vsf
#   endif
#endif

#ifndef VSF_RNG_CFG_FUNCTION_RENAME
#   define VSF_RNG_CFG_FUNCTION_RENAME          ENABLED
#endif

#ifndef VSF_RNG_CFG_INHERT_HAL_CAPABILITY
#   define VSF_RNG_CFG_INHERT_HAL_CAPABILITY       ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_RNG_APIS(__prefix_name)                                                                                              \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,            rng, init,             VSF_MCONNECT(__prefix_name, _rng_t) *rng_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                 rng, fini,             VSF_MCONNECT(__prefix_name, _rng_t) *rng_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_rng_capability_t, rng, capability,       VSF_MCONNECT(__prefix_name, _rng_t) *rng_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,            rng, generate_request, VSF_MCONNECT(__prefix_name, _rng_t) *rng_ptr, \
                                uint32_t *buffer, uint32_t num, void *param, vsf_rng_on_ready_callback_t * on_ready_cb)

/*============================ TYPES =========================================*/

typedef struct vsf_rng_t vsf_rng_t;
typedef void vsf_rng_on_ready_callback_t(void *param, uint32_t *buffer, uint32_t num);

typedef struct vsf_rng_capability_t {
#if VSF_RNG_CFG_INHERT_HAL_CAPABILITY == ENABLED
    inherit(vsf_peripheral_capability_t)
#endif
} vsf_rng_capability_t;

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

/**
 \~english
 @brief initialize a rng instance.
 @param[in] rng_ptr: a pointer to structure @ref vsf_rng_t
 @param[in] cfg_ptr: a pointer to structure @ref vsf_rng_cfg_t
 @return vsf_err_t: VSF_ERR_NONE if rng was initialized, or a negative error code

 @note It is not necessary to call vsf_rng_fini() to deinitialization.
       vsf_rng_init() should be called before any other rng API except vsf_rng_capability().

 \~chinese
 @brief 初始化一个 rng 实例
 @param[in] rng_ptr: 结构体 vsf_rng_t 的指针，参考 @ref vsf_rng_t
 @param[in] cfg_ptr: 结构体 vsf_rng_cfg_t 的指针，参考 @ref vsf_rng_cfg_t
 @return vsf_err_t: 如果 rng 初始化成功返回 VSF_ERR_NONE , 失败返回负数。

 @note 失败后不需要调用 vsf_rng_fini() 反初始化。
       vsf_rng_init() 应该在除 vsf_rng_capability() 之外的其他 rng API 之前调用。
 */
extern vsf_err_t vsf_rng_init(vsf_rng_t *rng_ptr);

/**
 \~english
 @brief finalize a rng instance.
 @param[in] rng_ptr: a pointer to structure @ref vsf_rng_t
 @return none

 \~chinese
 @brief 终止一个 rng 实例
 @param[in] rng_ptr: 结构体 vsf_rng_t 的指针，参考 @ref vsf_rng_t
 @param[in] cfg_ptr: 结构体 vsf_rng_cfg_t 的指针，参考 @ref vsf_rng_cfg_t
 @return 无。
 */
extern void vsf_rng_fini(vsf_rng_t *rng_ptr);

/**
 \~english
 @brief get the capability of rng instance.
 @param[in] rng_ptr: a pointer to structure @ref vsf_rng_t
 @return vsf_rng_capability_t: return all capability of current rng @ref vsf_rng_capability_t

 \~chinese
 @brief 获取 rng 实例的能力
 @param[in] rng_ptr: 结构体 vsf_rng_t 的指针，参考 @ref vsf_rng_t
 @return vsf_rng_capability_t: 返回当前 rng 的所有能力 @ref vsf_rng_capability_t
 */
extern vsf_rng_capability_t vsf_rng_capability(vsf_rng_t *rng_ptr);

/**
 \~english
 @brief rng generate request
 @param[in] rng_ptr: a pointer to structure @ref vsf_rng_t
 @param[in] buffer: rng data buffer
 @param[in] num: the length of rng data buffer
 @param[in] param: call back param
 @param[in] on_ready_cb: call back
 @return vsf_err_t: VSF_ERR_NONE if rng generate request was successfully, or a negative error code

 \~chinese
 @brief rng 生成请求
 @param[in] rng_ptr: 结构体 vsf_rng_t 的指针，参考 @ref vsf_rng_t
 @return vsf_err_t: 如果 rng 生成请求成功返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vsf_rng_generate_request(vsf_rng_t *rng_ptr, uint32_t *buffer, uint32_t num,
                                          void *param, vsf_rng_on_ready_callback_t * on_ready_cb);

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

#endif  /*__VSF_TEMPLATE_RNG_H__*/
