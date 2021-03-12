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

#undef VSF_ESPRESSIF_DRIVER_HEADER

#if     defined(__ESP32S2__)
#   define  VSF_ESPRESSIF_DRIVER_HEADER         "./ESP32S2/driver.h"
#elif   defined(__ESP32__)
#   define  VSF_ESPRESSIF_DRIVER_HEADER         "./ESP32/driver.h"
#else
#   error No supported device found.
#endif

/* include specified device driver header file */
#include VSF_ESPRESSIF_DRIVER_HEADER



#ifndef __HAL_DRIVER_ESPRESSIF_H__
#define __HAL_DRIVER_ESPRESSIF_H__


/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */
