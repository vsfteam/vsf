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

// multi-class support enabled by default for maximum availability.
#ifndef VSF_IO_CFG_MULTI_CLASS
#   define VSF_IO_CFG_MULTI_CLASS                   ENABLED
#endif

// application code can redefine it
#ifndef VSF_IO_CFG_PREFIX
#   if VSF_IO_CFG_MULTI_CLASS == ENABLED
#       define VSF_IO_CFG_PREFIX                    vsf
#   elif defined(VSF_HW_IO_PORT_MAX) && (VSF_HW_IO_PORT_MAX != 0)
#       define VSF_IO_CFG_PREFIX                    vsf_hw
#   else
#       define VSF_IO_CFG_PREFIX                    vsf
#   endif
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
    VSF_P ## __PORT_NUM ## __PIN_NUM = (((uint32_t) VSF_PORT ##__PORT_NUM) << 8) | __PIN_NUM,
#define __VSF_IO_PIN_NUM(__N, __NAME)       __NAME ## __N = __N,
#define __VSF_IO_PIN_MASK(__N, __NAME)      __NAME ## __N ##_MASK = (1ul << (__N)),

#define VSF_IO_APIS(__prefix_name) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t, io, config,         VSF_MCONNECT(__prefix_name, _io_t) *io_ptr, vsf_io_cfg_t *cfg_ptr, uint_fast8_t count) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t, io, config_one_pin, VSF_MCONNECT(__prefix_name, _io_t) *io_ptr, vsf_io_cfg_t *cfg_ptr)

/*============================ TYPES =========================================*/

#if VSF_IO_REIMPLEMENT_FEATURE == DISABLED
typedef enum vsf_io_feature_t {
    VSF_IO_PULL_UP              = (1<<3),           //!< enable pull-up resistor
    VSF_IO_PULL_DOWN            = (1<<4),           //!< enable pull-up resistor
    VSF_IO_OPEN_DRAIN           = (1<<10),          //!< enable open-drain mode

    VSF_IO_DISABLE_INPUT        = (1<<7),           //!< disable input
    VSF_IO_INVERT_INPUT         = (1<<6),           //!< invert the input pin level


    VSF_IO_FILTER_BYPASS        = (0<<11),          //!< filter is bypassed
    VSF_IO_FILTER_2CLK          = (1<<11),          //!< levels should keep 2 clks
    VSF_IO_FILTER_4CLK          = (2<<11),          //!< levels should keep 4 clks
    VSF_IO_FILTER_8CLK          = (3<<11),          //!< levels should keep 8 clks

    VSF_IO_FILTER_CLK_SRC0      = (0<<13),          //!< select clock src 0 for filter
    VSF_IO_FILTER_CLK_SRC1      = (1<<13),          //!< select clock src 1 for filter
    VSF_IO_FILTER_CLK_SRC2      = (2<<13),          //!< select clock src 2 for filter
    VSF_IO_FILTER_CLK_SRC3      = (3<<13),          //!< select clock src 3 for filter
    VSF_IO_FILTER_CLK_SRC4      = (4<<13),          //!< select clock src 4 for filter
    VSF_IO_FILTER_CLK_SRC5      = (5<<13),          //!< select clock src 5 for filter
    VSF_IO_FILTER_CLK_SRC6      = (6<<13),          //!< select clock src 6 for filter
    VSF_IO_FILTER_CLK_SRC7      = (7<<13),          //!< select clock src 7 for filter

    VSF_IO_HIGH_DRV             = (1<<9),           //!< enable high drive strength
    VSF_IO_HIGH_DRIVE           = (1<<9),           //!< enable high drive strength
    VSF_IO_HIGH_DRIVE_STRENGTH  = (1<<9),           //!< enable high drive strength

    VSF_IO_FEATURE_ALL_BITS     = VSF_IO_PULL_UP | VSF_IO_OPEN_DRAIN |
                                  VSF_IO_DISABLE_INPUT | VSF_IO_INVERT_INPUT |
                                  VSF_IO_FILTER_BYPASS | VSF_IO_FILTER_2CLK | VSF_IO_FILTER_4CLK | VSF_IO_FILTER_8CLK |
                                  VSF_IO_FILTER_CLK_SRC0 | VSF_IO_FILTER_CLK_SRC1 | VSF_IO_FILTER_CLK_SRC2 |
                                  VSF_IO_FILTER_CLK_SRC3 | VSF_IO_FILTER_CLK_SRC4 | VSF_IO_FILTER_CLK_SRC5 |
                                  VSF_IO_FILTER_CLK_SRC6 | VSF_IO_FILTER_CLK_SRC7 |
                                  VSF_IO_HIGH_DRV,
} vsf_io_feature_t;
#endif

typedef enum vsf_io_pin_msk_t {
    VSF_MREPEAT(VSF_IO_CFG_PORT_MAX_PIN_NUM, __VSF_IO_PIN_MASK, VSF_PIN)
} vsf_io_pin_msk_t;

typedef enum vsf_io_pin_no_t {
    VSF_MREPEAT(VSF_IO_CFG_PORT_MAX_PIN_NUM, __VSF_IO_PIN_NUM, VSF_PIN)
} vsf_io_pin_no_t;

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
} vsf_io_port_no_t;

typedef enum vsf_io_port_pin_no_t {
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
} vsf_io_port_pin_no_t;

//! io configuration structure
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
    uint16_t        function;                   //!< io Funcitons
    uint32_t        feature;                    //!< pin feature
} vsf_io_cfg_t;

typedef struct vsf_io_capability_t {
    inherit(vsf_peripheral_capability_t)
} vsf_io_capability_t;

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

extern vsf_err_t vsf_io_config(vsf_io_t *io_ptr, vsf_io_cfg_t *cfg_ptr, uint_fast8_t count);
extern vsf_err_t vsf_io_config_one_pin(vsf_io_t *io_ptr, vsf_io_cfg_t *cfg_ptr);

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
#endif

#ifdef __cplusplus
}
#endif

#endif  /* __HAL_DRIVER_IO_INTERFACE_H__ */
