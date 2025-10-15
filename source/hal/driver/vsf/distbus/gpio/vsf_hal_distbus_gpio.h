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

#ifndef __VSF_HAL_DISTBUS_GPIO_H__
#define __VSF_HAL_DISTBUS_GPIO_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_GPIO == ENABLED && VSF_HAL_DISTBUS_USE_GPIO == ENABLED

#if     defined(__VSF_HAL_DISTBUS_GPIO_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_HAL_DISTBUS_GPIO_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_HAL_DISTBUS_GPIO_CFG_MULTI_CLASS
#   define VSF_HAL_DISTBUS_GPIO_CFG_MULTI_CLASS     VSF_GPIO_CFG_MULTI_CLASS
#endif

#ifdef VSF_HAL_DISTBUS_AS_REAL_DRIVER
#   define VSF_GPIO_CFG_REIMPLEMENT_TYPE_MODE       ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

#if defined(__VSF_HAL_DISTBUS_GPIO_CLASS_IMPLEMENT) || defined(__VSF_HAL_DISTBUS_GPIO_CLASS_INHERIT__)
enum {
    // commands to slave
    VSF_HAL_DISTBUS_GPIO_CMD_PORT_CONFIG_PINS = 0,
    VSF_HAL_DISTBUS_GPIO_CMD_SET_DIRECTION,
    VSF_HAL_DISTBUS_GPIO_CMD_WRITE,
    VSF_HAL_DISTBUS_GPIO_CMD_SET,
    VSF_HAL_DISTBUS_GPIO_CMD_CLEAR,
    VSF_HAL_DISTBUS_GPIO_CMD_TOGGLE,
    VSF_HAL_DISTBUS_GPIO_CMD_OUTPUT_AND_SET,
    VSF_HAL_DISTBUS_GPIO_CMD_OUTPUT_AND_CLEAR,

    // commands to host
    VSF_HAL_DISTBUS_GPIO_CMD_SYNC,

    VSF_HAL_DISTBUS_GPIO_CMD_ADDR_RANGE,
};

typedef struct vsf_hal_distbus_gpio_port_config_pins_t {
    uint64_t            pin_mask;
    uint32_t            mode;
    uint16_t            alternate_function;
} VSF_CAL_PACKED vsf_hal_distbus_gpio_port_config_pins_t;

typedef struct vsf_hal_distbus_gpio_set_direction_t {
    uint64_t            pin_mask;
    uint64_t            direction_mask;
} VSF_CAL_PACKED vsf_hal_distbus_gpio_set_direction_t;

typedef struct vsf_hal_distbus_gpio_write_t {
    uint64_t            pin_mask;
    uint64_t            value;
} VSF_CAL_PACKED vsf_hal_distbus_gpio_write_t;

typedef struct vsf_hal_distbus_gpio_pin_mask_t {
    uint64_t            pin_mask;
} VSF_CAL_PACKED vsf_hal_distbus_gpio_pin_mask_t;

typedef struct vsf_hal_distbus_gpio_sync_t {
    uint64_t            value;
} VSF_CAL_PACKED vsf_hal_distbus_gpio_sync_t;
#endif

