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

#ifndef __HAL_DRIVER_NUVOTON_M484_IO_H__
#define __HAL_DRIVER_NUVOTON_M484_IO_H__

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"
#include "../../device.h"

/*============================ MACROS ========================================*/

#define __IO_PINA_NUM(__N, __OFFSET)        PA##__N = (__OFFSET) + (__N),       \
                                            PA##__N##_idx = (__OFFSET) + (__N),
#define __IO_PINB_NUM(__N, __OFFSET)        PB##__N = (__OFFSET) + (__N),       \
                                            PB##__N##_idx = (__OFFSET) + (__N),

#define __IO_PINA_MSK(__N, __OFFSET)        PA##__N##_msk = (1ul<<(__N)),
#define __IO_PINB_MSK(__N, __OFFSET)        PB##__N##_msk = (1ul<<(__N)),

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct gpio_reg_t {
    union {
        volatile uint32_t OUT;
        volatile uint32_t IN;
    };
    volatile uint32_t : 32;
    volatile uint32_t DIR;
    volatile uint32_t : 32;
    volatile uint32_t : 32;
    volatile uint32_t : 32;
    volatile uint32_t : 32;
    volatile uint32_t : 32;
    volatile uint32_t : 32;
    volatile uint32_t : 32;
    volatile uint32_t : 32;
    volatile uint32_t : 32;
    volatile uint32_t : 32;
    volatile uint32_t : 32;
    volatile uint32_t : 32;
};
typedef struct gpio_reg_t gpio_reg_t;

struct vsf_gpio_t {
    gpio_reg_t *reg;
};

//! \name PIN name 
//! @{
enum io_pin_no_t {
#if defined(PIO_PORTA)
    VSF_MREPEAT(PIO_PORTA_PIN_NUM, __IO_PINA_NUM, 0)
#endif
#if defined(PIO_PORTB)
    VSF_MREPEAT(PIO_PORTB_PIN_NUM, __IO_PINB_NUM, 16)
#endif
};
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
};
//! @}

//! \name port name
//! @{
enum io_port_no_t{
#if defined(PIO_PORTA)
    PORTA, PORTA_idx = PORTA,
#endif
#if defined(PIO_PORTB)
    PORTB, PORTB_idx = PORTB,
#endif
};
//! @}

//! \name IO model
//! @{
enum io_model_t{
    IO_ANALOG_INPUT         = 0x00,
    IO_INPUT_FLOAT          = 0x00,
    IO_INPUT_PU             = 0x10,
    IO_INPUT_PULLUP         = IO_INPUT_PU,
    IO_INPUT_PD             = 0x30,
    IO_INPUT_PULLDOWN       = IO_INPUT_PD,

    IO_OUTPUT_PP            = 0x01,
    IO_OUTPUT_PUSHPULL      = IO_OUTPUT_PP,
    IO_OUTPUT_OD            = 0x02,
    IO_OUTPUT_OPEN_DRAIN    = IO_OUTPUT_OD,

    IO_BIDIRECTION          = 0x03,


    IO_DISABLE_INPUT        = (1<<8),           //!< disable input
/*
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
*/
};
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ INCLUDES ======================================*/

#include "hal/driver/common/template/vsf_template_io.h"

/*============================ PROTOTYPES ====================================*/

#endif
/* EOF */
