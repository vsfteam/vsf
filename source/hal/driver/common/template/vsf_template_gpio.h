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
#   define VSF_HW_GPIO_PIN_MASK                     VSF_HAL_COUNT_TO_MASK(VSF_HW_GPIO_PIN_COUNT)
#endif

#if defined(VSF_HW_GPIO_PIN_MASK) && !defined(VSF_HW_GPIO_PIN_COUNT)
#   define VSF_HW_GPIO_PIN_COUNT                    VSF_HAL_MASK_TO_COUNT(VSF_HW_GPIO_PIN_MASK)
#endif

#ifdef VSF_HW_GPIO_PORT_COUNT
#	define VSF_HW_GPIO_COUNT                        VSF_HW_GPIO_PORT_COUNT
#endif

#ifdef VSF_HW_GPIO_PORT_MASK
#	define VSF_HW_GPIO_MASK                         VSF_HW_GPIO_PORT_MASK
#endif

#if !defined(VSF_GPIO_CFG_PORTA) && (VSF_HW_GPIO_PORT_MASK & (1ul << 0))
#   define VSF_GPIO_CFG_PORTA                         ENABLED
#endif

#if !defined(VSF_GPIO_CFG_PORTB) && (VSF_HW_GPIO_PORT_MASK & (1ul << 1))
#   define VSF_GPIO_CFG_PORTB                         ENABLED
#endif

#if !defined(VSF_GPIO_CFG_PORTC) && (VSF_HW_GPIO_PORT_MASK & (1ul << 2))
#   define VSF_GPIO_CFG_PORTC                         ENABLED
#endif

#if !defined(VSF_GPIO_CFG_PORTD) && (VSF_HW_GPIO_PORT_MASK & (1ul << 3))
#   define VSF_GPIO_CFG_PORTD                         ENABLED
#endif

#if !defined(VSF_GPIO_CFG_PORTE) && (VSF_HW_GPIO_PORT_MASK & (1ul << 4))
#   define VSF_GPIO_CFG_PORTE                         ENABLED
#endif

#if !defined(VSF_GPIO_CFG_PORTF) && (VSF_HW_GPIO_PORT_MASK & (1ul << 5))
#   define VSF_GPIO_CFG_PORTF                         ENABLED
#endif

#if !defined(VSF_GPIO_CFG_PORTG) && (VSF_HW_GPIO_PORT_MASK & (1ul << 6))
#   define VSF_GPIO_CFG_PORTG                         ENABLED
#endif

#if !defined(VSF_GPIO_CFG_PORTH) && (VSF_HW_GPIO_PORT_MASK & (1ul << 7))
#   define VSF_GPIO_CFG_PORTH                         ENABLED
#endif

#if !defined(VSF_GPIO_CFG_PORTI) && (VSF_HW_GPIO_PORT_MASK & (1ul << 8))
#   define VSF_GPIO_CFG_PORTI                         ENABLED
#endif

#if !defined(VSF_GPIO_CFG_PORTJ) && (VSF_HW_GPIO_PORT_MASK & (1ul << 9))
#   define VSF_GPIO_CFG_PORTJ                         ENABLED
#endif

#if !defined(VSF_GPIO_CFG_PORTK) && (VSF_HW_GPIO_PORT_MASK & (1ul << 10))
#   define VSF_GPIO_CFG_PORTK                         ENABLED
#endif


#ifndef VSF_GPIO_CFG_PIN_COUNT
#   if defined(VSF_HW_GPIO_PIN_COUNT) && (VSF_HW_GPIO_PIN_COUNT > 32)
#	    define vsf_io_pin_mask_t                    uint64_t
#       define VSF_GPIO_CFG_PIN_COUNT                 64
#       define VSF_GPIO_CFG_PIN_MASK                  0xFFFFFFFFFFFFFFFF
#   elif defined(VSF_HW_GPIO_PIN_AMSK) && (VSF_HW_GPIO_PIN_MASK & 0xFFFFFFFF00000000)
#	    define vsf_io_pin_mask_t                    uint64_t
#       define VSF_GPIO_CFG_PIN_COUNT                 64
#       define VSF_GPIO_CFG_PIN_MASK                  0xFFFFFFFFFFFFFFFF
#   else
#	    define vsf_io_pin_mask_t                    uint32_t
#       define VSF_GPIO_CFG_PIN_COUNT                 32
#       define VSF_GPIO_CFG_PIN_MASK                  0xFFFFFFFF
#   endif
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

#ifndef VSF_GPIO_CFG_FUNCTION_RENAME
#   define VSF_GPIO_CFG_FUNCTION_RENAME             ENABLED
#endif


//! Redefine struct vsf_gpio_cfg_t. The vsf_gpio_exti_isr_handler_t type also needs to
//! be redefined For compatibility, members should not be deleted when struct
//! @ref vsf_gpio_cfg_t redefining.
#if VSF_GPIO_CFG_REIMPLEMENT_TYPE_CFG == DISABLED
#    define VSF_GPIO_CFG_REIMPLEMENT_TYPE_CFG DISABLED
#endif

//! Redefine struct vsf_gpio_capability_t.
//! For compatibility, members should not be deleted when struct @ref
//! vsf_gpio_capability_t redefining.
#if VSF_GPIO_CFG_REIMPLEMENT_TYPE_CAPABILITY == DISABLED
#    define VSF_GPIO_CFG_REIMPLEMENT_TYPE_CAPABILITY DISABLED
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

#define __VSF_GPIO_PORT_PIN_NUM(__PIN_NUM, __PORT_NUM)                            \
    VSF_P ## __PORT_NUM ## __PIN_NUM = ((VSF_PORT ##__PORT_NUM) << 8) | __PIN_NUM,

#define VSF_GPIO_APIS(__prefix_name)                                                                                                                                                                               \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_gpio_capability_t, gpio, capability,              VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr)                                                                    \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,             gpio, port_config_pins,        VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, vsf_gpio_pin_mask_t pin_mask, vsf_gpio_cfg_t * cfg_ptr)            \
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
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,             gpio, exti_irq_config,         VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, vsf_gpio_exti_irq_cfg_t *irq_cfg_ptr)                              \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,             gpio, exti_irq_enable,         VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)                                      \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,             gpio, exti_irq_disable,        VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)

/*============================ TYPES =========================================*/

#if VSF_GPIO_CFG_REIMPLEMENT_TYPE_MODE == DISABLED
/**
 * \~english
 * @brief Predefined VSF GPIO modes that can be reimplemented in specific hal drivers.
 *
 * \~chinese
 * @brief 预定义的VSF GPIO 模式，可以在具体的hal驱动重新实现。
 *
 * \~english
 * Even if the hardware doesn't support these features, these modes must be implemented:
 * If the IO supports more modes, such as the alternative output push-pull and
 * the alternative open-drain output, We can implement it in the hardware driver.
 * However, VSF_GPIO_AF mode must be retained.
 * If we add a new mode in the hardware driver, then we also need to define the
 * macro VSF_GPIO_MODE_MASK, whose value is the OR of the value of all modes.
 * \~chinese
 * 即使硬件不支持这些功能，但是这些模式是必须实现的。如果硬件支持更多模式，例如复用推挽输出或者
 * 复用开漏输出，我们可以在硬件驱动里实现它，但是 VSF_GPIO_AF 模式必须保留。
 * 如果硬件驱动里我们添加了新的模式，那么也需要定义宏 VSF_GPIO_MODE_MASK，它的值是所有模式的值的或。
 *
 * - INPUT/OUTPUT
 *  - VSF_GPIO_INPUT
 *  - VSF_GPIO_ANALOG
 *  - VSF_GPIO_OUTPUT_PUSH_PULL
 *  - VSF_GPIO_OUTPUT_OPEN_DRAIN
 *  - VSF_GPIO_AF
 *  - VSF_GPIO_EXTI
 * - PULL_UP_AND_DOWN:
 *  - VSF_GPIO_NO_PULL_UP_DOWN
 *  - VSF_GPIO_PULL_UP
 *  - VSF_GPIO_PULL_DOWN
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
 * vsf_gpio_cfg_t cfg = {
 *     .mode = VSF_GPIO_OUTPUT_PUSH_PULL,
 *     ...
 * };
 * vsf_gpio_pin_mask pin_mask = 0x00000001;
 * vsf_gpio_port_config_pins(&vsf_hw_gpi0, pin_mask, &cfg);
 * ```
 *
 * \~chinese
 * 考虑上面的代码，用户调用 vsf_io_cfg() API 的时候，提供的 vsf_io_mode_t 可能会省略很多选项。
 * 所以我们要求部分选项的值应该是 0。包括:
 * - VSF_GPIO_NO_PULL_UP_DOWN
 * - VSF_GPIO_INTERRUPT_DISABLED
 * 可选特性选项的值也应该考虑默认值的问题。
 *
 * \~english
 * Consider the code above. the vsf_io_mode_t provided when the user calls the vsf_io_cfg() API
 * may omit many options. So we require that some of the options should have a value of 0. Including:
 * - VSF_GPIO_NO_PULL_UP_DOWN
 * - VSF_GPIO_INTERRUPT_DISABLED
 * The values of the optional feature options should also be considered for the default values.
 */
typedef enum vsf_gpio_mode_t {
    //! Set the GPIO sa the general-purpose input state.
    //! It should select GPIO for the alternative function
    //! It is independent of the pull-up, pull-down, and float configurations.
    VSF_GPIO_INPUT                    = (0 << 0),
    //! Set the IO as analog IO, including analog input and analog output.
    //! It should select analog or GPIO as an alternative function and set to input direction
    VSF_GPIO_ANALOG                   = (1 << 0),
    //! Set GPIO sa general-purpose IO push-pull output
    //! It should select GPIO for the alternative function
    VSF_GPIO_OUTPUT_PUSH_PULL         = (2 << 0),
    //! Set GPIO as general-purpose IO open drain output
    //! It should select GPIO for the alternative function
    VSF_GPIO_OUTPUT_OPEN_DRAIN        = (3 << 0),
    //! Set GPIO as external interrupt IO
    VSF_GPIO_EXTI                     = (4 << 0),
    //! Set GPIO to Alternative Function
    VSF_GPIO_AF                       = (5 << 0),

    // Turn on or off the internal pull-up and pull-down resistors of the IOs
    // It is independent of the direction of the IO and the alternative function
    VSF_GPIO_NO_PULL_UP_DOWN          = (0 << 4),
    VSF_GPIO_PULL_UP                  = (1 << 4),
    VSF_GPIO_PULL_DOWN                = (2 << 4),

    VSF_GPIO_EXTI_MODE_NONE           = (0 << 6),
    VSF_GPIO_EXTI_MODE_LOW_LEVEL      = (1 << 6),
    VSF_GPIO_EXTI_MODE_HIGH_LEVEL     = (2 << 6),
    VSF_GPIO_EXTI_MODE_RISING         = (3 << 6),
    VSF_GPIO_EXTI_MODE_FALLING        = (4 << 6),
    VSF_GPIO_EXTI_MODE_RISING_FALLING = (5 << 6),

/*
    //! Set GPIO to alternative function
    VSF_GPIO_AF                       = (5 << 0),

    VSF_GPIO_INVERT_INPUT             = (1 << 7),
    #define VSF_GPIO_INVERT_INPUT VSF_GPIO_INVERT_INPUT

    VSF_GPIO_HIGH_DRIVE_STRENGTH      = (1 << 8),
    #define VSF_GPIO_HIGH_DRIVE_STRENGTH VSF_GPIO_HIGH_DRIVE_STRENGTH

    VSF_GPIO_SPEED_LOW                = (0 << 9),
    VSF_GPIO_SPEED_MEDIUM             = (1 << 9),
    VSF_GPIO_SPEED_HIGH               = (2 << 9),
    VSF_GPIO_SPEED_VERY_HIGH          = (3 << 9),
    VSF_GPIO_SPEED_MASK               = (3 << 9),
    #define VSF_GPIO_SPEED_LOW        VSF_GPIO_SPEED_LOW
    #define VSF_GPIO_SPEED_MEDIUM     VSF_GPIO_SPEED_MEDIUM
    #define VSF_GPIO_SPEED_HIGH       VSF_GPIO_SPEED_HIGH
    #define VSF_GPIO_SPEED_VERY_HIGH  VSF_GPIO_SPEED_VERY_HIGH
    #define VSF_GPIO_SPEED_MASK       VSF_GPIO_SPEED_MASK
*/
} vsf_gpio_mode_t;
#endif

enum {
    VSF_GPIO_FLOATING               = VSF_GPIO_NO_PULL_UP_DOWN,

#ifndef VSF_GPIO_MODE_MASK
    VSF_GPIO_MODE_MASK              = VSF_GPIO_INPUT
                                    | VSF_GPIO_EXTI
                                    | VSF_GPIO_ANALOG
                                    | VSF_GPIO_OUTPUT_PUSH_PULL
                                    | VSF_GPIO_OUTPUT_OPEN_DRAIN
                                    | VSF_GPIO_AF,
#endif

    VSF_GPIO_PULL_UP_DOWN_MASK      = VSF_GPIO_NO_PULL_UP_DOWN
                                    | VSF_GPIO_PULL_UP
                                    | VSF_GPIO_PULL_DOWN,

    VSF_GPIO_EXTI_MODE_MASK         = VSF_GPIO_EXTI_MODE_NONE
                                    | VSF_GPIO_EXTI_MODE_LOW_LEVEL
                                    | VSF_GPIO_EXTI_MODE_HIGH_LEVEL
                                    | VSF_GPIO_EXTI_MODE_RISING
                                    | VSF_GPIO_EXTI_MODE_FALLING
                                    | VSF_GPIO_EXTI_MODE_RISING_FALLING,

