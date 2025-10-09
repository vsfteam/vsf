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

#include "hal/vsf_hal_cfg.h"

#undef VSF_HAL_DRIVER_HEADER

#if     defined(__EMULATION_DISTBUS__)
#   define  VSF_HAL_DRIVER_HEADER               "./distbus/driver.h"
#else
#   error No supported device found.
#endif

/* include specified driver header file */
#include VSF_HAL_DRIVER_HEADER

/* below is vendor specified driver part, put vendor-specified code(not vsf specified) here */

#ifndef __VSF_HAL_DRIVER_EMULATION_H__
#define __VSF_HAL_DRIVER_EMULATION_H__

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */
