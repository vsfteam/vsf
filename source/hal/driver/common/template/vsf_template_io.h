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

#ifndef __VSF_TEMPLATE_IO_H__
#define __VSF_TEMPLATE_IO_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_template_hal_driver.h"
#include "hal/arch/vsf_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

// multi-class support enabled by default for maximum availability.
#ifndef VSF_IO_CFG_MULTI_CLASS
#   define VSF_IO_CFG_MULTI_CLASS                   ENABLED
#endif

// get io configurations from GPIO module if not defined as IO moudle
#if !defined(VSF_HW_IO_PORT_COUNT) && !defined(VSF_HW_IO_PORT_MASK)
#   ifdef VSF_HW_GPIO_PORT_COUNT
#       define VSF_HW_IO_PORT_COUNT                 VSF_HW_GPIO_PORT_COUNT
#   endif
#   ifdef VSF_HW_GPIO_PORT_MASK
#       define VSF_HW_IO_PORT_MASK                  VSF_HW_GPIO_PORT_MASK
#   endif
#endif

#if defined(VSF_HW_IO_PORT_COUNT) && !defined(VSF_HW_IO_PORT_MASK)
#   define VSF_HW_IO_PORT_MASK                      VSF_HAL_COUNT_TO_MASK(VSF_HW_IO_PORT_COUNT)
#endif

#if defined(VSF_HW_IO_PORT_MASK) && !defined(VSF_HW_IO_PORT_COUNT)
#   define VSF_HW_IO_PORT_COUNT                     VSF_HAL_MASK_TO_COUNT(VSF_HW_IO_PORT_MASK)
#endif

#if defined(VSF_HW_IO_PIN_COUNT) && !defined(VSF_HW_IO_PIN_MASK)
#   define VSF_HW_IO_PIN_MASK                       VSF_HAL_COUNT_TO_MASK(VSF_HW_IO_PIN_COUNT)
#endif

#if defined(VSF_HW_IO_PIN_MASK) && !defined(VSF_HW_IO_PIN_COUNT)
#   define VSF_HW_IO_PIN_COUNT                      VSF_HAL_MASK_TO_COUNT(VSF_HW_IO_PIN_MASK)
#endif

// application code can redefine it
/*!
In the application code, we can redefine this macro to call the actual API directly without a runtime cost for example:
#define VSF_IO_CFG_MULTI_CLASS vsf_hw
vsf_io_config(&vsf_hw_io0, &cfg);
 It will be expanded at compile time to vsf_hw_io_config(&vsf_hw_io0, &cfg).

Dependency: VSF_IO_CFG_FUNCTION_RENAME enable
*/
#ifndef VSF_IO_CFG_PREFIX
#   if VSF_IO_CFG_MULTI_CLASS == ENABLED
#       define VSF_IO_CFG_PREFIX                    vsf
#   elif defined(VSF_HW_IO_PIN_MASK) && (VSF_HW_IO_PIN_MASK != 0)
#       define VSF_IO_CFG_PREFIX                    vsf_hw
#   else
#       define VSF_IO_CFG_PREFIX                    vsf
#   endif
#endif

//! If VSF_IO_CFG_FUNCTION_RENAME is enabled, the actual API is called based on
//! the value of VSF_IO_CFG_PREFIX
#ifndef VSF_IO_CFG_FUNCTION_RENAME
#    define VSF_IO_CFG_FUNCTION_RENAME ENABLED
#endif

//! In the specific hardware driver, we can enable
//! VSF_IO_CFG_REIMPLEMENT_TYPE_MODE to redefine vsf_io_mode_t as needed.
#ifndef VSF_IO_CFG_REIMPLEMENT_TYPE_MODE
#    define VSF_IO_CFG_REIMPLEMENT_TYPE_MODE DISABLED
#endif

//! Redefine struct vsf_io_cfg_t. The vsf_io_isr_handler_t type also needs to
//! be redefined For compatibility, members should not be deleted when struct
//! @ref vsf_io_cfg_t redefining.
#if VSF_IO_CFG_REIMPLEMENT_TYPE_CFG == DISABLED
#    define VSF_IO_CFG_REIMPLEMENT_TYPE_CFG DISABLED
#endif

//! Redefine struct vsf_io_capability_t.
//! For compatibility, members should not be deleted when struct @ref
//! vsf_io_capability_t redefining.
#if VSF_IO_CFG_REIMPLEMENT_TYPE_CAPABILITY == DISABLED
#    define VSF_IO_CFG_REIMPLEMENT_TYPE_CAPABILITY DISABLED
#endif

#if !defined(VSF_IO_CFG_PORTA) && (VSF_HW_IO_PORT_MASK & (1ul << 0))
#   define VSF_IO_CFG_PORTA                         ENABLED
#endif

#if !defined(VSF_IO_CFG_PORTB) && (VSF_HW_IO_PORT_MASK & (1ul << 1))
#   define VSF_IO_CFG_PORTB                         ENABLED
#endif

#if !defined(VSF_IO_CFG_PORTC) && (VSF_HW_IO_PORT_MASK & (1ul << 2))
#   define VSF_IO_CFG_PORTC                         ENABLED
#endif

#if !defined(VSF_IO_CFG_PORTD) && (VSF_HW_IO_PORT_MASK & (1ul << 3))
#   define VSF_IO_CFG_PORTD                         ENABLED
#endif

#if !defined(VSF_IO_CFG_PORTE) && (VSF_HW_IO_PORT_MASK & (1ul << 4))
#   define VSF_IO_CFG_PORTE                         ENABLED
#endif

#if !defined(VSF_IO_CFG_PORTF) && (VSF_HW_IO_PORT_MASK & (1ul << 5))
#   define VSF_IO_CFG_PORTF                         ENABLED
#endif

#if !defined(VSF_IO_CFG_PORTG) && (VSF_HW_IO_PORT_MASK & (1ul << 6))
#   define VSF_IO_CFG_PORTG                         ENABLED
#endif

#if !defined(VSF_IO_CFG_PORTH) && (VSF_HW_IO_PORT_MASK & (1ul << 7))
#   define VSF_IO_CFG_PORTH                         ENABLED
#endif

#if !defined(VSF_IO_CFG_PORTI) && (VSF_HW_IO_PORT_MASK & (1ul << 8))
#   define VSF_IO_CFG_PORTI                         ENABLED
#endif

#if !defined(VSF_IO_CFG_PORTJ) && (VSF_HW_IO_PORT_MASK & (1ul << 9))
#   define VSF_IO_CFG_PORTJ                         ENABLED
#endif

#if !defined(VSF_IO_CFG_PORTK) && (VSF_HW_IO_PORT_MASK & (1ul << 10))
#   define VSF_IO_CFG_PORTK                         ENABLED
#endif

#ifndef VSF_IO_CFG_PIN_COUNT
#   if defined(VSF_HW_IO_PIN_COUNT) && (VSF_HW_IO_PIN_COUNT > 32)
#	    define vsf_io_pin_mask_t                    uint64_t
#       define VSF_IO_CFG_PIN_COUNT                 64
#       define VSF_IO_CFG_PIN_MASK                  0xFFFFFFFFFFFFFFFF
#   elif defined(VSF_HW_IO_PIN_AMSK) && (VSF_HW_IO_PIN_MASK & 0xFFFFFFFF00000000)
#	    define vsf_io_pin_mask_t                    uint64_t
#       define VSF_IO_CFG_PIN_COUNT                 64
#       define VSF_IO_CFG_PIN_MASK                  0xFFFFFFFFFFFFFFFF
#   else
#	    define vsf_io_pin_mask_t                    uint32_t
#       define VSF_IO_CFG_PIN_COUNT                 32
#       define VSF_IO_CFG_PIN_MASK                  0xFFFFFFFF
#   endif
#endif

#ifndef VSF_IO_CFG_INHERT_HAL_CAPABILITY
#   define VSF_IO_CFG_INHERT_HAL_CAPABILITY       ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define __VSF_IO_PORT_PIN_NUM(__PIN_NUM, __PORT_NUM)                            \
    VSF_P ## __PORT_NUM ## __PIN_NUM = ((VSF_PORT ##__PORT_NUM) << 8) | __PIN_NUM,

#define VSF_IO_APIS(__prefix_name) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_io_capability_t,  io, capability,     VSF_MCONNECT(__prefix_name, _io_t) *io_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,            io, config,         VSF_MCONNECT(__prefix_name, _io_t) *io_ptr, vsf_io_cfg_t *cfg_ptr, uint_fast8_t count) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,            io, config_one_pin, VSF_MCONNECT(__prefix_name, _io_t) *io_ptr, vsf_io_cfg_t *cfg_ptr)

/*============================ TYPES =========================================*/

#if VSF_IO_CFG_REIMPLEMENT_TYPE_MODE == DISABLED
/**
 * \~english
 * @brief Predefined VSF IO modes that can be reimplemented in specific hal drivers.
 *
 * \~chinese
 * @brief 预定义的 VSF IO 模式，可以在具体的 HAL 驱动重新实现。
 *
 * \~english
 * Even if the hardware doesn't support these features, these modes must be implemented.
 * If the IO supports more modes, such as the alternative output push-pull and
 * the alternative open-drain output, We can implement it in the hardware driver.
 * However, VSF_IO_AF mode must be retained.
 * If we add a new mode in the hardware driver, then we also need to define the
 * macro VSF_IO_MODE_MASK, whose value is the OR of the value of all modes.
 * \~chinese
 * 即使硬件不支持这些功能，但是这些模式是必须实现的。如果硬件支持更多模式，例如复用推挽输出或者
 * 复用开漏输出，我们可以在硬件驱动里实现它，但是 VSF_IO_AF 模式必须保留。
 * 如果硬件驱动里我们添加了新的模式，那么也需要定义宏 VSF_IO_MODE_MASK，它的值是所有模式的值的或。
 *
 * - INPUT/OUTPUT
 *  - VSF_IO_INPUT
 *  - VSF_IO_ANALOG
 *  - VSF_IO_OUTPUT_PUSH_PULL
 *  - VSF_IO_OUTPUT_OPEN_DRAIN
 *  - VSF_IO_AF
 *  - VSF_IO_EXTI
 * - PULL_UP_AND_DOWN:
 *  - VSF_IO_NO_PULL_UP_DOWN
 *  - VSF_IO_PULL_UP
 *  - VSF_IO_PULL_DOWN
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
 * vsf_io_cfg_t cfg = {
 *  { VSF_PA0, 0, VSF_IO_OUTPUT_PUSH_PULL },
 *  { VSF_PA1, 0, VSF_IO_ANALOG },
 * };
 * vsf_io_config(&vsf_hw_io0, &cfg, 1);
 * ```
 *
 * \~chinese
 * 考虑上面的代码，用户调用 vsf_io_cfg() API 的时候，提供的 vsf_io_mode_t 可能会省略很多选项。
 * 所以我们要求部分选项的值应该是 0。包括:
 * - VSF_IO_NO_PULL_UP_DOWN
 * - VSF_IO_INTERRUPT_DISABLED
 * 可选特性选项的值也应该考虑默认值的问题。
 *
 * \~english
 * Consider the code above. the vsf_io_mode_t provided when the user calls the vsf_io_cfg() API
 * may omit many options. So we require that some of the options should have a value of 0. Including:
 * - VSF_IO_NO_PULL_UP_DOWN
 * - VSF_IO_INTERRUPT_DISABLED
 * The values of the optional feature options should also be considered for the default values.
 */
typedef enum vsf_io_mode_t {
    //! Set the IO to the general-purpose input state.
    //! It is independent of the pull-up, pull-down, and float configurations.
    VSF_IO_INPUT                    = (0 << 0),
    //! Set the IO to analog IO, including analog input and analog output.
    VSF_IO_ANALOG                   = (1 << 0),
    //! Set IO to push-pull output
    VSF_IO_OUTPUT_PUSH_PULL         = (2 << 0),
    //! Set IO to open drain output
    VSF_IO_OUTPUT_OPEN_DRAIN        = (3 << 0),
    //! Set IO to external interrupt
    VSF_IO_EXTI                     = (4 << 0),
    //! Set IO to alternative function.
    VSF_IO_AF                       = (5 << 0),

    // Turn on or off the internal pull-up and pull-down resistors of the IOs
    // It is independent of the direction of the IO and the alternative function
    VSF_IO_NO_PULL_UP_DOWN          = (0 << 4),
    VSF_IO_PULL_UP                  = (1 << 4),
    VSF_IO_PULL_DOWN                = (2 << 4),

    VSF_IO_EXTI_MODE_NONE           = (0 << 6),
    VSF_IO_EXTI_MODE_LOW_LEVEL      = (1 << 6),
    VSF_IO_EXTI_MODE_HIGH_LEVEL     = (2 << 6),
    VSF_IO_EXTI_MODE_RISING         = (3 << 6),
    VSF_IO_EXTI_MODE_FALLING        = (4 << 6),
    VSF_IO_EXTI_MODE_RISING_FALLING = (5 << 6),

/*
    VSF_IO_INVERT_INPUT             = (1 << 7),
    #define VSF_IO_INVERT_INPUT VSF_IO_INVERT_INPUT
*/

/*
    VSF_IO_HIGH_DRIVE_STRENGTH      = (1 << 8),
    #define VSF_IO_HIGH_DRIVE_STRENGTH VSF_IO_HIGH_DRIVE_STRENGTH
*/

/*
    VSF_IO_SPEED_LOW                  = (0 << 9),
    VSF_IO_SPEED_MEDIUM               = (1 << 9),
    VSF_IO_SPEED_HIGH                 = (2 << 9),
    VSF_IO_SPEED_VERY_HIGH            = (3 << 9),
    VSF_IO_SPEED_MASK                 = (3 << 9),

    #define VSF_IO_SPEED_LOW          VSF_IO_SPEED_LOW
    #define VSF_IO_SPEED_MEDIUM       VSF_IO_SPEED_MEDIUM
    #define VSF_IO_SPEED_HIGH         VSF_IO_SPEED_HIGH
    #define VSF_IO_SPEED_VERY_HIGH    VSF_IO_SPEED_VERY_HIGH
    #define VSF_IO_SPEED_MASK         VSF_IO_SPEED_MASK
*/
} vsf_io_mode_t;
#endif

enum {
    VSF_IO_FLOATING                 = VSF_IO_NO_PULL_UP_DOWN,

#ifndef VSF_IO_MODE_MASK
    VSF_IO_MODE_MASK                = VSF_IO_INPUT
                                    | VSF_IO_EXTI
                                    | VSF_IO_ANALOG
                                    | VSF_IO_OUTPUT_PUSH_PULL
                                    | VSF_IO_OUTPUT_OPEN_DRAIN
                                    | VSF_IO_AF,
#endif

    VSF_IO_PULL_UP_DOWN_MASK        = VSF_IO_NO_PULL_UP_DOWN
                                    | VSF_IO_PULL_UP
                                    | VSF_IO_PULL_DOWN,


    VSF_IO_EXTI_MODE_MASK           = VSF_IO_EXTI_MODE_NONE
                                    | VSF_IO_EXTI_MODE_LOW_LEVEL
                                    | VSF_IO_EXTI_MODE_HIGH_LEVEL
                                    | VSF_IO_EXTI_MODE_RISING
                                    | VSF_IO_EXTI_MODE_FALLING
                                    | VSF_IO_EXTI_MODE_RISING_FALLING,

    VSF_IO_MODE_ALL_BITS_MASK       = VSF_IO_MODE_MASK
                                    | VSF_IO_PULL_UP_DOWN_MASK
                                    | VSF_IO_EXTI_MODE_MASK
#ifdef VSF_IO_INVERT_INPUT
                                    | VSF_IO_INVERT_INPUT
#endif
#ifdef VSF_IO_HIGH_DRIVE_STRENGTH
                                    | VSF_IO_HIGH_DRIVE_STRENGTH
#endif
#ifdef VSF_IO_SPEED_MASK
                                    | VSF_IO_SPEED_MASK
#endif
};

#if VSF_HW_IO_PORT_COUNT > 0
typedef enum vsf_io_port_no_t {
#if defined(VSF_IO_CFG_PORTA)
    VSF_PORTA,
#endif
#if defined(VSF_IO_CFG_PORTB)
    VSF_PORTB,
#endif
#if defined(VSF_IO_CFG_PORTC)
    VSF_PORTC,
#endif
#if defined(VSF_IO_CFG_PORTD)
    VSF_PORTD,
#endif
#if defined(VSF_IO_CFG_PORTE)
    VSF_PORTE,
#endif
#if defined(VSF_IO_CFG_PORTF)
    VSF_PORTF,
#endif
#if defined(VSF_IO_CFG_PORTG)
    VSF_PORTG,
#endif
#if defined(VSF_IO_CFG_PORTH)
    VSF_PORTH,
#endif
#if defined(VSF_IO_CFG_PORTI)
    VSF_PORTI,
#endif
#if defined(VSF_IO_CFG_PORTJ)
    VSF_PORTJ,
#endif
#if defined(VSF_IO_CFG_PORTK)
    VSF_PORTK,
#endif
} vsf_io_port_no_t;

typedef enum vsf_io_port_pin_no_t {
#if defined(VSF_IO_CFG_PORTA)
    VSF_MREPEAT(VSF_IO_CFG_PIN_COUNT, __VSF_IO_PORT_PIN_NUM, A)
#endif
#if defined(VSF_IO_CFG_PORTB)
    VSF_MREPEAT(VSF_IO_CFG_PIN_COUNT, __VSF_IO_PORT_PIN_NUM, B)
#endif
#if defined(VSF_IO_CFG_PORTC)
    VSF_MREPEAT(VSF_IO_CFG_PIN_COUNT, __VSF_IO_PORT_PIN_NUM, C)
#endif
#if defined(VSF_IO_CFG_PORTD)
    VSF_MREPEAT(VSF_IO_CFG_PIN_COUNT, __VSF_IO_PORT_PIN_NUM, D)
#endif
#if defined(VSF_IO_CFG_PORTE)
    VSF_MREPEAT(VSF_IO_CFG_PIN_COUNT, __VSF_IO_PORT_PIN_NUM, E)
#endif
#if defined(VSF_IO_CFG_PORTF)
    VSF_MREPEAT(VSF_IO_CFG_PIN_COUNT, __VSF_IO_PORT_PIN_NUM, F)
#endif
#if defined(VSF_IO_CFG_PORTG)
    VSF_MREPEAT(VSF_IO_CFG_PIN_COUNT, __VSF_IO_PORT_PIN_NUM, G)
#endif
#if defined(VSF_IO_CFG_PORTH)
    VSF_MREPEAT(VSF_IO_CFG_PIN_COUNT, __VSF_IO_PORT_PIN_NUM, H)
#endif
#if defined(VSF_IO_CFG_PORTI)
    VSF_MREPEAT(VSF_IO_CFG_PIN_COUNT, __VSF_IO_PORT_PIN_NUM, I)
#endif
#if defined(VSF_IO_CFG_PORTJ)
    VSF_MREPEAT(VSF_IO_CFG_PIN_COUNT, __VSF_IO_PORT_PIN_NUM, J)
#endif
#if defined(VSF_IO_CFG_PORTK)
    VSF_MREPEAT(VSF_IO_CFG_PIN_COUNT, __VSF_IO_PORT_PIN_NUM, K)
#endif
} vsf_io_port_pin_no_t;
#endif

//! io configuration structure
#if VSF_IO_CFG_REIMPLEMENT_TYPE_CFG == DISABLED
typedef struct vsf_io_cfg_t {
    union {
        uint16_t port_pin_index;
        struct {
#if __BYTE_ORDER == __LITTLE_ENDIAN
            uint8_t pin_index;
            uint8_t port_index;
#else
            uint8_t port_index;
            uint8_t pin_index;
#endif
        };
    };
    //! IO Alternative Function Values for alternative functions for different pins
    uint16_t        function;
    //! IO mode
    vsf_io_mode_t   mode;
} vsf_io_cfg_t;
#endif

#if VSF_IO_CFG_REIMPLEMENT_TYPE_CAPABILITY == DISABLED
typedef struct vsf_io_capability_t {
#if VSF_IO_CFG_INHERT_HAL_CAPABILITY == ENABLED
    inherit(vsf_peripheral_capability_t)
#endif
    uint8_t pin_count;
    vsf_io_pin_mask_t pin_mask;
} vsf_io_capability_t;
#endif

typedef struct vsf_io_t vsf_io_t;

typedef struct vsf_io_op_t {
#undef __VSF_HAL_TEMPLATE_API
#define __VSF_HAL_TEMPLATE_API VSF_HAL_TEMPLATE_API_FP

    VSF_IO_APIS(vsf)
} vsf_io_op_t;

#if VSF_IO_CFG_MULTI_CLASS == ENABLED
struct vsf_io_t  {
    const vsf_io_op_t * op;
};
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

/**
 \~english
 @brief Configuring IO
 @param[in] io_ptr: a pointer to structure @ref vsf_io_t
 @param[in] cfg_ptr: structure vsf_io_cfg_t array, @ref vsf_io_t
 @param[in] count: the length of structure vsf_io_cfg_t array
 @return vsf_err_t: VSF_ERR_NONE if io configuration was complete, or a negative error code

 \~chinese
 @brief 配置 IO
 @param[in] io_ptr: 结构体 vsf_io_t 的指针，参考 @ref vsf_io_t
 @param[in] cfg_ptr: 结构体 vsf_io_cfg_t 数组，参考 @ref vsf_io_t
 @param[in] count: 结构体 vsf_io_cfg_t 数组的长度
 @return vsf_err_t: 如果 io 配置完成返回 VSF_ERR_NONE , 否则返回负数
 */
extern vsf_err_t vsf_io_config(vsf_io_t *io_ptr, vsf_io_cfg_t *cfg_ptr, uint_fast8_t count);

/**
 \~english
 @brief Configuring one pin
 @param[in] io_ptr: a pointer to structure @ref vsf_io_t
 @param[in] cfg_ptr: structure vsf_io_cfg_t array, @ref vsf_io_t
 @return vsf_err_t: VSF_ERR_NONE if io configuration was complete, or a negative error code

 \~chinese
 @brief 配置一个 io
 @param[in] io_ptr: 结构体 vsf_io_t 的指针，参考 @ref vsf_io_t
 @param[in] cfg_ptr: 结构体 vsf_io_cfg_t 数组，参考 @ref vsf_io_t
 @return vsf_err_t: 如果 io 配置完成返回 VSF_ERR_NONE , 否则返回负数
 */
extern vsf_err_t vsf_io_config_one_pin(vsf_io_t *io_ptr, vsf_io_cfg_t *cfg_ptr);

/**
 \~english
 @brief get the capability of io instance.
 @param[in] io_ptr: a pointer to structure @ref vsf_io_t
 @return vsf_io_capability_t: return all capability of current io @ref vsf_io_capability_t

 \~chinese
 @brief 获取 io 实例的能力
 @param[in] io_ptr: 结构体 vsf_io_t 的指针，参考 @ref vsf_io_t
 @return vsf_io_capability_t: 返回当前 io 的所有能力 @ref vsf_io_capability_t
 */
extern vsf_io_capability_t vsf_io_capability(vsf_io_t *io_ptr);

/*============================ MACROFIED FUNCTIONS ===========================*/

#ifndef VSF_IO_GET_PORT_NO_FROM_PIN_NO
#   define VSF_IO_GET_PORT_NO_FROM_PIN_NO(__p)      ((vsf_io_port_no_t)(__p / VSF_IO_CFG_PORT_MAX_PIN_NUM))
#endif

#ifndef VSF_IO_GET_REAL_PIN_NO_FROM_PIN_NO
#   define VSF_IO_GET_REAL_PIN_NO_FROM_PIN_NO(__p)  ((vsf_io_port_no_t)(__p % VSF_IO_CFG_PORT_MAX_PIN_NUM))
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ PROTOTYPES ====================================*/
/*============================ MACROS ========================================*/

#if VSF_IO_CFG_FUNCTION_RENAME == ENABLED
#   define __vsf_io_t                                   VSF_MCONNECT(VSF_IO_CFG_PREFIX, _io_t)
#   define vsf_io_config_one_pin                        VSF_MCONNECT(VSF_IO_CFG_PREFIX, _io_config_one_pin)

#   define __vsf_io_config                              VSF_MCONNECT(VSF_IO_CFG_PREFIX, _io_config)
#   define __vsf_io_config_eval3(__IO, __CFGS, __CNT)   __vsf_io_config( (__vsf_io_t *) __IO, __CFGS, __CNT)
#   define __vsf_io_config_eval2(__CFGS, __CNT)         __vsf_io_config( (__vsf_io_t *) &vsf_hw_io, __CFGS, __CNT)
#   define vsf_io_config(...)                           __PLOOC_EVAL(__vsf_io_config_eval, __VA_ARGS__)(__VA_ARGS__)
#   define vsf_io_capability(__IO)                      VSF_MCONNECT(VSF_IO_CFG_PREFIX, _io_capability)((__vsf_io_t *)(__IO))
#endif

// too long, put it end of file
#if VSF_IO_CFG_PIN_MASK & (0x01ul << 0)
#   define VSF_PIN0         0
#   define VSF_PIN0_MASK    (1 << VSF_PIN0)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 1)
#   define VSF_PIN1         1
#   define VSF_PIN1_MASK    (1 << VSF_PIN1)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 2)
#   define VSF_PIN2         2
#   define VSF_PIN2_MASK    (1 << VSF_PIN2)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 3)
#   define VSF_PIN3         3
#   define VSF_PIN3_MASK    (1 << VSF_PIN3)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 4)
#   define VSF_PIN4         4
#   define VSF_PIN4_MASK    (1 << VSF_PIN4)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 5)
#   define VSF_PIN5         5
#   define VSF_PIN5_MASK    (1 << VSF_PIN5)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 6)
#   define VSF_PIN6         6
#   define VSF_PIN6_MASK    (1 << VSF_PIN6)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 7)
#   define VSF_PIN7         7
#   define VSF_PIN7_MASK    (1 << VSF_PIN7)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 8)
#   define VSF_PIN8         8
#   define VSF_PIN8_MASK    (1 << VSF_PIN8)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 9)
#   define VSF_PIN9         9
#   define VSF_PIN9_MASK    (1 << VSF_PIN9)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 10)
#   define VSF_PIN10         10
#   define VSF_PIN10_MASK    (1 << VSF_PIN10)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 11)
#   define VSF_PIN11         11
#   define VSF_PIN11_MASK    (1 << VSF_PIN11)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 12)
#   define VSF_PIN12         12
#   define VSF_PIN12_MASK    (1 << VSF_PIN12)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 13)
#   define VSF_PIN13         13
#   define VSF_PIN13_MASK    (1 << VSF_PIN13)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 14)
#   define VSF_PIN14         14
#   define VSF_PIN14_MASK    (1 << VSF_PIN14)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 15)
#   define VSF_PIN15         15
#   define VSF_PIN15_MASK    (1 << VSF_PIN15)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 16)
#   define VSF_PIN16         16
#   define VSF_PIN16_MASK    (1 << VSF_PIN16)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 17)
#   define VSF_PIN17         17
#   define VSF_PIN17_MASK    (1 << VSF_PIN17)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 18)
#   define VSF_PIN18         18
#   define VSF_PIN18_MASK    (1 << VSF_PIN18)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 19)
#   define VSF_PIN19         19
#   define VSF_PIN19_MASK    (1 << VSF_PIN19)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 20)
#   define VSF_PIN20         20
#   define VSF_PIN20_MASK    (1 << VSF_PIN20)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 21)
#   define VSF_PIN21         21
#   define VSF_PIN21_MASK    (1 << VSF_PIN21)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 22)
#   define VSF_PIN22         22
#   define VSF_PIN22_MASK    (1 << VSF_PIN22)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 23)
#   define VSF_PIN23         23
#   define VSF_PIN23_MASK    (1 << VSF_PIN23)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 24)
#   define VSF_PIN24         24
#   define VSF_PIN24_MASK    (1 << VSF_PIN24)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 25)
#   define VSF_PIN25         25
#   define VSF_PIN25_MASK    (1 << VSF_PIN25)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 26)
#   define VSF_PIN26         26
#   define VSF_PIN26_MASK    (1 << VSF_PIN26)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 27)
#   define VSF_PIN27         27
#   define VSF_PIN27_MASK    (1 << VSF_PIN27)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 28)
#   define VSF_PIN28         28
#   define VSF_PIN28_MASK    (1 << VSF_PIN28)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 29)
#   define VSF_PIN29         29
#   define VSF_PIN29_MASK    (1 << VSF_PIN29)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 30)
#   define VSF_PIN30         30
#   define VSF_PIN30_MASK    (1 << VSF_PIN30)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 31)
#   define VSF_PIN31         31
#   define VSF_PIN31_MASK    (1 << VSF_PIN31)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 32)
#   define VSF_PIN32         32
#   define VSF_PIN32_MASK    (1 << VSF_PIN32)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 33)
#   define VSF_PIN33         33
#   define VSF_PIN33_MASK    (1 << VSF_PIN33)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 34)
#   define VSF_PIN34         34
#   define VSF_PIN34_MASK    (1 << VSF_PIN34)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 35)
#   define VSF_PIN35         35
#   define VSF_PIN35_MASK    (1 << VSF_PIN35)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 36)
#   define VSF_PIN36         36
#   define VSF_PIN36_MASK    (1 << VSF_PIN36)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 37)
#   define VSF_PIN37         37
#   define VSF_PIN37_MASK    (1 << VSF_PIN37)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 38)
#   define VSF_PIN38         38
#   define VSF_PIN38_MASK    (1 << VSF_PIN38)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 39)
#   define VSF_PIN39         39
#   define VSF_PIN39_MASK    (1 << VSF_PIN39)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 40)
#   define VSF_PIN40         40
#   define VSF_PIN40_MASK    (1 << VSF_PIN40)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 41)
#   define VSF_PIN41         41
#   define VSF_PIN41_MASK    (1 << VSF_PIN41)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 42)
#   define VSF_PIN42         42
#   define VSF_PIN42_MASK    (1 << VSF_PIN42)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 43)
#   define VSF_PIN43         43
#   define VSF_PIN43_MASK    (1 << VSF_PIN43)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 44)
#   define VSF_PIN44         44
#   define VSF_PIN44_MASK    (1 << VSF_PIN44)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 45)
#   define VSF_PIN45         45
#   define VSF_PIN45_MASK    (1 << VSF_PIN45)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 46)
#   define VSF_PIN46         46
#   define VSF_PIN46_MASK    (1 << VSF_PIN46)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 47)
#   define VSF_PIN47         47
#   define VSF_PIN47_MASK    (1 << VSF_PIN47)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 48)
#   define VSF_PIN48         48
#   define VSF_PIN48_MASK    (1 << VSF_PIN48)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 49)
#   define VSF_PIN49         49
#   define VSF_PIN49_MASK    (1 << VSF_PIN49)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 50)
#   define VSF_PIN50         50
#   define VSF_PIN50_MASK    (1 << VSF_PIN50)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 51)
#   define VSF_PIN51         51
#   define VSF_PIN51_MASK    (1 << VSF_PIN51)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 52)
#   define VSF_PIN52         52
#   define VSF_PIN52_MASK    (1 << VSF_PIN52)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 53)
#   define VSF_PIN53         53
#   define VSF_PIN53_MASK    (1 << VSF_PIN53)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 54)
#   define VSF_PIN54         54
#   define VSF_PIN54_MASK    (1 << VSF_PIN54)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 55)
#   define VSF_PIN55         55
#   define VSF_PIN55_MASK    (1 << VSF_PIN55)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 56)
#   define VSF_PIN56         56
#   define VSF_PIN56_MASK    (1 << VSF_PIN56)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 57)
#   define VSF_PIN57         57
#   define VSF_PIN57_MASK    (1 << VSF_PIN57)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 58)
#   define VSF_PIN58         58
#   define VSF_PIN58_MASK    (1 << VSF_PIN58)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 59)
#   define VSF_PIN59         59
#   define VSF_PIN59_MASK    (1 << VSF_PIN59)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 60)
#   define VSF_PIN60         60
#   define VSF_PIN60_MASK    (1 << VSF_PIN60)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 61)
#   define VSF_PIN61         61
#   define VSF_PIN61_MASK    (1 << VSF_PIN61)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 62)
#   define VSF_PIN62         62
#   define VSF_PIN62_MASK    (1 << VSF_PIN62)
#endif

#if VSF_IO_CFG_PIN_MASK & (0x01ul << 63)
#   define VSF_PIN63         63
#   define VSF_PIN63_MASK    (1 << VSF_PIN63)
#endif

#ifdef __cplusplus
}
#endif

#endif  /* __VSF_TEMPLATE_IO_H__ */
