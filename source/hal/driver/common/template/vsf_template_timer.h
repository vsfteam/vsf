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

// multi-class support enabled by default for maximum availability.
#ifndef VSF_TIMER_CFG_MULTI_CLASS
#   define VSF_TIMER_CFG_MULTI_CLASS                        ENABLED
#endif

#if defined(VSF_HW_TIMER_COUNT) && !defined(VSF_HW_TIMER_MASK)
#   define VSF_HW_TIMER_MASK                                VSF_HAL_COUNT_TO_MASK(VSF_HW_TIMER_COUNT)
#endif

#if defined(VSF_HW_TIMER_MASK) && !defined(VSF_HW_TIMER_COUNT)
#   define VSF_HW_TIMER_COUNT                               VSF_HAL_MASK_TO_COUNT(VSF_HW_TIMER_MASK)
#endif

// application code can redefine it
#ifndef VSF_TIMER_CFG_PREFIX
#   if VSF_TIMER_CFG_MULTI_CLASS == ENABLED
#       define VSF_TIMER_CFG_PREFIX                         vsf
#   elif defined(VSF_HW_TIMER_COUNT) && (VSF_HW_TIMER_COUNT != 0)
#       define VSF_TIMER_CFG_PREFIX                         vsf_hw
#   else
#       define VSF_TIMER_CFG_PREFIX                         vsf
#   endif
#endif

#ifndef VSF_TIMER_CFG_FUNCTION_RENAME
#   define VSF_TIMER_CFG_FUNCTION_RENAME                    ENABLED
#endif

#ifndef VSF_TIMER_CFG_REIMPLEMENT_TYPE_IRQ_MASK
#   define VSF_TIMER_CFG_REIMPLEMENT_TYPE_IRQ_MASK          DISABLED
#endif

//! Redefine struct vsf_timer_cfg_t. The vsf_timer_isr_handler_t type also needs to
//! be redefined For compatibility, members should not be deleted when struct
//! @ref vsf_timer_cfg_t redefining.
#if VSF_TIMER_CFG_REIMPLEMENT_TYPE_CFG == DISABLED
#    define VSF_TIMER_CFG_REIMPLEMENT_TYPE_CFG              DISABLED
#endif

#if VSF_TIMER_CFG_REIMPLEMENT_TYPE_CHANNAL_MODE == DISABLED
#    define VSF_TIMER_CFG_REIMPLEMENT_TYPE_CHANNAL_MODE     DISABLED
#endif

#if VSF_TIMER_CFG_REIMPLEMENT_TYPE_CHANNAL_CFG == DISABLED
#    define VSF_TIMER_CFG_REIMPLEMENT_TYPE_CHANNAL_CFG     DISABLED
#endif

//! Redefine struct vsf_timer_capability_t.
//! For compatibility, members should not be deleted when struct @ref
//! vsf_timer_capability_t redefining.
#if VSF_TIMER_CFG_REIMPLEMENT_TYPE_CAPABILITY == DISABLED
#    define VSF_TIMER_CFG_REIMPLEMENT_TYPE_CAPABILITY   DISABLED
#endif

#ifndef VSF_TIMER_CFG_INHERIT_HAL_CAPABILITY
#   define VSF_TIMER_CFG_INHERIT_HAL_CAPABILITY          ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_TIMER_APIS(__prefix_name) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              timer, init,                  VSF_MCONNECT(__prefix_name, _timer_t) *timer_ptr, vsf_timer_cfg_t *cfg_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                   timer, fini,                  VSF_MCONNECT(__prefix_name, _timer_t) *timer_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,               timer, enable,                VSF_MCONNECT(__prefix_name, _timer_t) *timer_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,               timer, disable,               VSF_MCONNECT(__prefix_name, _timer_t) *timer_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                   timer, irq_enable,            VSF_MCONNECT(__prefix_name, _timer_t) *timer_ptr, vsf_timer_irq_mask_t irq_mask) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                   timer, irq_disable,           VSF_MCONNECT(__prefix_name, _timer_t) *timer_ptr, vsf_timer_irq_mask_t irq_mask) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_timer_capability_t, timer, capability,            VSF_MCONNECT(__prefix_name, _timer_t) *timer_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              timer, set_period,            VSF_MCONNECT(__prefix_name, _timer_t) *timer_ptr, uint32_t period) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              timer, channel_config,        VSF_MCONNECT(__prefix_name, _timer_t) *timer_ptr, uint8_t channel, vsf_timer_channel_cfg_t *channel_cfg_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              timer, channel_start,         VSF_MCONNECT(__prefix_name, _timer_t) *timer_ptr, uint8_t channel) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              timer, channel_stop,          VSF_MCONNECT(__prefix_name, _timer_t) *timer_ptr, uint8_t channel) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              timer, channel_request_start, VSF_MCONNECT(__prefix_name, _timer_t) *timer_ptr, uint8_t channel, vsf_timer_channel_request_t *request_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              timer, channel_request_stop,  VSF_MCONNECT(__prefix_name, _timer_t) *timer_ptr, uint8_t channel)

/*============================ TYPES =========================================*/

#if VSF_TIMER_CFG_REIMPLEMENT_TYPE_IRQ_MASK == DISABLED
typedef enum vsf_timer_irq_mask_t {
    VSF_TIMER_IRQ_MASK_OVERFLOW = (0x01 << 0),

    VSF_TIMER_IRQ_MASK_CHANNEL_0_INPUT_CAPTURE = (0x01 << 1),
    VSF_TIMER_IRQ_MASK_CHANNEL_0_OUTPUT_COMPARE = (0x01 << 2),
    /*
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
typedef struct vsf_timer_cfg_t {
    // counting period
    uint32_t period;
    union {
        uint32_t freq;
        uint32_t min_freq;
    };
    vsf_timer_isr_t isr;
} vsf_timer_cfg_t;
#endif


#if VSF_TIMER_CFG_REIMPLEMENT_TYPE_CHANNAL_MODE == DISABLED
typedef enum vsf_timer_channel_mode_t {
    // channel mode : different timer channel may support different mode
    // Base Mode : The timer channel will be used as a base timer
    VSF_TIMER_CHANNEL_MODE_BASE             = (0x00 << 0),
    // PWM(Pulse Width Modulation) Mode : The timer channel will be used as a
    // PWM output, all channels's period should be the same
    VSF_TIMER_CHANNEL_MODE_PWM = (0x01 << 0),
    // Output Compare Mode: The timer channel will be used as a output compare,
    // different channels can have different period (change pin level in
    // interrupt)
    VSF_TIMER_CHANNEL_MODE_OUTPUT_COMPARE = (0x02 << 0),
    // Input Capture Mode : The timer channel will be used as a input capture
    VSF_TIMER_CHANNEL_MODE_INPUT_CAPTURE    = (0x03 << 0),
    // Encoder Mode : The timer channel will be used as a encoder
    VSF_TIMER_CHANNEL_MODE_ENCODER          = (0x04 << 0),
    /*
    // Some timer may support more channel mode like one pulse or hall encoder etc.
    VSF_TIMER_CHANNEL_MODE_ONE_PULSE        = (0x05 << 0),
    #define VSF_TIMER_CHANNEL_MODE_ONE_PULSE  VSF_TIMER_CHANNEL_MODE_ONE_PULSE
    VSF_TIMER_CHANNEL_MODE_HALL_ENCODER     = (0x06 << 0),
    #define VSF_TIMER_CHANNEL_MODE_HALL_ENCODER  VSF_TIMER_CHANNEL_MODE_HALL_ENCODER
    */

    /********************************************************************************/
    // The following mode only valid for VSF_TIMER_CHANNEL_MODE_BASE
    // Counter Direction
    // Counter Up : The timer will count up
    VSF_TIMER_BASE_COUNTER_UP               = (0x00 << 4),
    // Counter Down : The timer will count down
    VSF_TIMER_BASE_COUNTER_DOWN             = (0x01 << 4),
    /*
    // Some timer may support more counter mode like center-aligned etc.
    VSF_TIMER_BASE_COUNTER_CENTER_ALIGNED1  = (0x02 << 4),
    #define VSF_TIMER_BASE_COUNTER_CENTER_ALIGNED1  VSF_TIMER_BASE_COUNTER_CENTER_ALIGNED1
    VSF_TIMER_BASE_COUNTER_CENTER_ALIGNED2  = (0x03 << 4),
    #define VSF_TIMER_BASE_COUNTER_CENTER_ALIGNED2  VSF_TIMER_BASE_COUNTER_CENTER_ALIGNED2
    VSF_TIMER_BASE_COUNTER_CENTER_ALIGNED3  = (0x04 << 4),
    #define VSF_TIMER_BASE_COUNTER_CENTER_ALIGNED3  VSF_TIMER_BASE_COUNTER_CENTER_ALIGNED3
    */

    // Counter Mode
    // Oneshot : The timer will stop after the first overflow
    VSF_TIMER_BASE_ONESHOT                    = (0x00 << 5),
    // Continues : The timer will continue to count after overflow
    VSF_TIMER_BASE_CONTINUES                  = (0x01 << 5),


    /********************************************************************************/
    // The following mode only valid for VSF_TIMER_CHANNEL_MODE_OUTPUT_COMPARE
    // Output Compare Mode
    VSF_TIMER_CHANNEL_OUTPUT_COMPARE_ACTIVE             = (0x00 << 4),
    VSF_TIMER_CHANNEL_OUTPUT_COMPARE_INACTIVE           = (0x01 << 4),
    VSF_TIMER_CHANNEL_OUTPUT_COMPARE_TOGGLE             = (0x02 << 4),
    /*
    // Some timer may support more output compare mode like PWM etc.
    VSF_TIMER_CHANNEL_OUTPUT_COMPARE_PWM1               = (0x03 << 4),
    #define VSF_TIMER_CHANNEL_OUTPUT_COMPARE_PWM1  VSF_TIMER_CHANNEL_OUTPUT_COMPARE_PWM1
    VSF_TIMER_CHANNEL_OUTPUT_COMPARE_PWM2               = (0x04 << 4),
    #define VSF_TIMER_CHANNEL_OUTPUT_COMPARE_PWM2  VSF_TIMER_CHANNEL_OUTPUT_COMPARE_PWM2
    VSF_TIMER_CHANNEL_OUTPUT_COMPARE_FORCED_ACTIVE      = (0x05 << 4),
    #define VSF_TIMER_CHANNEL_OUTPUT_COMPARE_FORCED_ACTIVE  VSF_TIMER_CHANNEL_OUTPUT_COMPARE_FORCED_ACTIVE
    VSF_TIMER_CHANNEL_OUTPUT_COMPARE_FORCED_INACTIVE    = (0x06 << 4),
    #define VSF_TIMER_CHANNEL_OUTPUT_COMPARE_FORCED_INACTIVE  VSF_TIMER_CHANNEL_OUTPUT_COMPARE_FORCED_INACTIVE
    */

    /*
    // Output Compare Polarity and N Polarity
    VSF_TIMER_CHANNEL_OUTPUT_COMPARE_POLARITY_HIGH      = (0x00 << 8),
    #define VSF_TIMER_CHANNEL_OUTPUT_COMPARE_POLARITY_HIGH  VSF_TIMER_CHANNEL_OUTPUT_COMPARE_POLARITY_HIGH
    VSF_TIMER_CHANNEL_OUTPUT_COMPARE_POLARITY_LOW       = (0x01 << 8),
    #define VSF_TIMER_CHANNEL_OUTPUT_COMPARE_POLARITY_LOW   VSF_TIMER_CHANNEL_OUTPUT_COMPARE_POLARITY_LOW
    VSF_TIMER_CHANNEL_OUTPUT_COMPARE_N_POLARITY_HIGH    = (0x00 << 8),
    #define VSF_TIMER_CHANNEL_OUTPUT_COMPARE_N_POLARITY_HIGH  VSF_TIMER_CHANNEL_OUTPUT_COMPARE_N_POLARITY_HIGH
    VSF_TIMER_CHANNEL_OUTPUT_COMPARE_N_POLARITY_LOW     = (0x01 << 8),
    #define VSF_TIMER_CHANNEL_OUTPUT_COMPARE_N_POLARITY_LOW   VSF_TIMER_CHANNEL_OUTPUT_COMPARE_N_POLARITY_LOW
    */

    /********************************************************************************/
    // The following mode only valid for VSF_TIMER_CHANNEL_MODE_PWM
    // PWM Mode
    VSF_TIMER_PWM_OUTPUT_POLARITY_HIGH                  = (0x00 << 4),
    VSF_TIMER_PWM_OUTPUT_POLARITY_LOW                   = (0x01 << 4),


    /********************************************************************************/
    // The following mode only valid for VSF_TIMER_CHANNEL_MODE_INPUT_CAPTURE
    VSF_TIMER_CHANNEL_INPUT_CAPTURE_POLARITY_RISING     = (0x00 << 4),
    VSF_TIMER_CHANNEL_INPUT_CAPTURE_POLARITY_FALLING    = (0x01 << 4),
    VSF_TIMER_CHANNEL_INPUT_CAPTURE_POLARITY_BOTH       = (0x02 << 4),

    /*
    VSF_TIMER_CHANNEL_INPUT_CAPTURE_FILTER_MASK = (0x0F << 4),
    VSF_TIMER_CHANNEL_INPUT_CAPTURE_FILTER_OFFSET = 4,
    */

    /********************************************************************************/
    // The following mode only valid for VSF_TIMER_CHANNEL_MODE_ENCODER
    // Encoder Mode
    VSF_TIMER_CHANNEL_ENCODER_CHANNEL_A_POLARITY_RISING = (0x00 << 3),
    VSF_TIMER_CHANNEL_ENCODER_CHANNEL_A_POLARITY_FALLING = (0x01 << 3),
    VSF_TIMER_CHANNEL_ENCODER_CHANNEL_A_POLARITY_BOTH = (0x02 << 3),
} vsf_timer_channel_mode_t;
#endif

