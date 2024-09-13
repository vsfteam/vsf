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

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

#if defined(__VSF_HAL_DISTBUS_GPIO_CLASS_IMPLEMENT) || defined(__VSF_HAL_DISTBUS_GPIO_CLASS_INHERIT__)
enum {
    // commands to slave
    VSF_HAL_DISTBUS_GPIO_CMD_CONFIG_PIN = 0,
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

typedef enum vsf_hal_distbus_gpio_mode_t {
    // TODO: add more mode
    VSF_HAL_DISTBUS_GPIO_MODE_DUMMY,
} vsf_hal_distbus_gpio_mode_t;
typedef struct vsf_hal_distbus_gpio_port_config_pins_t {
    vsf_gpio_pin_mask_t pin_mask;
    vsf_gpio_mode_t     mode;
    uint16_t            alternate_function;
} VSF_CAL_PACKED vsf_hal_distbus_gpio_port_config_pins_t;

typedef struct vsf_hal_distbus_gpio_set_direction_t {
    vsf_gpio_pin_mask_t pin_mask;
    vsf_gpio_pin_mask_t direction_mask;
} VSF_CAL_PACKED vsf_hal_distbus_gpio_set_direction_t;

typedef struct vsf_hal_distbus_gpio_write_t {
    vsf_gpio_pin_mask_t pin_mask;
    vsf_gpio_pin_mask_t value;
} VSF_CAL_PACKED vsf_hal_distbus_gpio_write_t;

typedef struct vsf_hal_distbus_gpio_pin_mask_t {
    vsf_gpio_pin_mask_t pin_mask;
} VSF_CAL_PACKED vsf_hal_distbus_gpio_pin_mask_t;

typedef struct vsf_hal_distbus_gpio_sync_t {
    vsf_gpio_pin_mask_t value;
} VSF_CAL_PACKED vsf_hal_distbus_gpio_sync_t;
#endif

typedef struct vsf_hal_distbus_gpio_info_t {
    uint8_t support_config_pin;
    uint8_t support_output_and_set;
    uint8_t support_output_and_clear;
    uint8_t pin_count;
    vsf_gpio_pin_mask_t pin_mask;

    vsf_gpio_pin_mask_t direction;
    vsf_gpio_pin_mask_t value;
} VSF_CAL_PACKED vsf_hal_distbus_gpio_info_t;

vsf_class(vsf_hal_distbus_gpio_t) {
#if VSF_HAL_DISTBUS_GPIO_CFG_MULTI_CLASS == ENABLED
    public_member(
        implement(vsf_gpio_t)
    )
#endif
    protected_member(
        vsf_distbus_service_t               service;
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

#if defined(__VSF_HAL_DISTBUS_GPIO_CLASS_IMPLEMENT) || defined(__VSF_HAL_DISTBUS_GPIO_CLASS_INHERIT__)
extern vsf_gpio_mode_t vsf_hal_distbus_io_feature_to_generic_io_feature(uint32_t hal_distbus_io_feature);
extern uint32_t vsf_generic_io_feature_to_hal_distbus_io_feature(vsf_gpio_mode_t generic_io_feature);
#endif

/*============================ INCLUDES ======================================*/

#define VSF_GPIO_CFG_DEC_PREFIX             vsf_hal_distbus
#define VSF_GPIO_CFG_DEC_UPCASE_PREFIX      VSF_HAL_DISTBUS
#include "hal/driver/common/gpio/gpio_template.h"

#ifdef __cplusplus
}
#endif

#undef __VSF_HAL_DISTBUS_GPIO_CLASS_IMPLEMENT
#undef __VSF_HAL_DISTBUS_GPIO_CLASS_INHERIT__

#endif
#endif
/* EOF */

