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

#ifndef __VSF_ROMFS_H__
#define __VSF_ROMFS_H__

/*============================ INCLUDES ======================================*/

#include "../../vsf_fs_cfg.h"

#if VSF_USE_FS == ENABLED && VSF_FS_USE_ROMFS == ENABLED

#if     defined(__VSF_ROMFS_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_ROMFS_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_ROMFS_CFG_DIRECT_ACCESS
#   define VSF_ROMFS_CFG_DIRECT_ACCESS                  ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct vk_romfs_header_t {
    uint32_t nextfh;
    uint32_t spec;
    uint32_t size;
    uint32_t checksum;
    uint8_t name[16];
} VSF_CAL_PACKED;
typedef struct vk_romfs_header_t vk_romfs_header_t;

vsf_class(vk_romfs_file_t) {
    public_member(
        implement(vk_file_t)
    )
    private_member(
#if VSF_ROMFS_CFG_DIRECT_ACCESS == ENABLED
        vk_romfs_header_t *image;
        vk_romfs_header_t *header;
#endif
        uint8_t level;
    )
};

vsf_class(vk_romfs_info_t) {
    vk_romfs_file_t root;

    vk_romfs_header_t *image;
    // image_size/alignment is necessary only if is_chained is true
    uint32_t image_size;
    uint32_t alignment;
    // used to chian multiple romfs
    bool is_chained;
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_fs_op_t vk_romfs_op;

/*============================ PROTOTYPES ====================================*/

#if defined(__VSF_ROMFS_CLASS_INHERIT__) || defined(__VSF_ROMFS_CLASS_IMPLEMENT)
extern bool vsf_romfs_is_image_valid(vk_romfs_header_t *image);
extern vk_romfs_header_t * vsf_romfs_chain_get_next(vk_romfs_info_t *info, vk_romfs_header_t *image, bool force);
#endif

#ifdef __cplusplus
}
#endif

#undef __VSF_ROMFS_CLASS_IMPLEMENT
#undef __VSF_ROMFS_CLASS_INHERIT__

#endif      // VSF_USE_FS && VSF_FS_USE_ROMFS
#endif      // __VSF_ROMFS_H__
