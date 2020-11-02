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

#ifndef __VSF_FAKEFAT32_MAL_H__
#define __VSF_FAKEFAT32_MAL_H__

/*============================ INCLUDES ======================================*/

#include "../../vsf_mal_cfg.h"

#if VSF_USE_MAL == ENABLED && VSF_MAL_USE_FAKEFAT32_MAL == ENABLED

#include "component/fs/vsf_fs.h"
// DO NOT REMOVE vsf_memfs.h BELOW, unless you really know what you are doing.
// vsf_fs.h includes vsf_mal.h for malfs to use mal
// vsf_mal.h includes vsf_fs.h for fakefat32_mal to use memfs
#include "component/fs/driver/memfs/vsf_memfs.h"

#if     defined(__VSF_FAKEFAT32_MAL_CLASS_IMPLEMENT)
#   undef __VSF_FAKEFAT32_MAL_CLASS_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#if VSF_USE_FS != ENABLED || VSF_FS_USE_MEMFS != ENABLED
#   error FAKEFAT32 need memfs
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

dcl_simple_class(vk_fakefat32_mal_t)
dcl_simple_class(vk_fakefat32_file_t)

def_simple_class(vk_fakefat32_file_t) {
    public_member(
        implement(vk_memfs_file_t)
    )

    private_member(
        uint32_t first_cluster;
        vk_fakefat32_mal_t *mal;
        struct {
            // refer to :
            //  Microsoft Extensible Firmware Initiative FAT32 File System Specification
            uint8_t CrtTimeTenth;
            uint16_t CrtTime;
            uint16_t CrtDate;
            uint16_t LastAccDate;
            uint16_t FstClusHI;
            uint16_t WrtTime;
            uint16_t WrtDate;
            uint16_t FstClusLO;
        } PACKED record;
    )
};


def_simple_class(vk_fakefat32_mal_t) {
    public_member(
        implement(vk_mal_t)

        uint16_t sector_size;
        uint16_t sector_number;
        uint8_t sectors_per_cluster;

        uint32_t volume_id;
        uint32_t disk_id;

        vk_fakefat32_file_t root;
        vsf_err_t err;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_mal_drv_t vk_fakefat32_mal_drv;

/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_MAL && VSF_MAL_USE_FAKEFAT32_MAL
#endif      // __VSF_FAKEFAT32_MAL_H__
