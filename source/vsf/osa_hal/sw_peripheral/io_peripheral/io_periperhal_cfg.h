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



/*============================ INCLUDES ======================================*/
#include "../sw_peripheral.h"

#ifndef __OSA_HAL_DRIVER_COMMON_IO_PERIPHERAL_CFG_H__
#define __OSA_HAL_DRIVER_COMMON_IO_PERIPHERAL_CFG_H__

#ifdef __cplusplus
extern "C" {
#endif
/*============================ MACROS ========================================*/

#ifndef VSF_USE_IO_I2C
#   define VSF_USE_IO_I2C                           DISABLED
#endif

#ifndef VSF_USE_IO_SPI
#   define VSF_USE_IO_SPI                           DISABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#ifndef VSF_IOP_ASSERT
#   define VSF_IOP_ASSERT(__CON)        ASSERT(__CON)
#endif
#else
#ifndef VSF_IOP_ASSERT
#   define VSF_IOP_ASSERT(...)          ASSERT(__VA_ARGS__)
#endif
#endif

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
