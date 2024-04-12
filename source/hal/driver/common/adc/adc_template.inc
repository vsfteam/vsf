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

#define vsf_real_adc_t                                          VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t)
#define vsf_real_adc_capability                                 VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_capability)

#ifndef VSF_ADC_CFG_REIMPLEMENT_API_CAPABILITY
#   define  VSF_ADC_CFG_REIMPLEMENT_API_CAPABILITY              DISALBED
#endif

#if VSF_ADC_CFG_REIMPLEMENT_API_CAPABILITY == ENABLED
#   ifndef VSF_ADC_CFG_CAPABILITY_IRQ_MASK
#       define VSF_ADC_CFG_CAPABILITY_IRQ_MASK                  VSF_ADC_IRQ_ALL_BITS_MASK
#   endif
#   ifndef VSF_ADC_CFG_CAPABILITY_MAX_DATA_BITS
#       define VSF_ADC_CFG_CAPABILITY_MAX_DATA_BITS             12
#   endif
#   ifndef VSF_ADC_CFG_CAPABILITY_CHANNEL_COUNT
#       define VSF_ADC_CFG_CAPABILITY_CHANNEL_COUNT             4
#   endif
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_ADC_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
vsf_adc_capability_t vsf_real_adc_capability(vsf_real_adc_t *adc_ptr)
{
    vsf_adc_capability_t adc_capability = {
        .irq_mask       = VSF_ADC_CFG_CAPABILITY_IRQ_MASK,
        .max_data_bits  = VSF_ADC_CFG_CAPABILITY_MAX_DATA_BITS,
        .channel_count  = VSF_ADC_CFG_CAPABILITY_CHANNEL_COUNT,
    };

    return adc_capability;
}
#endif

/*============================ MACROS ========================================*/

#undef VSF_ADC_CFG_REIMPLEMENT_API_CAPABILITY
#undef VSF_ADC_CFG_CAPABILITY_IRQ_MASK
#undef VSF_ADC_CFG_CAPABILITY_MAX_DATA_BITS
#undef VSF_ADC_CFG_CAPABILITY_CHANNEL_COUNT

#undef vsf_real_adc_t
#undef vsf_real_adc_capability

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/
/*============================ MACROS ========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

#define VSF_HAL_TEMPLATE_IMP_NAME                     _adc
#define VSF_HAL_TEMPLATE_IMP_UPCASE_NAME              _ADC

#ifndef VSF_ADC_CFG_IMP_PREFIX
#   error "Please define VSF_ADC_CFG_IMP_PREFIX in adc driver"
#endif

#ifndef VSF_ADC_CFG_IMP_UPCASE_PREFIX
#   error "Please define VSF_ADC_CFG_IMP_UPCASE_PREFIX in adc driver"
#endif

#ifndef VSF_ADC_CFG_IMP_INSTANCE_PREFIX
#   define VSF_ADC_CFG_IMP_INSTANCE_PREFIX         VSF_ADC_CFG_IMP_PREFIX
#endif

#include "hal/driver/common/template/vsf_template_instance_implementation.h"

#undef VSF_ADC_CFG_IMP_PREFIX
#undef VSF_ADC_CFG_IMP_INSTANCE_PREFIX
#undef VSF_ADC_CFG_IMP_UPCASE_PREFIX
#undef VSF_ADC_CFG_IMP_LV0

#endif  /* VSF_HAL_USE_ADC */
