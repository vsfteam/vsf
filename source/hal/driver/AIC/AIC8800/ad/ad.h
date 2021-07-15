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

#include "./kernel/vsf_kernel.h"
#include "hal/vsf_hal_cfg.h"
#if VSF_HAL_USE_AD == ENABLED
#include "./hal/driver/AIC/AIC8800/vendor/plf/aic8800/src/driver/gpadc/gpadc_api.h"
#include "./hal/driver/AIC/AIC8800/vendor/plf/aic8800/src/driver/aic1000lite_regs/aic1000Lite_msadc.h"
#include "./hal/driver/AIC/AIC8800/vendor/plf/aic8800/src/driver/aic1000lite_regs/aic1000Lite_analog_reg.h"
#include "./hal/driver/AIC/AIC8800/vendor/plf/aic8800/src/driver/aic1000lite_regs/aic1000Lite_rtc_core.h"
#include "./hal/driver/AIC/AIC8800/vendor/plf/aic8800/src/driver/pmic/pmic_api.h"
#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/

#if     defined(__VSF_AD_CLASS_IMPLEMENT)
#   undef __VSF_AD_CLASS_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT__
#endif

#ifndef VSF_AD_CFG_CALLBACK_TIME_US
#   define VSF_AD_CFG_CALLBACK_TIME_US                          1000
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum ad_feature_t{
    DATA_ALIGN_RIGHT        = (0 << 0),  //!< ADC data alignment to right
    DATA_ALIGN_LEFT         = (1 << 0),  //!< ADC data alignment to left

    SCAN_CONV_SINGLE_MODE   = (0 << 1),  //!< Conversion is performed in single mode
    SCAN_CONV_SEQUENCE_MODE = (1 << 1),  //!< Conversions are performed in sequence mode

    //! Selects the external event used to trigger the conversion
    EXTERN_TRIGGER_0        = (0 << 2),
    EXTERN_TRIGGER_1        = (1 << 2),
    EXTERN_TRIGGER_2        = (2 << 2),
};

/*============================ INCLUDES ======================================*/

#include "hal/driver/common/template/vsf_template_ad.h"

/*============================ TYPES =========================================*/

vsf_class(vsf_ad_t) {
    public_member(
        void                    *data;
    )
    private_member(
        vsf_callback_timer_t    callback_timer;
        ad_channel_cfg_t        *cfg_channel;
        ad_cfg_t                cfg;
        uint_fast32_t           data_count;
        uint_fast32_t           data_index;
        uint_fast32_t           channel_count;
        uint_fast32_t           channel_index;
        struct {
            uint32_t            is_enable   : 1;
            uint32_t            is_busy     : 1;
            uint32_t            is_irq      : 1;
            uint32_t                        : 29;
        } status;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern vsf_ad_t vsf_ad0;

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#endif /* VSF_HAL_USE_AD */
#endif /* EOF */