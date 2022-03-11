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

#if VSF_GPIO_CFG_IMPLEMENT_OP == ENABLED
#   define __VSF_GPIO_OP_VAR             VSF_MCONNECT(__, VSF_GPIO_CFG_IMP_PREFIX, _i2c_op)
#   define VSF_GPIO_OP                   .vsf_i2c.op = & __VSF_GPIO_OP_VAR,
#else
#   define VSF_GPIO_OP
#endif

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

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

#if VSF_GPIO_CFG_MULTI_CLASS == ENABLED && VSF_GPIO_CFG_IMPLEMENT_OP == ENABLED
static const vsf_gpio_op_t __VSF_GPIO_OP_VAR = {

#undef VSF_GPIO_API
#define VSF_GPIO_API(__prefix_name, __return, __name, ...)                      \
    VSF_TEMPLATE_HAL_API_OP(__prefix_name, _gpio_, __return, __name, __VA_ARGS__)

     VSF_GPIO_APIS(VSF_GPIO_CFG_INSTANCE_PREFIX)
};
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_GPIO_CFG_REIMPLEMENT_SET_INPUT == DISALBED
void vsf_gpio_set_input(vsf_gpio_t *gpio_ptr, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);

    vsf_gpio_set_direction(gpio_ptr, 0, pin_mask);
}
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_SET_OUTPUT == DISALBED
void vsf_gpio_set_output(vsf_gpio_t *gpio_ptr, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);

    vsf_gpio_set_direction(gpio_ptr, pin_mask, pin_mask);
}
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_SWITCH_DIRECTION == DISALBED
void vsf_gpio_switch_direction(vsf_gpio_t *gpio_ptr, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);

    uint32_t ret = ~vsf_gpio_get_direction(gpio_ptr, pin_mask);
    vsf_gpio_set_direction(gpio_ptr, ret, pin_mask);
}
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_SET == DISALBED
void vsf_gpio_set(vsf_gpio_t *gpio_ptr, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);

    vsf_gpio_write(gpio_ptr, pin_mask, pin_mask);
}
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_CLEAR == DISALBED
void vsf_gpio_clear(vsf_gpio_t *gpio_ptr, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);

    vsf_gpio_write(gpio_ptr, 0, pin_mask);
}
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_OUTPUT_AND_SET == DISALBED
void vsf_gpio_output_and_set(vsf_gpio_t *gpio_ptr, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);

    vsf_gpio_set(gpio_ptr, pin_mask);
    vsf_gpio_set_output(gpio_ptr, pin_mask);
}
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_OUTPUT_AND_CLEAR == DISALBED
void vsf_gpio_output_and_clear(vsf_gpio_t *gpio_ptr, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);

    vsf_gpio_clear(gpio_ptr, pin_mask);
    vsf_gpio_set_output(gpio_ptr, pin_mask);
}
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#ifdef VSF_GPIO_CFG_TEMPLATE_COUNT
#   ifndef VSF_GPIO_CFG_TEMPLATE_MASK
#       define VSF_GPIO_CFG_TEMPLATE_MASK     ((1ul << VSF_GPIO_CFG_TEMPLATE_COUNT) - 1)
#   endif

#   if VSF_GPIO_CFG_TEMPLATE_MASK & (1 << 0)
        VSF_GPIO_CFG_IMP_LV0(0, NULL)
#   endif
#   if VSF_GPIO_CFG_TEMPLATE_MASK & (1 << 1)
        VSF_GPIO_CFG_IMP_LV0(1, NULL)
#   endif
#   if VSF_GPIO_CFG_TEMPLATE_MASK & (1 << 2)
        VSF_GPIO_CFG_IMP_LV0(2, NULL)
#   endif
#   if VSF_GPIO_CFG_TEMPLATE_MASK & (1 << 3)
        VSF_GPIO_CFG_IMP_LV0(3, NULL)
#   endif
#   if VSF_GPIO_CFG_TEMPLATE_MASK & (1 << 4)
        VSF_GPIO_CFG_IMP_LV0(4, NULL)
#   endif
#   if VSF_GPIO_CFG_TEMPLATE_MASK & (1 << 5)
        VSF_GPIO_CFG_IMP_LV0(5, NULL)
#   endif
#   if VSF_GPIO_CFG_TEMPLATE_MASK & (1 << 6)
        VSF_GPIO_CFG_IMP_LV0(6, NULL)
#   endif
#   if VSF_GPIO_CFG_TEMPLATE_MASK & (1 << 7)
        VSF_GPIO_CFG_IMP_LV0(7, NULL)
#   endif

#   undef VSF_GPIO_CFG_TEMPLATE_COUNT
#   undef VSF_GPIO_CFG_TEMPLATE_MASK
#   undef VSF_GPIO_CFG_IMP_LV0
#endif

#endif // VSF_HAL_USE_GPIO == ENABLED
