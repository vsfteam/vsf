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

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_GPIO == ENABLED && VSF_HAL_DISTBUS_USE_GPIO == ENABLED

#define __VSF_DISTBUS_CLASS_INHERIT__
#define __VSF_HAL_DISTBUS_GPIO_CLASS_IMPLEMENT
#include "../driver.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#ifndef VSF_HW_GPIO_CFG_PROTECT_LEVEL
#   define VSF_HW_GPIO_CFG_PROTECT_LEVEL    interrupt
#endif

#define __vsf_gpio_protect                  vsf_protect(VSF_HW_GPIO_CFG_PROTECT_LEVEL)
#define __vsf_gpio_unprotect                vsf_unprotect(VSF_HW_GPIO_CFG_PROTECT_LEVEL)

/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

static bool __vsf_hal_distbus_gpio_msghandler(vsf_distbus_t *distbus, vsf_distbus_service_t *service, vsf_distbus_msg_t *msg);

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static const vsf_distbus_service_info_t __vsf_hal_distbus_gpio_info = {
    .mtu                = VSF_HAL_DISTBUS_CFG_MTU,
    .type               = 0,
    .addr_range         = VSF_HAL_DISTBUS_GPIO_CMD_ADDR_RANGE,
    .handler            = __vsf_hal_distbus_gpio_msghandler,
};

#if VSF_HAL_DISTBUS_GPIO_CFG_MULTI_CLASS == ENABLED
static const vsf_gpio_op_t __vsf_hal_distbus_gpio_op = {
#   undef __VSF_HAL_TEMPLATE_API
#   define __VSF_HAL_TEMPLATE_API VSF_HAL_TEMPLATE_API_OP

    VSF_GPIO_APIS(vsf_hal_distbus)
};
#endif

/*============================ IMPLEMENTATION ================================*/

static bool __vsf_hal_distbus_gpio_msghandler(vsf_distbus_t *distbus, vsf_distbus_service_t *service, vsf_distbus_msg_t *msg)
{
    vsf_hal_distbus_gpio_t *gpio = container_of(service, vsf_hal_distbus_gpio_t, service);
    bool retain_msg = false;

    union {
        void *ptr;
        vsf_hal_distbus_gpio_sync_t *sync;
    } u_arg;

    u_arg.ptr = (uint8_t *)&msg->header + sizeof(msg->header);
    switch (msg->header.addr) {
    case VSF_HAL_DISTBUS_GPIO_CMD_SYNC:
        gpio->info.value = le32_to_cpu(u_arg.sync->value);
        break;
    default:
        VSF_HAL_ASSERT(false);
        break;
    }
    return retain_msg;
}

uint32_t vsf_hal_distbus_gpio_register_service(vsf_distbus_t *distbus, vsf_hal_distbus_gpio_t *gpio, void *info, uint32_t infolen)
{
    if (infolen >= sizeof(vsf_hal_distbus_gpio_info_t)) {
        gpio->info = *(vsf_hal_distbus_gpio_info_t *)info;
        gpio->info.pin_mask = le32_to_cpu(gpio->info.pin_mask);
        gpio->info.direction = le32_to_cpu(gpio->info.direction);
        gpio->info.value = le32_to_cpu(gpio->info.value);

        gpio->distbus = distbus;
        gpio->service.info = &__vsf_hal_distbus_gpio_info;
#if VSF_HAL_DISTBUS_GPIO_CFG_MULTI_CLASS == ENABLED
        gpio->op = &__vsf_hal_distbus_gpio_op;
#endif
        vsf_distbus_register_service(distbus, &gpio->service);
    }
    return sizeof(vsf_hal_distbus_gpio_info_t);
}

// TODO:
vsf_io_feature_t vsf_hal_distbus_io_feature_to_generic_io_feature(uint32_t hal_distbus_io_feature)
{
    return (vsf_io_feature_t)0;
}
uint32_t vsf_generic_io_feature_to_hal_distbus_io_feature(vsf_io_feature_t generic_io_feature)
{
    return 0;
}

void vsf_hal_distbus_gpio_config_pin(vsf_hal_distbus_gpio_t *gpio, uint32_t pin_mask, vsf_io_feature_t feature)
{
    VSF_HAL_ASSERT(NULL != gpio);
    VSF_HAL_ASSERT(0 != pin_mask);
    VSF_HAL_ASSERT(gpio->info.support_config_pin);

    vsf_hal_distbus_gpio_config_pin_t *param;
    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(gpio->distbus, sizeof(*param), (uint8_t **)&param);
    VSF_HAL_ASSERT(msg != NULL);

    msg->header.addr = VSF_HAL_DISTBUS_GPIO_CMD_CONFIG_PIN;
    param->pin_mask = cpu_to_le32(pin_mask);
    param->feature = cpu_to_le32(vsf_generic_io_feature_to_hal_distbus_io_feature(feature));
    vsf_distbus_send_msg(gpio->distbus, &gpio->service, msg);
}

void vsf_hal_distbus_gpio_set_direction(vsf_hal_distbus_gpio_t *gpio, uint32_t pin_mask, uint32_t direction_mask)
{
    VSF_HAL_ASSERT(NULL != gpio);
    VSF_HAL_ASSERT(0 != pin_mask);

    vsf_hal_distbus_gpio_set_direction_t *param;
    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(gpio->distbus, sizeof(*param), (uint8_t **)&param);
    VSF_HAL_ASSERT(msg != NULL);

    msg->header.addr = VSF_HAL_DISTBUS_GPIO_CMD_SET_DIRECTION;
    param->pin_mask = cpu_to_le32(pin_mask);
    param->direction_mask = cpu_to_le32(direction_mask);
    vsf_distbus_send_msg(gpio->distbus, &gpio->service, msg);

    vsf_protect_t orig = __vsf_gpio_protect();
        gpio->info.direction &= ~pin_mask;
        gpio->info.direction |= pin_mask & direction_mask;
    __vsf_gpio_unprotect(orig);
}

uint32_t vsf_hal_distbus_gpio_get_direction(vsf_hal_distbus_gpio_t *gpio, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio);
    return gpio->info.direction & pin_mask;
}

void vsf_hal_distbus_gpio_switch_direction(vsf_hal_distbus_gpio_t *gpio, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio);
    VSF_HAL_ASSERT(0 != pin_mask);

    uint32_t ret = ~vsf_hal_distbus_gpio_get_direction(gpio, pin_mask);
    vsf_hal_distbus_gpio_set_direction(gpio, pin_mask, ret);
}

void vsf_hal_distbus_gpio_set_input(vsf_hal_distbus_gpio_t *gpio, uint32_t pin_mask)
{
    vsf_hal_distbus_gpio_set_direction(gpio, pin_mask, 0);
}

void vsf_hal_distbus_gpio_set_output(vsf_hal_distbus_gpio_t *gpio, uint32_t pin_mask)
{
    vsf_hal_distbus_gpio_set_direction(gpio, pin_mask, pin_mask);
}

uint32_t vsf_hal_distbus_gpio_read(vsf_hal_distbus_gpio_t *gpio)
{
    VSF_HAL_ASSERT(NULL != gpio);
    return gpio->info.value;
}

void vsf_hal_distbus_gpio_write(vsf_hal_distbus_gpio_t *gpio, uint32_t pin_mask, uint32_t value)
{
    VSF_HAL_ASSERT(NULL != gpio);
    VSF_HAL_ASSERT(0 != pin_mask);

    vsf_hal_distbus_gpio_write_t *param;
    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(gpio->distbus, sizeof(*param), (uint8_t **)&param);
    VSF_HAL_ASSERT(msg != NULL);

    msg->header.addr = VSF_HAL_DISTBUS_GPIO_CMD_WRITE;
    param->pin_mask = cpu_to_le32(pin_mask);
    param->value = cpu_to_le32(value);
    vsf_distbus_send_msg(gpio->distbus, &gpio->service, msg);
}

