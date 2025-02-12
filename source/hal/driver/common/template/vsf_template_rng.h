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

/**
 * \~english
 * @brief Enable multi-class support by default for maximum availability.
 * \~chinese
 * @brief 默认启用多类支持，以获得最大可用性。
 */
#ifndef VSF_RNG_CFG_MULTI_CLASS
#   define VSF_RNG_CFG_MULTI_CLASS              ENABLED
#endif

/**
 * \~english
 * @brief Convert count to mask in specific hardware driver.
 * \~chinese
 * @brief 在特定硬件驱动中将数量转换为掩码。
 */
#if defined(VSF_HW_RNG_COUNT) && !defined(VSF_HW_RNG_MASK)
#   define VSF_HW_RNG_MASK                      VSF_HAL_COUNT_TO_MASK(VSF_HW_RNG_COUNT)
#endif

/**
 * \~english
 * @brief Convert mask to count in specific hardware driver.
 * \~chinese
 * @brief 在特定硬件驱动中将掩码转换为数量。
 */
#if defined(VSF_HW_RNG_MASK) && !defined(VSF_HW_RNG_COUNT)
#   define VSF_HW_RNG_COUNT                     VSF_HAL_MASK_TO_COUNT(VSF_HW_RNG_MASK)
#endif

/**
 * \~english
 * @brief We can redefine macro VSF_RNG_CFG_PREFIX to specify a prefix
 * to call a specific driver directly in the application code.
 * \~chinese
 * @brief 可重新定义宏 VSF_RNG_CFG_PREFIX，以在应用代码中直接调用
 * 特定驱动函数。
 */
#ifndef VSF_RNG_CFG_PREFIX
#   if VSF_RNG_CFG_MULTI_CLASS == ENABLED
#       define VSF_RNG_CFG_PREFIX               vsf
#   elif defined(VSF_HW_RNG_COUNT) && (VSF_HW_RNG_COUNT != 0)
#       define VSF_RNG_CFG_PREFIX               vsf_hw
#   else
#       define VSF_RNG_CFG_PREFIX               vsf
#   endif
#endif

/**
 * \~english
 * @brief Disable VSF_RNG_CFG_FUNCTION_RENAME to use the original function
 * names (e.g., vsf_rng_init()).
 * \~chinese
 * @brief 禁用 VSF_RNG_CFG_FUNCTION_RENAME 以使用原始函数名
 * (例如 vsf_rng_init())。
 */
#ifndef VSF_RNG_CFG_FUNCTION_RENAME
#   define VSF_RNG_CFG_FUNCTION_RENAME          ENABLED
#endif

/**
 * \~english
 * @brief Enable macro VSF_RNG_CFG_REIMPLEMENT_TYPE_CFG in specific hardware
 * drivers to redefine struct @ref vsf_rng_cfg_t. For compatibility, members
 * should not be deleted when redefining. The vsf_rng_isr_handler_t type also
 * needs to be redefined.
 * \~chinese
 * @brief 在特定硬件驱动中，可以启用宏 VSF_RNG_CFG_REIMPLEMENT_TYPE_CFG 来
 * 重新定义结构体 @ref vsf_rng_cfg_t。为保证兼容性，重新定义时不应删除成员。
 * 同时需要重新定义类型 vsf_rng_isr_handler_t。
 */
#if VSF_RNG_CFG_REIMPLEMENT_TYPE_CFG == DISABLED
#    define VSF_RNG_CFG_REIMPLEMENT_TYPE_CFG DISABLED
#endif

/**
 * \~english
 * @brief Enable macro VSF_RNG_CFG_REIMPLEMENT_TYPE_CAPABILITY in specific
 * hardware drivers to redefine struct @ref vsf_rng_capability_t. For
 * compatibility, members should not be deleted when redefining.
 * \~chinese
 * @brief 在特定硬件驱动中，可以启用宏 VSF_RNG_CFG_REIMPLEMENT_TYPE_CAPABILITY
 * 来重新定义结构体 @ref vsf_rng_capability_t。为保证兼容性，重新定义时
 * 不应删除成员。
 */
#if VSF_RNG_CFG_REIMPLEMENT_TYPE_CAPABILITY == DISABLED
#    define VSF_RNG_CFG_REIMPLEMENT_TYPE_CAPABILITY DISABLED
#endif

/**
 * \~english
 * @brief Enable macro VSF_RNG_CFG_INHERIT_HAL_CAPABILITY to inherit the
 * structure @ref vsf_peripheral_capability_t.
 * \~chinese
 * @brief 启用宏 VSF_RNG_CFG_INHERIT_HAL_CAPABILITY 来继承结构体 @ref
 * vsf_peripheral_capability_t。
 */
#ifndef VSF_RNG_CFG_INHERIT_HAL_CAPABILITY
#   define VSF_RNG_CFG_INHERIT_HAL_CAPABILITY       ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

/**
 * \~english
 * @brief RNG API template, used to generate RNG type, specific prefix
 * function declarations, etc.
 * @param[in] __prefix_name The prefix used for generating RNG functions.
 * \~chinese
 * @brief RNG API 模板，用于生成 RNG 类型、特定前缀的函数声明等。
 * @param[in] __prefix_name 用于生成 RNG 函数的前缀。
 */
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
#if VSF_RNG_CFG_INHERIT_HAL_CAPABILITY == ENABLED
    inherit(vsf_peripheral_capability_t)
#endif
} vsf_rng_capability_t;

typedef struct vsf_rng_op_t {
/// @cond
#undef  __VSF_HAL_TEMPLATE_API
#define __VSF_HAL_TEMPLATE_API VSF_HAL_TEMPLATE_API_FP
/// @endcond

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
 @brief Initialize a RNG instance
 @param[in] rng_ptr: a pointer to structure @ref vsf_rng_t
 @return vsf_err_t: VSF_ERR_NONE if initialization successful, otherwise returns error code

 @note It is not necessary to call vsf_rng_fini() to deinitialize.
       vsf_rng_init() should be called before any other RNG API except vsf_rng_capability().

 \~chinese
 @brief 初始化一个 RNG 实例
 @param[in] rng_ptr: 指向结构体 @ref vsf_rng_t 的指针
 @return vsf_err_t: 如果初始化成功返回 VSF_ERR_NONE，否则返回错误码

 @note 失败后不需要调用 vsf_rng_fini() 进行反初始化。
       vsf_rng_init() 应该在除 vsf_rng_capability() 之外的其他 RNG API 之前调用。
 */
extern vsf_err_t vsf_rng_init(vsf_rng_t *rng_ptr);

/**
 \~english
 @brief Finalize a RNG instance
 @param[in] rng_ptr: a pointer to structure @ref vsf_rng_t
 @return none

 \~chinese
 @brief 终止一个 RNG 实例
 @param[in] rng_ptr: 指向结构体 @ref vsf_rng_t 的指针
 @return 无
 */
extern void vsf_rng_fini(vsf_rng_t *rng_ptr);

/**
 \~english
 @brief Get the capability of RNG instance
 @param[in] rng_ptr: a pointer to structure @ref vsf_rng_t
 @return vsf_rng_capability_t: All capabilities of current RNG @ref vsf_rng_capability_t

 \~chinese
 @brief 获取 RNG 实例的能力
 @param[in] rng_ptr: 指向结构体 @ref vsf_rng_t 的指针
 @return vsf_rng_capability_t: 返回当前 RNG 的所有能力 @ref vsf_rng_capability_t
 */
extern vsf_rng_capability_t vsf_rng_capability(vsf_rng_t *rng_ptr);

/**
 \~english
 @brief RNG generate request
 @param[in] rng_ptr: a pointer to structure @ref vsf_rng_t
 @param[in] buffer: RNG data buffer
 @param[in] num: the length of RNG data buffer
 @param[in] param: callback parameter
 @param[in] on_ready_cb: callback function
 @return vsf_err_t: VSF_ERR_NONE if the request was successful, otherwise returns error code

 \~chinese
 @brief RNG 生成请求
 @param[in] rng_ptr: 指向结构体 @ref vsf_rng_t 的指针
 @param[in] buffer: RNG 数据缓冲区
 @param[in] num: RNG 数据缓冲区长度
 @param[in] param: 回调函数参数
 @param[in] on_ready_cb: 完成回调函数
 @return vsf_err_t: 如果请求成功返回 VSF_ERR_NONE，否则返回错误码
 */
extern vsf_err_t vsf_rng_generate_request(vsf_rng_t *rng_ptr, uint32_t *buffer, uint32_t num,
                                          void *param, vsf_rng_on_ready_callback_t * on_ready_cb);

/*============================ INCLUDES ======================================*/

/*============================ MACROFIED FUNCTIONS ===========================*/

/// @cond
#if VSF_RNG_CFG_FUNCTION_RENAME == ENABLED
#   define __vsf_rng_t                          VSF_MCONNECT(VSF_RNG_CFG_PREFIX, _rng_t)
#   define vsf_rng_init(__RNG, ...)             VSF_MCONNECT(VSF_RNG_CFG_PREFIX, _rng_init)             ((__vsf_rng_t *)(__RNG), ##__VA_ARGS__)
#   define vsf_rng_fini(__RNG)                  VSF_MCONNECT(VSF_RNG_CFG_PREFIX, _rng_fini)             ((__vsf_rng_t *)(__RNG))
#   define vsf_rng_capability(__RNG)            VSF_MCONNECT(VSF_RNG_CFG_PREFIX, _rng_capability)       ((__vsf_rng_t *)(__RNG))
#   define vsf_rng_generate_request(__RNG, ...) VSF_MCONNECT(VSF_RNG_CFG_PREFIX, _rng_generate_request) ((__vsf_rng_t *)(__RNG), ##__VA_ARGS__)
#endif
/// @endcond

#ifdef __cplusplus
}
#endif

#endif  /*__VSF_TEMPLATE_RNG_H__*/
