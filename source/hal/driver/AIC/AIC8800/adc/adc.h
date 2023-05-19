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

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ INCLUDES ======================================*/

#endif /* VSF_HAL_USE_AD */
#endif /* EOF */
