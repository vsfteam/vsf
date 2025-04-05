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

#ifndef __VSF_TEMPLATE_WDT_H__
#define __VSF_TEMPLATE_WDT_H__

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
#ifndef VSF_WDT_CFG_MULTI_CLASS
#   define VSF_WDT_CFG_MULTI_CLASS                      ENABLED
#endif

#if defined(VSF_HW_WDT_COUNT) && !defined(VSF_HW_WDT_MASK)
/**
 * \~english
 * @brief Define WDT hardware mask if count is defined
 *
 * \~chinese
 * @brief 如果定义了 WDT 硬件数量，则定义对应的掩码
 */
#   define VSF_HW_WDT_MASK                              VSF_HAL_COUNT_TO_MASK(VSF_HW_WDT_COUNT)
#endif

#if defined(VSF_HW_WDT_MASK) && !defined(VSF_HW_WDT_COUNT)
/**
 * \~english
 * @brief Define WDT hardware count if mask is defined
 *
 * \~chinese
 * @brief 如果定义了 WDT 硬件掩码，则定义对应的数量
 */
#   define VSF_HW_WDT_COUNT                             VSF_HAL_MASK_TO_COUNT(VSF_HW_WDT_MASK)
#endif

/**
 * \~english
 * @brief We can redefine macro VSF_WDT_CFG_PREFIX to specify a prefix
 * to call a specific driver directly in the application code.
 * Example:
 * @code
 * #define VSF_WDT_CFG_PREFIX    my_device    // Use my device hardware WDT driver
 * vsf_wdt_init()                             // Will be expanded to my_device_wdt_init()
 * @endcode
 * @note This macro is only valid when VSF_WDT_CFG_FUNCTION_RENAME is ENABLED.
 *
 * \~chinese
 * @brief 可重新定义宏 VSF_WDT_CFG_PREFIX，以在应用代码中直接调用
 * 特定驱动。
 * 示例:
 * @code
 * #define VSF_WDT_CFG_PREFIX    my_device    // 使用自定义设备的 WDT 驱动
 * vsf_wdt_init()                             // 将展开为 my_device_wdt_init()
 * @endcode
 * @note 这个宏仅在 VSF_WDT_CFG_FUNCTION_RENAME 为 ENABLED 时有效。
 */
#ifndef VSF_WDT_CFG_PREFIX
#   if (VSF_WDT_CFG_MULTI_CLASS == DISABLED) && defined(VSF_HW_WDT_COUNT) && (VSF_HW_WDT_COUNT != 0)
#       define VSF_WDT_CFG_PREFIX                       vsf_hw
#   else
#       define VSF_WDT_CFG_PREFIX                       vsf
#   endif
#endif

/**
 * \~english
 * @brief After define VSF_WDT_CFG_FUNCTION_RENAME to DISABLED, calling any
 * vsf_wdt_xxx() function will not be renamed. It has higher priority than
 * VSF_WDT_CFG_PREFIX
 *
 * \~chinese
 * @brief 定义宏 VSF_WDT_CFG_FUNCTION_RENAME 的值为 DISABLED 后，调用任何
 * vsf_wdt_xxx() 函数将不会被重命名。它的优先级将高于 VSF_WDT_CFG_PREFIX。
 */
#ifndef VSF_WDT_CFG_FUNCTION_RENAME
#   define VSF_WDT_CFG_FUNCTION_RENAME                  ENABLED
#endif

/**
 * \~english
 * @brief In specific hardware driver, we can enable macro
 * VSF_WDT_CFG_REIMPLEMENT_TYPE_MODE to redefine enum @ref vsf_wdt_mode_t.
 *
 * \~chinese
 * @brief 在具体硬件驱动中，我们可启用宏 VSF_WDT_CFG_REIMPLEMENT_TYPE_MODE
 * 来重新定义枚举 @ref vsf_wdt_mode_t。
 */
#ifndef VSF_WDT_CFG_REIMPLEMENT_TYPE_MODE
#   define VSF_WDT_CFG_REIMPLEMENT_TYPE_MODE            DISABLED
#endif

/**
 * \~english
 * @brief In specific hardware driver, we can enable macro
 * VSF_WDT_CFG_REIMPLEMENT_TYPE_CFG to redefine struct
 * @ref vsf_wdt_cfg_t. For compatibility, members should not be
 * deleted when redefining it.
 *
 * \~chinese
 * @brief 在具体硬件驱动中，我们可以启用宏 VSF_WDT_CFG_REIMPLEMENT_TYPE_CFG
 * 来重新定义结构体 @ref vsf_wdt_cfg_t。为保证兼容性，重新定义时不应
 * 删除成员。
 */
#if VSF_WDT_CFG_REIMPLEMENT_TYPE_CFG == DISABLED
#    define VSF_WDT_CFG_REIMPLEMENT_TYPE_CFG DISABLED
#endif

/**
 * \~english
 * @brief In specific hardware driver, we can enable macro
 * VSF_WDT_CFG_REIMPLEMENT_TYPE_CAPABILITY to redefine struct
 * @ref vsf_wdt_capability_t. For compatibility, members should not
 * be deleted when redefining it.
 *
 * \~chinese
 * @brief 在具体硬件驱动中，我们可以启用宏
 * VSF_WDT_CFG_REIMPLEMENT_TYPE_CAPABILITY 来重新定义结构体
 * @ref vsf_wdt_capability_t。为保证兼容性，重新定义时不应删除成员。
 */
#if VSF_WDT_CFG_REIMPLEMENT_TYPE_CAPABILITY == DISABLED
#    define VSF_WDT_CFG_REIMPLEMENT_TYPE_CAPABILITY DISABLED
#endif

/**
 * \~english
 * @brief In specific hardware driver, we can enable macro
 * VSF_WDT_CFG_INHERIT_HAL_CAPABILITY to inherit the structure
 * @ref vsf_peripheral_capability_t.
 *
 * \~chinese
 * @brief 在具体硬件驱动中，我们可以启用宏 VSF_WDT_CFG_INHERIT_HAL_CAPABILITY
 * 来继承结构体 @ref vsf_peripheral_capability_t。
 */
#ifndef VSF_WDT_CFG_INHERIT_HAL_CAPABILITY
#   define VSF_WDT_CFG_INHERIT_HAL_CAPABILITY            ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

/**
 * \~english
 * @brief WDT API template, used to generate WDT type, specific prefix function declarations, etc.
 * @param[in] __prefix The prefix used for generating WDT functions.
 *
 * \~chinese
 * @brief WDT API 模板，用于生成 WDT 类型、特定前缀的函数声明等。
 * @param[in] __prefix 用于生成 WDT 函数的前缀。
 */
#define VSF_WDT_APIS(__prefix) \
    __VSF_HAL_TEMPLATE_API(__prefix, vsf_err_t,            wdt, init,        VSF_MCONNECT(__prefix, _wdt_t) *wdt_ptr, vsf_wdt_cfg_t *cfg_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix, void,                 wdt, fini,        VSF_MCONNECT(__prefix, _wdt_t) *wdt_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix, fsm_rt_t,             wdt, enable,      VSF_MCONNECT(__prefix, _wdt_t) *wdt_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix, fsm_rt_t,             wdt, disable,     VSF_MCONNECT(__prefix, _wdt_t) *wdt_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix, vsf_wdt_capability_t, wdt, capability,  VSF_MCONNECT(__prefix, _wdt_t) *wdt_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix, void,                 wdt, feed,        VSF_MCONNECT(__prefix, _wdt_t) *wdt_ptr)

/*============================ TYPES =========================================*/

#if VSF_WDT_CFG_REIMPLEMENT_TYPE_MODE == DISABLED
/**
 * \~english
 * @brief Predefined VSF WDT modes that can be reimplemented in specific HAL
 * drivers.
 * \~chinese
 * @brief 预定义的 VSF WDT 模式，可以在具体的 HAL 驱动重新实现。
 *
 * \~english
 * The following modes must be retained even if not supported by hardware.
 * If hardware supports more modes, we can implement them in the hardware driver:
 * - VSF_WDT_MODE_NO_EARLY_WAKEUP
 * - VSF_WDT_MODE_EARLY_WAKEUP
 * - VSF_WDT_MODE_RESET_NONE
 * - VSF_WDT_MODE_RESET_CPU
 * - VSF_WDT_MODE_RESET_SOC
 * \~chinese
 * 以下的模式即使硬件不支持也必须保留，如果硬件支持更多模式，我们可以在硬件驱动里实现它：
 * - VSF_WDT_MODE_NO_EARLY_WAKEUP
 * - VSF_WDT_MODE_EARLY_WAKEUP
 * - VSF_WDT_MODE_RESET_NONE
 * - VSF_WDT_MODE_RESET_CPU
 * - VSF_WDT_MODE_RESET_SOC
 *
 * \~english
 * In specific drivers, we can implement optional modes. Optional modes require
 * the driver to provide one or more enumeration options, and provide macros
 * with the same name (users can determine whether to support the mode at
 * compile time). If these options are N to 1, the corresponding MASK option is
 * also required (users can select different modes at runtime).
 * \~chinese
 * 在特定驱动里，我们可以实现可选的模式。可选模式需要该驱动提供一个或多个枚举选项，
 * 并且提供和枚举选项同名的宏（用户可以在编译时判断是否支持该模式）。
 * 如果这些选项是 N 选 1 的关系，还需要提供对应的 MASK
 * 选项（用户可以在运行时选择不同的模式）
 */
typedef enum vsf_wdt_mode_t {
    VSF_WDT_MODE_NO_EARLY_WAKEUP = (0 << 0),    //!< \~english No early wakeup \~chinese 无提前唤醒
    VSF_WDT_MODE_EARLY_WAKEUP    = (1 << 0),    //!< \~english Early wakeup \~chinese 提前唤醒

    VSF_WDT_MODE_RESET_NONE      = (0 << 1),    //!< \~english No reset when timeout \~chinese 超时时不复位
    VSF_WDT_MODE_RESET_CPU       = (1 << 1),    //!< \~english Reset CPU when timeout \~chinese 超时时复位 CPU
    VSF_WDT_MODE_RESET_SOC       = (2 << 1),    //!< \~english Reset SOC when timeout \~chinese 超时时复位 SOC

	/*
    /// \~english optional: prescaler mode
    /// \~chinese 可选项：预分频器模式
    VSF_WDT_MODE_PRESCALER_1    = (0 << 2),
    VSF_WDT_MODE_PRESCALER_2    = (1 << 2),
    VSF_WDT_MODE_PRESCALER_4    = (2 << 2),
    #define VSF_WDT_MODE_PRESCALER_8        VSF_WDT_MODE_PRESCALER_8
    #define VSF_WDT_MODE_PRESCALER_1        VSF_WDT_MODE_PRESCALER_1
    #define VSF_WDT_MODE_PRESCALER_2        VSF_WDT_MODE_PRESCALER_2
    #define VSF_WDT_MODE_PRESCALER_4        VSF_WDT_MODE_PRESCALER_4
    #define VSF_WDT_MODE_PRESCALER_MASK     (0x03 << 2)
    */
} vsf_wdt_mode_t;
#endif

/**
 * \~english
 * @brief WDT mode completion, used to simplify the definition of WDT mode.
 * \~chinese
 * @brief WDT 模式补全，用来简化 WDT 模式的定义
 */
enum {
    //! \~english early wakeup mask
    //! \~chinese 提前唤醒掩码
    VSF_WDT_MODE_EARLY_WAKEUP_MASK  = VSF_WDT_MODE_NO_EARLY_WAKEUP
                                    | VSF_WDT_MODE_EARLY_WAKEUP,

#ifndef VSF_WDT_MODE_RESET_MASK
    //! \~english reset mask
    //! \~chinese 复位掩码
    VSF_WDT_MODE_RESET_MASK         = VSF_WDT_MODE_RESET_NONE
                                    | VSF_WDT_MODE_RESET_CPU
                                    | VSF_WDT_MODE_RESET_SOC,
#endif

#ifndef VSF_WDT_MODE_ALL_BITS_MASK
    VSF_WDT_MODE_ALL_BITS_MASK      = VSF_WDT_MODE_EARLY_WAKEUP_MASK
                                    | VSF_WDT_MODE_RESET_MASK
#ifdef VSF_WDT_MODE_PRESCALER_MASK
                                    | VSF_WDT_MODE_PRESCALER_MASK
#endif
#endif
};

#if VSF_WDT_CFG_REIMPLEMENT_TYPE_CFG == DISABLED
/**
 * \~english @brief WDT forward declaration.
 * \~chinese @brief WDT 前置声明.
 */
typedef struct vsf_wdt_t vsf_wdt_t;

/**
 * \~english @brief WDT interrupt handler type declaration.
 * @param[in,out] target_ptr: user defined target pointer
 * @param[in,out] wdt_ptr: @ref vsf_wdt_t watchdog timer structure pointer
 * \~chinese @brief WDT 中断处理函数的类型声明.
 * @param[in,out] target_ptr: 用户自定义目标指针
 * @param[in,out] wdt_ptr: @ref vsf_wdt_t 看门狗定时器结构指针
 */
typedef void vsf_wdt_isr_handler_t(void *target_ptr, vsf_wdt_t *wdt_ptr);

/**
 * \~english @brief WDT interrupt configuration structure.
 * \~chinese @brief WDT 中断配置结构体.
 */
typedef struct vsf_wdt_isr_t {
    vsf_wdt_isr_handler_t  *handler_fn; //!< \~english Interrupt handler function pointer \~chinese 中断处理函数指针
    void                   *target_ptr; //!< \~english User-defined target pointer \~chinese 用户自定义目标指针
    vsf_arch_prio_t        prio;        //!< \~english Interrupt priority \~chinese 中断优先级
} vsf_wdt_isr_t;

/**
 * \~english
 * @brief WDT configuration structure.
 *
 * \~chinese
 * @brief WDT 配置结构体
 */
typedef struct vsf_wdt_cfg_t {
    vsf_wdt_mode_t  mode;   //!< \~english WDT mode, refer to @ref vsf_wdt_mode_t \~chinese WDT 模式，参考 @ref vsf_wdt_mode_t
    uint32_t        max_ms; //!< \~english Maximum timeout in milliseconds \~chinese 最大超时时间（毫秒）
    uint32_t        min_ms; //!< \~english Minimum timeout in milliseconds \~chinese 最小超时时间（毫秒）
    vsf_wdt_isr_t   isr;    //!< \~english WDT interrupt configuration, refer to @ref vsf_wdt_isr_t \~chinese WDT 中断配置，参考 @ref vsf_wdt_isr_t
} vsf_wdt_cfg_t;
#endif

#if VSF_WDT_CFG_REIMPLEMENT_TYPE_CAPABILITY == DISABLED
/**
 * \~english
 * @brief Predefined VSF WDT capability that can be reimplemented in specific hal drivers.
 * The vsf_wdt_capability_t structure defines the features supported by the WDT hardware.
 * When reimplementing this structure, all existing members must be preserved.
 * \~chinese
 * @brief 预定义的 VSF WDT 能力，可以在具体的 HAL 驱动重新实现。
 * vsf_wdt_capability_t 结构体定义了 WDT 硬件支持的特性，重新实现这
 * 个结构体的时候需要保留所有现有的成员。
 */
