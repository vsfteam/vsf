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

#ifndef __VSF_TEMPLATE_PWM_H__
#define __VSF_TEMPLATE_PWM_H__

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
#ifndef VSF_PWM_CFG_MULTI_CLASS
#   define VSF_PWM_CFG_MULTI_CLASS              ENABLED
#endif

/**
 * \~english
 * @brief Convert count to mask in specific hardware driver.
 * \~chinese
 * @brief 在特定硬件驱动中将数量转换为掩码。
 */
#if defined(VSF_HW_PWM_COUNT) && !defined(VSF_HW_PWM_MASK)
#   define VSF_HW_PWM_MASK                      VSF_HAL_COUNT_TO_MASK(VSF_HW_PWM_COUNT)
#endif

/**
 * \~english
 * @brief Convert mask to count in specific hardware driver.
 * \~chinese
 * @brief 在特定硬件驱动中将掩码转换为数量。
 */
#if defined(VSF_HW_PWM_MASK) && !defined(VSF_HW_PWM_COUNT)
#   define VSF_HW_PWM_COUNT                     VSF_HAL_MASK_TO_COUNT(VSF_HW_PWM_MASK)
#endif

/**
 * \~english
 * @brief We can redefine macro VSF_PWM_CFG_PREFIX to specify a prefix
 * to call a specific driver directly in the application code.
 * \~chinese
 * @brief 可重新定义宏 VSF_PWM_CFG_PREFIX，以在应用代码中直接调用
 * 特定驱动函数。
 */
#ifndef VSF_PWM_CFG_PREFIX
#   if VSF_PWM_CFG_MULTI_CLASS == ENABLED
#       define VSF_PWM_CFG_PREFIX               vsf
#   elif defined(VSF_HW_PWM_COUNT) && (VSF_HW_PWM_COUNT != 0)
#       define VSF_PWM_CFG_PREFIX               vsf_hw
#   else
#       define VSF_PWM_CFG_PREFIX               vsf
#   endif
#endif

#ifndef VSF_PWM_CFG_FUNCTION_RENAME
#   define VSF_PWM_CFG_FUNCTION_RENAME          ENABLED
#endif

/**
 * \~english
 * @brief Enable macro VSF_PWM_CFG_REIMPLEMENT_TYPE_CFG in specific hardware
 * drivers to redefine struct @ref vsf_pwm_cfg_t. For compatibility, members
 * should not be deleted when redefining. The vsf_pwm_isr_handler_t type also
 * needs to be redefined.
 * \~chinese
 * @brief 在特定硬件驱动中，可以启用宏 VSF_PWM_CFG_REIMPLEMENT_TYPE_CFG 来
 * 重新定义结构体 @ref vsf_pwm_cfg_t。为保证兼容性，重新定义时不应删除成员。
 * 同时需要重新定义类型 vsf_pwm_isr_handler_t。
 */
#ifndef VSF_PWM_CFG_REIMPLEMENT_TYPE_CFG
#    define VSF_PWM_CFG_REIMPLEMENT_TYPE_CFG DISABLED
#endif

/**
 * \~english
 * @brief Redefine struct vsf_pwm_capability_t. For compatibility, members
 * should not be deleted when redefining.
 * \~chinese
 * @brief 重新定义结构体 vsf_pwm_capability_t。为保证兼容性，重新定义时不应删除成员。
 */
#ifndef VSF_PWM_CFG_REIMPLEMENT_TYPE_CAPABILITY
#    define VSF_PWM_CFG_REIMPLEMENT_TYPE_CAPABILITY DISABLED
#endif

/**
 * \~english
 * @brief In specific hardware driver, we can enable macro
 * VSF_PWM_CFG_INHERIT_HAL_CAPABILITY to inherit the structure
 * @ref vsf_peripheral_capability_t.
 * \~chinese
 * @brief 在特定硬件驱动中，可以启用宏 VSF_PWM_CFG_INHERIT_HAL_CAPABILITY 来
 * 继承结构体 @ref vsf_peripheral_capability_t。
 */
#ifndef VSF_PWM_CFG_INHERIT_HAL_CAPABILITY
#   define VSF_PWM_CFG_INHERIT_HAL_CAPABILITY       ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

/**
 * \~english
 * @brief PWM API template, used to generate PWM type, specific prefix
 * function declarations, etc.
 * @param[in] __prefix_name The prefix used for generating PWM functions.
 * \~chinese
 * @brief PWM API 模板，用于生成 PWM 类型、特定前缀的函数声明等。
 * @param[in] __prefix_name 用于生成 PWM 函数的前缀。
 */
#define VSF_PWM_APIS(__prefix_name)                                                                                                                            \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,             pwm, init,           VSF_MCONNECT(__prefix_name, _pwm_t) *pwm_ptr, vsf_pwm_cfg_t *cfg_ptr)                              \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                  pwm, fini,           VSF_MCONNECT(__prefix_name, _pwm_t) *pwm_ptr)                              \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,              pwm, enable,         VSF_MCONNECT(__prefix_name, _pwm_t) *pwm_ptr)                                                  \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,              pwm, disable,        VSF_MCONNECT(__prefix_name, _pwm_t) *pwm_ptr)                                                  \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_pwm_capability_t,  pwm, capability,     VSF_MCONNECT(__prefix_name, _pwm_t) *pwm_ptr)                                                                \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,             pwm, set,            VSF_MCONNECT(__prefix_name, _pwm_t) *pwm_ptr, uint8_t channel, uint32_t period, uint32_t pulse)\
    __VSF_HAL_TEMPLATE_API(__prefix_name, uint32_t,              pwm, get_freq,       VSF_MCONNECT(__prefix_name, _pwm_t) *pwm_ptr)

/*============================ TYPES =========================================*/

#if VSF_PWM_CFG_REIMPLEMENT_TYPE_CFG == DISABLED
/**
 * \~english
 * @brief PWM configuration structure.
 * Used to configure the PWM generator's frequency settings.
 * \~chinese
 * @brief PWM 配置结构体。
 * 用于配置 PWM 发生器的频率设置。
 */
typedef struct vsf_pwm_cfg_t {
    union {
        /**
         * \~english
         * @brief The PWM clock frequency in Hz.
         * Determines the base time period for the PWM generator.
         * \~chinese
         * @brief PWM 时钟频率，单位为 Hz。
         * 确定 PWM 发生器的基本时间周期。
         */
        uint32_t freq;

        /**
         * \~english
         * @brief The minimum allowed PWM clock frequency in Hz.
         * Used when configuring adaptive frequency ranges.
         * \~chinese
         * @brief 允许的最小 PWM 时钟频率，单位为 Hz。
         * 在配置自适应频率范围时使用。
         */
        uint32_t min_freq;
    };
} vsf_pwm_cfg_t;
#endif

#if VSF_PWM_CFG_REIMPLEMENT_TYPE_CAPABILITY == DISABLED
/**
 * \~english
 * @brief PWM capability structure.
 * Describes the capabilities and limitations of the PWM hardware.
 * \~chinese
 * @brief PWM 能力结构体。
 * 描述 PWM 硬件的能力和限制。
 */
typedef struct vsf_pwm_capability_t {
#if VSF_PWM_CFG_INHERIT_HAL_CAPABILITY == ENABLED
    inherit(vsf_peripheral_capability_t)
#endif

    /**
     * \~english
     * @brief Maximum supported PWM frequency in Hz.
     * The upper limit of the PWM generator's frequency range.
     * \~chinese
     * @brief 支持的最大 PWM 频率，单位为 Hz。
     * PWM 发生器频率范围的上限。
     */
    uint32_t max_freq;

    /**
     * \~english
     * @brief Minimum supported PWM frequency in Hz.
     * The lower limit of the PWM generator's frequency range.
     * \~chinese
     * @brief 支持的最小 PWM 频率，单位为 Hz。
     * PWM 发生器频率范围的下限。
     */
    uint32_t min_freq;
} vsf_pwm_capability_t;
#endif

typedef struct vsf_pwm_t vsf_pwm_t;

typedef struct vsf_pwm_op_t {
/// @cond
#undef __VSF_HAL_TEMPLATE_API
#define __VSF_HAL_TEMPLATE_API VSF_HAL_TEMPLATE_API_FP
/// @endcond

    VSF_PWM_APIS(vsf)
} vsf_pwm_op_t;

#if VSF_PWM_CFG_MULTI_CLASS == ENABLED
struct vsf_pwm_t  {
    const vsf_pwm_op_t * op;
};
#endif

/*============================ PROTOTYPES ====================================*/

/**
 * \~english
 * @brief Initialize a PWM instance
 * @param[in] pwm_ptr: pointer to structure @ref vsf_pwm_t
 * @param[in] cfg_ptr: pointer to configuration structure @ref vsf_pwm_cfg_t
 * @return vsf_err_t: VSF_ERR_NONE if initialization successful, otherwise return error code
 *
 * @note It is not necessary to call vsf_pwm_fini() to deinitialize.
 *       vsf_pwm_init() should be called before any other PWM API except vsf_pwm_capability().
 *
 * \~chinese
 * @brief 初始化一个 PWM 实例
 * @param[in] pwm_ptr: 指向结构体 @ref vsf_pwm_t 的指针
 * @param[in] cfg_ptr: 指向配置结构体 @ref vsf_pwm_cfg_t 的指针
 * @return vsf_err_t: 如果初始化成功返回 VSF_ERR_NONE，否则返回错误码
 *
 * @note 失败后不需要调用 vsf_pwm_fini() 进行反初始化。
 *       vsf_pwm_init() 应该在除 vsf_pwm_capability() 之外的其他 PWM API 之前调用。
 */
