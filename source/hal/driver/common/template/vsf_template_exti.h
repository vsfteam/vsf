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

#ifndef __VSF_TEMPLATE_EXTI_H__
#define __VSF_TEMPLATE_EXTI_H__

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
 *
 * \~chinese
 * @brief 默认启用多类支持，以获得最大可用性。
 */
#ifndef VSF_EXTI_CFG_MULTI_CLASS
#   define VSF_EXTI_CFG_MULTI_CLASS                 ENABLED
#endif

/**
 * \~english
 * @brief Define EXTI hardware mask if count is defined.
 *
 * \~chinese
 * @brief 如果定义了 EXTI 硬件数量，则定义对应的掩码。
 */
#if defined(VSF_HW_EXTI_COUNT) && !defined(VSF_HW_EXTI_MASK)
#   define VSF_HW_EXTI_MASK                         VSF_HAL_COUNT_TO_MASK(VSF_HW_EXTI_COUNT)
#endif

/**
 * \~english
 * @brief Define EXTI hardware count if mask is defined.
 *
 * \~chinese
 * @brief 如果定义了 EXTI 硬件掩码，则定义对应的数量。
 */
#if defined(VSF_HW_EXTI_MASK) && !defined(VSF_HW_EXTI_COUNT)
#   define VSF_HW_EXTI_COUNT                        VSF_HAL_MASK_TO_COUNT(VSF_HW_EXTI_MASK)
#endif

/**
 * \~english
 * @brief Define EXTI channel mask if count is defined.
 *
 * \~chinese
 * @brief 如果定义了 EXTI 通道数量，则定义对应的掩码。
 */
#if defined(VSF_HW_EXTI_CHANNEL_COUNT) && !defined(VSF_HW_EXTI_CHANNEL_MASK)
#   define VSF_HW_EXTI_CHANNEL_MASK                 VSF_HAL_COUNT_TO_MASK(VSF_HW_EXTI_CHANNEL_COUNT)
#endif

/**
 * \~english
 * @brief Define EXTI channel count if mask is defined.
 *
 * \~chinese
 * @brief 如果定义了 EXTI 通道掩码，则定义对应的数量。
 */
#if defined(VSF_HW_EXTI_CHANNEL_MASK) && !defined(VSF_HW_EXTI_CHANNEL_COUNT)
#   define VSF_HW_EXTI_CHANNEL_COUNT                VSF_HAL_MASK_TO_COUNT(VSF_HW_EXTI_CHANNEL_MASK)
#endif

/**
 \~english
 VSF_EXTI_CFG_PREFIX is used to set the actual API call when calling the vsf_exti_*().
 For example, if you configure VSF_EXTI_CFG_PREFIX to be vsf_hw, then call
 vsf_exti_enable(), which is actually vsf_hw_exti_enable()

 If we want the call to vsf_exti_enable in xxxx.c to actually call vsf_example_exti_enable,
 then it can be configured in front of the .c:

 \~chinese
 VSF_EXTI_CFG_PREFIX 是用来配置调用 vsf_exti_*() 的时候实际调用的 API。
 例如，当 VSF_EXTI_CFG_PREFIX 配置成 vsf_hw 的时候，调用 vsf_exti_enable()
 实际调用的是 vsf_hw_exti_enable()

 如果我们想要 xxxx.c 里调用 vsf_exti_enable() 实际调用的是 vsf_example_exti_enable()，
 那么可以在 xxxx.c 源码前面配置：

 \~
 \code {.c}
    #undef  VSF_EXTI_CFG_PREFIX
    #define VSF_EXTI_CFG_PREFIX     vsf_example
 \endcode
 */
#ifndef VSF_EXTI_CFG_PREFIX
#   if VSF_EXTI_CFG_MULTI_CLASS == ENABLED
#       define VSF_EXTI_CFG_PREFIX                  vsf
#   elif defined(VSF_HW_EXTI_CHANNEL_MASK) && (VSF_HW_EXTI_CHANNEL_MASK != 0)
#       define VSF_EXTI_CFG_PREFIX                  vsf_hw
#   else
#       define VSF_EXTI_CFG_PREFIX                  vsf
#   endif
#endif

/**
 * \~english
 * @brief Enable option to reimplement mode type in specific hardware drivers.
 *
 * \~chinese
 * @brief 在具体硬件驱动中启用重新实现模式类型的选项。
 */
#ifndef VSF_EXTI_CFG_REIMPLEMENT_TYPE_MODE
#   define VSF_EXTI_CFG_REIMPLEMENT_TYPE_MODE       DISABLED
#endif

/**
 * \~english
 * @brief Enable function rename feature.
 *
 * \~chinese
 * @brief 启用函数重命名特性。
 */
#ifndef VSF_EXTI_CFG_FUNCTION_RENAME
#   define VSF_EXTI_CFG_FUNCTION_RENAME             ENABLED
#endif


//! Redefine struct vsf_exti_channel_cfg_t. The vsf_exti_isr_handler_t type also needs to
//! be redefined For compatibility, members should not be deleted when struct
//! @ref vsf_exti_channel_cfg_t redefining.
#ifndef VSF_EXTI_CFG_REIMPLEMENT_TYPE_CFG
#    define VSF_EXTI_CFG_REIMPLEMENT_TYPE_CFG       DISABLED
#endif

//! Redefine struct vsf_exti_capability_t.
//! For compatibility, members should not be deleted when struct @ref
//! vsf_exti_capability_t redefining.
#ifndef VSF_EXTI_CFG_REIMPLEMENT_TYPE_CAPABILITY
#    define VSF_EXTI_CFG_REIMPLEMENT_TYPE_CAPABILITY DISABLED
#endif


#ifndef VSF_EXTI_CFG_INHERIT_HAL_CAPABILITY
#   define VSF_EXTI_CFG_INHERIT_HAL_CAPABILITY       ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_EXTI_APIS(__prefix_name)                                                                                                                                                                \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,                exti, init,             VSF_MCONNECT(__prefix_name, _t) *exti_ptr)                                                              \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                     exti, fini,             VSF_MCONNECT(__prefix_name, _t) *exti_ptr)                                                              \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_exti_status_t,        exti, status,           VSF_MCONNECT(__prefix_name, _t) *exti_ptr)                                                              \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_exti_capability_t,    exti, capability,       VSF_MCONNECT(__prefix_name, _t) *exti_ptr)                                                              \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,                exti, trigger,          VSF_MCONNECT(__prefix_name, _t) *exti_ptr, vsf_exti_channel_mask_t channel_mask)                        \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,                exti, config_channels,  VSF_MCONNECT(__prefix_name, _t) *exti_ptr, vsf_exti_channel_mask_t channel_mask, vsf_exti_channel_cfg_t * cfg_ptr)\
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,                exti, irq_enable,       VSF_MCONNECT(__prefix_name, _t) *exti_ptr, vsf_exti_channel_mask_t channel_mask)                        \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,                exti, irq_disable,      VSF_MCONNECT(__prefix_name, _t) *exti_ptr, vsf_exti_channel_mask_t channel_mask)                        \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_exti_channel_mask_t, exti, irq_clear,        VSF_MCONNECT(__prefix_name, _t) *exti_ptr, vsf_exti_channel_mask_t channel_mask)

/*============================ TYPES =========================================*/

#ifndef vsf_exti_channel_mask_t
#   if defined(VSF_HW_EXTI_CHANNEL_COUNT) && (VSF_HW_EXTI_CHANNEL_COUNT > 32)
typedef uint64_t vsf_exti_channel_mask_t;
#   elif defined(VSF_HW_EXTI_CHANNEL_MASK) && (VSF_HW_EXTI_CHANNEL_MASK & 0xFFFFFFFF00000000)
typedef uint64_t vsf_exti_channel_mask_t;
#   else
typedef uint32_t vsf_exti_channel_mask_t;
#   endif
#   define vsf_exti_channel_mask_t vsf_exti_channel_mask_t
#endif