typedef enum VSF_MCONNECT(VSF_HAL_DISTBUS_PREFIX, gpio_mode_t) {
    // mode: 3 bits
    VSF_HAL_DISTBUS_ENUM(VSF_GPIO_INPUT)                    = (0 << 0),
    VSF_HAL_DISTBUS_ENUM(VSF_GPIO_ANALOG)                   = (1 << 0),
    VSF_HAL_DISTBUS_ENUM(VSF_GPIO_OUTPUT_PUSH_PULL)         = (2 << 0),
    VSF_HAL_DISTBUS_ENUM(VSF_GPIO_OUTPUT_OPEN_DRAIN)        = (3 << 0),
    VSF_HAL_DISTBUS_ENUM(VSF_GPIO_EXTI)                     = (4 << 0),
    VSF_HAL_DISTBUS_ENUM(VSF_GPIO_AF)                       = (5 << 0),

    // pull: 2 bits
    VSF_HAL_DISTBUS_ENUM(VSF_GPIO_NO_PULL_UP_DOWN)          = (0 << 3),
    VSF_HAL_DISTBUS_ENUM(VSF_GPIO_PULL_UP)                  = (1 << 3),
    VSF_HAL_DISTBUS_ENUM(VSF_GPIO_PULL_DOWN)                = (2 << 3),

    // exti_mode: 3 bits
    VSF_HAL_DISTBUS_ENUM(VSF_GPIO_EXTI_MODE_NONE)           = (0 << 5),
    VSF_HAL_DISTBUS_ENUM(VSF_GPIO_EXTI_MODE_LOW_LEVEL)      = (1 << 5),
    VSF_HAL_DISTBUS_ENUM(VSF_GPIO_EXTI_MODE_HIGH_LEVEL)     = (2 << 5),
    VSF_HAL_DISTBUS_ENUM(VSF_GPIO_EXTI_MODE_RISING)         = (3 << 5),
    VSF_HAL_DISTBUS_ENUM(VSF_GPIO_EXTI_MODE_FALLING)        = (4 << 5),
    VSF_HAL_DISTBUS_ENUM(VSF_GPIO_EXTI_MODE_RISING_FALLING) = (5 << 5),

    // drive_strength: 2 bits
    VSF_HAL_DISTBUS_ENUM(VSF_GPIO_DRIVE_STRENGTH_LOW)       = (0 << 8),
    VSF_HAL_DISTBUS_ENUM(VSF_GPIO_DRIVE_STRENGTH_MEDIUM)    = (1 << 8),
    VSF_HAL_DISTBUS_ENUM(VSF_GPIO_DRIVE_STRENGTH_HIGH)      = (2 << 8),
    VSF_HAL_DISTBUS_ENUM(VSF_GPIO_DRIVE_STRENGTH_VERY_HIGH) = (3 << 8),
    VSF_HAL_DISTBUS_ENUM(VSF_GPIO_DRIVE_STRENGTH_MASK)      = (3 << 8),

    // speed: 2 bits
    VSF_HAL_DISTBUS_ENUM(VSF_GPIO_SPEED_LOW)                = (0 << 10),
    VSF_HAL_DISTBUS_ENUM(VSF_GPIO_SPEED_MEDIUM)             = (1 << 10),
    VSF_HAL_DISTBUS_ENUM(VSF_GPIO_SPEED_HIGH)               = (2 << 10),
    VSF_HAL_DISTBUS_ENUM(VSF_GPIO_SPEED_VERY_HIGH)          = (3 << 10),
    VSF_HAL_DISTBUS_ENUM(VSF_GPIO_SPEED_MASK)               = (3 << 10),

#ifdef VSF_HAL_DISTBUS_AS_REAL_DRIVER
#   define VSF_GPIO_DRIVE_STRENGTH_LOW                      VSF_GPIO_DRIVE_STRENGTH_LOW
#   define VSF_GPIO_DRIVE_STRENGTH_MEDIUM                   VSF_GPIO_DRIVE_STRENGTH_MEDIUM
#   define VSF_GPIO_DRIVE_STRENGTH_HIGH                     VSF_GPIO_DRIVE_STRENGTH_HIGH
#   define VSF_GPIO_DRIVE_STRENGTH_VERY_HIGH                VSF_GPIO_DRIVE_STRENGTH_VERY_HIGH
#   define VSF_GPIO_DRIVE_STRENGTH_MASK                     VSF_GPIO_DRIVE_STRENGTH_MASK

#   define VSF_GPIO_SPEED_LOW                               VSF_GPIO_SPEED_LOW
#   define VSF_GPIO_SPEED_MEDIUM                            VSF_GPIO_SPEED_MEDIUM
#   define VSF_GPIO_SPEED_HIGH                              VSF_GPIO_SPEED_HIGH
#   define VSF_GPIO_SPEED_VERY_HIGH                         VSF_GPIO_SPEED_VERY_HIGH
#   define VSF_GPIO_SPEED_MASK                              VSF_GPIO_SPEED_MASK
#else
    VSF_HAL_DISTBUS_ENUM(VSF_GPIO_MODE_MASK)                = VSF_HAL_DISTBUS_ENUM(VSF_GPIO_INPUT)
                                                            | VSF_HAL_DISTBUS_ENUM(VSF_GPIO_ANALOG)
                                                            | VSF_HAL_DISTBUS_ENUM(VSF_GPIO_OUTPUT_PUSH_PULL)
                                                            | VSF_HAL_DISTBUS_ENUM(VSF_GPIO_OUTPUT_OPEN_DRAIN)
                                                            | VSF_HAL_DISTBUS_ENUM(VSF_GPIO_EXTI)
                                                            | VSF_HAL_DISTBUS_ENUM(VSF_GPIO_AF),
    VSF_HAL_DISTBUS_ENUM(VSF_GPIO_PULL_UP_DOWN_MASK)        = VSF_HAL_DISTBUS_ENUM(VSF_GPIO_NO_PULL_UP_DOWN)
                                                            | VSF_HAL_DISTBUS_ENUM(VSF_GPIO_PULL_UP)
                                                            | VSF_HAL_DISTBUS_ENUM(VSF_GPIO_PULL_DOWN),
    
    VSF_HAL_DISTBUS_ENUM(VSF_GPIO_EXTI_MODE_MASK)           = VSF_HAL_DISTBUS_ENUM(VSF_GPIO_EXTI_MODE_NONE)
                                                            | VSF_HAL_DISTBUS_ENUM(VSF_GPIO_EXTI_MODE_LOW_LEVEL)
                                                            | VSF_HAL_DISTBUS_ENUM(VSF_GPIO_EXTI_MODE_HIGH_LEVEL)
                                                            | VSF_HAL_DISTBUS_ENUM(VSF_GPIO_EXTI_MODE_RISING)
                                                            | VSF_HAL_DISTBUS_ENUM(VSF_GPIO_EXTI_MODE_FALLING)
                                                            | VSF_HAL_DISTBUS_ENUM(VSF_GPIO_EXTI_MODE_RISING_FALLING),

    VSF_HAL_DISTBUS_ENUM(VSF_GPIO_MODE_ALL_BITS_MASK)       = VSF_HAL_DISTBUS_ENUM(VSF_GPIO_MODE_MASK)
                                                            | VSF_HAL_DISTBUS_ENUM(VSF_GPIO_PULL_UP_DOWN_MASK)
                                                            | VSF_HAL_DISTBUS_ENUM(VSF_GPIO_EXTI_MODE_MASK)
                                                            | VSF_HAL_DISTBUS_ENUM(VSF_GPIO_DRIVE_STRENGTH_MASK)
                                                            | VSF_HAL_DISTBUS_ENUM(VSF_GPIO_SPEED_MASK),
#endif
} VSF_MCONNECT(VSF_HAL_DISTBUS_PREFIX, gpio_mode_t);

typedef struct vsf_hal_distbus_gpio_info_t {
    uint8_t             support_output_and_set;
    uint8_t             support_output_and_clear;
    uint8_t             pin_count;
    uint64_t            pin_mask;

    uint64_t            direction;
    uint64_t            value;
    uint64_t            output_value;
} VSF_CAL_PACKED vsf_hal_distbus_gpio_info_t;

/*============================ INCLUDES ======================================*/

#ifdef VSF_HAL_DISTBUS_AS_REAL_DRIVER
#   include "hal/driver/common/template/vsf_template_gpio.h"
#endif

/*============================ TYPES =========================================*/

vsf_class(vsf_hal_distbus_gpio_t) {
#if VSF_HAL_DISTBUS_GPIO_CFG_MULTI_CLASS == ENABLED
    public_member(
        implement(vsf_gpio_t)
    )
#endif
    protected_member(
        vsf_distbus_service_t               service;
        struct {
            vsf_gpio_exti_isr_handler_t     *handler;
            void                            *target;
            uint32_t                        enabled_mask;
            uint32_t                        triggered_mask;
            uint16_t                        no;
        } irq;
    )
    private_member(
        vsf_distbus_t                       *distbus;
        vsf_hal_distbus_gpio_info_t         info;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

extern uint32_t vsf_hal_distbus_gpio_register_service(vsf_distbus_t *distbus, vsf_hal_distbus_gpio_t *gpio, void *info, uint32_t infolen);

#ifdef __cplusplus
}
#endif

#undef __VSF_HAL_DISTBUS_GPIO_CLASS_IMPLEMENT
#undef __VSF_HAL_DISTBUS_GPIO_CLASS_INHERIT__

#endif
#endif
/* EOF */

