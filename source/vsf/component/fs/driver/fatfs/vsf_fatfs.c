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

#if VSF_USE_FS == ENABLED && VSF_FS_USE_FATFS == ENABLED

#define __VSF_FS_CLASS_INHERIT__
#define __VSF_MALFS_CLASS_INHERIT__
#define __VSF_FATFS_CLASS_IMPLEMENT

// for ctype.h
#include "utilities/vsf_utilities.h"
#include "../../vsf_fs.h"

/*============================ MACROS ========================================*/

#ifndef VSF_FATFS_CFG_MAX_FILENAME
#   define VSF_FATFS_CFG_MAX_FILENAME   128
#endif

#define FAT_ATTR_LFN                    0x0F
#define FAT_ATTR_READ_ONLY              0x01
#define FAT_ATTR_HIDDEN                 0x02
#define FAT_ATTR_SYSTEM                 0x04
#define FAT_ATTR_VOLUME_ID              0x08
#define FAT_ATTR_DIRECTORY              0x10
#define FAT_ATTR_ARCHIVE                0x20

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct fatfs_bpb_t {
    uint16_t BytsPerSec;
    uint8_t SecPerClus;
    uint16_t RsvdSecCnt;
    uint8_t NumFATs;
    uint16_t RootEntCnt;
    uint16_t TotSec16;
    uint8_t Media;
    uint16_t FATSz16;
    uint16_t SecPerTrk;
    uint16_t NumHeads;
    uint32_t HiddSec;
    uint32_t TotSec32;
} PACKED fatfs_bpb_t;

typedef struct fatfs_ebpb_t {
    uint8_t DrvNo;
    uint8_t Reserved;
    uint8_t BootSig;
    uint32_t VolID;
    uint8_t VolLab[11];
    uint8_t FilSysType[8];
} PACKED fatfs_ebpb_t;

typedef struct fatfs_dbr_t {
    uint8_t jmp[3];
    uint8_t oem[8];
    // bpb All 0 for exFAT
    fatfs_bpb_t bpb;
    union {
        struct {
            struct {
                uint32_t FATSz32;
                uint16_t ExtFlags;
                uint16_t FSVer;
                uint32_t RootClus;
                uint16_t FSInfo;
                uint16_t BkBootSec;
                uint8_t Reserved[12];
            } PACKED bpb;
            fatfs_ebpb_t ebpb;
            uint8_t Bootstrap[420];
        } PACKED fat32;
        struct {
            fatfs_ebpb_t ebpb;
            uint8_t Bootstrap[448];
        } PACKED fat1216;
        struct {
            uint8_t Reserved_All0[28];
            struct {
                uint64_t SecStart;
                uint64_t SecCount;
                uint32_t FATSecStart;
                uint32_t FATSecCount;
                uint32_t ClusSecStart;
                uint32_t ClusSecCount;
                uint32_t RootClus;
                uint32_t VolSerial;
                struct {
                    uint8_t Minor;
                    uint8_t Major;
                } PACKED Ver;
                uint16_t VolState;
                uint8_t SecBits;
                uint8_t SPCBits;
                uint8_t NumFATs;
                uint8_t DrvNo;
                uint8_t AllocPercnet;
                uint8_t Reserved_All0[397];
            } PACKED bpb;
        } PACKED exfat;
    } PACKED;
    uint16_t Magic;
} PACKED fatfs_dbr_t;

typedef struct fatfs_dentry_t {
    union {
        struct {
            char Name[8];
            char Ext[3];
            uint8_t Attr;
            uint8_t LCase;
            uint8_t CrtTimeTenth;
            uint16_t CrtTime;
            uint16_t CrtData;
            uint16_t LstAccData;
            uint16_t FstClusHi;
            uint16_t WrtTime;
            uint16_t WrtData;
            uint16_t FstClusLo;
            uint32_t FileSize;
        } PACKED fat;
    } PACKED;
} PACKED fatfs_dentry_t;

typedef struct vk_fatfs_read_local {
    uint32_t cur_cluster;
    uint32_t cur_sector;
    uint64_t cur_offset;
    uint32_t cur_size;
    uint32_t cur_run_size;
    uint32_t cur_run_sector;
} vk_fatfs_read_local;

typedef struct vk_fatfs_lookup_local {
    uint32_t cur_cluster;
    uint32_t cur_sector;
    char *filename;
    uint32_t cur_sector_in_cluster;
    vk_fatfs_dentry_parser_t dparser;
} vk_fatfs_lookup_local;

/*============================ PROTOTYPES ====================================*/

dcl_vsf_peda_methods(static, __vk_fatfs_mount)
dcl_vsf_peda_methods(static, __vk_fatfs_unmount)
dcl_vsf_peda_methods(static, __vk_fatfs_lookup)
dcl_vsf_peda_methods(static, __vk_fatfs_read)
dcl_vsf_peda_methods(static, __vk_fatfs_write)
dcl_vsf_peda_methods(static, __vk_fatfs_close)

/*============================ GLOBAL VARIABLES ==============================*/

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-function-type"
#endif

const vk_fs_op_t vk_fatfs_op = {
    .fn_mount               = (vsf_peda_evthandler_t)vsf_peda_func(__vk_fatfs_mount),
    .fn_unmount             = (vsf_peda_evthandler_t)vsf_peda_func(__vk_fatfs_unmount),
#if VSF_FS_CFG_USE_CACHE == ENABLED
    .fn_sync                = vk_file_dummy,
#endif
    .fop                    = {
        .read_local_size    = sizeof(vk_fatfs_read_local),
        .fn_read            = (vsf_peda_evthandler_t)vsf_peda_func(__vk_fatfs_read),
        .fn_write           = (vsf_peda_evthandler_t)vsf_peda_func(__vk_fatfs_write),
        .fn_close           = (vsf_peda_evthandler_t)vsf_peda_func(__vk_fatfs_close),
        .fn_resize          = (vsf_peda_evthandler_t)vsf_peda_func(vk_dummyfs_not_support),
    },
    .dop                    = {
        .lookup_local_size  = sizeof(vk_fatfs_lookup_local),
        .fn_lookup          = (vsf_peda_evthandler_t)vsf_peda_func(__vk_fatfs_lookup),
        .fn_create          = (vsf_peda_evthandler_t)vsf_peda_func(vk_dummyfs_not_support),
        .fn_unlink          = (vsf_peda_evthandler_t)vsf_peda_func(vk_dummyfs_not_support),
        .fn_chmod           = (vsf_peda_evthandler_t)vsf_peda_func(vk_dummyfs_not_support),
        .fn_rename          = (vsf_peda_evthandler_t)vsf_peda_func(vk_dummyfs_not_support),
    },
};

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

/*============================ LOCAL VARIABLES ===============================*/

static const uint8_t __vk_fatfs_fat_bitsize[] = {
    [VSF_FAT_NONE]  = 0,
    [VSF_FAT_12]    = 12,
    [VSF_FAT_16]    = 16,
    [VSF_FAT_32]    = 32,
    [VSF_FAT_EX]    = 32,
};

/*============================ IMPLEMENTATION ================================*/

bool vk_fatfs_is_lfn(char *name)
{
    char *ext = NULL;
    bool has_lower = false, has_upper = false;
    uint_fast32_t i, name_len = 0, ext_len = 0;

    if (name != NULL) {
        name_len = strlen(name);
        ext = vk_file_getfileext(name);
    }
    if (ext != NULL) {
        ext_len = strlen(ext);
        name_len -= ext_len + 1;    // 1 more byte for dot
    }
    if ((name_len > 8) || (ext_len > 3)) {
        return true;
    }

    for (i = 0; name[i] != '\0'; i++) {
        if (islower((int)name[i])) {
            has_lower = true;
        }
        if (isupper((int)name[i])) {
            has_upper = true;
        }
    }
    return has_lower && has_upper;
}

static vsf_err_t __vk_fatfs_parse_dbr(__vk_fatfs_info_t *info, uint8_t *buff)
{
    fatfs_dbr_t *dbr = (fatfs_dbr_t *)buff;
    uint_fast16_t sector_size;
    uint_fast32_t tmp32;

    if (dbr->Magic != cpu_to_be16(0x55AA)) {
        return VSF_ERR_FAIL;
    }

    for (tmp32 = 0; (tmp32 < 53) && *buff; tmp32++, buff++);
    if (tmp32 < 53) {
        // normal FAT12, FAT16, FAT32
        uint_fast32_t sector_num, cluster_num;
        uint_fast16_t reserved_size, root_entry;

        sector_size = le16_to_cpu(dbr->bpb.BytsPerSec);
        info->sector_size_bits = msb(sector_size);
        info->cluster_size_bits = msb(dbr->bpb.SecPerClus);
        reserved_size = le16_to_cpu(dbr->bpb.RsvdSecCnt);
        info->fat_num = dbr->bpb.NumFATs;
        sector_num = le16_to_cpu(dbr->bpb.TotSec16);
        if (!sector_num) {
            sector_num = le32_to_cpu(dbr->bpb.TotSec32);
        }
        info->fat_size = dbr->bpb.FATSz16 ?
            le16_to_cpu(dbr->bpb.FATSz16) : le32_to_cpu(dbr->fat32.bpb.FATSz32);

        // sector_size MUST the same as mal blocksize, and MUST following value:
        //         512, 1024, 2048, 4096
        // SecPerClus MUST be power of 2
        // RsvdSecCnt CANNOT be 0
        // NumFATs CANNOT be 0
        if (    (   (sector_size != info->block_size)
                ||  (   (sector_size != 512) && (sector_size != 1024)
                    &&  (sector_size != 2048) && (sector_size != 4096)))
            ||  (!dbr->bpb.SecPerClus || !info->cluster_size_bits)
            ||  !reserved_size
            ||  !info->fat_num
            ||  !sector_num
            ||  !info->fat_size) {
            return VSF_ERR_FAIL;
        }

        root_entry = le16_to_cpu(dbr->bpb.RootEntCnt);
        if (root_entry) {
            info->root_size = ((root_entry >> 5) + sector_size - 1) / sector_size;
        } else {
            info->root_size = 0;
        }
        // calculate base
        info->fat_sector = reserved_size;
        info->root_sector = info->fat_sector + info->fat_num * info->fat_size;
        info->data_sector = info->root_sector + info->root_size;
        // calculate cluster number: note that cluster starts from root_cluster
        cluster_num = (sector_num - reserved_size) >> info->cluster_size_bits;

        // for FAT32 RootEntCnt MUST be 0
        if (!root_entry) {
            // FAT32
            info->type = VSF_FAT_32;

            // for FAT32, TotSec16 and FATSz16 MUST be 0
            if (dbr->bpb.FATSz16 || dbr->bpb.TotSec16) {
                return VSF_ERR_FAIL;
            }

            // RootClus CANNOT be less than 2
            info->root.first_cluster = le32_to_cpu(dbr->fat32.bpb.RootClus);
            if (info->root.first_cluster < 2) {
                return VSF_ERR_FAIL;
            }

            info->cluster_num = info->root.first_cluster + cluster_num;
        } else {
            // FAT12 or FAT16
            info->type = (cluster_num < 4085) ? VSF_FAT_12 : VSF_FAT_16;

            // root has no cluster
            info->root.first_cluster = 0;
            info->cluster_num = cluster_num + 2;
        }
    } else {
        // bpb all 0, exFAT
        info->type = VSF_FAT_EX;

        info->sector_size_bits = dbr->exfat.bpb.SecBits;
        info->cluster_size_bits = dbr->exfat.bpb.SPCBits;
        info->fat_num = dbr->exfat.bpb.NumFATs;
        info->fat_size = le32_to_cpu(dbr->exfat.bpb.FATSecCount);
        info->root_size = 0;
        info->fat_sector = le32_to_cpu(dbr->exfat.bpb.FATSecStart);
        info->root_sector = le32_to_cpu(dbr->exfat.bpb.ClusSecStart);
        info->data_sector = info->root_sector;
        info->root.first_cluster = le32_to_cpu(dbr->exfat.bpb.RootClus);
        info->cluster_num = le32_to_cpu(dbr->exfat.bpb.ClusSecCount) + 2;

        // SecBits CANNOT be smaller than 9, which is 512 byte
        // RootClus CANNOT be less than 2
        if ((info->sector_size_bits < 9) || (info->root.first_cluster < 2)) {
            return VSF_ERR_FAIL;
        }
    }

    return VSF_ERR_NONE;
}

static uint_fast32_t __vk_fatfs_clus2sec(__vk_fatfs_info_t *fsinfo, uint_fast32_t cluster)
{
    cluster -= fsinfo->root.first_cluster ? fsinfo->root.first_cluster : 2;
    return fsinfo->data_sector + (cluster << fsinfo->cluster_size_bits);
}

static bool __vk_fatfs_fat_entry_is_valid(__vk_fatfs_info_t *fsinfo, uint_fast32_t cluster)
{
    uint_fast8_t fat_bit = __vk_fatfs_fat_bitsize[fsinfo->type];
    uint_fast32_t mask = ((1ULL << fat_bit) - 1) & 0x0FFFFFFF;

    cluster &= (32 == fat_bit) ? 0x0FFFFFFF : mask;
    return (cluster >= 2) && (cluster <= mask);
}

static uint_fast8_t __vk_fatfs_parse_file_attr(uint_fast8_t fat_attr)
{
    uint_fast8_t attr = VSF_FILE_ATTR_READ | VSF_FILE_ATTR_WRITE;
    if (fat_attr & FAT_ATTR_READ_ONLY) {
        attr &= ~VSF_FILE_ATTR_WRITE;
    }
    if (fat_attr & FAT_ATTR_HIDDEN) {
        attr |= VSF_FILE_ATTR_HIDDEN;
    }
    if (fat_attr & FAT_ATTR_DIRECTORY) {
        attr |= VSF_FILE_ATTR_DIRECTORY;
    }
    return attr;
}

static bool __vk_fatfs_fat_entry_is_eof(__vk_fatfs_info_t *fsinfo, uint_fast32_t cluster)
{
    uint_fast8_t fat_bit = __vk_fatfs_fat_bitsize[fsinfo->type];
    uint_fast32_t mask = ((1ULL << fat_bit) - 1) & 0x0FFFFFFF;

    cluster &= (32 == fat_bit) ? 0x0FFFFFFF : mask;
    return (cluster >= (mask - 8)) && (cluster <= mask);
}

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#elif   __IS_COMPILER_LLVM__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#endif

// entry_num is the number of entry remain in buffer,
//     and the entry_num of entry for current filename parsed
// lfn is unicode encoded, but we just support ascii
// if a filename parsed, parser->entry will point to the sfn
bool vk_fatfs_parse_dentry_fat(vk_fatfs_dentry_parser_t *parser)
{
    fatfs_dentry_t *entry = (fatfs_dentry_t *)parser->entry;
    bool parsed = false;

    while (parser->entry_num-- > 0) {
        if (!entry->fat.Name[0]) {
            break;
        } else if (entry->fat.Name[0] != (char)0xE5) {
            char *ptr;
            int i;

            if (entry->fat.Attr == FAT_ATTR_LFN) {
                const uint8_t lfn_offsets[] = {1, 3, 5, 7, 9, 14, 16, 18, 20, 22, 24, 28, 30};
                uint_fast8_t index = entry->fat.Name[0];
                uint_fast8_t pos = ((index & 0x0F) - 1) * 13;
                uint_fast16_t uchar;
                uint8_t *buf = (uint8_t *)entry;

                parser->lfn = index & 0x0F;
                ptr = parser->filename + (pos << 1);

                for (uint_fast8_t i = 0; i < dimof(lfn_offsets); i++) {
                    uchar = buf[lfn_offsets[i]] + (buf[lfn_offsets[i] + 1] << 8);
                    if (0 == uchar) {
                        break;
                    } else {
                        *ptr++ = (char)(uchar >> 0);
                        *ptr++ = (char)(uchar >> 8);
                    }
                }

                if ((index & 0xF0) == 0x40) {
                    *ptr++ = '\0';
                    *ptr = '\0';
                }
            } else if (entry->fat.Attr != FAT_ATTR_VOLUME_ID) {
                bool lower;
                if (parser->lfn == 1) {
                    // previous lfn parsed, igure sfn and return
                    uint16_t *uchar = (uint16_t *)parser->filename;
                    parser->is_unicode = false;
                    while (*uchar != 0) {
                        if (*uchar++ >= 128) {
                            parser->is_unicode = true;
                            break;
                        }
                    }
                    if (!parser->is_unicode) {
                        char *ptr = parser->filename;
                        uchar = (uint16_t *)parser->filename;
                        while (*uchar != 0) {
                            *ptr++ = *uchar++;
                        }
                    }

                    parser->lfn = 0;
                    parsed = true;
                    break;
                }

                parser->lfn = 0;
                ptr = parser->filename;
                lower = (entry->fat.LCase & 0x08) > 0;
                for (i = 0; (i < 8) && (entry->fat.Name[i] != ' '); i++) {
                    *ptr = entry->fat.Name[i];
                    if (lower) *ptr = tolower(*ptr);
                    ptr++;
                }
                if (entry->fat.Ext[0] != ' ') {
                    *ptr++ = '.';
                    lower = (entry->fat.LCase & 0x10) > 0;
                    for (i = 0; (i < 3) && (entry->fat.Ext[i] != ' '); i++) {
                        *ptr = entry->fat.Ext[i];
                        if (lower) *ptr = tolower(*ptr);
                        ptr++;
                    }
                }
                *ptr = '\0';
                parser->is_unicode = false;
                parsed = true;
                break;
            }
        } else if (parser->lfn > 0) {
            // an erased entry with previous parsed lfn entry?
            parser->lfn = 0;
        }

        entry++;
    }
    parser->entry = (uint8_t *)entry;
    return parsed;
}

__vsf_component_peda_ifs_entry(__vk_fatfs_unmount, vk_fs_unmount)
{
    vsf_peda_begin();
    vk_vfs_file_t *dir = (vk_vfs_file_t *)&vsf_this;
    __vk_fatfs_info_t *fsinfo = dir->subfs.data;
    __vk_malfs_info_t *malfs_info = &fsinfo->use_as____vk_malfs_info_t;

    __vk_malfs_unmount(malfs_info);
    vsf_eda_return();
    vsf_peda_end();
}

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_suppress=pe111
#endif

__vsf_component_peda_ifs_entry(__vk_fatfs_mount, vk_fs_mount)
{
    vsf_peda_begin();
    enum {
        MOUNT_STATE_PARSE_DBR,
        MOUNT_STATE_PARSE_ROOT,
    };
    vk_vfs_file_t *dir = (vk_vfs_file_t *)&vsf_this;
    __vk_fatfs_info_t *fsinfo = dir->subfs.data;
    __vk_malfs_info_t *malfs_info = &fsinfo->use_as____vk_malfs_info_t;
    VSF_FS_ASSERT(fsinfo != NULL);

    switch (evt) {
    case VSF_EVT_INIT:
        __vk_malfs_init(malfs_info);
        vsf_eda_frame_user_value_set(MOUNT_STATE_PARSE_DBR);
        __vk_malfs_read(malfs_info, 0, 1, NULL);
        break;
    case VSF_EVT_RETURN: {
            __vsf_frame_uint_t state;
            uint8_t *buff = (uint8_t *)vsf_eda_get_return_value();

            if (NULL == buff) {
                VSF_FS_ASSERT(false);
                vsf_eda_return(VSF_ERR_FAIL);
                return;
            }

            vsf_eda_frame_user_value_get(&state);
            switch (state) {
            case MOUNT_STATE_PARSE_DBR:
                if (VSF_ERR_NONE != __vk_fatfs_parse_dbr(fsinfo, buff)) {
                return_fail:
                    VSF_FS_ASSERT(false);
                    vsf_eda_return(VSF_ERR_FAIL);
                    return;
                }
                vsf_eda_frame_user_value_set(MOUNT_STATE_PARSE_ROOT);
                __vk_malfs_read(malfs_info, fsinfo->root_sector, 1, NULL);
                break;
            case MOUNT_STATE_PARSE_ROOT: {
                    fatfs_dentry_t *dentry = (fatfs_dentry_t *)buff;
                    malfs_info->volume_name = NULL;
                    if (VSF_FAT_EX == fsinfo->type) {
                        // TODO: parse VolID for exfat
                        goto return_fail;
                    } else if (FAT_ATTR_VOLUME_ID == dentry->fat.Attr) {
                        fsinfo->fat_volume_name[11] = '\0';
                        memcpy(fsinfo->fat_volume_name, dentry->fat.Name, 11);
                        for (int_fast8_t i = 10; i >= 0; i--) {
                            if (fsinfo->fat_volume_name[i] != ' ') {
                                break;
                            }
                            fsinfo->fat_volume_name[i] = '\0';
                        }
                        malfs_info->volume_name = fsinfo->fat_volume_name;
                    }
                    fsinfo->root.info = fsinfo;
                    dir->subfs.root = &fsinfo->root.use_as__vk_file_t;
                    vsf_eda_return(VSF_ERR_NONE);
                }
                break;
            }
        }
        break;
    }
    vsf_peda_end();
}

