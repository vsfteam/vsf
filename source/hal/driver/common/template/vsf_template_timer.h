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

#ifndef __VSF_TEMPLATE_TIMER_H__
#define __VSF_TEMPLATE_TIMER_H__

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
#ifndef VSF_TIMER_CFG_MULTI_CLASS
#   define VSF_TIMER_CFG_MULTI_CLASS                    ENABLED
#endif

/**
 * \~english
 * @brief In the specific hardware driver, either VSF_HW_TIMER_COUNT or
 * VSF_HW_TIMER_MASK is defined.
 * \~chinese
 * @brief 在特定硬件驱动中，需要定义 VSF_HW_TIMER_COUNT 或
 * VSF_HW_TIMER_MASK 其中之一。
 */
#if defined(VSF_HW_TIMER_COUNT) && !defined(VSF_HW_TIMER_MASK)
#   define VSF_HW_TIMER_MASK                            VSF_HAL_COUNT_TO_MASK(VSF_HW_TIMER_COUNT)
#endif

#if defined(VSF_HW_TIMER_MASK) && !defined(VSF_HW_TIMER_COUNT)
#   define VSF_HW_TIMER_COUNT                           VSF_HAL_MASK_TO_COUNT(VSF_HW_TIMER_MASK)
#endif

/**
 * \~english
 * @brief Application code can redefine this prefix to specify a specific driver to call.
 * \~chinese
 * @brief 应用代码可以重新定义此前缀以指定要调用的特定驱动。
 */
#ifndef VSF_TIMER_CFG_PREFIX
#   if VSF_TIMER_CFG_MULTI_CLASS == ENABLED
#       define VSF_TIMER_CFG_PREFIX                     vsf
#   elif defined(VSF_HW_TIMER_COUNT) && (VSF_HW_TIMER_COUNT != 0)
#       define VSF_TIMER_CFG_PREFIX                     vsf_hw
#   else
#       define VSF_TIMER_CFG_PREFIX                     vsf
#   endif
#endif

/**
 * \~english
 * @brief Disable VSF_TIMER_CFG_FUNCTION_RENAME to use the original function names.
 * \~chinese
 * @brief 禁用 VSF_TIMER_CFG_FUNCTION_RENAME 以使用原始函数名。
 */
#ifndef VSF_TIMER_CFG_FUNCTION_RENAME
#   define VSF_TIMER_CFG_FUNCTION_RENAME                ENABLED
#endif

/**
 * \~english
 * @brief Enable macro VSF_TIMER_CFG_REIMPLEMENT_TYPE_IRQ_MASK in specific hardware
 * drivers to redefine enum vsf_timer_irq_mask_t.
 * \~chinese
 * @brief 在特定硬件驱动中启用宏 VSF_TIMER_CFG_REIMPLEMENT_TYPE_IRQ_MASK 来重新定义枚举
 * vsf_timer_irq_mask_t。
 */
#ifndef VSF_TIMER_CFG_REIMPLEMENT_TYPE_IRQ_MASK
#   define VSF_TIMER_CFG_REIMPLEMENT_TYPE_IRQ_MASK      DISABLED
#endif

/**
 * \~english
 * @brief Enable macro VSF_TIMER_CFG_REIMPLEMENT_TYPE_CFG in specific hardware
 * drivers to redefine struct vsf_timer_cfg_t. vsf_timer_isr_handler_t type also needs to
 * be redefined. For compatibility, members should not be deleted when redefining.
 * \~chinese
 * @brief 在具体硬件驱动中启用宏 VSF_TIMER_CFG_REIMPLEMENT_TYPE_CFG 来重新定义结构体
 * vsf_timer_cfg_t。vsf_timer_isr_handler_t 类型也需要重新定义。为了保持兼容性，
 * 重新定义时不应删除成员。
 */
#ifndef VSF_TIMER_CFG_REIMPLEMENT_TYPE_CFG
#    define VSF_TIMER_CFG_REIMPLEMENT_TYPE_CFG          DISABLED
#endif

/**
 * \~english
 * @brief Enable macro VSF_TIMER_CFG_REIMPLEMENT_TYPE_CHANNEL_MODE in specific hardware
 * drivers to redefine enum vsf_timer_channel_mode_t.
 * \~chinese
 * @brief 在具体硬件驱动中启用宏 VSF_TIMER_CFG_REIMPLEMENT_TYPE_CHANNEL_MODE 来重新定义枚举
 * vsf_timer_channel_mode_t。
 */
#ifndef VSF_TIMER_CFG_REIMPLEMENT_TYPE_CHANNEL_MODE
#   define VSF_TIMER_CFG_REIMPLEMENT_TYPE_CHANNEL_MODE  DISABLED
#endif

/**
 * \~english
 * @brief Enable macro VSF_TIMER_CFG_REIMPLEMENT_TYPE_CHANNEL_CFG in specific hardware
 * drivers to redefine struct vsf_timer_channel_cfg_t. For compatibility, members should
 * not be deleted when redefining.
 * \~chinese
 * @brief 在具体硬件驱动中启用宏 VSF_TIMER_CFG_REIMPLEMENT_TYPE_CHANNEL_CFG 来重新定义
 * 结构体 vsf_timer_channel_cfg_t。为了保持兼容性,重新定义时不应删除成员。
 */
#ifndef VSF_TIMER_CFG_REIMPLEMENT_TYPE_CHANNEL_CFG
#   define VSF_TIMER_CFG_REIMPLEMENT_TYPE_CHANNEL_CFG   DISABLED
#endif

/**
 * \~english
 * @brief Enable macro VSF_TIMER_CFG_REIMPLEMENT_TYPE_CHANNEL_REQUEST in specific hardware
 * drivers to redefine struct vsf_timer_channel_request_t. For compatibility, members should
 * not be deleted when redefining.
 * \~chinese
 * @brief 在具体硬件驱动中启用宏 VSF_TIMER_CFG_REIMPLEMENT_TYPE_CHANNEL_REQUEST 来重新定义
 * 结构体 vsf_timer_channel_request_t。为了保持兼容性,重新定义时不应删除成员。
 */
#ifndef VSF_TIMER_CFG_REIMPLEMENT_TYPE_CHANNEL_REQUEST
#   define VSF_TIMER_CFG_REIMPLEMENT_TYPE_CHANNEL_REQUEST   DISABLED
#endif

/**
 * \~english
 * @brief In the specific hardware driver, we can enable
 * VSF_TIMER_CFG_REIMPLEMENT_TYPE_STATUS to redefine struct vsf_timer_status_t.
 * \~chinese
 * @brief 在具体硬件驱动中，我们可以启用宏 VSF_TIMER_CFG_REIMPLEMENT_TYPE_STATUS
 * 来重新定义结构体 vsf_timer_status_t。
 */
#ifndef VSF_TIMER_CFG_REIMPLEMENT_TYPE_STATUS
#   define VSF_TIMER_CFG_REIMPLEMENT_TYPE_STATUS        DISABLED
#endif

/**
 * \~english
 * @brief In the specific hardware driver, we can enable
 * VSF_TIMER_CFG_REIMPLEMENT_TYPE_CTRL to redefine vsf_timer_ctrl_t as needed.
 * \~chinese
 * @brief 在具体硬件驱动中，我们可以启用宏 VSF_TIMER_CFG_REIMPLEMENT_TYPE_CTRL
 * 来重新定义 vsf_timer_ctrl_t。
 */
#ifndef VSF_TIMER_CFG_REIMPLEMENT_TYPE_CTRL
#   define VSF_TIMER_CFG_REIMPLEMENT_TYPE_CTRL          DISABLED
#endif

/**
 * \~english
 * @brief In the specific hardware driver, we can enable
 * VSF_TIMER_CFG_REIMPLEMENT_TYPE_CHANNEL_CTRL to redefine vsf_timer_channel_ctrl_t as needed.
 * \~chinese
 * @brief 在具体硬件驱动中，我们可以启用宏 VSF_TIMER_CFG_REIMPLEMENT_TYPE_CHANNEL_CTRL
 * 来重新定义 vsf_timer_channel_ctrl_t。
 */
#ifndef VSF_TIMER_CFG_REIMPLEMENT_TYPE_CHANNEL_CTRL
#   define VSF_TIMER_CFG_REIMPLEMENT_TYPE_CHANNEL_CTRL DISABLED
#endif

/**
 * \~english
 * @brief Enable macro VSF_TIMER_CFG_REIMPLEMENT_TYPE_CAPABILITY in specific hardware
 * drivers to redefine struct vsf_timer_capability_t. For compatibility, members should
 * not be deleted when redefining.
 * \~chinese
 * @brief 在具体硬件驱动中启用宏 VSF_TIMER_CFG_REIMPLEMENT_TYPE_CAPABILITY 来重新定义
 * 结构体 vsf_timer_capability_t。为了保持兼容性,重新定义时不应删除成员。
 */
#ifndef VSF_TIMER_CFG_REIMPLEMENT_TYPE_CAPABILITY
#   define VSF_TIMER_CFG_REIMPLEMENT_TYPE_CAPABILITY   DISABLED
#endif

/**
 * \~english
 * @brief Enable macro VSF_TIMER_CFG_INHERIT_HAL_CAPABILITY to inherit
 * capability from HAL.
 * \~chinese
 * @brief 启用宏 VSF_TIMER_CFG_INHERIT_HAL_CAPABILITY 来继承 HAL 的能力。
 */
#ifndef VSF_TIMER_CFG_INHERIT_HAL_CAPABILITY
#   define VSF_TIMER_CFG_INHERIT_HAL_CAPABILITY          ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

/**
 * \~english
 * @brief Timer API template macros for generating timer function declarations and implementations.
 * \~chinese
 * @brief 计时器 API 模板宏,用于生成计时器函数声明和实现。
 */
#define VSF_TIMER_APIS(__prefix_name) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              timer, init,                  VSF_MCONNECT(__prefix_name, _timer_t) *timer_ptr, vsf_timer_cfg_t *cfg_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                   timer, fini,                  VSF_MCONNECT(__prefix_name, _timer_t) *timer_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,               timer, enable,                VSF_MCONNECT(__prefix_name, _timer_t) *timer_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,               timer, disable,               VSF_MCONNECT(__prefix_name, _timer_t) *timer_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                   timer, irq_enable,            VSF_MCONNECT(__prefix_name, _timer_t) *timer_ptr, vsf_timer_irq_mask_t irq_mask) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                   timer, irq_disable,           VSF_MCONNECT(__prefix_name, _timer_t) *timer_ptr, vsf_timer_irq_mask_t irq_mask) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_timer_status_t,     timer, status,                VSF_MCONNECT(__prefix_name, _timer_t) *timer_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_timer_capability_t, timer, capability,            VSF_MCONNECT(__prefix_name, _timer_t) *timer_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              timer, set_period,            VSF_MCONNECT(__prefix_name, _timer_t) *timer_ptr, uint32_t period) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              timer, ctrl,                  VSF_MCONNECT(__prefix_name, _timer_t) *timer_ptr, vsf_timer_ctrl_t ctrl, void* param) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              timer, channel_config,        VSF_MCONNECT(__prefix_name, _timer_t) *timer_ptr, uint8_t channel, vsf_timer_channel_cfg_t *channel_cfg_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              timer, channel_start,         VSF_MCONNECT(__prefix_name, _timer_t) *timer_ptr, uint8_t channel) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              timer, channel_stop,          VSF_MCONNECT(__prefix_name, _timer_t) *timer_ptr, uint8_t channel) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              timer, channel_request_start, VSF_MCONNECT(__prefix_name, _timer_t) *timer_ptr, uint8_t channel, vsf_timer_channel_request_t *request_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              timer, channel_request_stop,  VSF_MCONNECT(__prefix_name, _timer_t) *timer_ptr, uint8_t channel) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              timer, channel_ctrl,          VSF_MCONNECT(__prefix_name, _timer_t) *timer_ptr, uint8_t channel, vsf_timer_channel_ctrl_t ctrl, void* param)

/*============================ TYPES =========================================*/

#if VSF_TIMER_CFG_REIMPLEMENT_TYPE_CHANNEL_MODE == DISABLED
typedef enum vsf_timer_channel_mode_t {
    // channel mode : different timer channel may support different mode
    // Base Mode : The timer channel will be used as a base timer
    VSF_TIMER_CHANNEL_MODE_BASE             = (0x00 << 0),

    /*
    // \~english Some timer may support more mode like PWM, Output Compare etc.
    // \~chinese 有些定时器可能支持更多的模式，如 PWM、输出比较等。

    // \~english PWM(Pulse Width Modulation) Mode : The timer channel will be used as a
    // PWM output, all channels's period should be the same
    // \~chinese PWM（脉宽调制）模式：定时器通道将用作 PWM 输出，所有通道的周期应该相同
    VSF_TIMER_CHANNEL_MODE_PWM              = (0x01 << 0),
    #define VSF_TIMER_CHANNEL_MODE_PWM  VSF_TIMER_CHANNEL_MODE_PWM

    // \~english Output Compare Mode: The timer channel will be used as a output compare,
    //           different channels can have different period (change pin level in interrupt)
    //           PWM mode can be seen as a special case of output compare mode, that is all
    //           channels have the same period.
    // \~chinese 输出比较模式：定时器通道将用作输出比较，不同通道可以有不同的周期（在中断中改变引脚电平）。
    //           PWM 模式可以看作是输出比较模式的一种特殊情况，即所有通道的周期相同。
    VSF_TIMER_CHANNEL_MODE_OUTPUT_COMPARE   = (0x02 << 0),
    #define VSF_TIMER_CHANNEL_MODE_OUTPUT_COMPARE  VSF_TIMER_CHANNEL_MODE_OUTPUT_COMPARE

    // \~english Input Capture Mode : The timer channel will be used as a input capture
    // \~chinese 输入捕获模式：定时器通道将用作输入捕获
    VSF_TIMER_CHANNEL_MODE_INPUT_CAPTURE    = (0x03 << 0),
    #define VSF_TIMER_CHANNEL_MODE_INPUT_CAPTURE  VSF_TIMER_CHANNEL_MODE_INPUT_CAPTURE

    // \~english Encoder Mode : The timer channel will be used as a encoder
    // \~chinese 编码器模式：定时器通道将用作编码器
    VSF_TIMER_CHANNEL_MODE_ENCODER          = (0x04 << 0),
    #define VSF_TIMER_CHANNEL_MODE_ENCODER  VSF_TIMER_CHANNEL_MODE_ENCODER

    // \~english One Pulse Mode : The timer channel will be used as a one pulse mode
    // \~chinese 单脉冲模式：定时器通道将用作单脉冲模式
    VSF_TIMER_CHANNEL_MODE_ONE_PULSE        = (0x05 << 0),
    #define VSF_TIMER_CHANNEL_MODE_ONE_PULSE  VSF_TIMER_CHANNEL_MODE_ONE_PULSE

    // \~english Hall Sensor Mode : The timer channel will be used as a hall sensor
    // \~chinese 霍尔传感器模式：定时器通道将用作霍尔传感器
    VSF_TIMER_CHANNEL_MODE_HALL_SENSOR      = (0x06 << 0),
    #define VSF_TIMER_CHANNEL_MODE_HALL_SENSOR  VSF_TIMER_CHANNEL_MODE_HALL_SENSOR
    */

    /********************************************************************************/
    //! \~english The following mode only valid for VSF_TIMER_CHANNEL_MODE_BASE
    //! \~chinese 以下模式仅适用于 VSF_TIMER_CHANNEL_MODE_BASE

    // Counter Mode
    // Oneshot : The timer will stop after the first overflow
    VSF_TIMER_BASE_ONESHOT                      = (0x00 << 5),
    // Continues : The timer will continue to count after overflow
    VSF_TIMER_BASE_CONTINUES                    = (0x01 << 5),
    /*
    // \~english Some timer may support more counter direction like center-aligned etc.
    //           The following mode only valid for VSF_TIMER_CHANNEL_MODE_BASE
    // \~chinese 有些定时器可能支持更多的计数模式，如中心对齐等。以下模式仅适用于 VSF_TIMER_CHANNEL_MODE_BASE
    VSF_TIMER_BASE_COUNTER_UP                   = (0x00 << 4),
    VSF_TIMER_BASE_COUNTER_DOWN                 = (0x01 << 4),
    VSF_TIMER_BASE_COUNTER_CENTER_ALIGNED1      = (0x02 << 4),
    VSF_TIMER_BASE_COUNTER_CENTER_ALIGNED2      = (0x03 << 4),
    VSF_TIMER_BASE_COUNTER_CENTER_ALIGNED3      = (0x04 << 4),
    #define VSF_TIMER_BASE_COUNTER_UP  VSF_TIMER_BASE_COUNTER_UP
    #define VSF_TIMER_BASE_COUNTER_DOWN  VSF_TIMER_BASE_COUNTER_DOWN
    #define VSF_TIMER_BASE_COUNTER_CENTER_ALIGNED1  VSF_TIMER_BASE_COUNTER_CENTER_ALIGNED1
    #define VSF_TIMER_BASE_COUNTER_CENTER_ALIGNED2  VSF_TIMER_BASE_COUNTER_CENTER_ALIGNED2
    #define VSF_TIMER_BASE_COUNTER_CENTER_ALIGNED3  VSF_TIMER_BASE_COUNTER_CENTER_ALIGNED3
    */

    /*
    // \~english Some timer may support more clock division like 1, 2, 4 etc.
    // \~chinese 有些定时器可能支持更多的时钟分频，如 1、2、4 等。
    VSF_TIEMR_BASE_CLOCKDIVISION_DIV_1           = (0x00 << 8),
    VSF_TIEMR_BASE_CLOCKDIVISION_DIV_2           = (0x01 << 8),
    VSF_TIEMR_BASE_CLOCKDIVISION_DIV_4           = (0x02 << 8),
    #define VSF_TIEMR_BASE_CLOCKDIVISION_DIV_1  VSF_TIEMR_BASE_CLOCKDIVISION_DIV_1
    #define VSF_TIEMR_BASE_CLOCKDIVISION_DIV_2  VSF_TIEMR_BASE_CLOCKDIVISION_DIV_2
    #define VSF_TIEMR_BASE_CLOCKDIVISION_DIV_4  VSF_TIEMR_BASE_CLOCKDIVISION_DIV_4
    */

    /********************************************************************************/
    /*
    // \~english The following mode only valid for VSF_TIMER_CHANNEL_MODE_PWM
    // \~chinese 以下模式仅适用于 VSF_TIMER_CHANNEL_MODE_PWM
    // Output Compare Mode
    VSF_TIMER_OUTPUT_COMPARE_ACTIVE             = (0x00 << 4),
    VSF_TIMER_OUTPUT_COMPARE_INACTIVE           = (0x01 << 4),
    VSF_TIMER_OUTPUT_COMPARE_TOGGLE             = (0x02 << 4),
    VSF_TIMER_OUTPUT_COMPARE_PWM1               = (0x03 << 4),
    VSF_TIMER_OUTPUT_COMPARE_PWM2               = (0x04 << 4),
    VSF_TIMER_OUTPUT_COMPARE_FORCED_ACTIVE      = (0x05 << 4),
    VSF_TIMER_OUTPUT_COMPARE_FORCED_INACTIVE    = (0x06 << 4),
    #define VSF_TIMER_OUTPUT_COMPARE_ACTIVE  VSF_TIMER_OUTPUT_COMPARE_ACTIVE
    #define VSF_TIMER_OUTPUT_COMPARE_INACTIVE  VSF_TIMER_OUTPUT_COMPARE_INACTIVE
    #define VSF_TIMER_OUTPUT_COMPARE_TOGGLE  VSF_TIMER_OUTPUT_COMPARE_TOGGLE
    #define VSF_TIMER_OUTPUT_COMPARE_PWM1  VSF_TIMER_OUTPUT_COMPARE_PWM1
    #define VSF_TIMER_OUTPUT_COMPARE_PWM2  VSF_TIMER_OUTPUT_COMPARE_PWM2
    #define VSF_TIMER_OUTPUT_COMPARE_FORCED_ACTIVE  VSF_TIMER_OUTPUT_COMPARE_FORCED_ACTIVE
    #define VSF_TIMER_OUTPUT_COMPARE_FORCED_INACTIVE  VSF_TIMER_OUTPUT_COMPARE_FORCED_INACTIVE
    */

    /********************************************************************************/
    /*
    // \~english The following mode only valid for VSF_TIMER_CHANNEL_MODE_OUTPUT_COMPARE
    // \~chinese 以下模式仅适用于 VSF_TIMER_CHANNEL_MODE_OUTPUT_COMPARE

    // \~english Output Compare Polarity and N Polarity
    // \~chinese 输出比较极性和 N 极性
    VSF_TIMER_OUTPUT_COMPARE_POLARITY_HIGH      = (0x00 << 8),
    VSF_TIMER_OUTPUT_COMPARE_POLARITY_LOW       = (0x01 << 8),
    VSF_TIMER_OUTPUT_COMPARE_N_POLARITY_HIGH    = (0x00 << 8),
    VSF_TIMER_OUTPUT_COMPARE_N_POLARITY_LOW     = (0x01 << 8),
    #define VSF_TIMER_OUTPUT_COMPARE_POLARITY_HIGH  VSF_TIMER_OUTPUT_COMPARE_POLARITY_HIGH
    #define VSF_TIMER_OUTPUT_COMPARE_POLARITY_LOW   VSF_TIMER_OUTPUT_COMPARE_POLARITY_LOW
    #define VSF_TIMER_OUTPUT_COMPARE_N_POLARITY_HIGH  VSF_TIMER_OUTPUT_COMPARE_N_POLARITY_HIGH
    #define VSF_TIMER_OUTPUT_COMPARE_N_POLARITY_LOW   VSF_TIMER_OUTPUT_COMPARE_N_POLARITY_LOW
    */

    /********************************************************************************/
    /*
    // \~english The following mode only valid for VSF_TIMER_CHANNEL_MODE_PWM
    // \~chinese 以下模式仅适用于 VSF_TIMER_CHANNEL_MODE_PWM

    // \~english PWM Output Polarity
    // \~chinese PWM 输出极性
    VSF_TIMER_PWM_OUTPUT_POLARITY_HIGH                  = (0x00 << 4),
    VSF_TIMER_PWM_OUTPUT_POLARITY_LOW                   = (0x01 << 4),
    #define VSF_TIMER_PWM_OUTPUT_POLARITY_HIGH  VSF_TIMER_PWM_OUTPUT_POLARITY_HIGH
    #define VSF_TIMER_PWM_OUTPUT_POLARITY_LOW   VSF_TIMER_PWM_OUTPUT_POLARITY_LOW
    */

    /********************************************************************************/
    /*
    // \~english The following mode only valid for VSF_TIMER_CHANNEL_MODE_INPUT_CAPTURE
    // \~chinese 以下模式仅适用于 VSF_TIMER_CHANNEL_MODE_INPUT_CAPTURE

    // \~english Input Capture Polarity
    // \~chinese 输入捕获极性
    VSF_TIMER_INPUT_CAPTURE_POLARITY_RISING             = (0x00 << 4),
    VSF_TIMER_INPUT_CAPTURE_POLARITY_FALLING            = (0x01 << 4),
    VSF_TIMER_INPUT_CAPTURE_POLARITY_BOTH               = (0x02 << 4),
    #define VSF_TIMER_INPUT_CAPTURE_POLARITY_RISING  VSF_TIMER_INPUT_CAPTURE_POLARITY_RISING
    #define VSF_TIMER_INPUT_CAPTURE_POLARITY_FALLING  VSF_TIMER_INPUT_CAPTURE_POLARITY_FALLING
    #define VSF_TIMER_INPUT_CAPTURE_POLARITY_BOTH  VSF_TIMER_INPUT_CAPTURE_POLARITY_BOTH

    // \~english Input Capture Filter
    // \~chinese 输入捕获滤波
    VSF_TIMER_INPUT_CAPTURE_FILTER_MASK                 = (0x0F << 4),
    VSF_TIMER_INPUT_CAPTURE_FILTER_OFFSET               = 4,
    */

    /********************************************************************************/
    /*
    // \~english The following mode only valid for VSF_TIMER_CHANNEL_MODE_ENCODER
    // \~chinese 以下模式仅适用于 VSF_TIMER_CHANNEL_MODE_ENCODER

    // \~english Encoder Mode
    // \~chinese 编码器模式
    VSF_TIMER_ENCODER_CHANNEL_A_POLARITY_RISING         = (0x00 << 4),
    VSF_TIMER_ENCODER_CHANNEL_A_POLARITY_FALLING        = (0x01 << 4),
    VSF_TIMER_ENCODER_CHANNEL_A_POLARITY_BOTH           = (0x02 << 4),
    VSF_TIMER_ENCODER_CHANNEL_B_POLARITY_RISING         = (0x00 << 6),
    VSF_TIMER_ENCODER_CHANNEL_B_POLARITY_FALLING        = (0x01 << 6),
    VSF_TIMER_ENCODER_CHANNEL_B_POLARITY_BOTH           = (0x02 << 6),
    #define VSF_TIMER_ENCODER_CHANNEL_A_POLARITY_RISING  VSF_TIMER_ENCODER_CHANNEL_A_POLARITY_RISING
    #define VSF_TIMER_ENCODER_CHANNEL_A_POLARITY_FALLING  VSF_TIMER_ENCODER_CHANNEL_A_POLARITY_FALLING
    #define VSF_TIMER_ENCODER_CHANNEL_A_POLARITY_BOTH  VSF_TIMER_ENCODER_CHANNEL_A_POLARITY_BOTH
    #define VSF_TIMER_ENCODER_CHANNEL_B_POLARITY_RISING  VSF_TIMER_ENCODER_CHANNEL_B_POLARITY_RISING
    #define VSF_TIMER_ENCODER_CHANNEL_B_POLARITY_FALLING  VSF_TIMER_ENCODER_CHANNEL_B_POLARITY_FALLING
    #define VSF_TIMER_ENCODER_CHANNEL_B_POLARITY_BOTH  VSF_TIMER_ENCODER_CHANNEL_B_POLARITY_BOTH
    */
} vsf_timer_channel_mode_t;
#endif

enum {
#ifndef VSF_TIMER_CHANNEL_MODE_MASK
    VSF_TIMER_CHANNEL_MODE_MASK = VSF_TIMER_CHANNEL_MODE_BASE
#ifdef VSF_TIMER_CHANNEL_MODE_PWM
                                | VSF_TIMER_CHANNEL_MODE_PWM
#endif
#ifdef VSF_TIMER_CHANNEL_MODE_OUTPUT_COMPARE
                                | VSF_TIMER_CHANNEL_MODE_OUTPUT_COMPARE
#endif
#ifdef VSF_TIMER_CHANNEL_MODE_INPUT_CAPTURE
                                | VSF_TIMER_CHANNEL_MODE_INPUT_CAPTURE
#endif
#ifdef VSF_TIMER_CHANNEL_MODE_ENCODER
                                | VSF_TIMER_CHANNEL_MODE_ENCODER
#endif
#ifdef VSF_TIMER_CHANNEL_MODE_ONE_PULSE
                                | VSF_TIMER_CHANNEL_MODE_ONE_PULSE
#endif
#ifdef VSF_TIMER_CHANNEL_MODE_HALL_SENSOR
                                | VSF_TIMER_CHANNEL_MODE_HALL_SENSOR
#endif
                                ,
#endif

#ifndef VSF_TIMER_BASE_COUNTER_MASK
    VSF_TIMER_BASE_COUNTER_MASK = VSF_TIMER_BASE_ONESHOT
                                | VSF_TIMER_BASE_CONTINUES,
#endif

#ifndef VSF_TIMER_CHANNEL_MODE_ALL_MASK
    VSF_TIMER_CHANNEL_MODE_ALL_MASK = VSF_TIMER_CHANNEL_MODE_MASK
                                    | VSF_TIMER_BASE_COUNTER_MASK
#endif
};

#if VSF_TIMER_CFG_REIMPLEMENT_TYPE_IRQ_MASK == DISABLED
typedef enum vsf_timer_irq_mask_t {
    VSF_TIMER_IRQ_MASK_OVERFLOW = (0x01 << 0),

    /*
    // \~english We can define more channel irq mask here
    // \~chinese 我们可以在这里定义更多的通道的不同模式的中断掩码

    VSF_TIMER_IRQ_MASK_CHANNEL_0_INPUT_CAPTURE  = (0x01 << 1),
    VSF_TIMER_IRQ_MASK_CHANNEL_0_OUTPUT_COMPARE = (0x01 << 2),
    VSF_TIMER_IRQ_MASK_CHANNEL_0_PWM            = (0x01 << 3),

    // We can define more channel irq mask here
    VSF_TIMER_IRQ_MASK_CHANNEL_1_INPUT_CAPTURE = (0x01 << 3),
    #define VSF_TIMER_IRQ_MASK_CHANNEL_1_INPUT_CAPTURE  VSF_TIMER_IRQ_MASK_CHANNEL_1_INPUT_CAPTURE
    VSF_TIMER_IRQ_MASK_CHANNEL_1_OUTPUT_COMPARE = (0x01 << 4),
    #define VSF_TIMER_IRQ_MASK_CHANNEL_1_OUTPUT_COMPARE  VSF_TIMER_IRQ_MASK_CHANNEL_1_OUTPUT_COMPARE
    VSF_TIMER_IRQ_MASK_CHANNEL_2_INPUT_CAPTURE = (0x01 << 5),
    #define VSF_TIMER_IRQ_MASK_CHANNEL_2_INPUT_CAPTURE  VSF_TIMER_IRQ_MASK_CHANNEL_2_INPUT_CAPTURE
    VSF_TIMER_IRQ_MASK_CHANNEL_2_OUTPUT_COMPARE = (0x01 << 6),
    #define VSF_TIMER_IRQ_MASK_CHANNEL_2_OUTPUT_COMPARE  VSF_TIMER_IRQ_MASK_CHANNEL_2_OUTPUT_COMPARE
    */
} vsf_timer_irq_mask_t;
#endif

enum {
    VSF_TIMER_IRQ_COUNT         = 1,
    VSF_TIMER_IRQ_ALL_BITS_MASK = VSF_TIMER_IRQ_MASK_OVERFLOW,
};

#if VSF_TIMER_CFG_REIMPLEMENT_TYPE_CFG == DISABLED
typedef struct vsf_timer_t vsf_timer_t;

typedef void vsf_timer_isr_handler_t(void *target_ptr,
                                     vsf_timer_t *timer_ptr,
                                     vsf_timer_irq_mask_t irq_mask);

typedef struct vsf_timer_isr_t {
    vsf_timer_isr_handler_t *handler_fn;
    void *target_ptr;
    vsf_arch_prio_t prio;
} vsf_timer_isr_t;

//! timer configuration
/**
 * \~english
 * @brief Timer configuration structure
 *
 * \~chinese
 * @brief 定时器配置结构体
 */
typedef struct vsf_timer_cfg_t {
    uint32_t period;          //!< \~english Period in Tick \~chinese 周期（单位：Tick）
    union {
        uint32_t freq;
        uint32_t min_freq;
    };
    vsf_timer_isr_t isr;         //!< \~english ISR config \~chinese 中断配置
} vsf_timer_cfg_t;
#endif

#if VSF_TIMER_CFG_REIMPLEMENT_TYPE_CHANNEL_CFG == DISABLED
typedef struct vsf_timer_channel_cfg_t {
    vsf_timer_channel_mode_t mode;
    uint32_t                 pulse;
} vsf_timer_channel_cfg_t;
#endif


#if VSF_TIMER_CFG_REIMPLEMENT_TYPE_CTRL == DISABLED
/**
 *
 * \~english
 * @brief Predefined VSF TIMER control command that can be reimplemented in specific hal drivers.
 * \~chinese
 * @brief 预定义的 VSF TIMER 控制命令，可以在具体的 HAL 驱动重新实现。
 *
 * \~english
 *  Optional control command require one or more enumeration options and a macro with the same
 *  name to determine if they are supported at runtime. If the feature supports more than
 *  one option, it is recommended to provide the corresponding MASK option, so that the
 *  user can switch to different modes at compile-time.
 * \~chinese
 * 可选控制命令需要提供一个或者多个枚举选项，还需要提供同名的宏，方便用户在编译时判断是否支持。
 * 如果它支持多个选项，建议提供对应的 MASK 选项，方便用户在运行时切换到不同的模式。
 */
typedef enum vsf_timer_ctrl_t {
    __VSF_TIMER_CTRL_DUMMY = 0,               //!< \~english Dummy value for compilation \~chinese 编译占位值
} vsf_timer_ctrl_t;
#endif

#if VSF_TIMER_CFG_REIMPLEMENT_TYPE_CHANNEL_CTRL == DISABLED
/**
 *
 * \~english
 * @brief Predefined VSF TIMER control command that can be reimplemented in specific hal drivers.
 * \~chinese
 * @brief 预定义的 VSF TIMER 控制命令，可以在具体的 HAL 驱动重新实现。
 *
 * \~english
 *  Optional control command require one or more enumeration options and a macro with the same
 *  name to determine if they are supported at runtime. If the feature supports more than
 *  one option, it is recommended to provide the corresponding MASK option, so that the
 *  user can switch to different modes at compile-time.
 * \~chinese
 * 可选控制命令需要提供一个或者多个枚举选项，还需要提供同名的宏，方便用户在编译时判断是否支持。
 * 如果它支持多个选项，建议提供对应的 MASK 选项，方便用户在运行时切换到不同的模式。
 */
typedef enum vsf_timer_channel_ctrl_t {
    __VSF_TIMER_CHANNEL_CTRL_DUMMY = 0,               //!< \~english Dummy value for compilation \~chinese 编译占位值
} vsf_timer_channel_ctrl_t;
#endif

#if VSF_TIMER_CFG_REIMPLEMENT_TYPE_CHANNEL_CFG == DISABLED
typedef struct vsf_timer_channel_request_t {
    uint16_t length;
    union {
        // for VSF_TIMER_CHANNEL_MODE_BASE mode
        uint32_t *period_buffer;
        // for VSF_TIMER_CHANNEL_MODE_PWM mode and VSF_TIMER_CHANNEL_MODE_OUTPUT_COMPARE mode
        uint32_t *pulse_buffer;
        // for VSF_TIMER_CHANNEL_MODE_INPUT_CAPTURE mode
        uint32_t *input_capture_buffer;
        // for VSF_TIMER_CHANNEL_MODE_ENCODER mode
        struct {
            uint32_t *channel_a_buffer;
            uint32_t *channel_b_buffer;
        };
    };
} vsf_timer_channel_request_t;
#endif

/**
 * \~english
 * @brief TIMER status information structure
 * Contains the current status of TIMER operations
 * \~chinese
 * @brief TIMER 状态信息结构体
 * 包含 TIMER 操作的当前状态
 */
#if VSF_TIMER_CFG_REIMPLEMENT_TYPE_STATUS == DISABLED
typedef struct vsf_timer_status_t {
    union {
        inherit(vsf_peripheral_status_t)
        uint32_t value;
    };
} vsf_timer_status_t;
#endif


#if VSF_TIMER_CFG_REIMPLEMENT_TYPE_CAPABILITY == DISABLED
typedef struct vsf_timer_capability_t {
#if VSF_TIMER_CFG_INHERIT_HAL_CAPABILITY == ENABLED
    inherit(vsf_peripheral_capability_t)
#endif
    // support interrupt for timer
    vsf_timer_irq_mask_t irq_mask;

    // timer bit length, like 16bit, 32bit etc.
    uint8_t timer_bitlen;
    // timer channel count, like 1, 2, 4 etc.
    uint8_t channel_cnt;

    // support different channel mode
    // we can redefine it if different channel support different mode.
    uint8_t support_pwm : 1;
    uint8_t support_output_compare : 1;
    uint8_t support_input_capture : 1;
    uint8_t support_one_pulse : 1;
} vsf_timer_capability_t;
#endif

typedef struct vsf_timer_op_t {
/// @cond
#undef __VSF_HAL_TEMPLATE_API
#define __VSF_HAL_TEMPLATE_API VSF_HAL_TEMPLATE_API_FP
/// @endcond

    VSF_TIMER_APIS(vsf)
} vsf_timer_op_t;

#if VSF_TIMER_CFG_MULTI_CLASS == ENABLED
struct vsf_timer_t  {
    const vsf_timer_op_t * op;
};
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

/**
 * \~english
 * @brief Initialize a Timer instance
 * @param[in] timer_ptr: pointer to Timer instance
 * @param[in] cfg_ptr: pointer to configuration structure
 * @return vsf_err_t: VSF_ERR_NONE if successful, otherwise return error code
 *
 *
 * @note It is not necessary to call vsf_timer_fini() to deinitialization.
 *       vsf_timer_init() should be called before any other Timer API except vsf_timer_capability().
 * \~chinese
 * @brief 初始化定时器实例
 * @param[in] timer_ptr: 指向结构体 @ref vsf_timer_t 的指针
 * @param[in] cfg_ptr: 指向配置结构体 @ref vsf_timer_cfg_t 的指针
 * @return vsf_err_t: 如果初始化成功返回 VSF_ERR_NONE，否则返回错误码
 *
 * @note 失败后不需要调用 vsf_timer_fini() 进行反初始化。
 *       vsf_timer_init() 应该在除 vsf_timer_capability() 之外的其他 Timer API 之前调用。
 */
extern vsf_err_t vsf_timer_init(vsf_timer_t *timer_ptr, vsf_timer_cfg_t *cfg_ptr);

/**
 \~english
 @brief Finalize a TIMER instance
 @param[in] timer_ptr: a pointer to structure @ref vsf_timer_t
 @return none

 \~chinese
 @brief 终止一个 TIMER 实例
 @param[in] timer_ptr: 指向结构体 @ref vsf_timer_t 的指针
 @return 无
 */
extern void vsf_timer_fini(vsf_timer_t *timer_ptr);

/**
 \~english
 @brief Enable a TIMER instance
 @param[in] timer_ptr: a pointer to structure @ref vsf_timer_t
 @return fsm_rt_t: FSM_RT_CPL if TIMER was enabled, fsm_rt_on_going if TIMER is still enabling

 \~chinese
 @brief 启用一个 TIMER 实例
 @param[in] timer_ptr: 指向结构体 @ref vsf_timer_t 的指针
 @return fsm_rt_t: 如果 TIMER 实例已启用返回 FSM_RT_CPL，如果 TIMER 实例正在启用过程中返回 fsm_rt_on_going
 */
extern fsm_rt_t vsf_timer_enable(vsf_timer_t *timer_ptr);

/**
 \~english
 @brief Disable a TIMER instance
 @param[in] timer_ptr: a pointer to structure @ref vsf_timer_t
 @return fsm_rt_t: FSM_RT_CPL if TIMER was disabled, fsm_rt_on_going if TIMER is still disabling

 \~chinese
 @brief 禁用一个 TIMER 实例
 @param[in] timer_ptr: 指向结构体 @ref vsf_timer_t 的指针
 @return fsm_rt_t: 如果 TIMER 实例已禁用返回 FSM_RT_CPL，如果 TIMER 实例正在禁用过程中返回 fsm_rt_on_going
 */
extern fsm_rt_t vsf_timer_disable(vsf_timer_t *timer_ptr);

/**
 \~english
 @brief Enable interrupt masks of TIMER instance
 @param[in] timer_ptr: a pointer to structure @ref vsf_timer_t
 @param[in] irq_mask: one or more value of enum @ref vsf_timer_irq_mask_t
 @return none
 @note All pending interrupts should be cleared before interrupts are enabled.

 \~chinese
 @brief 启用 TIMER 实例的中断
 @param[in] timer_ptr: 指向结构体 @ref vsf_timer_t 的指针
 @param[in] irq_mask: 一个或多个枚举 vsf_timer_irq_mask_t 的值的按位或，@ref vsf_timer_irq_mask_t
 @return 无
 @note 在中断启用之前，应该清除所有挂起的中断。
 */
extern void vsf_timer_irq_enable(vsf_timer_t *timer_ptr, vsf_timer_irq_mask_t irq_mask);

/**
 \~english
 @brief Disable interrupt masks of TIMER instance
 @param[in] timer_ptr: a pointer to structure @ref vsf_timer_t
 @param[in] irq_mask: one or more value of enum vsf_timer_irq_mask_t, @ref vsf_timer_irq_mask_t
 @return none

 \~chinese
 @brief 禁用 TIMER 实例的中断
 @param[in] timer_ptr: 指向结构体 @ref vsf_timer_t 的指针
 @param[in] irq_mask: 一个或多个枚举 vsf_timer_irq_mask_t 的值的按位或，@ref vsf_timer_irq_mask_t
 @return 无
 */
extern void vsf_timer_irq_disable(vsf_timer_t *timer_ptr, vsf_timer_irq_mask_t irq_mask);

/**
 \~english
 @brief Get the status of TIMER instance
 @param[in] timer_ptr: a pointer to structure @ref vsf_timer_t
 @return vsf_timer_status_t: return all status of current TIMER

 \~chinese
 @brief 获取 TIMER 实例的状态
 @param[in] timer_ptr: 指向结构体 @ref vsf_timer_t 的指针
 @return vsf_timer_status_t: 返回当前 TIMER 的所有状态
 */
extern vsf_timer_status_t vsf_timer_status(vsf_timer_t *timer_ptr);

/**
 \~english
 @brief get the capability of TIMER instance.
 @param[in] timer_ptr: a pointer to structure @ref vsf_timer_t
 @return vsf_timer_capability_t: all capability of current TIMER @ref vsf_timer_capability_t

 \~chinese
 @brief 获取 TIMER 实例的能力
 @param[in] timer_ptr: 指向结构体 @ref vsf_timer_t 的指针
 @return vsf_timer_capability_t: 返回当前 TIMER 的所有能力 @ref vsf_timer_capability_t
 */
extern vsf_timer_capability_t vsf_timer_capability(vsf_timer_t *timer_ptr);

/**
 \~english
 @brief TIMER set period (maximum count), used for change the period of TIMER dynamically
 @param[in] timer_ptr: a pointer to structure @ref vsf_timer_t
 @param[in] period: timer period width (in clock counter)
 @return vsf_err_t: VSF_ERR_NONE if the TIMER set period was successfully, or a negative error code

 \~chinese
 @brief TIMER 设置一个定时器的周期（最大计数值）, 用于动态改变定时器的周期
 @param[in] timer_ptr: 指向结构体 @ref vsf_timer_t 的指针
 @param[in] period: TIMER 周期宽度 (时钟计数)
 @return vsf_err_t: 如果 TIMER 设置周期成功返回 VSF_ERR_NONE，否则返回负数。
 */
extern vsf_err_t vsf_timer_set_period(vsf_timer_t *timer_ptr, uint32_t period);

/**
 * \~english
 * @brief Execute a control command on the TIMER instance
 * @param[in,out] timer_ptr: Pointer to TIMER instance structure @ref vsf_timer_t
 * @param[in] ctrl: Control command from @ref vsf_timer_ctrl_t enumeration
 * @param[in] param: Command-specific parameter (can be NULL depending on command)
 * @return vsf_err_t: VSF_ERR_NONE if command executed successfully,
 *                    VSF_ERR_NOT_SUPPORT if command is not supported,
 *                    other negative error codes for specific failures
 * @note Available commands and their parameters are hardware-dependent
 * @note Some commands may not be supported on all hardware platforms
 * \~chinese
 * @brief 执行 TIMER 实例的控制命令
 * @param[in,out] timer_ptr: TIMER 实例结构 @ref vsf_timer_t 的指针
 * @param[in] ctrl: 来自 @ref vsf_timer_ctrl_t 枚举的控制命令
 * @param[in] param: 命令特定的参数（根据命令可以为 NULL）
 * @return vsf_err_t: 如果命令执行成功返回 VSF_ERR_NONE，
 *                    如果命令不支持返回 VSF_ERR_NOT_SUPPORT，
 *                    其他特定失败情况返回相应的负数错误码
 * @note 可用的命令和它们的参数取决于硬件
 * @note 某些命令可能不是在所有硬件平台上都支持
 */
extern vsf_err_t vsf_timer_ctrl(vsf_timer_t *timer_ptr, vsf_timer_ctrl_t ctrl, void *param);

/**
 \~english
 @brief TIMER set the period width and pulse width for a channel
 @param[in] timer_ptr: a pointer to structure @ref vsf_timer_t
 @param[in] channel: timer channel
 @param[in] channel_cfg_ptr: a pointer to structure @ref vsf_timer_channel_cfg_t
 @return vsf_err_t: VSF_ERR_NONE if the TIMER channel set was successfully, or a negative error code

 \~chinese
 @brief TIMER 通道 设置一个通道的周期和宽度
 @param[in] timer_ptr: 指向结构体 @ref vsf_timer_t 的指针
 @param[in] channel: TIMER 通道
 @param[in] channel_cfg_ptr: a pointer to structure @ref vsf_timer_channel_cfg_t
 @return vsf_err_t: 如果 TIMER 通道设置成功返回 VSF_ERR_NONE，否则返回负数。
 */
extern vsf_err_t vsf_timer_channel_config(vsf_timer_t             *timer_ptr,
                                          uint8_t                 channel,
                                          vsf_timer_channel_cfg_t *channel_cfg_ptr);

/**
 \~english
 @brief start a TIMER channel
 @param[in] timer_ptr: a pointer to structure @ref vsf_timer_t
 @param[in] channel: timer channel
 @return vsf_err_t: VSF_ERR_NONE if the TIMER channel start was successfully, or a negative error code

 \~chinese
 @brief 开始一个 TIMER 通道
 @param[in] timer_ptr: 指向结构体 @ref vsf_timer_t 的指针
 @param[in] channel: TIMER 通道
 @return vsf_err_t: 如果 TIMER 通道开始成功返回 VSF_ERR_NONE，否则返回负数。
 */
extern vsf_err_t vsf_timer_channel_start(vsf_timer_t *timer_ptr, uint8_t channel);

/**
 \~english
 @brief stop a TIMER channel
 @param[in] timer_ptr: a pointer to structure @ref vsf_timer_t
 @param[in] channel: timer channel
 @return vsf_err_t: VSF_ERR_NONE if the TIMER channel stop was successfully, or a negative error code

 \~chinese
 @brief 停止一个 TIMER 通道
 @param[in] timer_ptr: 指向结构体 @ref vsf_timer_t 的指针
 @param[in] channel: TIMER 通道
 @return vsf_err_t: 如果 TIMER 通道停止成功返回 VSF_ERR_NONE, 否则返回负数。
 */
extern vsf_err_t vsf_timer_channel_stop(vsf_timer_t *timer_ptr, uint8_t channel);

/**
 \~english
 @brief start a TIMER channel request (usually based on DMA)
 @param[in] timer_ptr: a pointer to structure @ref vsf_timer_t
 @param[in] channel: timer channel
 @param[in] request_ptr: a pointer to request structure @ref vsf_timer_channel_request_t
            For VSF_TIMER_CHANNEL_MODE_BASE mode, period_buffer in structure
            @ref vsf_timer_channel_request_t is array of period value to be set after each overflow.
            For VSF_TIMER_CHANNEL_MODE_PWM mode, pwm_buffer in structure
            @ref vsf_timer_channel_request_t is array of pulse value to be set after each overflow.
            For VSF_TIMER_CHANNEL_MODE_OUTPUT_COMPARE mode, pulse_buffer in structure
            @ref vsf_timer_channel_request_t is array of pulse value to be set after each overflow.
            For VSF_TIMER_CHANNEL_MODE_INPUT_CAPTURE mode, input_capture_buffer in structure
            @ref vsf_timer_channel_request_t is array of input capture value.
            For VSF_TIMER_CHANNEL_MODE_ENCODER mode, channel_a_buffer and channel_b_buffer in structure
            @ref vsf_timer_channel_request_t is array of encoder value for channel A and channel B.
 @return vsf_err_t: VSF_ERR_NONE if the TIMER channel start was successfully, or a negative error code

 \~chinese
 @brief 开始一个 TIMER 通道的请求 (通常基于 DMA)
 @param[in] timer_ptr: 指向结构体 @ref vsf_timer_t 的指针
 @param[in] channel: TIMER 通道
 @param[in] request_ptr: 指向结构体 vsf_timer_channel_request_t 的指针，参考 @ref vsf_timer_channel_request_t
            对于 VSF_TIMER_CHANNEL_MODE_BASE 模式, @ref vsf_timer_channel_request_t 中的 period_buffer 是每次溢出后要设置的周期值数组。
            对于 VSF_TIMER_CHANNEL_MODE_PWM 模式, @ref vsf_timer_channel_request_t 中的 pwm_buffer 是每次溢出后要设置的脉冲值数组。
            对于 VSF_TIMER_CHANNEL_MODE_OUTPUT_COMPARE 模式, @ref vsf_timer_channel_request_t 中的 pulse_buffer 是每次溢出后要设置的脉冲值数组。
            对于 VSF_TIMER_CHANNEL_MODE_INPUT_CAPTURE 模式, @ref vsf_timer_channel_request_t 中的 input_capture_buffer 是输入捕获值数组。
            对于 VSF_TIMER_CHANNEL_MODE_ENCODER 模式, @ref vsf_timer_channel_request_t 中的 channel_a_buffer 和 channel_b_buffer 是通道 A 和通道 B 的编码器值数组。
 @return vsf_err_t: 如果 TIMER 通道开始成功返回 VSF_ERR_NONE, 否则返回负数。
 */
extern vsf_err_t vsf_timer_channel_request_start(vsf_timer_t *timer_ptr, uint8_t channel, vsf_timer_channel_request_t *request_ptr);

/**
 \~english
 @brief stop a TIMER channel request.
 @param[in] timer_ptr: a pointer to structure @ref vsf_timer_t
 @param[in] channel: timer channel
 @return vsf_err_t: VSF_ERR_NONE if the TIMER channel stop was successfully, or a negative error code

 \~chinese
 @brief 停止一个 TIMER 通道的请求
 @param[in] timer_ptr: 指向结构体 @ref vsf_timer_t 的指针
 @param[in] channel: TIMER 通道
 @return vsf_err_t: 如果 TIMER 通道停止成功返回 VSF_ERR_NONE, 否则返回负数。
 */
extern vsf_err_t vsf_timer_channel_request_stop(vsf_timer_t *timer_ptr, uint8_t channel);

/**
 * \~english
 * @brief Execute a control command on the TIMER channel
 * @param[in,out] timer_ptr: Pointer to TIMER instance structure @ref vsf_timer_t
 * @param[in] channel: timer channel
 * @param[in] ctrl: Control command from @ref vsf_timer_channel_ctrl_t enumeration
 * @param[in] param: Command-specific parameter (can be NULL depending on command)
 * @return vsf_err_t: VSF_ERR_NONE if command executed successfully,
 *                    VSF_ERR_NOT_SUPPORT if command is not supported,
 *                    other negative error codes for specific failures
 * @note Available commands and their parameters are hardware-dependent
 * @note Some commands may not be supported on all hardware platforms
 * \~chinese
 * @brief 执行 TIMER 通道的控制命令
 * @param[in,out] timer_ptr: TIMER 实例结构 @ref vsf_timer_t 的指针
 * @param[in] channel: TIMER 通道
 * @param[in] ctrl: 来自 @ref vsf_timer_channel_ctrl_t 枚举的控制命令
 * @param[in] param: 命令特定的参数（根据命令可以为 NULL）
 * @return vsf_err_t: 如果命令执行成功返回 VSF_ERR_NONE，
 *                    如果命令不支持返回 VSF_ERR_NOT_SUPPORT，
 *                    其他特定失败情况返回相应的负数错误码
 * @note 可用的命令和它们的参数取决于硬件
 * @note 某些命令可能不是在所有硬件平台上都支持
 */
extern vsf_err_t vsf_timer_channel_ctrl(vsf_timer_t *timer_ptr, uint8_t channel, vsf_timer_channel_ctrl_t ctrl, void *param);

/*============================ MACROFIED FUNCTIONS ===========================*/

/// @cond
#if VSF_TIMER_CFG_FUNCTION_RENAME == ENABLED
#   define __vsf_timer_t                                VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_t)
#   define vsf_timer_init(__TIME, ...)                  VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_init)                 ((__vsf_timer_t *)(__TIME), ##__VA_ARGS__)
#   define vsf_timer_fini(__TIME)                       VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_fini)                 ((__vsf_timer_t *)(__TIME))
#   define vsf_timer_enable(__TIME)                     VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_enable)               ((__vsf_timer_t *)(__TIME))
#   define vsf_timer_disable(__TIME)                    VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_disable)              ((__vsf_timer_t *)(__TIME))
#   define vsf_timer_status(__TIMER)                    VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_status)               ((__vsf_timer_t *)(__TIMER))
#   define vsf_timer_capability(__TIME)                 VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_capability)           ((__vsf_timer_t *)(__TIME))
#   define vsf_timer_irq_enable(__TIME, ...)            VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_irq_enable)           ((__vsf_timer_t *)(__TIME), ##__VA_ARGS__)
#   define vsf_timer_irq_disable(__TIME, ...)           VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_irq_disable)          ((__vsf_timer_t *)(__TIME), ##__VA_ARGS__)
#   define vsf_timer_set_period(__TIME, ...)            VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_set_period)           ((__vsf_timer_t *)(__TIME), ##__VA_ARGS__)
#   define vsf_timer_ctrl(__TIME, ...)                  VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_ctrl)                 ((__vsf_timer_t *)(__TIME), ##__VA_ARGS__)
#   define vsf_timer_channel_config(__TIME, ...)        VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_channel_config)       ((__vsf_timer_t *)(__TIME), ##__VA_ARGS__)
#   define vsf_timer_channel_start(__TIME, ...)         VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_channel_start)        ((__vsf_timer_t *)(__TIME), ##__VA_ARGS__)
#   define vsf_timer_channel_stop(__TIME, ...)          VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_channel_stop)         ((__vsf_timer_t *)(__TIME), ##__VA_ARGS__)
#   define vsf_timer_channel_request_start(__TIME, ...) VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_channel_request_start)((__vsf_timer_t *)(__TIME), ##__VA_ARGS__)
#   define vsf_timer_channel_request_stop(__TIME, ...)  VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_channel_request_stop) ((__vsf_timer_t *)(__TIME), ##__VA_ARGS__)
#   define vsf_timer_channel_ctrl(__TIME, ...)          VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_channel_ctrl)         ((__vsf_timer_t *)(__TIME), ##__VA_ARGS__)
#endif
/// @endcond

#ifdef __cplusplus
}
#endif

#endif  /*__VSF_TEMPLATE_TIMER_H__*/
