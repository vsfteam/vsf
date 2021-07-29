/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
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
#include "./kernel/vsf_kernel.h"
#include "./hal/driver/AIC/AIC8800/vendor/plf/aic8800/src/driver/gpadc/gpadc_api.h"
#include "./hal/driver/AIC/AIC8800/vendor/plf/aic8800/src/driver/aic1000lite_regs/aic1000Lite_msadc.h"
#include "./hal/driver/AIC/AIC8800/vendor/plf/aic8800/src/driver/aic1000lite_regs/aic1000Lite_analog_reg.h"
#include "./hal/driver/AIC/AIC8800/vendor/plf/aic8800/src/driver/aic1000lite_regs/aic1000Lite_rtc_core.h"
#include "./hal/driver/AIC/AIC8800/vendor/plf/aic8800/src/driver/pmic/pmic_api.h"

/*============================ MACROS ========================================*/

#if     defined(__VSF_ADC_CLASS_IMPLEMENT)
#   undef __VSF_ADC_CLASS_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT__
#endif

#ifndef VSF_ADC_CFG_CALLBACK_TIME_POSTPONE_US
#   define VSF_ADC_CFG_CALLBACK_TIME_POSTPONE_US                    10
#endif

#ifndef VSF_ADC_CFG_CHANNEL_COUNT
#   define VSF_ADC_CFG_CHANNEL_COUNT                                8
#endif

#define VSF_HAL_ADC_IMP_REQUEST_MULTI                               ENABLED

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum ad_feature_t{
    ADC_REF_VDD_1           = 0,                //ADC_TYPE_VBAT
    ADC_REF_VDD_1_2,                            //Not activated
    ADC_REF_VDD_1_3,                            //ADC_TYPE_TEMP0
    ADC_REF_VDD             = ADC_REF_VDD_1 | ADC_REF_VDD_1_2 | ADC_REF_VDD_1_3,

    DATA_ALIGN_RIGHT        = 0,                //Not activated
    DATA_ALIGN_LEFT         = 0,                //Not activated
    SCAN_CONV_SINGLE_MODE   = 0,                //Not activated
    SCAN_CONV_SEQUENCE_MODE = 0,                //Not activated
    EXTERN_TRIGGER_0        = 0,                //Not activated
    EXTERN_TRIGGER_1        = 0,                //Not activated
    EXTERN_TRIGGER_2        = 0,                //Not activated
};

enum ad_channel_feature_t {
    ADC_CHANNEL_GAIN_1_6    = (0 << 0),         //Not activated
    ADC_CHANNEL_GAIN_1_5    = (1 << 0),         //Not activated
    ADC_CHANNEL_GAIN_1_4    = (2 << 0),         //Not activated
    ADC_CHANNEL_GAIN_1_3    = (3 << 0),         //Not activated
    ADC_CHANNEL_GAIN_1_2    = (4 << 0),         //Not activated
    ADC_CHANNEL_GAIN_1      = (5 << 0),         //Not activated

    ADC_CHANNEL_REF_VDD_1   = (0 << 4),         //Not activated
    ADC_CHANNEL_REF_VDD_1_2 = (1 << 4),         //Not activated
    ADC_CHANNEL_REF_VDD_1_3 = (2 << 4),         //Not activated
    ADC_CHANNEL_REF_VDD_1_4 = (3 << 4),         //Not activated
};

/*============================ INCLUDES ======================================*/

#include "utilities/ooc_class.h"
#include "hal/driver/common/template/vsf_template_adc.h"
#include "hal/driver/common/adc/__adc_common.h"

/*============================ TYPES =========================================*/

vsf_class(vsf_adc_t) {
    public_member(
        void                    *data;
    )
    private_member(
        vsf_callback_timer_t    callback_timer;
        adc_channel_cfg_t       *current_channel;
        adc_channel_cfg_t       cfg_channel[VSF_ADC_CFG_CHANNEL_COUNT];
        adc_cfg_t               cfg;
        uint_fast32_t           channel_count;
        uint_fast32_t           channel_index;
        vsf_hal_adc_def_req_by_once();
        struct {
            uint32_t            is_enable       : 1;
            uint32_t            is_busy         : 1;
            uint32_t            is_irq          : 1;
            uint32_t                            : 29;
        } status;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern vsf_adc_t vsf_adc0;

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#endif /* VSF_HAL_USE_AD */
#endif /* EOF */