#if VSF_TIMER_CFG_REIMPLEMENT_TYPE_CHANNAL_CFG == DISABLED
typedef struct vsf_timer_channel_cfg_t {
    vsf_timer_channel_mode_t mode;
    uint32_t                 pulse;
} vsf_timer_channel_cfg_t;
#endif

#if VSF_TIMER_CFG_REIMPLEMENT_TYPE_CHANNAL_CFG == DISABLED
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
            uint32_t *channal_a_buffer;
            uint32_t *channal_b_buffer;
        };
    };
} vsf_timer_channel_request_t;
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
#undef __VSF_HAL_TEMPLATE_API
#define __VSF_HAL_TEMPLATE_API VSF_HAL_TEMPLATE_API_FP

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
 \~english
 @brief initialize a timer instance.
 @param[in] timer_ptr: a pointer to structure @ref vsf_timer_t
 @param[in] cfg_ptr: a pointer to structure @ref vsf_timer_cfg_t
 @return vsf_err_t: VSF_ERR_NONE if timer was initialized, or a negative error code

 @note It is not necessary to call vsf_timer_fini() to deinitialization.
       vsf_timer_init() should be called before any other timer API except vsf_timer_capability().

 \~chinese
 @brief 初始化一个 timer 实例
 @param[in] timer_ptr: 结构体 vsf_timer_t 的指针，参考 @ref vsf_timer_t
 @param[in] cfg_ptr: 结构体 vsf_timer_cfg_t 的指针，参考 @ref vsf_timer_cfg_t
 @return vsf_err_t: 如果 timer 初始化成功返回 VSF_ERR_NONE , 失败返回负数。

 @note 失败后不需要调用 vsf_timer_fini() 反初始化。
       vsf_timer_init() 应该在除 vsf_timer_capability() 之外的其他 timer API 之前调用。
 */
