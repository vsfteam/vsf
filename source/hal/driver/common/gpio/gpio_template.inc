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
#       VSF_GPIO_CFG_INSTANCE_PREFIX                    vsf_hw
#   endif
#   define ____VSF_GPIO_OP(__p) .vsf_gpio.op = & __p ## _gpio_op,
#   define __VSF_GPIO_OP(__p)   ____VSF_GPIO_OP(__p)
#   define VSF_GPIO_OP          __VSF_GPIO_OP(VSF_GPIO_CFG_INSTANCE_PREFIX)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define ____VSF_GPIO_FN_VAR_CONFIG_PIN(__p)         ((void (*)(vsf_gpio_t *, uint32_t, uint_fast32_t))&__p ## _gpio_config_pin)
#define ____VSF_GPIO_FN_VAR_SET_DIRECTION(__p)      ((void (*)(vsf_gpio_t *, uint32_t, uint32_t))     &__p ## _gpio_set_direction)
#define ____VSF_GPIO_FN_VAR_GET_DIRECTION(__p)      ((uint32_t (*)(vsf_gpio_t *, uint32_t))           &__p ## _gpio_get_direction)
#define ____VSF_GPIO_FN_VAR_SET_INPUT(__p)          ((void (*)(vsf_gpio_t *, uint32_t))               &__p ## _gpio_set_input)
#define ____VSF_GPIO_FN_VAR_SET_OUTPUT(__p)         ((void (*)(vsf_gpio_t *, uint32_t))               &__p ## _gpio_set_output)
#define ____VSF_GPIO_FN_VAR_SWITCH_DIRECTION(__p)   ((void (*)(vsf_gpio_t *, uint32_t))               &__p ## _gpio_switch_direction)
#define ____VSF_GPIO_FN_VAR_READ(__p)               ((uint32_t (*)(vsf_gpio_t *))                     &__p ## _gpio_read)
#define ____VSF_GPIO_FN_VAR_WRITE(__p)              ((void (*)(vsf_gpio_t *, uint32_t, uint32_t))     &__p ## _gpio_write)
#define ____VSF_GPIO_FN_VAR_SET(__p)                ((void (*)(vsf_gpio_t *, uint32_t))               &__p ## _gpio_set)
#define ____VSF_GPIO_FN_VAR_CLEAR(__p)              ((void (*)(vsf_gpio_t *, uint32_t))               &__p ## _gpio_clear)
#define ____VSF_GPIO_FN_VAR_TOGGLE(__p)             ((void (*)(vsf_gpio_t *, uint32_t))               &__p ## _gpio_toggle)
#define ____VSF_GPIO_FN_VAR_OUTPUT_AND_SET(__p)     ((void (*)(vsf_gpio_t *, uint32_t))               &__p ## _gpio_output_and_set)
#define ____VSF_GPIO_FN_VAR_OUTPUT_AND_CLEAR(__p)   ((void (*)(vsf_gpio_t *, uint32_t))               &__p ## _gpio_output_and_clear)

#define __VSF_GPIO_FN_VAR_CONFIG_PIN(__p)           ____VSF_GPIO_FN_VAR_CONFIG_PIN(__p)
#define __VSF_GPIO_FN_VAR_SET_DIRECTION(__p)        ____VSF_GPIO_FN_VAR_SET_DIRECTION(__p)
#define __VSF_GPIO_FN_VAR_GET_DIRECTION(__p)        ____VSF_GPIO_FN_VAR_GET_DIRECTION(__p)
#define __VSF_GPIO_FN_VAR_SET_INPUT(__p)            ____VSF_GPIO_FN_VAR_SET_INPUT(__p)
#define __VSF_GPIO_FN_VAR_SET_OUTPUT(__p)           ____VSF_GPIO_FN_VAR_SET_OUTPUT(__p)
#define __VSF_GPIO_FN_VAR_SWITCH_DIRECTION(__p)     ____VSF_GPIO_FN_VAR_SWITCH_DIRECTION(__p)
#define __VSF_GPIO_FN_VAR_READ(__p)                 ____VSF_GPIO_FN_VAR_READ(__p)
#define __VSF_GPIO_FN_VAR_WRITE(__p)                ____VSF_GPIO_FN_VAR_WRITE(__p)
#define __VSF_GPIO_FN_VAR_SET(__p)                  ____VSF_GPIO_FN_VAR_SET(__p)
#define __VSF_GPIO_FN_VAR_CLEAR(__p)                ____VSF_GPIO_FN_VAR_CLEAR(__p)
#define __VSF_GPIO_FN_VAR_TOGGLE(__p)               ____VSF_GPIO_FN_VAR_TOGGLE(__p)
#define __VSF_GPIO_FN_VAR_OUTPUT_AND_SET(__p)       ____VSF_GPIO_FN_VAR_OUTPUT_AND_SET(__p)
#define __VSF_GPIO_FN_VAR_OUTPUT_AND_CLEAR(__p)     ____VSF_GPIO_FN_VAR_OUTPUT_AND_CLEAR(__p)

#define __VSF_GPIO_FN_CONFIG_PIN(__p)         void     __p ## _gpio_config_pin(vsf_gpio_t *gpio_ptr, uint32_t pin_mask, uint_fast32_t feature)
#define __VSF_GPIO_FN_SET_DIRECTION(__p)      void     __p ## _gpio_set_direction(vsf_gpio_t *gpio_ptr, uint32_t direction_mask, uint32_t pin_mask)
#define __VSF_GPIO_FN_GET_DIRECTION(__p)      uint32_t __p ## _gpio_get_direction(vsf_gpio_t *gpio_ptr, uint32_t pin_mask)
#define __VSF_GPIO_FN_SET_INPUT(__p)          void     __p ## _gpio_set_input(vsf_gpio_t *gpio_ptr, uint32_t pin_mask)
#define __VSF_GPIO_FN_SET_OUTPUT(__p)         void     __p ## _gpio_set_output(vsf_gpio_t *gpio_ptr, uint32_t pin_mask)
#define __VSF_GPIO_FN_SWITCH_DIRECTION(__p)   void     __p ## _gpio_switch_direction(vsf_gpio_t *gpio_ptr, uint32_t pin_mask)
#define __VSF_GPIO_FN_READ(__p)               uint32_t __p ## _gpio_read(vsf_gpio_t *gpio_ptr)
#define __VSF_GPIO_FN_WRITE(__p)              void     __p ## _gpio_write(vsf_gpio_t *gpio_ptr, uint32_t value, uint32_t pin_mask)
#define __VSF_GPIO_FN_SET(__p)                void     __p ## _gpio_set(vsf_gpio_t *gpio_ptr, uint32_t pin_mask)
#define __VSF_GPIO_FN_CLEAR(__p)              void     __p ## _gpio_clear(vsf_gpio_t *gpio_ptr, uint32_t pin_mask)
#define __VSF_GPIO_FN_TOGGLE(__p)             void     __p ## _gpio_toggle(vsf_gpio_t *gpio_ptr, uint32_t pin_mask)
#define __VSF_GPIO_FN_OUTPUT_AND_SET(__p)     void     __p ## _gpio_output_and_set(vsf_gpio_t *gpio_ptr, uint32_t pin_mask)
#define __VSF_GPIO_FN_OUTPUT_AND_CLEAR(__p)   void     __p ## _gpio_output_and_clear(vsf_gpio_t *gpio_ptr, uint32_t pin_mask)

#define VSF_GPIO_FN_CONFIG_PIN(__p)           __VSF_GPIO_FN_CONFIG_PIN(__p)
#define VSF_GPIO_FN_SET_DIRECTION(__p)        __VSF_GPIO_FN_SET_DIRECTION(__p)
#define VSF_GPIO_FN_GET_DIRECTION(__p)        __VSF_GPIO_FN_GET_DIRECTION(__p)
#define VSF_GPIO_FN_SET_INPUT(__p)            __VSF_GPIO_FN_SET_INPUT(__p)
#define VSF_GPIO_FN_SET_OUTPUT(__p)           __VSF_GPIO_FN_SET_OUTPUT(__p)
#define VSF_GPIO_FN_SWITCH_DIRECTION(__p)     __VSF_GPIO_FN_SWITCH_DIRECTION(__p)
#define VSF_GPIO_FN_READ(__p)                 __VSF_GPIO_FN_READ(__p)
#define VSF_GPIO_FN_WRITE(__p)                __VSF_GPIO_FN_WRITE(__p)
#define VSF_GPIO_FN_SET(__p)                  __VSF_GPIO_FN_SET(__p)
#define VSF_GPIO_FN_CLEAR(__p)                __VSF_GPIO_FN_CLEAR(__p)
#define VSF_GPIO_FN_TOGGLE(__p)               __VSF_GPIO_FN_TOGGLE(__p)
#define VSF_GPIO_FN_OUTPUT_AND_SET(__p)       __VSF_GPIO_FN_OUTPUT_AND_SET(__p)
#define VSF_GPIO_FN_OUTPUT_AND_CLEAR(__p)     __VSF_GPIO_FN_OUTPUT_AND_CLEAR(__p)

/*============================ PROTOTYPES ====================================*/

extern VSF_GPIO_FN_CONFIG_PIN(VSF_GPIO_CFG_INSTANCE_PREFIX);
extern VSF_GPIO_FN_SET_DIRECTION(VSF_GPIO_CFG_INSTANCE_PREFIX);
extern VSF_GPIO_FN_GET_DIRECTION(VSF_GPIO_CFG_INSTANCE_PREFIX);
extern VSF_GPIO_FN_SET_INPUT(VSF_GPIO_CFG_INSTANCE_PREFIX);
extern VSF_GPIO_FN_SET_OUTPUT(VSF_GPIO_CFG_INSTANCE_PREFIX);
extern VSF_GPIO_FN_SWITCH_DIRECTION(VSF_GPIO_CFG_INSTANCE_PREFIX);
extern VSF_GPIO_FN_READ(VSF_GPIO_CFG_INSTANCE_PREFIX);
extern VSF_GPIO_FN_WRITE(VSF_GPIO_CFG_INSTANCE_PREFIX);
extern VSF_GPIO_FN_SET(VSF_GPIO_CFG_INSTANCE_PREFIX);
extern VSF_GPIO_FN_CLEAR(VSF_GPIO_CFG_INSTANCE_PREFIX);
extern VSF_GPIO_FN_TOGGLE(VSF_GPIO_CFG_INSTANCE_PREFIX);
extern VSF_GPIO_FN_OUTPUT_AND_SET(VSF_GPIO_CFG_INSTANCE_PREFIX);
extern VSF_GPIO_FN_OUTPUT_AND_CLEAR(VSF_GPIO_CFG_INSTANCE_PREFIX);

/*============================ LOCAL VARIABLES ===============================*/

#if VSF_GPIO_CFG_MULTI_INSTANCES == ENABLED && VSF_GPIO_CFG_IMPLEMENT_OP == ENABLED
static const vsf_gpio_op_t vsf_hw_gpio_op = {
    .config_pin       = __VSF_GPIO_FN_VAR_CONFIG_PIN(VSF_GPIO_CFG_INSTANCE_PREFIX),
    .set_direction    = __VSF_GPIO_FN_VAR_SET_DIRECTION(VSF_GPIO_CFG_INSTANCE_PREFIX),
    .get_direction    = __VSF_GPIO_FN_VAR_GET_DIRECTION(VSF_GPIO_CFG_INSTANCE_PREFIX),
    .set_input        = __VSF_GPIO_FN_VAR_SET_INPUT(VSF_GPIO_CFG_INSTANCE_PREFIX),
    .set_output       = __VSF_GPIO_FN_VAR_SET_OUTPUT(VSF_GPIO_CFG_INSTANCE_PREFIX),
    .switch_direction = __VSF_GPIO_FN_VAR_SWITCH_DIRECTION(VSF_GPIO_CFG_INSTANCE_PREFIX),
    .read             = __VSF_GPIO_FN_VAR_READ(VSF_GPIO_CFG_INSTANCE_PREFIX),
    .write            = __VSF_GPIO_FN_VAR_WRITE(VSF_GPIO_CFG_INSTANCE_PREFIX),
    .set              = __VSF_GPIO_FN_VAR_SET(VSF_GPIO_CFG_INSTANCE_PREFIX),
    .clear            = __VSF_GPIO_FN_VAR_CLEAR(VSF_GPIO_CFG_INSTANCE_PREFIX),
    .toggle           = __VSF_GPIO_FN_VAR_TOGGLE(VSF_GPIO_CFG_INSTANCE_PREFIX),
    .output_and_set   = __VSF_GPIO_FN_VAR_OUTPUT_AND_SET(VSF_GPIO_CFG_INSTANCE_PREFIX),
    .output_and_clear = __VSF_GPIO_FN_VAR_OUTPUT_AND_CLEAR(VSF_GPIO_CFG_INSTANCE_PREFIX),
};
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
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

#endif // VSF_HAL_USE_GPIO == ENABLED
