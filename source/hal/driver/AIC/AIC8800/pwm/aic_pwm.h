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
 *  limitations under the License.                                           *x
 *                                                                           *
 ****************************************************************************/

#ifndef __HAL_DRIVER_AIC8800_PWM_H__
#define __HAL_DRIVER_AIC8800_PWM_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_PWM == ENABLED

/*============================ MACROS ========================================*/

#ifndef VSF_HAL_PWM_IMP_TIME_SETTING
#   define VSF_HAL_PWM_IMP_TIME_SETTING                 ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ INCLUDES ======================================*/

#define VSF_PWM_CFG_API_DECLARATION_PREFIX          vsf_hw_aic
#define VSF_PWM_CFG_INSTANCE_DECLARATION_PREFIX     VSF_HW_AIC
#include "hal/driver/common/pwm/pwm_template.h"

#endif /* VSF_HAL_USE_PWM */
#endif /* EOF */