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

#undef VSF_ARM_MPS2_DRIVER_HEADER

#if   defined CMSDK_CM0
#   define VSF_ARM_MPS2_DRIVER_HEADER       "./CMSDK_CM0/driver.h"                        
#elif defined CMSDK_CM0plus
#   define VSF_ARM_MPS2_DRIVER_HEADER       "./CMSDK_CM0plus/driver.h"                  
#elif defined CMSDK_CM3
#   define VSF_ARM_MPS2_DRIVER_HEADER       "./CMSDK_CM3/driver.h"                       
#elif defined CMSDK_CM4
#   define VSF_ARM_MPS2_DRIVER_HEADER       "./CMSDK_CM4/driver.h"                       
#elif defined CMSDK_CM4_FP
#   define VSF_ARM_MPS2_DRIVER_HEADER       "./CMSDK_CM4_FP/driver.h"                    
#elif defined CMSDK_CM7
#   define VSF_ARM_MPS2_DRIVER_HEADER       "./CMSDK_CM7/driver.h"                       
#elif defined CMSDK_CM7_SP
#   define VSF_ARM_MPS2_DRIVER_HEADER       "./CMSDK_CM7/driver.h"                    
#elif defined CMSDK_CM7_DP
#   define VSF_ARM_MPS2_DRIVER_HEADER       "./CMSDK_CM7/driver.h"                   
#elif defined CMSDK_ARMv8MBL
#   define VSF_ARM_MPS2_DRIVER_HEADER       "./CMSDK_ARMv8MBL/driver.h"                
#elif defined CMSDK_ARMv8MML
#   define VSF_ARM_MPS2_DRIVER_HEADER       "./CMSDK_ARMv8MML/driver.h"                 
#elif defined CMSDK_ARMv8MML_SP
#   define VSF_ARM_MPS2_DRIVER_HEADER       "./CMSDK_ARMv8MML_SP/driver.h"             
#elif defined CMSDK_ARMv8MML_DP
#   define VSF_ARM_MPS2_DRIVER_HEADER       "./CMSDK_ARMv8MML_DP/driver.h"             
#else
#   warning "no appropriate header file found!"
#endif

/* include specified device driver header file */
#include VSF_ARM_MPS2_DRIVER_HEADER



#ifndef __HAL_DRIVER_ARM_MPS2_H__
#define __HAL_DRIVER_ARM_MPS2_H__


#if VSF_USE_SERVICE_VSFSTREAM == ENABLED || VSF_USE_SERVICE_STREAM == ENABLED
#include "service/vsf_service.h"
#endif



/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

#if     VSF_USE_SERVICE_VSFSTREAM == ENABLED
#define VSF_DEBUG_STREAM_TX     VSF_DEBUG_STREAM
#define VSF_DEBUG_STREAM_RX     VSF_DEBUG_STREAM

//extern vsf_stream_t  VSF_DEBUG_STREAM;
#elif   VSF_USE_SERVICE_STREAM == ENABLED
//extern const vsf_stream_tx_t VSF_DEBUG_STREAM_TX;
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */
