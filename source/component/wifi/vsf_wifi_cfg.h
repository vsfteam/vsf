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

/*============================ INCLUDES ======================================*/
//! \note do not move this pre-processor statement to other places
#include "component/vsf_component_cfg.h"

#ifndef __VSF_WIFI_CFG_H__
#define __VSF_WIFI_CFG_H__

/*============================ MACROS ========================================*/

#ifndef VSF_USE_WIFI
#   define VSF_USE_WIFI                 DISABLED
#endif

#ifndef VSF_WIFI_ASSERT
#   define VSF_WIFI_ASSERT              VSF_ASSERT
#endif

/*
 * Chip driver enable switches.
 *
 * Each chip family has its own ENABLE switch so applications can choose
 * which chip drivers to compile in.  The switches are bus-agnostic; the
 * same chip driver object can be used by USB / SDIO / SPI shims.
 *
 * A bus shim (e.g. vsf_usbh_wifi) MUST require at least one chip driver
 * to be enabled when the bus class itself is enabled.
 */
#ifndef VSF_WIFI_USE_RT28XX
#   define VSF_WIFI_USE_RT28XX          ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */
