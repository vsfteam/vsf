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

#ifndef __VSF_TEMPLATE_DAC_H__
#define __VSF_TEMPLATE_DAC_H__

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
#ifndef VSF_DAC_CFG_MULTI_CLASS
#   define VSF_DAC_CFG_MULTI_CLASS                      ENABLED
#endif

/**
 * \~english
 * @brief Define DAC hardware mask if count is defined.
 *
 * \~chinese
 * @brief 如果定义了 DAC 硬件数量，则定义对应的掩码。
 */
#if defined(VSF_HW_DAC_COUNT) && !defined(VSF_HW_DAC_MASK)
#   define VSF_HW_DAC_MASK                              VSF_HAL_COUNT_TO_MASK(VSF_HW_DAC_COUNT)
#endif

/**
 * \~english
 * @brief Define DAC hardware count if mask is defined.
 *
 * \~chinese
 * @brief 如果定义了 DAC 硬件掩码，则定义对应的数量。
 */
#if defined(VSF_HW_DAC_MASK) && !defined(VSF_HW_DAC_COUNT)
#   define VSF_HW_DAC_COUNT                             VSF_HAL_MASK_TO_COUNT(VSF_HW_DAC_MASK)
#endif

/**
 * \~english
 * @brief We can redefine macro VSF_DAC_CFG_PREFIX to specify a prefix to call a
 *        specific driver directly in the application code.
 *
 * \~chinese
 * @brief 可以重定义宏 VSF_DAC_CFG_PREFIX，以在应用代码中直接调用特定的驱动程序。
 */
#ifndef VSF_DAC_CFG_PREFIX
#   if VSF_DAC_CFG_MULTI_CLASS == ENABLED
#       define VSF_DAC_CFG_PREFIX                       vsf
#   elif defined(VSF_HW_DAC_COUNT) && (VSF_HW_DAC_COUNT != 0)
#       define VSF_DAC_CFG_PREFIX                       vsf_hw
#   else
#       define VSF_DAC_CFG_PREFIX                       vsf
#   endif
#endif

/**
 * \~english
 * @brief Disable VSF_DAC_CFG_FUNCTION_RENAME to use the original function
 * names (e.g., vsf_dac_init()).
 *
 * \~chinese
 * @brief 禁用 VSF_DAC_CFG_FUNCTION_RENAME 以使用原始函数名
 * (例如 vsf_dac_init())。
 */
#ifndef VSF_DAC_CFG_FUNCTION_RENAME
#   define VSF_DAC_CFG_FUNCTION_RENAME                  ENABLED
#endif

/**
 * \~english
 * @brief Enable VSF_DAC_CFG_REIMPLEMENT_TYPE_IRQ_MASK to reimplement interrupt mask type @ref vsf_dac_irq_mask_t in specific hardware drivers
 *
 * \~chinese
 * @brief 在特定硬件驱动中，可启用宏 VSF_DAC_CFG_REIMPLEMENT_TYPE_IRQ_MASK 来重新定义中断掩码类型 @ref vsf_dac_irq_mask_t。
 */
#ifndef VSF_DAC_CFG_REIMPLEMENT_TYPE_IRQ_MASK
#   define VSF_DAC_CFG_REIMPLEMENT_TYPE_IRQ_MASK        DISABLED
#endif

/**
 * \~english
 * @brief Enable VSF_DAC_CFG_REIMPLEMENT_TYPE_CHANNEL_CFG to reimplement channel configuration type @ref vsf_dac_channel_cfg_t in specific hardware drivers.
 * For compatibility, do not delete members when redefining vsf_dac_channel_cfg_t
 *
 * \~chinese
 * @brief 在特定硬件驱动中，可启用宏 VSF_DAC_CFG_REIMPLEMENT_TYPE_CHANNEL_CFG 来重新定义通道配置类型 @ref vsf_dac_channel_cfg_t。
 * 为保证兼容性，重新定义vsf_dac_channel_cfg_t时不要删除成员
 */
#if VSF_DAC_CFG_REIMPLEMENT_TYPE_CHANNEL_CFG == DISABLED
#   define VSF_DAC_CFG_REIMPLEMENT_TYPE_CHANNEL_CFG     DISABLED
#endif

