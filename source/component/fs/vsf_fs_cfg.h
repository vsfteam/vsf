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

//! \note do not move this pre-processor statement to other places
#include "component/vsf_component_cfg.h"

#ifndef __VSF_FS_CFG_H__
#define __VSF_FS_CFG_H__

/*============================ MACROS ========================================*/

#ifndef VSF_FS_ASSERT
#   define VSF_FS_ASSERT                VSF_ASSERT
#endif

#if VSF_FS_USE_FATFS == ENABLED
#   if VSF_FS_USE_MALFS != ENABLED
#       undef VSF_FS_USE_MALFS
#       define VSF_FS_USE_MALFS         ENABLED
#   endif
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

// for libc sorce codes which want types and consts below
typedef enum vk_file_attr_t {
    VSF_FILE_ATTR_READ          = 1 << 0,
    VSF_FILE_ATTR_WRITE         = 1 << 1,
    VSF_FILE_ATTR_EXECUTE       = 1 << 2,
    VSF_FILE_ATTR_HIDDEN        = 1 << 3,
    VSF_FILE_ATTR_DIRECTORY     = 1 << 4,
    VSF_FILE_ATTR_DYN           = 1 << 7,
    VSF_FILE_ATTR_EXT           = 1 << 8,
    VSF_FILE_ATTR_USER          = 1 << 9,
} vk_file_attr_t;

typedef enum vk_file_whence_t {
    VSF_FILE_SEEK_SET = 0,
    VSF_FILE_SEEK_CUR = 1,
    VSF_FILE_SEEK_END = 2,
} vk_file_whence_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/



#endif
/* EOF */