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

#if VSF_HAL_USE_GPIO == ENABLED

#define VSF_GPIO_CFG_RETAIN_DEFINED_PREFIX      ENABLED
#include "hal/driver/common/gpio/gpio_template_base.inc"
#undef VSF_GPIO_CFG_RETAIN_DEFINED_PREFIX

#define VSF_IMP_GPIO_EXTI_PORT_MASK                 VSF_MCONNECT(VSF_GPIO_CFG_IMP_UPCASE_PREFIX, _GPIO_EXTI_PORT_MASK)
#define VSF_GPIO_EXTI_IMP_PORT_COUNT                VSF_MCONNECT(VSF_GPIO_CFG_IMP_UPCASE_PREFIX, _GPIO_EXTI_PORT_COUNT)

#if !defined(VSF_GPIO_EXTI_IMP_MACRO_ONCE) && ((VSF_GPIO_EXTI_IMP_PORT_COUNT > 0) || (VSF_IMP_GPIO_EXTI_PORT_MASK > 0))
#   define VSF_GPIO_CFG_IMP_REMAP_PREFIX            vsf_exti

#   define VSF_HAL_TEMPLATE_IMP_COUNT_MASK_PREFIX   VSF_GPIO_CFG_IMP_UPCASE_PREFIX
#   define VSF_HAL_TEMPLATE_IMP_COUNT_SUFFIX        _EXTI_PORT_COUNT
#   define VSF_HAL_TEMPLATE_IMP_MASK_SUFFIX         _EXTI_PORT_MASK

#   define VSF_GPIO_CFG_IMP_INSTANCE                VSF_GPIO_CFG_IMP_PREFIX
#   define VSF_GPIO_CFG_REAL_PREFIX                 VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _exti)
#   define VSF_GPIO_CFG_ORIGIN_PREFIX               VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _exti)
#   define VSF_HAL_TEMPLATE_IMP_PREFIX              VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _exti)
#   define VSF_HAL_TEMPLATE_IMP_INSTANCE_PREFIX     VSF_MCONNECT(VSF_GPIO_CFG_IMP_PREFIX, _exti)
#   define VSF_HAL_TEMPLATE_IMP_UPCASE_PREFIX       VSF_MCONNECT(VSF_GPIO_CFG_IMP_UPCASE_PREFIX, _EXTI)

#   define VSF_GPIO_CFG_IMP_LV0(__IDX, __HAL_OP)                                \
        describe_exti_gpio(                                                     \
            VSF_MCONNECT(VSF_HAL_TEMPLATE_IMP_PREFIX, _gpio, __IDX),            \
            VSF_MCONNECT(VSF_GPIO_CFG_IMP_INSTANCE, _gpio, __IDX),              \
            VSF_MCONNECT(VSF_GPIO_CFG_IMP_UPCASE_PREFIX, _GPIO_PIN_COUNT),      \
            __HAL_OP                                                            \
        )
#   include "hal/driver/common/gpio/gpio_template_base.inc"
#   undef VSF_GPIO_CFG_RETAIN_DEFINED_PREFIX

#   undef VSF_IMP_GPIO_EXTI_PORT_MASK
#   undef VSF_GPIO_EXTI_IMP_PORT_COUNT
#   define VSF_GPIO_EXTI_IMP_MACRO_ONCE
#else
#   undef VSF_GPIO_CFG_IMP_PREFIX
#   undef VSF_GPIO_CFG_IMP_UPCASE_PREFIX
#endif

#endif  /* VSF_HAL_USE_GPIO */