void vsf_hal_distbus_gpio_toggle(vsf_hal_distbus_gpio_t *gpio, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio);
    VSF_HAL_ASSERT(0 != pin_mask);

    vsf_hal_distbus_gpio_pin_mask_t *param;
    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(gpio->distbus, sizeof(*param), (uint8_t **)&param);
    VSF_HAL_ASSERT(msg != NULL);

    msg->header.addr = VSF_HAL_DISTBUS_GPIO_CMD_TOGGLE;
    param->pin_mask = cpu_to_le32(pin_mask);
    vsf_distbus_send_msg(gpio->distbus, &gpio->service, msg);
}

void vsf_hal_distbus_gpio_set(vsf_hal_distbus_gpio_t *gpio, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio);
    VSF_HAL_ASSERT(0 != pin_mask);

    vsf_hal_distbus_gpio_pin_mask_t *param;
    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(gpio->distbus, sizeof(*param), (uint8_t **)&param);
    VSF_HAL_ASSERT(msg != NULL);

    msg->header.addr = VSF_HAL_DISTBUS_GPIO_CMD_SET;
    param->pin_mask = cpu_to_le32(pin_mask);
    vsf_distbus_send_msg(gpio->distbus, &gpio->service, msg);
}

void vsf_hal_distbus_gpio_clear(vsf_hal_distbus_gpio_t *gpio, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio);
    VSF_HAL_ASSERT(0 != pin_mask);

    vsf_hal_distbus_gpio_pin_mask_t *param;
    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(gpio->distbus, sizeof(*param), (uint8_t **)&param);
    VSF_HAL_ASSERT(msg != NULL);

    msg->header.addr = VSF_HAL_DISTBUS_GPIO_CMD_CLEAR;
    param->pin_mask = cpu_to_le32(pin_mask);
    vsf_distbus_send_msg(gpio->distbus, &gpio->service, msg);
}

void vsf_hal_distbus_gpio_output_and_set(vsf_hal_distbus_gpio_t *gpio, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio);
    VSF_HAL_ASSERT(0 != pin_mask);
    VSF_HAL_ASSERT(gpio->info.support_output_and_set);

    vsf_hal_distbus_gpio_pin_mask_t *param;
    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(gpio->distbus, sizeof(*param), (uint8_t **)&param);
    VSF_HAL_ASSERT(msg != NULL);

    msg->header.addr = VSF_HAL_DISTBUS_GPIO_CMD_OUTPUT_AND_SET;
    param->pin_mask = cpu_to_le32(pin_mask);
    vsf_distbus_send_msg(gpio->distbus, &gpio->service, msg);
}

void vsf_hal_distbus_gpio_output_and_clear(vsf_hal_distbus_gpio_t *gpio, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio);
    VSF_HAL_ASSERT(0 != pin_mask);
    VSF_HAL_ASSERT(gpio->info.support_output_and_set);

    vsf_hal_distbus_gpio_pin_mask_t *param;
    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(gpio->distbus, sizeof(*param), (uint8_t **)&param);
    VSF_HAL_ASSERT(msg != NULL);

    msg->header.addr = VSF_HAL_DISTBUS_GPIO_CMD_OUTPUT_AND_CLEAR;
    param->pin_mask = cpu_to_le32(pin_mask);
    vsf_distbus_send_msg(gpio->distbus, &gpio->service, msg);
}

gpio_capability_t vsf_hal_distbus_gpio_capability(vsf_hal_distbus_gpio_t *gpio)
{
    return (gpio_capability_t) {
        .is_async                       = true,
        .is_support_config_pin          = gpio->info.support_config_pin,
        .is_support_output_and_set      = gpio->info.support_output_and_set,
        .is_support_output_and_clear    = gpio->info.support_output_and_clear,
        .pin_count                      = gpio->info.pin_count,
        .avail_pin_mask                 = gpio->info.pin_mask,
    };
}

#endif
