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

#ifndef __VSF_REMAPPED_MMC_H__
#define __VSF_REMAPPED_MMC_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_MMC == ENABLED

#if VSF_MMC_CFG_MULTI_CLASS == ENABLED

#if     defined(__VSF_REMAPPED_MMC_CLASS_IMPLEMENT)
#   undef __VSF_REMAPPED_MMC_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_REMAPPED_MMC_CLASS_INHERIT__)
#   undef __VSF_REMAPPED_MMC_CLASS_INHERIT__
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_REMAPPED_MMC_CFG_MULTI_CLASS
#   define VSF_REMAPPED_MMC_CFG_MULTI_CLASS     VSF_MMC_CFG_MULTI_CLASS
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

vsf_class(vsf_remapped_mmc_t) {
#if VSF_REMAPPED_MMC_CFG_MULTI_CLASS == ENABLED
    public_member(
        implement(vsf_mmc_t)
    )
#endif
    public_member(
        vsf_mmc_t                           *target;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

#if VSF_REMAPPED_MMC_CFG_MULTI_CLASS == ENABLED
extern const vsf_mmc_op_t vsf_remapped_mmc_op;
#endif

/*============================ PROTOTYPES ====================================*/
/*============================ INCLUDES ======================================*/


#define VSF_MMC_CFG_DEC_PREFIX              vsf_remapped
#define VSF_MMC_CFG_DEC_UPCASE_PREFIX       VSF_REMAPPED
#include "hal/driver/common/mmc/mmc_template.h"

#ifdef __cplusplus
}
#endif

#endif      // VSF_MMC_CFG_MULTI_CLASS
#endif      // VSF_HAL_USE_MMC
#endif
/* EOF */

