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

#ifndef __VSF_MAL_SCSI_H__
#define __VSF_MAL_SCSI_H__

/*============================ INCLUDES ======================================*/

#include "../../vsf_scsi_cfg.h"

#if VSF_USE_SCSI == ENABLED && VSF_USE_MAL == ENABLED && VSF_SCSI_USE_MAL_SCSI == ENABLED

#include "component/mal/vsf_mal.h"

#if     defined(__VSF_MAL_SCSI_CLASS_IMPLEMENT)
#   undef __VSF_MAL_SCSI_CLASS_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

dcl_simple_class(vk_mal_scsi_t)

def_simple_class(vk_mal_scsi_t) {
    public_member(
        implement(vk_virtual_scsi_t)
        vk_mal_t *mal;
    )
#if VSF_USE_SIMPLE_STREAM == ENABLED
    private_member(
        vk_mal_stream_t mal_stream;
    )
#endif
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_virtual_scsi_drv_t vk_mal_virtual_scsi_drv;

/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_SCSI && VSF_SCSI_USE_MAL_SCSI
#endif      // __VSF_MAL_SCSI_H__
