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

/* note: MACRO is UPPER-CASE, directory name is not case-sensitive.
    __SERIES_NAME_A__
        __DEVICE_NAME_A__
        __DEVICE_NAME_B__
    __SERIES_NAME_B__
        __DEVICE_NAME_C__
    __SERIES_NAME_C__
        __DEVICE_NAME_D__
*/

#if     defined(__DEVICE_NAME_A__)
#   define  VSF_HAL_DRIVER_HEADER               "./__SERIES_NAME_A__/__DEVICE_NAME_A__/driver.h"
#elif   defined(__DEVICE_NAME_B__)
#   define  VSF_HAL_DRIVER_HEADER               "./__SERIES_NAME_A__/__DEVICE_NAME_B__/driver.h"
#elif   defined(__DEVICE_NAME_C__)
#   define  VSF_HAL_DRIVER_HEADER               "./__SERIES_NAME_B__/__DEVICE_NAME_C__/driver.h"
#elif   defined(__DEVICE_NAME_D__)
#   define  VSF_HAL_DRIVER_HEADER               "./__SERIES_NAME_C__/__DEVICE_NAME_D__/driver.h"
#else
#   error No supported device found.
#endif

/* include specified driver header file */
#include VSF_HAL_DRIVER_HEADER

/* below is vendor specified driver part, put vendor-specified code(not vsf specified) here */

#ifndef __VSF_HAL_DRIVER_${VENDOR}_H__
#define __VSF_HAL_DRIVER_${VENDOR}_H__

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */
