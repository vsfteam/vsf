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

#if VSF_USE_FS == ENABLED

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

#if VSF_FS_CFG_CACHE == ENABLED
#   warning cache is not supported now, disable VSF_FS_CFG_CACHE
#   undef VSF_FS_CFG_CACHE
#   define VSF_FS_CFG_CACHE             DISABLED
#endif

#ifndef VSF_FS_CFG_LOCK
#   define VSF_FS_CFG_LOCK              ENABLED
#endif

#ifndef VSF_FS_CFG_TIME
#   define VSF_FS_CFG_TIME              ENABLED
#endif

#ifndef VSF_FS_CFG_VFS_FILE_HAS_OP
#   define VSF_FS_CFG_VFS_FILE_HAS_OP   ENABLED
#endif

// VSF_FS_CFG_FILE_POOL_FILE_SIZE is file byte size in file pool
// VSF_FS_CFG_FILE_POOL_SIZE is file number in file pool
#if     (defined(VSF_FS_CFG_FILE_POOL_SIZE) && !defined(VSF_FS_CFG_FILE_POOL_FILE_SIZE))\
    ||  (!defined(VSF_FS_CFG_FILE_POOL_SIZE) && defined(VSF_FS_CFG_FILE_POOL_FILE_SIZE))
#   warning both VSF_FS_CFG_FILE_POOL_SIZE and VSF_FS_CFG_FILE_POOL_FILE_SIZE MUST\
            be defined to enable file pool.
#endif
#if defined(VSF_FS_CFG_FILE_POOL_SIZE) && defined(VSF_FS_CFG_FILE_POOL_FILE_SIZE)
#   define VSF_FS_CFG_FILE_POOL         ENABLED
#endif
#if VSF_FS_CFG_FILE_POOL == ENABLED && VSF_USE_POOL != ENABLED
#   error please enable VSF_USE_POOL to use file_pool
#endif

#if VSF_FS_CFG_FILE_POOL != ENABLED
// file pool not enabled, check VSF_FS_CFG_MALLOC and  VSF_FS_CFG_FREE
#if defined(VSF_FS_CFG_MALLOC) && !defined(VSF_FS_CFG_FREE)
#   error VSF_FS_CFG_FREE must be defined
#endif
#if !defined(VSF_FS_CFG_MALLOC) && defined(VSF_FS_CFG_FREE)
#   error VSF_FS_CFG_MALLOC must be defined
#endif

#undef __VSF_FS_WARN_HEAP
#ifndef VSF_FS_CFG_MALLOC
#   if VSF_USE_HEAP != ENABLED
#       define VSF_FS_CFG_MALLOC(...)   ((vk_file_t *)NULL)
#       define VSF_FS_CFG_FREE(...)
#       define __VSF_FS_WARN_HEAP
#   else
#       define VSF_FS_CFG_MALLOC        vsf_heap_malloc
#       define VSF_FS_CFG_FREE          vsf_heap_free
#   endif
#endif
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

// for libc source codes which want types and consts below
typedef enum vk_file_attr_t {
    VSF_FILE_ATTR_READ          = 1UL << 0,
    VSF_FILE_ATTR_WRITE         = 1UL << 1,
    VSF_FILE_ATTR_EXECUTE       = 1UL << 2,
    VSF_FILE_ATTR_HIDDEN        = 1UL << 3,
    VSF_FILE_ATTR_DIRECTORY     = 1UL << 4,
    VSF_FILE_ATTR_LNK           = 1UL << 5,
    // __VSF_FILE_ATTR_DYN is for internal usage only, to indicated resources are dynamically allocated
    __VSF_FILE_ATTR_DYN         = 1UL << 7,
    VSF_FILE_ATTR_EXT           = 1UL << 8,
    VSF_FILE_ATTR_USER          = 1UL << 9,
    // make vk_file_attr_t 32bit
    VSF_FILE_ATTR_MAX           = 1UL << 31,
} vk_file_attr_t;

typedef enum vk_file_whence_t {
    VSF_FILE_SEEK_SET = 0,
    VSF_FILE_SEEK_CUR = 1,
    VSF_FILE_SEEK_END = 2,
} vk_file_whence_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif      // VSF_USE_FS
#endif      // __VSF_FS_CFG_H__
/* EOF */