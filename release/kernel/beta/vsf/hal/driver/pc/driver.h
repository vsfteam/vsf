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

#ifndef __HAL_DRIVER_TEMPLATE_H__
#define __HAL_DRIVER_TEMPLATE_H__

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"

#undef VSF_DRIVER_HEADER

#if     defined(__DEVICE_NAME_A__)
#   define  VSF_DRIVER_HEADER       "./__series_name__/__device_name_1__/driver.h"
/* example
#elif   defined(__DEVICE_NAME_B__)  
#       define  VSF_DRIVER_HEADER   "./__series_name__/__device_name_2__/driver.h"
#elif   defined(__DEVICE_NAME_C__)  
#   define  VSF_DRIVER_HEADER       "./__series_name__/__device_name_3__/driver.h"
#elif   defined(__DEVICE_NAME_X__)
#   define  VSF_DRIVER_HEADER       "./__series_name__/__device_name_n__/driver.h"
*/
#elif   defined(__CPU_PC__)
#       define  VSF_DRIVER_HEADER   "./pc/driver.h"
#else
#   error No supported device found.
#endif

/* include specified device driver header file */
#include VSF_DRIVER_HEADER

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */
