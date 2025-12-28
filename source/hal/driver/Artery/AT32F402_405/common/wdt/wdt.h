/*****************************************************************************
 *   Copyright(C)2009-2024 by VSF Team                                       *
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
 ****************************************************************************/

#ifndef __HAL_DRIVER_ARTERY_AT32F402_405_WDT_H__
#define __HAL_DRIVER_ARTERY_AT32F402_405_WDT_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_WDT == ENABLED

// HW/IPCore
#include "hal/driver/common/template/vsf_template_hal_driver.h"
// HW/IPCore end

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

// HW
#define VSF_WDT_CFG_REIMPLEMENT_TYPE_MODE            ENABLED
#define VSF_WDT_CFG_REIMPLEMENT_TYPE_CFG             DISABLED
#define VSF_WDT_CFG_REIMPLEMENT_TYPE_CAPABILITY      DISABLED

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

// HW/IPCore, not for emulated drivers
#if VSF_WDT_CFG_REIMPLEMENT_TYPE_MODE == ENABLED
typedef enum vsf_wdt_mode_t {
    VSF_WDT_MODE_NO_EARLY_WAKEUP = (0 << 0),
    VSF_WDT_MODE_EARLY_WAKEUP    = (1 << 0),
    VSF_WDT_MODE_RESET_NONE      = (0 << 1),
    VSF_WDT_MODE_RESET_CPU       = (1 << 1),
    VSF_WDT_MODE_RESET_SOC       = (2 << 1),
} vsf_wdt_mode_t;
#endif


// HW/IPCore end

/*============================ INCLUDES ======================================*/

#ifdef __cplusplus
}
#endif

#endif      // VSF_HAL_USE_WDT
#endif      // __HAL_DRIVER_ARTERY_AT32F402_405_WDT_H__
/* EOF */
