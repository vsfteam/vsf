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

#ifndef __VSF_KERNEL_COMMON_H__
#define __VSF_KERNEL_COMMON_H__

/*============================ INCLUDES ======================================*/
#include "./vsf_kernel_cfg.h"
#include "hal/vsf_hal.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define __VSF_SWI_PRIO_INIT(__index, __unused)                                  \
    vsf_priority_##__index,

/*============================ TYPES =========================================*/
enum vsf_priority_t {
    vsf_priority_inherit = -1,
    MREPEAT(VSF_OS_EVTQ_SWI_NUM, __VSF_SWI_PRIO_INIT, NULL)
    vsf_priority_highest = VSF_OS_EVTQ_SWI_NUM - 1,
};
typedef enum vsf_priority_t vsf_priority_t;


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */
