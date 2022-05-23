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

#ifndef __HAL_DRIVER_AIC8800_LP_TIMER_H__
#define __HAL_DRIVER_AIC8800_LP_TIMER_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"
#if VSF_HAL_USE_TIMER == ENABLED
#include "../__device.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ INCLUDES ======================================*/

#define VSF_TIMER_CFG_API_DECLARATION_PREFIX        vsf_hw_lp
#define VSF_TIMER_CFG_INSTANCE_DECLARATION_PREFIX   VSF_HW_LP
#include "hal/driver/common/timer/timer_template.h"

#endif /* VSF_HAL_USE_TIMER */
#endif /* __HAL_DRIVER_AIC8800_LP_TIMER_H__ */