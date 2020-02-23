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

#if VSF_USE_KERNEL == ENABLED
#include "hal/vsf_hal.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define __VSF_SWI_PRIO_INIT(__index, __unused)                                  \
    vsf_prio_##__index,

/*============================ TYPES =========================================*/

#define MFUNC_IN_U8_DEC_VALUE                    VSF_OS_CFG_PRIORITY_NUM
#   include "utilities/preprocessor/mf_u8_dec2str.h"
enum vsf_prio_t {
    vsf_prio_inherit                    = -1,
    REPEAT_MACRO(MFUNC_OUT_DEC_STR, __VSF_SWI_PRIO_INIT, NULL)
    vsf_prio_highest                    = MFUNC_OUT_DEC_STR - 1,
};
typedef enum vsf_prio_t vsf_prio_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif
#endif
/* EOF */