/**
 * \~english
 * @brief Enable VSF_DAC_CFG_REIMPLEMENT_TYPE_CFG to reimplement DAC configuration type @ref vsf_dac_cfg_t in specific hardware drivers.
 * For compatibility, do not delete members when redefining vsf_dac_cfg_t.
 * The vsf_dac_isr_handler_t type also needs to be redefined.
 *
 * \~chinese
 * @brief 在特定硬件驱动中，可启用宏 VSF_DAC_CFG_REIMPLEMENT_TYPE_CFG 来重新定义 DAC 配置类型 @ref vsf_dac_cfg_t。
 * 为保证兼容性，重新定义vsf_dac_cfg_t时不要删除成员。
 * 同时也需要重新定义vsf_dac_isr_handler_t类型。
 */
#if VSF_DAC_CFG_REIMPLEMENT_TYPE_CFG == DISABLED
#   define VSF_DAC_CFG_REIMPLEMENT_TYPE_CFG             DISABLED
#endif

/**
 * \~english
 * @brief Enable VSF_DAC_CFG_REIMPLEMENT_TYPE_CAPABILITY to reimplement capability type @ref vsf_dac_capability_t in specific hardware drivers.
 * For compatibility, do not delete members when redefining vsf_dac_capability_t.
 *
 * \~chinese
 * @brief 在特定硬件驱动中，可启用宏 VSF_DAC_CFG_REIMPLEMENT_TYPE_CAPABILITY 来重新定义能力类型。
 * 为保证兼容性，重新定义vsf_dac_capability_t时不要删除成员
 */
#if VSF_DAC_CFG_REIMPLEMENT_TYPE_CAPABILITY == DISABLED
#   define VSF_DAC_CFG_REIMPLEMENT_TYPE_CAPABILITY     DISABLED
#endif

/**
 * \~english
 * @brief In specific hardware driver, we can enable macro VSF_DAC_CFG_INHERIT_HAL_CAPABILITY to inherit structure @ref vsf_peripheral_capability_t.
 * \~chinese
 * @brief 在特定硬件驱动中，可启用宏 VSF_DAC_CFG_INHERIT_HAL_CAPABILITY 来继承结构体 @ref vsf_peripheral_capability_t。
 */
#ifndef VSF_DAC_CFG_INHERIT_HAL_CAPABILITY
#   define VSF_DAC_CFG_INHERIT_HAL_CAPABILITY            ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

/**
 * \~english
 * @brief DAC API template, used to generate DAC type, specific prefix function declarations, etc.
 * @param[in] __prefix_name The prefix used for generating DAC functions.
 *
 * \~chinese
 * @brief DAC API 模板，用于生成 DAC 类型、特定前缀的函数声明等。
 * @param[in] __prefix_name 用于生成 DAC 函数的前缀。
 */
#define VSF_DAC_APIS(__prefix_name) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,            dac, init,                 VSF_MCONNECT(__prefix_name, _dac_t) *dac_ptr, vsf_dac_cfg_t *cfg_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                 dac, fini,                 VSF_MCONNECT(__prefix_name, _dac_t) *dac_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,             dac, enable,               VSF_MCONNECT(__prefix_name, _dac_t) *dac_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,             dac, disable,              VSF_MCONNECT(__prefix_name, _dac_t) *dac_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_dac_status_t,     dac, status,               VSF_MCONNECT(__prefix_name, _dac_t) *dac_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_dac_capability_t, dac, capability,           VSF_MCONNECT(__prefix_name, _dac_t) *dac_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                 dac, irq_enable,           VSF_MCONNECT(__prefix_name, _dac_t) *dac_ptr, vsf_dac_irq_mask_t irq_mask) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                 dac, irq_disable,          VSF_MCONNECT(__prefix_name, _dac_t) *dac_ptr, vsf_dac_irq_mask_t irq_mask) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,            dac, channel_request_once, VSF_MCONNECT(__prefix_name, _dac_t) *dac_ptr, vsf_dac_channel_cfg_t *cfg, uint_fast16_t value) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,            dac, channel_config,       VSF_MCONNECT(__prefix_name, _dac_t) *dac_ptr, vsf_dac_channel_cfg_t *cfgs_ptr, uint_fast8_t cnt) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,            dac, channel_request,      VSF_MCONNECT(__prefix_name, _dac_t) *dac_ptr, void *values_ptr, uint_fast32_t cnt)

/*============================ TYPES =========================================*/

#if VSF_DAC_CFG_REIMPLEMENT_TYPE_IRQ_MASK == DISABLED
/**
 * \~english
 * @brief Predefined VSF DAC interrupts that can be reimplemented in specific HAL drivers.
 * Even if the hardware doesn't support these features, these interrupts must be kept:
 * - VSF_DAC_IRQ_MASK_IDLE
 * - VSF_DAC_IRQ_MASK_CPL
 *
 * \~chinese
 * @brief 预定义的 VSF DAC 中断，可以在特定的 HAL 驱动中重新实现。
 * 即使硬件不支持这些中断，但是这些中断是必须实现的：
 * - VSF_DAC_IRQ_MASK_IDLE
 * - VSF_DAC_IRQ_MASK_CPL
 */
typedef enum vsf_dac_irq_mask_t {
    VSF_DAC_IRQ_MASK_IDLE = (0x01ul << 0),    //!< \~english DAC is idle \~chinese DAC 处于空闲状态
    VSF_DAC_IRQ_MASK_CPL  = (0x1ul << 0),     //!< \~english DAC transfer complete \~chinese DAC 传输完成
} vsf_dac_irq_mask_t;
#endif

enum {
    VSF_DAC_IRQ_COUNT               = 2,
    VSF_DAC_IRQ_ALL_BITS_MASK       = VSF_DAC_IRQ_MASK_IDLE | VSF_DAC_IRQ_MASK_CPL,
};

//! dac channel configuration
#if VSF_ADC_CFG_REIMPLEMENT_TYPE_CHANNEL_CFG == DISABLED
/**
 * \~english
 * @brief Configuration structure for DAC channel
 *
 * \~chinese
 * @brief DAC 通道的配置结构体
 */
typedef struct vsf_dac_channel_cfg_t {
    uint8_t channel;      //!< \~english Channel index \~chinese 通道索引
    uint8_t resolution;   //!< \~english Channel resolution \~chinese 通道分辨率
} vsf_dac_channel_cfg_t;
#endif

#if VSF_DAC_CFG_REIMPLEMENT_TYPE_CFG == DISABLED
typedef struct vsf_dac_t vsf_dac_t;

/**
 * \~english
 * @brief DAC interrupt handler function type.
 * @param[in] target_ptr User defined target pointer
 * @param[in] dac_ptr DAC instance pointer
 * @param[in] irq_mask Interrupt mask
 *
 * \~chinese
 * @brief DAC 中断处理函数类型
 * @param[in] target_ptr 用户定义的目标指针
 * @param[in] dac_ptr DAC 实例指针
 * @param[in] irq_mask 中断掩码
 */
typedef void vsf_dac_isr_handler_t(void *target_ptr, vsf_dac_t *dac_ptr, vsf_dac_irq_mask_t irq_mask);

/**
 * \~english
 * @brief DAC interrupt configuration structure
 *
 * \~chinese
 * @brief DAC 中断配置结构体
 */
typedef struct vsf_dac_isr_t {
    vsf_dac_isr_handler_t  *handler_fn;    //!< \~english Interrupt handler \~chinese 中断处理函数
    void                   *target_ptr;    //!< \~english User target pointer \~chinese 用户目标指针
    vsf_arch_prio_t         prio;          //!< \~english Interrupt priority \~chinese 中断优先级
} vsf_dac_isr_t;

/**
 * \~english
 * @brief DAC configuration structure
 *
 * \~chinese
 * @brief DAC 配置结构体
 */
typedef struct vsf_dac_cfg_t {
    vsf_dac_isr_t   isr;    //!< \~english Interrupt configuration \~chinese 中断配置
} vsf_dac_cfg_t;
#endif

#if VSF_DAC_CFG_REIMPLEMENT_TYPE_STATUS == DISABLED
/**
 * \~english
 * @brief DAC status structure that can be reimplemented in specific HAL drivers
 *
 * \~chinese
 * @brief 可在具体 HAL 驱动中重新实现的 DAC 状态结构体
 */
typedef struct vsf_dac_status_t {
    union {
        inherit(vsf_peripheral_status_t)
        struct {
            uint32_t is_busy : 1;    //!< \~english DAC is busy \~chinese DAC 处于忙状态
        };
    };
} vsf_dac_status_t;
#endif

#if VSF_DAC_CFG_REIMPLEMENT_TYPE_CAPABILITY == DISABLED
/**
 * \~english
 * @brief DAC capability structure that can be reimplemented in specific HAL drivers
 *
 * \~chinese
 * @brief 可在具体 HAL 驱动中重新实现的 DAC 能力结构体
 */
typedef struct vsf_dac_capability_t {
#if VSF_DAC_CFG_INHERIT_HAL_CAPABILITY == ENABLED
    inherit(vsf_peripheral_capability_t)
#endif
    vsf_dac_irq_mask_t irq_mask;           //!< \~english Supported interrupts \~chinese 支持的中断
    uint8_t max_resolution_bits;           //!< \~english Maximum resolution bits \~chinese 最大分辨率位数
    uint8_t min_resolution_bits;           //!< \~english Minimum resolution bits \~chinese 最小分辨率位数
    uint8_t channel_count;                 //!< \~english Number of channels \~chinese 通道数量
} vsf_dac_capability_t;
#endif

typedef struct vsf_dac_op_t {
/// @cond
#undef  __VSF_HAL_TEMPLATE_API
#define __VSF_HAL_TEMPLATE_API VSF_HAL_TEMPLATE_API_FP
/// @endcond

    VSF_DAC_APIS(vsf)
} vsf_dac_op_t;

#if VSF_DAC_CFG_MULTI_CLASS == ENABLED
struct vsf_dac_t  {
    const vsf_dac_op_t * op;
};
#endif

/*============================ PROTOTYPES ====================================*/

/**
 \~english
 @brief Initialize a DAC instance
 @param[in] dac_ptr: a pointer to structure @ref vsf_dac_t
 @param[in] cfg_ptr: a pointer to structure @ref vsf_dac_cfg_t
 @return vsf_err_t: VSF_ERR_NONE if the initialization was successful, otherwise returns error code

 @note It is not necessary to call vsf_dac_fini() to deinitialize.
      vsf_dac_init() should be called before any other DAC API except vsf_dac_capability().

 \~chinese
 @brief 初始化一个 DAC 实例
 @param[in] dac_ptr: 指向结构体 @ref vsf_dac_t 的指针
 @param[in] cfg_ptr: 指向结构体 @ref vsf_dac_cfg_t 的指针
 @return vsf_err_t: 如果初始化成功返回 VSF_ERR_NONE，否则返回错误码

 @note 初始化失败后不需要调用 vsf_dac_fini() 进行反初始化。
       vsf_dac_init() 应该在除 vsf_dac_capability() 之外的其他 DAC API 之前调用。
 */
extern vsf_err_t vsf_dac_init(vsf_dac_t *dac_ptr, vsf_dac_cfg_t *cfg_ptr);

/**
 \~english
 @brief Finalize a DAC instance
 @param[in] dac_ptr: a pointer to structure @ref vsf_dac_t
 @return none

 \~chinese
 @brief 终止一个 DAC 实例
 @param[in] dac_ptr: 指向结构体 @ref vsf_dac_t 的指针
 @return 无
 */
extern void vsf_dac_fini(vsf_dac_t *dac_ptr);

/**
 \~english
 @brief Enable a DAC instance
 @param[in] dac_ptr: a pointer to structure @ref vsf_dac_t
 @return fsm_rt_t: fsm_rt_cpl if DAC was enabled, fsm_rt_on_going if DAC is still enabling

 \~chinese
 @brief 启用一个 DAC 实例
 @param[in] dac_ptr: 指向结构体 @ref vsf_dac_t 的指针
 @return fsm_rt_t: 如果 DAC 实例已启用返回 fsm_rt_cpl，如果 DAC 实例正在启用过程中返回 fsm_rt_on_going
 */
extern fsm_rt_t vsf_dac_enable(vsf_dac_t *dac_ptr);

/**
 \~english
 @brief Disable a DAC instance
 @param[in] dac_ptr: a pointer to structure @ref vsf_dac_t
 @return fsm_rt_t: fsm_rt_cpl if DAC was disabled, fsm_rt_on_going if DAC is still disabling

 \~chinese
 @brief 禁用一个 DAC 实例
 @param[in] dac_ptr: 指向结构体 @ref vsf_dac_t 的指针
 @return fsm_rt_t: 如果 DAC 实例已禁用返回 fsm_rt_cpl，如果 DAC 实例正在禁用过程中返回 fsm_rt_on_going
 */
extern fsm_rt_t vsf_dac_disable(vsf_dac_t *dac_ptr);

/**
 \~english
 @brief Enable interrupt masks of DAC instance
 @param[in] dac_ptr: a pointer to structure @ref vsf_dac_t
 @param[in] irq_mask: one or more value of enum vsf_dac_irq_mask_t, @ref vsf_dac_irq_mask_t
 @return none
 @note All pending interrupts should be cleared before interrupts are enabled

 \~chinese
 @brief 启用 DAC 实例的中断
 @param[in] dac_ptr: 指向结构体 @ref vsf_dac_t 的指针
 @param[in] irq_mask: 一个或者多个枚举 vsf_dac_irq_mask_t 的值的按位或，@ref vsf_dac_irq_mask_t
 @return 无
 @note 在中断启用之前，应该清除所有挂起的中断
 */
extern void vsf_dac_irq_enable(vsf_dac_t *dac_ptr, vsf_dac_irq_mask_t irq_mask);

/**
 \~english
 @brief Disable interrupt masks of DAC instance
 @param[in] dac_ptr: a pointer to structure @ref vsf_dac_t
 @param[in] irq_mask: one or more value of enum vsf_dac_irq_mask_t, @ref vsf_dac_irq_mask_t
 @return none

 \~chinese
 @brief 禁用 DAC 实例的中断
 @param[in] dac_ptr: 指向结构体 @ref vsf_dac_t 的指针
 @param[in] irq_mask: 一个或者多个枚举 vsf_dac_irq_mask_t 的值的按位或，@ref vsf_dac_irq_mask_t
 @return 无
 */
extern void vsf_dac_irq_disable(vsf_dac_t *dac_ptr, vsf_dac_irq_mask_t irq_mask);

/**
 \~english
 @brief Get the status of DAC instance
 @param[in] dac_ptr: a pointer to structure @ref vsf_dac_t
 @return vsf_dac_status_t: All status of current DAC

 \~chinese
 @brief 获取 DAC 实例的状态
 @param[in] dac_ptr: 指向结构体 @ref vsf_dac_t 的指针
 @return vsf_dac_status_t: 返回当前 DAC 的所有状态
 */
extern vsf_dac_status_t vsf_dac_status(vsf_dac_t *dac_ptr);

/**
 \~english
 @brief Get the capability of DAC instance
 @param[in] dac_ptr: a pointer to structure @ref vsf_dac_t
 @return vsf_dac_capability_t: All capability of current DAC @ref vsf_dac_capability_t

 \~chinese
 @brief 获取 DAC 实例的能力
 @param[in] dac_ptr: 指向结构体 @ref vsf_dac_t 的指针
 @return vsf_dac_capability_t: 返回当前 DAC 的所有能力 @ref vsf_dac_capability_t
 */
extern vsf_dac_capability_t vsf_dac_capability(vsf_dac_t *dac_ptr);


/**
 \~english
 @brief DAC request convert once
 @param[in] dac_ptr: a pointer to structure @ref vsf_dac_t
 @param[in] channel_cfg: a pointer to convert channel configuration
 @param[in] value: value to be converted
 @return vsf_err_t: VSF_ERR_NONE if the request was successful, otherwise returns error code

 \~chinese
 @brief DAC 请求转换一次
 @param[in] dac_ptr: 指向结构体 @ref vsf_dac_t 的指针
 @param[in] channel_cfg: 转换通道配置的指针
 @param[in] value: 待转换的值
 @return vsf_err_t: 如果请求成功返回 VSF_ERR_NONE，否则返回错误码
 */
extern vsf_err_t vsf_dac_channel_request_once(vsf_dac_t *dac_ptr,
                                              vsf_dac_channel_cfg_t *channel_cfg,
                                              uint_fast16_t value);

/**
 \~english
 @brief DAC channel configuration
 @param[in] dac_ptr: a pointer to structure @ref vsf_dac_t
 @param[in] cfgs_ptr: convert channel configuration array
 @param[in] cnt: the length of convert channel configuration array
 @return vsf_err_t: VSF_ERR_NONE if the configuration was successful, otherwise returns error code

 \~chinese
 @brief DAC 通道配置
 @param[in] dac_ptr: 指向结构体 @ref vsf_dac_t 的指针
 @param[in] cfgs_ptr: 转换通道配置数组
 @param[in] cnt: 转换通道配置数组的长度
 @return vsf_err_t: 如果配置成功返回 VSF_ERR_NONE，否则返回错误码
 */
extern vsf_err_t vsf_dac_channel_config(vsf_dac_t *dac_ptr,
                                        vsf_dac_channel_cfg_t *cfgs_ptr,
                                        uint_fast8_t cnt);

/**
 \~english
 @brief DAC channel request
 @param[in] dac_ptr: a pointer to structure @ref vsf_dac_t
 @param[out] buffer_ptr: convert channel data array
 @param[in] count: the length of convert channel configuration data array
 @return vsf_err_t: VSF_ERR_NONE if the request was successful, otherwise returns error code

 \~chinese
 @brief DAC 通道请求
 @param[in] dac_ptr: 指向结构体 @ref vsf_dac_t 的指针
 @param[out] buffer_ptr: 转换通道数据数组
 @param[in] count: 转换通道配置数据数组的长度
 @return vsf_err_t: 如果请求成功返回 VSF_ERR_NONE，否则返回错误码
 */
extern vsf_err_t vsf_dac_channel_request(vsf_dac_t *dac_ptr,
                                         void *buffer_ptr,
                                         uint_fast32_t count);

/*============================ MACROFIED FUNCTIONS ===========================*/

/// @cond
#if VSF_DAC_CFG_FUNCTION_RENAME == ENABLED
#   define __vsf_dac_t                              VSF_MCONNECT(VSF_DAC_CFG_PREFIX, _dac_t)
#   define vsf_dac_init(__DAC, ...)                 VSF_MCONNECT(VSF_DAC_CFG_PREFIX, _dac_init)                 ((__vsf_dac_t *)(__DAC), ##__VA_ARGS__)
#   define vsf_dac_fini(__DAC)                      VSF_MCONNECT(VSF_DAC_CFG_PREFIX, _dac_fini)                 ((__vsf_dac_t *)(__DAC))
#   define vsf_dac_enable(__DAC)                    VSF_MCONNECT(VSF_DAC_CFG_PREFIX, _dac_enable)               ((__vsf_dac_t *)(__DAC))
#   define vsf_dac_disable(__DAC)                   VSF_MCONNECT(VSF_DAC_CFG_PREFIX, _dac_disable)              ((__vsf_dac_t *)(__DAC))
#   define vsf_dac_status(__DAC)                    VSF_MCONNECT(VSF_DAC_CFG_PREFIX, _dac_status)               ((__vsf_dac_t *)(__DAC))
#   define vsf_dac_capability(__DAC)                VSF_MCONNECT(VSF_DAC_CFG_PREFIX, _dac_capability)           ((__vsf_dac_t *)(__DAC))
#   define vsf_dac_irq_enable(__DAC, ...)           VSF_MCONNECT(VSF_DAC_CFG_PREFIX, _dac_irq_enable)           ((__vsf_dac_t *)(__DAC), ##__VA_ARGS__)
#   define vsf_dac_irq_disable(__DAC, ...)          VSF_MCONNECT(VSF_DAC_CFG_PREFIX, _dac_irq_disable)          ((__vsf_dac_t *)(__DAC), ##__VA_ARGS__)
#   define vsf_dac_channel_request_once(__DAC, ...) VSF_MCONNECT(VSF_DAC_CFG_PREFIX, _dac_channel_request_once) ((__vsf_dac_t *)(__DAC), ##__VA_ARGS__)
#   define vsf_dac_channel_config(__DAC, ...)       VSF_MCONNECT(VSF_DAC_CFG_PREFIX, _dac_channel_config)       ((__vsf_dac_t *)(__DAC), ##__VA_ARGS__)
#   define vsf_dac_channel_request(__DAC, ...)      VSF_MCONNECT(VSF_DAC_CFG_PREFIX, _dac_channel_request)      ((__vsf_dac_t *)(__DAC), ##__VA_ARGS__)
#endif
/// @endcond

#ifdef __cplusplus
}
#endif

#endif  /*__VSF_TEMPLATE_DAC_H__*/
