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
 *  See the License for the flashecific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

#ifndef __HAL_DRIVER_AIC8800_FLASH_H__
#define __HAL_DRIVER_AIC8800_FLASH_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"
#if VSF_HAL_USE_FLASH == ENABLED
#include "../__device.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ TYPES =========================================*/
/*============================ INCLUDES ======================================*/

#define VSF_FLASH_CFG_API_DECLARATION_PREFIX        vsf_hw
#define VSF_FLASH_CFG_INSTANCE_DECLARATION_PREFIX   VSF_HW
#include "hal/driver/common/flash/flash_template.h"

#endif /* VSF_HAL_USE_FLASH */
#endif /* EOF */