    VSF_GPIO_MODE_ALL_BITS_MASK     = VSF_GPIO_MODE_MASK
                                    | VSF_GPIO_PULL_UP_DOWN_MASK
                                    | VSF_GPIO_EXTI_MODE_MASK
#ifdef VSF_GPIO_INVERT_INPUT
                                    | VSF_GPIO_INVERT_INPUT
#endif
#ifdef VSF_GPIO_HIGH_DRIVE_STRENGTH
                                    | VSF_GPIO_HIGH_DRIVE_STRENGTH
#endif
#ifdef VSF_GPIO_SPEED_MASK
                                    | VSF_GPIO_SPEED_MASK
#endif
#ifdef VSF_GPIO_SPEED_MASK
                                    | VSF_GPIO_SPEED_MASK
#endif
};

#if VSF_GPIO_CFG_REIMPLEMENT_TYPE_CFG == DISABLED
//! Pre-declaration of GPIO structures
typedef struct vsf_gpio_t vsf_gpio_t;

//! External interrupt callback function for gpio
typedef void vsf_gpio_exti_isr_handler_t(void *target_ptr, vsf_gpio_t *gpio_ptr,
                                         vsf_gpio_pin_mask_t pin_mask);

//! Configure external interrupts for the entire GPIO port, not the specific
//! pins.
typedef struct vsf_gpio_exti_irq_cfg_t {
    vsf_gpio_exti_isr_handler_t *handler_fn;
    void                        *target_ptr;
    vsf_arch_prio_t              prio;
} vsf_gpio_exti_irq_cfg_t;

//! gpio channel configuration
typedef struct vsf_gpio_cfg_t {
    vsf_gpio_mode_t     mode;
    //! alternate function is only valid in GPIO_AF mode
    uint16_t            alternate_function;
} vsf_gpio_cfg_t;

typedef struct vsf_gpio_port_cfg_pin_t {
    uint16_t            port_pin_index;
    //! For simplicity, the expansion of type vsf_gpio_cfg_t is used here instead
    //! of using type vsf_gpio_cfg_t directly
    vsf_gpio_mode_t     mode;
    //! alternate function is only valid in GPIO_AF mode
    uint16_t            alternate_function;
} vsf_gpio_port_cfg_pin_t;

typedef struct vsf_gpio_port_cfg_pins_t {
    uint16_t            port_index;
    vsf_gpio_pin_mask_t pin_mask;
    //! For simplicity, the expansion of type vsf_gpio_cfg_t is used here instead
    //! of using type vsf_gpio_cfg_t directly
    vsf_gpio_mode_t     mode;
    //! alternate function is only valid in GPIO_AF mode
    uint16_t            alternate_function;
} vsf_gpio_port_cfg_pins_t;
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_TYPE_CAPABILITY == DISABLED
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
    //!  0xFFFFFFFE (31 pins, PIN0 is not available)
    vsf_gpio_pin_mask_t pin_mask;
} vsf_gpio_capability_t;
#endif

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


#if VSF_HW_GPIO_PORT_COUNT > 0
typedef enum vsf_io_port_no_t {
#if defined(VSF_GPIO_CFG_PORTA)
    VSF_PORTA,
#endif
#if defined(VSF_GPIO_CFG_PORTB)
    VSF_PORTB,
#endif
#if defined(VSF_GPIO_CFG_PORTC)
    VSF_PORTC,
#endif
#if defined(VSF_GPIO_CFG_PORTD)
    VSF_PORTD,
#endif
#if defined(VSF_GPIO_CFG_PORTE)
    VSF_PORTE,
#endif
#if defined(VSF_GPIO_CFG_PORTF)
    VSF_PORTF,
#endif
#if defined(VSF_GPIO_CFG_PORTG)
    VSF_PORTG,
#endif
#if defined(VSF_GPIO_CFG_PORTH)
    VSF_PORTH,
#endif
#if defined(VSF_GPIO_CFG_PORTI)
    VSF_PORTI,
#endif
#if defined(VSF_GPIO_CFG_PORTJ)
    VSF_PORTJ,
#endif
#if defined(VSF_GPIO_CFG_PORTK)
    VSF_PORTK,
#endif
} vsf_io_port_no_t;

typedef enum vsf_io_port_pin_no_t {
#if defined(VSF_GPIO_CFG_PORTA)
    VSF_MREPEAT(VSF_GPIO_CFG_PIN_COUNT, __VSF_GPIO_PORT_PIN_NUM, A)
#endif
#if defined(VSF_GPIO_CFG_PORTB)
    VSF_MREPEAT(VSF_GPIO_CFG_PIN_COUNT, __VSF_GPIO_PORT_PIN_NUM, B)
#endif
#if defined(VSF_GPIO_CFG_PORTC)
    VSF_MREPEAT(VSF_GPIO_CFG_PIN_COUNT, __VSF_GPIO_PORT_PIN_NUM, C)
#endif
#if defined(VSF_GPIO_CFG_PORTD)
    VSF_MREPEAT(VSF_GPIO_CFG_PIN_COUNT, __VSF_GPIO_PORT_PIN_NUM, D)
#endif
#if defined(VSF_GPIO_CFG_PORTE)
    VSF_MREPEAT(VSF_GPIO_CFG_PIN_COUNT, __VSF_GPIO_PORT_PIN_NUM, E)
#endif
#if defined(VSF_GPIO_CFG_PORTF)
    VSF_MREPEAT(VSF_GPIO_CFG_PIN_COUNT, __VSF_GPIO_PORT_PIN_NUM, F)
#endif
#if defined(VSF_GPIO_CFG_PORTG)
    VSF_MREPEAT(VSF_GPIO_CFG_PIN_COUNT, __VSF_GPIO_PORT_PIN_NUM, G)
#endif
#if defined(VSF_GPIO_CFG_PORTH)
    VSF_MREPEAT(VSF_GPIO_CFG_PIN_COUNT, __VSF_GPIO_PORT_PIN_NUM, H)
#endif
#if defined(VSF_GPIO_CFG_PORTI)
    VSF_MREPEAT(VSF_GPIO_CFG_PIN_COUNT, __VSF_GPIO_PORT_PIN_NUM, I)
#endif
#if defined(VSF_GPIO_CFG_PORTJ)
    VSF_MREPEAT(VSF_GPIO_CFG_PIN_COUNT, __VSF_GPIO_PORT_PIN_NUM, J)
#endif
#if defined(VSF_GPIO_CFG_PORTK)
    VSF_MREPEAT(VSF_GPIO_CFG_PIN_COUNT, __VSF_GPIO_PORT_PIN_NUM, K)
#endif
} vsf_io_port_pin_no_t;
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
 @param[in] cfg: a pointer to structure @ref vsf_gpio_cfg_t
 @return vsf_err_t: VSF_ERR_NONE if GPIO Configuration Successful, or a negative error code


 \~chinese
 @brief 配置 gpio 实例的一个或者多个引脚
 @param[in] gpio_ptr: 结构体 vsf_gpio_t 的指针，参考 @ref vsf_gpio_t
 @param[in] pin_mask: 引脚掩码，每一个引脚对应一个位，该bit的值位1表示需要配置
 @param[in] cfg: 结构体 vsf_gpio_cfg_t 的指针，参考 @ref vsf_gpio_cfg_t
 @return vsf_err_t: 如果 GPIO 配置成功返回 VSF_ERR_NONE , 失败返回负数。
 */
extern vsf_err_t vsf_gpio_port_config_pins(vsf_gpio_t         *gpio_ptr,
                                           vsf_gpio_pin_mask_t pin_mask,
                                           vsf_gpio_cfg_t     *cfg);

///**
// \~english
// @brief Configure one or more ports and pin of the gpio instance
// @param[in] cfg: a pointer to structure @ref vsf_gpio_port_cfg_pin_t
// @return vsf_err_t: VSF_ERR_NONE if GPIO Configuration Successful, or a negative error code
// @note The VSF_PREFIX prefix of this can be replaced with the actual prefix, e.g. vsf_hw
//
// \~chinese
// @brief 配置 gpio 的一个或者多个端口和引脚
// @param[in] cfg: 结构体 vsf_gpio_port_cfg_pin_t 的指针，参考 @ref vsf_gpio_port_cfg_pin_t
// @return vsf_err_t: 如果 GPIO 配置成功返回 VSF_ERR_NONE , 失败返回负数。
// @note VSF_PREFIX 前缀可以替换成实际的前缀，例如 vsf_hw
// */
//extern vsf_err_t VSF_PREFIX_gpio_ports_config_pins(vsf_gpio_t         *gpio_ptr,
//                                           vsf_gpio_pin_mask_t  pin_mask,
//                                           vsf_gpio_cfg_t      *cfg);
//
///**
// \~english
// @brief Configure one pin for one or more ports of the gpio instance
// @param[in] cfg: a pointer to structure @ref vsf_gpio_port_cfg_pins_t
// @return vsf_err_t: VSF_ERR_NONE if GPIO Configuration Successful, or a negative error code
// @note The VSF_PREFIX prefix of this can be replaced with the actual prefix, e.g. vsf_hw
//
// \~chinese
// @brief 配置 gpio 的一个或者多个端口的一个引脚
// @param[in] cfg: 结构体 vsf_gpio_port_cfg_pins_t 的指针，参考 @ref vsf_gpio_port_cfg_pins_t
// @return vsf_err_t: 如果 GPIO 配置成功返回 VSF_ERR_NONE , 失败返回负数。
// @note VSF_PREFIX 前缀可以替换成实际的前缀，例如 vsf_hw
// */
//extern vsf_err_t VSF_PREFIX_gpio_ports_config_pin(vsf_gpio_t         *gpio_ptr,
//                                           vsf_gpio_pin_mask_t    pin_mask,
//                                           vsf_gpio_cfg_t        *cfg);


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
 @brief Configure external interrupt of the gpio instance
 @param[in] gpio_ptr: a pointer to structure @ref vsf_gpio_t
 @param[in] cfg: a pointer to structure @ref vsf_gpio_exti_irq_cfg_t

 \~chinese
 @brief 配置 gpio 实例的外部中断
 @param[in] gpio_ptr: 结构体 vsf_gpio_t 的指针，参考 @ref vsf_gpio_t
 @param[in] cfg: 结构体 vsf_gpio_exti_irq_cfg_t 的指针，参考 @ref vsf_gpio_exti_irq_cfg_t
 */