__vsf_component_peda_private_entry(__vk_fatfs_get_fat_entry,,
    uint32_t cluster;
    uint32_t *entry;
    ,
    uint32_t cur_fat_bit;
) {
    vsf_peda_begin();
    enum {
        LOOKUP_FAT_STATE_START,
        LOOKUP_FAT_STATE_PARSE,
    };
    __vk_fatfs_info_t *fsinfo = (__vk_fatfs_info_t *)&vsf_this;
    __vk_malfs_info_t *malfs_info = &fsinfo->use_as____vk_malfs_info_t;
    uint_fast8_t fat_bit = __vk_fatfs_fat_bitsize[fsinfo->type];
    uint_fast32_t start_bit = vsf_local.cluster * fat_bit;
    uint_fast32_t sector_bit = 1 << (fsinfo->sector_size_bits + 3);
    uint_fast32_t start_bit_sec = start_bit & (sector_bit - 1);

    switch (evt) {
    case VSF_EVT_INIT:
        vsf_local.cur_fat_bit = 0;
        vsf_eda_frame_user_value_set(LOOKUP_FAT_STATE_START);
        // fall through
    case VSF_EVT_RETURN: {
            __vsf_frame_uint_t state;
            vsf_eda_frame_user_value_get(&state);
            switch (state) {
            case LOOKUP_FAT_STATE_START:
            read_fat_sector:
                if (vsf_local.cur_fat_bit < fat_bit) {
                    start_bit = fsinfo->fat_sector + (start_bit >> (fsinfo->sector_size_bits + 3));
                    start_bit += vsf_local.cur_fat_bit ? 1 : 0;

                    vsf_eda_frame_user_value_set(LOOKUP_FAT_STATE_PARSE);
                    __vk_malfs_read(malfs_info, start_bit, 1, NULL);
                }
                break;
            case LOOKUP_FAT_STATE_PARSE: {
                    uint8_t *buff = (uint8_t *)vsf_eda_get_return_value();
                    if (NULL == buff) {
                        VSF_FS_ASSERT(false);
                        vsf_eda_return(VSF_ERR_FAIL);
                        break;
                    }

                    if (vsf_local.cur_fat_bit) {
                        *vsf_local.entry |= get_unaligned_le32(buff) << vsf_local.cur_fat_bit;
                        *vsf_local.entry &= (1 << fat_bit) - 1;
                        vsf_eda_return(VSF_ERR_NONE);
                        break;
                    }

                    vsf_local.cur_fat_bit += min(fat_bit, sector_bit - start_bit_sec);
                    *vsf_local.entry = get_unaligned_le32(&buff[start_bit_sec >> 3]);
                    *vsf_local.entry = (*vsf_local.entry >> (start_bit & 7)) & ((1 << vsf_local.cur_fat_bit) - 1);
                    goto read_fat_sector;
                }
            }
        }
    }
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_fatfs_lookup, vk_file_lookup,
    implement(vk_fatfs_lookup_local)
) {
    vsf_peda_begin();
    enum {
        LOOKUP_STATE_READ_SECTOR,
        LOOKUP_STATE_PARSE_SECTOR,
        LOOKUP_STATE_READ_FAT,
    };
    vk_fatfs_file_t *dir = (vk_fatfs_file_t *)&vsf_this;
    __vk_fatfs_info_t *fsinfo = (__vk_fatfs_info_t *)dir->info;
    __vk_malfs_info_t *malfs_info = &fsinfo->use_as____vk_malfs_info_t;
    const char *name = vsf_local.name;
    vsf_err_t err = VSF_ERR_NONE;

    switch (evt) {
    case VSF_EVT_INIT:
        vsf_local.filename = vsf_heap_malloc(VSF_FATFS_CFG_MAX_FILENAME);
        if (NULL == vsf_local.filename) {
            err = VSF_ERR_NOT_ENOUGH_RESOURCES;
            goto exit;
        }
        vsf_local.cur_cluster = dir->first_cluster;
        if (!dir->first_cluster) {
            if ((fsinfo->type != VSF_FAT_12) && (fsinfo->type != VSF_FAT_16)) {
                VSF_FS_ASSERT(false);
                err = VSF_ERR_FAIL;
                goto exit;
            }
            vsf_local.cur_sector = fsinfo->root_sector;
        } else {
            vsf_local.cur_sector = __vk_fatfs_clus2sec(fsinfo, vsf_local.cur_cluster);
        }
        vsf_local.cur_sector_in_cluster = 0;
        vsf_local.dparser.lfn = 0;
        vsf_eda_frame_user_value_set(LOOKUP_STATE_READ_SECTOR);

        // fall through
    case VSF_EVT_RETURN: {
            __vsf_frame_uint_t state;
            vsf_eda_frame_user_value_get(&state);
            switch (state) {
            case LOOKUP_STATE_READ_SECTOR:
            read_sector:
                vsf_eda_frame_user_value_set(LOOKUP_STATE_PARSE_SECTOR);
                __vk_malfs_read(malfs_info, vsf_local.cur_sector, 1, NULL);
                break;
            case LOOKUP_STATE_PARSE_SECTOR: {
                    fatfs_dentry_t *dentry = (fatfs_dentry_t *)vsf_eda_get_return_value();
                    vk_fatfs_dentry_parser_t *dparser = &vsf_local.dparser;

                    if (NULL == dentry) {
                        goto exit;
                    }

                    if (fsinfo->type == VSF_FAT_EX) {
                        err = VSF_ERR_NOT_SUPPORT;
                        goto exit;
                    }

                    dparser->entry = (uint8_t *)dentry;
                    dparser->entry_num = 1 << (fsinfo->sector_size_bits - 5);
                    dparser->filename = vsf_local.filename;
                    while (dparser->entry_num) {
                        if (vk_fatfs_parse_dentry_fat(dparser)) {
                            if (    (name && vk_file_is_match((char *)name, dparser->filename))
                                ||  (!name && !vsf_local.idx--)) {

                                // matched
                                vk_fatfs_file_t *fatfs_file = (vk_fatfs_file_t *)vk_file_alloc(sizeof(vk_fatfs_file_t));
                                if (NULL == fatfs_file) {
                                fail_mem:
                                    err = VSF_ERR_NOT_ENOUGH_RESOURCES;
                                    goto exit;
                                }

                                uint_fast16_t filename_len;
                                if (dparser->is_unicode) {
                                    uint16_t *uchar = (uint16_t *)dparser->filename;
                                    filename_len = 0;
                                    while (*uchar++ != 0) {
                                        filename_len += 2;
                                    }
                                    filename_len += 2;
                                } else {
                                    filename_len = strlen(dparser->filename) + 1;
                                }

                                fatfs_file->name = vsf_heap_malloc(filename_len);
                                if (NULL == fatfs_file->name) {
                                    vk_file_free(&fatfs_file->use_as__vk_file_t);
                                    goto fail_mem;
                                }
                                memcpy(fatfs_file->name, dparser->filename, filename_len);

                                dentry = (fatfs_dentry_t *)dparser->entry;
                                fatfs_file->attr = (vk_file_attr_t)__vk_fatfs_parse_file_attr(dentry->fat.Attr);
                                fatfs_file->coding = dparser->is_unicode ? VSF_FILE_NAME_CODING_UCS2 : VSF_FILE_NAME_CODING_UNKNOWN;
                                fatfs_file->fsop = &vk_fatfs_op;
                                fatfs_file->size = dentry->fat.FileSize;
                                fatfs_file->info = fsinfo;
                                fatfs_file->first_cluster = dentry->fat.FstClusLo + (dentry->fat.FstClusHi << 16);

                                *vsf_local.result = &fatfs_file->use_as__vk_file_t;
                                goto exit;
                            }
                            dparser->entry += 32;
                        } else if (dparser->entry_num > 0) {
                            err = VSF_ERR_NOT_AVAILABLE;
                            goto exit;
                        } else {
                            break;
                        }
                    }

                    if (    (!vsf_local.cur_cluster && (vsf_local.cur_sector < fsinfo->root_size))
                        ||  (vsf_local.cur_sector_in_cluster < ((1 << fsinfo->cluster_size_bits) - 1))) {
                        // not found in current sector, find next sector
                        vsf_local.cur_sector++;
                        vsf_local.cur_sector_in_cluster++;
                        goto read_sector;
                    } else {
                        vsf_err_t err;
                        // not found in current cluster, find next cluster if exists
                        vsf_eda_frame_user_value_set(LOOKUP_STATE_READ_FAT);
                        __vsf_component_call_peda(__vk_fatfs_get_fat_entry, err, fsinfo,
                            .cluster = vsf_local.cur_cluster,
                            .entry = &vsf_local.cur_cluster,
                        )
                        UNUSED_PARAM(err);
                    }
                }
                break;
            case LOOKUP_STATE_READ_FAT:
                if (    ((vsf_err_t)vsf_eda_get_return_value() != VSF_ERR_NONE)
                    ||  !__vk_fatfs_fat_entry_is_valid(fsinfo, vsf_local.cur_cluster)
                    ||  __vk_fatfs_fat_entry_is_eof(fsinfo, vsf_local.cur_cluster)) {
                    err = VSF_ERR_NOT_AVAILABLE;
                    goto exit;
                }

                // remove MSB 4-bit for 32-bit FAT entry
                vsf_local.cur_cluster &= 0x0FFFFFFF;
                vsf_local.cur_sector = __vk_fatfs_clus2sec(fsinfo, vsf_local.cur_cluster);
                vsf_local.cur_sector_in_cluster = 0;
                goto read_sector;
            }
        }
        break;
    }
    return;
exit:
    if (vsf_local.filename != NULL) {
        vsf_heap_free(vsf_local.filename);
    }
    vsf_eda_return(err);
    vsf_peda_end();
}

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_warning=pe111
#endif

