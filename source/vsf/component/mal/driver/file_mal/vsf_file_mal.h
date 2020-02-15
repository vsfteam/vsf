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

#if VSF_USE_MAL == ENABLED && VSF_USE_FS == ENABLED && VSF_USE_FILE_MAL == ENABLED

#include "component/fs/vsf_fs.h"

#if     defined(VSF_FILE_MAL_IMPLEMENT)
#   undef VSF_FILE_MAL_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT
#elif   defined(VSF_FILE_MAL_INHERIT)
#   undef VSF_FILE_MAL_INHERIT
#   define __PLOOC_CLASS_INHERIT
#endif

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_simple_class(vk_file_mal_t)

def_simple_class(vk_file_mal_t) {
    public_member(
        implement(vk_mal_t)
        vk_file_t *file;
        uint32_t block_size;
    )
    private_member(
        int32_t rw_size;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const i_mal_drv_t VK_FILE_MAL_DRV;

/*============================ PROTOTYPES ====================================*/


#endif      // VSF_USE_MAL && VSF_USE_FS
#endif      // __VSF_FILE_MAL_H__
