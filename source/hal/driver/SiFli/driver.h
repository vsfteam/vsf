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

#undef VSF_SIFLI_DRIVER_HEADER

#if     defined(__SF32LB520U36__) || defined(__SF32LB523UB6__) || defined(__SF32LB525UC6__) || defined(__SF32LB527UD6__)
#   define  VSF_SIFLI_DRIVER_HEADER             "./SF32LB52X/common/driver.h"
#elif   defined(__SF32LB551U4O5__) || defined(__SF32LB555V4O6__) || defined(__SF32LB555V436__) || defined(__SF32LB557VD3A6__)
#   define  VSF_SIFLI_DRIVER_HEADER             "./SF32LB55X/common/driver.h"
#elif   defined(__SF32LB567VND36__) || defined(__SF32LB566VCB36__) || defined(__SF32LB56WUND26__)\
    ||  defined(__SF32LB563UCN26__) || defined(__SF32LB561UBN26__) || defined(__SF32LB560UNN26__)\
#   define  VSF_SIFLI_DRIVER_HEADER             "./SF32LB56X/common/driver.h"
#elif   defined(__SF32LB583VCC36__) || defined(__SF32LB585V5E56__) || defined(__SF32LB587VEE56__)
#   define  VSF_SIFLI_DRIVER_HEADER             "./SF32LB58X/common/driver.h"
#else
#   error No supported device found.
#endif

/* include specified device driver header file */
#include VSF_SIFLI_DRIVER_HEADER



#ifndef __HAL_DRIVER_SIFLI_H__
#define __HAL_DRIVER_SIFLI_H__


/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */
