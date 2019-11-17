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

#ifndef __VSF_MEMFS_H__
#define __VSF_MEMFS_H__

/*============================ INCLUDES ======================================*/

#include "../../vsf_fs_cfg.h"

#if VSF_USE_FS == ENABLED && VSF_USE_MEMFS == ENABLED

#if     defined(VSF_MEMFS_IMPLEMENT)
#   undef VSF_MEMFS_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT
#elif   defined(VSF_MEMFS_INHERIT)
#   undef VSF_MEMFS_INHERIT
#   define __PLOOC_CLASS_INHERIT
#endif

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_simple_class(vsf_memfs_file_t)

def_simple_class(vsf_memfs_file_t) {
    implement(vsf_file_t)

    public_member(
        union {
            struct {
                uint8_t *buff;
            } f;
            struct {
                vsf_memfs_file_t *child;
                uint16_t child_num;
                uint16_t child_size;
            } d;
        };
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vsf_fs_op_t vsf_memfs_op;

/*============================ PROTOTYPES ====================================*/

#endif      // VSF_USE_FS && VSF_USE_FATFS
#endif      // __VSF_MEMFS_H__
