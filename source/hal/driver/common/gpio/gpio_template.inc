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
#   define VSF_GPIO_CFG_REIMPLEMENT_API_SET_INPUT               DISABLED
#endif

#ifndef VSF_GPIO_CFG_REIMPLEMENT_API_SET_OUTPUT
#   define VSF_GPIO_CFG_REIMPLEMENT_API_SET_OUTPUT              DISABLED
#endif

#ifndef VSF_GPIO_CFG_REIMPLEMENT_API_SWITCH_DIRECTION
#   define VSF_GPIO_CFG_REIMPLEMENT_API_SWITCH_DIRECTION        DISABLED
#endif

#ifndef VSF_GPIO_CFG_REIMPLEMENT_API_SET
#   define VSF_GPIO_CFG_REIMPLEMENT_API_SET                     DISABLED
#endif

#ifndef VSF_GPIO_CFG_REIMPLEMENT_API_CLEAR
#   define VSF_GPIO_CFG_REIMPLEMENT_API_CLEAR                   DISABLED
#endif

#ifndef VSF_GPIO_CFG_REIMPLEMENT_API_OUTPUT_AND_SET
#   define VSF_GPIO_CFG_REIMPLEMENT_API_OUTPUT_AND_SET          DISABLED
#endif

#ifndef VSF_GPIO_CFG_REIMPLEMENT_API_OUTPUT_AND_CLEAR
#   define VSF_GPIO_CFG_REIMPLEMENT_API_OUTPUT_AND_CLEAR        DISABLED
#endif

#ifndef VSF_GPIO_CFG_CHANGE_DIR_FIRST
#   define VSF_GPIO_CFG_CHANGE_DIR_FIRST                        ENABLED
#endif

#ifndef VSF_GPIO_CFG_CAPABILITY_SUPPORT_CONFIG_PIN
#    define VSF_GPIO_CFG_CAPABILITY_SUPPORT_CONFIG_PIN          1
#endif

#ifndef VSF_GPIO_CFG_CAPABILITY_SUPPORT_OUTPUT_AND_SET
#    define VSF_GPIO_CFG_CAPABILITY_SUPPORT_OUTPUT_AND_SET      0
#endif

#ifndef VSF_GPIO_CFG_CAPABILITY_SUPPORT_OUTPUT_AND_CLEAR
#    define VSF_GPIO_CFG_CAPABILITY_SUPPORT_OUTPUT_AND_CLEAR    0
#endif

#ifndef VSF_GPIO_CFG_CAPABILITY_SUPPORT_INTERRUPT
#    define VSF_GPIO_CFG_CAPABILITY_SUPPORT_INTERRUPT           1
#endif

#ifndef VSF_GPIO_CFG_REIMPLEMENT_API_CAPABILITY
#   define  VSF_GPIO_CFG_REIMPLEMENT_API_CAPABILITY              DISABLED
#endif

#if VSF_GPIO_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
#   ifndef VSF_GPIO_CFG_CAPABILITY_IS_ASYNC
#       define VSF_GPIO_CFG_CAPABILITY_IS_ASYNC                 0
#   endif

#   ifndef VSF_GPIO_CFG_CAPABILITY_PIN_COUNT
#       define VSF_GPIO_CFG_CAPABILITY_PIN_COUNT                32
#   endif
#   ifndef VSF_GPIO_CFG_CAPABILITY_PIN_MASK
#       define VSF_GPIO_CFG_CAPABILITY_PIN_MASK                 0xFFFFFFFF
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

#if VSF_GPIO_CFG_CAPABILITY_SUPPORT_OUTPUT_AND_SET == 0
    VSF_HAL_ASSERT(0);
#else
#if VSF_GPIO_CFG_CHANGE_DIR_FIRST == ENABLED
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

#if VSF_GPIO_CFG_CAPABILITY_SUPPORT_OUTPUT_AND_CLEAR == 0
    VSF_HAL_ASSERT(0);
#else
#if VSF_GPIO_CFG_CHANGE_DIR_FIRST == ENABLED
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
        .is_async                   = VSF_GPIO_CFG_CAPABILITY_IS_ASYNC,
        .support_config_pin         = VSF_GPIO_CFG_CAPABILITY_SUPPORT_CONFIG_PIN,
        .support_output_and_set     = VSF_GPIO_CFG_CAPABILITY_SUPPORT_OUTPUT_AND_SET,
        .support_output_and_clear   = VSF_GPIO_CFG_CAPABILITY_SUPPORT_OUTPUT_AND_CLEAR,
        .support_interrupt          = VSF_GPIO_CFG_CAPABILITY_SUPPORT_INTERRUPT,
        .pin_count                  = VSF_GPIO_CFG_CAPABILITY_PIN_COUNT,
        .pin_mask                   = VSF_GPIO_CFG_CAPABILITY_PIN_MASK,
    };

    return gpio_capability;
}
#endif

/*============================ MACROS ========================================*/

#undef VSF_GPIO_CFG_REIMPLEMENT_API_SET_INPUT
#undef VSF_GPIO_CFG_REIMPLEMENT_API_SET_OUTPUT
#undef VSF_GPIO_CFG_REIMPLEMENT_API_SWITCH_DIRECTION
#undef VSF_GPIO_CFG_REIMPLEMENT_API_SET
#undef VSF_GPIO_CFG_REIMPLEMENT_API_CLEAR
#undef VSF_GPIO_CFG_REIMPLEMENT_API_OUTPUT_AND_SET
#undef VSF_GPIO_CFG_REIMPLEMENT_API_OUTPUT_AND_CLEAR
#undef VSF_GPIO_CFG_CHANGE_DIR_FIRST
#undef VSF_GPIO_CFG_CAPABILITY_SUPPORT_CONFIG_PIN
#undef VSF_GPIO_CFG_CAPABILITY_SUPPORT_OUTPUT_AND_SET
#undef VSF_GPIO_CFG_CAPABILITY_SUPPORT_OUTPUT_AND_CLEAR
#undef VSF_GPIO_CFG_CAPABILITY_SUPPORT_INTERRUPT
#undef VSF_GPIO_CFG_REIMPLEMENT_API_CAPABILITY
#undef VSF_GPIO_CFG_CAPABILITY_IS_ASYNC
#undef VSF_GPIO_CFG_CAPABILITY_PIN_COUNT
#undef VSF_GPIO_CFG_CAPABILITY_PIN_MASK
#undef vsf_real_gpio_t
#undef vsf_real_gpio_set_input
#undef vsf_real_gpio_set_output
#undef vsf_real_gpio_get_direction
#undef vsf_real_gpio_set_direction
#undef vsf_real_gpio_switch_direction
#undef vsf_real_gpio_write
#undef vsf_real_gpio_set
#undef vsf_real_gpio_clear
#undef vsf_real_gpio_output_and_set
#undef vsf_real_gpio_output_and_clear
#undef vsf_real_gpio_capability

/*============================ MACROS ========================================*/

#ifdef VSF_GPIO_CFG_IMP_REMAP_PREFIX
#   define vsf_imp_gpio_t                       VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_t)
#   define vsf_imp_gpio_capability              VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_capability)
#   define vsf_imp_gpio_config_pin              VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_config_pin)
#   define vsf_imp_gpio_config_pin_interrupt    VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_config_pin_interrupt)
#   define vsf_imp_gpio_set_direction           VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_set_direction)
#   define vsf_imp_gpio_get_direction           VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_get_direction)
#   define vsf_imp_gpio_set_input               VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_set_input)
#   define vsf_imp_gpio_set_output              VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_set_output)
#   define vsf_imp_gpio_switch_direction        VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_switch_direction)
#   define vsf_imp_gpio_read                    VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_read)
#   define vsf_imp_gpio_write                   VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_write)
#   define vsf_imp_gpio_set                     VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_set)
#   define vsf_imp_gpio_clear                   VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_clear)
#   define vsf_imp_gpio_output_and_set          VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_output_and_set)
#   define vsf_imp_gpio_output_and_clear        VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_output_and_clear)
#   define vsf_imp_gpio_toggle                  VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_toggle)
#   define vsf_imp_gpio_pin_interrupt_enable    VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_pin_interrupt_enable)
#   define vsf_imp_gpio_pin_interrupt_disable   VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_pin_interrupt_disable)
#   define vsf_imp_gpio_pin_interrupt_config    VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_pin_interrupt_config)

#   define vsf_remap_gpio_t                     VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_t)
#   define vsf_remap_gpio_capability            VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_capability)
#   define vsf_remap_gpio_config_pin            VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_config_pin)
#   define vsf_remap_gpio_config_pin_interrupt  VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_config_pin_interrupt)
#   define vsf_remap_gpio_set_direction         VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_set_direction)
#   define vsf_remap_gpio_get_direction         VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_get_direction)
#   define vsf_remap_gpio_set_input             VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_set_input)
#   define vsf_remap_gpio_set_output            VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_set_output)
#   define vsf_remap_gpio_switch_direction      VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_switch_direction)
#   define vsf_remap_gpio_read                  VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_read)
#   define vsf_remap_gpio_write                 VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_write)
#   define vsf_remap_gpio_set                   VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_set)
#   define vsf_remap_gpio_clear                 VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_clear)
#   define vsf_remap_gpio_output_and_set        VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_output_and_set)
#   define vsf_remap_gpio_output_and_clear      VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_output_and_clear)
#   define vsf_remap_gpio_toggle                VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_toggle)
#   define vsf_remap_gpio_pin_interrupt_enable  VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_pin_interrupt_enable)
#   define vsf_remap_gpio_pin_interrupt_disable VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_pin_interrupt_disable)
#   define vsf_remap_gpio_pin_interrupt_config  VSF_MCONNECT(VSF_GPIO_CFG_PREFIX, _gpio_pin_interrupt_config)

#   define VSF_GPIO_CFG_IMP_REMAP_FUNCTIONS                                                                                         \
        void vsf_imp_gpio_config_pin(vsf_impl_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask, vsf_gpio_mode_t feature)              \
        {                                                                                                                           \
            VSF_HAL_ASSERT(gpio_ptr != NULL);                                                                                       \
            vsf_remap_gpio_config_pin(gpio_ptr, pin_mask, feature);                                                                 \
        }                                                                                                                           \
        void vsf_imp_gpio_set_direction(vsf_impl_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask, vsf_gpio_pin_mask_t direction_mask)\
        {                                                                                                                           \
            VSF_HAL_ASSERT(gpio_ptr != NULL);                                                                                       \
            vsf_remap_gpio_set_direction(gpio_ptr, pin_mask, direction_mask);                                                       \
        }                                                                                                                           \
        vsf_gpio_pin_mask_t vsf_imp_gpio_get_direction(vsf_impl_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)                     \
        {                                                                                                                           \
            VSF_HAL_ASSERT(gpio_ptr != NULL);                                                                                       \
            return vsf_remap_gpio_get_direction(gpio_ptr, pin_mask);                                                                \
        }                                                                                                                           \
        void vsf_imp_gpio_set_input(vsf_impl_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)                                        \
        {                                                                                                                           \
            VSF_HAL_ASSERT(gpio_ptr != NULL);                                                                                       \
            vsf_remap_gpio_set_input(gpio_ptr, pin_mask);                                                                           \
        }                                                                                                                           \
        void vsf_imp_gpio_set_output(vsf_impl_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)                                       \
        {                                                                                                                           \
            VSF_HAL_ASSERT(gpio_ptr != NULL);                                                                                       \
            vsf_remap_gpio_set_output(gpio_ptr, pin_mask);                                                                          \
        }                                                                                                                           \
        void vsf_imp_gpio_switch_direction(vsf_impl_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)                                 \
        {                                                                                                                           \
            VSF_HAL_ASSERT(gpio_ptr != NULL);                                                                                       \
            vsf_remap_gpio_switch_direction(gpio_ptr, pin_mask);                                                                    \
        }                                                                                                                           \
        vsf_gpio_pin_mask_t vsf_imp_gpio_read(vsf_impl_gpio_t *gpio_ptr)                                                            \
        {                                                                                                                           \
            VSF_HAL_ASSERT(gpio_ptr != NULL);                                                                                       \
            return vsf_remap_gpio_read(gpio_ptr);                                                                                   \
        }                                                                                                                           \
        void vsf_imp_gpio_write(vsf_impl_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask, vsf_gpio_pin_mask_t value)                 \
        {                                                                                                                           \
            VSF_HAL_ASSERT(gpio_ptr != NULL);                                                                                       \
            vsf_remap_gpio_write(gpio_ptr, pin_mask, value);                                                                        \
        }                                                                                                                           \
        void vsf_imp_gpio_set(vsf_impl_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)                                              \
        {                                                                                                                           \
            VSF_HAL_ASSERT(gpio_ptr != NULL);                                                                                       \
            vsf_remap_gpio_set(gpio_ptr, pin_mask);                                                                                 \
        }                                                                                                                           \
        void vsf_imp_gpio_clear(vsf_impl_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)                                            \
        {                                                                                                                           \
            VSF_HAL_ASSERT(gpio_ptr != NULL);                                                                                       \
            vsf_remap_gpio_clear(gpio_ptr, pin_mask);                                                                               \
        }                                                                                                                           \
        void vsf_imp_gpio_toggle(vsf_impl_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)                                           \
        {                                                                                                                           \
            VSF_HAL_ASSERT(gpio_ptr != NULL);                                                                                       \
            vsf_remap_gpio_toggle(gpio_ptr, pin_mask);                                                                              \
        }                                                                                                                           \
        void vsf_imp_gpio_output_and_set(vsf_impl_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)                                   \
        {                                                                                                                           \
            VSF_HAL_ASSERT(gpio_ptr != NULL);                                                                                       \
            vsf_remap_gpio_output_and_set(gpio_ptr, pin_mask);                                                                      \
        }                                                                                                                           \
        void vsf_imp_gpio_output_and_clear(vsf_impl_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)                                 \
        {                                                                                                                           \
            VSF_HAL_ASSERT(gpio_ptr != NULL);                                                                                       \
            vsf_remap_gpio_output_and_clear(gpio_ptr, pin_mask);                                                                    \
        }                                                                                                                           \
        vsf_gpio_capability_t vsf_imp_gpio_capability(vsf_impl_gpio_t *gpio_ptr)                                                    \
        {                                                                                                                           \
            VSF_HAL_ASSERT(gpio_ptr != NULL);                                                                                       \
            return vsf_remap_gpio_capability(gpio_ptr);                                                                             \
        }                                                                                                                           \
        vsf_err_t vsf_imp_gpio_pin_interrupt_enable(vsf_impl_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask, vsf_arch_prio_t prio)  \
        {                                                                                                                           \
            VSF_HAL_ASSERT(gpio_ptr != NULL);                                                                                       \
            return vsf_remap_gpio_pin_interrupt_enable(gpio_ptr, pin_mask, prio);                                                   \
        }                                                                                                                           \
        vsf_err_t vsf_imp_gpio_pin_interrupt_enable(vsf_impl_gpio_t *gpio_ptr, vsf_gpio_pin_mask_t pin_mask)                        \
        {                                                                                                                           \
            VSF_HAL_ASSERT(gpio_ptr != NULL);                                                                                       \
            return vsf_remap_gpio_pin_interrupt_disable(gpio_ptr, pin_mask);                                                        \
        }                                                                                                                           \
        vsf_err_t vsf_imp_gpio_pin_interrupt_config(vsf_impl_gpio_t *gpio_ptr, vsf_gpio_pin_irq_cfg_t *cfg)                         \
        {                                                                                                                           \
            VSF_HAL_ASSERT(gpio_ptr != NULL);                                                                                       \
            return vsf_remap_gpio_pin_interrupt_config(gpio_ptr, cfg);                                                              \
        }
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#define VSF_HAL_TEMPLATE_IMP_NAME                   _gpio
#define VSF_HAL_TEMPLATE_IMP_UPCASE_NAME            _GPIO
#define VSF_HAL_TEMPLATE_IMP_COUNT_SUFFIX           _PORT_COUNT
#define VSF_HAL_TEMPLATE_IMP_MASK_SUFFIX            _PORT_MASK

#ifndef VSF_GPIO_CFG_IMP_PREFIX
#   error "Please define VSF_GPIO_CFG_IMP_PREFIX in gpio driver"
#endif

#ifndef VSF_GPIO_CFG_IMP_UPCASE_PREFIX
#   error "Please define VSF_GPIO_CFG_IMP_UPCASE_PREFIX in gpio driver"
#endif

#ifndef VSF_GPIO_CFG_IMP_COUNT_MASK_PREFIX
#   define VSF_GPIO_CFG_IMP_COUNT_MASK_PREFIX       VSF_GPIO_CFG_IMP_UPCASE_PREFIX
#endif

#ifdef VSF_GPIO_CFG_IMP_REMAP_FUNCTIONS
#   define VSF_HAL_CFG_IMP_REMAP_FUNCTIONS          VSF_GPIO_CFG_IMP_REMAP_FUNCTIONS
#endif

#include "hal/driver/common/template/vsf_template_instance_implementation.h"

#undef VSF_GPIO_CFG_IMP_PREFIX
#undef VSF_GPIO_CFG_IMP_COUNT_MASK_PREFIX
#undef VSF_GPIO_CFG_IMP_UPCASE_PREFIX
#undef VSF_GPIO_CFG_IMP_LV0
#undef VSF_GPIO_CFG_IMP_REMAP_FUNCTIONS
#undef VSF_GPIO_CFG_IMP_HAS_OP
#undef VSF_GPIO_CFG_IMP_EXTERN_OP

#undef vsf_imp_gpio_t
#undef vsf_imp_gpio_capability
#undef vsf_imp_gpio_config_pin
#undef vsf_imp_gpio_config_pin_interrupt
#undef vsf_imp_gpio_set_direction
#undef vsf_imp_gpio_get_direction
#undef vsf_imp_gpio_set_input
#undef vsf_imp_gpio_set_output
#undef vsf_imp_gpio_switch_direction
#undef vsf_imp_gpio_read
#undef vsf_imp_gpio_write
#undef vsf_imp_gpio_set
#undef vsf_imp_gpio_clear
#undef vsf_imp_gpio_output_and_set
#undef vsf_imp_gpio_output_and_clear
#undef vsf_imp_gpio_toggle
#undef vsf_imp_gpio_pin_interrupt_enable
#undef vsf_imp_gpio_pin_interrupt_config

#undef vsf_remap_gpio_t
#undef vsf_remap_gpio_capability
#undef vsf_remap_gpio_config_pin
#undef vsf_remap_gpio_config_pin_interrupt
#undef vsf_remap_gpio_set_direction
#undef vsf_remap_gpio_get_direction
#undef vsf_remap_gpio_set_input
#undef vsf_remap_gpio_set_output
#undef vsf_remap_gpio_switch_direction
#undef vsf_remap_gpio_read
#undef vsf_remap_gpio_write
#undef vsf_remap_gpio_set
#undef vsf_remap_gpio_clear
#undef vsf_remap_gpio_output_and_set
#undef vsf_remap_gpio_output_and_clear
#undef vsf_remap_gpio_toggle
#undef vsf_remap_gpio_pin_interrupt_enable
#undef vsf_remap_gpio_pin_interrupt_config

#endif  /* VSF_HAL_USE_GPIO */
