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

/*============================ INCLUDES ======================================*/

#include "../../vsf_fs_cfg.h"

#if VSF_USE_FS == ENABLED && VSF_USE_MALFS == ENABLED

#define VSF_FS_INHERIT
#define VSF_MALFS_IMPLEMENT
#define VSF_MAL_INHERIT

// TODO: use dedicated include
#include "vsf.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum vk_malfs_partition_type_t {
    VSF_MBR_PARTITION_TYPE_NONE             = 0x00,
    VSF_MBR_PARTITION_TYPE_FAT12_CHS        = 0x01,
    VSF_MBR_PARTITION_TYPE_FAT16_16_32_CHS  = 0x04,
    VSF_MBR_PARTITION_TYPE_MS_EXT           = 0x05,
    VSF_MBR_PARTITION_TYPE_FAT16_32_CHS     = 0x06,
    VSF_MBR_PARTITION_TYPE_NTFS             = 0x07,
    VSF_MBR_PARTITION_TYPE_FAT32_CHS        = 0x0B,
    VSF_MBR_PARTITION_TYPE_FAT32_LBA        = 0x0C,
    VSF_MBR_PARTITION_TYPE_FAT16_32_2G_LBA  = 0x0E,
    VSF_MBR_PARTITION_TYPE_MS_EXT_LBA       = 0x0F,
};
typedef enum vk_malfs_partition_type_t vk_malfs_partition_type_t;

struct vk_malfs_dpt_t {
    uint8_t boot_signature;
    uint8_t start_head_number;
    uint8_t start_sector_number;
    uint8_t start_cylinder_number;
    uint8_t paritition_type;
    uint8_t end_head_number;
    uint8_t end_sector_number;
    uint8_t end_cylinder_number;
    uint32_t sectors_preceding;
    uint32_t sectors_in_partition;
} PACKED;
typedef struct vk_malfs_dpt_t vk_malfs_dpt_t;

struct vk_malfs_mbr_t {
    uint8_t boot_record[440];
    uint8_t disk_signature[4];
    uint16_t reserved;
    vk_malfs_dpt_t dpt[4];
    uint16_t magic;
} PACKED;
typedef struct vk_malfs_mbr_t vk_malfs_mbr_t;

/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

void __vk_malfs_init(__vk_malfs_info_t *info)
{
#if VSF_FS_CFG_SUPPORT_MULTI_THREAD == ENABLED
    vsf_eda_crit_init(&info->crit);
#endif
    __vk_malfs_cache_init(info, &info->cache);
}

static uint8_t * __vk_malfs_get_cache_buff(__vk_malfs_cache_t *cache, __vk_malfs_cache_node_t *node)
{
    int idx = node - cache->nodes;
    uint8_t *buff = (uint8_t *)&cache->nodes[cache->number];
    return &buff[idx * cache->info->block_size];
}

void __vk_malfs_cache_init(__vk_malfs_info_t *info, __vk_malfs_cache_t *cache)
{
    cache->info = info;
#if VSF_FS_CFG_SUPPORT_MULTI_THREAD == ENABLED
    vsf_eda_crit_init(&cache->crit);
#endif
}

static void __vk_malfs_alloc_cache_imp(uintptr_t target, vsf_evt_t evt)
{
    __vk_malfs_cache_t *cache = (__vk_malfs_cache_t *)target;
    __vk_malfs_info_t *info = cache->info;
    __vk_malfs_cache_node_t *nodes = cache->nodes;

    switch (evt) {
    case VSF_EVT_INIT: {
            __vk_malfs_cache_node_t *least_node = nodes, *unalloced_node = NULL;
            for (uint_fast16_t i = 0; i < cache->number; i++) {
                if (nodes[i].is_alloced) {
                    if (nodes[i].block_addr == cache->ctx.block_addr) {
                        cache->ctx.result = &nodes[i];
                        vsf_eda_return();
                        return;
                    }
                    if (nodes[i].access_time_sec < least_node->access_time_sec) {
                        least_node = &nodes[i];
                    }
                } else if (NULL == unalloced_node) {
                    unalloced_node = &nodes[i];
                }
            }
            if (unalloced_node != NULL) {
                unalloced_node->is_alloced = true;
                cache->ctx.result = unalloced_node;
                vsf_eda_return();
                return;
            }

            cache->ctx.result = least_node;
            // missed, get least cache
            // TODO: add bit to indicate the node is not currently used
            if (!least_node->is_dirty) {
                cache->ctx.result = least_node;
                vsf_eda_return();
                return;
            }

            least_node->is_dirty = false;
            __vk_malfs_write(info, least_node->block_addr, 1, __vk_malfs_get_cache_buff(&info->cache, least_node));
            break;
        }
        break;
    case VSF_EVT_RETURN:
        vsf_eda_return();
        break;
    }
}

