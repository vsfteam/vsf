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

#undef VSF_ARM_MPS2_DEVICE_HEADER

#if   defined CMSDK_CM0
#   define VSF_ARM_MPS2_DEVICE_HEADER       "./CMSDK_CM0/device.h"                        
#elif defined CMSDK_CM0plus
#   define VSF_ARM_MPS2_DEVICE_HEADER       "./CMSDK_CM0plus/device.h"                  
#elif defined CMSDK_CM3
#   define VSF_ARM_MPS2_DEVICE_HEADER       "./CMSDK_CM3/device.h"                       
#elif defined CMSDK_CM4
#   define VSF_ARM_MPS2_DEVICE_HEADER       "./CMSDK_CM4/device.h"                       
#elif defined CMSDK_CM4_FP
#   define VSF_ARM_MPS2_DEVICE_HEADER       "./CMSDK_CM4_FP/device.h"                    
#elif defined CMSDK_CM7
#   define VSF_ARM_MPS2_DEVICE_HEADER       "./CMSDK_CM7/device.h"                       
#elif defined CMSDK_CM7_SP
#   define VSF_ARM_MPS2_DEVICE_HEADER       "./CMSDK_CM7/device.h"                    
#elif defined CMSDK_CM7_DP
#   define VSF_ARM_MPS2_DEVICE_HEADER       "./CMSDK_CM7/device.h"                   
#elif defined CMSDK_ARMv8MBL
#   define VSF_ARM_MPS2_DEVICE_HEADER       "./CMSDK_ARMv8MBL/device.h"                
#elif defined CMSDK_ARMv8MML
#   define VSF_ARM_MPS2_DEVICE_HEADER       "./CMSDK_ARMv8MML/device.h"                 
#elif defined CMSDK_ARMv8MML_SP
#   define VSF_ARM_MPS2_DEVICE_HEADER       "./CMSDK_ARMv8MML_SP/device.h"             
#elif defined CMSDK_ARMv8MML_DP
#   define VSF_ARM_MPS2_DEVICE_HEADER       "./CMSDK_ARMv8MML_DP/device.h"             
#else
#   warning "no appropriate header file found!"
#endif

/* include specified device driver header file */
#include VSF_ARM_MPS2_DEVICE_HEADER

#ifdef __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
#   ifndef __CPU_ARM__
#       define __CPU_ARM__
#   endif
#else

#ifndef __HAL_DEVICE_ARM_MPS2_H__
#define __HAL_DEVICE_ARM_MPS2_H__

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif      // __HAL_DRIVER_ARM_MPS2_DEVICE_H__
#endif      // __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
/* EOF */
