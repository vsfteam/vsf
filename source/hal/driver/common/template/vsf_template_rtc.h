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

#ifndef __VSF_TEMPLATE_RTC_H__
#define __VSF_TEMPLATE_RTC_H__

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
#ifndef VSF_RTC_CFG_MULTI_CLASS
#   define VSF_RTC_CFG_MULTI_CLASS                  ENABLED
#endif

/**
 * \~english
 * @brief Convert count to mask in specific hardware driver
 * @note This macro is used to convert a count value to its corresponding bit mask.
 *       For example, if VSF_HW_RTC_COUNT is 3, VSF_HW_RTC_MASK will be 0x07 (binary: 111).
 * \~chinese
 * @brief 在特定硬件驱动中将数量转换为掩码
 * @note 此宏用于将计数值转换为其对应的位掩码。
 *       例如，如果 VSF_HW_RTC_COUNT 为 3，则 VSF_HW_RTC_MASK 将为 0x07（二进制：111）。
 */
#if defined(VSF_HW_RTC_COUNT) && !defined(VSF_HW_RTC_MASK)
#   define VSF_HW_RTC_MASK                          VSF_HAL_COUNT_TO_MASK(VSF_HW_RTC_COUNT)
#endif

/**
 * \~english
 * @brief Convert mask to count in specific hardware driver.
 * \~chinese
 * @brief 在特定硬件驱动中将掩码转换为数量。
 */
#if defined(VSF_HW_RTC_MASK) && !defined(VSF_HW_RTC_COUNT)
#   define VSF_HW_RTC_COUNT                         VSF_HAL_MASK_TO_COUNT(VSF_HW_RTC_MASK)
#endif

/**
 * \~english
 * @brief We can redefine macro VSF_RTC_CFG_PREFIX to specify a prefix
 * to call a specific driver directly in the application code.
 * \~chinese
 * @brief 可以重新定义宏 VSF_RTC_CFG_PREFIX，以在应用代码中直接调用
 * 特定驱动函数。
 */
#ifndef VSF_RTC_CFG_PREFIX
#   if VSF_RTC_CFG_MULTI_CLASS == ENABLED
#       define VSF_RTC_CFG_PREFIX                   vsf
#   elif defined(VSF_HW_RTC_COUNT) && (VSF_HW_RTC_COUNT != 0)
#       define VSF_RTC_CFG_PREFIX                   vsf_hw
#   else
#       define VSF_RTC_CFG_PREFIX                   vsf
#   endif
#endif

/**
 * \~english
 * @brief Disable VSF_RTC_CFG_FUNCTION_RENAME to use the original function
 * names (e.g., vsf_rtc_init()).
 * \~chinese
 * @brief 禁用 VSF_RTC_CFG_FUNCTION_RENAME 以使用原始函数名
 * (例如 vsf_rtc_init())。
 */
#ifndef VSF_RTC_CFG_FUNCTION_RENAME
#   define VSF_RTC_CFG_FUNCTION_RENAME              ENABLED
#endif

/**
 * \~english
 * @brief Enable macro VSF_RTC_CFG_REIMPLEMENT_TYPE_IRQ_MASK in specific hardware
 * drivers to redefine enum @ref vsf_rtc_irq_mask_t.
 * \~chinese
 * @brief 在特定硬件驱动中启用宏 VSF_RTC_CFG_REIMPLEMENT_TYPE_IRQ_MASK
 * 来重新定义枚举 @ref vsf_rtc_irq_mask_t。
 */
#ifndef VSF_RTC_CFG_REIMPLEMENT_TYPE_IRQ_MASK
#   define VSF_RTC_CFG_REIMPLEMENT_TYPE_IRQ_MASK    DISABLED
#endif

/**
 * \~english
 * @brief Enable reimplementation of configuration type
 * \~chinese
 * @brief 启用配置类型的重新实现
 */
#ifndef VSF_RTC_CFG_REIMPLEMENT_TYPE_CFG
#   define VSF_RTC_CFG_REIMPLEMENT_TYPE_CFG        DISABLED
#endif

/**
 * \~english
 * @brief Enable reimplementation of capability type
 * \~chinese
 * @brief 启用能力类型的重新实现
 */
#ifndef VSF_RTC_CFG_REIMPLEMENT_TYPE_CAPABILITY
#   define VSF_RTC_CFG_REIMPLEMENT_TYPE_CAPABILITY DISABLED
#endif

/**
 * \~english
 * @brief Configure the data type used for time representation
 * @note Uses uint64_t by default for maximum range
 * \~chinese
 * @brief 配置用于时间表示的数据类型
 * @note 默认使用 uint64_t 以获得最大范围
 */
#ifndef VSF_RTC_CFG_TIME_TYPE
#   define VSF_RTC_CFG_TIME_TYPE                    uint64_t
#endif

/**
 * \~english
 * @brief Enable inheritance of HAL capability
 * @note When enabled RTC capability structure inherits from vsf_peripheral_capability_t
 * \~chinese
 * @brief 启用 HAL 能力继承
 * @note 启用时，RTC 能力结构体继承自 vsf_peripheral_capability_t
 */
#ifndef VSF_RTC_CFG_INHERIT_HAL_CAPABILITY
#   define VSF_RTC_CFG_INHERIT_HAL_CAPABILITY        ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

/**
 * \~english
 * @brief RTC API template macro, used to generate RTC type, specific prefix
 * function declarations, etc.
 * @param[in] __prefix_name The prefix to use for all generated functions
 *
 * \~chinese
 * @brief RTC API 模板宏，用于生成 RTC 类型、特定前缀的函数声明等。
 * @param[in] __prefix_name 用于所有生成函数的前缀
 */
#define VSF_RTC_APIS(__prefix_name) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,            rtc, init,       VSF_MCONNECT(__prefix_name, _rtc_t) *rtc_ptr, vsf_rtc_cfg_t *cfg_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                 rtc, fini,       VSF_MCONNECT(__prefix_name, _rtc_t) *rtc_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,             rtc, enable,     VSF_MCONNECT(__prefix_name, _rtc_t) *rtc_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,             rtc, disable,    VSF_MCONNECT(__prefix_name, _rtc_t) *rtc_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_rtc_capability_t, rtc, capability, VSF_MCONNECT(__prefix_name, _rtc_t) *rtc_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,            rtc, get,        VSF_MCONNECT(__prefix_name, _rtc_t) *rtc_ptr, vsf_rtc_tm_t *rtc_tm) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,            rtc, set,        VSF_MCONNECT(__prefix_name, _rtc_t) *rtc_ptr, const vsf_rtc_tm_t *rtc_tm) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,            rtc, get_time,   VSF_MCONNECT(__prefix_name, _rtc_t) *rtc_ptr, vsf_rtc_time_t *second_ptr, vsf_rtc_time_t *millisecond_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,            rtc, set_time,   VSF_MCONNECT(__prefix_name, _rtc_t) *rtc_ptr, vsf_rtc_time_t seconds, vsf_rtc_time_t milliseconds)

/*============================ TYPES =========================================*/

#if VSF_RTC_CFG_REIMPLEMENT_TYPE_IRQ_MASK == DISABLED
/**
 * \~english
 * @brief RTC interrupt mask definition.
 * Used to identify and manage different types of RTC interrupts.
 * \~chinese
 * @brief RTC 中断掩码定义。
 * 用于识别和管理不同类型的 RTC 中断。
 */
typedef enum vsf_rtc_irq_mask_t {
    /**
     * \~english
     * @brief Alarm interrupt mask.
     * Triggers when the RTC time matches the configured alarm time.
     * \~chinese
     * @brief 闹钟中断掩码。
     * 当 RTC 时间与配置的闹钟时间匹配时触发。
     */
    VSF_RTC_IRQ_MASK_ALARM = (1 << 0),
} vsf_rtc_irq_mask_t;
#endif

/**
 * \~english
 * @brief RTC interrupt mask completion.
 * Used to define the combination of all available interrupt masks.
 * \~chinese
 * @brief RTC 中断掩码补充。
 * 用于定义所有可用中断掩码的组合。
 */
enum {
#ifndef VSF_RTC_IRQ_ALL_BITS_MASK
    /**
     * \~english
     * @brief All supported RTC interrupt masks combined.
     * Includes all interrupt types the RTC hardware supports.
     * \~chinese
     * @brief 所有支持的 RTC 中断掩码的组合。
     * 包括 RTC 硬件支持的所有中断类型。
     */
    VSF_RTC_IRQ_ALL_BITS_MASK = VSF_RTC_IRQ_MASK_ALARM,
#endif
};

/**
 * \~english
 * @brief RTC time type definition
 * @note This type is used for storing time values
 * \~chinese
 * @brief RTC 时间类型定义
 * @note 此类型用于存储时间值
 */
typedef VSF_RTC_CFG_TIME_TYPE vsf_rtc_time_t;

/**
 * \~english
 * @brief Time structure for RTC operations.
 * Used to represent date and time information in a standardized format.
 * \~chinese
 * @brief RTC 操作的时间结构体。
 * 用于以标准化格式表示日期和时间信息。
 */
typedef struct vsf_rtc_tm_t {
    /**
     * \~english
     * @brief Seconds component of time [0-59].
     * \~chinese
     * @brief 时间的秒数分量 [0-59]。
     */
    uint8_t tm_sec;

    /**
     * \~english
     * @brief Minutes component of time [0-59].
     * \~chinese
     * @brief 时间的分钟分量 [0-59]。
     */
    uint8_t tm_min;

    /**
     * \~english
     * @brief Hours component of time [0-23].
     * Uses 24-hour format.
     * \~chinese
     * @brief 时间的小时分量 [0-23]。
     * 使用24小时制。
     */
    uint8_t tm_hour;

    /**
     * \~english
     * @brief Day of month [1-31].
     * \~chinese
     * @brief 月中的日期 [1-31]。
     */
    uint8_t tm_mday;

    /**
     * \~english
     * @brief Day of week [1-7].
     * Sunday=1, Monday=2, ..., Saturday=7.
     * \~chinese
     * @brief 星期几 [1-7]。
     * 星期日=1，星期一=2，...，星期六=7。
     */
    uint8_t tm_wday;

    /**
     * \~english
     * @brief Month [1-12].
     * January=1, February=2, ..., December=12.
     * \~chinese
     * @brief 月份 [1-12]。
     * 一月=1，二月=2，...，十二月=12。
     */
    uint8_t tm_mon;

    /**
     * \~english
     * @brief Year value [0-65535].
     * \~chinese
     * @brief 年份值 [0-65535]。
     */
    uint16_t tm_year;

    /**
     * \~english
     * @brief Milliseconds component of time [0-999].
     * Provides sub-second precision.
     * \~chinese
     * @brief 时间的毫秒分量 [0-999]。
     * 提供亚秒级精度。
     */
    uint16_t tm_ms;
} vsf_rtc_tm_t;


#if VSF_RTC_CFG_REIMPLEMENT_TYPE_CFG == DISABLED
typedef struct vsf_rtc_t vsf_rtc_t;

/**
 * \~english
 * @brief RTC interrupt handler function type
 * @param[in,out] target_ptr User defined target pointer for context data
 * @param[in] rtc_ptr RTC instance pointer for accessing RTC data
 * @param[in] irq_mask Interrupt mask indicating which interrupt(s) occurred
 * \~chinese
 * @brief RTC 中断处理函数类型
 * @param[in,out] target_ptr 用户定义的目标指针，用于上下文数据
 * @param[in] rtc_ptr RTC 实例指针，用于访问 RTC 数据
 * @param[in] irq_mask 指示发生哪些中断的中断掩码
 */
typedef void vsf_rtc_isr_handler_t(void *target_ptr, vsf_rtc_t *rtc_ptr, vsf_rtc_irq_mask_t irq_mask);

/**
 * \~english
 * @brief RTC interrupt service routine configuration structure
 * \~chinese
 * @brief RTC 中断服务程序配置结构体
 */
typedef struct vsf_rtc_isr_t {
    vsf_rtc_isr_handler_t *handler_fn;    //!< \~english Interrupt handler function to process RTC interrupts \~chinese 处理 RTC 中断的中断处理函数
    void *target_ptr;                     //!< \~english User context pointer passed to the handler \~chinese 传递给处理函数的用户上下文指针
    vsf_arch_prio_t prio;                 //!< \~english Priority level for RTC interrupts \~chinese RTC 中断的优先级
} vsf_rtc_isr_t;

/**
 * \~english
 * @brief RTC configuration structure
 * \~chinese
 * @brief RTC 配置结构体
 */
typedef struct vsf_rtc_cfg_t {
    vsf_rtc_isr_t isr;                    //!< \~english Interrupt service routine configuration \~chinese 中断服务程序配置
} vsf_rtc_cfg_t;
#endif

/**
 * \~english
 * @brief RTC capability structure.
 * Describes the features and capabilities supported by the RTC hardware.
 * \~chinese
 * @brief RTC 能力结构体。
 * 描述 RTC 硬件支持的特性和能力。
 */
#if VSF_RTC_CFG_REIMPLEMENT_TYPE_CAPABILITY == DISABLED
typedef struct vsf_rtc_capability_t {
#if VSF_RTC_CFG_INHERIT_HAL_CAPABILITY == ENABLED
    /**
     * \~english
     * @brief Inherit common peripheral capabilities.
     * Must be the first member of the structure.
     * \~chinese
     * @brief 继承通用外设能力。
     * 必须是结构体的第一个成员。
     */
    inherit(vsf_peripheral_capability_t)
#endif  /* VSF_RTC_CFG_INHERIT_HAL_CAPABILITY */

    /**
     * \~english
     * @brief Bitmask of supported RTC interrupts.
     * Indicates which interrupt types are available in this RTC implementation.
     * \~chinese
     * @brief RTC 支持的中断位掩码。
     * 指示此 RTC 实现中可用的中断类型。
     */
    vsf_rtc_irq_mask_t irq_mask;
} vsf_rtc_capability_t;
#endif  /* VSF_RTC_CFG_REIMPLEMENT_TYPE_CAPABILITY */

/**
 * \~english @brief RTC operation function pointer type, used for RTC Multi Class support
 * \~chinese @brief RTC 操作函数指针类型，用于 RTC Multi Class 支持
 */
typedef struct vsf_rtc_op_t {
/// @cond
#undef __VSF_HAL_TEMPLATE_API
#define __VSF_HAL_TEMPLATE_API VSF_HAL_TEMPLATE_API_FP
/// @endcond

    VSF_RTC_APIS(vsf)
} vsf_rtc_op_t;

#if VSF_RTC_CFG_MULTI_CLASS == ENABLED
/**
 * \~english @brief RTC instance structure, used for RTC Multi Class support, not needed in non Multi Class mode
 * \~chinese @brief RTC 实例结构体，用于 RTC Multi Class 支持，在非 Multi Class 模式下不需要
 */
struct vsf_rtc_t  {
    //! \~english Operation interface pointer for RTC functions
    //! \~chinese RTC 函数的操作接口指针
    const vsf_rtc_op_t * op;
};
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

/**
 * \~english
 * @brief Initialize a RTC instance
 * @param[in] rtc_ptr Pointer to structure @ref vsf_rtc_t
 * @param[in] cfg_ptr Pointer to configuration structure @ref vsf_rtc_cfg_t
 * @return vsf_err_t VSF_ERR_NONE if initialization successful, otherwise returns error code
 *
 * @note It is not necessary to call vsf_rtc_fini() to deinitialize.
 *       vsf_rtc_init() should be called before any other RTC API except vsf_rtc_capability().
 * \~chinese
 * @brief 初始化 RTC 实例
 * @param[in] rtc_ptr 结构体 @ref vsf_rtc_t 的指针
 * @param[in] cfg_ptr 配置结构体 @ref vsf_rtc_cfg_t 的指针
 * @return vsf_err_t 如果初始化成功返回 VSF_ERR_NONE，否则返回错误码
 *
 * @note 失败后不需要调用 vsf_rtc_fini() 反初始化。
 *       vsf_rtc_init() 应该在除 vsf_rtc_capability() 之外的其他 RTC API 之前调用。
 */
extern vsf_err_t vsf_rtc_init(vsf_rtc_t *rtc_ptr, vsf_rtc_cfg_t *cfg_ptr);

/**
 * \~english
 * @brief Finalize a RTC instance
 * @param[in] rtc_ptr Pointer to structure @ref vsf_rtc_t
 * @return None
 *
 * \~chinese
 * @brief 终止 RTC 实例
 * @param[in] rtc_ptr 结构体 @ref vsf_rtc_t 的指针
 * @return 无
 */
extern void vsf_rtc_fini(vsf_rtc_t *rtc_ptr);

/**
 * \~english
 * @brief Enable a RTC instance
 * @param[in] rtc_ptr Pointer to structure @ref vsf_rtc_t
 * @return fsm_rt_t FSM_RT_CPL if RTC was enabled, fsm_rt_on_going if RTC is still enabling
 *
 * \~chinese
 * @brief 启用 RTC 实例
 * @param[in] rtc_ptr 指向结构体 @ref vsf_rtc_t 的指针
 * @return fsm_rt_t 如果 RTC 实例已启用则返回 FSM_RT_CPL，如果 RTC 实例正在启用过程中则返回 fsm_rt_on_going
 */
extern fsm_rt_t vsf_rtc_enable(vsf_rtc_t *rtc_ptr);

/**
 * \~english
 * @brief Disable a RTC instance
 * @param[in] rtc_ptr Pointer to structure @ref vsf_rtc_t
 * @return fsm_rt_t FSM_RT_CPL if RTC was disabled, fsm_rt_on_going if RTC is still disabling
 *
 * \~chinese
 * @brief 禁用 RTC 实例
 * @param[in] rtc_ptr 指向结构体 @ref vsf_rtc_t 的指针
 * @return fsm_rt_t 如果 RTC 实例已禁用则返回 FSM_RT_CPL，如果 RTC 实例正在禁用过程中则返回 fsm_rt_on_going
 */
extern fsm_rt_t vsf_rtc_disable(vsf_rtc_t *rtc_ptr);

/**
 * \~english
 * @brief Get the capability of RTC instance
 * @param[in] rtc_ptr Pointer to structure @ref vsf_rtc_t
 * @return vsf_rtc_capability_t All capabilities of current RTC, reference @ref vsf_rtc_capability_t
 *
 * \~chinese
 * @brief 获取 RTC 实例的能力
 * @param[in] rtc_ptr 指向结构体 @ref vsf_rtc_t 的指针
 * @return vsf_rtc_capability_t 返回当前 RTC 的所有能力，参考 @ref vsf_rtc_capability_t
 */
extern vsf_rtc_capability_t vsf_rtc_capability(vsf_rtc_t *rtc_ptr);

/**
 * \~english
 * @brief Get RTC date and time
 * @param[in] rtc_ptr Pointer to structure @ref vsf_rtc_t
 * @param[out] rtc_tm Date and time (year, month, day, hour, minute, second, etc.)
 * @return vsf_err_t VSF_ERR_NONE if operation successful, otherwise returns error code
 *
 * \~chinese
 * @brief 获取 RTC 日期和时间
 * @param[in] rtc_ptr 指向结构体 @ref vsf_rtc_t 的指针
 * @param[out] rtc_tm 日期和时间（年、月、日、时、分、秒等）
 * @return vsf_err_t 如果操作成功返回 VSF_ERR_NONE，否则返回错误码
 */
extern vsf_err_t vsf_rtc_get(vsf_rtc_t *rtc_ptr, vsf_rtc_tm_t *rtc_tm);

/**
 * \~english
 * @brief Set RTC date and time
 * @param[in] rtc_ptr Pointer to structure @ref vsf_rtc_t
 * @param[in] rtc_tm Date and time (year, month, day, hour, minute, second, etc.)
 * @return vsf_err_t VSF_ERR_NONE if operation successful, otherwise returns error code
 *
 * \~chinese
 * @brief 设置 RTC 日期和时间
 * @param[in] rtc_ptr 指向结构体 @ref vsf_rtc_t 的指针
 * @param[in] rtc_tm 日期和时间（年、月、日、时、分、秒等）
 * @return vsf_err_t 如果操作成功返回 VSF_ERR_NONE，否则返回错误码
 */
extern vsf_err_t vsf_rtc_set(vsf_rtc_t *rtc_ptr, const vsf_rtc_tm_t *rtc_tm);

/**
 * \~english
 * @brief Get RTC time in seconds and milliseconds
 * @param[in] rtc_ptr Pointer to structure @ref vsf_rtc_t
 * @param[out] second_ptr Seconds
 * @param[out] millisecond_ptr Milliseconds
 * @return vsf_err_t VSF_ERR_NONE if operation successful, otherwise returns error code
 *
 * \~chinese
 * @brief 获取 RTC 的秒和毫秒时间
 * @param[in] rtc_ptr 指向结构体 @ref vsf_rtc_t 的指针
 * @param[out] second_ptr 秒
 * @param[out] millisecond_ptr 毫秒
 * @return vsf_err_t 如果操作成功返回 VSF_ERR_NONE，否则返回错误码
 */
extern vsf_err_t vsf_rtc_get_time(vsf_rtc_t *rtc_ptr, vsf_rtc_time_t *second_ptr, vsf_rtc_time_t *millisecond_ptr);

/**
 * \~english
 * @brief Set RTC time in seconds and milliseconds
 * @param[in] rtc_ptr Pointer to structure @ref vsf_rtc_t
 * @param[in] seconds Seconds
 * @param[in] milliseconds Milliseconds
 * @return vsf_err_t VSF_ERR_NONE if operation successful, otherwise returns error code
 *
 * \~chinese
 * @brief 设置 RTC 的秒和毫秒时间
 * @param[in] rtc_ptr 指向结构体 @ref vsf_rtc_t 的指针
 * @param[in] seconds 秒
 * @param[in] milliseconds 毫秒
 * @return vsf_err_t 如果操作成功返回 VSF_ERR_NONE，否则返回错误码
 */
extern vsf_err_t vsf_rtc_set_time(vsf_rtc_t *rtc_ptr, vsf_rtc_time_t seconds, vsf_rtc_time_t milliseconds);

/*============================ MACROFIED FUNCTIONS ===========================*/

/// @cond
#if VSF_RTC_CFG_FUNCTION_RENAME == ENABLED
#   define __vsf_rtc_t                  VSF_MCONNECT(VSF_RTC_CFG_PREFIX, _rtc_t)
#   define vsf_rtc_init(__RTC, ...)     VSF_MCONNECT(VSF_RTC_CFG_PREFIX, _rtc_init)         ((__vsf_rtc_t *)(__RTC), ##__VA_ARGS__)
#   define vsf_rtc_enable(__RTC)        VSF_MCONNECT(VSF_RTC_CFG_PREFIX, _rtc_enable)       ((__vsf_rtc_t *)(__RTC))
#   define vsf_rtc_disable(__RTC)       VSF_MCONNECT(VSF_RTC_CFG_PREFIX, _rtc_disable)      ((__vsf_rtc_t *)(__RTC))
#   define vsf_rtc_capability(__RTC)    VSF_MCONNECT(VSF_RTC_CFG_PREFIX, _rtc_capability)   ((__vsf_rtc_t *)(__RTC))
#   define vsf_rtc_get(__RTC, ...)      VSF_MCONNECT(VSF_RTC_CFG_PREFIX, _rtc_get)          ((__vsf_rtc_t *)(__RTC), ##__VA_ARGS__)
#   define vsf_rtc_set(__RTC, ...)      VSF_MCONNECT(VSF_RTC_CFG_PREFIX, _rtc_set)          ((__vsf_rtc_t *)(__RTC), ##__VA_ARGS__)
#   define vsf_rtc_get_time(__RTC, ...) VSF_MCONNECT(VSF_RTC_CFG_PREFIX, _rtc_get_time)     ((__vsf_rtc_t *)(__RTC), ##__VA_ARGS__)
#   define vsf_rtc_set_time(__RTC, ...) VSF_MCONNECT(VSF_RTC_CFG_PREFIX, _rtc_set_time)     ((__vsf_rtc_t *)(__RTC), ##__VA_ARGS__)
#endif
/// @endcond

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* __VSF_TEMPLATE_RTC_H__ */
