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


#if VSF_GPIO_CFG_MULTI_CLASS == DISABLED

#   ifndef VSF_GPIO_CFG_PREFIX
#       define VSF_GPIO_CFG_PREFIX      vsf_hw
#   endif

#   define ____VSF_GPIO_WRAPPER(__header, __api)    __header ## _ ## __api
#   define __VSF_GPIO_WRAPPER(__header, __api)      ____VSF_GPIO_WRAPPER(__header, __api)
#   define vsf_gpio_config_pin       __VSF_GPIO_WRAPPER(VSF_GPIO_CFG_PREFIX, gpio_config_pin)
#   define vsf_gpio_set_direction    __VSF_GPIO_WRAPPER(VSF_GPIO_CFG_PREFIX, gpio_set_direction)
#   define vsf_gpio_get_direction    __VSF_GPIO_WRAPPER(VSF_GPIO_CFG_PREFIX, gpio_get_direction)
#   define vsf_gpio_set_input        __VSF_GPIO_WRAPPER(VSF_GPIO_CFG_PREFIX, gpio_set_input)
#   define vsf_gpio_set_output       __VSF_GPIO_WRAPPER(VSF_GPIO_CFG_PREFIX, gpio_set_output)
#   define vsf_gpio_switch_direction __VSF_GPIO_WRAPPER(VSF_GPIO_CFG_PREFIX, gpio_switch_direction)
#   define vsf_gpio_read             __VSF_GPIO_WRAPPER(VSF_GPIO_CFG_PREFIX, gpio_read)
#   define vsf_gpio_write            __VSF_GPIO_WRAPPER(VSF_GPIO_CFG_PREFIX, gpio_write)
#   define vsf_gpio_set              __VSF_GPIO_WRAPPER(VSF_GPIO_CFG_PREFIX, gpio_set)
#   define vsf_gpio_clear            __VSF_GPIO_WRAPPER(VSF_GPIO_CFG_PREFIX, gpio_clear)
#   define vsf_gpio_output_and_set   __VSF_GPIO_WRAPPER(VSF_GPIO_CFG_PREFIX, gpio_output_and_set)
#   define vsf_gpio_output_and_clear __VSF_GPIO_WRAPPER(VSF_GPIO_CFG_PREFIX, gpio_output_and_clear)
#   define vsf_gpio_toggle           __VSF_GPIO_WRAPPER(VSF_GPIO_CFG_PREFIX, gpio_toggle)
#endif

#define VSF_GPIO_CONFIG_PIN(__GPIO, __PIN_MASK, __FEATURE)                      \
    vsf_gpio_config_pin((vsf_gpio_t *)__GPIO, __PIN_MASK, __FEATURE)
#define VSF_GPIO_SET_DIRECTION(__GPIO, DIRECTION_MASK, __PIN_MASK)              \
    vsf_gpio_set_direction((vsf_gpio_t *)__GPIO, DIRECTION_MASK, __PIN_MASK)
#define VSF_GPIO_GET_DIRECTION(__GPIO, __PIN_MASK)                              \
    vsf_gpio_get_direction((vsf_gpio_t *)__GPIO, __PIN_MASK)
#define VSF_GPIO_SET_INPUT(__GPIO, __PIN_MASK)                                  \
    vsf_gpio_set_input((vsf_gpio_t *)__GPIO, __PIN_MASK)
#define VSF_GPIO_SET_OUTPUT(__GPIO, __PIN_MASK)                                 \
    vsf_gpio_set_output((vsf_gpio_t *)__GPIO, __PIN_MASK)
#define VSF_GPIO_SWITCH_DIRECTION(__GPIO, __PIN_MASK)                           \
    vsf_gpio_switch_direction((vsf_gpio_t *)__GPIO, __PIN_MASK)
#define VSF_GPIO_READ(__GPIO)                                                   \
    vsf_gpio_read((vsf_gpio_t *)__GPIO)
#define VSF_GPIO_WRITE(__GPIO, __VALUE, __PIN_MASK)                             \
    vsf_gpio_write((vsf_gpio_t *)__GPIO, __VALUE, __PIN_MASK)
#define VSF_GPIO_SET(__GPIO, __PIN_MASK)                                        \
    vsf_gpio_set((vsf_gpio_t *)__GPIO, __PIN_MASK)
#define VSF_GPIO_CLEAR(__GPIO, __PIN_MASK)                                      \
    vsf_gpio_clear((vsf_gpio_t *)__GPIO, __PIN_MASK)
#define VSF_GPIO_OUTPUT_AND_SET(__GPIO, __PIN_MASK)                             \
    vsf_gpio_output_and_set((vsf_gpio_t *)__GPIO, __PIN_MASK)
#define VSF_GPIO_OUTPUT_AND_CLEAR(__GPIO, __PIN_MASK)                           \
    vsf_gpio_output_and_clear((vsf_gpio_t *)__GPIO, __PIN_MASK)
#define VSF_GPIO_TOGGLE(__GPIO, __PIN_MASK)                                     \
    vsf_gpio_toggle((vsf_gpio_t *)__GPIO, __PIN_MASK)

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
typedef struct io_cfg_t{
    io_pin_no_t     pin_index;                  //!< pin index number
    uint16_t        function;                   //!< io Funcitons
    uint32_t        feature;                    //!< pin feature
} io_cfg_t;

typedef struct gpio_reg_t   gpio_reg_t;

typedef struct vsf_gpio_t vsf_gpio_t;

typedef struct vsf_gpio_op_t {
    void            (*config_pin)       (vsf_gpio_t * gpio_ptr, uint32_t pin_mask, uint_fast32_t feature);
    void            (*set_direction)    (vsf_gpio_t * gpio_ptr, uint32_t dir_bitmap, uint32_t pin_mask);
    uint32_t        (*get_direction)    (vsf_gpio_t * gpio_ptr, uint32_t pin_mask);
    void            (*set_input)        (vsf_gpio_t * gpio_ptr, uint32_t pin_mask);
    void            (*set_output)       (vsf_gpio_t * gpio_ptr, uint32_t pin_mask);
    void            (*switch_direction) (vsf_gpio_t * gpio_ptr, uint32_t pin_mask);
    uint32_t        (*read)             (vsf_gpio_t * gpio_ptr);
    void            (*write)            (vsf_gpio_t * gpio_ptr, uint32_t value, uint32_t pin_mask);
    void            (*set)              (vsf_gpio_t * gpio_ptr, uint32_t pin_mask);
    void            (*clear)            (vsf_gpio_t * gpio_ptr, uint32_t pin_mask);
    void            (*toggle)           (vsf_gpio_t * gpio_ptr, uint32_t pin_mask);
    void            (*output_and_set)   (vsf_gpio_t * gpio_ptr, uint32_t pin_mask);
    void            (*output_and_clear) (vsf_gpio_t * gpio_ptr, uint32_t pin_mask);
} vsf_gpio_op_t;

#if VSF_GPIO_CFG_MULTI_CLASS == ENABLED
typedef struct vsf_gpio_t  {
    const vsf_gpio_op_t * op;
} vsf_gpio_t;
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

#ifdef VSF_GPIO_CFG_TEMPLATE_COUNT
#   ifndef VSF_GPIO_CFG_TEMPLATE_MASK
#       define VSF_GPIO_CFG_TEMPLATE_MASK   ((1ul << VSF_GPIO_CFG_TEMPLATE_COUNT) - 1)
#   endif

#   if VSF_GPIO_CFG_TEMPLATE_MASK & (1 << 0)
        VSF_GPIO_CFG_DEC_LV0(0, NULL)
#   endif
#   if VSF_GPIO_CFG_TEMPLATE_MASK & (1 << 1)
        VSF_GPIO_CFG_DEC_LV0(1, NULL)
#   endif
#   if VSF_GPIO_CFG_TEMPLATE_MASK & (1 << 2)
        VSF_GPIO_CFG_DEC_LV0(2, NULL)
#   endif
#   if VSF_GPIO_CFG_TEMPLATE_MASK & (1 << 3)
        VSF_GPIO_CFG_DEC_LV0(3, NULL)
#   endif
#   if VSF_GPIO_CFG_TEMPLATE_MASK & (1 << 4)
        VSF_GPIO_CFG_DEC_LV0(4, NULL)
#   endif
#   if VSF_GPIO_CFG_TEMPLATE_MASK & (1 << 5)
        VSF_GPIO_CFG_DEC_LV0(5, NULL)
#   endif
#   if VSF_GPIO_CFG_TEMPLATE_MASK & (1 << 6)
        VSF_GPIO_CFG_DEC_LV0(6, NULL)
#   endif
#   if VSF_GPIO_CFG_TEMPLATE_MASK & (1 << 7)
        VSF_GPIO_CFG_DEC_LV0(7, NULL)
#   endif

#   undef VSF_GPIO_CFG_TEMPLATE_COUNT
#   undef VSF_GPIO_CFG_TEMPLATE_MASK
#   undef VSF_GPIO_CFG_DEC_LV0
#endif

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

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
