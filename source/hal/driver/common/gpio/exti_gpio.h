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

#ifndef __HAL_DRIVER_GPIO_EXTI_H__
#define __HAL_DRIVER_GPIO_EXTI_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_GPIO == ENABLED

#if defined(__VSF_HAL_USE_GPIO_EXTI_CLASS_IMPLEMENT)
#   undef __VSF_HAL_USE_GPIO_EXTI_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ MACROS ========================================*/

#ifndef VSF_EXTI_GPIO_CFG_MULTI_CLASS
#   define VSF_EXTI_GPIO_CFG_MULTI_CLASS   VSF_GPIO_CFG_MULTI_CLASS
#endif


#define __describe_exti_gpio(__name, __gpio, __pin_count, ...)                  \
    static vsf_gpio_irq_distributor_irq_t                                       \
        VSF_MCONNECT(__name, _irqs)[__pin_count];                               \
    vsf_gpio_irq_distributor_t __name = {                                       \
        .gpio = (vsf_gpio_t *)&__gpio,                                          \
        .prio = vsf_arch_prio_invalid,                                          \
        .exti_irq = VSF_MCONNECT(__name, _irqs),                                \
        __VA_ARGS__                                                             \
    };

#define describe_gpio_irq_distributor(__name, __gpio, ...)                      \
    __describe_exti_gpio(__name, __gpio, __VA_ARGS__)


/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_gpio_irq_distributor_irq_t {
    vsf_gpio_exti_isr_handler_t *handler_fn;
    void                        *target_ptr;
} vsf_gpio_irq_distributor_irq_t;

typedef struct vsf_gpio_irq_distributor_t {
    vsf_gpio_t          *gpio;
    vsf_arch_prio_t      prio;
    vsf_gpio_irq_distributor_irq_t *exti_irq;
} vsf_gpio_irq_distributor_t;

/*============================ INCLUDES ======================================*/

#define VSF_GPIO_CFG_DEC_PREFIX              vsf_exti
#define VSF_GPIO_CFG_DEC_UPCASE_PREFIX       vsf_exti
#define VSF_GPIO_CFG_DEC_EXTERN_OP           ENABLED
#include "hal/driver/common/gpio/gpio_template.h"

/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vsf_gpio_irq_distributor_pin_config(
    vsf_gpio_irq_distributor_t *hw_exti_gpio_ptr, vsf_gpio_pin_mask_t pin_mask,
    vsf_gpio_exti_irq_cfg_t *irq_cfg_ptr);

/*============================ IMPLEMENTATION ================================*/

#endif

#endif
/* EOF */
