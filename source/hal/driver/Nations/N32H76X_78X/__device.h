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

#undef VSF_HAL_DEVICE_HEADER

#if     defined(__N32H765IIB7EC__)
#   define  VSF_HAL_DEVICE_HEADER               "./N32H765IIB7EC/device.h"
#else
#   error No supported device found.
#endif

/* include specified device header file */
#include VSF_HAL_DEVICE_HEADER

#ifdef __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
// define arch information
//  ARM:        __CPU_ARM__
//  Riscv:      __CPU_RV__
//  ......
#   ifndef __CPU_ARM__
#       define __CPU_ARM__
#   endif
#else      // __VSF_HEADER_ONLY_SHOW_ARCH_INFO__

/* below is vendor specified device part, put vendor-specified code(not vsf specified) here */

#ifndef __VSF_HAL_DEVICE_NATIONS_N32H76X_N32H78X_H__
#define __VSF_HAL_DEVICE_NATIONS_N32H76X_N32H78X_H__

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif      // __VSF_HAL_DEVICE_NATIONS_N32H76X_N32H78X_H__
#endif      // __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
/* EOF */
