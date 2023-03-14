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

#ifndef VSF_GPIO_CFG_REIMPLEMENT_API_SET_INPUT
#   define VSF_GPIO_CFG_REIMPLEMENT_API_SET_INPUT           DISABLED
#endif

#ifndef VSF_GPIO_CFG_REIMPLEMENT_API_SET_OUTPUT
#   define VSF_GPIO_CFG_REIMPLEMENT_API_SET_OUTPUT          DISABLED
#endif

#ifndef VSF_GPIO_CFG_REIMPLEMENT_API_SWITCH_DIRECTION
#   define VSF_GPIO_CFG_REIMPLEMENT_API_SWITCH_DIRECTION    DISABLED
#endif

#ifndef VSF_GPIO_CFG_REIMPLEMENT_API_SET
#   define VSF_GPIO_CFG_REIMPLEMENT_API_SET                 DISABLED
#endif

#ifndef VSF_GPIO_CFG_REIMPLEMENT_API_CLEAR
#   define VSF_GPIO_CFG_REIMPLEMENT_API_CLEAR               DISABLED
#endif

#ifndef VSF_GPIO_CFG_REIMPLEMENT_API_OUTPUT_AND_SET
#   define VSF_GPIO_CFG_REIMPLEMENT_API_OUTPUT_AND_SET      DISABLED
#endif

#ifndef VSF_GPIO_CFG_REIMPLEMENT_API_OUTPUT_AND_CLEAR
#   define VSF_GPIO_CFG_REIMPLEMENT_API_OUTPUT_AND_CLEAR    DISABLED
#endif

#ifndef VSF_GPIO_CFG_CHNAGE_DIR_FIRST
#   define VSF_GPIO_CFG_CHNAGE_DIR_FIRST                    ENABLED
#endif

#ifndef VSF_GPIO_CFG_CAPABILITY_IS_OUTPUT_AND_SET
#    define VSF_GPIO_CFG_CAPABILITY_IS_OUTPUT_AND_SET        0
#endif
#ifndef VSF_GPIO_CFG_CAPABILITY_IS_OUTPUT_AND_CLEAR
#    define VSF_GPIO_CFG_CAPABILITY_IS_OUTPUT_AND_CLEAR      0
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
#   ifndef VSF_GPIO_CFG_CAPABILITY_IS_ASYNC
#       define VSF_GPIO_CFG_CAPABILITY_IS_ASYNC             0
#   endif

#   ifndef VSF_GPIO_CFG_CAPABILITY_PIN_COUNT
#       define VSF_GPIO_CFG_CAPABILITY_PIN_COUNT            32
#   endif
#   ifndef VSF_GPIO_CFG_CAPABILITY_AVAIL_PIN_MASK
#       define VSF_GPIO_CFG_CAPABILITY_AVAIL_PIN_MASK       0xFFFFFFFF
#   endif
#endif

#define vsf_real_gpio_t                 VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_t)
#define vsf_real_gpio_set_input         VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_set_input)
#define vsf_real_gpio_set_output        VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_set_output)
#define vsf_real_gpio_get_direction     VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_get_direction)
#define vsf_real_gpio_set_direction     VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_set_direction)
#define vsf_real_gpio_switch_direction  VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_switch_direction)
#define vsf_real_gpio_write             VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_write)
#define vsf_real_gpio_set               VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_set)
#define vsf_real_gpio_clear             VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_clear)
#define vsf_real_gpio_output_and_set    VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_output_and_set)
#define vsf_real_gpio_output_and_clear  VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_output_and_clear)
#define vsf_real_gpio_capability        VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _gpio_capability)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_GPIO_CFG_REIMPLEMENT_API_SET_INPUT == DISABLED
void vsf_real_gpio_set_input(vsf_real_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    VSF_HAL_ASSERT(0 != pin_mask);

    vsf_real_gpio_set_direction(gpio_ptr, pin_mask, 0);
}
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_API_SET_OUTPUT == DISABLED
void vsf_real_gpio_set_output(vsf_real_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    VSF_HAL_ASSERT(0 != pin_mask);

    vsf_real_gpio_set_direction(gpio_ptr, pin_mask, pin_mask);
}
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_API_SWITCH_DIRECTION == DISABLED
void vsf_real_gpio_switch_direction(vsf_real_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    VSF_HAL_ASSERT(0 != pin_mask);

    uint32_t ret = ~vsf_real_gpio_get_direction(gpio_ptr, pin_mask);
    vsf_real_gpio_set_direction(gpio_ptr, pin_mask, ret);
}
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_API_SET == DISABLED
void vsf_real_gpio_set(vsf_real_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    VSF_HAL_ASSERT(0 != pin_mask);

    vsf_real_gpio_write(gpio_ptr, pin_mask, pin_mask);
}
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_API_CLEAR == DISABLED
void vsf_real_gpio_clear(vsf_real_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    VSF_HAL_ASSERT(0 != pin_mask);

    vsf_real_gpio_write(gpio_ptr, pin_mask, 0);
}
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_API_OUTPUT_AND_SET == DISABLED
void vsf_real_gpio_output_and_set(vsf_real_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    VSF_HAL_ASSERT(0 != pin_mask);

#if VSF_GPIO_CFG_CAPABILITY_IS_OUTPUT_AND_SET == 0
    VSF_HAL_ASSERT(0);
#else
#if VSF_GPIO_CFG_CHNAGE_DIR_FIRST == ENABLED
    vsf_real_gpio_set_output(gpio_ptr, pin_mask);
    vsf_real_gpio_set(gpio_ptr, pin_mask);
#else
    vsf_real_gpio_set(gpio_ptr, pin_mask);
    vsf_real_gpio_set_output(gpio_ptr, pin_mask);
#endif
#endif
}
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_API_OUTPUT_AND_CLEAR == DISABLED
void vsf_real_gpio_output_and_clear(vsf_real_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)
{
    VSF_HAL_ASSERT(NULL != gpio_ptr);
    VSF_HAL_ASSERT(0 != pin_mask);

#if VSF_GPIO_CFG_CAPABILITY_IS_OUTPUT_AND_SET == 0
    VSF_HAL_ASSERT(0);
#else
#if VSF_GPIO_CFG_CHNAGE_DIR_FIRST == ENABLED
    vsf_real_gpio_set_output(gpio_ptr, pin_mask);
    vsf_real_gpio_clear(gpio_ptr, pin_mask);
#else
    vsf_real_gpio_clear(gpio_ptr, pin_mask);
    vsf_real_gpio_set_output(gpio_ptr, pin_mask);
#endif
#endif
}
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
vsf_gpio_capability_t vsf_real_gpio_capability(vsf_real_gpio_t *gpio_ptr)
{
    vsf_gpio_capability_t gpio_capability = {
        .is_async = VSF_GPIO_CFG_CAPABILITY_IS_ASYNC,
        .is_support_output_and_set = VSF_GPIO_CFG_CAPABILITY_IS_OUTPUT_AND_SET,
        .is_support_output_and_clear = VSF_GPIO_CFG_CAPABILITY_IS_OUTPUT_AND_CLEAR,
        .pin_count = VSF_GPIO_CFG_CAPABILITY_PIN_COUNT,
        .avail_pin_mask = VSF_GPIO_CFG_CAPABILITY_AVAIL_PIN_MASK,
    };

    return gpio_capability;
}
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#define __VSF_HAL_TEMPLATE_NAME                     _gpio
#define __VSF_HAL_TEMPLATE_UPCASE_NAME              _GPIO

#ifndef VSF_GPIO_CFG_IMP_PREFIX
#   error "Please define VSF_GPIO_CFG_IMP_PREFIX in gpio driver"
#endif

#ifndef VSF_GPIO_CFG_IMP_UPCASE_PREFIX
#   error "Please define VSF_GPIO_CFG_IMP_UPCASE_PREFIX    in gpio driver"
#endif

#include "hal/driver/common/template/vsf_template_implementation.h"

#endif // VSF_HAL_USE_GPIO == ENABLED
