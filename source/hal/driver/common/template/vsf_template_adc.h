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

#ifndef __VSF_TEMPLATE_ADC_H__
#define __VSF_TEMPLATE_ADC_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_template_hal_driver.h"
#include "hal/arch/vsf_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

/**
 * \~english
 * @brief Enable multi-class support by default for maximum availability
 *
 * \~chinese
 * @brief 默认启用多类支持，以获得最大可用性
 */
#ifndef VSF_ADC_CFG_MULTI_CLASS
#   define VSF_ADC_CFG_MULTI_CLASS                      ENABLED
#endif

/**
 * \~english
 * @brief Define ADC hardware mask if count is defined
 *
 * \~chinese
 * @brief 如果定义了 ADC 硬件数量，则定义对应的掩码
 */
#if defined(VSF_HW_ADC_COUNT) && !defined(VSF_HW_ADC_MASK)
#   define VSF_HW_ADC_MASK                              VSF_HAL_COUNT_TO_MASK(VSF_HW_ADC_COUNT)
#endif

/**
 * \~english
 * @brief Define ADC hardware count if mask is defined
 *
 * \~chinese
 * @brief 如果定义了 ADC 硬件掩码，则定义对应的数量
 */
#if defined(VSF_HW_ADC_MASK) && !defined(VSF_HW_ADC_COUNT)
#   define VSF_HW_ADC_COUNT                             VSF_HAL_MASK_TO_COUNT(VSF_HW_ADC_MASK)
#endif

/**
 * \~english
 * @brief We can redefine macro VSF_ADC_CFG_PREFIX to specify a prefix
 * to call a specific driver directly in the application code.
 *
 * \~chinese
 * @brief 可重新定义宏 VSF_ADC_CFG_PREFIX，以在应用代码中直接调用
 * 特定驱动函数。
 */
#ifndef VSF_ADC_CFG_PREFIX
#   if VSF_ADC_CFG_MULTI_CLASS == ENABLED
#       define VSF_ADC_CFG_PREFIX                       vsf
#   elif defined(VSF_HW_ADC_COUNT) && (VSF_HW_ADC_COUNT != 0)
#       define VSF_ADC_CFG_PREFIX                       vsf_hw
#   else
#       define VSF_ADC_CFG_PREFIX                       vsf
#   endif
#endif

/**
 * \~english
 * @brief Disable VSF_ADC_CFG_FUNCTION_RENAME to use the original function
 * names (e.g., vsf_adc_init()).
 *
 * \~chinese
 * @brief 禁用 VSF_ADC_CFG_FUNCTION_RENAME 以使用原始函数名
 * (例如 vsf_adc_init())。
 */
#ifndef VSF_ADC_CFG_FUNCTION_RENAME
#   define VSF_ADC_CFG_FUNCTION_RENAME                  ENABLED
#endif

/**
 * \~english
 * @brief Enable VSF_ADC_CFG_REIMPLEMENT_TYPE_MODE to reimplement mode type @ref
 * vsf_adc_mode_t in specific hardware drivers
 * \~chinese
 * @brief 在特定硬件驱动中，可启用宏 VSF_ADC_CFG_REIMPLEMENT_TYPE_MODE
 * 来重新定义枚举 @ref vsf_adc_mode_t。
 */
#ifndef VSF_ADC_CFG_REIMPLEMENT_TYPE_MODE
#   define VSF_ADC_CFG_REIMPLEMENT_TYPE_MODE            DISABLED
#endif

/**
 * \~english
 * @brief Enable VSF_ADC_CFG_REIMPLEMENT_TYPE_CHANNEL_MODE to reimplement
 * channel mode type @ref vsf_adc_channel_mode_t in specific hardware drivers
 * \~chinese
 * @brief 在特定硬件驱动中，可启用宏 VSF_ADC_CFG_REIMPLEMENT_TYPE_CHANNEL_MODE
 * 来重新定义枚举 @ref vsf_adc_channel_mode_t。
 */
#ifndef VSF_ADC_CFG_REIMPLEMENT_TYPE_CHANNEL_MODE
#   define VSF_ADC_CFG_REIMPLEMENT_TYPE_CHANNEL_MODE    DISABLED
#endif

/**
 * \~english
 * @brief Enable VSF_ADC_CFG_REIMPLEMENT_TYPE_IRQ_MASK to reimplement interrupt
 * mask type @ref vsf_adc_irq_mask_t in specific hardware drivers
 * \~chinese
 * @brief 在特定硬件驱动中，可启用宏 VSF_ADC_CFG_REIMPLEMENT_TYPE_IRQ_MASK
 * 来重新定义枚举 @ref vsf_adc_irq_mask_t。
 */
#ifndef VSF_ADC_CFG_REIMPLEMENT_TYPE_IRQ_MASK
#   define VSF_ADC_CFG_REIMPLEMENT_TYPE_IRQ_MASK        DISABLED
#endif

/**
 * \~english
 * @brief Enable VSF_ADC_CFG_REIMPLEMENT_TYPE_CHANNEL_CFG to reimplement channel
 * configuration type @ref vsf_adc_channel_cfg_t in specific hardware drivers
 * For compatibility, do not delete members when redefining
 * vsf_adc_channel_cfg_t
 * \~chinese
 * @brief 在特定硬件驱动中，可启用宏 VSF_ADC_CFG_REIMPLEMENT_TYPE_CHANNEL_CFG
 * 来重新定义通道配置类型 @ref vsf_adc_channel_cfg_t。为保证兼容性，重新定义
 * vsf_adc_channel_cfg_t 时不要删除成员
 */
#if VSF_ADC_CFG_REIMPLEMENT_TYPE_CHANNEL_CFG == DISABLED
#    define VSF_ADC_CFG_REIMPLEMENT_TYPE_CHANNEL_CFG DISABLED
#endif

/**
 * \~english
 * @brief Enable VSF_ADC_CFG_REIMPLEMENT_TYPE_CFG to reimplement ADC
 * configuration type @ref vsf_adc_cfg_t in specific hardware drivers For
 * compatibility, do not delete members when redefining vsf_adc_cfg_t The
 * vsf_adc_isr_handler_t type also needs to be redefined
 * \~chinese
 * @brief 在特定的硬件驱动中，可启用宏 VSF_ADC_CFG_REIMPLEMENT_TYPE_CFG
 * 来重新定义 ADC 配置类型 @ref vsf_adc_cfg_t。
 * 为保证兼容性，重新定义 vsf_adc_cfg_t 时不要删除成员。
 * 同时也需要重新定义 vsf_adc_isr_handler_t 类型
 */
#if VSF_ADC_CFG_REIMPLEMENT_TYPE_CFG == DISABLED
#    define VSF_ADC_CFG_REIMPLEMENT_TYPE_CFG DISABLED
#endif

/**
 * \~english
 * @brief Enable the option to reimplement capability type.
 * For compatibility, do not delete members when redefining vsf_adc_capability_t
 * \~chinese
 * @brief 在特定的硬件驱动中，可启用宏 VSF_ADC_CFG_REIMPLEMENT_TYPE_CAPABILITY
 * 来重新定义能力类型 @ref vsf_adc_capability_t。
 * 为保证兼容性，重新定义 vsf_adc_capability_t 时不要删除成员
 */
#if VSF_ADC_CFG_REIMPLEMENT_TYPE_CAPABILITY == DISABLED
#    define VSF_ADC_CFG_REIMPLEMENT_TYPE_CAPABILITY DISABLED
#endif

/**
 * \~english
 * @brief In specific hardware driver, we can enable macro
 * VSF_ADC_CFG_INHERIT_HAL_CAPABILITY to inherit structure @ref
 * vsf_peripheral_capability_t.
 * \~chinese
 * @brief 在特定的硬件驱动中，可启用宏 VSF_ADC_CFG_INHERIT_HAL_CAPABILITY
 * 来继承结构体 @ref vsf_peripheral_capability_t。
 */
#ifndef VSF_ADC_CFG_INHERIT_HAL_CAPABILITY
#   define VSF_ADC_CFG_INHERIT_HAL_CAPABILITY            ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

/**
 * \~english
 * @brief ADC API template, used to generate ADC type, specific prefix function declarations, etc.
 * @param[in] __PREFIX_NAME The prefix used for generating ADC functions.
 *
 * \~chinese
 * @brief ADC API 模板，用于生成 ADC 类型、特定前缀的函数声明等。
 * @param[in] __PREFIX_NAME 用于生成 ADC 函数的前缀。
 */
#define VSF_ADC_APIS(__PREFIX_NAME) \
    __VSF_HAL_TEMPLATE_API(__PREFIX_NAME, vsf_err_t,            adc, init,                 VSF_MCONNECT(__PREFIX_NAME, _adc_t) *adc_ptr, vsf_adc_cfg_t *cfg_ptr) \
    __VSF_HAL_TEMPLATE_API(__PREFIX_NAME, void,                 adc, fini,                 VSF_MCONNECT(__PREFIX_NAME, _adc_t) *adc_ptr) \
    __VSF_HAL_TEMPLATE_API(__PREFIX_NAME, fsm_rt_t,             adc, enable,               VSF_MCONNECT(__PREFIX_NAME, _adc_t) *adc_ptr) \
    __VSF_HAL_TEMPLATE_API(__PREFIX_NAME, fsm_rt_t,             adc, disable,              VSF_MCONNECT(__PREFIX_NAME, _adc_t) *adc_ptr) \
    __VSF_HAL_TEMPLATE_API(__PREFIX_NAME, vsf_adc_status_t,     adc, status,               VSF_MCONNECT(__PREFIX_NAME, _adc_t) *adc_ptr) \
    __VSF_HAL_TEMPLATE_API(__PREFIX_NAME, vsf_adc_capability_t, adc, capability,           VSF_MCONNECT(__PREFIX_NAME, _adc_t) *adc_ptr) \
    __VSF_HAL_TEMPLATE_API(__PREFIX_NAME, void,                 adc, irq_enable,           VSF_MCONNECT(__PREFIX_NAME, _adc_t) *adc_ptr, vsf_adc_irq_mask_t irq_mask) \
    __VSF_HAL_TEMPLATE_API(__PREFIX_NAME, void,                 adc, irq_disable,          VSF_MCONNECT(__PREFIX_NAME, _adc_t) *adc_ptr, vsf_adc_irq_mask_t irq_mask) \
    __VSF_HAL_TEMPLATE_API(__PREFIX_NAME, vsf_err_t,            adc, channel_request_once, VSF_MCONNECT(__PREFIX_NAME, _adc_t) *adc_ptr, vsf_adc_channel_cfg_t *channel_cfg, void *buffer_ptr) \
    __VSF_HAL_TEMPLATE_API(__PREFIX_NAME, vsf_err_t,            adc, channel_config,       VSF_MCONNECT(__PREFIX_NAME, _adc_t) *adc_ptr, vsf_adc_channel_cfg_t *channel_cfgs_ptr, uint32_t channel_cfgs_cnt) \
    __VSF_HAL_TEMPLATE_API(__PREFIX_NAME, vsf_err_t,            adc, channel_request,      VSF_MCONNECT(__PREFIX_NAME, _adc_t) *adc_ptr, void *buffer_ptr, uint_fast32_t count)

/*============================ TYPES =========================================*/

#if VSF_ADC_CFG_REIMPLEMENT_TYPE_MODE == DISABLED
/**
 * \~english
 * @brief Predefined VSF ADC modes that can be reimplemented in specific HAL drivers.
 *
 * \~chinese
 * @brief 预定义的 VSF ADC 模式，可以在特定的 HAL 驱动中重新实现。
 *
 * \~english
 * Even if the hardware doesn't support these modes, the following modes must be kept，
 * If the hardware supports more modes, e.g. more reference voltage, more data alignment,
 * we can implement it in the hardware driver:
 * \~chinese
 * 即使硬件不支持这些功能，但以下的模式必须保留，如果硬件支持更多模式，例如更多的参考电压，
 * 更多的数据对齐方式，可以在硬件驱动中实现它：
 *
 * - VSF_ADC_REF_VDD_1
 * - VSF_ADC_REF_VDD_1_2
 * - VSF_ADC_REF_VDD_1_3
 * - VSF_ADC_REF_VDD_1_4
 * - VSF_ADC_DATA_ALIGN_RIGHT
 * - VSF_ADC_DATA_ALIGN_LEFT
 * - VSF_ADC_SCAN_CONV_SINGLE_MODE
 * - VSF_ADC_SCAN_CONV_SEQUENCE_MODE
 * - VSF_ADC_EXTERN_TRIGGER_0
 * - VSF_ADC_EXTERN_TRIGGER_1
 * - VSF_ADC_EXTERN_TRIGGER_2
 *
 * \~english
 * If more new modes are added to the driver, then the corresponding MASK macros need to
 * be defined to include the values of the new modes. For example, Adding the new reference voltage option
 * requires that the macro VSF_ADC_REF_VDD_MASK be defined.
 * \~chinese
 * 如果在驱动中添加更多的新模式，则需要定义对应的 MASK 宏来包含新模式的值。例如添加新的
 * 参考电压选项，就需要定义宏 VSF_ADC_REF_VDD_MASK。
 *
 * \~english
 *  Optional mode require one or more enumeration options and a macro with the same
 *  name to determine if they are supported at runtime. If the feature supports more than
 *  one option, it is recommended to provide the corresponding MASK option, so that the
 *  user can switch to different modes at compile-time.
 * \~chinese
 * 可选模式需要提供一个或多个枚举选项，还需要提供同名的宏，方便用户在编译时判断是否支持。
 * 如果该特性支持多个选项，建议提供对应的 MASK 选项，方便用户在运行时切换到不同的模式。
 */
typedef enum vsf_adc_mode_t {
    /**
     * \~english
     * @brief Reference voltage set to VDD.
     * Uses the full supply voltage as reference for ADC conversion.
     * \~chinese
     * @brief 参考电压设置为 VDD。
     * 使用全部供电电压作为 ADC 转换的参考。
     */
    VSF_ADC_REF_VDD_1               = (0 << 0),

    /**
     * \~english
     * @brief Reference voltage set to VDD/2.
     * Uses half of the supply voltage as reference for ADC conversion.
     * \~chinese
     * @brief 参考电压设置为 VDD/2。
     * 使用供电电压的一半作为 ADC 转换的参考。
     */
    VSF_ADC_REF_VDD_1_2             = (1 << 0),

    /**
     * \~english
     * @brief Reference voltage set to VDD/3.
     * Uses one-third of the supply voltage as reference for ADC conversion.
     * \~chinese
     * @brief 参考电压设置为 VDD/3。
     * 使用供电电压的三分之一作为 ADC 转换的参考。
     */
    VSF_ADC_REF_VDD_1_3             = (2 << 0),

    /**
     * \~english
     * @brief Reference voltage set to VDD/4.
     * Uses one-fourth of the supply voltage as reference for ADC conversion.
     * \~chinese
     * @brief 参考电压设置为 VDD/4。
     * 使用供电电压的四分之一作为 ADC 转换的参考。
     */
    VSF_ADC_REF_VDD_1_4             = (3 << 0),

    /**
     * \~english
     * @brief Data alignment set to right.
     * The converted data is right-aligned in the data register.
     * \~chinese
     * @brief 数据对齐方式设置为右对齐。
     * 转换后的数据在数据寄存器中右对齐。
     */
    VSF_ADC_DATA_ALIGN_RIGHT        = (0 << 2),

    /**
     * \~english
     * @brief Data alignment set to left.
     * The converted data is left-aligned in the data register.
     * \~chinese
     * @brief 数据对齐方式设置为左对齐。
     * 转换后的数据在数据寄存器中左对齐。
     */
    VSF_ADC_DATA_ALIGN_LEFT         = (1 << 2),

    /**
     * \~english
     * @brief Single conversion mode.
     * Conversion is performed once for a single channel.
     * \~chinese
     * @brief 单次转换模式。
     * 对单个通道进行一次转换。
     */
    VSF_ADC_SCAN_CONV_SINGLE_MODE   = (0 << 3),

    /**
     * \~english
     * @brief Sequence conversion mode.
     * Conversions are performed in sequence for multiple channels.
     * \~chinese
     * @brief 序列转换模式。
     * 对多个通道按序列进行转换。
     */
    VSF_ADC_SCAN_CONV_SEQUENCE_MODE = (1 << 3),

    /**
     * \~english
     * @brief External trigger source 0.
     * Selects the first external event source to trigger conversion.
     * \~chinese
     * @brief 外部触发源 0。
     * 选择第一个外部事件源触发转换。
     */
    VSF_ADC_EXTERN_TRIGGER_0        = (0 << 4),

    /**
     * \~english
     * @brief External trigger source 1.
     * Selects the second external event source to trigger conversion.
     * \~chinese
     * @brief 外部触发源 1。
     * 选择第二个外部事件源触发转换。
     */
    VSF_ADC_EXTERN_TRIGGER_1        = (1 << 4),

    /**
     * \~english
     * @brief External trigger source 2.
     * Selects the third external event source to trigger conversion.
     * \~chinese
     * @brief 外部触发源 2。
     * 选择第三个外部事件源触发转换。
     */
    VSF_ADC_EXTERN_TRIGGER_2        = (2 << 4),
} vsf_adc_mode_t;
#endif

enum {
    VSF_ADC_REF_VDD_COUNT           = 4,
    VSF_ADC_REF_VDD_MASK            = VSF_ADC_REF_VDD_1 |
                                      VSF_ADC_REF_VDD_1_2 |
                                      VSF_ADC_REF_VDD_1_3 |
                                      VSF_ADC_REF_VDD_1_4,

    VSF_ADC_DATA_ALIGN_COUNT        = 2,
    VSF_ADC_DATA_ALIGN_MASK         = VSF_ADC_DATA_ALIGN_RIGHT |
                                      VSF_ADC_DATA_ALIGN_LEFT,

    VSF_ADC_SCAN_CONV_COUNT         = 2,
    VSF_ADC_SCAN_CONV_MASK          = VSF_ADC_SCAN_CONV_SINGLE_MODE |
                                      VSF_ADC_SCAN_CONV_SEQUENCE_MODE,

    VSF_ADC_EXTERN_TRIGGER_COUNT    = 3,
    VSF_ADC_EXTERN_TRIGGER_MASK     = VSF_ADC_EXTERN_TRIGGER_0 |
                                      VSF_ADC_EXTERN_TRIGGER_1 |
                                      VSF_ADC_EXTERN_TRIGGER_2,

    VSF_ADC_MODE_MASK_COUNT         = 4,
    VSF_ADC_MODE_ALL_BITS_MASK      = VSF_ADC_REF_VDD_MASK |
                                      VSF_ADC_DATA_ALIGN_MASK |
                                      VSF_ADC_SCAN_CONV_MASK |
                                      VSF_ADC_EXTERN_TRIGGER_MASK,
};

#if VSF_ADC_CFG_REIMPLEMENT_TYPE_CHANNEL_MODE == DISABLED
/**
 * \~english
 * @brief Predefined VSF ADC channel modes that can be reimplemented in specific HAL drivers.
 *
 * \~chinese
 * @brief 预定义的 VSF ADC 通道模式，可以在特定的 HAL 驱动中重新实现。
 *
 * \~english
 * Even if the hardware doesn't support these modes, the following modes must be kept，
 * If the hardware supports more modes, e.g. more gain, more reference voltage,
 * we can implement it in the hardware driver:
 * \~chinese
 * 即使硬件不支持这些功能，但以下的模式必须保留，如果硬件支持更多模式，例如更多的增益，
 * 更多的参考电压，可以在硬件驱动中实现它：
 *
 * - VSF_ADC_CHANNEL_GAIN_1_6
 * - VSF_ADC_CHANNEL_GAIN_1_5
 * - VSF_ADC_CHANNEL_GAIN_1_4
 * - VSF_ADC_CHANNEL_GAIN_1_3
 * - VSF_ADC_CHANNEL_GAIN_1_2
 * - VSF_ADC_CHANNEL_GAIN_1
 * - VSF_ADC_CHANNEL_REF_VDD_1
 * - VSF_ADC_CHANNEL_REF_VDD_1_2
 * - VSF_ADC_CHANNEL_REF_VDD_1_3
 * - VSF_ADC_CHANNEL_REF_VDD_1_4
 *
 * \~english
 * If more new modes are added to the driver, then the corresponding MASK macros need to
 * be defined to include the values of the new modes. For example, Adding the new gain option
 * requires that the macro VSF_ADC_CHANNEL_GAIN_MASK be defined.
 * \~chinese
 * 如果在驱动中添加更多的新模式，则需要定义对应的 MASK 宏来包含新模式的值。例如添加新的
 * 增益选项，就需要定义宏 VSF_ADC_CHANNEL_GAIN_MASK。
 *
 * \~english
 *  Optional mode require one or more enumeration options and a macro with the same
 *  name to determine if they are supported at runtime. If the feature supports more than
 *  one option, it is recommended to provide the corresponding MASK option, so that the
 *  user can switch to different modes at compile-time.
 * \~chinese
 * 可选模式需要提供一个或多个枚举选项，还需要提供同名的宏，方便用户在编译时判断是否支持。
 * 如果该特性支持多个选项，建议提供对应的 MASK 选项，方便用户在运行时切换到不同的模式。
 */
typedef enum vsf_adc_channel_mode_t {
    /**
     * \~english
     * @brief Channel gain set to 1/6.
     * Input signal is attenuated to 1/6 of its original value.
     * \~chinese
     * @brief 通道增益设置为 1/6。
     * 输入信号衰减为原始值的 1/6。
     */
    VSF_ADC_CHANNEL_GAIN_1_6        = (0 << 0),

    /**
     * \~english
     * @brief Channel gain set to 1/5.
     * Input signal is attenuated to 1/5 of its original value.
     * \~chinese
     * @brief 通道增益设置为 1/5。
     * 输入信号衰减为原始值的 1/5。
     */
    VSF_ADC_CHANNEL_GAIN_1_5        = (1 << 0),

    /**
     * \~english
     * @brief Channel gain set to 1/4.
     * Input signal is attenuated to 1/4 of its original value.
     * \~chinese
     * @brief 通道增益设置为 1/4。
     * 输入信号衰减为原始值的 1/4。
     */
    VSF_ADC_CHANNEL_GAIN_1_4        = (2 << 0),

    /**
     * \~english
     * @brief Channel gain set to 1/3.
     * Input signal is attenuated to 1/3 of its original value.
     * \~chinese
     * @brief 通道增益设置为 1/3。
     * 输入信号衰减为原始值的 1/3。
     */
    VSF_ADC_CHANNEL_GAIN_1_3        = (3 << 0),

    /**
     * \~english
     * @brief Channel gain set to 1/2.
     * Input signal is attenuated to 1/2 of its original value.
     * \~chinese
     * @brief 通道增益设置为 1/2。
     * 输入信号衰减为原始值的 1/2。
     */
    VSF_ADC_CHANNEL_GAIN_1_2        = (4 << 0),

    /**
     * \~english
     * @brief Channel gain set to 1.
     * Input signal remains at its original value (no amplification or attenuation).
     * \~chinese
     * @brief 通道增益设置为 1。
     * 输入信号保持原始值（无放大或衰减）。
     */
    VSF_ADC_CHANNEL_GAIN_1          = (5 << 0),

    /**
     * \~english
     * @brief Channel reference voltage set to VDD/1.
     * Uses the full supply voltage as reference for this channel.
     * \~chinese
     * @brief 通道参考电压设置为 VDD/1。
     * 使用全部供电电压作为此通道的参考。
     */
    VSF_ADC_CHANNEL_REF_VDD_1       = (0 << 4),

    /**
     * \~english
     * @brief Channel reference voltage set to VDD/1.2.
     * Uses VDD/1.2 as reference voltage for this channel.
     * \~chinese
     * @brief 通道参考电压设置为 VDD/1.2。
     * 使用 VDD/1.2 作为此通道的参考电压。
     */
    VSF_ADC_CHANNEL_REF_VDD_1_2     = (1 << 4),

    /**
     * \~english
     * @brief Channel reference voltage set to VDD/1.3.
     * Uses VDD/1.3 as reference voltage for this channel.
     * \~chinese
     * @brief 通道参考电压设置为 VDD/1.3。
     * 使用 VDD/1.3 作为此通道的参考电压。
     */
    VSF_ADC_CHANNEL_REF_VDD_1_3     = (2 << 4),

    /**
     * \~english
     * @brief Channel reference voltage set to VDD/1.4.
     * Uses VDD/1.4 as reference voltage for this channel.
     * \~chinese
     * @brief 通道参考电压设置为 VDD/1.4。
     * 使用 VDD/1.4 作为此通道的参考电压。
     */
    VSF_ADC_CHANNEL_REF_VDD_1_4     = (3 << 4),
} vsf_adc_channel_mode_t;
#endif

enum {
    VSF_ADC_CHANNEL_GAIN_COUNT      = 6,
    VSF_ADC_CHANNEL_GAIN_MASK       = VSF_ADC_CHANNEL_GAIN_1_6 |
                                      VSF_ADC_CHANNEL_GAIN_1_5 |
                                      VSF_ADC_CHANNEL_GAIN_1_4 |
                                      VSF_ADC_CHANNEL_GAIN_1_3 |
                                      VSF_ADC_CHANNEL_GAIN_1_2 |
                                      VSF_ADC_CHANNEL_GAIN_1,

    VSF_ADC_CHANNEL_REF_VDD_COUNT   = 4,
    VSF_ADC_CHANNEL_REF_VDD_MASK    = VSF_ADC_CHANNEL_REF_VDD_1 |
                                      VSF_ADC_CHANNEL_REF_VDD_1_2 |
                                      VSF_ADC_CHANNEL_REF_VDD_1_3 |
                                      VSF_ADC_CHANNEL_REF_VDD_1_4,

    VSF_ADC_CHANNEL_MODE_MASK_COUNT = 2,
    VSF_ADC_CHANNEL_MODE_ALL_BITS_MASK
                                    = VSF_ADC_CHANNEL_GAIN_MASK |
                                      VSF_ADC_CHANNEL_REF_VDD_MASK,
};

#if VSF_ADC_CFG_REIMPLEMENT_TYPE_IRQ_MASK == DISABLED
/**
 * \~english
 * @brief Predefined VSF ADC interrupt that can be reimplemented in specific HAL
 * drivers.
 *
 * \~chinese
 * @brief 预定义的 VSF ADC 中断，可以在特定的 HAL 驱动中重新实现。
 *
 * \~english
 * Even if the hardware doesn't support these features, these interrupt must be
 * kept.
 * \~chinese
 * 即使硬件不支持这些中断，但是这些中断是必须实现的：
 *
 * - VSF_ADC_IRQ_MASK_CPL
 *
 * \~english
 * Optional interrupt require one or more enumeration options and a macro with
 * the same, so that the user can check if the interrupt is supported at
 * compile-time.
 * \~chinese
 * 可选中断需要提供一个或多个枚举选项，还需要提供同名的宏，方便用户在编译时判断是否支持。
 */
typedef enum vsf_adc_irq_mask_t {
    VSF_ADC_IRQ_MASK_CPL = (0x1ul << 0),  //!< Conversion complete interrupt
} vsf_adc_irq_mask_t;
#endif

enum {
    VSF_ADC_IRQ_COUNT               = 1,
    VSF_ADC_IRQ_ALL_BITS_MASK       = VSF_ADC_IRQ_MASK_CPL,
};

//! adc channel configuration
#if VSF_ADC_CFG_REIMPLEMENT_TYPE_CHANNEL_CFG == DISABLED
/**
 * \~english
 * @brief Configuration structure for ADC channel.
 *
 * \~chinese
 * @brief ADC 通道的配置结构体。
 */
typedef struct vsf_adc_channel_cfg_t {
    vsf_adc_channel_mode_t mode;    //!< Channel mode
    uint16_t sample_cycles;         //!< Sample cycles
    uint8_t  channel;               //!< Channel Index
} vsf_adc_channel_cfg_t;
#endif

#if VSF_ADC_CFG_REIMPLEMENT_TYPE_CFG == DISABLED
typedef struct vsf_adc_t vsf_adc_t;
typedef void vsf_adc_isr_handler_t(void *target_ptr,
                                   vsf_adc_t *adc_ptr,
                                   vsf_adc_irq_mask_t irq_mask);
//! adc isr for api
typedef struct vsf_adc_isr_t {
    vsf_adc_isr_handler_t  *handler_fn;
    void                   *target_ptr;
    vsf_arch_prio_t         prio;
} vsf_adc_isr_t;

/**
 * \~english
 * @brief Configuration structure for ADC.
 *
 * \~chinese
 * @brief ADC 的配置结构体。
 */
typedef struct vsf_adc_cfg_t {
    vsf_adc_mode_t  mode;           //!< ADC working mode.
    vsf_adc_isr_t   isr;            //!< Interrupt service routine configuration.
    uint32_t        clock_hz;       //!< Clock frequency in Hertz.
} vsf_adc_cfg_t;
#endif

#if VSF_ADC_CFG_REIMPLEMENT_TYPE_STATUS == DISABLED
/**
 * \~english
 * @brief Predefined VSF ADC status that can be reimplemented in specific HAL drivers.
 *
 * \~chinese
 * @brief 预定义的 VSF ADC 状态，可以在特定的 HAL 驱动中重新实现。
 *
 * \~english
 * Even if the hardware doesn't support these features, these status must be kept.
 * \~chinese
 * 即使硬件不支持这些状态，但是这些状态是必须实现的：
 *
 * - is_busy: indicates if the ADC is currently busy
 */
typedef struct vsf_adc_status_t {
    union {
        inherit(vsf_peripheral_status_t)
        struct {
            uint32_t is_busy : 1;
        };
        uint32_t value;
    };
} vsf_adc_status_t;
#endif

#if VSF_ADC_CFG_REIMPLEMENT_TYPE_CAPABILITY == DISABLED
/**
 * \~english
 * @brief Predefined VSF ADC capability that can be reimplemented in specific HAL drivers.
 *
 * \~chinese
 * @brief 预定义的 VSF ADC 能力，可以在特定的 HAL 驱动中重新实现。
 *
 * \~english
 * Even if the hardware doesn't support these features, these capabilities must be kept.
 * \~chinese
 * 即使硬件不支持这些能力，但是这些能力是必须保留的：
 *
 * - irq_mask: indicates the supported interrupt masks
 * - max_data_bits: the maximum data bits supported
 * - channel_count: the number of channels supported
 */
typedef struct vsf_adc_capability_t {
#if VSF_ADC_CFG_INHERIT_HAL_CAPABILITY == ENABLED
    inherit(vsf_peripheral_capability_t)
#endif
    vsf_adc_irq_mask_t irq_mask;

    uint8_t max_data_bits;
    uint8_t channel_count;
} vsf_adc_capability_t;
#endif

typedef struct vsf_adc_op_t {
/// @cond
#undef  __VSF_HAL_TEMPLATE_API
#define __VSF_HAL_TEMPLATE_API VSF_HAL_TEMPLATE_API_FP
/// @endcond

    VSF_ADC_APIS(vsf)
} vsf_adc_op_t;

#if VSF_ADC_CFG_MULTI_CLASS == ENABLED
struct vsf_adc_t  {
    const vsf_adc_op_t * op;
};
#endif

/*============================ PROTOTYPES ====================================*/

/**
 \~english
 @brief Initialize an ADC instance
 @param[in] adc_ptr: a pointer to structure @ref vsf_adc_t
 @param[in] cfg_ptr: a pointer to structure @ref vsf_adc_cfg_t
 @return vsf_err_t: VSF_ERR_NONE if initialization successful, otherwise returns error code

 @note It is not necessary to call vsf_adc_fini() to deinitialize.
 @note vsf_adc_init() should be called before any other ADC API except vsf_adc_capability().

 \~chinese
 @brief 初始化一个 ADC 实例
 @param[in] adc_ptr: 指向结构体 @ref vsf_adc_t 的指针
 @param[in] cfg_ptr: 指向结构体 @ref vsf_adc_cfg_t 的指针
 @return vsf_err_t: 如果初始化成功返回 VSF_ERR_NONE，否则返回错误码

 @note 失败后不需要调用 vsf_adc_fini() 进行反初始化。
 @note vsf_adc_init() 应该在除 vsf_adc_capability() 之外的其他 ADC API 之前调用。
 */
extern vsf_err_t vsf_adc_init(vsf_adc_t *adc_ptr, vsf_adc_cfg_t *cfg_ptr);

/**
 \~english
 @brief Finalize an ADC instance
 @param[in] adc_ptr: a pointer to structure @ref vsf_adc_t
 @return none

 \~chinese
 @brief 终止一个 ADC 实例
 @param[in] adc_ptr: 指向结构体 @ref vsf_adc_t 的指针
 @return 无
 */
extern void vsf_adc_fini(vsf_adc_t *adc_ptr);

/**
 \~english
 @brief Enable an ADC instance
 @param[in] adc_ptr: a pointer to structure @ref vsf_adc_t
 @return fsm_rt_t: FSM_RT_CPL if ADC was enabled, FSM_RT_ON_GOING if ADC is still enabling

 \~chinese
 @brief 启用 ADC 实例
 @param[in] adc_ptr: 指向结构体 @ref vsf_adc_t 的指针
 @return fsm_rt_t: 如果 ADC 实例已启用返回 FSM_RT_CPL，如果 ADC 实例正在启用过程中返回 FSM_RT_ON_GOING
 */
extern fsm_rt_t vsf_adc_enable(vsf_adc_t *adc_ptr);

/**
 \~english
 @brief Disable an ADC instance
 @param[in] adc_ptr: a pointer to structure @ref vsf_adc_t
 @return fsm_rt_t: FSM_RT_CPL if ADC was disabled, FSM_RT_ON_GOING if ADC is still disabling

 \~chinese
 @brief 禁用 ADC 实例
 @param[in] adc_ptr: 指向结构体 @ref vsf_adc_t 的指针
 @return fsm_rt_t: 如果 ADC 实例已禁用返回 FSM_RT_CPL，如果 ADC 实例正在禁用过程中返回 FSM_RT_ON_GOING
 */
extern fsm_rt_t vsf_adc_disable(vsf_adc_t *adc_ptr);

/**
 \~english
 @brief Enable interrupt masks of ADC instance
 @param[in] adc_ptr: a pointer to structure @ref vsf_adc_t
 @param[in] irq_mask: one or more values of enum @ref vsf_adc_irq_mask_t
 @return none
 @note All pending interrupts should be cleared before interrupts are enabled.

 \~chinese
 @brief 启用 ADC 实例的中断
 @param[in] adc_ptr: 指向结构体 @ref vsf_adc_t 的指针
 @param[in] irq_mask: 一个或多个枚举 vsf_adc_irq_mask_t 值的按位或，参考 @ref vsf_adc_irq_mask_t
 @return 无
 @note 在启用中断之前，应该清除所有挂起的中断。
 */
extern void vsf_adc_irq_enable(vsf_adc_t *adc_ptr, vsf_adc_irq_mask_t irq_mask);

/**
 \~english
 @brief Disable interrupt masks of ADC instance
 @param[in] adc_ptr: a pointer to structure @ref vsf_adc_t
 @param[in] irq_mask: one or more values of enum @ref vsf_adc_irq_mask_t
 @return none

 \~chinese
 @brief 禁用 ADC 实例的中断
 @param[in] adc_ptr: 指向结构体 @ref vsf_adc_t 的指针
 @param[in] irq_mask: 一个或多个枚举 vsf_adc_irq_mask_t 值的按位或，参考 @ref vsf_adc_irq_mask_t
 @return 无
 */
extern void vsf_adc_irq_disable(vsf_adc_t *adc_ptr, vsf_adc_irq_mask_t irq_mask);

/**
 \~english
 @brief Get the status of ADC instance
 @param[in] adc_ptr: a pointer to structure @ref vsf_adc_t
 @return vsf_adc_status_t: All status of current ADC

 \~chinese
 @brief 获取 ADC 实例的状态
 @param[in] adc_ptr: 指向结构体 @ref vsf_adc_t 的指针
 @return vsf_adc_status_t: 返回当前 ADC 的所有状态
 */
extern vsf_adc_status_t vsf_adc_status(vsf_adc_t *adc_ptr);

/**
 \~english
 @brief Get the capability of ADC instance
 @param[in] adc_ptr: a pointer to structure @ref vsf_adc_t
 @return vsf_adc_capability_t: All capabilities of current ADC @ref vsf_adc_capability_t

 \~chinese
 @brief 获取 ADC 实例的能力
 @param[in] adc_ptr: 指向结构体 @ref vsf_adc_t 的指针
 @return vsf_adc_capability_t: 返回当前 ADC 的所有能力 @ref vsf_adc_capability_t
 */
extern vsf_adc_capability_t vsf_adc_capability(vsf_adc_t *adc_ptr);

/**
 \~english
 @brief ADC request sampling once
 @param[in] adc_ptr: a pointer to structure @ref vsf_adc_t
 @param[in] channel_cfg: a pointer to convert channel configuration
 @param[in] buffer_ptr: value to be converted
 @return vsf_err_t: VSF_ERR_NONE if the request was successful, otherwise returns error code

 \~chinese
 @brief ADC 请求采样一次
 @param[in] adc_ptr: 指向结构体 @ref vsf_adc_t 的指针
 @param[in] channel_cfg: 转换通道配置的指针
 @param[in] buffer_ptr: 待转换的值的缓冲区
 @return vsf_err_t: 如果请求成功返回 VSF_ERR_NONE，否则返回错误码
 */
extern vsf_err_t vsf_adc_channel_request_once(vsf_adc_t *adc_ptr,
                                              vsf_adc_channel_cfg_t *channel_cfg,
                                              void *buffer_ptr);

/**
 \~english
 @brief ADC channel configuration
 @param[in] adc_ptr: a pointer to structure @ref vsf_adc_t
 @param[in] cfgs_ptr: convert channel configuration array
 @param[in] cnt: the length of convert channel configuration array
 @return vsf_err_t: VSF_ERR_NONE if the configuration was successful, otherwise returns error code
 *
 * \~chinese
 * @brief ADC 通道配置
 * @param[in] adc_ptr: 指向结构体 @ref vsf_adc_t 的指针
 * @param[in] cfgs_ptr: 转换通道配置数组
 * @param[in] cnt: 转换通道配置数组的长度
 * @return vsf_err_t: 如果配置成功返回 VSF_ERR_NONE，否则返回错误码
 */
extern vsf_err_t vsf_adc_channel_config(vsf_adc_t *adc_ptr,
                                        vsf_adc_channel_cfg_t *cfgs_ptr,
                                        uint32_t cnt);

/**
 \~english
 @brief ADC channel request
 @param[in] adc_ptr: a pointer to structure @ref vsf_adc_t
 @param[out] buffer_ptr: convert channel data array
 @param[in] count: the length of convert channel configuration data array
 @return vsf_err_t: VSF_ERR_NONE if the request was successful, otherwise returns error code

 \~chinese
 @brief ADC 通道请求
 @param[in] adc_ptr: 指向结构体 @ref vsf_adc_t 的指针
 @param[out] buffer_ptr: 转换通道数据数组
 @param[in] count: 转换通道配置数据数组的长度
 @return vsf_err_t: 如果请求成功返回 VSF_ERR_NONE，否则返回错误码
 */
extern vsf_err_t vsf_adc_channel_request(vsf_adc_t *adc_ptr,
                                         void *buffer_ptr,
                                         uint_fast32_t count);

/*============================ MACROFIED FUNCTIONS ===========================*/

/// @cond
#if VSF_ADC_CFG_FUNCTION_RENAME == ENABLED
#   define __vsf_adc_t                              VSF_MCONNECT(VSF_ADC_CFG_PREFIX, _adc_t)
#   define vsf_adc_init(__ADC, ...)                 VSF_MCONNECT(VSF_ADC_CFG_PREFIX, _adc_init)                 ((__vsf_adc_t *)(__ADC), ##__VA_ARGS__)
#   define vsf_adc_fini(__ADC)                      VSF_MCONNECT(VSF_ADC_CFG_PREFIX, _adc_fini)                 ((__vsf_adc_t *)(__ADC))
#   define vsf_adc_enable(__ADC)                    VSF_MCONNECT(VSF_ADC_CFG_PREFIX, _adc_enable)               ((__vsf_adc_t *)(__ADC))
#   define vsf_adc_disable(__ADC)                   VSF_MCONNECT(VSF_ADC_CFG_PREFIX, _adc_disable)              ((__vsf_adc_t *)(__ADC))
#   define vsf_adc_status(__ADC)                    VSF_MCONNECT(VSF_ADC_CFG_PREFIX, _adc_status)               ((__vsf_adc_t *)(__ADC))
#   define vsf_adc_capability(__ADC)                VSF_MCONNECT(VSF_ADC_CFG_PREFIX, _adc_capability)           ((__vsf_adc_t *)(__ADC))
#   define vsf_adc_irq_enable(__ADC, ...)           VSF_MCONNECT(VSF_ADC_CFG_PREFIX, _adc_irq_enable)           ((__vsf_adc_t *)(__ADC), ##__VA_ARGS__)
#   define vsf_adc_irq_disable(__ADC, ...)          VSF_MCONNECT(VSF_ADC_CFG_PREFIX, _adc_irq_disable)          ((__vsf_adc_t *)(__ADC), ##__VA_ARGS__)
#   define vsf_adc_channel_request_once(__ADC, ...) VSF_MCONNECT(VSF_ADC_CFG_PREFIX, _adc_channel_request_once) ((__vsf_adc_t *)(__ADC), ##__VA_ARGS__)
#   define vsf_adc_channel_config(__ADC, ...)       VSF_MCONNECT(VSF_ADC_CFG_PREFIX, _adc_channel_config)       ((__vsf_adc_t *)(__ADC), ##__VA_ARGS__)
#   define vsf_adc_channel_request(__ADC, ...)      VSF_MCONNECT(VSF_ADC_CFG_PREFIX, _adc_channel_request)      ((__vsf_adc_t *)(__ADC), ##__VA_ARGS__)
#endif
/// @endcond

#ifdef __cplusplus
}
#endif

#endif  /*__VSF_TEMPLATE_ADC_H__*/
