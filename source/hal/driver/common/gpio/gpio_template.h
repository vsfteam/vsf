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
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or DEClied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

/*============================ INCLUDES ======================================*/

#define VSF_HAL_TEMPLATE_DEC_RETAIN_DEFINED_PREFIX  ENABLED
#include "hal/driver/common/gpio/gpio_template_base.h"
#undef VSF_HAL_TEMPLATE_DEC_RETAIN_DEFINED_PREFIX

#define VSF_GPIO_EXTI_DEC_PORT_MASK                 VSF_MCONNECT(VSF_GPIO_CFG_DEC_UPCASE_PREFIX, _GPIO_EXTI_PORT_MASK)
#define VSF_GPIO_EXTI_DEC_PORT_COUNT                VSF_MCONNECT(VSF_GPIO_CFG_DEC_UPCASE_PREFIX, _GPIO_EXTI_PORT_COUNT)

#if !defined(VSF_GPIO_EXTI_DEC_MACRO_ONCE) && ((VSF_GPIO_EXTI_DEC_PORT_COUNT > 0) || (VSF_GPIO_EXTI_DEC_PORT_MASK > 0))
#   define VSF_HAL_TEMPLATE_DEC_PREFIX              VSF_MCONNECT(VSF_GPIO_CFG_DEC_PREFIX, _exti)
#   define VSF_HAL_TEMPLATE_DEC_INSTANCE_PREFIX     VSF_MCONNECT(VSF_GPIO_CFG_DEC_PREFIX, _exti)
#   define VSF_HAL_TEMPLATE_DEC_UPCASE_PREFIX       VSF_MCONNECT(VSF_GPIO_CFG_DEC_UPCASE_PREFIX, _EXTI)

#   define VSF_HAL_TEMPLATE_DEC_COUNT_MASK_PREFIX   VSF_GPIO_CFG_DEC_UPCASE_PREFIX
#   define VSF_HAL_TEMPLATE_DEC_COUNT_SUFFIX        _EXTI_PORT_COUNT
#   define VSF_HAL_TEMPLATE_DEC_MASK_SUFFIX         _EXTI_PORT_MASK

#   define VSF_GPIO_CFG_DEC_REMAP_PREFIX            vsf_exti
#   include "hal/driver/common/gpio/gpio_template_base.h"
#   undef VSF_HAL_TEMPLATE_DEC_RETAIN_DEFINED_PREFIX

#   undef VSF_GPIO_EXTI_DEC_PORT_MASK
#   undef VSF_GPIO_EXTI_DEC_PORT_COUNT
#   define VSF_GPIO_EXTI_DEC_MACRO_ONCE
#else
#   undef VSF_GPIO_CFG_DEC_PREFIX
#   undef VSF_GPIO_CFG_DEC_UPCASE_PREFIX
#endif

#include "hal/driver/common/gpio/exti_gpio.h"
