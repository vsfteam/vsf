/*****************************************************************************
 *   Copyright(C)2009-2020 by VSF Team                                       *
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

#ifndef __VSF_OSA_HAL_DEVICE_SPECIFIC_DRIVER_H__
#define __VSF_OSA_HAL_DEVICE_SPECIFIC_DRIVER_H__

/*============================ INCLUDES ======================================*/
#include "osa_hal/vsf_osa_hal_cfg.h"

/*! \note User can define VSF_OSA_DRIVER_HEADER to specify the targer device driver 
 *!       header file. If it is not specified, this driver abstraction header 
 *!       file will use predefined device-specific macros to decide which device
 *!       driver header file should be included.
 */
#ifndef VSF_OSA_DRIVER_HEADER
#   if      defined(__WCH__)
#       define VSF_OSA_DRIVER_HEADER    "./WCH/driver.h"
#   elif    defined(__Allwinner__)
#       define VSF_OSA_DRIVER_HEADER    "./Allwinner/driver.h"
#   endif
#endif

//! you don't have to implement a driver in your system.
#ifdef VSF_OSA_DRIVER_HEADER
/* include specified device driver header file */
#   include VSF_OSA_DRIVER_HEADER
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */
