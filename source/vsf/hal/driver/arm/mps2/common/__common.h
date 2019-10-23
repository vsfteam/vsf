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

#ifndef __HAL_DRIVER_ARM_MPS2_COMMON_H__
#define __HAL_DRIVER_ARM_MPS2_COMMON_H__

/* \note __common.h should only be included by device.h */

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"
#include "hal/arch/vsf_arch.h"
#include "./V2M-MPS2_CMx_BSP/1.7.0/Boards/ARM/V2M-MPS2/Common/Device.h"
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

/* todo: should remove this but keep it for a while
extern vsf_err_t vsf_drv_swi_init(  uint_fast8_t idx, 
                                    vsf_arch_prio_t priority,
                                    vsf_swi_handler_t *handler, 
                                    void *pparam);
extern void vsf_drv_swi_trigger(uint_fast8_t idx);
*/
#endif
/* EOF */
