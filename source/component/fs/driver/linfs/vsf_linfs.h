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

#ifndef __VSF_LINFS_H__
#define __VSF_LINFS_H__

/*============================ INCLUDES ======================================*/

#include "../../vsf_fs_cfg.h"

#if VSF_USE_FS == ENABLED && VSF_FS_USE_LINFS == ENABLED

#if     defined(__VSF_LINFS_CLASS_IMPLEMENT)
#   undef __VSF_LINFS_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

vsf_class(vk_linfs_file_t) {
    public_member(
        implement(vk_file_t)
    )

    private_member(
        union {
            struct {
                int fd;
            } f;
            struct {
                void *dir;
            } d;
        };
        char *fullpath;
    )
};

typedef struct vk_linfs_info_t {
    vk_linfs_file_t root;
} vk_linfs_info_t;

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_fs_op_t vk_linfs_op;

/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_FS && VSF_FS_USE_LINFS
#endif      // __VSF_LINFS_H__