vsf_err_t __vk_malfs_alloc_cache(__vk_malfs_info_t *info, __vk_malfs_cache_t *cache, uint_fast64_t block_addr)
{
    // TODO: add lock
    cache->ctx.block_addr = block_addr;
    cache->ctx.result = NULL;
    return __vsf_call_eda((uintptr_t)__vk_malfs_alloc_cache_imp, (uintptr_t)cache);
}

static void __vk_malfs_read_imp(uintptr_t target, vsf_evt_t evt)
{
    __vk_malfs_info_t *info = (__vk_malfs_info_t *)target;
    enum {
        STATE_GET_CACHE,
        STATE_COMMIT_READ,
        STATE_FINISH_READ,
    };

    switch (evt) {
    case VSF_EVT_INIT:
        if (NULL == info->ctx_io.buff) {
            VSF_FS_ASSERT(1 == info->ctx_io.block_num);
            vsf_eda_frame_user_value_set(STATE_GET_CACHE);
            __vk_malfs_alloc_cache(info, &info->cache, info->ctx_io.block_addr);
            return;
        }
        vsf_eda_frame_user_value_set(STATE_COMMIT_READ);
    case VSF_EVT_RETURN: {
            uint8_t state;
            vsf_eda_frame_user_value_get(&state);
            switch (state) {
            case STATE_GET_CACHE:
                VSF_FS_ASSERT(info->cache.ctx.result != NULL);
                info->ctx_io.buff = __vk_malfs_get_cache_buff(&info->cache, info->cache.ctx.result);
                // fall through
            case STATE_COMMIT_READ:
                vsf_eda_frame_user_value_set(STATE_FINISH_READ);
                vk_mal_read(info->mal, info->block_size * info->ctx_io.block_addr,
                            info->block_size * info->ctx_io.block_num, info->ctx_io.buff);
                break;
            case STATE_FINISH_READ:
                info->ctx_io.err = info->mal->result.errcode;
                vsf_eda_return();
                break;
            }
        }
        break;
    }
}

vsf_err_t __vk_malfs_read(__vk_malfs_info_t *info, uint_fast64_t block_addr, uint_fast32_t block_num, uint8_t *buff)
{
    // TODO: add lock and unlock
    info->ctx_io.block_addr = block_addr;
    info->ctx_io.block_num = block_num;
    info->ctx_io.buff = buff;
    return __vsf_call_eda((uintptr_t)__vk_malfs_read_imp, (uintptr_t)info);
}

vsf_err_t __vk_malfs_write(__vk_malfs_info_t *info, uint_fast64_t block_addr, uint_fast32_t block_num, uint8_t *buff)
{
    // TODO: add lock and unlock
//    info->ctx_io.block_addr = block_addr;
//    info->ctx_io.block_num = block_num;
//    info->ctx_io.buff = buff;
    return vk_mal_write(info->mal, info->block_size * block_addr, info->block_size * block_num, buff);
}

#if VSF_USE_HEAP == ENABLED
void __vk_malfs_unmount(__vk_malfs_info_t *info)
{
    if (info->total_cb != NULL) {
        vsf_heap_free(info->total_cb);
    }
}

static void __vk_malfs_mount_mbr_imp(uintptr_t target, vsf_evt_t evt)
{
    vk_malfs_mounter_t *mounter = (vk_malfs_mounter_t *)target;
    vk_malfs_mbr_t *mbr = (vk_malfs_mbr_t *)mounter->mbr;
    vk_malfs_dpt_t *dpt;

    vk_malfs_mount_partition_t *partition = &mounter->cur_partition;
    vk_mal_t *mal = mounter->mal;

    switch (evt) {
    case VSF_EVT_INIT:
        mounter->mbr = vsf_heap_malloc(512);
        if (NULL == mounter->mbr) {
        return_not_enough_resources:
            mounter->err = VSF_ERR_NOT_ENOUGH_RESOURCES;
        do_return:
            if (mounter->mbr != NULL) {
                vsf_heap_free(mounter->mbr);
            }
            vsf_eda_return();
            break;
        }

        mounter->mount_state = VSF_MOUNT_STATE_READ_MBR;
        if (VSF_ERR_NONE != vk_mal_read(mal, 0, 512, mounter->mbr)) {
        return_failed:
            mounter->err = VSF_ERR_FAIL;
            goto do_return;
        }
        break;
    case VSF_EVT_RETURN:
        switch (mounter->mount_state) {
        case VSF_MOUNT_STATE_READ_MBR:
            if (    (VSF_ERR_NONE != mal->result.errcode)
                ||  (0xAA55 != le16_to_cpu(mbr->magic))) {
                goto return_failed;
            }
            mounter->partition_idx = -1;
        next_partition:
            if (++mounter->partition_idx >= dimof(mbr->dpt)) {
                mounter->err = VSF_ERR_NONE;
                goto do_return;
            }
            dpt = &mbr->dpt[mounter->partition_idx];
            switch (dpt->paritition_type) {
            default:
                // not supported
            case VSF_MBR_PARTITION_TYPE_NONE:
                goto next_partition;
#if VSF_USE_FATFS == ENABLED
            case VSF_MBR_PARTITION_TYPE_FAT12_CHS:
            case VSF_MBR_PARTITION_TYPE_FAT16_16_32_CHS:
            case VSF_MBR_PARTITION_TYPE_FAT16_32_CHS:
            case VSF_MBR_PARTITION_TYPE_FAT32_CHS:
            case VSF_MBR_PARTITION_TYPE_FAT32_LBA:
            case VSF_MBR_PARTITION_TYPE_FAT16_32_2G_LBA: {
                    struct vk_malfs_fat_t {
                        vk_mim_mal_t fat_mal;
                        char root_name[6];
                        implement_fatfs_info(512, 1);
                    };
                    typedef struct vk_malfs_fat_t vk_malfs_fat_t;

                    vk_malfs_fat_t *malfs_fat = (vk_malfs_fat_t *)vsf_heap_malloc(sizeof(vk_malfs_fat_t));
                    malfs_fat->total_cb = malfs_fat;
                    partition->fsinfo = &malfs_fat->use_as____vk_fatfs_info_t;
                    partition->malfs_info = &malfs_fat->use_as____vk_malfs_info_t;
                    partition->fsop = &vk_fatfs_op;
                    if (NULL == malfs_fat) {
                        goto return_not_enough_resources;
                    }

                    malfs_fat->fat_mal.host_mal = mal;
                    malfs_fat->fat_mal.drv = &VK_MIM_MAL_DRV;
                    malfs_fat->fat_mal.offset = le32_to_cpu(dpt->sectors_preceding) * 512;
                    malfs_fat->fat_mal.size = le32_to_cpu(dpt->sectors_in_partition) * 512;
                    malfs_fat->mal = &malfs_fat->fat_mal.use_as__vk_mal_t;
                    init_fatfs_info_ex(malfs_fat, 512, 1, malfs_fat);

                    mounter->mount_state = VSF_MOUNT_STATE_CREATE_ROOT;
                    strcpy(malfs_fat->root_name, "root");
                    malfs_fat->root_name[4] = '0' + mounter->partition_idx;
                    malfs_fat->root_name[5] = '\0';
                    mounter->cur_root_name = malfs_fat->root_name;
                    if (VSF_ERR_NONE != vk_file_create(mounter->dir, mounter->cur_root_name, VSF_FILE_ATTR_DIRECTORY, 0)) {
                        goto return_mount_failed;
                    }
                }
                break;
#endif
            }
            break;
        case VSF_MOUNT_STATE_CREATE_ROOT:
            if (VSF_ERR_NONE != mounter->dir->ctx.err) {
                goto return_mount_failed;
            }
            mounter->mount_state = VSF_MOUNT_STATE_OPEN_ROOT;
            if (VSF_ERR_NONE != vk_file_open(mounter->dir, mounter->cur_root_name, 0, &partition->root)) {
                goto return_mount_failed;
            }
            break;
        case VSF_MOUNT_STATE_OPEN_ROOT:
            if (VSF_ERR_NONE != mounter->dir->ctx.err) {
                goto return_mount_failed;
            }
            mounter->mount_state = VSF_MOUNT_STATE_MOUNT;
            if (VSF_ERR_NONE != vk_fs_mount(partition->root, partition->fsop, partition->fsinfo)) {
                goto return_mount_failed;
            }
            break;
        case VSF_MOUNT_STATE_MOUNT:
            if (VSF_ERR_NONE != partition->root->ctx.err) {
            return_mount_failed:
                if (partition->malfs_info->total_cb != NULL) {
                    vsf_heap_free(partition->malfs_info->total_cb);
                }
                goto return_failed;
            }
            if (partition->malfs_info->volume_name != NULL) {
                partition->root->name = partition->malfs_info->volume_name;
            } else {
                // TODO: rename root
            }
            goto next_partition;
        }
        break;
    }
}

vsf_err_t vk_malfs_mount_mbr(vk_malfs_mounter_t *mounter)
{
    VSF_FS_ASSERT((mounter != NULL) && (mounter->mal != NULL) && (mounter->dir != NULL));
    return __vsf_call_eda((uintptr_t)__vk_malfs_mount_mbr_imp, (uintptr_t)mounter);
}
#else
void __vk_malfs_unmount(__vk_malfs_info_t *info)
{
}
#endif

#endif