typedef struct vsf_wdt_capability_t {
#if VSF_WDT_CFG_INHERIT_HAL_CAPABILITY == ENABLED
    inherit(vsf_peripheral_capability_t)    //!< \~english Inherit the capability of peripheral \~chinese 继承外设的能力
#endif

    uint8_t support_early_wakeup : 1;       //!< \~english Indicates if early wakeup is supported \~chinese 指示是否支持提前唤醒
    uint8_t support_reset_none   : 1;       //!< \~english Indicates if no reset mode is supported \~chinese 指示是否支持无复位模式
    uint8_t support_reset_cpu    : 1;       //!< \~english Indicates if core reset mode is supported \~chinese 指示是否支持内核复位模式
    uint8_t support_reset_soc    : 1;       //!< \~english Indicates if SoC reset mode is supported \~chinese 指示是否支持 SoC 复位模式
    uint8_t support_disable      : 1;       //!< \~english Indicates if WDT can be disabled after startup \~chinese 指示是否支持启动之后禁用 WDT
    uint8_t support_min_timeout  : 1;       //!< \~english Indicates if minimum timeout is supported \~chinese 指示是否支持最小超时时间
    uint32_t max_timeout_ms;                //!< \~english Maximum timeout in milliseconds \~chinese 最大超时时间（毫秒）
} vsf_wdt_capability_t;
#endif

/**
 * \~english @brief WDT operation function pointer type, used for WDT Multi Class support
 * \~chinese @brief WDT 操作函数指针类型，用于 WDT Multi Class 支持
 */
typedef struct vsf_wdt_op_t {
/// @cond
#undef  __VSF_HAL_TEMPLATE_API
#define __VSF_HAL_TEMPLATE_API VSF_HAL_TEMPLATE_API_FP
/// @endcond

    VSF_WDT_APIS(vsf)
} vsf_wdt_op_t;

#if VSF_WDT_CFG_MULTI_CLASS == ENABLED
/**
 * \~english @brief WDT instance structure, used for WDT Multi Class support, not needed in non Multi Class mode
 * \~chinese @brief WDT 实例结构体，用于 WDT Multi Class 支持，非 Multi Class 模式下不需要
 */
struct vsf_wdt_t  {
    const vsf_wdt_op_t * op;
};
#endif

/*============================ PROTOTYPES ====================================*/

/**
 * \~english
 * @brief Initialize a WDT instance.
 * @param[in] wdt_ptr: a pointer to structure @ref vsf_wdt_t
 * @param[in] cfg_ptr: a pointer to structure @ref vsf_wdt_cfg_t
 * @return vsf_err_t: VSF_ERR_NONE if WDT was initialized, or a negative error code
 *
 * @note It is not necessary to call vsf_wdt_fini() for deinitialization.
 *       vsf_wdt_init() should be called before any other WDT API except vsf_wdt_capability().
 *
 * \~chinese
 * @brief 初始化一个 WDT 实例
 * @param[in] wdt_ptr：指向结构体 @ref vsf_wdt_t 的指针
 * @param[in] cfg_ptr：指向结构体 @ref vsf_wdt_cfg_t 的指针
 * @return vsf_err_t：如果 WDT 初始化成功返回 VSF_ERR_NONE，失败返回负数
 *
 * @note 初始化失败后不需要调用 vsf_wdt_fini() 进行反初始化。
 *       必须在调用除 vsf_wdt_capability() 外的其他 WDT API 之前调用 vsf_wdt_init()。
 */
extern vsf_err_t vsf_wdt_init(vsf_wdt_t *wdt_ptr, vsf_wdt_cfg_t *cfg_ptr);

/**
 * \~english
 * @brief Finalize a WDT instance.
 * @param[in] wdt_ptr: a pointer to structure @ref vsf_wdt_t
 * @return none
 *
 * \~chinese
 * @brief 终止一个 WDT 实例
 * @param[in] wdt_ptr: 指向结构体 @ref vsf_wdt_t 的指针
 * @return 无
 */
extern void vsf_wdt_fini(vsf_wdt_t *wdt_ptr);