extern vsf_err_t vsf_pwm_init(vsf_pwm_t *pwm_ptr, vsf_pwm_cfg_t *cfg_ptr);

/**
 \~english
 @brief Finalize a PWM instance
 @param[in] pwm_ptr: a pointer to structure @ref vsf_pwm_t
 @return none

 \~chinese
 @brief 终止一个 PWM 实例
 @param[in] pwm_ptr: 指向结构体 @ref vsf_pwm_t 的指针
 @return 无
 */
extern void vsf_pwm_fini(vsf_pwm_t *pwm_ptr);

/**
 * \~english
 * @brief Enable PWM instance
 * @param[in] pwm_ptr: pointer to structure @ref vsf_pwm_t
 * @return fsm_rt_t: fsm_rt_cpl if PWM was enabled, fsm_rt_on_going if PWM is still enabling
 *
 * \~chinese
 * @brief 启用 PWM 实例
 * @param[in] pwm_ptr: 指向结构体 @ref vsf_pwm_t 的指针
 * @return fsm_rt_t: 如果 PWM 实例已启用返回 fsm_rt_cpl，如果 PWM 实例正在启用过程中返回 fsm_rt_on_going
 */
extern fsm_rt_t vsf_pwm_enable(vsf_pwm_t *pwm_ptr);

/**
 \~english
 @brief Disable a PWM instance
 @param[in] pwm_ptr: a pointer to structure @ref vsf_pwm_t
 @return fsm_rt_t: fsm_rt_cpl if PWM was disabled, fsm_rt_on_going if PWM is still disabling

 \~chinese
 @brief 禁用 PWM 实例
 @param[in] pwm_ptr: 指向结构体 @ref vsf_pwm_t 的指针
 @return fsm_rt_t: 如果 PWM 实例已禁用返回 fsm_rt_cpl，如果 PWM 实例正在禁用过程中返回 fsm_rt_on_going
 */
extern fsm_rt_t vsf_pwm_disable(vsf_pwm_t *pwm_ptr);

/**
 \~english
 @brief Get the capability of PWM instance
 @param[in] pwm_ptr: a pointer to structure @ref vsf_pwm_t
 @return vsf_pwm_capability_t: all capabilities of current PWM @ref vsf_pwm_capability_t

 \~chinese
 @brief 获取 PWM 实例的能力
 @param[in] pwm_ptr: 指向结构体 @ref vsf_pwm_t 的指针
 @return vsf_pwm_capability_t: 返回当前 PWM 的所有能力 @ref vsf_pwm_capability_t
 */
extern vsf_pwm_capability_t vsf_pwm_capability(vsf_pwm_t *pwm_ptr);

/**
 \~english
 @brief PWM set the period width and pulse width for a channel
 @param[in] pwm_ptr: a pointer to structure @ref vsf_pwm_t
 @param[in] channel: PWM channel
 @param[in] period: PWM period width (in clock counter)
 @param[in] pulse: PWM pulse width (in clock counter)
 @return vsf_err_t: VSF_ERR_NONE if the PWM set was successful, or a negative error code

 \~chinese
 @brief PWM 设置一个通道的周期和脉冲宽度
 @param[in] pwm_ptr: 指向结构体 @ref vsf_pwm_t 的指针
 @param[in] channel: PWM 通道
 @param[in] period: PWM 周期宽度（时钟计数）
 @param[in] pulse: PWM 脉冲宽度（时钟计数）
 @return vsf_err_t: 如果 PWM 设置成功返回 VSF_ERR_NONE，否则返回负数错误码
 */
extern vsf_err_t vsf_pwm_set(vsf_pwm_t *pwm_ptr, uint8_t channel,
                             uint32_t period, uint32_t pulse);

/**
 \~english
 @brief PWM get clock frequency
 @param[in] pwm_ptr: a pointer to structure @ref vsf_pwm_t
 @return uint32_t: PWM clock frequency in Hz

 \~chinese
 @brief PWM 获取时钟频率
 @param[in] pwm_ptr: 指向结构体 @ref vsf_pwm_t 的指针
 @return uint32_t: PWM 时钟频率，单位为赫兹(Hz)
 */