extern vsf_err_t vsf_timer_init(vsf_timer_t *timer_ptr, vsf_timer_cfg_t *cfg_ptr);

/**
 \~english
 @brief finalize a timer instance.
 @param[in] timer_ptr: a pointer to structure @ref vsf_timer_t
 @return none

 \~chinese
 @brief 终止一个 timer 实例
 @param[in] timer_ptr: 结构体 vsf_timer_t 的指针，参考 @ref vsf_timer_t
 @return 无。
 */
extern void vsf_timer_fini(vsf_timer_t *timer_ptr);

/**
 \~english
 @brief enable a timer instance.
 @param[in] timer_ptr: a pointer to structure @ref vsf_timer_t
 @return fsm_rt_t: return FSM_RT_CPL if timer was enabled, return FSM_RT_ONGOING if timer is enabling

 \~chinese
 @brief 使能一个 timer 实例
 @param[in] timer_ptr: 结构体 vsf_timer_t 的指针，参考 @ref vsf_timer_t
 @return fsm_rt_t: 返回 FSM_RT_CPL 如果 timer 实例被使能，返回 FSM_RT_ONGOING 如果 timer 实例正在使能
 */
extern fsm_rt_t vsf_timer_enable(vsf_timer_t *timer_ptr);

/**
 \~english
 @brief disable a timer instance.
 @param[in] timer_ptr: a pointer to structure @ref vsf_timer_t
 @return fsm_rt_t: return FSM_RT_CPL if timer was disabled, return FSM_RT_ONGOING if timer is disabling

 \~chinese
 @brief 禁能一个 timer 实例
 @param[in] timer_ptr: 结构体 vsf_timer_t 的指针，参考 @ref vsf_timer_t
 @return fsm_rt_t: 返回 FSM_RT_CPL 如果 timer 实例被禁能，返回 FSM_RT_ONGOING 如果 timer 实例正在禁能
 */
extern fsm_rt_t vsf_timer_disable(vsf_timer_t *timer_ptr);

/**
 \~english
 @brief enable interrupt masks of timer instance.
 @param[in] timer_ptr: a pointer to structure @ref vsf_timer_t
 @param[in] irq_mask: one or more value of enum @ref vsf_timer_irq_mask_t
 @return none.
 @note All pending interrupts should be cleared before interrupts are enabled.

 \~chinese
 @brief 使能 timer 实例的中断
 @param[in] timer_ptr: 结构体 vsf_timer_t 的指针，参考 @ref vsf_timer_t
 @param[in] irq_mask: 一个或者多个枚举 vsf_timer_irq_mask_t 的值的按位或，@ref vsf_timer_irq_mask_t
 @return 无。
 @note 在中断使能之前，应该清除所有悬挂的中断。
 */
extern void vsf_timer_irq_enable(vsf_timer_t *timer_ptr, vsf_timer_irq_mask_t irq_mask);

/**
 \~english
 @brief disable interrupt masks of timer instance.
 @param[in] timer_ptr: a pointer to structure @ref vsf_timer_t
 @param[in] irq_mask: one or more value of enum vsf_timer_irq_mask_t, @ref vsf_timer_irq_mask_t
 @return none.

 \~chinese
 @brief 禁能 timer 实例的中断
 @param[in] timer_ptr: 结构体 vsf_timer_t 的指针，参考 @ref vsf_timer_t
 @param[in] irq_mask: 一个或者多个枚举 vsf_timer_irq_mask_t 的值的按位或，@ref vsf_timer_irq_mask_t
 @return 无。
 */
extern void vsf_timer_irq_disable(vsf_timer_t *timer_ptr, vsf_timer_irq_mask_t irq_mask);

/**
 \~english
 @brief get the capability of timer instance.
 @param[in] timer_ptr: a pointer to structure @ref vsf_timer_t
 @return vsf_timer_capability_t: return all capability of current timer @ref vsf_timer_capability_t

 \~chinese
 @brief 获取 timer 实例的能力
 @param[in] timer_ptr: 结构体 vsf_timer_t 的指针，参考 @ref vsf_timer_t
 @return vsf_timer_capability_t: 返回当前 timer 的所有能力 @ref vsf_timer_capability_t
 */
extern vsf_timer_capability_t vsf_timer_capability(vsf_timer_t *timer_ptr);

