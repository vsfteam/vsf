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

#ifndef __VSF_TEMPLATE_GPIO_H__
#define __VSF_TEMPLATE_GPIO_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_template_hal_driver.h"
#include "hal/arch/vsf_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

// multi-class support enabled by default for maximum availability.
#ifndef VSF_GPIO_CFG_MULTI_CLASS
#   define VSF_GPIO_CFG_MULTI_CLASS                 ENABLED
#endif

#if defined(VSF_HW_GPIO_PORT_COUNT) && !defined(VSF_HW_GPIO_PORT_MASK)
#   define VSF_HW_GPIO_PORT_MASK                    VSF_HAL_COUNT_TO_MASK(VSF_HW_GPIO_PORT_COUNT)
#endif

#if defined(VSF_HW_GPIO_PORT_MASK) && !defined(VSF_HW_GPIO_PORT_COUNT)
#   define VSF_HW_GPIO_PORT_COUNT                   VSF_HAL_MASK_TO_COUNT(VSF_HW_GPIO_PORT_MASK)
#endif

#if defined(VSF_HW_GPIO_PIN_COUNT) && !defined(VSF_HW_GPIO_PIN_MASK)
#   define VSF_HW_GPIO_PIN_MASK                    VSF_HAL_COUNT_TO_MASK(VSF_HW_GPIO_PIN_COUNT)
#endif

#if defined(VSF_HW_GPIO_PIN_MASK) && !defined(VSF_HW_GPIO_PIN_COUNT)
#   define VSF_HW_GPIO_PIN_COUNT                   VSF_HAL_MASK_TO_COUNT(VSF_HW_GPIO_PIN_MASK)
#endif

#ifdef VSF_HW_GPIO_PORT_COUNT
#	define VSF_HW_GPIO_COUNT                        VSF_HW_GPIO_PORT_COUNT
#endif

#ifdef VSF_HW_GPIO_PORT_MASK
#	define VSF_HW_GPIO_MASK                         VSF_HW_GPIO_PORT_MASK
#endif

/**
 \~english
 VSF_GPIO_CFG_PREFIX is used to set the actual API call when calling the vsf_gpio_*().
 For example, if you configure VSF_GPIO_CFG_PREFIX to be vsf_hw, then call
 vsf_gpio_set_output(), which is actually vsf_hw_gpio_set_output()

 If we want the call to vsf_gpio_set_output in xxxx.c to actually call vsf_example_gpio_set_output,
 then it can be configured in front of the .c:

 \~chinese
 VSF_GPIO_CFG_PREFIX 是用来配置调用 vsf_gpio_*() 的时候实际调用的API。
 例如，当 VSF_GPIO_CFG_PREFIX 配置成 vsf_hw 的时候，调用 vsf_gpio_set_output()
 实际调用的是 vsf_hw_gpio_set_output()

 如果我们想要 xxxx.c 里调用 vsf_gpio_set_output() 实际调用的是 vsf_example_gpio_set_output()，
 那么可以在 xxxx.c 源码前面配置：

 \~
 \code {.c}
    #undef  VSF_GPIO_CFG_PREFIX
    #define VSF_GPIO_CFG_PREFIX     vsf_example
 \endcode
 */
#ifndef VSF_GPIO_CFG_PREFIX
#   if VSF_GPIO_CFG_MULTI_CLASS == ENABLED
#       define VSF_GPIO_CFG_PREFIX                  vsf
#   elif defined(VSF_HW_GPIO_PIN_MASK) && (VSF_HW_GPIO_PIN_MASK != 0)
#       define VSF_GPIO_CFG_PREFIX                  vsf_hw
#   else
#       define VSF_GPIO_CFG_PREFIX                  vsf
#   endif
#endif

#ifndef VSF_GPIO_CFG_REIMPLEMENT_TYPE_MODE
#   define VSF_GPIO_CFG_REIMPLEMENT_TYPE_MODE       DISABLED
#endif

#ifndef VSF_GPIO_CFG_REIMPLEMENT_TYPE_EXT_MODE
#   define VSF_GPIO_CFG_REIMPLEMENT_TYPE_EXT_MODE   DISABLED
#endif

#ifndef VSF_GPIO_USE_IO_MODE_TYPE
#   define VSF_GPIO_USE_IO_MODE_TYPE                DISABLED
#endif

#ifndef VSF_GPIO_CFG_FUNCTION_RENAME
#   define VSF_GPIO_CFG_FUNCTION_RENAME             ENABLED
#endif

#ifndef VSF_GPIO_CFG_INHERT_HAL_CAPABILITY
#   define VSF_GPIO_CFG_INHERT_HAL_CAPABILITY       ENABLED
#endif

#ifndef vsf_gpio_pin_mask_t
#   if defined(VSF_HW_GPIO_PIN_MASK) && (VSF_HW_GPIO_PIN_MASK & 0xFFFFFFFF00000000)
#	    define vsf_gpio_pin_mask_t uint64_t
#   else
#	    define vsf_gpio_pin_mask_t uint32_t
#   endif
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_GPIO_APIS(__prefix_name)                                                                                                                                                                               \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_gpio_capability_t, gpio, capability,              VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr)                                                                    \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                  gpio, config_pin,              VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, vsf_gpio_pin_mask_t pin_mask, vsf_gpio_mode_t mode)                \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                  gpio, set_direction,           VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, vsf_gpio_pin_mask_t pin_mask, vsf_gpio_pin_mask_t direction_mask)  \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_gpio_pin_mask_t,   gpio, get_direction,           VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)                                      \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                  gpio, set_input,               VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)                                      \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                  gpio, set_output,              VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)                                      \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                  gpio, switch_direction,        VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)                                      \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_gpio_pin_mask_t,   gpio, read,                    VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr)                                                                    \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                  gpio, write,                   VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, vsf_gpio_pin_mask_t pin_mask, vsf_gpio_pin_mask_t value)           \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                  gpio, set,                     VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)                                      \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                  gpio, clear,                   VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)                                      \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                  gpio, toggle,                  VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)                                      \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                  gpio, output_and_set,          VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)                                      \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                  gpio, output_and_clear,        VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)                                      \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,             gpio, exti_config,             VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, vsf_gpio_pin_irq_cfg_t *cfg_ptr)                                   \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,             gpio, exti_irq_enable,         VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, vsf_gpio_pin_mask_t pin_mask, vsf_arch_prio_t prio)                \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,             gpio, exti_irq_disable,        VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)

/*============================ TYPES =========================================*/

#if VSF_GPIO_CFG_REIMPLEMENT_TYPE_MODE == DISABLED
typedef enum vsf_gpio_mode_t {
    VSF_GPIO_PULL_UP                    = (0 << 0),         //!< enable pull-up resistor
    VSF_GPIO_PULL_DOWN                  = (1 << 0),         //!< enable pull-down resistor
    VSF_GPIO_OPEN_DRAIN                 = (2 << 0),         //!< enable open-drain mode
    VSF_GPIO_ANALOG                     = (3 << 0),         //!< enable analog function

    VSF_GPIO_NORMAL_INPUT               = (0 << 3),         //!< normal input pin level
    VSF_GPIO_INVERT_INPUT               = (1 << 3),         //!< inverted input pin level
    VSF_GPIO_DISABLE_INPUT              = (1 << 4),         //!< disable input

    VSF_GPIO_FILTER_BYPASS              = (0 << 5),         //!< filter is bypassed
    VSF_GPIO_FILTER_2CLK                = (1 << 5),         //!< levels should keep 2 clks
    VSF_GPIO_FILTER_4CLK                = (2 << 5),         //!< levels should keep 4 clks
    VSF_GPIO_FILTER_8CLK                = (3 << 5),         //!< levels should keep 8 clks

    VSF_GPIO_FILTER_CLK_SRC0            = (0 << 7),         //!< select clock src 0 for filter
    VSF_GPIO_FILTER_CLK_SRC1            = (1 << 7),         //!< select clock src 1 for filter
    VSF_GPIO_FILTER_CLK_SRC2            = (2 << 7),         //!< select clock src 2 for filter
    VSF_GPIO_FILTER_CLK_SRC3            = (3 << 7),         //!< select clock src 3 for filter
    VSF_GPIO_FILTER_CLK_SRC4            = (4 << 7),         //!< select clock src 4 for filter
    VSF_GPIO_FILTER_CLK_SRC5            = (5 << 7),         //!< select clock src 5 for filter
    VSF_GPIO_FILTER_CLK_SRC6            = (6 << 7),         //!< select clock src 6 for filter
    VSF_GPIO_FILTER_CLK_SRC7            = (7 << 7),         //!< select clock src 7 for filter

    VSF_GPIO_HIGH_DRIVE_STRENGTH        = (1 << 10),        //!< enable high drive strength
    VSF_GPIO_HIGH_DRIVE_NO_STRENGTH     = (1 << 10),        //!< enable high drive strength

    VSF_GPIO_EXTI_DISABLED              = (0 << 11),
    VSF_GPIO_EXTI_ENABLED               = (1 << 11),
} vsf_gpio_mode_t;
#elif VSF_GPIO_USE_IO_MODE_TYPE == ENABLED
typedef enum vsf_gpio_mode_t {
    VSF_GPIO_PULL_UP                    = VSF_IO_PULL_UP,
    VSF_GPIO_PULL_DOWN                  = VSF_IO_PULL_DOWN,
    VSF_GPIO_OPEN_DRAIN                 = VSF_IO_OPEN_DRAIN,
    VSF_GPIO_ANALOG                     = VSF_IO_ANALOG,

    VSF_GPIO_NORMAL_INPUT               = VSF_IO_NORMAL_INPUT,
    VSF_GPIO_INVERT_INPUT               = VSF_IO_INVERT_INPUT,
    VSF_GPIO_DISABLE_INPUT              = VSF_IO_DISABLE_INPUT,

    VSF_GPIO_FILTER_BYPASS              = VSF_IO_FILTER_BYPASS,
    VSF_GPIO_FILTER_2CLK                = VSF_IO_FILTER_2CLK,
    VSF_GPIO_FILTER_4CLK                = VSF_IO_FILTER_4CLK,
    VSF_GPIO_FILTER_8CLK                = VSF_IO_FILTER_8CLK,

    VSF_GPIO_FILTER_CLK_SRC0            = VSF_IO_FILTER_CLK_SRC0,
    VSF_GPIO_FILTER_CLK_SRC1            = VSF_IO_FILTER_CLK_SRC1,
    VSF_GPIO_FILTER_CLK_SRC2            = VSF_IO_FILTER_CLK_SRC2,
    VSF_GPIO_FILTER_CLK_SRC3            = VSF_IO_FILTER_CLK_SRC3,
    VSF_GPIO_FILTER_CLK_SRC4            = VSF_IO_FILTER_CLK_SRC4,
    VSF_GPIO_FILTER_CLK_SRC5            = VSF_IO_FILTER_CLK_SRC5,
    VSF_GPIO_FILTER_CLK_SRC6            = VSF_IO_FILTER_CLK_SRC6,
    VSF_GPIO_FILTER_CLK_SRC7            = VSF_IO_FILTER_CLK_SRC7,

    VSF_GPIO_HIGH_DRIVE_STRENGTH        = VSF_IO_HIGH_DRIVE_STRENGTH,
    VSF_GPIO_HIGH_DRIVE_NO_STRENGTH     = VSF_IO_HIGH_DRIVE_NO_STRENGTH,

    VSF_GPIO_EXTI_DISABLED              = (0 << 11),
    VSF_GPIO_EXTI_ENABLED               = (1 << 11),
} vsf_gpio_mode_t;
#endif

enum {
    VSF_GPIO_OUTPUT_COUNT               = 3,
    VSF_GPIO_OUTPUT_MASK                = VSF_GPIO_PULL_UP
                                        | VSF_GPIO_PULL_DOWN
                                        | VSF_GPIO_OPEN_DRAIN,

    VSF_GPIO_INPUT_COUNT                = 3,
    VSF_GPIO_INPUT_MASK                 = VSF_GPIO_NORMAL_INPUT
                                        | VSF_GPIO_INVERT_INPUT
                                        | VSF_GPIO_DISABLE_INPUT,

    VSF_GPIO_FILTER_COUNT               = 4,
    VSF_GPIO_FILTER_MASK                = VSF_GPIO_FILTER_BYPASS
                                        | VSF_GPIO_FILTER_2CLK
                                        | VSF_GPIO_FILTER_4CLK
                                        | VSF_GPIO_FILTER_8CLK,

    VSF_GPIO_FILTER_CLK_COUNT           = 8,
    VSF_GPIO_FILTER_CLK_MASK            = VSF_GPIO_FILTER_CLK_SRC0
                                        | VSF_GPIO_FILTER_CLK_SRC1
                                        | VSF_GPIO_FILTER_CLK_SRC2
                                        | VSF_GPIO_FILTER_CLK_SRC3
                                        | VSF_GPIO_FILTER_CLK_SRC4
                                        | VSF_GPIO_FILTER_CLK_SRC5
                                        | VSF_GPIO_FILTER_CLK_SRC6
                                        | VSF_GPIO_FILTER_CLK_SRC7,

    // Independent switching options
    // VSF_GPIO_HIGH_DRIVE_STRENGTH
    VSF_GPIO_HIGH_DRIVE_STRENGTH_COUNT  = 2,
    VSF_GPIO_HIGH_DRIVE_STRENGTH_MASK   = VSF_GPIO_HIGH_DRIVE_STRENGTH
                                        | VSF_GPIO_HIGH_DRIVE_NO_STRENGTH,

    VSF_GPIO_EXTI_COUNT                 = 2,
    VSF_GPIO_EXTI_MASK                  = VSF_GPIO_EXTI_ENABLED
                                        | VSF_GPIO_EXTI_DISABLED,

    VSF_GPIO_MODE_MASK_COUNT            = 5,
    VSF_GPIO_MODE_ALL_BITS_MASK         = VSF_GPIO_PULL_UP
                                        | VSF_GPIO_OPEN_DRAIN
                                        | VSF_GPIO_DISABLE_INPUT
                                        | VSF_GPIO_INVERT_INPUT
                                        | VSF_GPIO_FILTER_MASK
                                        | VSF_GPIO_FILTER_CLK_MASK
                                        | VSF_GPIO_HIGH_DRIVE_STRENGTH
                                        | VSF_GPIO_EXTI_MASK,
};

#if VSF_GPIO_CFG_REIMPLEMENT_TYPE_EXT_MODE == DISABLED
typedef enum vsf_gpio_interrupt_mode_t {
    VSF_GPIO_INT_MODE_NONE              = (0x0 << 0),
    VSF_GPIO_INT_MODE_LOW_LEVEL         = (0x1 << 0),
    VSF_GPIO_INT_MODE_HIGH_LEVEL        = (0x2 << 0),
    VSF_GPIO_INT_MODE_RISING            = (0x3 << 0),
    VSF_GPIO_INT_MODE_FALLING           = (0x4 << 0),
    VSF_GPIO_INT_MODE_RISING_FALLING    = (0x5 << 0),
} vsf_gpio_interrupt_mode_t;
#endif

enum {
    VSF_GPIO_INT_MODE_COUNT             = 6,
    VSF_GPIO_INT_MODE_MASK              = VSF_GPIO_INT_MODE_NONE
                                        | VSF_GPIO_INT_MODE_LOW_LEVEL
                                        | VSF_GPIO_INT_MODE_HIGH_LEVEL
                                        | VSF_GPIO_INT_MODE_RISING
                                        | VSF_GPIO_INT_MODE_FALLING
                                        | VSF_GPIO_INT_MODE_RISING_FALLING,

    VSF_GPIO_INT_MODE_MASK_COUNT        = 1,
    VSF_GPIO_INT_MODE_ALL_BITS_MASK     = VSF_GPIO_INT_MODE_MASK,
};

typedef struct vsf_gpio_t vsf_gpio_t;

typedef void vsf_gpio_isr_handler_t(void *target_ptr, vsf_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask);

typedef struct vsf_gpio_isr_t {
    vsf_gpio_isr_handler_t              *handler_fn;
    void                                *target_ptr;
} vsf_gpio_isr_t;

//! gpio channel configuration
typedef struct vsf_gpio_pin_irq_cfg_t {
    vsf_gpio_pin_mask_t                 pin_mask;
    vsf_gpio_interrupt_mode_t           mode;
    vsf_gpio_isr_t                      isr;
} vsf_gpio_pin_irq_cfg_t;

typedef struct vsf_gpio_capability_t {
#if VSF_GPIO_CFG_INHERT_HAL_CAPABILITY == ENABLED
    inherit(vsf_peripheral_capability_t)
#endif
    //! Asynchronous GPIO
    //!  they are only guaranteed to be sequential when operating the current port consecutively.
    //!
    //!  When they are used together with other ports, they are not guaranteed to
    //!  be sequential for the operation of the pins of different ports.
    uint8_t is_async                     : 1;

    //! If not supported, then vsf_gpio_config_pin() cannot be used
    uint8_t support_config_pin           : 1;

    //! To avoid bumps when converting from input to output
    uint8_t support_output_and_set       : 1;
    uint8_t support_output_and_clear     : 1;

    //! supports external interrupts
    uint8_t support_interrupt   : 1;

    //! total number of pins of hardware, even if some pins cannot be configured as GPIO.
    //! if some of the pins cannot be used as GPIO,the corresponding bit
    //! in pin_mask is 0, otherwise is 1.
    uint8_t pin_count;

    //! available pin mask
    //! may be:
    //!  0x000000FF (8  pins),
    //!  0x0000FFFF (16  pins),
    //!  0xFFFFFFFF (32 pins),
    //!  0xFFFFFFFE (32 pins, but pin0 cannot be used as GPIO)
    vsf_gpio_pin_mask_t pin_mask;
} vsf_gpio_capability_t;

typedef struct vsf_gpio_t vsf_gpio_t;

typedef struct vsf_gpio_op_t {
#undef __VSF_HAL_TEMPLATE_API
#define __VSF_HAL_TEMPLATE_API VSF_HAL_TEMPLATE_API_FP

    VSF_GPIO_APIS(vsf)
} vsf_gpio_op_t;

#if VSF_GPIO_CFG_MULTI_CLASS == ENABLED
/**
 \~english
 When enabling multiple classes, the first member of all gpio implementations
 needs to be of type const "vsf_gpio_op_t * op"

 \~chinese
 当使能多类的时候，所有的具体gpio实现的第一个成员都需要是const vsf_gpio_op_t *类型。
 */
struct vsf_gpio_t  {
    const vsf_gpio_op_t * op;
};
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ PROTOTYPES ====================================*/

/**
 \~english
 @brief Configure one or more pins of the gpio instance
 @param[in] gpio_ptr: a pointer to structure @ref vsf_gpio_t
 @param[in] pin_mask: pin mask, each pin corresponds to one bit, the value of
            this bit 1 means configuration is required
 @param[in] mode: enumeration type @ref vsf_gpio_mode_t

 \~chinese
 @brief 配置 gpio 实例的一个或者多个引脚
 @param[in] gpio_ptr: 结构体 vsf_gpio_t 的指针，参考 @ref vsf_gpio_t
 @param[in] pin_mask: 引脚掩码，每一个引脚对应一个位，该bit的值位1表示需要配置
 @param[in] mode: 枚举类型 @ref vsf_gpio_mode_t
 */
extern void vsf_gpio_config_pin(vsf_gpio_t *gpio_ptr,
                                vsf_gpio_pin_mask_t pin_mask,
                                vsf_gpio_mode_t mode);

/**
 \~english
 @brief Set the orientation of one or more pins of the gpio instance
 @param[in] gpio_ptr: a pointer to structure @ref vsf_gpio_t
 @param[in] direction_mask: direction mask, 1 for output, 0 for input
 @param[in] pin_mask: pin mask, each pin corresponds to one bit, the value of
            this bit 1 means configuration is required

 \~chinese
 @brief 设置 gpio 实例的一个或者多个引脚的方向
 @param[in] gpio_ptr: 结构体 vsf_gpio_t 的指针，参考 @ref vsf_gpio_t
 @param[in] pin_mask: 引脚掩码，每一个引脚对应一个位，该bit的值位1表示需要配置
 @param[in] direction_mask: 方向掩码，1 表示输出，0 表示输入
 */
extern void vsf_gpio_set_direction(vsf_gpio_t *gpio_ptr,
                                   vsf_gpio_pin_mask_t pin_mask,
                                   vsf_gpio_pin_mask_t direction_mask);

/**
 \~english
 @brief Get the orientation of one or more pins of the gpio instance
 @param[in] gpio_ptr: a pointer to structure @ref vsf_gpio_t
 @param[in] pin_mask: pin mask, each pin corresponds to one bit, the value of
            this bit 1 means configuration is required
 @return vsf_gpio_pin_mask_t: The value of all pin_mask directions, 1 for output, 0 for input

 \~chinese
 @brief 获取 gpio 实例的一个或者多个引脚的方向
 @param[in] gpio_ptr: 结构体 vsf_gpio_t 的指针，参考 @ref vsf_gpio_t
 @param[in] pin_mask: 引脚掩码，每一个引脚对应一个位，该bit的值位1表示需要获取
 @return vsf_gpio_pin_mask_t: 所有pin_mask的方向的值，1 表示输出，0 表示输入
 */
extern vsf_gpio_pin_mask_t vsf_gpio_get_direction(vsf_gpio_t *gpio_ptr,
                                                  vsf_gpio_pin_mask_t pin_mask);

/**
 \~english
 @brief Set the direction of one or more pins of the gpio instance to input
 @param[in] gpio_ptr: a pointer to structure @ref vsf_gpio_t
 @param[in] Pin mask, each pin corresponds to one bit, the value of the bit 1
            indicates the need to set to input

 \~chinese
 @brief 设置 gpio 实例的一个或者多个引脚的方向为输入
 @param[in] gpio_ptr: 结构体 vsf_gpio_t 的指针，参考 @ref vsf_gpio_t
 @param[in] pin_mask: 引脚掩码，每一个引脚对应一个位，该bit的值位1表示需要设置成输入
 */
extern void vsf_gpio_set_input(vsf_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask);

/**
 \~english
 @brief Set the direction of one or more pins of the gpio instance to output
 @param[in] gpio_ptr: a pointer to structure @ref vsf_gpio_t
 @param[in] Pin mask, each pin corresponds to one bit, the value of the bit 1
            indicates the need to set to output

 \~chinese
 @brief 设置 gpio 实例的一个或者多个引脚的方向为输出
 @param[in] gpio_ptr: 结构体 vsf_gpio_t 的指针，参考 @ref vsf_gpio_t
 @param[in] pin_mask: 引脚掩码，每一个引脚对应一个位，该bit的值位1表示需要设置成输出
 */
extern void vsf_gpio_set_output(vsf_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask);

/**
 \~english
 @brief Toggle the orientation of one or more pins of a gpio instance
 @param[in] gpio_ptr: a pointer to structure @ref vsf_gpio_t
 @param[in] Pin mask, each pin corresponds to one bit, the value of the bit 1
            indicates the need to set to output

 \~chinese
 @brief 反转 gpio 实例的一个或者多个引脚的方向
 @param[in] gpio_ptr: 结构体 vsf_gpio_t 的指针，参考 @ref vsf_gpio_t
 @param[in] pin_mask: 引脚掩码，每一个引脚对应一个位，该bit的值位1表示需要反转
 */
extern void vsf_gpio_switch_direction(vsf_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask);

/**
 \~english
 @brief Read the values of all pins of the gpio instance
 @param[in] gpio_ptr: a pointer to structure @ref vsf_gpio_t
 @return vsf_gpio_pin_mask_t: Value of all pins, 1 for output, 0 for input

 \~chinese
 @brief 读取 gpio 实例的所有引脚的值
 @param[in] gpio_ptr: 结构体 vsf_gpio_t 的指针，参考 @ref vsf_gpio_t
 @return vsf_gpio_pin_mask_t: 所有引脚的值，1 表示输出，0 表示输入
 */
extern vsf_gpio_pin_mask_t vsf_gpio_read(vsf_gpio_t *gpio_ptr);

/**
 \~english
 @brief set the value of one or more of the gpio instances
 @param[in] gpio_ptr: pointer to the structure vsf_gpio_t, refer to @ref vsf_gpio_t
 @param[in] value: value of the pin, each pin corresponds to a bit, 1 means high, 0 means high
 @param[in] pin_mask: pin mask, each pin corresponds to one bit, 1 means the bit
            needs to be written, 0 means the bit does not need to be updated

 \~chinese
 @brief 设置 gpio 实例的一个或者多个的值
 @param[in] gpio_ptr: 结构体 vsf_gpio_t 的指针，参考 @ref vsf_gpio_t
 @param[in] pin_mask: 引脚掩码，每一个引脚对应一个位，1表示该位需要写入，0表示该位不需要更新
 @param[in] value: 引脚的值，每一个引脚对应一个位，1 表示高电平，0 表示高电平
 */
extern void vsf_gpio_write(vsf_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask,
                           vsf_gpio_pin_mask_t value);

/**
 \~english
 @brief Set the value of one or more pins of the gpio instance to high
 @param[in] gpio_ptr: pointer to the structure vsf_gpio_t, refer to @ref vsf_gpio_t
 @param[in] pin_mask: pin mask, each pin corresponds to one bit, 1 means the bit
            needs to be written, 0 means the bit does not need to be updated

 \~chinese
 @brief 设置 gpio 实例的一个或者多个引脚的值为高电平
 @param[in] gpio_ptr: 结构体 vsf_gpio_t 的指针，参考 @ref vsf_gpio_t
 @param[in] pin_mask: 引脚掩码，每一个引脚对应一个位，1表示该位需要写入，0表示该位不需要更新
 */
extern void vsf_gpio_set(vsf_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask);

/**
 \~english
 @brief Set the value of one or more pins of the gpio instance to low
 @param[in] gpio_ptr: pointer to the structure vsf_gpio_t, refer to @ref vsf_gpio_t
 @param[in] pin_mask: pin mask, each pin corresponds to one bit, 1 means the bit
            needs to be written, 0 means the bit does not need to be updated

 \~chinese
 @brief 设置 gpio 实例的一个或者多个引脚的值为低电平
 @param[in] gpio_ptr: 结构体 vsf_gpio_t 的指针，参考 @ref vsf_gpio_t
 @param[in] pin_mask: 引脚掩码，每一个引脚对应一个位，1表示该位需要写入，0表示该位不需要更新
 */
extern void vsf_gpio_clear(vsf_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask);

/**
 \~english
 @brief Toggle the value of one or more pins of the gpio instance
 @param[in] gpio_ptr: pointer to the structure vsf_gpio_t, refer to @ref vsf_gpio_t
 @param[in] pin_mask: pin mask, each pin corresponds to one bit, 1 means the bit
            needs to be written, 0 means the bit does not need to be updated

 \~chinese
 @brief 反转 gpio 实例的一个或者多个引脚的值
 @param[in] gpio_ptr: 结构体 vsf_gpio_t 的指针，参考 @ref vsf_gpio_t
 @param[in] pin_mask: 引脚掩码，每一个引脚对应一个位，1表示该位需要写入，0表示该位不需要更新
 */
extern void vsf_gpio_toggle(vsf_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask);

/**
 \~english
 @brief Set the direction of one or more pins of the gpio instance to output high
 @param[in] gpio_ptr: pointer to the structure vsf_gpio_t, refer to @ref vsf_gpio_t
 @param[in] pin_mask: pin mask, each pin corresponds to one bit, 1 means the bit
            needs to be written, 0 means the bit does not need to be updated
 @note This API can be used when modifying the IO direction to output and set to
       high in order to avoid possible pulses. Note that it is not supported by all hardware.
       You can get if this mode is supported with vsf_gpio_capability()
       (is_support_output_and_set)

 \~chinese
 @brief 设置 gpio 实例的一个或者多个引脚的方向为输出高电平
 @param[in] gpio_ptr: 结构体 vsf_gpio_t 的指针，参考 @ref vsf_gpio_t
 @param[in] pin_mask: 引脚掩码，每一个引脚对应一个位，1表示该位需要写入，0表示该位不需要更新
 @note 修改IO方向为输出并且设置成高电平的时候，为了避免可能的脉冲，可以使用这个API。
       需要注意的是，并不是所有硬件都支持。可以通过 vsf_gpio_capability() 获取是否支持该特性
      （ is_support_output_and_set ）
 */
extern void vsf_gpio_output_and_set(vsf_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask);

/**
 \~english
 @brief Set the direction of one or more pins of the gpio instance to output low
 @param[in] gpio_ptr: pointer to the structure vsf_gpio_t, refer to @ref vsf_gpio_t
 @param[in] pin_mask: pin mask, each pin corresponds to one bit, 1 means the bit
            needs to be written, 0 means the bit does not need to be updated
 @note This API can be used when modifying the IO direction to output and set to
       low in order to avoid possible pulses. Note that it is not supported by all hardware.
       You can get if this mode is supported with vsf_gpio_capability()
       (is_support_output_and_set)

 \~chinese
 @brief 设置 gpio 实例的一个或者多个引脚的方向为输出低电平
 @param[in] gpio_ptr: 结构体 vsf_gpio_t 的指针，参考 @ref vsf_gpio_t
 @param[in] pin_mask: 引脚掩码，每一个引脚对应一个位，1表示该位需要写入，0表示该位不需要更新
 @note 修改IO方向为输出并且设置成低电平的时候，为了避免可能的脉冲，可以使用这个API。
       需要注意的是，并不是所有硬件都支持。可以通过 vsf_gpio_capability() 获取是否支持该特性
      （ is_support_output_and_set ）
 */
extern void vsf_gpio_output_and_clear(vsf_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask);

/**
 \~english
 @brief Get the capability of gpio instance
 @param[in] gpio_ptr: pointer to the structure vsf_gpio_t, refer to @ref vsf_gpio_t

 \~chinese
 @brief 获取 gpio 实例的能力
 @param[in] gpio_ptr: 结构体 vsf_gpio_t 的指针，参考 @ref vsf_gpio_t
 */
extern vsf_gpio_capability_t vsf_gpio_capability(vsf_gpio_t *gpio_ptr);


/**
 \~english
 @brief Configure interrupt of one or more pins of the gpio instance
 @param[in] gpio_ptr: a pointer to structure @ref vsf_gpio_t
 @param[in] cfg: a pointer to structure @ref vsf_gpio_pin_irq_cfg_t

 \~chinese
 @brief 配置 gpio 实例的一个或者多个引脚
 @param[in] gpio_ptr: 结构体 vsf_gpio_t 的指针，参考 @ref vsf_gpio_t
 @param[in] cfg: 结构体 vsf_gpio_pin_irq_cfg_t 的指针，参考 @ref vsf_gpio_pin_irq_cfg_t
 */
extern vsf_err_t vsf_gpio_exti_config(vsf_gpio_t *gpio_ptr, vsf_gpio_pin_irq_cfg_t *cfg_ptr);

