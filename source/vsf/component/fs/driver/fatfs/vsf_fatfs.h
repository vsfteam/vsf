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

#ifndef __VSF_FATFS_H__
#define __VSF_FATFS_H__

/*============================ INCLUDES ======================================*/

#include "../../vsf_fs_cfg.h"

#if VSF_USE_FS == ENABLED && VSF_USE_FATFS == ENABLED

#if     defined(VSF_FATFS_IMPLEMENT)
#   undef VSF_FATFS_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT
#elif   defined(VSF_FATFS_INHERIT)
#   undef VSF_FATFS_INHERIT
#   define __PLOOC_CLASS_INHERIT
#endif

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_simple_class(vk_fatfs_file_t)

enum vk_fat32_file_attr_t {
    VSF_FAT32_FILE_ATTR_VOLUMID = VSF_FILE_ATTR_EXT,
    VSF_FAT32_FILE_ATTR_SYSTEM  = VSF_FILE_ATTR_EXT << 1,
    VSF_FAT32_FILE_ATTR_ARCHIVE = VSF_FILE_ATTR_EXT << 2,
};
typedef enum vk_fat32_file_attr_t vk_fat32_file_attr_t;

struct vk_fatfs_dentry_parser_t {
    uint8_t *entry;
    uint16_t entry_num;
    uint8_t lfn;
    char *filename;
};
typedef struct vk_fatfs_dentry_parser_t vk_fatfs_dentry_parser_t;

def_simple_class(vk_fatfs_file_t) {
    implement(vk_file_t)
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_fs_op_t vk_fatfs_op;

/*============================ PROTOTYPES ====================================*/

extern bool vk_fatfs_is_lfn(char *name);
extern bool vk_fatfs_parse_dentry_fat(vk_fatfs_dentry_parser_t *parser);

#endif      // VSF_USE_FS && VSF_USE_FATFS
#endif      // __VSF_FATFS_H__
