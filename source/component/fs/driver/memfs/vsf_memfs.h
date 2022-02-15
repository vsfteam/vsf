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

#ifndef __VSF_MEMFS_H__
#define __VSF_MEMFS_H__

/*============================ INCLUDES ======================================*/

#include "../../vsf_fs_cfg.h"

#if VSF_USE_FS == ENABLED && VSF_FS_USE_MEMFS == ENABLED

#if     defined(__VSF_MEMFS_CLASS_IMPLEMENT)
#   undef __VSF_MEMFS_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

__vsf_component_peda_ifs(vk_memfs_callback_read,
    uint64_t        offset;
    uint32_t        size;
    uint8_t         *buff;
    uint32_t        rsize;
)
__vsf_component_peda_ifs(vk_memfs_callback_write,
    uint64_t        offset;
    uint32_t        size;
    uint8_t         *buff;
    uint32_t        wsize;
)

vsf_class(vk_memfs_file_t) {
    public_member(
        implement(vk_file_t)

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
            void (*fn_read)(uintptr_t target, vsf_evt_t evt);
            void (*fn_write)(uintptr_t target, vsf_evt_t evt);
        } callback;
    )
};

typedef struct vk_memfs_info_t {
    vk_memfs_file_t root;
} vk_memfs_info_t;

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_fs_op_t vk_memfs_op;

/*============================ PROTOTYPES ====================================*/

extern void vk_memfs_init(vk_memfs_info_t *memfs);
extern vk_memfs_file_t * vk_memfs_open(vk_memfs_file_t *dir, const char *path);
extern uint_fast64_t vk_memfs_tell(vk_memfs_file_t *file);
extern uint_fast64_t vk_memfs_setpos(vk_memfs_file_t *file, uint_fast64_t offset);
extern int_fast32_t vk_memfs_read(vk_memfs_file_t *file, uint8_t *buff, uint_fast32_t size);
extern int_fast32_t vk_memfs_write(vk_memfs_file_t *file, uint8_t *buff, uint_fast32_t size);

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_FS && VSF_FS_USE_MEMFS
#endif      // __VSF_MEMFS_H__