__vsf_component_peda_ifs_entry(__vk_fatfs_close, vk_file_close)
{
    vsf_peda_begin();
    vk_fatfs_file_t *fatfs_file = (vk_fatfs_file_t *)&vsf_this;

    // TODO: flush file buffer if enabled
    if (fatfs_file->name != NULL) {
        vsf_heap_free(fatfs_file->name);
    }
    vk_file_free(&fatfs_file->use_as__vk_file_t);
    vsf_eda_return(VSF_ERR_NONE);
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_fatfs_read, vk_file_read,
    implement(vk_fatfs_read_local)
) {
    vsf_peda_begin();
    enum {
        READ_STATE_GET_FAT_ENTRY,
        READ_STATE_GET_FAT_ENTRY_DONE,
        READ_STATE_READ,
        READ_STATE_READ_DONE,
        READ_STATE_GET_NEXT_FAT_ENTRY_DONE,
    };

    vk_fatfs_file_t *fatfs_file = (vk_fatfs_file_t *)&vsf_this;
    __vk_fatfs_info_t *fsinfo = (__vk_fatfs_info_t *)fatfs_file->info;
    __vk_malfs_info_t *malfs_info = &fsinfo->use_as____vk_malfs_info_t;
    uint32_t clustersize = 1 << (fsinfo->cluster_size_bits + fsinfo->sector_size_bits);

    switch (evt) {
    case VSF_EVT_INIT:
        if (vsf_local.size > (fatfs_file->size - vsf_local.offset)) {
            vsf_local.size = fatfs_file->size - vsf_local.offset;
        }
        // locate the first cluster for access
        vsf_local.cur_cluster = fatfs_file->first_cluster;
        vsf_local.cur_offset = 0;
        vsf_eda_frame_user_value_set(READ_STATE_GET_FAT_ENTRY);
        // fall through
    case VSF_EVT_RETURN: {
            union {
                uintptr_t value;
                vsf_err_t err;
                int_fast32_t size;
                uint8_t *buffer;
            } result;
            __vsf_frame_uint_t state;

            result.value = vsf_eda_get_return_value();
            vsf_eda_frame_user_value_get(&state);
            switch (state) {
            case READ_STATE_GET_FAT_ENTRY_DONE:
                if (result.err < 0) {
                    vsf_eda_return(result.err);
                    break;
                }
                if (!__vk_fatfs_fat_entry_is_valid(fsinfo, vsf_local.cur_cluster)) {
                    vsf_eda_return(VSF_ERR_FAIL);
                    break;
                }

                // remove MSB 4-bit for 32-bit FAT entry
                vsf_local.cur_cluster &= 0x0FFFFFFF;
                vsf_local.cur_offset += clustersize;
                // fall through
            case READ_STATE_GET_FAT_ENTRY:
                if ((vsf_local.cur_offset + clustersize) <= vsf_local.offset) {
                    vsf_err_t err;
                    vsf_eda_frame_user_value_set(READ_STATE_GET_FAT_ENTRY_DONE);
                    __vsf_component_call_peda(__vk_fatfs_get_fat_entry, err, fsinfo,
                        .cluster = vsf_local.cur_cluster,
                        .entry = &vsf_local.cur_cluster,
                    );
                    UNUSED_PARAM(err);
                    break;
                }

                vsf_local.cur_size = 0;
                vsf_local.cur_sector = __vk_fatfs_clus2sec(fsinfo, vsf_local.cur_cluster);
                vsf_local.cur_sector += (vsf_local.offset & (clustersize - 1)) >> fsinfo->sector_size_bits;
                vsf_local.cur_offset = vsf_local.offset & ~((1 << fsinfo->sector_size_bits) - 1);
                vsf_eda_frame_user_value_set(READ_STATE_READ);
                // fall through
            case READ_STATE_READ:
            read_next:
                if (vsf_local.size) {
                    uint8_t *buffer;

                    if (vsf_local.cur_offset != vsf_local.offset) {
                        // read first non-page-aligned data
                        vsf_local.cur_run_size = vsf_local.size & ((1 << fsinfo->sector_size_bits) - 1);
                        vsf_local.cur_run_sector = 1;
                        buffer = NULL;
                    } else if (vsf_local.size < (1 << fsinfo->sector_size_bits)) {
                        // read last non-page-aligned data
                        vsf_local.cur_run_size = vsf_local.size;
                        vsf_local.cur_run_sector = 1;
                        buffer = NULL;
                    } else {
                        // read page-aligned data in cluster
                        // get remain sector in clusrer
                        vsf_local.cur_run_sector =
                            vsf_local.cur_sector & ((1 << fsinfo->cluster_size_bits) - 1);
                        vsf_local.cur_run_sector = min(vsf_local.cur_run_sector,
                            vsf_local.size >> fsinfo->sector_size_bits);
                        vsf_local.cur_run_size = vsf_local.cur_run_sector << fsinfo->sector_size_bits;
                        buffer = vsf_local.buff + vsf_local.cur_size;
                    }
                    vsf_eda_frame_user_value_set(READ_STATE_READ_DONE);
                    __vk_malfs_read(malfs_info, vsf_local.cur_sector, vsf_local.cur_run_sector, buffer);
                    break;
                }

                vsf_eda_return(vsf_local.cur_size);
                break;
            case READ_STATE_READ_DONE:
                if (NULL == result.buffer) {
                    vsf_eda_return(VSF_ERR_FAIL);
                    break;
                }

                if (vsf_local.cur_offset != vsf_local.offset) {
                    uint8_t *src = result.buffer;
                    src += vsf_local.offset - vsf_local.cur_offset;
                    memcpy(vsf_local.buff, src, vsf_local.cur_run_size);
                    vsf_local.cur_offset += vsf_local.cur_run_size;
                } else if (vsf_local.size < (1 << fsinfo->sector_size_bits)) {
                    uint8_t *dst = vsf_local.buff + vsf_local.cur_size;
                    memcpy(dst, result.buffer, vsf_local.cur_run_size);
                }
                vsf_local.cur_size += vsf_local.cur_run_size;
                vsf_local.offset += vsf_local.cur_run_size;
                vsf_local.size -= vsf_local.cur_run_size;

                // get next cluster if necessary
                if (vsf_local.size && !(vsf_local.cur_sector & ((1 << fsinfo->cluster_size_bits) - 1))) {
                    vsf_err_t err;
                    vsf_eda_frame_user_value_set(READ_STATE_GET_NEXT_FAT_ENTRY_DONE);
                    __vsf_component_call_peda(__vk_fatfs_get_fat_entry, err, fsinfo,
                        .cluster = vsf_local.cur_cluster,
                        .entry = &vsf_local.cur_cluster,
                    );
                    UNUSED_PARAM(err);
                    break;
                }
                goto read_next;
            case READ_STATE_GET_NEXT_FAT_ENTRY_DONE:
                if (result.err < 0) {
                    vsf_eda_return(result.err);
                    break;
                }
                if (!__vk_fatfs_fat_entry_is_valid(fsinfo, vsf_local.cur_cluster)) {
                    vsf_eda_return(VSF_ERR_FAIL);
                    break;
                }

                // remove MSB 4-bit for 32-bit FAT entry
			    vsf_local.cur_cluster &= 0x0FFFFFFF;
                goto read_next;
            }
        }
    }
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_fatfs_write, vk_file_write)
{
    vsf_peda_begin();
    vsf_peda_end();
}

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#elif   __IS_COMPILER_LLVM__
#   pragma clang diagnostic pop
#endif

#endif
