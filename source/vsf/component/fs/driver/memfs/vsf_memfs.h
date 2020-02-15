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

declare_simple_class(vk_memfs_file_t)

def_simple_class(vk_memfs_file_t) {
    implement(vk_file_t)

    public_member(
        union {
            struct {
                uint8_t *buff;
            } f;
            struct {
                vk_memfs_file_t *child;
                uint16_t child_num;
                uint16_t child_size;
            } d;
        };
        struct {
            void (*read)(uintptr_t target, vsf_evt_t evt);
            void (*write)(uintptr_t target, vsf_evt_t evt);
        } callback;
    )
};

struct vk_memfs_info_t {
    vk_memfs_file_t root;
};
typedef struct vk_memfs_info_t vk_memfs_info_t;

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_fs_op_t vk_memfs_op;

/*============================ PROTOTYPES ====================================*/

extern void vk_memfs_init(vk_memfs_info_t *memfs);
extern vk_memfs_file_t * vk_memfs_open(vk_memfs_file_t *dir, const char *path);
extern int_fast32_t vk_memfs_read(vk_memfs_file_t *file, uint_fast64_t addr, uint_fast32_t size, uint8_t *buff);
extern int_fast32_t vk_memfs_write(vk_memfs_file_t *file, uint_fast64_t addr, uint_fast32_t size, uint8_t *buff);

#endif      // VSF_USE_FS && VSF_USE_FATFS
#endif      // __VSF_MEMFS_H__
