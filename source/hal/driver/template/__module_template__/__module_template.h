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

#ifndef __HAL_EXAMPLE_MODULE_H__
#define __HAL_EXAMPLE_MODULE_H__

/*============================ INCLUDES ======================================*/

// Get the configuration of vsf hal
// Note: It does not contain various peripheral (usart/spi/i2c) templates
#include "hal/vsf_hal_cfg.h"

/*============================ MACROS ========================================*/

// Peripherals that this module depends on
#if VSF_HAL_USE_ADC == ENABLED && VSF_HAL_USE_EXAMPLE_MODULE == ENABLED

// If this module is outside source/hal/driver, you need to include hal/vsf_hal.h here
// #include "hal/vsf_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

// Multiple classes allowed to be closed
#ifndef VSF_EXAMPLE_MODULE_ADC_CFG_MULTI_CLASS
#   define VSF_EXAMPLE_MODULE_ADC_CFG_MULTI_CLASS      VSF_ADC_CFG_MULTI_CLASS
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct example_module_adc_t {
#if VSF_HAL_DISTBUS_ADC_CFG_MULTI_CLASS == ENABLED
    vsf_adc_t adc;
#endif

    uint8_t dummy;
} example_module_adc_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*============================ INCLUDES ======================================*/

// The template here is used to generate the module's declaration
#define VSF_ADC_CFG_DEC_PREFIX              example_module
#define VSF_ADC_CFG_DEC_UPCASE_PREFIX       EXAMPLE_MODULE
#define VSF_I2C_CFG_DEC_EXTERN_OP           ENABLED
#include "hal/driver/common/adc/adc_template.h"

#ifdef __cplusplus
}
#endif

#endif /* VSF_HAL_USE_ADC == ENABLED && VSF_HAL_USE_EXAMPLE_MODULE == ENABLED */
#endif /* __HAL_EXAMPLE_MODULE_H__ */
/* EOF */
