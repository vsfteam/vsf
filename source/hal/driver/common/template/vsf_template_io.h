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

#ifndef __HAL_DRIVER_IO_INTERFACE_H__
#define __HAL_DRIVER_IO_INTERFACE_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_template_hal_driver.h"
#include "hal/arch/vsf_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_IO_REIMPLEMENT_FEATURE
#   define VSF_IO_REIMPLEMENT_FEATURE               DISABLED
#endif

#ifndef VSF_IO_REIMPLEMENT_PORT_NO
#   define VSF_IO_REIMPLEMENT_PORT_NO               DISABLED
#endif

#ifndef VSF_IO_CFG_PORTA
#   define VSF_IO_CFG_PORTA                         ENABLED
#endif

#ifndef VSF_IO_CFG_PORTA_PIN_NUM
#   define VSF_IO_CFG_PORTA_PIN_NUM                 32
#endif

#ifndef VSF_IO_CFG_PORTB
#   define VSF_IO_CFG_PORTB                         ENABLED
#endif

#ifndef VSF_IO_CFG_PORTB_PIN_NUM
#   define VSF_IO_CFG_PORTB_PIN_NUM                 32
#endif

#ifndef VSF_IO_CFG_PORTC
#   define VSF_IO_CFG_PORTC                         ENABLED
#endif

#ifndef VSF_IO_CFG_PORTC_PIN_NUM
#   define VSF_IO_CFG_PORTC_PIN_NUM                 32
#endif

#ifndef VSF_IO_CFG_PORTD
#   define VSF_IO_CFG_PORTD                         ENABLED
#endif

#ifndef VSF_IO_CFG_PORTD_PIN_NUM
#   define VSF_IO_CFG_PORTD_PIN_NUM                 32
#endif

#ifndef VSF_IO_REIMPLEMENT_PIN_MSK
#   define VSF_IO_REIMPLEMENT_PIN_MSK               DISABLED
#endif

#ifndef VSF_IO_REIMPLEMENT_PIN_NUNBER
#   define VSF_IO_REIMPLEMENT_PIN_NUNBER            DISABLED
#endif

#ifndef VSF_GPIO_CFG_MULTI_CLASS
#   define VSF_GPIO_CFG_MULTI_CLASS                 DISABLED
#endif

// Turn off multi class support for the current implementation
// when the VSF_GPIO_CFG_MULTI_CLASS is enabled
#ifndef VSF_GPIO_CFG_IMPLEMENT_OP
#   if VSF_GPIO_CFG_MULTI_CLASS == ENABLED
#       define VSF_GPIO_CFG_IMPLEMENT_OP            ENABLED
#   else
#       define VSF_GPIO_CFG_IMPLEMENT_OP            DISABLED
#   endif
#endif

// VSF_GPIO_CFG_PREFIX: use for macro vsf_gpio_{init, enable, ...}
#ifndef VSF_GPIO_CFG_PREFIX
#   if VSF_GPIO_CFG_MULTI_CLASS == ENABLED
#       define VSF_GPIO_CFG_PREFIX                  vsf
#   elif defined(VSF_HW_GPIO_COUNT) && (VSF_HW_GPIO_COUNT != 0)
#       define VSF_GPIO_CFG_PREFIX                  vsf_hw
#   elif VSF_HAL_USE_GPIO_GPIO == ENABLED
#       define VSF_GPIO_CFG_PREFIX                  vsf_gpio
#   endif
#endif

#ifndef VSF_GPIO_CFG_FUNCTION_RENAME
#   define VSF_GPIO_CFG_FUNCTION_RENAME             ENABLED
#endif

#ifndef VSF_GPIO_CFG_REIMPLEMENT_CAPABILITY
#   define VSF_GPIO_CFG_REIMPLEMENT_CAPABILITY      DISABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define __VSF_IO_PINA_NUM(__N, __OFFSET)    PA ## __N = (__OFFSET) + (__N),       \
                                            PA ## __N ##_idx = (__OFFSET) + (__N),
#define __VSF_IO_PINB_NUM(__N, __OFFSET)    PB ## __N = (__OFFSET) + (__N),       \
                                            PB ## __N ##_idx = (__OFFSET) + (__N),
#define __VSF_IO_PINC_NUM(__N, __OFFSET)    PC ## __N = (__OFFSET) + (__N),       \
                                            PC ## __N ##_idx = (__OFFSET) + (__N),
#define __VSF_IO_PIND_NUM(__N, __OFFSET)    PD ## __N = (__OFFSET) + (__N),       \
                                            PD ## __N ##_idx = (__OFFSET) + (__N),

#define __VSF_IO_PINA_MSK(__N, __OFFSET)    PA ## __N ##_msk = (1ul<<(__N)),
#define __VSF_IO_PINB_MSK(__N, __OFFSET)    PB ## __N ##_msk = (1ul<<(__N)),
#define __VSF_IO_PINC_MSK(__N, __OFFSET)    PC ## __N ##_msk = (1ul<<(__N)),
#define __VSF_IO_PIND_MSK(__N, __OFFSET)    PD ## __N ##_msk = (1ul<<(__N)),

#define VSF_GPIO_APIS(__prefix_name)                                                                                                                                            \
    __VSF_HAL_TEMPLATE_API(__prefix_name, gpio_capability_t, gpio, capability,       VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr)                                              \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,              gpio, config_pin,       VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, uint32_t pin_mask, uint_fast32_t feature)  \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,              gpio, set_direction,    VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, uint32_t direction_mask, uint32_t pin_mask)\
    __VSF_HAL_TEMPLATE_API(__prefix_name, uint32_t,          gpio, get_direction,    VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, uint32_t pin_mask)                         \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,              gpio, set_input,        VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, uint32_t pin_mask)                         \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,              gpio, set_output,       VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, uint32_t pin_mask)                         \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,              gpio, switch_direction, VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, uint32_t pin_mask)                         \
    __VSF_HAL_TEMPLATE_API(__prefix_name, uint32_t,          gpio, read,             VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr)                                            \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,              gpio, write,            VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, uint32_t value, uint32_t pin_mask)         \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,              gpio, set,              VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, uint32_t pin_mask)                         \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,              gpio, clear,            VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, uint32_t pin_mask)                         \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,              gpio, toggle,           VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, uint32_t pin_mask)                         \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,              gpio, output_and_set,   VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, uint32_t pin_mask)                         \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,              gpio, output_and_clear, VSF_MCONNECT(__prefix_name, _gpio_t) *gpio_ptr, uint32_t pin_mask)

/*============================ TYPES =========================================*/

#if VSF_IO_REIMPLEMENT_FEATURE == DISABLED
typedef enum io_feature_t {
    IO_PULL_UP              = (1<<4),           //!< enable pull-up resistor
    IO_OPEN_DRAIN           = (1<<10),          //!< enable open-drain mode

    IO_DISABLE_INPUT        = (1<<7),           //!< disable input
    IO_INVERT_INPUT         = (1<<6),           //!< invert the input pin level


    IO_FILTER_BYPASS        = (0<<11),          //!< filter is bypassed
    IO_FILTER_2CLK          = (1<<11),          //!< levels should keep 2 clks
    IO_FILTER_4CLK          = (2<<11),          //!< levels should keep 4 clks
    IO_FILTER_8CLK          = (3<<11),          //!< levels should keep 8 clks

    IO_FILTER_CLK_SRC0      = (0<<13),          //!< select clock src 0 for filter
    IO_FILTER_CLK_SRC1      = (1<<13),          //!< select clock src 1 for filter
    IO_FILTER_CLK_SRC2      = (2<<13),          //!< select clock src 2 for filter
    IO_FILTER_CLK_SRC3      = (3<<13),          //!< select clock src 3 for filter
    IO_FILTER_CLK_SRC4      = (4<<13),          //!< select clock src 4 for filter
    IO_FILTER_CLK_SRC5      = (5<<13),          //!< select clock src 5 for filter
    IO_FILTER_CLK_SRC6      = (6<<13),          //!< select clock src 6 for filter
    IO_FILTER_CLK_SRC7      = (7<<13),          //!< select clock src 7 for filter

    IO_HIGH_DRV             = (1<<9),           //!< enable high drive strength
    IO_HIGH_DRIVE           = (1<<9),           //!< enable high drive strength
    IO_HIGH_DRIVE_STRENGTH  = (1<<9),           //!< enable high drive strength
} io_feature_t;
#endif

#if VSF_IO_REIMPLEMENT_PORT_NO == DISABLED
typedef enum io_port_no_t {
#if defined(VSF_IO_CFG_PORTA)
    PORTA, PORTA_idx = PORTA,
#endif
#if defined(VSF_IO_CFG_PORTB)
    PORTB, PORTB_idx = PORTB,
#endif
#if defined(VSF_IO_CFG_PORTC)
    PORTC, PORTC_idx = PORTC,
#endif
#if defined(VSF_IO_CFG_PORTD)
    PORTD, PORTD_idx = PORTD,
#endif
} io_port_no_t;
#endif

#if VSF_IO_REIMPLEMENT_PIN_MSK == DISABLED
typedef enum io_pin_msk_t {
#if defined(VSF_IO_CFG_PORTA)
    VSF_MREPEAT(VSF_IO_CFG_PORTA_PIN_NUM, __VSF_IO_PINA_MSK, 0)
#endif
#if defined(VSF_IO_CFG_PORTB)
    VSF_MREPEAT(VSF_IO_CFG_PORTB_PIN_NUM, __VSF_IO_PINB_MSK, 0)
#endif
#if defined(VSF_IO_CFG_PORTC)
    VSF_MREPEAT(VSF_IO_CFG_PORTC_PIN_NUM, __VSF_IO_PINC_MSK, 0)
#endif
#if defined(VSF_IO_CFG_PORTD)
    VSF_MREPEAT(VSF_IO_CFG_PORTD_PIN_NUM, __VSF_IO_PIND_MSK, 0)
#endif
} io_pin_msk_t;
#endif

#if VSF_IO_REIMPLEMENT_PIN_NUNBER == DISABLED
typedef enum io_pin_no_t {
#if defined(VSF_IO_CFG_PORTA)
    VSF_MREPEAT(VSF_IO_CFG_PORTA_PIN_NUM, __VSF_IO_PINA_NUM, 0)
#endif
#if defined(VSF_IO_CFG_PORTB)
    VSF_MREPEAT(VSF_IO_CFG_PORTB_PIN_NUM, __VSF_IO_PINB_NUM, 32)
#endif
#if defined(VSF_IO_CFG_PORTC)
    VSF_MREPEAT(VSF_IO_CFG_PORTC_PIN_NUM, __VSF_IO_PINC_NUM, 64)
#endif
#if defined(VSF_IO_CFG_PORTD)
    VSF_MREPEAT(VSF_IO_CFG_PORTD_PIN_NUM, __VSF_IO_PIND_NUM, 96)
#endif
} io_pin_no_t;
#endif

//! io configuration structure
typedef struct io_cfg_t {
    io_pin_no_t     pin_index;                  //!< pin index number
    uint16_t        function;                   //!< io Funcitons
    uint32_t        feature;                    //!< pin feature
} io_cfg_t;


#if VSF_GPIO_CFG_REIMPLEMENT_CAPABILITY == DISABLED
typedef struct gpio_capability_t {
    inherit(peripheral_capability_t)
} gpio_capability_t;
#endif

typedef struct gpio_reg_t gpio_reg_t;

typedef struct vsf_gpio_t vsf_gpio_t;

typedef struct vsf_gpio_op_t {
#undef __VSF_HAL_TEMPLATE_API
#define __VSF_HAL_TEMPLATE_API VSF_HAL_TEMPLATE_API_FP

    VSF_GPIO_APIS(vsf)
} vsf_gpio_op_t;

#if VSF_GPIO_CFG_MULTI_CLASS == ENABLED
struct vsf_gpio_t  {
    const vsf_gpio_op_t * op;
};
#endif


//! gpio control interface
//! @{
dcl_interface(i_gpio_t)
def_interface(i_gpio_t)

    //! config pin mode
    void            (*ConfigPin)        (uint32_t pin_mask,
                                         uint_fast32_t feature);

    //! set pin directions with pin-mask
    void            (*SetDirection)     (uint32_t dir_bitmap,
                                         uint32_t pin_mask);
    //! get pin direction with pin-mask
    uint32_t        (*GetDirection)     (uint32_t pin_mask);
    //! Set specified pin direction to input
    void            (*SetInput)         (uint32_t pin_mask);
    //! Set specified pin direction to output
    void            (*SetOutput)        (uint32_t pin_mask);
    //! Switch specified pin direction
    void            (*SwitchDirection)  (uint32_t pin_mask);
    //! get pin value on specified port
    uint32_t        (*Read)             (void);
    //! write pin value with pin-mask
    void            (*Write)            (uint32_t value, uint32_t pin_mask);
    //! set specified pins
    void            (*Set)              (uint32_t pin_mask);
    //! clear specified pins
    void            (*Clear)            (uint32_t pin_mask);
    //! toggle specified pins
    void            (*Toggle)           (uint32_t pin_mask);
    //! get base address of specified port
    gpio_reg_t *const reg_ptr;

end_def_interface(i_gpio_t)
//! @}

#if 0
//! gpio user interface
dcl_interface(i_io_t)
def_interface(i_io_t)
    //! general io configuration
    vsf_err_t (*Config)(io_cfg_t *cfg_ptr, uint_fast8_t count);
    union {
        i_gpio_t  PORT[GPIO_COUNT];         //!< dedicated gpio control interface
        struct {
            VSF_MREPEAT(GPIO_COUNT, __GPIO_INTERFACE, NULL)
        };
    };
end_def_interface(i_io_t)
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#if 0
//! \brief io interface
extern const i_io_t VSF_IO;
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ PROTOTYPES ====================================*/

extern void vsf_gpio_config_pin(vsf_gpio_t *gpio_ptr,
                                uint32_t pin_mask,
                                uint_fast32_t feature);

/*! \brief set the specified pins to corresponding directions
 *!        pseudo code:
 *!        uint32_t temp = VSF_GPIOx.DIR;
 *!        temp &= ~pin_mask;
 *!        VSF_GPIOx.DIR = temp | (direction_bitmap & pin_mask);
 *!
 *! \param gpio_ptr the address of target vsf_gpio_t object (given port)
 *! \param direction_bitmap direction bitmap which will be ANDDed with pin_mask
 *!          direction value is 1 for output
 *!          direction value is 0 for input
 *! \param pin_mask pin mask to mark the target pin within a given port
 *! \return none
 */
extern void vsf_gpio_set_direction( vsf_gpio_t *gpio_ptr,
                                    uint32_t direction_mask,
                                    uint32_t pin_mask);

extern uint32_t vsf_gpio_get_direction(vsf_gpio_t *gpio_ptr,
                                       uint32_t pin_mask);

extern void vsf_gpio_set_input(vsf_gpio_t *gpio_ptr, uint32_t pin_mask);

extern void vsf_gpio_set_output(vsf_gpio_t *gpio_ptr, uint32_t pin_mask);

extern void vsf_gpio_switch_direction(vsf_gpio_t *gpio_ptr, uint32_t pin_mask);

extern uint32_t vsf_gpio_read(vsf_gpio_t *gpio_ptr);

/*! \brief set the specified pins to corresponding value
 *!        pseudo code:
 *!        VSF_GPIOx.OUT &= ~pin_mask;
 *!        VSF_GPIOx.OUT |= (value & pin_mask);
 *!
 *! \param gpio_ptr the address of target vsf_gpio_t object (given port)
 *! \param value value bitmap which will be ANDDed with pin_mask
 *! \param pin_mask pin mask to mark the target pin within a given port
 *! \return none
 */
extern void vsf_gpio_write( vsf_gpio_t *gpio_ptr,
                                uint32_t value,
                                uint32_t pin_mask);

extern void vsf_gpio_set(vsf_gpio_t *gpio_ptr, uint32_t pin_mask);

extern void vsf_gpio_clear(vsf_gpio_t *gpio_ptr, uint32_t pin_mask);

extern void vsf_gpio_output_and_set(vsf_gpio_t *gpio_ptr, uint32_t pin_mask);

extern void vsf_gpio_output_and_clear(vsf_gpio_t *gpio_ptr, uint32_t pin_mask);

extern void vsf_gpio_toggle(vsf_gpio_t *gpio_ptr, uint32_t pin_mask);

/*============================ MACROS ========================================*/

#if VSF_GPIO_CFG_FUNCTION_RENAME == ENABLED
#   define vsf_gpio_config_pin(__GPIO, ...)                                     \
        VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_config_pin)         ((VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_t) *)__GPIO, ##__VA_ARGS__)
#   define vsf_gpio_set_direction(__GPIO, ...)                                  \
        VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_set_direction)      ((VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_t) *)__GPIO, ##__VA_ARGS__)
#   define vsf_gpio_get_direction(__GPIO, ...)                                  \
        VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_get_direction)      ((VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_t) *)__GPIO, ##__VA_ARGS__)
#   define vsf_gpio_set_input(__GPIO, ...)                                      \
        VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_set_input)          ((VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_t) *)__GPIO, ##__VA_ARGS__)
#   define vsf_gpio_set_output(__GPIO, ...)                                     \
        VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_set_output)         ((VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_t) *)__GPIO, ##__VA_ARGS__)
#   define vsf_gpio_switch_direction(__GPIO, ...)                               \
        VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_switch_direction)   ((VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_t) *)__GPIO, ##__VA_ARGS__)
#   define vsf_gpio_read(__GPIO)                                                \
        VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_read)               ((VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_t) *)__GPIO)
#   define vsf_gpio_write(__GPIO, __VALUE, ...)                                 \
        VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_write)              ((VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_t) *)__GPIO, ##__VA_ARGS__)
#   define vsf_gpio_set(__GPIO, ...)                                            \
        VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_set)                ((VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_t) *)__GPIO, ##__VA_ARGS__)
#   define vsf_gpio_clear(__GPIO, ...)                                          \
        VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_clear)              ((VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_t) *)__GPIO, ##__VA_ARGS__)
#   define vsf_gpio_output_and_set(__GPIO, ...)                                 \
        VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_output_and_set)     ((VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_t) *)__GPIO, ##__VA_ARGS__)
#   define vsf_gpio_output_and_clear(__GPIO, ...)                               \
        VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_output_and_clear)   ((VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_t) *)__GPIO, ##__VA_ARGS__)
#   define vsf_gpio_toggle(__GPIO, ...)                                         \
        VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_toggle)             ((VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_t) *)__GPIO, ##__VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif  /* __HAL_DRIVER_IO_INTERFACE_H__ */
