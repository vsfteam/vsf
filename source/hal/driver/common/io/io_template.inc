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

#include "hal/driver/common/template/vsf_template_hal_driver.h"

/*============================ MACROS ========================================*/

#if VSF_HAL_USE_GPIO == ENABLED

#ifndef VSF_GPIO_CFG_REIMPLEMENT_SET_INPUT
#   define VSF_GPIO_CFG_REIMPLEMENT_SET_INPUT           DISABLED
#endif

#ifndef VSF_GPIO_CFG_REIMPLEMENT_SET_OUTPUT
#   define VSF_GPIO_CFG_REIMPLEMENT_SET_OUTPUT          DISABLED
#endif

#ifndef VSF_GPIO_CFG_REIMPLEMENT_SWITCH_DIRECTION
#   define VSF_GPIO_CFG_REIMPLEMENT_SWITCH_DIRECTION    DISABLED
#endif

#ifndef VSF_GPIO_CFG_REIMPLEMENT_SET
#   define VSF_GPIO_CFG_REIMPLEMENT_SET                 DISABLED
#endif

#ifndef VSF_GPIO_CFG_REIMPLEMENT_CLEAR
#   define VSF_GPIO_CFG_REIMPLEMENT_CLEAR               DISABLED
#endif

#ifndef VSF_GPIO_CFG_REIMPLEMENT_OUTPUT_AND_SET
#   define VSF_GPIO_CFG_REIMPLEMENT_OUTPUT_AND_SET      DISABLED
#endif

#ifndef VSF_GPIO_CFG_REIMPLEMENT_OUTPUT_AND_CLEAR
#   define VSF_GPIO_CFG_REIMPLEMENT_OUTPUT_AND_CLEAR    DISABLED
#endif

#define vsf_real_gpio_t                 VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_t)
#define vsf_real_gpio_set_input         VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_set_input)
#define vsf_real_gpio_set_output        VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_set_output)
#define vsf_real_gpio_get_direction     VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_get_direction)
#define vsf_real_gpio_set_direction     VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_set_direction)
#define vsf_real_gpio_switch_direction  VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_switch_direction)
#define vsf_real_gpio_write             VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_write)
#define vsf_real_gpio_set               VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_set)
#define vsf_real_gpio_clear             VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_clear)
#define vsf_real_gpio_output_and_set    VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_output_and_set)
#define vsf_real_gpio_output_and_clear  VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_output_and_clear)
#define vsf_real_gpio_capability        VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_capability)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_GPIO_CFG_REIMPLEMENT_SET_INPUT == DISALBED
void vsf_real_gpio_set_input(vsf_real_gpio_t *gpio_ptr, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);

    vsf_real_gpio_set_direction(gpio_ptr, 0, pin_mask);
}
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_SET_OUTPUT == DISALBED
void vsf_real_gpio_set_output(vsf_real_gpio_t *gpio_ptr, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);

    vsf_real_gpio_set_direction(gpio_ptr, pin_mask, pin_mask);
}
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_SWITCH_DIRECTION == DISALBED
void vsf_real_gpio_switch_direction(vsf_real_gpio_t *gpio_ptr, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);

    uint32_t ret = ~vsf_real_gpio_get_direction(gpio_ptr, pin_mask);
    vsf_real_gpio_set_direction(gpio_ptr, ret, pin_mask);
}
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_SET == DISALBED
void vsf_real_gpio_set(vsf_real_gpio_t *gpio_ptr, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);

    vsf_real_gpio_write(gpio_ptr, pin_mask, pin_mask);
}
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_CLEAR == DISALBED
void vsf_real_gpio_clear(vsf_real_gpio_t *gpio_ptr, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);

    vsf_real_gpio_write(gpio_ptr, 0, pin_mask);
}
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_OUTPUT_AND_SET == DISALBED
void vsf_real_gpio_output_and_set(vsf_real_gpio_t *gpio_ptr, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);

    vsf_real_gpio_set(gpio_ptr, pin_mask);
    vsf_real_gpio_set_output(gpio_ptr, pin_mask);
}
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_OUTPUT_AND_CLEAR == DISALBED
void vsf_real_gpio_output_and_clear(vsf_real_gpio_t *gpio_ptr, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);

    vsf_real_gpio_clear(gpio_ptr, pin_mask);
    vsf_real_gpio_set_output(gpio_ptr, pin_mask);
}
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_CAPABILITY == DISABLED
gpio_capability_t vsf_real_gpio_capability(vsf_real_gpio_t *gpio_ptr)
{
    gpio_capability_t gpio_capability = {
        0,
    };

    return gpio_capability;
}
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#define __VSF_HAL_TEMPLATE_NAME                     _gpio
#define __VSF_HAL_TEMPLATE_UPCASE_NAME              _GPIO

#ifndef VSF_GPIO_CFG_PREFIX
#   error "Please define VSF_GPIO_CFG_PREFIX in gpio driver"
#endif

#ifndef VSF_GPIO_CFG_UPPERCASE_PREFIX
#   error "Please define VSF_GPIO_CFG_UPPERCASE_PREFIX in gpio driver"
#endif

#include "hal/driver/common/template/vsf_template_implementation.h"

#endif // VSF_HAL_USE_GPIO == ENABLED
