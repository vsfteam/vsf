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

#include "../../vsf_fs_cfg.h"

#if VSF_USE_FS == ENABLED && VSF_FS_USE_MALFS == ENABLED

#define __VSF_FS_CLASS_INHERIT__
#define __VSF_MAL_CLASS_INHERIT__
#define __VSF_MALFS_CLASS_IMPLEMENT
#include "../../vsf_fs.h"

/*============================ MACROS ========================================*/

#if VSF_KERNEL_CFG_SUPPORT_SYNC != ENABLED
#   warning Since VSF_KERNEL_CFG_SUPPORT_SYNC is not enabled, partitions will be\
            non-reentrant.
#endif

#ifdef __VSF64__
#   if defined(VSF_OS_CFG_EDA_FRAME_POOL_EXTRA_SIZE) && VSF_OS_CFG_EDA_FRAME_POOL_EXTRA_SIZE < 128
#       error VSF_OS_CFG_EDA_FRAME_POOL_EXTRA_SIZE MUST be >= 128
#   endif
#else
#   if defined(VSF_OS_CFG_EDA_FRAME_POOL_EXTRA_SIZE) && VSF_OS_CFG_EDA_FRAME_POOL_EXTRA_SIZE < 64
#       error VSF_OS_CFG_EDA_FRAME_POOL_EXTRA_SIZE MUST be >= 64
#   endif
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum vk_malfs_mount_state_t {
    VSF_MOUNT_STATE_READ_MBR,
    VSF_MOUNT_STATE_READ_GPT_HEADER,
    VSF_MOUNT_STATE_READ_GPT_PARTITION_ENTRY,
    VSF_MOUNT_STATE_PROBE_FS,
    VSF_MOUNT_STATE_PARSE_GPT_PARTITION_ENTRY,
    VSF_MOUNT_STATE_CREATE_ROOT,
    VSF_MOUNT_STATE_OPEN_ROOT,
    VSF_MOUNT_STATE_MOUNT,
    VSF_MOUNT_STATE_RENAME_ROOT,
    VSF_MOUNT_STATE_UNLINK_ROOT,
} vk_malfs_mount_state_t;

typedef enum vk_malfs_partition_type_t {
    VSF_MBR_PARTITION_TYPE_NONE             = 0x00,
    VSF_MBR_PARTITION_TYPE_FAT12_CHS        = 0x01,
    VSF_MBR_PARTITION_TYPE_FAT16_16_32_CHS  = 0x04,
    VSF_MBR_PARTITION_TYPE_MS_EXT           = 0x05,
    VSF_MBR_PARTITION_TYPE_FAT16_32_CHS     = 0x06,
    VSF_MBR_PARTITION_TYPE_NTFS_EXFAT       = 0x07,
    VSF_MBR_PARTITION_TYPE_FAT32_CHS        = 0x0B,
    VSF_MBR_PARTITION_TYPE_FAT32_LBA        = 0x0C,
    VSF_MBR_PARTITION_TYPE_FAT16_32_2G_LBA  = 0x0E,
    VSF_MBR_PARTITION_TYPE_MS_EXT_LBA       = 0x0F,
} vk_malfs_partition_type_t;

typedef struct vk_malfs_mount_partition_t {
    void *fsinfo;
    __vk_malfs_info_t *malfs_info;
    const vk_fs_op_t *fsop;
    vk_file_t *root;
} vk_malfs_mount_partition_t;

typedef struct vk_malfs_dpt_t {
    uint8_t boot_signature;
    uint8_t start_head_number;
    uint8_t start_sector_number;
    uint8_t start_cylinder_number;
    uint8_t partition_type;
    uint8_t end_head_number;
    uint8_t end_sector_number;
    uint8_t end_cylinder_number;
    uint32_t sectors_preceding;
    uint32_t sectors_in_partition;
} VSF_CAL_PACKED vk_malfs_dpt_t;

typedef struct vk_malfs_mbr_t {
    uint8_t boot_record[440];
    uint8_t disk_signature[4];
    uint16_t reserved;
    vk_malfs_dpt_t dpt[4];
    uint16_t magic;
} VSF_CAL_PACKED vk_malfs_mbr_t;

typedef struct vk_malfs_gpt_header_t {
    char signature[8];              // should be "EFI PART"
    uint32_t revision;
    uint32_t header_size;
    uint32_t header_crc;
    uint32_t reserved;
    uint64_t current_lba;
    uint64_t backup_lba;
    uint64_t first_lba;
    uint64_t last_lba;
    uint8_t disk_guid[16];
    uint64_t partition_entry_lba;
    uint32_t partition_entry_num;
    uint32_t partition_entry_size;
    uint32_t partition_entry_crc;
} VSF_CAL_PACKED vk_malfs_gpt_header_t;

typedef struct vk_mal_gpt_entry_t {
    uint8_t type_guid[16];
    uint8_t guid[16];
    uint64_t first_lba;
    uint64_t last_lba;
    uint64_t attr;
    uint16_t name_utf16[36];
} VSF_CAL_PACKED vk_mal_gpt_entry_t;

/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

#if VSF_FS_USE_FATFS == ENABLED
struct {
    const vk_fs_op_t *fsop;
    uint8_t partition_type;
} static const __vk_malfs_ops[] = {
    // fn_probe of supported fsop MUST not be zero
#   if VSF_FS_USE_FATFS == ENABLED
    { &vk_fatfs_op, VSF_MBR_PARTITION_TYPE_FAT32_LBA },
#   endif
};
#endif

/*============================ IMPLEMENTATION ================================*/

void __vk_malfs_init(__vk_malfs_info_t *info)
{
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
    __vk_malfs_cache_node_t *nodes = cache->nodes;

    cache->info = info;
    for (uint_fast16_t i = 0; i < cache->number; i++) {
        nodes[i].block_addr = (uint64_t)-1;
    }
}

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#endif

__vsf_component_peda_private_entry(__vk_malfs_alloc_cache,
    uint64_t block_addr;
    __vk_malfs_cache_node_t *result;
) {
    vsf_peda_begin();
    __vk_malfs_cache_t *cache = (__vk_malfs_cache_t *)&vsf_this;
    __vk_malfs_info_t *info = cache->info;
    __vk_malfs_cache_node_t *nodes = cache->nodes;

    switch (evt) {
    case VSF_EVT_INIT: {
            __vk_malfs_cache_node_t *least_node = nodes, *unalloced_node = NULL;
            for (uint_fast16_t i = 0; i < cache->number; i++) {
                if (nodes[i].is_alloced) {
                    if (nodes[i].block_addr == vsf_local.block_addr) {
                        vsf_eda_return(&nodes[i]);
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
                vsf_eda_return(unalloced_node);
                return;
            }

            vsf_local.result = least_node;
            // missed, get least cache
            // TODO: add bit to indicate the node is not currently used
            if (!least_node->is_dirty) {
                vsf_eda_return(least_node);
                return;
            }

            least_node->is_dirty = false;
            __vk_malfs_write(info, least_node->block_addr, 1, __vk_malfs_get_cache_buff(&info->cache, least_node));
            break;
        }
        break;
    case VSF_EVT_RETURN:
        vsf_eda_return(vsf_local.result);
        break;
    }
    vsf_peda_end();
}

__vsf_component_peda_private_entry(__vk_malfs_read,
    uint64_t block_addr;
    uint32_t block_num;
    uint8_t *buff;

    uint8_t *cur_buff;
    uint8_t *read_buff;
    __vk_malfs_cache_node_t *node;
    uint32_t read_block_num;
    uint8_t state;
) {
    vsf_peda_begin();
    __vk_malfs_info_t *info = (__vk_malfs_info_t *)&vsf_this;
    vsf_mal_capability_t cap = vk_mal_capability(info->mal);
    enum {
        STATE_GET_CACHE,
        STATE_COMMIT_READ,
        STATE_FINISH_READ,
    };

    switch (evt) {
    case VSF_EVT_INIT:
        vsf_local.cur_buff = vsf_local.buff;
        if ((NULL == vsf_local.buff) || ((uintptr_t)vsf_local.buff & (cap.data_ptr_alignment - 1))) {
            if (NULL == vsf_local.buff) {
                VSF_FS_ASSERT(vsf_local.block_num == 1);
            }
            vsf_local.read_block_num = 1;
            vsf_local.state = STATE_GET_CACHE;
            __vk_malfs_alloc_cache(info, &info->cache, vsf_local.block_addr);
            return;
        }
        vsf_local.read_block_num = vsf_local.block_num;
        vsf_local.read_buff = vsf_local.buff;
        vsf_local.state = STATE_COMMIT_READ;
    case VSF_EVT_RETURN:
        switch (vsf_local.state) {
        case STATE_GET_CACHE: {
                vsf_local.node = (__vk_malfs_cache_node_t *)vsf_eda_get_return_value();
                VSF_FS_ASSERT(vsf_local.node != NULL);

                vsf_local.read_buff = __vk_malfs_get_cache_buff(&info->cache, vsf_local.node);
                if (vsf_local.node->block_addr == vsf_local.block_addr) {
                    vsf_eda_return(vsf_local.read_buff);
                    break;
                }
                vsf_local.node->block_addr = vsf_local.block_addr;
            }
            // fall through
        case STATE_COMMIT_READ:
        read_next_block:
            vsf_local.state = STATE_FINISH_READ;
            vk_mal_read(info->mal, info->block_size * vsf_local.block_addr,
                        info->block_size * vsf_local.read_block_num, vsf_local.read_buff);
            break;
        case STATE_FINISH_READ:
            if (NULL == vsf_local.buff) {
                vsf_local.buff = vsf_local.read_buff;
            } else if (vsf_local.buff != vsf_local.read_buff) {
                // unaligned buffer
                memcpy(vsf_local.cur_buff, vsf_local.read_buff, info->block_size);

                if (--vsf_local.block_num > 0) {
                    vsf_local.block_addr++;
                    vsf_local.node->block_addr = vsf_local.block_addr;
                    vsf_local.cur_buff += info->block_size;
                    goto read_next_block;
                }
            }

            vsf_eda_return((int32_t)vsf_eda_get_return_value() > 0 ? vsf_local.buff : NULL);
            break;
        }
        break;
    }
    vsf_peda_end();
}

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#elif   __IS_COMPILER_LLVM__
#   pragma clang diagnostic pop
#endif

vsf_err_t __vk_malfs_alloc_cache(__vk_malfs_info_t *info, __vk_malfs_cache_t *cache, uint_fast64_t block_addr)
{
    vsf_err_t err;
    __vsf_component_call_peda(__vk_malfs_alloc_cache, err, cache,
        .block_addr     = block_addr,
    )
    return err;
}

vsf_err_t __vk_malfs_read(__vk_malfs_info_t *info, uint_fast64_t block_addr, uint_fast32_t block_num, uint8_t *buff)
{
    // TODO: add lock and unlock
    vsf_err_t err;
    __vsf_component_call_peda(__vk_malfs_read, err, info,
        .block_addr     = block_addr,
        .block_num      = block_num,
        .buff           = buff,
    )
    return err;
}

vsf_err_t __vk_malfs_write(__vk_malfs_info_t *info, uint_fast64_t block_addr, uint_fast32_t block_num, uint8_t *buff)
{
    // TODO: add lock and unlock
    vsf_mal_capability_t cap = vk_mal_capability(info->mal);
    VSF_FS_ASSERT(((uintptr_t)buff & (cap.data_ptr_alignment - 1)));
    return vk_mal_write(info->mal, info->block_size * block_addr, info->block_size * block_num, buff);
}

#if VSF_USE_HEAP == ENABLED
void __vk_malfs_unmount(__vk_malfs_info_t *info)
{
    if (info->total_cb != NULL) {
        vsf_heap_free(info->total_cb);
    }
}

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#elif   __IS_COMPILER_LLVM__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#endif

__vsf_component_peda_private_entry(__vk_malfs_mount,
    uint64_t partition_entry_lba;
    vk_malfs_mount_partition_t cur_partition;
    uint8_t *sectbuf;
    uint8_t *sectbuf_probe;
    char *cur_root_name;
    uint32_t start_sector;
    uint32_t sector_count;
    vk_malfs_mount_state_t mount_state;
    uint8_t partition_idx;
    uint8_t partition_type;
    uint8_t partition_num;      // only used in gpt partition
    uint8_t try_idx;
)
{
    vsf_peda_begin();

    // partition_num is valid only for gpt
#define __is_gpt()              (vsf_local.partition_num > 0)

    static const uint8_t __partition_type_idxes[] = { VSF_MBR_PARTITION_TYPE_FAT12_CHS };
    vk_malfs_mounter_t *mounter = (vk_malfs_mounter_t *)&vsf_this;
    vk_malfs_mbr_t *mbr = (vk_malfs_mbr_t *)vsf_local.sectbuf;
    vk_malfs_dpt_t *dpt;

    vk_malfs_mount_partition_t *partition = &vsf_local.cur_partition;
    vk_mal_t *mal = mounter->mal;
    uint32_t mal_block_size = vk_mal_blksz(mal, 0, 0, VSF_MAL_OP_ERASE);
    int32_t result;

    switch (evt) {
    case VSF_EVT_INIT:
#if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
        if (mounter->mutex != NULL) {
            vsf_eda_mutex_init(mounter->mutex);
        }
#endif
        mounter->partition_mounted = 0;
        vsf_local.try_idx = 0;
        vsf_local.sectbuf = vsf_heap_malloc(mal_block_size);
        if (NULL == vsf_local.sectbuf) {
        return_not_enough_resources:
            mounter->err = VSF_ERR_NOT_ENOUGH_RESOURCES;
        do_return:
            if (vsf_local.sectbuf != NULL) {
                vsf_heap_free(vsf_local.sectbuf);
            }
            vsf_eda_return();
            break;
        }

        vsf_local.mount_state = VSF_MOUNT_STATE_READ_MBR;
        if (VSF_ERR_NONE != vk_mal_read(mal, 0 * mal_block_size,
                mal_block_size, vsf_local.sectbuf)) {
        return_failed:
            mounter->err = VSF_ERR_FAIL;
            goto do_return;
        }
        break;
    case VSF_EVT_RETURN:
        result = (int32_t)vsf_eda_get_return_value();
        switch (vsf_local.mount_state) {
        case VSF_MOUNT_STATE_READ_GPT_HEADER: {
                vk_malfs_gpt_header_t *gpt = (vk_malfs_gpt_header_t *)vsf_local.sectbuf;

                if (    (result != mal_block_size)
                    ||  strncmp(gpt->signature, "EFI PART", sizeof(gpt->signature))
                    ||  (le32_to_cpu(gpt->header_size) != 92)
                    ||  (le64_to_cpu(gpt->current_lba) != 1)
                    ||  (le32_to_cpu(gpt->partition_entry_size) != sizeof(vk_mal_gpt_entry_t))
                    ||  (le32_to_cpu(gpt->partition_entry_num) >= 256)) {
                    goto return_failed;
                }
                vsf_local.partition_entry_lba = le64_to_cpu(gpt->partition_entry_lba);
                vsf_local.partition_num = le32_to_cpu(gpt->partition_entry_num);
                if (0 == vsf_local.partition_num) {
                    mounter->err = VSF_ERR_NONE;
                    goto do_return;
                }
            }

            goto next_partition;
        case VSF_MOUNT_STATE_READ_GPT_PARTITION_ENTRY:
            if (result != mal_block_size) {
                goto return_failed;
            }
            goto next_partition;
        case VSF_MOUNT_STATE_PROBE_FS:
            if (result != mal_block_size) {
                vsf_heap_free(vsf_local.sectbuf_probe);
                goto return_failed;
            }
#if VSF_FS_USE_FATFS == ENABLED
            for (int i = 0; i < dimof(__vk_malfs_ops); i++) {
                if (VSF_ERR_NONE == __vk_malfs_ops[i].fsop->fn_probe(vsf_local.sectbuf_probe, mal_block_size)) {
                    vsf_heap_free(vsf_local.sectbuf_probe);
                    vsf_local.partition_type = __vk_malfs_ops[i].partition_type;
                    goto mount_partition;
                }
            }
#endif
            vsf_heap_free(vsf_local.sectbuf_probe);
            goto next_partition;
        case VSF_MOUNT_STATE_READ_MBR:
            if (    (result != mal_block_size)
                ||  (0xAA55 != le16_to_cpu(mbr->magic))) {
                goto return_failed;
            }
            vsf_local.partition_idx = -1;
            if (mbr->dpt[0].partition_type == 0xEE) {
                vsf_local.mount_state = VSF_MOUNT_STATE_READ_GPT_HEADER;
                // lba1 is gpt header
                if (VSF_ERR_NONE != vk_mal_read(mal, 1 * mal_block_size,
                        mal_block_size, vsf_local.sectbuf)) {
                    goto return_failed;
                }
                break;
            }

        next_partition:
            // partition_num is valid only for gpt
            if (__is_gpt()) {
                if (vsf_local.mount_state != VSF_MOUNT_STATE_READ_GPT_PARTITION_ENTRY) {
                    ++vsf_local.partition_idx;
                }
                if (vsf_local.partition_idx >= vsf_local.partition_num) {
                    mounter->err = VSF_ERR_NONE;
                    goto do_return;
                }

                uint8_t entry_number_in_block = mal_block_size / sizeof(vk_mal_gpt_entry_t);
                uint8_t partition_idx_in_block = vsf_local.partition_idx % entry_number_in_block;
                if (!partition_idx_in_block && (vsf_local.mount_state != VSF_MOUNT_STATE_READ_GPT_PARTITION_ENTRY)) {
                    uint8_t cur_partition_entry_lba = (vsf_local.partition_idx + entry_number_in_block - 1) / entry_number_in_block;
                    vsf_local.mount_state = VSF_MOUNT_STATE_READ_GPT_PARTITION_ENTRY;
                    if (VSF_ERR_NONE != vk_mal_read(mal,
                            (vsf_local.partition_entry_lba + cur_partition_entry_lba) * mal_block_size,
                            mal_block_size, vsf_local.sectbuf)) {
                        goto return_failed;
                    }
                    break;
                }

                vsf_local.mount_state = VSF_MOUNT_STATE_PARSE_GPT_PARTITION_ENTRY;
                vk_mal_gpt_entry_t *entry = (vk_mal_gpt_entry_t *)(vsf_local.sectbuf
                        + partition_idx_in_block * sizeof(vk_mal_gpt_entry_t));
                if (!(((uint64_t *)entry)[0] + ((uint64_t *)entry)[1])) {
                    mounter->err = VSF_ERR_NONE;
                    goto do_return;
                }

                vsf_local.start_sector = le32_to_cpu(entry->first_lba);
                vsf_local.sector_count = le32_to_cpu(entry->last_lba - entry->first_lba);
                vsf_local.mount_state = VSF_MOUNT_STATE_PROBE_FS;
                vsf_local.sectbuf_probe = vsf_heap_malloc(mal_block_size);
                if (NULL == vsf_local.sectbuf_probe) {
                    mounter->err = VSF_ERR_NOT_ENOUGH_RESOURCES;
                    goto do_return;
                }
                if (VSF_ERR_NONE != vk_mal_read(mal,
                        vsf_local.start_sector * mal_block_size,
                        mal_block_size, vsf_local.sectbuf_probe)) {
                    goto return_failed;
                }
                break;
            } else {
                if (++vsf_local.partition_idx >= dimof(mbr->dpt)) {
                    if (!mounter->partition_mounted) {
                        if (vsf_local.try_idx < dimof(__partition_type_idxes)) {
                            // nothing mounted, try using fat/exfat for the whole mal
                            vsf_local.partition_type = __partition_type_idxes[vsf_local.try_idx++];
                            vsf_local.start_sector = 0;
                            vsf_local.sector_count = mal->size / mal_block_size;
                            goto mount_partition;
                        }
                    }
                    mounter->err = VSF_ERR_NONE;
                    goto do_return;
                }
                dpt = &mbr->dpt[vsf_local.partition_idx];
                vsf_local.partition_type = dpt->partition_type;
                vsf_local.start_sector = le32_to_cpu(dpt->sectors_preceding);
                vsf_local.sector_count = le32_to_cpu(dpt->sectors_in_partition);
            }

        mount_partition:
            switch (vsf_local.partition_type) {
            default:
                // not supported
            case VSF_MBR_PARTITION_TYPE_NONE:
                goto next_partition;
#if VSF_FS_USE_FATFS == ENABLED
            case VSF_MBR_PARTITION_TYPE_FAT12_CHS:
            case VSF_MBR_PARTITION_TYPE_FAT16_16_32_CHS:
            case VSF_MBR_PARTITION_TYPE_FAT16_32_CHS:
            case VSF_MBR_PARTITION_TYPE_NTFS_EXFAT:     // for exfat only, ntfs is not supported
            case VSF_MBR_PARTITION_TYPE_FAT32_CHS:
            case VSF_MBR_PARTITION_TYPE_FAT32_LBA:
            case VSF_MBR_PARTITION_TYPE_FAT16_32_2G_LBA: {
                    typedef struct vk_malfs_fat_t {
#if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
                        vk_reentrant_mal_t fat_mal;
#else
                        vk_mim_mal_t fat_mal;
#endif
                        char root_name[8];          // "rootxxx\0"
                        implement_fatfs_info(0, 1);
                    } vk_malfs_fat_t;

                    vk_malfs_fat_t *malfs_fat = (vk_malfs_fat_t *)vsf_heap_malloc(sizeof(vk_malfs_fat_t) + mal_block_size);
                    if (NULL == malfs_fat) {
                        goto return_not_enough_resources;
                    }
                    memset(malfs_fat, 0, sizeof(*malfs_fat));

                    malfs_fat->total_cb = malfs_fat;
                    partition->fsinfo = &malfs_fat->use_as____vk_fatfs_info_t;
                    partition->malfs_info = &malfs_fat->use_as____vk_malfs_info_t;
                    partition->fsop = &vk_fatfs_op;

#if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
                    malfs_fat->fat_mal.mal = mal;
                    malfs_fat->fat_mal.mutex = mounter->mutex;
                    malfs_fat->fat_mal.drv = &vk_reentrant_mal_drv;
#else
                    malfs_fat->fat_mal.host_mal = mal;
                    malfs_fat->fat_mal.drv = &vk_mim_mal_drv;
#endif
                    malfs_fat->fat_mal.offset = vsf_local.start_sector * mal_block_size;
                    malfs_fat->fat_mal.size = vsf_local.sector_count * mal_block_size;
                    malfs_fat->mal = &malfs_fat->fat_mal.use_as__vk_mal_t;
                    init_fatfs_info_ex(malfs_fat, mal_block_size, 1, malfs_fat);

                    vsf_local.mount_state = VSF_MOUNT_STATE_CREATE_ROOT;
                    strcpy(malfs_fat->root_name, "root");
                    itoa(vsf_local.partition_idx, &malfs_fat->root_name[4], 10);
                    vsf_local.cur_root_name = malfs_fat->root_name;
                    if (VSF_ERR_NONE != vk_file_create(mounter->dir, vsf_local.cur_root_name, VSF_FILE_ATTR_DIRECTORY)) {
                        goto return_mount_failed;
                    }
                }
                break;
#endif
            }
            break;
        case VSF_MOUNT_STATE_CREATE_ROOT:
            if (result < 0) {
                goto return_mount_failed;
            }
            vsf_local.mount_state = VSF_MOUNT_STATE_OPEN_ROOT;
            if (VSF_ERR_NONE != vk_file_open(mounter->dir, vsf_local.cur_root_name, &partition->root)) {
                goto return_mount_failed_remove_dir;
            }
            break;
        case VSF_MOUNT_STATE_OPEN_ROOT:
            if (result < 0) {
                goto return_mount_failed_remove_dir;
            }
            vsf_local.mount_state = VSF_MOUNT_STATE_MOUNT;
            if (VSF_ERR_NONE != vk_fs_mount(partition->root, partition->fsop, partition->fsinfo)) {
                goto return_mount_failed_remove_dir;
            }
            break;
        case VSF_MOUNT_STATE_MOUNT:
            if (result < 0) {
            return_mount_failed_remove_dir:
                vsf_local.mount_state = VSF_MOUNT_STATE_UNLINK_ROOT;
                if (VSF_ERR_NONE != vk_file_unlink(mounter->dir, vsf_local.cur_root_name)) {
                    goto return_mount_failed;
                }
                break;
            } else if (partition->malfs_info->volume_name != NULL) {
                vsf_local.mount_state = VSF_MOUNT_STATE_RENAME_ROOT;
                vk_file_rename(mounter->dir, partition->root->name, NULL, partition->malfs_info->volume_name);
                break;
            }
            // fall through
        case VSF_MOUNT_STATE_RENAME_ROOT:
            mounter->partition_mounted++;
            goto next_partition;
        case VSF_MOUNT_STATE_UNLINK_ROOT:
        return_mount_failed:
            if (partition->malfs_info->total_cb != NULL) {
                vsf_heap_free(partition->malfs_info->total_cb);
            }
            goto next_partition;
        }
        break;
    }
    vsf_peda_end();
}

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic pop
#endif

vsf_err_t vk_malfs_mount(vk_malfs_mounter_t *mounter)
{
    vsf_err_t err;
    VSF_FS_ASSERT((mounter != NULL) && (mounter->mal != NULL) && (mounter->dir != NULL));
    __vsf_component_call_peda(__vk_malfs_mount, err, mounter);
    return err;
}
#else
void __vk_malfs_unmount(__vk_malfs_info_t *info)
{
}
#endif

#endif
