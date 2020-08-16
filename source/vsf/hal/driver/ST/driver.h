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

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"

#undef VSF_ST_DRIVER_HEADER

#if     defined(__STM32F730R8__)
#   define  VSF_ST_DRIVER_HEADER       "./STM32F730/STM32F730R8/driver.h"
#elif   defined(__STM32F103ZE__)
#   define  VSF_ST_DRIVER_HEADER       "./STM32F103/STM32F103ZE/driver.h"
#else
#   error No supported device found.
#endif

/* include specified device driver header file */
#include VSF_ST_DRIVER_HEADER



#ifndef __HAL_DRIVER_ST_H__
#define __HAL_DRIVER_ST_H__

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/



#endif
/* EOF */
