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

#if VSF_GPIO_CFG_REIMPLEMENT_SET_INPUT
#   define VSF_GPIO_CFG_REIMPLEMENT_SET_INPUT           DISABLED
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_SET_OUTPUT
#   define VSF_GPIO_CFG_REIMPLEMENT_SET_OUTPUT          DISABLED
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_SWITCH_DIRECTION
#   define VSF_GPIO_CFG_REIMPLEMENT_SWITCH_DIRECTION    DISABLED
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_SET
#   define VSF_GPIO_CFG_REIMPLEMENT_SET                 DISABLED
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_CLEAR
#   define VSF_GPIO_CFG_REIMPLEMENT_CLEAR               DISABLED
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_OUTPUT_AND_SET
#   define VSF_GPIO_CFG_REIMPLEMENT_OUTPUT_AND_SET      DISABLED
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_OUTPUT_AND_CLEAR
#   define VSF_GPIO_CFG_REIMPLEMENT_OUTPUT_AND_CLEAR    DISABLED
#endif

#if VSF_HAL_GPIO_CFG_MULTI_INSTANCES == DISABLED
#   define VSF_HAL_GPIO_MULTI_OP
#else

#   ifndef VSF_HAL_GPIO_CFG_INSTANCE_PREFIX
#       define VSF_HAL_GPIO_CFG_INSTANCE_PREFIX vsf_hw_
#   endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#   define VSF_GPIO_WRAPPER(__header, __api)    __VSF_GPIO_WRAPPER(__header, __api)
#   define __VSF_GPIO_WRAPPER(__header, __api)  __header ## __api

#   define vsf_gpio_config_pin          VSF_GPIO_WRAPPER(VSF_HAL_GPIO_CFG_INSTANCE_PREFIX, gpio_config_pin)
#   define vsf_gpio_set_direction       VSF_GPIO_WRAPPER(VSF_HAL_GPIO_CFG_INSTANCE_PREFIX, gpio_set_direction)
#   define vsf_gpio_get_direction       VSF_GPIO_WRAPPER(VSF_HAL_GPIO_CFG_INSTANCE_PREFIX, gpio_get_direction)
#   define vsf_gpio_set_input           VSF_GPIO_WRAPPER(VSF_HAL_GPIO_CFG_INSTANCE_PREFIX, gpio_set_input)
#   define vsf_gpio_set_output          VSF_GPIO_WRAPPER(VSF_HAL_GPIO_CFG_INSTANCE_PREFIX, gpio_set_output)
#   define vsf_gpio_switch_direction    VSF_GPIO_WRAPPER(VSF_HAL_GPIO_CFG_INSTANCE_PREFIX, gpio_switch_direction)
#   define vsf_gpio_read                VSF_GPIO_WRAPPER(VSF_HAL_GPIO_CFG_INSTANCE_PREFIX, gpio_read)
#   define vsf_gpio_write               VSF_GPIO_WRAPPER(VSF_HAL_GPIO_CFG_INSTANCE_PREFIX, gpio_write)
#   define vsf_gpio_set                 VSF_GPIO_WRAPPER(VSF_HAL_GPIO_CFG_INSTANCE_PREFIX, gpio_set)
#   define vsf_gpio_clear               VSF_GPIO_WRAPPER(VSF_HAL_GPIO_CFG_INSTANCE_PREFIX, gpio_clear)
#   define vsf_gpio_toggle              VSF_GPIO_WRAPPER(VSF_HAL_GPIO_CFG_INSTANCE_PREFIX, gpio_toggle)

#   define VSF_HAL_GPIO_MULTI_OP        .op = &VSF_GPIO_WRAPPER(VSF_HAL_GPIO_CFG_INSTANCE_PREFIX, gpio_op),

/*============================ PROTOTYPES ====================================*/

extern void vsf_gpio_config_pin(vsf_gpio_t *gpio_ptr, uint32_t pin_mask, uint_fast32_t feature);
extern void vsf_gpio_set_direction(vsf_gpio_t *gpio_ptr, uint32_t direction_mask, uint32_t pin_mask);
extern uint32_t vsf_gpio_get_direction(vsf_gpio_t *gpio_ptr, uint32_t pin_mask);
extern void vsf_gpio_set_input(vsf_gpio_t *gpio_ptr, uint32_t pin_mask);
extern void vsf_gpio_set_output(vsf_gpio_t *gpio_ptr, uint32_t pin_mask);
extern void vsf_gpio_switch_direction(vsf_gpio_t *gpio_ptr, uint32_t pin_mask);
extern uint32_t vsf_gpio_read(vsf_gpio_t *gpio_ptr);
extern void vsf_gpio_write(vsf_gpio_t *gpio_ptr, uint32_t value, uint32_t pin_mask);
extern void vsf_gpio_set(vsf_gpio_t *gpio_ptr, uint32_t pin_mask);
extern void vsf_gpio_clear(vsf_gpio_t *gpio_ptr, uint32_t pin_mask);
extern void vsf_gpio_toggle(vsf_gpio_t *gpio_ptr, uint32_t pin_mask);

/*============================ LOCAL VARIABLES ===============================*/

static const vsf_gpio_op_t vsf_hw_gpio_op = {
    .config_pin       = (void (*)(vsf_gpio_t *, uint32_t, uint_fast32_t)) &vsf_gpio_config_pin,
    .set_direction    = (void (*)(vsf_gpio_t *, uint32_t, uint32_t))      &vsf_gpio_set_direction,
    .get_direction    = (uint32_t (*)(vsf_gpio_t *, uint32_t))            &vsf_gpio_get_direction,
    .set_input        = (void (*)(vsf_gpio_t *, uint32_t))                &vsf_gpio_set_input,
    .set_output       = (void (*)(vsf_gpio_t *, uint32_t))                &vsf_gpio_set_output,
    .switch_direction = (void (*)(vsf_gpio_t *, uint32_t))                &vsf_gpio_switch_direction,
    .read             = (uint32_t (*)(vsf_gpio_t *))                      &vsf_gpio_read,
    .write            = (void (*)(vsf_gpio_t *, uint32_t, uint32_t))      &vsf_gpio_write,
    .set              = (void (*)(vsf_gpio_t *, uint32_t))                &vsf_gpio_set,
    .clear            = (void (*)(vsf_gpio_t *, uint32_t))                &vsf_gpio_clear,
    .toggle           = (void (*)(vsf_gpio_t *, uint32_t))                &vsf_gpio_toggle,
};
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
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

    vsf_gpio_write(gpio_ptr, pin_mask, pin_mask);
}
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_OUTPUT_AND_SET == DISALBED
void vsf_gpio_output_and_set(vsf_gpio_t *gpio_ptr, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);

    vsf_gpio_set(gpio_ptr, pin_mask);
    vsf_gpio_set_direction(gpio_ptr, pin_mask, pin_mask);
}
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_OUTPUT_AND_CLEAR == DISALBED
void vsf_gpio_output_and_clear(vsf_gpio_t *gpio_ptr, uint32_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);

    vsf_gpio_clear(gpio_ptr, pin_mask);
    vsf_gpio_set_direction(gpio_ptr, pin_mask, pin_mask);
}
#endif

#endif
