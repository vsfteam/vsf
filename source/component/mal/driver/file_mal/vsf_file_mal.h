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

#ifndef __VSF_FILE_MAL_H__
#define __VSF_FILE_MAL_H__

/*============================ INCLUDES ======================================*/

#include "../../vsf_mal_cfg.h"

#if VSF_USE_MAL == ENABLED && VSF_USE_FS == ENABLED && VSF_MAL_USE_FILE_MAL == ENABLED

#include "component/fs/vsf_fs.h"

#if     defined(__VSF_FILE_MAL_CLASS_IMPLEMENT)
#   undef __VSF_FILE_MAL_CLASS_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

dcl_simple_class(vk_file_mal_t)

def_simple_class(vk_file_mal_t) {
    public_member(
        implement(vk_mal_t)
        vk_file_t *file;
        uint32_t block_size;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_mal_drv_t vk_file_mal_drv;

/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_MAL && VSF_USE_FS
#endif      // __VSF_FILE_MAL_H__
