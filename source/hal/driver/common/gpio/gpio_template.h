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
/*============================ MACROFIED FUNCTIONS ===========================*/

#ifndef VSF_HAL_TEMPLATE_DEC_RETAIN_DEFINED_PREFIX
#   define VSF_HAL_TEMPLATE_DEC_RETAIN_DEFINED_PREFIX   DISABLED
#endif

#ifndef VSF_HAL_TEMPLATE_DEC_NAME
#   define VSF_HAL_TEMPLATE_DEC_NAME                   _gpio
#endif

#ifndef VSF_HAL_TEMPLATE_DEC_UPCASE_NAME
#   define VSF_HAL_TEMPLATE_DEC_UPCASE_NAME            _GPIO
#endif

#ifndef VSF_HAL_TEMPLATE_DEC_COUNT_SUFFIX
#   define VSF_HAL_TEMPLATE_DEC_COUNT_SUFFIX           _PORT_COUNT
#endif

#ifndef VSF_HAL_TEMPLATE_DEC_MASK_SUFFIX
#   define VSF_HAL_TEMPLATE_DEC_MASK_SUFFIX            _PORT_MASK
#endif

#ifndef __VSF_TEMPLATE_GPIO_H__
#   error "Please include \"vsf_template_gpio.h\" before include gpio_template.h"
#endif

#if !defined(VSF_GPIO_CFG_DEC_PREFIX) && !defined(VSF_GPIO_CFG_DEC_DEVICE_PREFIX)
#   error "Please define VSF_GPIO_CFG_DEC_PREFIX or VSF_GPIO_CFG_DEC_DEVICE_PREFIX before include gpio_template.h"
#endif

#if !defined(VSF_GPIO_CFG_DEC_UPCASE_PREFIX) && !defined(VSF_GPIO_CFG_DEC_DEVICE_UPCASE_PREFIX)
#   error "Please define VSF_GPIO_CFG_DEC_UPCASE_PREFIX or VSF_GPIO_CFG_DEC_DEVICE_UPCASE_PREFIX before include gpio_template.h"
#endif

#ifndef VSF_GPIO_CFG_DEC_REMAP_PREFIX
#   define VSF_GPIO_CFG_DEC_REMAP_PREFIX                VSF_GPIO_CFG_DEC_PREFIX
#endif

#ifndef VSF_GPIO_CFG_DEC_COUNT_MASK_PREFIX
#   define VSF_GPIO_CFG_DEC_COUNT_MASK_PREFIX           VSF_GPIO_CFG_DEC_UPCASE_PREFIX
#endif

#define VSF_HAL_TEMPLATE_DEC_INSTANCE_APIS(__prefix_name)                       \
    VSF_GPIO_APIS(__prefix_name)                                                \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t, gpio, port_config_pin,   VSF_MCONNECT(__prefix_name, _t) *gpio_ptr, uint16_t pin_index, vsf_gpio_cfg_t * cfg_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t, gpio, ports_config_pin,  vsf_gpio_port_cfg_pin_t *cfg_ptr, uint_fast8_t count)                                         \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t, gpio, ports_config_pins, vsf_gpio_port_cfg_pins_t *cfg_ptr, uint_fast8_t count)

#include "hal/driver/common/template/vsf_template_instance_declaration.h"

#if VSF_HAL_TEMPLATE_DEC_RETAIN_DEFINED_PREFIX == DISABLED
#   undef VSF_GPIO_CFG_DEC_PREFIX
#   undef VSF_GPIO_CFG_DEC_UPCASE_PREFIX
#   undef VSF_GPIO_CFG_DEC_DEVICE_PREFIX
#   undef VSF_GPIO_CFG_DEC_DEVICE_UPCASE_PREFIX
#   undef VSF_GPIO_CFG_DEC_RENAME_DEVICE_PREFIX
#endif

#undef VSF_GPIO_CFG_DEC_REMAP_PREFIX
#undef VSF_GPIO_CFG_DEC_COUNT_MASK_PREFIX
#undef VSF_GPIO_CFG_DEC_EXTERN_OP
#undef VSF_GPIO_CFG_IMP_LV0

/*============================ INCLUDES ======================================*/