extern uint32_t vsf_pwm_get_freq(vsf_pwm_t *pwm_ptr);

/**
 \~english
 @brief Set PWM period in milliseconds
 @param[in] pwm_ptr: a pointer to structure @ref vsf_pwm_t
 @param[in] channel: PWM channel
 @param[in] period: PWM period width (in milliseconds)
 @param[in] pulse: PWM pulse width (in milliseconds)
 @return vsf_err_t: VSF_ERR_NONE if the PWM set was successful, otherwise returns error code

 \~chinese
 @brief 设置 PWM 通道的周期和脉冲宽度（毫秒）
 @param[in] pwm_ptr: the pointer to structure @ref vsf_pwm_t
 @param[in] channel: PWM 通道
 @param[in] period: PWM 周期宽度（毫秒）
 @param[in] pulse: PWM 脉冲宽度（毫秒）
 @return vsf_err_t: 如果 PWM 设置成功返回 VSF_ERR_NONE，否则返回错误码
 */
extern vsf_err_t vsf_pwm_set_ms(vsf_pwm_t *pwm_ptr,
                                uint8_t    channel,
                                uint32_t   period,
                                uint32_t   pulse);

/**
 \~english
 @brief Set PWM period in microseconds
 @param[in] pwm_ptr: a pointer to structure @ref vsf_pwm_t
 @param[in] channel: PWM channel
 @param[in] period: PWM period width (in microseconds)
 @param[in] pulse: PWM pulse width (in microseconds)
 @return vsf_err_t: VSF_ERR_NONE if the PWM set was successful, otherwise returns error code

 \~chinese
 @brief 设置 PWM 通道的周期和脉冲宽度（微秒）
 @param[in] pwm_ptr: 指向结构体 @ref vsf_pwm_t 的指针
 @param[in] channel: PWM 通道
 @param[in] period: PWM 周期宽度（微秒）
 @param[in] pulse: PWM 脉冲宽度（微秒）
 @return vsf_err_t: 如果 PWM 设置成功返回 VSF_ERR_NONE，否则返回错误码
 */
extern vsf_err_t vsf_pwm_set_us(vsf_pwm_t *pwm_ptr,
                                uint8_t    channel,
                                uint32_t   period,
                                uint32_t   pulse);

/**
 \~english
 @brief Set PWM period in nanoseconds
 @param[in] pwm_ptr: a pointer to structure @ref vsf_pwm_t
 @param[in] channel: PWM channel
 @param[in] period: PWM period width (in nanoseconds)
 @param[in] pulse: PWM pulse width (in nanoseconds)
 @return vsf_err_t: VSF_ERR_NONE if the PWM set was successful, otherwise returns error code

 \~chinese
 @brief 设置 PWM 通道的周期和脉冲宽度（纳秒）
 @param[in] pwm_ptr: 指向结构体 @ref vsf_pwm_t 的指针
 @param[in] channel: PWM 通道
 @param[in] period: PWM 周期宽度（纳秒）
 @param[in] pulse: PWM 脉冲宽度（纳秒）
 @return vsf_err_t: 如果 PWM 设置成功返回 VSF_ERR_NONE，否则返回错误码
 */
extern vsf_err_t vsf_pwm_set_ns(vsf_pwm_t *pwm_ptr,
                                uint8_t    channel,
                                uint32_t   period,
                                uint32_t   pulse);

/*============================ MACROFIED FUNCTIONS ===========================*/

/// @cond
#if VSF_PWM_CFG_FUNCTION_RENAME == ENABLED
#   define __vsf_pwm_t               VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_t)
#   define vsf_pwm_init(__PM, ...)   VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_init)       ((__vsf_pwm_t *)(__PM), ##__VA_ARGS__)
#   define vsf_pwm_fini(__PM)        VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_fini)       ((__vsf_pwm_t *)(__PM))
#   define vsf_pwm_enable(__PM)      VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_enable)     ((__vsf_pwm_t *)(__PM))
#   define vsf_pwm_disable(__PM)     VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_disable)    ((__vsf_pwm_t *)(__PM))
#   define vsf_pwm_capability(__PM)  VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_capability) ((__vsf_pwm_t *)(__PM))
#   define vsf_pwm_set(__PM, ...)    VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_set)        ((__vsf_pwm_t *)(__PM), ##__VA_ARGS__)
#   define vsf_pwm_get_freq(__PM)    VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_get_freq)   ((__vsf_pwm_t *)(__PM))
#endif
/// @endcond

#ifdef __cplusplus
}
#endif

#endif  /*__VSF_TEMPLATE_PWM_H__ */
