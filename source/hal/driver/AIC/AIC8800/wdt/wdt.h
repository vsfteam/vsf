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

#ifndef __HAL_DRIVER_AIC8800_WDT_H__
#define __HAL_DRIVER_AIC8800_WDT_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"
#if VSF_HAL_USE_WDT == ENABLED
#include "../__device.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum vsf_wdt_mode_t {
    VSF_WDT_MODE_NO_EARLY_WAKEUP = (0 << 0),
    VSF_WDT_MODE_EARLY_WAKEUP    = (1 << 0),
    __AIC8800_WDT_MODE_ALL_BITS  = VSF_WDT_MODE_EARLY_WAKEUP,

    VSF_WDT_MODE_RESET_SOC       = (0 << 16),
    VSF_WDT_MODE_RESET_CPU       = (1 << 16),
    VSF_WDT_MODE_RESET_NONE      = (2 << 16),
} vsf_wdt_mode_t;

/*============================ INCLUDES ======================================*/

#endif /* VSF_HAL_USE_WDT */
#endif /* EOF */