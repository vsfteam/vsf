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

#if VSF_HAL_USE_ADC == ENABLED

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

#define vsf_real_adc_t          VSF_MCONNECT(VSF_ADC_CFG_PREFIX, _adc_t)
#define vsf_real_adc_capability VSF_MCONNECT(VSF_ADC_CFG_PREFIX, _adc_capability)

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_ADC_CFG_REIMPLEMENT_CAPABILITY == DISABLED
adc_capability_t vsf_real_adc_capability(vsf_real_adc_t *adc_ptr)
{
    adc_capability_t adc_capability = {
        0,
    };

    return adc_capability;
}
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#define __VSF_HAL_TEMPLATE_NAME                     _adc
#define __VSF_HAL_TEMPLATE_UPCASE_NAME              _ADC

#ifndef VSF_ADC_CFG_PREFIX
#   error "Please define VSF_ADC_CFG_PREFIX in adc driver"
#endif

#ifndef VSF_ADC_CFG_UPPERCASE_PREFIX
#   error "Please define VSF_ADC_CFG_UPPERCASE_PREFIX in adc driver"
#endif

#include "hal/driver/common/template/vsf_template_implementation.h"

#endif
