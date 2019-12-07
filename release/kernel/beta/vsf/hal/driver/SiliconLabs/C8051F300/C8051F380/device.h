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

#ifndef __HAL_DEVICE_SILICONLABS_C8051F380_H__
#define __HAL_DEVICE_SILICONLABS_C8051F380_H__

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"

/*============================ MACROS ========================================*/

/*\note first define basic info for arch. */
#if defined(__VSF_HEADER_ONLY_SHOW_ARCH_INFO__)
#   undef __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
#endif

// software interrupt provided by a dedicated device
#define VSF_DEV_SWI_NUM             0

/*============================ INCLUDES ======================================*/

/*\note this is should be the only place where __common.h is included.*/
//#include "../common/__common.h"

#if __IS_COMPILER_IAR__
#   include <ioC8051F380.h>
#elif __IS_COMPILER_51_KEIL__
#   include <c8051F360.h>
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */
