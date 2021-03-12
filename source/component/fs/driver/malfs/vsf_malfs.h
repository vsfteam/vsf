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

#ifndef __VSF_MALFS_H__
#define __VSF_MALFS_H__

/*============================ INCLUDES ======================================*/

#include "../../vsf_fs_cfg.h"

#if VSF_USE_FS == ENABLED && VSF_FS_USE_MALFS == ENABLED

#include "component/mal/vsf_mal.h"

#if     defined(__VSF_MALFS_CLASS_IMPLEMENT)
#   undef __VSF_MALFS_CLASS_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT__
#elif   defined(__VSF_MALFS_CLASS_INHERIT__)
#   undef __VSF_MALFS_CLASS_INHERIT__
#   define __PLOOC_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define __implement_malfs_cache(__size, __number)                               \
    __vk_malfs_cache_node_t __cache_nodes[__number];                            \
    uint8_t __buffer[__size * __number];

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

dcl_simple_class(__vk_malfs_file_t)
dcl_simple_class(__vk_malfs_info_t)
dcl_simple_class(__vk_malfs_cache_t)
dcl_simple_class(vk_malfs_mounter_t)

typedef struct __vk_malfs_cache_node_t {
    uint64_t block_addr;
    uint32_t access_time_sec    : 30;
    uint32_t is_dirty           : 1;
    uint32_t is_alloced         : 1;
} __vk_malfs_cache_node_t;

def_simple_class(__vk_malfs_cache_t) {
    public_member(
        uint16_t number;
        __vk_malfs_cache_node_t *nodes;
    )

    private_member(
        __vk_malfs_info_t *info;
    )
};

def_simple_class(__vk_malfs_file_t) {
    public_member(
        implement(vk_file_t)
        void *info;
    )
};

// memory layout:
//  |-------------------------------|
//  |       __vk_malfs_info_t       |
//  |-------------------------------|
//  |  __vk_malfs_cache_node_t[num] |
//  |-------------------------------|
//  |       cache_buffer[num]       |
//  |-------------------------------|
def_simple_class(__vk_malfs_info_t) {
    public_member(
        vk_mal_t *mal;
        uint32_t block_size;
        __vk_malfs_cache_t cache;
    )

    protected_member(
#if VSF_USE_HEAP == ENABLED
        // total control block is only used by vk_malfs_mount
        //  and will be freed by __vk_malfs_unmount
        void *total_cb;
#endif
        char *volume_name;
    )
};

#if VSF_USE_HEAP == ENABLED
typedef enum vk_malfs_mount_state_t {
    VSF_MOUNT_STATE_READ_MBR,
    VSF_MOUNT_STATE_CREATE_ROOT,
    VSF_MOUNT_STATE_OPEN_ROOT,
    VSF_MOUNT_STATE_MOUNT,
} vk_malfs_mount_state_t;

typedef struct vk_malfs_mount_partition_t {
    void *fsinfo;
    __vk_malfs_info_t *malfs_info;
    const vk_fs_op_t *fsop;
    vk_file_t *root;
} vk_malfs_mount_partition_t;

def_simple_class(vk_malfs_mounter_t) {
    public_member(
        vk_mal_t *mal;
        vk_file_t *dir;
        vsf_err_t err;
    )
    private_member(
        uint8_t *mbr;
        vk_malfs_mount_partition_t cur_partition;
        vk_malfs_mount_state_t mount_state;
        char *cur_root_name;
        uint8_t partition_idx;
    )
};
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern void __vk_malfs_init(__vk_malfs_info_t *info);
extern void __vk_malfs_cache_init(__vk_malfs_info_t *info, __vk_malfs_cache_t *cache);
// read/write/get_cache will lock/unlock automatically
extern vsf_err_t __vk_malfs_alloc_cache(__vk_malfs_info_t *info, __vk_malfs_cache_t *cache, uint_fast64_t block_addr);
extern vsf_err_t __vk_malfs_read(__vk_malfs_info_t *info, uint_fast64_t block_addr, uint_fast32_t block_num, uint8_t *buff);
extern vsf_err_t __vk_malfs_write(__vk_malfs_info_t *info, uint_fast64_t block_addr, uint_fast32_t block_num, uint8_t *buff);
extern void __vk_malfs_unmount(__vk_malfs_info_t *info);

#if VSF_USE_HEAP == ENABLED
// user should set the mal and root in mounter, then call vk_malfs_mount and wait VSF_EVT_RETURN
extern vsf_err_t vk_malfs_mount_mbr(vk_malfs_mounter_t *mounter);
#endif

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_FS && VSF_FS_USE_MALFS
#endif      // __VSF_MALFS_H__