extern vsf_err_t vsf_gpio_exti_irq_config(vsf_gpio_t *gpio_ptr, vsf_gpio_exti_irq_cfg_t *cfg_ptr);

/**
 \~english
 @brief Enable interrupt of one or more pins
 @param[in] gpio_ptr: a pointer to structure @ref vsf_gpio_t
 @param[in] pin_mask: pin mask, each pin corresponds to one bit, 1 means the bit
 @param[in] prio: priority of the interrupt or vsf_arch_prio_invalid for disable
 @note For some devices, prio parameter maybe shared between pins on the gpio.
 @note All pending interrupts should be cleared before interrupts are enabled.

 \~chinese
 @brief 使能指定的一个或者多个引脚的中断
 @param[in] gpio_ptr: 结构体 vsf_gpio_t 的指针，参考 @ref vsf_gpio_t
 @param[in] pin_mask: 引脚掩码，每一个引脚对应一个位，1表示该位需要使能，0表示该位不需要使能
 @note 对于一些芯片, 中断优先级可能是 gpio 上所有引脚公用的。
 @note 在中断使能之前，应该清除所有悬挂的中断。
 */
extern vsf_err_t vsf_gpio_exti_irq_enable(vsf_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask);

/**
 \~english
 @brief Disable interrupt of one or more pins
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

/*============================ INLINE FUNCTIONS ==============================*/

static inline uint8_t vsf_gpio_get_port(uint16_t port_pin_index)
{
    return port_pin_index >> 8;
}

static inline uint8_t vsf_gpio_get_pin(uint16_t port_pin_index)
{
    return port_pin_index & 0xFF;
}

static inline vsf_err_t vsf_gpio_port_config_pin(vsf_gpio_t      *gpio_ptr,
                                                 uint16_t         pin_index,
                                                 vsf_gpio_cfg_t  *cfg_ptr)
{
    return vsf_gpio_port_config_pins(gpio_ptr, 1 << pin_index, cfg_ptr);
}

/*============================ MACROS ========================================*/

#if VSF_GPIO_CFG_FUNCTION_RENAME == ENABLED
#   define __vsf_gpio_t                                 VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_t)
#   define vsf_gpio_capability(__GPIO)                  VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_capability)             ((__vsf_gpio_t *)(__GPIO))
#   define vsf_gpio_port_config_pins(__GPIO, ...)       VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_port_config_pins)       ((__vsf_gpio_t *)(__GPIO), ##__VA_ARGS__)
#   define vsf_gpio_set_direction(__GPIO, ...)          VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_set_direction)          ((__vsf_gpio_t *)(__GPIO), ##__VA_ARGS__)
#   define vsf_gpio_get_direction(__GPIO, ...)          VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_get_direction)          ((__vsf_gpio_t *)(__GPIO), ##__VA_ARGS__)
#   define vsf_gpio_set_input(__GPIO, ...)              VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_set_input)              ((__vsf_gpio_t *)(__GPIO), ##__VA_ARGS__)
#   define vsf_gpio_set_output(__GPIO, ...)             VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_set_output)             ((__vsf_gpio_t *)(__GPIO), ##__VA_ARGS__)
#   define vsf_gpio_switch_direction(__GPIO, ...)       VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_switch_direction)       ((__vsf_gpio_t *)(__GPIO), ##__VA_ARGS__)
#   define vsf_gpio_read(__GPIO)                        VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_read)                   ((__vsf_gpio_t *)(__GPIO))
#   define vsf_gpio_write(__GPIO, ...)                  VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_write)                  ((__vsf_gpio_t *)(__GPIO), ##__VA_ARGS__)
#   define vsf_gpio_set(__GPIO, ...)                    VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_set)                    ((__vsf_gpio_t *)(__GPIO), ##__VA_ARGS__)
#   define vsf_gpio_clear(__GPIO, ...)                  VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_clear)                  ((__vsf_gpio_t *)(__GPIO), ##__VA_ARGS__)
#   define vsf_gpio_output_and_set(__GPIO, ...)         VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_output_and_set)         ((__vsf_gpio_t *)(__GPIO), ##__VA_ARGS__)
#   define vsf_gpio_output_and_clear(__GPIO, ...)       VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_output_and_clear)       ((__vsf_gpio_t *)(__GPIO), ##__VA_ARGS__)
#   define vsf_gpio_toggle(__GPIO, ...)                 VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_toggle)                 ((__vsf_gpio_t *)(__GPIO), ##__VA_ARGS__)
#   define vsf_gpio_exti_irq_config(__GPIO, ...)        VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_exti_irq_config)        ((__vsf_gpio_t *)(__GPIO), ##__VA_ARGS__)
#   define vsf_gpio_exti_irq_enable(__GPIO, ...)        VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_exti_irq_enable)        ((__vsf_gpio_t *)(__GPIO), ##__VA_ARGS__)
#   define vsf_gpio_exti_irq_disable(__GPIO, ...)       VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_exti_irq_disable)       ((__vsf_gpio_t *)(__GPIO), ##__VA_ARGS__)