#if VSF_EXTI_CFG_REIMPLEMENT_TYPE_MODE == DISABLED
/**
 * \~english
 * @brief Predefined VSF EXTI modes that can be reimplemented in specific hal drivers.
 *
 * \~chinese
 * @brief 预定义的 VSF EXTI 模式，可以在具体的 hal 驱动重新实现。
 *
 * \~english
 * Even if the hardware doesn't support these features, these modes must be implemented:
 * If the IO supports more modes, We can implement it in the hardware driver.
 * If we add a new mode in the hardware driver, then we also need to define the
 * macro VSF_EXTI_MODE_MASK, whose value is the OR of the value of all modes.
 * \~chinese
 * 即使硬件不支持这些功能，但是这些模式是必须实现的。
 * 如果硬件驱动里我们添加了新的模式，那么也需要定义宏 VSF_EXTI_MODE_MASK，它的值是所有模式的值的或。
 *
 * - Trigger mode
 *  - VSF_EXTI_MODE_NONE
 *  - VSF_EXTI_MODE_EDGE_RISING
 *  - VSF_EXTI_MODE_EDGE_FALLING
 *  - VSF_EXTI_MODE_EDGE_RISING_FALLING
 *
 * \~english
 *  Optional features require one or more enumeration options and a macro with the same
 *  name to determine if they are supported at runtime. If the feature supports more than
 *  one option, it is recommended to provide the corresponding MASK option, so that the
 *  user can switch to different modes at runtime.
 * \~chinese
 * 可选特性需要提供一个或者多个枚举选项，还需要提供同名的宏，方便用户在运行时判断是否支持。
 * 如果它特性支持多个选项，建议提供对应的 MASK 选项，方便用户在运行时切换到不同的模式。
 *
 * ```c
 * vsf_exti_channel_cfg_t cfg = {
 *     .mode = VSF_EXTI_EDGE_RISING_FALLING,
 *     ...
 * };
 * vsf_exti_channel_mask channel_mask = 0x00000001;
 * vsf_exti_config_channels(&vsf_hw_exti0, channel_mask, &cfg);
 * ```
 */
typedef enum vsf_exti_mode_t {
    /**
     * \~english
     * @brief No external interrupt detection enabled.
     * \~chinese
     * @brief 不启用外部中断检测。
     */
    VSF_EXTI_MODE_NONE                  = (0 << 0),

    /**
     * \~english
     * @brief Detect interrupts on rising edge of signal.
     * \~chinese
     * @brief 在信号上升沿检测中断。
     */
    VSF_EXTI_MODE_RISING                = (3 << 0),

    /**
     * \~english
     * @brief Detect interrupts on falling edge of signal.
     * \~chinese
     * @brief 在信号下降沿检测中断。
     */
    VSF_EXTI_MODE_FALLING               = (4 << 0),

    /**
     * \~english
     * @brief Detect interrupts on both rising and falling edges.
     * \~chinese
     * @brief 在信号上升沿和下降沿检测中断。
     */
    VSF_EXTI_MODE_EDGE_RISING_FALLING   = VSF_EXTI_MODE_RISING | VSF_EXTI_MODE_FALLING,

    /**
     * \~english
     * @brief Detect interrupts on low signal level.
     * \~chinese
     * @brief 在低电平信号上检测中断。
     */
    VSF_EXTI_MODE_LOW_LEVEL             = (1 << 0),
#define VSF_EXTI_MODE_LOW_LEVEL         VSF_EXTI_MODE_LOW_LEVEL

    /**
     * \~english
     * @brief Detect interrupts on high signal level.
     * \~chinese
     * @brief 在高电平信号上检测中断。
     */
    VSF_EXTI_MODE_HIGH_LEVEL            = (2 << 0),
#define VSF_EXTI_MODE_HIGH_LEVEL        VSF_EXTI_MODE_HIGH_LEVEL
} vsf_exti_mode_t;
#endif

enum {
    VSF_EXTI_MODE_MASK                  = VSF_EXTI_MODE_NONE
#ifdef VSF_EXTI_MODE_LOW_LEVEL
                                        | VSF_EXTI_MODE_LOW_LEVEL
#endif
#ifdef VSF_EXTI_MODE_HIGH_LEVEL
                                        | VSF_EXTI_MODE_HIGH_LEVEL
#endif
                                        | VSF_EXTI_MODE_RISING
                                        | VSF_EXTI_MODE_FALLING
                                        | VSF_EXTI_MODE_RISING_FALLING,
};

#if VSF_EXTI_CFG_REIMPLEMENT_TYPE_CFG == DISABLED
//! Pre-declaration of EXTI structures
typedef struct vsf_exti_t vsf_exti_t;

//! External interrupt callback function for exti
typedef void vsf_exti_isr_handler_t(void *target_ptr, vsf_exti_t *exti_ptr,
                                         vsf_exti_channel_mask_t channel_mask);

typedef struct vsf_exti_channel_cfg_t {
    vsf_exti_mode_t             mode;
    vsf_exti_isr_handler_t      *handler_fn;
    void                        *target_ptr;
    vsf_arch_prio_t              prio;
} vsf_exti_channel_cfg_t;
#endif

#if VSF_EXTI_CFG_REIMPLEMENT_TYPE_STATUS == DISABLED
/**
 * \~english
 * @brief Predefined VSF EXTI status that can be reimplemented in specific hal drivers.
 * \~chinese
 * @brief 预定义的 VSF EXTI 状态，可以在具体的 HAL 驱动中重新实现。
 */
typedef struct vsf_exti_status_t {
    vsf_exti_channel_mask_t     enable_mask;
    vsf_exti_channel_mask_t     status_mask;
} vsf_exti_status_t;
#endif

#if VSF_EXTI_CFG_REIMPLEMENT_TYPE_CAPABILITY == DISABLED
typedef struct vsf_exti_capability_t {
#if VSF_EXTI_CFG_INHERIT_HAL_CAPABILITY == ENABLED
    inherit(vsf_peripheral_capability_t)
#endif

    /**
     * \~english
     * @brief Support for level trigger.
     * \~chinese
     * @brief 支持电平触发模式。
     */
    uint8_t support_level_trigger   : 1;

    /**
     * \~english
     * @brief Support for edge trigger.
     * \~chinese
     * @brief 支持边沿触发模式。
     */
    uint8_t support_edge_trigger    : 1;

    /**
     * \~english
     * @brief Support for software trigger provided by vsf_exti_trigger.
     * \~chinese
     * @brief 支持由 vsf_exti_trigger 提供的软件触发模式。
     */
    uint8_t support_sw_trigger      : 1;

    /**
     * \~english
     * @brief interrupt number for exti.
     * Some EXTI supports multiple interrupt, and the actual length of irq_channel_mask MUST be irq_num
     * \~chinese
     * @brief 支持的中断的数量。
     * 有些 EXTI 对应多个中断，后续的 irq_channel_mask 的实际长度必须是 irq_num
     */
    uint8_t irq_num;

    /**
     * \~english
     * @brief channel number for exti.
     * \~chinese
     * @brief 支持的通道数量。
     */
    vsf_exti_channel_mask_t channel_mask;

    /**
     * \~english
     * @brief interrupt channel mask for corresponding irq.
     * Some EXTI supports multiple interrupt, and the actual length of irq_channel_mask MUST be irq_num
     * \~chinese
     * @brief 对应 irq 的中断通道掩码。
     * 有些 EXTI 对应多个中断，后续的 irq_channel_mask 的实际长度必须是 irq_num
     */
    vsf_exti_channel_mask_t irq_channel_mask[0];
} vsf_exti_capability_t;
#endif

typedef struct vsf_exti_op_t {
/// @cond
#undef __VSF_HAL_TEMPLATE_API
#define __VSF_HAL_TEMPLATE_API VSF_HAL_TEMPLATE_API_FP
/// @endcond

    VSF_EXTI_APIS(vsf_exti)
} vsf_exti_op_t;

#if VSF_EXTI_CFG_MULTI_CLASS == ENABLED
/**
 \~english
 When enabling multiple classes, the first member of all EXTI implementations
 needs to be of type const "vsf_exti_op_t * op"

 \~chinese
 当使能多类的时候，所有的具体 exti 实现的第一个成员都需要是 const vsf_exti_op_t * 类型。
 */
struct vsf_exti_t  {
    const vsf_exti_op_t * op;
};
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ PROTOTYPES ====================================*/

/**
 * \~english
 * @brief Initialize a EXTI instance
 * @param[in] exti_ptr: a pointer to structure @ref vsf_exti_t
 * @return vsf_err_t: VSF_ERR_NONE if initialization successful, otherwise returns error code
 *
 * @note It is not necessary to call vsf_exti_fini() to deinitialization.
 *       vsf_exti_init() should be called before any other EXTI API except vsf_exti_capability().
 *
 * \~chinese
 * @brief 初始化一个 EXTI 实例
 * @param[in] exti_ptr: 结构体 @ref vsf_exti_t 的指针
 * @return vsf_err_t: 如果初始化成功返回 VSF_ERR_NONE，否则返回错误码
 *
 * @note 初始化失败后不需要调用 vsf_exti_fini() 进行反初始化。
 *       vsf_ext6i_init() 应该在除 vsf_exti_capability() 之外的其他 EXTI API 之前调用。
 */
extern vsf_err_t vsf_exti_init(vsf_exti_t *exti_ptr);

/**
 * \~english
 * @brief Finalize a EXTI instance
 * @param[in] exti_ptr: a pointer to structure @ref vsf_exti_t
 * @return none
 *
 * \~chinese
 * @brief 终止一个 EXTI 实例
 * @param[in] exti_ptr: 指向结构体 @ref vsf_exti_t 的指针
 * @return 无
 */
extern void vsf_exti_fini(vsf_exti_t *exti_ptr);

/**
 * \~english
 * @brief Trigger EXTI channels
 * @param[in] exti_ptr: a pointer to structure @ref vsf_exti_t
 * @param[in] channel_mask: channel mask, each channel corresponds to one bit, the value of
 *            this bit 1 means the configuration will be applied to the corresponding channel
 * @return vsf_err_t: VSF_ERR_NONE if required channels Triggered, or a negative error code
 *
 * \~chinese
 * @brief 触发 EXTI 通道中断
 * @param[in] exti_ptr: 指向结构体 @ref vsf_exti_t 的指针
 * @param[in] channel_mask: 通道掩码，每一个通道对应一个位，该 bit 的值位 1 表示需要配置
 * @return vsf_err_t: 如果指定通道正常触发，返回 VSF_ERR_NONE，否则返回负值错误码。
 */
extern vsf_err_t vsf_exti_trigger(vsf_exti_t *exti_ptr, vsf_exti_channel_mask_t  channel_mask);

/**
 * \~english
 * @brief Get the status of EXTI instance
 * @param[in] exti_ptr: a pointer to structure @ref vsf_exti_t
 * @return vsf_exti_status_t: all status of current EXTI
 *
 * \~chinese
 * @brief 获取 EXTI 实例的状态
 * @param[in] exti_ptr: 指向结构体 @ref vsf_exti_t 的指针
 * @return vsf_exti_status_t: 返回当前 EXTI 的所有状态
 */
extern vsf_exti_status_t vsf_exti_status(vsf_exti_t *exti_ptr);

/**
 \~english
 @brief Configure one or more channels of the exti instance
 @param[in] exti_ptr: a pointer to structure @ref vsf_exti_t
 @param[in] channel_mask: channel mask, each channel corresponds to one bit, the value of
            this bit 1 means the configuration will be applied to the corresponding channel
 @param[in] cfg_ptr: a pointer to structure @ref vsf_exti_channel_cfg_t
 @return vsf_err_t: VSF_ERR_NONE if EXTI Configuration Successful, or a negative error code

 \~chinese
 @brief 配置 exti 实例的一个或者多个通道
 @param[in] exti_ptr: 结构体 vsf_exti_t 的指针，参考 @ref vsf_exti_t
 @param[in] channel_mask: 通道掩码，每一个通道对应一个位，该 bit 的值位 1 表示需要配置
 @param[in] cfg_ptr: 结构体 vsf_exti_channel_cfg_t 的指针，参考 @ref vsf_exti_channel_cfg_t
 @return vsf_err_t: 如果 EXTI 配置成功返回 VSF_ERR_NONE，失败返回负值错误码。
 */
extern vsf_err_t vsf_exti_config_channels(vsf_exti_t                *exti_ptr,
                                           vsf_exti_channel_mask_t  channel_mask,
                                           vsf_exti_channel_cfg_t   *cfg_ptr);

/**
 \~english
 @brief Get the capability of exti instance
 @param[in] exti_ptr: pointer to the structure vsf_exti_t, refer to @ref vsf_exti_t
 @return vsf_exti_capability_t: all capabilities of current EXTI @ref vsf_exti_capability_t

 \~chinese
 @brief 获取 exti 实例的能力
 @param[in] exti_ptr: 结构体 vsf_exti_t 的指针，参考 @ref vsf_exti_t
 @return vsf_exti_capability_t: 返回当前 EXTI 的所有能力 @ref vsf_exti_capability_t
 */
extern vsf_exti_capability_t vsf_exti_capability(vsf_exti_t *exti_ptr);

/**
 \~english
 @brief Enable interrupt of one or more channels
 @param[in] exti_ptr: a pointer to structure @ref vsf_exti_t
 @param[in] channel_mask: channel mask, each channel corresponds to one bit, 1 means the corresponding
            channel needs to be enabled, 0 means the corresponding channel does not need to be enabled
 @return vsf_err_t: VSF_ERR_NONE if successful, or a negative error code
 @note For some devices, the interrupt priority may be shared between channels on the exti.
 @note All pending interrupts should be cleared before interrupts are enabled.

 \~chinese
 @brief 使能指定的一个或者多个通道的中断
 @param[in] exti_ptr: 指向结构体 @ref vsf_exti_t 的指针
 @param[in] channel_mask: 通道掩码，每一个通道对应一个位，1 表示该位需要使能，0 表示该位不需要使能
 @return vsf_err_t: 如果成功返回 VSF_ERR_NONE，失败返回负值错误码
 @note 对于一些芯片，中断优先级可能是 exti 上所有通道共用的。
 @note 在中断使能之前，应该清除所有悬挂的中断。
 */
extern vsf_err_t vsf_exti_irq_enable(vsf_exti_t *exti_ptr, vsf_exti_channel_mask_t channel_mask);

/**
 \~english
 @brief Disable interrupt of one or more channels
 @param[in] exti_ptr: a pointer to structure @ref vsf_exti_t
 @param[in] channel_mask: channel mask, each channel corresponds to one bit, 1 means the corresponding
            channel needs to be disabled, 0 means the corresponding channel does not need to be disabled
 @return vsf_err_t: VSF_ERR_NONE if successful, or a negative error code
 @note For some devices, the interrupt settings may be shared between channels on the exti.

 \~chinese
 @brief 禁能指定通道的中断
 @param[in] exti_ptr: 指向结构体 @ref vsf_exti_t 的指针
 @param[in] channel_mask: 通道掩码，每一个通道对应一个位，1 表示该位需要禁能，0 表示该位不需要禁能
 @return vsf_err_t: 如果成功返回 VSF_ERR_NONE，失败返回负值错误码
 @note 对于一些芯片，中断设置可能是 exti 上所有通道共用的。
 */
extern vsf_err_t vsf_exti_irq_disable(vsf_exti_t *exti_ptr, vsf_exti_channel_mask_t channel_mask);

