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

#ifndef VSF_GPIO_CFG_MULTI_CLASS
#   define VSF_GPIO_CFG_MULTI_CLASS DISABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_GPIO_CFG_MULTI_CLASS == DISABLED

#   ifndef VSF_GPIO_CFG_PREFIX
#       define VSF_GPIO_CFG_PREFIX   vsf_hw
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


/*! \note device driver should define this macros
 *
//! \brief enable pull-up resistor
#define IOCTRL_ENABLE_PULL_UP(__PIN)                                            \
        do {                                                                    \
            GSP_IOCTRL.PIN[(__PIN)].PE = 1;                                     \
        } while(0)

//! \brief disable pull-up resistor
#define IOCTRL_DISABLE_PULL_UP(__PIN)                                           \
        do {                                                                    \
            GSP_IOCTRL.PIN[(__PIN)].PE = 0;                                     \
        } while(0)

//! \brief enable input
#define IOCTRL_ENABLE_INPUT(__PIN)                                              \
        do {                                                                    \
            GSP_IOCTRL.PIN[(__PIN)].IE = 1;                                     \
        } while(0)

//! \brief disable input
#define IOCTRL_DISABLE_INPUT(__PIN)                                             \
        do {                                                                    \
            GSP_IOCTRL.PIN[(__PIN)].IE = 0;                                     \
        } while(0)

//! \brief enable open-drain-output
#define IOCTRL_ENABLE_OPEN_DRAIN(__PIN)                                         \
        do {                                                                    \
            GSP_IOCTRL.PIN[(__PIN)].ODE = 1;                                    \
        } while(0)

//! \brief disable open-drain-output
#define IOCTRL_DISABLE_OPEN_DRAIN(__PIN)                                        \
        do {                                                                    \
            GSP_IOCTRL.PIN[(__PIN)].ODE = 0;                                    \
        } while(0)

//! \brief invert input
#define IOCTRL_INVERT_INPUT(__PIN)                                              \
        do {                                                                    \
            GSP_IOCTRL.PIN[(__PIN)].II = 1;                                     \
        } while(0)

//! \brief normal input
#define IOCTRL_NORMAL_INPUT(__PIN)                                              \
        do {                                                                    \
            GSP_IOCTRL.PIN[(__PIN)].II = 0;                                     \
        } while(0)

//! \brief invert output
#define IOCTRL_INVERT_OUTPUT(__PIN)                                             \
        do {                                                                    \
            GSP_IOCTRL.PIN[(__PIN)].IO = 1;                                     \
        } while(0)

//! \brief normal output
#define IOCTRL_NORMAL_OUTPUT(__PIN)                                             \
        do {                                                                    \
            GSP_IOCTRL.PIN[(__PIN)].IO = 0;                                     \
        } while(0)


//! \brief function selection
#define IOCTRL_FUNCTION_SELECT(__PIN, __FUNC)                                   \
        do {                                                                    \
            GSP_IOCTRL.PIN[(__PIN)].FSEL = __FUNC;                              \
        } while(0)

//! \brief filter configuration
#define IOCTRL_FILTER_CFG(__PIN, __MODE, __SRC)                                 \
        do {                                                                    \
            uint32_t wValue = GSP_IOCTRL.PIN[(__PIN)].Value;                    \
            wValue &= ~(    IOCTRL_CONTRL_DFCLKSEL_MASK |                       \
                            IOCTRL_CONTRL_DFSMPMOD_MASK );                      \
            wValue |=   IOCTRL_CONTRL_DFCLKSEL(__SRC) |                         \
                        IOCTRL_CONTRL_DFSMPMOD(__MODE);                         \
            GSP_IOCTRL.PIN[(__PIN)].Value = wValue;                             \
        } while(0)

//! \brief enable high drive-strength
#define IOCTRL_ENABLE_HIGH_DRIVER_STRENGH(__PIN)                                \
        do {                                                                    \
            GSP_IOCTRL.PIN[(__PIN)].DS = 1;                                     \
        } while(0)

//! \brief disable high drive-strength
#define IOCTRL_DISABLE_HIGH_DRIVER_STRENGH(__PIN)                               \
        do {                                                                    \
            GSP_IOCTRL.PIN[(__PIN)].DS = 0;                                     \
        } while(0)
*/


/*! \brief following macros are used to define/declare the body of interfaces,
 *!        and initialise gpio interface
 *!          __GPIO_FUNC_DEF()  __GPIO_INTERFACE_DEF()  __GPIO_INTERFACE()
 *!          __GPIO_FUNC_BODY()
 *!
 *!        these macros should be used in gpio.c. E.g.
 *!

        VSF_MREPEAT(GPIO_COUNT, __GPIO_FUNC_DEF, NULL)  // declare the api functions

        //! \brief io interface
        const io_t IO = {
            &vsfhal_gpio_config,                  //!< general io configuration
            {
                {VSF_MREPEAT(GPIO_COUNT, __GPIO_INTERFACE_DEF, NULL)}
            }
        };

        VSF_MREPEAT(GPIO_COUNT, __GPIO_FUNC_BODY, NULL) // api function body

 */

#define __GPIO_FUNC_DEF(__N,__VALUE)                                            \
static void gpio##__N##_set_direction(uint32_t direction, uint32_t pin_mask);   \
static uint32_t    gpio##__N##_get_direction(uint32_t pin_mask);                \
static void        gpio##__N##_set_input(uint32_t pin_mask);                    \
static void        gpio##__N##_set_output(uint32_t pin_mask);                   \
static void        gpio##__N##_switch_direction(uint32_t pin_mask);             \
static uint32_t    gpio##__N##_read(void);                                      \
static void        gpio##__N##_write(uint32_t value, uint32_t pin_mask);        \
static void        gpio##__N##_set(uint32_t pin_mask);                          \
static void        gpio##__N##_clear(uint32_t pin_mask);                        \
static void        gpio##__N##_toggle(uint32_t pin_mask);

#define __GPIO_INTERFACE_DEF(__N, __VALUE)                                      \
            {                                                                   \
                &gpio##__N##_set_direction,                                     \
                &gpio##__N##_get_direction,                                     \
                &gpio##__N##_set_input,                                         \
                &gpio##__N##_set_output,                                        \
                &gpio##__N##_switch_direction,                                  \
                &gpio##__N##_read,                                              \
                &gpio##__N##_write,                                             \
                &gpio##__N##_set,                                               \
                &gpio##__N##_clear,                                             \
                &gpio##__N##_toggle,                                            \
                ( gpio_reg_t *)GPIO##__N##_BASE_ADDRESS,                        \
            },

#define __GPIO_INTERFACE(__N, __VALUE)          const i_gpio_t GPIO##__N;

/*! \NOTE: __GPIO_FUNC_BODY should be modified according to target device
 *!
 *!
#define __GPIO_FUNC_BODY(__N, __VALUE)                                          \
static void gpio##__N##_set_direction(uint32_t diretion, uint32_t pin_mask)     \
{                                                                               \
    vsf_interrupt_safe (                                                        \
        uint32_t temp = GSP_GPIO##__N.DIR & ~pin_mask;                          \
        temp |= (direction & pin_mask);                                         \
        GSP_GPIO##__N.DIR = temp;                                               \
    )                                                                           \
}                                                                               \
static uint32_t gpio##__N##_get_direction(uint32_t pin_mask)                    \
{                                                                               \
    return GSP_GPIO##__N.DIR & pin_mask;                                        \
}                                                                               \
static void gpio##__N##_set_input(uint32_t pin_mask)                            \
{                                                                               \
    GSP_GPIO##__N.DIR &= ~pin_mask;                                             \
}                                                                               \
static void gpio##__N##_set_output(uint32_t pin_mask)                           \
{                                                                               \
    GSP_GPIO##__N.DIR |= pin_mask;                                              \
}                                                                               \
static void gpio##__N##_switch_direction(uint32_t pin_mask)                     \
{                                                                               \
    GSP_GPIO##__N.DIR ^= pin_mask;                                              \
}                                                                               \
static uint32_t gpio##__N##_read(void)                                          \
{                                                                               \
    return GSP_GPIO##__N.IN;                                                    \
}                                                                               \
static void gpio##__N##_write(uint32_t value, uint32_t pin_mask)                \
{                                                                               \
    vsf_interrupt_safe (                                                        \
        uint32_t temp = GSP_GPIO##__N.OUT & ~pin_mask;                          \
        temp |= (value & value);                                                \
        GSP_GPIO##__N.OUT = temp;                                               \
    )                                                                           \
                                                                                \
}                                                                               \
static void gpio##__N##_set(uint32_t pin_mask)                                  \
{                                                                               \
    GSP_GPIO##__N.OUTSET = pin_mask;                                            \
}                                                                               \
static void gpio##__N##_clear(uint32_t pin_mask)                                \
{                                                                               \
    GSP_GPIO##__N.OUTCLR = pin_mask;                                            \
}                                                                               \
static void gpio##__N##_toggle(uint32_t pin_mask)                               \
{                                                                               \
    GSP_GPIO##__N.OUTTOG = pin_mask;                                            \
}
*/

/*============================ TYPES =========================================*/


/*! \note those pin mask should be defined by io.h of target device,
 *        PIO_PORTB_PIN_NUM is defined by device.h of target device,
 *        PIO_PORTx are defined by device.h of target device

#define __IO_PINA_NUM(__N, __OFFSET)        PA##__N = (__OFFSET) + (__N),       \
                                            PA##__N##_idx = (__OFFSET) + (__N)
#define __IO_PINB_NUM(__N, __OFFSET)        PB##__N = (__OFFSET) + (__N),       \
                                            PB##__N##_idx = (__OFFSET) + (__N)
#define __IO_PINC_NUM(__N, __OFFSET)        PC##__N = (__OFFSET) + (__N),       \
                                            PC##__N##_idx = (__OFFSET) + (__N)
#define __IO_PIND_NUM(__N, __OFFSET)        PD##__N = (__OFFSET) + (__N),       \
                                            PD##__N##_idx = (__OFFSET) + (__N)

#define __IO_PINA_MSK(__N, __OFFSET)        PA##__N##_msk = (1ul<<(__N)),
#define __IO_PINB_MSK(__N, __OFFSET)        PB##__N##_msk = (1ul<<(__N)),
#define __IO_PINC_MSK(__N, __OFFSET)        PC##__N##_msk = (1ul<<(__N)),
#define __IO_PIND_MSK(__N, __OFFSET)        PD##__N##_msk = (1ul<<(__N)),

//! \name PIN name
//! @{
enum io_pin_no_t {
#if defined(PIO_PORTA)
    VSF_MREPEAT(PIO_PORTA_PIN_NUM, __IO_PINA_NUM, 0)
#endif
#if defined(PIO_PORTB)
    VSF_MREPEAT(PIO_PORTB_PIN_NUM, __IO_PINB_NUM, 32)
#endif
#if defined(PIO_PORTC)
    VSF_MREPEAT(PIO_PORTC_PIN_NUM, __IO_PINC_NUM, 64)
#endif
#if defined(PIO_PORTD)
    VSF_MREPEAT(PIO_PORTD_PIN_NUM, __IO_PIND_NUM, 96)
#endif
}em_io_pin_no_t;
//! @}

//! \name PIN name
//! @{
enum io_pin_msk_t{
#if defined(PIO_PORTA)
    VSF_MREPEAT(PIO_PORTA_PIN_NUM, __IO_PINA_MSK, 0)
#endif
#if defined(PIO_PORTB)
    VSF_MREPEAT(PIO_PORTB_PIN_NUM, __IO_PINB_MSK, 0)
#endif
#if defined(PIO_PORTC)
    VSF_MREPEAT(PIO_PORTC_PIN_NUM, __IO_PINC_MSK, 0)
#endif
#if defined(PIO_PORTD)
    VSF_MREPEAT(PIO_PORTD_PIN_NUM, __IO_PIND_MSK, 0)
#endif
};
//! @}
*/

/*! \note port name should be defined by io.h of target device
//! \name port name
//! @{
enum io_port_no_t{
#if defined(PIO_PORTA)
    PORTA, PORTA_idx = PORTA,
#endif
#if defined(PIO_PORTB)
    PORTB, PORTB_idx = PORTB,
#endif
#if defined(PIO_PORTC)
    PORTC, PORTC_idx = PORTC,
#endif
#if defined(PIO_PORTD)
    PORTD, PORTD_idx = PORTD,
#endif
};
//! @}
*/

/*! \note em_io_feature_t should be defined by io.h of the target device driver
//! \name IO feature
//! @{
enum io_feature_t{
    IO_PULL_UP              = (1<<4),           //!< enable pull-up resistor
    IO_OPEN_DRAIN           = (1<<10),          //!< enable open-drain mode

    IO_DISABLE_INPUT        = (1<<7),          //!< disable input
    IO_INVERT_INPUT         = (1<<6),          //!< invert the input pin level


    IO_FILTER_BYPASS        = (0<<11),           //!< filter is bypassed
    IO_FILTER_2CLK          = (1<<11),           //!< levels should keep 2 clks
    IO_FILTER_4CLK          = (2<<11),           //!< levels should keep 4 clks
    IO_FILTER_8CLK          = (3<<11),           //!< levels should keep 8 clks

    IO_FILTER_CLK_SRC0      = (0<<13),           //!< select clock src 0 for filter
    IO_FILTER_CLK_SRC1      = (1<<13),           //!< select clock src 1 for filter
    IO_FILTER_CLK_SRC2      = (2<<13),           //!< select clock src 2 for filter
    IO_FILTER_CLK_SRC3      = (3<<13),           //!< select clock src 3 for filter
    IO_FILTER_CLK_SRC4      = (4<<13),           //!< select clock src 4 for filter
    IO_FILTER_CLK_SRC5      = (5<<13),           //!< select clock src 5 for filter
    IO_FILTER_CLK_SRC6      = (6<<13),           //!< select clock src 6 for filter
    IO_FILTER_CLK_SRC7      = (7<<13),           //!< select clock src 7 for filter

    IO_HIGH_DRV             = (1<<9),           //!< enable high drive strength
    IO_HIGH_DRIVE           = (1<<9),           //!< enable high drive strength
    IO_HIGH_DRIVE_STRENGTH  = (1<<9),           //!< enable high drive strength
};
//! @}
*/

#ifndef __cplusplus
typedef enum io_pin_no_t    io_pin_no_t;
typedef enum io_feature_t   io_feature_t;
typedef enum io_port_no_t   io_port_no_t;
typedef enum io_pin_msk_t   io_pin_msk_t;
typedef struct gpio_reg_t   gpio_reg_t;
#endif

//! \name io configuration structure
//! @{
typedef struct {
    io_pin_no_t     pin_index;                  //!< pin index number
    uint16_t        function;                   //!< io Funcitons
    uint32_t        feature;                    //!< pin feature
}io_cfg_t;
//! @}

typedef struct vsf_gpio_t vsf_gpio_t;

#if VSF_GPIO_CFG_MULTI_CLASS == ENABLED
//! \name gpio multiplex
//! @{
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
//! @}

typedef struct vsf_gpio_t  {
    const vsf_gpio_op_t * op;
} vsf_gpio_t;
#endif

// TODO: support c plus plus
#ifndef __cplusplus
//! \name gpio control interface
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

//! \name gpio user interface
//! @{
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
//! @}
#endif

/*============================ GLOBAL VARIABLES ==============================*/

// TODO: support c plus plus
#ifndef __cplusplus
//! \brief io interface
extern const i_io_t VSF_IO;
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*! \brief gpio batch configuration
           an implementation example:

static bool vsf_gpio_config( io_cfg_t *cfg_ptr, uint_fast8_t count )
{
    bool result = true;
    if (NULL == cfg_ptr || 0 == count) {
        return false;
    }

    //! io configure
    do {
        uint_fast8_t pin_index = cfg_ptr->pin_index;  //!< get pin index number
        uint32_t function = cfg_ptr->function;  //!< get pin function selection

        //! get pin feature and make sure pin-input is enabled by default
        //! this is an example to enable some feature to be default.
        uint_fast8_t feature = cfg_ptr->feature ^ IOCTRL_PIN_IE_MSK;

        //! set pin feature: this is the most optimal solution
        GSP_IOCTRL.PIN[pin_index].Value = feature;

        //! but if we are not lucky enough, we can only use the following way
        if (feature & IO_PULL_UP) {
            IOCTRL_ENABLE_PULL_UP(pin_index);
        } else {
            IOCTRL_DISABLE_PULL_UP
        }
        if (feature & IO_HIGH_DRV) {
            IOCTRL_ENABLE_HIGH_DRIVER_STRENGH(pin_index);
        } else {
            IOCTRL_DISABLE_HIGH_DRIVER_STRENGH(pin_index);
        }
        ...

        //! I know this is ugly, but some times, the two methods aforementioned
        //! can be combined. So you should fully use the 32 bit of the
        //! cfg_ptr->feature

        //! set pin function selection
        IOCTRL_FUNCTION_SELECT(pin_index, function);


        cfg_ptr++;                                //!< next one...
    } while(--count);

    return result;
}
 *! \param cfg_ptr the pointer points to configuration array
 *! \param count the count of configurations in the array
 *! \return configuration result
 */
extern vsf_err_t vsf_gpio_config(io_cfg_t *cfg_ptr, uint_fast8_t count);

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
