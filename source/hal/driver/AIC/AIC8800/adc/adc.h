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

#ifndef __HAL_DRIVER_AIC8800_AD_H__
#define __HAL_DRIVER_AIC8800_AD_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"
#if VSF_HAL_USE_ADC == ENABLED

/*============================ MACROS ========================================*/

#ifndef VSF_HW_ADC_CFG_CALLBACK_TIME_POSTPONE_US
#   define VSF_HW_ADC_CFG_CALLBACK_TIME_POSTPONE_US                 10
#endif

#ifndef VSF_HW_ADC_CFG_CHANNEL_COUNT
#   define VSF_HW_ADC_CFG_CHANNEL_COUNT                             8
#endif

#define VSF_ADC_CFG_REIMPLEMENT_TYPE_FEATURE                             ENABLED
#define VSF_ADC_CFG_REIMPLEMENT_TYPE_CHANNEL_FEATURE                     ENABLED

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum vsf_adc_feature_t{
    VSF_ADC_REF_VDD_1           = 0,                //ADC_TYPE_VBAT
    VSF_ADC_REF_VDD_1_2,                            //Not activated
    VSF_ADC_REF_VDD_1_3,                            //ADC_TYPE_TEMP0
    ADC_REF_VDD             = VSF_ADC_REF_VDD_1 | VSF_ADC_REF_VDD_1_2 | VSF_ADC_REF_VDD_1_3,

    VSF_DATA_ALIGN_RIGHT        = 0,                //Not activated
    VSF_DATA_ALIGN_LEFT         = 0,                //Not activated
    VSF_SCAN_CONV_SINGLE_MODE   = 0,                //Not activated
    VSF_SCAN_CONV_SEQUENCE_MODE = 0,                //Not activated
    VSF_EXTERN_TRIGGER_0        = 0,                //Not activated
    VSF_EXTERN_TRIGGER_1        = 0,                //Not activated
    VSF_EXTERN_TRIGGER_2        = 0,                //Not activated
} vsf_adc_feature_t;

typedef enum vsf_adc_channel_feature_t {
    VSF_ADC_CHANNEL_GAIN_1_6    = (0 << 0),         //Not activated
    VSF_ADC_CHANNEL_GAIN_1_5    = (1 << 0),         //Not activated
    VSF_ADC_CHANNEL_GAIN_1_4    = (2 << 0),         //Not activated
    VSF_ADC_CHANNEL_GAIN_1_3    = (3 << 0),         //Not activated
    VSF_ADC_CHANNEL_GAIN_1_2    = (4 << 0),         //Not activated
    VSF_ADC_CHANNEL_GAIN_1      = (5 << 0),         //Not activated

    VSF_ADC_CHANNEL_REF_VDD_1   = (0 << 4),         //Not activated
    VSF_ADC_CHANNEL_REF_VDD_1_2 = (1 << 4),         //Not activated
    VSF_ADC_CHANNEL_REF_VDD_1_3 = (2 << 4),         //Not activated
    VSF_ADC_CHANNEL_REF_VDD_1_4 = (3 << 4),         //Not activated
} vsf_adc_channel_feature_t;

/*============================ INCLUDES ======================================*/

#define VSF_ADC_CFG_DEC_PREFIX                  vsf_hw
#define VSF_ADC_CFG_DEC_UPCASE_PREFIX           VSF_HW

#include "hal/driver/common/adc/adc_template.h"

#endif /* VSF_HAL_USE_AD */
#endif /* EOF */
