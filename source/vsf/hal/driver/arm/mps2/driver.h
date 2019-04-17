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

#ifndef __HAL_DRIVER_ARM_MPS2_H__
#define __HAL_DRIVER_ARM_MPS2_H__

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"
#include "Device.h"

#if 0
#if __IS_COMPILER_ARM_COMPILER_5__
    #if defined __TARGET_FPU_VFP
        #include <ARMCM4/Include/ARMCM4_FP.h>
    #else
        #include <ARMCM3/Include/ARMCM3.h>
    #endif

#elif __IS_COMPILER_ARM_COMPILER_6__
    #if defined __ARM_PCS_VFP
        #include <ARMCM4/Include/ARMCM4_FP.h>
    #else
        #include <ARMCM3/Include/ARMCM3.h>
    #endif

#elif __IS_COMPILER_GCC__
    #if defined (__VFP_FP__) && !defined(__SOFTFP__)
        #include <ARMCM4/Include/ARMCM4_FP.h>
    #else
        #include <ARMCM3/Include/ARMCM3.h>
    #endif

#elif __IS_COMPILER_IAR__
    #if defined __ARMVFP__
        #include <ARMCM4/Include/ARMCM4_FP.h>
    #else
        #include <ARMCM3/Include/ARMCM3.h>
    #endif

#elif defined ( __TI_ARM__ )
    #if defined __TI_VFP_SUPPORT__
        #include <ARMCM4/Include/ARMCM4_FP.h>
    #else
        #include <ARMCM3/Include/ARMCM3.h>
    #endif

#elif defined ( __TASKING__ )
    #if defined __FPU_VFP__
        #include <ARMCM4/Include/ARMCM4_FP.h>
    #else
        #include <ARMCM3/Include/ARMCM3.h>
    #endif

#elif defined ( __CSMC__ )
    #if ( __CSMC__ & 0x400U)
        #include <ARMCM4/Include/ARMCM4_FP.h>
    #else
        #include <ARMCM3/Include/ARMCM3.h>
    #endif

#endif
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */
