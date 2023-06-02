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

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_ADC == ENABLED && VSF_HAL_USE_EXAMPLE_MODULE == ENABLED

// At least hal/vsf_hal.h (recommended) or hal/driver/driver.h should be included to
// provide complete type information
//#include "hal/driver/driver.h"
#include "hal/vsf_hal.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/*
//Implement required api
vsf_err_t vsf_example_adc_init(vsf_adc_t *adc_ptr, vsf_adc_cfg_t *cfg_ptr)
{
    return VSF_ERR_NONE;
}

....

*/


// Generate the type definition needed for the example
#define VSF_ADC_CFG_IMP_PREFIX              example_module
#define VSF_ADC_CFG_IMP_UPCASE_PREFIX       EXAMPLE_MODULE
#define VSF_ADC_CFG_IMP_EXTERN_OP           ENABLED
#include "hal/driver/common/adc/adc_template.inc"

/* EOF */