/**
 * \~english
 * @brief Clear interrupt flags of EXTI instance and return previous state
 * @param[in] exti_ptr: a pointer to structure @ref vsf_exti_t
 * @param[in] channel_mask: one or more values of enum @ref vsf_exti_channel_mask_t to clear
 * @return vsf_exti_channel_mask_t: the channel mask state before clearing (0 if no flags were set)
 *
 * @note This function attempts to clear the specified interrupt flags if they are set,
 *       and returns the state of those flags before clearing. This is useful for
 *       polling operations and determining if interrupts occurred.
 *       Note that if interrupts are enabled and an interrupt handler is active,
 *       the interrupt handler may clear the interrupt flags automatically.
 *       In such cases, this function will return 0 even if interrupts occurred.
 *
 * \~chinese
 * @brief 清除 EXTI 实例的中断标志并返回之前的状态
 * @param[in] exti_ptr: 指向结构体 @ref vsf_exti_t 的指针
 * @param[in] channel_mask: 要清除的一个或多个枚举 vsf_exti_channel_mask_t 值的按位或
 * @return vsf_exti_channel_mask_t: 清除前的通道掩码状态（如果没有标志被设置则返回0）
 *
 * @note 此函数尝试清除指定的中断标志（如果它们已设置），并返回清除前这些标志的状态。
 *       这对于轮询操作和确定是否发生了中断很有用。
 *       注意：如果中断已启用且中断处理函数处于活动状态，中断处理函数可能会自动清除中断标志。
 *       在这种情况下，即使发生了中断，此函数也会返回0。
 */
extern vsf_exti_channel_mask_t vsf_exti_irq_clear(vsf_exti_t *exti_ptr, vsf_exti_channel_mask_t channel_mask);

/*============================ MACROS ========================================*/

/// @cond
#if VSF_EXTI_CFG_FUNCTION_RENAME == ENABLED
#   define __vsf_exti_t                                 VSF_MCONNECT(VSF_EXTI_CFG_PREFIX, _exti_t)
#   define vsf_exti_init(__EXTI)                        VSF_MCONNECT(VSF_EXTI_CFG_PREFIX, _exti_init)                   ((__vsf_exti_t *)(__EXTI))
#   define vsf_exti_fini(__EXTI)                        VSF_MCONNECT(VSF_EXTI_CFG_PREFIX, _exti_fini)                   ((__vsf_exti_t *)(__EXTI))
#   define vsf_exti_status(__EXTI)                      VSF_MCONNECT(VSF_EXTI_CFG_PREFIX, _exti_status)                 ((__vsf_exti_t *)(__EXTI))
#   define vsf_exti_capability(__EXTI)                  VSF_MCONNECT(VSF_EXTI_CFG_PREFIX, _exti_capability)             ((__vsf_exti_t *)(__EXTI))
#   define vsf_exti_trigger(__EXTI, ...)                VSF_MCONNECT(VSF_EXTI_CFG_PREFIX, _exti_trigger)                ((__vsf_exti_t *)(__EXTI), ##__VA_ARGS__)
#   define vsf_exti_config_channels(__EXTI, ...)        VSF_MCONNECT(VSF_EXTI_CFG_PREFIX, _exti_config_channels)        ((__vsf_exti_t *)(__EXTI), ##__VA_ARGS__)
#   define vsf_exti_irq_enable(__EXTI, ...)             VSF_MCONNECT(VSF_EXTI_CFG_PREFIX, _exti_irq_enable)             ((__vsf_exti_t *)(__EXTI), ##__VA_ARGS__)
#   define vsf_exti_irq_disable(__EXTI, ...)            VSF_MCONNECT(VSF_EXTI_CFG_PREFIX, _exti_irq_disable)            ((__vsf_exti_t *)(__EXTI), ##__VA_ARGS__)
#   define vsf_exti_irq_clear(__EXTI, ...)              VSF_MCONNECT(VSF_EXTI_CFG_PREFIX, _exti_irq_clear)              ((__vsf_exti_t *)(__EXTI), ##__VA_ARGS__)
#endif
/// @endcond

#ifdef __cplusplus
}
#endif

#endif  /* __VSF_TEMPLATE_EXTI_H__ */