/**
 \~english
 @brief Enable interrupt of a pin
 @param[in] gpio_ptr: a pointer to structure @ref vsf_gpio_t
 @param[in] pin_mask: pin mask, each pin corresponds to one bit, 1 means the bit
 @param[in] prio: priority of the interrupt or vsf_arch_prio_invalid for disable
 @note For some devices, prio parameter maybe shared between pins on the gpio.
 @note All pending interrupts should be cleared before interrupts are enabled.

 \~chinese
 @brief 使能指定引脚的中断
 @param[in] gpio_ptr: 结构体 vsf_gpio_t 的指针，参考 @ref vsf_gpio_t
 @param[in] pin_mask: 引脚掩码，每一个引脚对应一个位，1表示该位需要使能，0表示该位不需要使能
 @param[in] prio: 中断优先级，或者用 vsf_arch_prio_invalid 表示关闭中断
 @note 对于一些芯片, 中断优先级可能是 gpio 上所有引脚公用的。
 @note 在中断使能之前，应该清除所有悬挂的中断。
 */
extern vsf_err_t vsf_gpio_exti_irq_enable(vsf_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask, vsf_arch_prio_t prio);

/**
 \~english
 @brief Disable interrupt of a pin
 @param[in] gpio_ptr: a pointer to structure @ref vsf_gpio_t
 @param[in] pin_mask: pin mask, each pin corresponds to one bit, 1 means the bit
 @note For some devices, prio parameter maybe shared between pins on the gpio.

 \~chinese
 @brief 禁能指定引脚的中断
 @param[in] gpio_ptr: 结构体 vsf_gpio_t 的指针，参考 @ref vsf_gpio_t
 @param[in] pin_mask: 引脚掩码，每一个引脚对应一个位，1表示该位需要使能，0表示该位不需要使能
 @note 对于一些芯片, 中断优先级可能是 gpio 上所有引脚公用的。
 */
extern vsf_err_t vsf_gpio_exti_irq_disable(vsf_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask);

/*============================ MACROS ========================================*/

#if VSF_GPIO_CFG_FUNCTION_RENAME == ENABLED
#   define __vsf_gpio_t                                 VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_t)
#   define vsf_gpio_capability(__GPIO)                  VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_capability)             ((__vsf_gpio_t *)__GPIO)
#   define vsf_gpio_config_pin(__GPIO, ...)             VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_config_pin)             ((__vsf_gpio_t *)__GPIO, ##__VA_ARGS__)
#   define vsf_gpio_set_direction(__GPIO, ...)          VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_set_direction)          ((__vsf_gpio_t *)__GPIO, ##__VA_ARGS__)
#   define vsf_gpio_get_direction(__GPIO, ...)          VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_get_direction)          ((__vsf_gpio_t *)__GPIO, ##__VA_ARGS__)
#   define vsf_gpio_set_input(__GPIO, ...)              VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_set_input)              ((__vsf_gpio_t *)__GPIO, ##__VA_ARGS__)
#   define vsf_gpio_set_output(__GPIO, ...)             VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_set_output)             ((__vsf_gpio_t *)__GPIO, ##__VA_ARGS__)
#   define vsf_gpio_switch_direction(__GPIO, ...)       VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_switch_direction)       ((__vsf_gpio_t *)__GPIO, ##__VA_ARGS__)
#   define vsf_gpio_read(__GPIO)                        VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_read)                   ((__vsf_gpio_t *)__GPIO)
#   define vsf_gpio_write(__GPIO, ...)                  VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_write)                  ((__vsf_gpio_t *)__GPIO, ##__VA_ARGS__)
#   define vsf_gpio_set(__GPIO, ...)                    VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_set)                    ((__vsf_gpio_t *)__GPIO, ##__VA_ARGS__)
#   define vsf_gpio_clear(__GPIO, ...)                  VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_clear)                  ((__vsf_gpio_t *)__GPIO, ##__VA_ARGS__)
#   define vsf_gpio_output_and_set(__GPIO, ...)         VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_output_and_set)         ((__vsf_gpio_t *)__GPIO, ##__VA_ARGS__)
#   define vsf_gpio_output_and_clear(__GPIO, ...)       VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_output_and_clear)       ((__vsf_gpio_t *)__GPIO, ##__VA_ARGS__)
#   define vsf_gpio_toggle(__GPIO, ...)                 VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_toggle)                 ((__vsf_gpio_t *)__GPIO, ##__VA_ARGS__)
#   define vsf_gpio_exti_config(__GPIO, ...)            VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_exti_config)            ((__vsf_gpio_t *)__GPIO, ##__VA_ARGS__)
#   define vsf_gpio_exti_irq_enable(__GPIO, ...)        VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_exti_irq_enable)        ((__vsf_gpio_t *)__GPIO, ##__VA_ARGS__)
#   define vsf_gpio_exti_irq_disable(__GPIO, ...)       VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_exti_irq_disable)       ((__vsf_gpio_t *)__GPIO, ##__VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif  /* __VSF_TEMPLATE_GPIO_H__ */