#   define vsf_gpio_exti_irq_pin_config(__GPIO, ...)    VSF_MCONNECT(vsf_exti_gpio, _exti_irq_pin_config)               ((vsf_exti_gpio_t *)(__GPIO), ##__VA_ARGS__)
#endif

// too long, put it end of file
#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 0)
#   define VSF_PIN0         0
#   define VSF_PIN0_MASK    (1 << VSF_PIN0)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 1)
#   define VSF_PIN1         1
#   define VSF_PIN1_MASK    (1 << VSF_PIN1)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 2)
#   define VSF_PIN2         2
#   define VSF_PIN2_MASK    (1 << VSF_PIN2)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 3)
#   define VSF_PIN3         3
#   define VSF_PIN3_MASK    (1 << VSF_PIN3)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 4)
#   define VSF_PIN4         4
#   define VSF_PIN4_MASK    (1 << VSF_PIN4)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 5)
#   define VSF_PIN5         5
#   define VSF_PIN5_MASK    (1 << VSF_PIN5)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 6)
#   define VSF_PIN6         6
#   define VSF_PIN6_MASK    (1 << VSF_PIN6)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 7)
#   define VSF_PIN7         7
#   define VSF_PIN7_MASK    (1 << VSF_PIN7)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 8)
#   define VSF_PIN8         8
#   define VSF_PIN8_MASK    (1 << VSF_PIN8)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 9)
#   define VSF_PIN9         9
#   define VSF_PIN9_MASK    (1 << VSF_PIN9)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 10)
#   define VSF_PIN10         10
#   define VSF_PIN10_MASK    (1 << VSF_PIN10)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 11)
#   define VSF_PIN11         11
#   define VSF_PIN11_MASK    (1 << VSF_PIN11)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 12)
#   define VSF_PIN12         12
#   define VSF_PIN12_MASK    (1 << VSF_PIN12)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 13)
#   define VSF_PIN13         13
#   define VSF_PIN13_MASK    (1 << VSF_PIN13)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 14)
#   define VSF_PIN14         14
#   define VSF_PIN14_MASK    (1 << VSF_PIN14)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 15)
#   define VSF_PIN15         15
#   define VSF_PIN15_MASK    (1 << VSF_PIN15)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 16)
#   define VSF_PIN16         16
#   define VSF_PIN16_MASK    (1 << VSF_PIN16)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 17)
#   define VSF_PIN17         17
#   define VSF_PIN17_MASK    (1 << VSF_PIN17)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 18)
#   define VSF_PIN18         18
#   define VSF_PIN18_MASK    (1 << VSF_PIN18)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 19)
#   define VSF_PIN19         19
#   define VSF_PIN19_MASK    (1 << VSF_PIN19)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 20)
#   define VSF_PIN20         20
#   define VSF_PIN20_MASK    (1 << VSF_PIN20)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 21)
#   define VSF_PIN21         21
#   define VSF_PIN21_MASK    (1 << VSF_PIN21)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 22)
#   define VSF_PIN22         22
#   define VSF_PIN22_MASK    (1 << VSF_PIN22)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 23)
#   define VSF_PIN23         23
#   define VSF_PIN23_MASK    (1 << VSF_PIN23)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 24)
#   define VSF_PIN24         24
#   define VSF_PIN24_MASK    (1 << VSF_PIN24)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 25)
#   define VSF_PIN25         25
#   define VSF_PIN25_MASK    (1 << VSF_PIN25)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 26)
#   define VSF_PIN26         26
#   define VSF_PIN26_MASK    (1 << VSF_PIN26)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 27)
#   define VSF_PIN27         27
#   define VSF_PIN27_MASK    (1 << VSF_PIN27)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 28)
#   define VSF_PIN28         28
#   define VSF_PIN28_MASK    (1 << VSF_PIN28)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 29)
#   define VSF_PIN29         29
#   define VSF_PIN29_MASK    (1 << VSF_PIN29)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 30)
#   define VSF_PIN30         30
#   define VSF_PIN30_MASK    (1 << VSF_PIN30)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 31)
#   define VSF_PIN31         31
#   define VSF_PIN31_MASK    (1 << VSF_PIN31)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 32)
#   define VSF_PIN32         32
#   define VSF_PIN32_MASK    (1 << VSF_PIN32)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 33)
#   define VSF_PIN33         33
#   define VSF_PIN33_MASK    (1 << VSF_PIN33)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 34)
#   define VSF_PIN34         34
#   define VSF_PIN34_MASK    (1 << VSF_PIN34)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 35)
#   define VSF_PIN35         35
#   define VSF_PIN35_MASK    (1 << VSF_PIN35)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 36)
#   define VSF_PIN36         36
#   define VSF_PIN36_MASK    (1 << VSF_PIN36)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 37)
#   define VSF_PIN37         37
#   define VSF_PIN37_MASK    (1 << VSF_PIN37)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 38)
#   define VSF_PIN38         38
#   define VSF_PIN38_MASK    (1 << VSF_PIN38)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 39)
#   define VSF_PIN39         39
#   define VSF_PIN39_MASK    (1 << VSF_PIN39)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 40)
#   define VSF_PIN40         40
#   define VSF_PIN40_MASK    (1 << VSF_PIN40)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 41)
#   define VSF_PIN41         41
#   define VSF_PIN41_MASK    (1 << VSF_PIN41)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 42)
#   define VSF_PIN42         42
#   define VSF_PIN42_MASK    (1 << VSF_PIN42)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 43)
#   define VSF_PIN43         43
#   define VSF_PIN43_MASK    (1 << VSF_PIN43)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 44)
#   define VSF_PIN44         44
#   define VSF_PIN44_MASK    (1 << VSF_PIN44)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 45)
#   define VSF_PIN45         45
#   define VSF_PIN45_MASK    (1 << VSF_PIN45)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 46)
#   define VSF_PIN46         46
#   define VSF_PIN46_MASK    (1 << VSF_PIN46)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 47)
#   define VSF_PIN47         47
#   define VSF_PIN47_MASK    (1 << VSF_PIN47)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 48)
#   define VSF_PIN48         48
#   define VSF_PIN48_MASK    (1 << VSF_PIN48)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 49)
#   define VSF_PIN49         49
#   define VSF_PIN49_MASK    (1 << VSF_PIN49)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 50)
#   define VSF_PIN50         50
#   define VSF_PIN50_MASK    (1 << VSF_PIN50)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 51)
#   define VSF_PIN51         51
#   define VSF_PIN51_MASK    (1 << VSF_PIN51)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 52)
#   define VSF_PIN52         52
#   define VSF_PIN52_MASK    (1 << VSF_PIN52)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 53)
#   define VSF_PIN53         53
#   define VSF_PIN53_MASK    (1 << VSF_PIN53)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 54)
#   define VSF_PIN54         54
#   define VSF_PIN54_MASK    (1 << VSF_PIN54)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 55)
#   define VSF_PIN55         55
#   define VSF_PIN55_MASK    (1 << VSF_PIN55)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 56)
#   define VSF_PIN56         56
#   define VSF_PIN56_MASK    (1 << VSF_PIN56)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 57)
#   define VSF_PIN57         57
#   define VSF_PIN57_MASK    (1 << VSF_PIN57)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 58)
#   define VSF_PIN58         58
#   define VSF_PIN58_MASK    (1 << VSF_PIN58)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 59)
#   define VSF_PIN59         59
#   define VSF_PIN59_MASK    (1 << VSF_PIN59)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 60)
#   define VSF_PIN60         60
#   define VSF_PIN60_MASK    (1 << VSF_PIN60)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 61)
#   define VSF_PIN61         61
#   define VSF_PIN61_MASK    (1 << VSF_PIN61)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 62)
#   define VSF_PIN62         62
#   define VSF_PIN62_MASK    (1 << VSF_PIN62)
#endif

#if VSF_GPIO_CFG_PIN_MASK & (0x01ul << 63)
#   define VSF_PIN63         63
#   define VSF_PIN63_MASK    (1 << VSF_PIN63)
#endif

#ifdef __cplusplus
}
#endif

#endif  /* __VSF_TEMPLATE_GPIO_H__ */