/**
 \~english
 @brief timer set period (maximum count), used for change the period of timer dynamically
 @param[in] timer_ptr: a pointer to structure @ref vsf_timer_t
 @param[in] period: timer period width (in clock counter)
 @return vsf_err_t: VSF_ERR_NONE if the timer set period was successfully, or a negative error code

 \~chinese
 @brief timer 设置一个定时器的周期（最大计数值）, 用于动态改变定时器的周期
 @param[in] timer_ptr: 结构体 vsf_timer_t 的指针，参考 @ref vsf_timer_t
 @param[in] period: timer 周期宽度 (时钟计数)
 @return vsf_err_t: 如果 timer 设置周期成功成功返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vsf_timer_set_period(vsf_timer_t *timer_ptr, uint32_t period);

/**
 \~english
 @brief timer set the period width and pulse width for a channel
 @param[in] timer_ptr: a pointer to structure @ref vsf_timer_t
 @param[in] channel: timer channel
 @param[in] channel_cfg_ptr: a pointer to structure @ref vsf_timer_channel_cfg_t
 @return vsf_err_t: VSF_ERR_NONE if the timer channel set was successfully, or a negative error code

 \~chinese
 @brief timer 通道 设置一个通道的周期和宽度
 @param[in] timer_ptr: 结构体 vsf_timer_t 的指针，参考 @ref vsf_timer_t
 @param[in] channel: timer 通道
 @param[in] channel_cfg_ptr: a pointer to structure @ref vsf_timer_channel_cfg_t
 @return vsf_err_t: 如果 timer 通道设置成功成功返回 VSF_ERR_NONE, 否则返回负数。
 */
extern vsf_err_t vsf_timer_channel_config(vsf_timer_t             *timer_ptr,
                                          uint8_t                 channel,
                                          vsf_timer_channel_cfg_t *channel_cfg_ptr);

/**
 \~english
 @brief start a timer channel
 @param[in] timer_ptr: a pointer to structure @ref vsf_timer_t
 @param[in] channel: timer channel
 @return vsf_err_t: VSF_ERR_NONE if the timer channel start was successfully, or a negative error code

 \~chinese
 @brief 开始一个 timer 通道
 @param[in] timer_ptr: 结构体 vsf_timer_t 的指针，参考 @ref vsf_timer_t
 @param[in] channel: timer 通道
 @return vsf_err_t: 如果 timer 通道开始成功返回 VSF_ERR_NONE, 否则返回负数。
 */
extern vsf_err_t vsf_timer_channel_start(vsf_timer_t *timer_ptr, uint8_t channel);

/**
 \~english
 @brief stop a timer channel
 @param[in] timer_ptr: a pointer to structure @ref vsf_timer_t
 @param[in] channel: timer channel
 @return vsf_err_t: VSF_ERR_NONE if the timer channel stop was successfully, or a negative error code

 \~chinese
 @brief 停止一个 timer 通道
 @param[in] timer_ptr: 结构体 vsf_timer_t 的指针，参考 @ref vsf_timer_t
 @param[in] channel: timer 通道
 @return vsf_err_t: 如果 timer 通道停止成功返回 VSF_ERR_NONE, 否则返回负数。
 */
extern vsf_err_t vsf_timer_channel_stop(vsf_timer_t *timer_ptr, uint8_t channel);