/**
 * \~english
 * @brief Enable a WDT instance.
 * @param[in] wdt_ptr: a pointer to structure @ref vsf_wdt_t
 * @return fsm_rt_t: FSM_RT_CPL if WDT was enabled, fsm_rt_on_going if WDT is still enabling
 *
 * \~chinese
 * @brief 启用一个 WDT 实例
 * @param[in] wdt_ptr: 指向结构体 @ref vsf_wdt_t 的指针
 * @return fsm_rt_t: 如果 WDT 实例已启用返回 FSM_RT_CPL，如果 WDT 实例正在启用过程中返回 fsm_rt_on_going
 */
extern fsm_rt_t vsf_wdt_enable(vsf_wdt_t *wdt_ptr);

/**
 * \~english
 * @brief Disable a WDT instance.
 * @param[in] wdt_ptr: a pointer to structure @ref vsf_wdt_t
 * @return fsm_rt_t: FSM_RT_CPL if WDT was disabled, fsm_rt_on_going if WDT is still disabling
 *
 * \~chinese
 * @brief 禁用一个 WDT 实例
 * @param[in] wdt_ptr: 指向结构体 @ref vsf_wdt_t 的指针
 * @return fsm_rt_t: 如果 WDT 实例已禁用返回 FSM_RT_CPL，如果 WDT 实例正在禁用过程中返回 fsm_rt_on_going
 */
extern fsm_rt_t vsf_wdt_disable(vsf_wdt_t *wdt_ptr);

/**
 * \~english
 * @brief Get the capability of WDT instance.
 * @param[in] wdt_ptr: a pointer to structure @ref vsf_wdt_t
 * @return vsf_wdt_capability_t: all capability of current WDT @ref vsf_wdt_capability_t
 *
 * \~chinese
 * @brief 获取 WDT 实例的能力
 * @param[in] wdt_ptr: 指向结构体 @ref vsf_wdt_t 的指针
 * @return vsf_wdt_capability_t: 返回当前 WDT 的所有能力 @ref vsf_wdt_capability_t
 */
extern vsf_wdt_capability_t vsf_wdt_capability(vsf_wdt_t *wdt_ptr);

/**
 * \~english
 * @brief WDT feed operation
 * @param[in] wdt_ptr: a pointer to structure @ref vsf_wdt_t
 *
 * \~chinese
 * @brief WDT 喂狗操作
 * @param[in] wdt_ptr: 指向结构体 @ref vsf_wdt_t 的指针
 */
extern void vsf_wdt_feed(vsf_wdt_t *wdt_ptr);

/*============================ MACROFIED FUNCTIONS ===========================*/

/// @cond
#if VSF_WDT_CFG_FUNCTION_RENAME == ENABLED
#   define __vsf_wdt_t                VSF_MCONNECT(VSF_WDT_CFG_PREFIX, _wdt_t)
#   define vsf_wdt_init(__WDT, ...)   VSF_MCONNECT(VSF_WDT_CFG_PREFIX, _wdt_init)                 ((__vsf_wdt_t *)(__WDT), ##__VA_ARGS__)
#   define vsf_wdt_fini(__WDT)        VSF_MCONNECT(VSF_WDT_CFG_PREFIX, _wdt_fini)                 ((__vsf_wdt_t *)(__WDT))
#   define vsf_wdt_enable(__WDT)      VSF_MCONNECT(VSF_WDT_CFG_PREFIX, _wdt_enable)               ((__vsf_wdt_t *)(__WDT))
#   define vsf_wdt_disable(__WDT)     VSF_MCONNECT(VSF_WDT_CFG_PREFIX, _wdt_disable)              ((__vsf_wdt_t *)(__WDT))
#   define vsf_wdt_capability(__WDT)  VSF_MCONNECT(VSF_WDT_CFG_PREFIX, _wdt_capability)           ((__vsf_wdt_t *)(__WDT))
#   define vsf_wdt_feed(__WDT, ...)   VSF_MCONNECT(VSF_WDT_CFG_PREFIX, _wdt_feed)                 ((__vsf_wdt_t *)(__WDT))
#endif
/// @endcond

#ifdef __cplusplus
}
#endif

#endif  /*__VSF_TEMPLATE_WDT_H__*/
