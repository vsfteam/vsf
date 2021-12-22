/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
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

#ifndef VSF_GPIO_CFG_IMPLEMENT_OP
#   define VSF_GPIO_CFG_IMPLEMENT_OP                    DISABLED
#endif

#if VSF_GPIO_CFG_MULTI_INSTANCES == DISABLED
#   define VSF_GPIO_OP
#   undef VSF_GPIO_CFG_INSTANCE_PREFIX
#   define VSF_GPIO_CFG_INSTANCE_PREFIX                 vsf
#else
#   ifndef VSF_GPIO_CFG_INSTANCE_PREFIX
#       define VSF_GPIO_CFG_INSTANCE_PREFIX             vsf_hw
#   endif
#   define ____VSF_GPIO_OP(__p) .vsf_gpio.op = & __p ## _gpio_op,
#   define __VSF_GPIO_OP(__p)   ____VSF_GPIO_OP(__p)
#   define VSF_GPIO_OP          __VSF_GPIO_OP(VSF_GPIO_CFG_INSTANCE_PREFIX)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define  __VSF_GPIO_FN(__prefix_name, __return, __name, ...)                   \
    VSF_TEMPLATE_HAL_API_DEFINE(__prefix_name, _gpio_, __return, __name, __VA_ARGS__)

#define VSF_GPIO_FN_CONFIG_PIN(__prefix_name)        __VSF_GPIO_FN(__prefix_name, void, config_pin, vsf_gpio_t *gpio_ptr, uint32_t pin_mask, uint_fast32_t feature)
#define VSF_GPIO_FN_SET_DIRECTION(__prefix_name)     __VSF_GPIO_FN(__prefix_name, void,     set_direction,    vsf_gpio_t *gpio_ptr, uint32_t direction_mask, uint32_t pin_mask)
#define VSF_GPIO_FN_GET_DIRECTION(__prefix_name)     __VSF_GPIO_FN(__prefix_name, uint32_t, get_direction,    vsf_gpio_t *gpio_ptr, uint32_t pin_mask)
#define VSF_GPIO_FN_SET_INPUT(__prefix_name)         __VSF_GPIO_FN(__prefix_name, void,     set_input,        vsf_gpio_t *gpio_ptr, uint32_t pin_mask)
#define VSF_GPIO_FN_SET_OUTPUT(__prefix_name)        __VSF_GPIO_FN(__prefix_name, void,     set_output,       vsf_gpio_t *gpio_ptr, uint32_t pin_mask)
#define VSF_GPIO_FN_SWITCH_DIRECTION(__prefix_name)  __VSF_GPIO_FN(__prefix_name, void,     switch_direction, vsf_gpio_t *gpio_ptr, uint32_t pin_mask)
#define VSF_GPIO_FN_READ(__prefix_name)              __VSF_GPIO_FN(__prefix_name, uint32_t, read,             vsf_gpio_t *gpio_ptr)
#define VSF_GPIO_FN_WRITE(__prefix_name)             __VSF_GPIO_FN(__prefix_name, void,     write,            vsf_gpio_t *gpio_ptr, uint32_t value, uint32_t pin_mask)
#define VSF_GPIO_FN_SET(__prefix_name)               __VSF_GPIO_FN(__prefix_name, void,     set,              vsf_gpio_t *gpio_ptr, uint32_t pin_mask)
#define VSF_GPIO_FN_CLEAR(__prefix_name)             __VSF_GPIO_FN(__prefix_name, void,     clear,            vsf_gpio_t *gpio_ptr, uint32_t pin_mask)
#define VSF_GPIO_FN_TOGGLE(__prefix_name)            __VSF_GPIO_FN(__prefix_name, void,     toggle,           vsf_gpio_t *gpio_ptr, uint32_t pin_mask)
#define VSF_GPIO_FN_OUTPUT_AND_SET(__prefix_name)    __VSF_GPIO_FN(__prefix_name, void,     output_and_set,   vsf_gpio_t *gpio_ptr, uint32_t pin_mask)
#define VSF_GPIO_FN_OUTPUT_AND_CLEAR(__prefix_name)  __VSF_GPIO_FN(__prefix_name, void,     output_and_clear, vsf_gpio_t *gpio_ptr, uint32_t pin_mask)

#define __VSF_GPIO_APIS(__prefix_name)                                                                                         \
    __VSF_GPIO_API(__prefix_name, void, config_pin, vsf_gpio_t *gpio_ptr, uint32_t pin_mask, uint_fast32_t feature)            \
    __VSF_GPIO_API(__prefix_name, void,     set_direction,    vsf_gpio_t *gpio_ptr, uint32_t direction_mask, uint32_t pin_mask)\
    __VSF_GPIO_API(__prefix_name, uint32_t, get_direction,    vsf_gpio_t *gpio_ptr, uint32_t pin_mask)                         \
    __VSF_GPIO_API(__prefix_name, void,     set_input,        vsf_gpio_t *gpio_ptr, uint32_t pin_mask)                         \
    __VSF_GPIO_API(__prefix_name, void,     set_output,       vsf_gpio_t *gpio_ptr, uint32_t pin_mask)                         \
    __VSF_GPIO_API(__prefix_name, void,     switch_direction, vsf_gpio_t *gpio_ptr, uint32_t pin_mask)                         \
    __VSF_GPIO_API(__prefix_name, uint32_t, read,             vsf_gpio_t *gpio_ptr)                                            \
    __VSF_GPIO_API(__prefix_name, void,     write,            vsf_gpio_t *gpio_ptr, uint32_t value, uint32_t pin_mask)         \
    __VSF_GPIO_API(__prefix_name, void,     set,              vsf_gpio_t *gpio_ptr, uint32_t pin_mask)                         \
    __VSF_GPIO_API(__prefix_name, void,     clear,            vsf_gpio_t *gpio_ptr, uint32_t pin_mask)                         \
    __VSF_GPIO_API(__prefix_name, void,     toggle,           vsf_gpio_t *gpio_ptr, uint32_t pin_mask)                         \
    __VSF_GPIO_API(__prefix_name, void,     output_and_set,   vsf_gpio_t *gpio_ptr, uint32_t pin_mask)                         \
    __VSF_GPIO_API(__prefix_name, void,     output_and_clear, vsf_gpio_t *gpio_ptr, uint32_t pin_mask)

/*============================ PROTOTYPES ====================================*/

#undef   __VSF_GPIO_API
#define  __VSF_GPIO_API(__prefix_name, __return, __name, ...)                   \
    VSF_TEMPLATE_HAL_API_EXTERN(__prefix_name, _gpio_, __return, __name, __VA_ARGS__)

__VSF_GPIO_APIS(VSF_GPIO_CFG_INSTANCE_PREFIX)

/*============================ LOCAL VARIABLES ===============================*/

#if VSF_GPIO_CFG_MULTI_INSTANCES == ENABLED && VSF_GPIO_CFG_IMPLEMENT_OP == ENABLED
static const vsf_gpio_op_t vsf_hw_gpio_op = {

#undef   __VSF_GPIO_API
#define  __VSF_GPIO_API(__prefix_name, __return, __name, ...)                   \
    VSF_TEMPLATE_HAL_API_OP(__prefix_name, _gpio_, __return, __name, __VA_ARGS__)

    __VSF_GPIO_APIS(VSF_GPIO_CFG_INSTANCE_PREFIX)
};
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_GPIO_CFG_REIMPLEMENT_SET_INPUT == DISALBED
VSF_GPIO_FN_SET_INPUT(VSF_GPIO_CFG_INSTANCE_PREFIX)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);

    vsf_gpio_set_direction(gpio_ptr, 0, pin_mask);
}
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_SET_OUTPUT == DISALBED
VSF_GPIO_FN_SET_OUTPUT(VSF_GPIO_CFG_INSTANCE_PREFIX)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);

    vsf_gpio_set_direction(gpio_ptr, pin_mask, pin_mask);
}
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_SWITCH_DIRECTION == DISALBED
VSF_GPIO_FN_SWITCH_DIRECTION(VSF_GPIO_CFG_INSTANCE_PREFIX)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);

    uint32_t ret = ~vsf_gpio_get_direction(gpio_ptr, pin_mask);
    vsf_gpio_set_direction(gpio_ptr, ret, pin_mask);
}
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_SET == DISALBED
VSF_GPIO_FN_SET(VSF_GPIO_CFG_INSTANCE_PREFIX)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);

    vsf_gpio_write(gpio_ptr, pin_mask, pin_mask);
}
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_CLEAR == DISALBED
VSF_GPIO_FN_CLEAR(VSF_GPIO_CFG_INSTANCE_PREFIX)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);

    vsf_gpio_write(gpio_ptr, 0, pin_mask);
}
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_OUTPUT_AND_SET == DISALBED
VSF_GPIO_FN_OUTPUT_AND_SET(VSF_GPIO_CFG_INSTANCE_PREFIX)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);

    vsf_gpio_set(gpio_ptr, pin_mask);
    vsf_gpio_set_output(gpio_ptr, pin_mask);
}
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_OUTPUT_AND_CLEAR == DISALBED
VSF_GPIO_FN_OUTPUT_AND_CLEAR(VSF_GPIO_CFG_INSTANCE_PREFIX)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);

    vsf_gpio_clear(gpio_ptr, pin_mask);
    vsf_gpio_set_output(gpio_ptr, pin_mask);
}
#endif


/*============================ TYPES =========================================*/
/*============================ IMPLEMENTATION ================================*/

#endif // VSF_HAL_USE_GPIO == ENABLED