/**
 \~english
 @brief start a timer channel request (usually based on DMA)
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
            For VSF_TIMER_CHANNEL_MODE_ENCODER mode, channal_a_buffer and channal_b_buffer in structure
            @ref vsf_timer_channel_request_t is array of encoder value for channel A and channel B.
 @return vsf_err_t: VSF_ERR_NONE if the timer channel start was successfully, or a negative error code

 \~chinese
 @brief 开始一个 timer 通道的请求 (通常基于 DMA)
 @param[in] timer_ptr: 结构体 vsf_timer_t 的指针，参考 @ref vsf_timer_t
 @param[in] channel: timer 通道
 @param[in] request_ptr: 结构体 vsf_timer_channel_request_t 的指针，参考 @ref vsf_timer_channel_request_t
            对于 VSF_TIMER_CHANNEL_MODE_BASE 模式, @ref vsf_timer_channel_request_t 中的 period_buffer 是每次溢出后要设置的周期值数组。
            对于 VSF_TIMER_CHANNEL_MODE_PWM 模式, @ref vsf_timer_channel_request_t 中的 pwm_buffer 是每次溢出后要设置的脉冲值数组。
            对于 VSF_TIMER_CHANNEL_MODE_OUTPUT_COMPARE 模式, @ref vsf_timer_channel_request_t 中的 pulse_buffer 是每次溢出后要设置的脉冲值数组。
            对于 VSF_TIMER_CHANNEL_MODE_INPUT_CAPTURE 模式, @ref vsf_timer_channel_request_t 中的 input_capture_buffer 是输入捕获值数组。
            对于 VSF_TIMER_CHANNEL_MODE_ENCODER 模式, @ref vsf_timer_channel_request_t 中的 channal_a_buffer 和 channal_b_buffer 是通道 A 和通道 B 的编码器值数组。
 @return vsf_err_t: 如果 timer 通道开始成功返回 VSF_ERR_NONE, 否则返回负数。
 */
extern vsf_err_t vsf_timer_channel_request_start(vsf_timer_t *timer_ptr, uint8_t channel, vsf_timer_channel_request_t *request_ptr);

/**
 \~english
 @brief stop a timer channel request.
 @param[in] timer_ptr: a pointer to structure @ref vsf_timer_t
 @param[in] channel: timer channel
 @return vsf_err_t: VSF_ERR_NONE if the timer channel stop was successfully, or a negative error code

 \~chinese
 @brief 停止一个 timer 通道的请求
 @param[in] timer_ptr: 结构体 vsf_timer_t 的指针，参考 @ref vsf_timer_t
 @param[in] channel: timer 通道
 @return vsf_err_t: 如果 timer 通道停止成功返回 VSF_ERR_NONE, 否则返回负数。
 */
extern vsf_err_t vsf_timer_channel_request_stop(vsf_timer_t *timer_ptr, uint8_t channel);

/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_TIMER_CFG_FUNCTION_RENAME == ENABLED
#   define __vsf_timer_t                                VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_t)
#   define vsf_timer_init(__TIME, ...)                  VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_init)                 ((__vsf_timer_t *)(__TIME), ##__VA_ARGS__)
#   define vsf_timer_fini(__TIME, ...)                  VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_fini)                 ((__vsf_timer_t *)(__TIME))
#   define vsf_timer_enable(__TIME)                     VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_enable)               ((__vsf_timer_t *)(__TIME))
#   define vsf_timer_disable(__TIME)                    VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_disable)              ((__vsf_timer_t *)(__TIME))
#   define vsf_timer_capability(__TIME)                 VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_capability)           ((__vsf_timer_t *)(__TIME))
#   define vsf_timer_irq_enable(__TIME, ...)            VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_irq_enable)           ((__vsf_timer_t *)(__TIME), ##__VA_ARGS__)
#   define vsf_timer_irq_disable(__TIME, ...)           VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_irq_disable)          ((__vsf_timer_t *)(__TIME), ##__VA_ARGS__)
#   define vsf_timer_set_period(__TIME, ...)            VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_set_period)           ((__vsf_timer_t *)(__TIME), ##__VA_ARGS__)
#   define vsf_timer_channel_config(__TIME, ...)        VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_channel_config)       ((__vsf_timer_t *)(__TIME), ##__VA_ARGS__)
#   define vsf_timer_channel_start(__TIME, ...)         VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_channel_start)        ((__vsf_timer_t *)(__TIME), ##__VA_ARGS__)
#   define vsf_timer_channel_stop(__TIME, ...)          VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_channel_stop)         ((__vsf_timer_t *)(__TIME), ##__VA_ARGS__)
#   define vsf_timer_channel_request_start(__TIME, ...) VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_channel_request_start)((__vsf_timer_t *)(__TIME), ##__VA_ARGS__)
#   define vsf_timer_channel_request_stop(__TIME, ...)  VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_channel_request_stop) ((__vsf_timer_t *)(__TIME), ##__VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif  /*__VSF_TEMPLATE_TIMER_H__*/
