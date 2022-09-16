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

// application code can redefine it
#ifndef VSF_IO_CFG_PREFIX
#   if defined(VSF_HW_IO_COUNT) && (VSF_HW_IO_COUNT != 0)
#       define VSF_IO_CFG_PREFIX                    vsf_hw
#   else
#       define VSF_IO_CFG_PREFIX                    vsf
#   endif
#endif

// multi-class support enabled by default for maximum availability.
#ifndef VSF_IO_CFG_MULTI_CLASS
#   define VSF_IO_CFG_MULTI_CLASS                   ENABLED
#endif

#ifndef VSF_IO_CFG_FUNCTION_RENAME
#   define VSF_IO_CFG_FUNCTION_RENAME               ENABLED
#endif

#ifndef VSF_IO_REIMPLEMENT_FEATURE
#   define VSF_IO_REIMPLEMENT_FEATURE               DISABLED
#endif

#ifndef VSF_IO_CFG_PORTA
#   define VSF_IO_CFG_PORTA                         ENABLED
#endif

#ifndef VSF_IO_CFG_PORTB
#   define VSF_IO_CFG_PORTB                         ENABLED
#endif

#ifndef VSF_IO_CFG_PORTC
#   define VSF_IO_CFG_PORTC                         ENABLED
#endif

#ifndef VSF_IO_CFG_PORTD
#   define VSF_IO_CFG_PORTD                         ENABLED
#endif

#ifndef VSF_IO_CFG_PORT_MAX_PIN_NUM
#   define VSF_IO_CFG_PORT_MAX_PIN_NUM              32
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
#define __VSF_IO_PORT_PIN_NUM(__PIN_NUM, __PORT_NUM)                            \
    VSF_P ## __PORT_NUM ## __PIN_NUM = (((uint32_t) PORT ##__PORT_NUM) << 16) | __PIN_NUM,
#define __VSF_IO_PIN_NUM(__N, __NAME)       __NAME ## __N = __N,
#define __VSF_IO_PIN_MSK(__N, __NAME)       __NAME ## __N ##_MSK = (1ul << (__N)),

#define VSF_IO_APIS(__prefix_name) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t, io, config,         VSF_MCONNECT(__prefix_name, _io_t) *io_ptr, io_cfg_t *cfg_ptr, uint_fast8_t count) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t, io, config_one_pin, VSF_MCONNECT(__prefix_name, _io_t) *io_ptr, io_cfg_t *cfg_ptr)

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

    IO_FEATURE_ALL_BITS     = IO_PULL_UP | IO_OPEN_DRAIN |
                              IO_DISABLE_INPUT | IO_INVERT_INPUT |
                              IO_FILTER_BYPASS | IO_FILTER_2CLK | IO_FILTER_4CLK | IO_FILTER_8CLK |
                              IO_FILTER_CLK_SRC0 | IO_FILTER_CLK_SRC1 | IO_FILTER_CLK_SRC2 |
                              IO_FILTER_CLK_SRC3 | IO_FILTER_CLK_SRC4 | IO_FILTER_CLK_SRC5 |
                              IO_FILTER_CLK_SRC6 | IO_FILTER_CLK_SRC7 |
                              IO_HIGH_DRV,
} io_feature_t;
#endif

typedef enum io_pin_msk_t {
    VSF_MREPEAT(VSF_IO_CFG_PORT_MAX_PIN_NUM, __VSF_IO_PIN_MSK, PIN)
} io_pin_msk_t;

typedef enum io_pin_no_t {
    VSF_MREPEAT(VSF_IO_CFG_PORT_MAX_PIN_NUM, __VSF_IO_PIN_NUM, PIN)
} io_pin_no_t;

typedef enum io_port_no_t {
#if defined(VSF_IO_CFG_PORTA)
    PORTA,
#endif
#if defined(VSF_IO_CFG_PORTB)
    PORTB,
#endif
#if defined(VSF_IO_CFG_PORTC)
    PORTC,
#endif
#if defined(VSF_IO_CFG_PORTD)
    PORTD,
#endif
} io_port_no_t;

typedef enum io_port_pin_no_t {
#if defined(VSF_IO_CFG_PORTA)
    VSF_MREPEAT(VSF_IO_CFG_PORT_MAX_PIN_NUM, __VSF_IO_PORT_PIN_NUM, A)
#endif
#if defined(VSF_IO_CFG_PORTB)
    VSF_MREPEAT(VSF_IO_CFG_PORT_MAX_PIN_NUM, __VSF_IO_PORT_PIN_NUM, B)
#endif
#if defined(VSF_IO_CFG_PORTC)
    VSF_MREPEAT(VSF_IO_CFG_PORT_MAX_PIN_NUM, __VSF_IO_PORT_PIN_NUM, C)
#endif
#if defined(VSF_IO_CFG_PORTD)
    VSF_MREPEAT(VSF_IO_CFG_PORT_MAX_PIN_NUM, __VSF_IO_PORT_PIN_NUM, D)
#endif
} io_port_pin_no_t;

//! io configuration structure
typedef struct io_cfg_t {
    union {
        uint32_t port_pin_index;
        struct {
#if __BYTE_ORDER == __LITTLE_ENDIAN
            uint16_t pin_index;
            uint16_t port_index;
#else
            uint16_t port_index;
            uint16_t pin_index;
#endif
        };
    };
    uint16_t        function;                   //!< io Funcitons
    uint32_t        feature;                    //!< pin feature
} io_cfg_t;

#if VSF_IO_CFG_REIMPLEMENT_CAPABILITY == DISABLED
typedef struct vsf_io_capability_t {
    inherit(peripheral_capability_t)
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

#ifndef VSF_IO_GET_PORT_NO_FROM_PIN_NO
#   define VSF_IO_GET_PORT_NO_FROM_PIN_NO(__p)      ((io_port_no_t)(__p / VSF_IO_CFG_PORT_MAX_PIN_NUM))
#endif

#ifndef VSF_IO_GET_REAL_PIN_NO_FROM_PIN_NO
#   define VSF_IO_GET_REAL_PIN_NO_FROM_PIN_NO(__p)  ((io_port_no_t)(__p % VSF_IO_CFG_PORT_MAX_PIN_NUM))
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ PROTOTYPES ====================================*/
/*============================ MACROS ========================================*/

#if VSF_IO_CFG_FUNCTION_RENAME == ENABLED
#   define vsf_io_config_one_pin(__IO, ...)                                     \
        VSF_MCONNECT(VSF_IO_CFG_PREFIX, _io_config_one_pin) ((VSF_MCONNECT(VSF_IO_CFG_PREFIX, _io_t) *)__IO, ##__VA_ARGS__)
#   define vsf_io_config(__IO, ...)                                             \
        VSF_MCONNECT(VSF_IO_CFG_PREFIX, _io_config)         ((VSF_MCONNECT(VSF_IO_CFG_PREFIX, _io_t) *)__IO, ##__VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif  /* __HAL_DRIVER_IO_INTERFACE_H__ */
