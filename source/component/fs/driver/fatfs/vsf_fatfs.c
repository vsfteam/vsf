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

#if VSF_USE_FS == ENABLED && VSF_FS_USE_FATFS == ENABLED

#define __VSF_FS_CLASS_INHERIT__
#define __VSF_MALFS_CLASS_INHERIT__
#define __VSF_FATFS_CLASS_IMPLEMENT

// for ctype.h
#include "utilities/vsf_utilities.h"
#include "../../vsf_fs.h"

/*============================ MACROS ========================================*/

#ifndef VSF_FATFS_CFG_MAX_FILENAME
#   define VSF_FATFS_CFG_MAX_FILENAME   (31 * 13 + 1)
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

typedef struct vk_fatfs_rw_local {
    uint64_t offset;
    uint32_t cur_sector;
    uint32_t cur_size;
    uint32_t cur_run_size;
    uint32_t cur_run_sector;
    uint16_t offset_in_sector;
} vk_fatfs_rw_local;

typedef struct vk_fatfs_setsize_local {
    int32_t clusters;
    uint32_t cur_cluster;
    uint32_t next_cluster;
    vk_fatfs_file_pos_t orig_fatfs_pos;
    uint64_t orig_pos;
} vk_fatfs_setsize_local;

typedef struct vk_fatfs_lookup_local {
    uint32_t cur_sector;
    char *filename;
    uint32_t cur_offset_in_sector;
    vk_fatfs_dentry_parser_t dparser;
    struct {
        uint32_t sector;
        uint8_t entry_offset_in_sector;
        uint8_t entry_num;
    } dentry;
} vk_fatfs_lookup_local;

typedef struct vk_fatfs_setpos_local {
    uint64_t cur_offset;
    uint32_t cur_cluster;
    uint32_t cur_sector;
} vk_fatfs_setpos_local;

/*============================ PROTOTYPES ====================================*/

dcl_vsf_peda_methods(static, __vk_fatfs_mount)
dcl_vsf_peda_methods(static, __vk_fatfs_unmount)
dcl_vsf_peda_methods(static, __vk_fatfs_lookup)
dcl_vsf_peda_methods(static, __vk_fatfs_read)
dcl_vsf_peda_methods(static, __vk_fatfs_write)
dcl_vsf_peda_methods(static, __vk_fatfs_close)
dcl_vsf_peda_methods(static, __vk_fatfs_setpos)
dcl_vsf_peda_methods(static, __vk_fatfs_setsize)

/*============================ GLOBAL VARIABLES ==============================*/

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-function-type"
#endif

const vk_fs_op_t vk_fatfs_op = {
    .fn_mount               = (vsf_peda_evthandler_t)vsf_peda_func(__vk_fatfs_mount),
    .fn_unmount             = (vsf_peda_evthandler_t)vsf_peda_func(__vk_fatfs_unmount),
#if VSF_FS_CFG_USE_CACHE == ENABLED
    .fn_sync                = (vsf_peda_evthandler_t)vsf_peda_func(vk_fsop_succeed),
#endif
    .fn_rename              = (vsf_peda_evthandler_t)vsf_peda_func(vk_fsop_not_support),
    .fop                    = {
        .read_local_size    = sizeof(vk_fatfs_rw_local),
        .write_local_size   = sizeof(vk_fatfs_rw_local),
        .setpos_local_size  = sizeof(vk_fatfs_setpos_local),
        .setsize_local_size = sizeof(vk_fatfs_setsize_local),
        .fn_read            = (vsf_peda_evthandler_t)vsf_peda_func(__vk_fatfs_read),
        .fn_write           = (vsf_peda_evthandler_t)vsf_peda_func(__vk_fatfs_write),
        .fn_close           = (vsf_peda_evthandler_t)vsf_peda_func(__vk_fatfs_close),
        .fn_setsize         = (vsf_peda_evthandler_t)vsf_peda_func(__vk_fatfs_setsize),
        .fn_setpos          = (vsf_peda_evthandler_t)vsf_peda_func(__vk_fatfs_setpos),
    },
    .dop                    = {
        .lookup_local_size  = sizeof(vk_fatfs_lookup_local),
        .fn_lookup          = (vsf_peda_evthandler_t)vsf_peda_func(__vk_fatfs_lookup),
        .fn_create          = (vsf_peda_evthandler_t)vsf_peda_func(vk_fsop_not_support),
        .fn_unlink          = (vsf_peda_evthandler_t)vsf_peda_func(vk_fsop_not_support),
        .fn_chmod           = (vsf_peda_evthandler_t)vsf_peda_func(vk_fsop_not_support),
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
        info->sector_size_bits = vsf_msb32(sector_size);
        info->cluster_size_bits = vsf_msb32(dbr->bpb.SecPerClus);
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
    info->root.cur.cluster = info->root.first_cluster;

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
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
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
                        *ptr++ = '\0';
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
        vsf_eda_set_user_value(MOUNT_STATE_PARSE_DBR);
        __vk_malfs_read(malfs_info, 0, 1, NULL);
        break;
    case VSF_EVT_RETURN: {
            uint8_t *buff = (uint8_t *)vsf_eda_get_return_value();

            if (NULL == buff) {
                VSF_FS_ASSERT(false);
                vsf_eda_return(VSF_ERR_FAIL);
                return;
            }

            switch (vsf_eda_get_user_value()) {
            case MOUNT_STATE_PARSE_DBR:
                if (VSF_ERR_NONE != __vk_fatfs_parse_dbr(fsinfo, buff)) {
                return_fail:
                    VSF_FS_ASSERT(false);
                    vsf_eda_return(VSF_ERR_FAIL);
                    return;
                }
                vsf_eda_set_user_value(MOUNT_STATE_PARSE_ROOT);
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
        vsf_eda_set_user_value(LOOKUP_FAT_STATE_START);
        // fall through
    case VSF_EVT_RETURN:
        switch (vsf_eda_get_user_value()) {
        case LOOKUP_FAT_STATE_START:
        read_fat_sector:
            if (vsf_local.cur_fat_bit < fat_bit) {
                start_bit = fsinfo->fat_sector + (start_bit >> (fsinfo->sector_size_bits + 3));
                start_bit += vsf_local.cur_fat_bit ? 1 : 0;

                vsf_eda_set_user_value(LOOKUP_FAT_STATE_PARSE);
                __vk_malfs_read(malfs_info, start_bit, 1, NULL);
            } else {
                vsf_eda_return(VSF_ERR_NONE);
                break;
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

                vsf_local.cur_fat_bit += vsf_min(fat_bit, sector_bit - start_bit_sec);
                *vsf_local.entry = get_unaligned_le32(&buff[start_bit_sec >> 3]);
                *vsf_local.entry = (*vsf_local.entry >> (start_bit & 7)) & ((1ULL << vsf_local.cur_fat_bit) - 1);
                goto read_fat_sector;
            }
        }
    }
    vsf_peda_end();
}

__vsf_component_peda_private_entry(__vk_fatfs_set_fat_entry,,
    uint32_t cluster;
    uint32_t next_cluster;
    ,
    uint32_t cur_fat_idx;
    uint32_t cur_fat_bit;
    uint32_t cur_fat_sector_offset;
    uint32_t cur_fat_bit_offset_in_sector;
    uint32_t cur_next_cluster;
) {
    vsf_peda_begin();
    enum {
        APPEND_FAT_STATE_READ_FAT_DONE,
        APPEND_FAT_STATE_WRITE_FAT_DONE,
    };
    __vk_fatfs_info_t *fsinfo = (__vk_fatfs_info_t *)&vsf_this;
    __vk_malfs_info_t *malfs_info = &fsinfo->use_as____vk_malfs_info_t;
    uint_fast8_t fat_bit = __vk_fatfs_fat_bitsize[fsinfo->type];
    uint32_t sector_bit_size_bits = fsinfo->sector_size_bits + 3;
    uint_fast32_t sector_bit = 1 << sector_bit_size_bits;

    switch (evt) {
    case VSF_EVT_INIT:
        vsf_local.cur_fat_idx = 0;

    write_next_fat:
        if (vsf_local.cur_fat_idx >= fsinfo->fat_num) {
            vsf_eda_return(VSF_ERR_NONE);
            return;
        }

        uint64_t sector_bit_offset = vsf_local.cluster * fat_bit;
        vsf_local.cur_fat_sector_offset = (uint32_t)(sector_bit_offset >> sector_bit_size_bits);
        vsf_local.cur_fat_bit_offset_in_sector = sector_bit_offset & (sector_bit - 1);
        vsf_local.cur_fat_bit = 0;
        vsf_local.cur_next_cluster = vsf_local.next_cluster;

    read_next_fat_sector:
        vsf_eda_set_user_value(APPEND_FAT_STATE_READ_FAT_DONE);
        uint32_t cur_sector = fsinfo->fat_sector + vsf_local.cur_fat_idx * fsinfo->fat_size + vsf_local.cur_fat_sector_offset;
        __vk_malfs_read(malfs_info, cur_sector, 1, NULL);
        break;
    case VSF_EVT_RETURN: {
            union {
                uintptr_t value;
                vsf_err_t err;
                uint8_t *buffer;
            } result;
            result.value = vsf_eda_get_return_value();

            switch (vsf_eda_get_user_value()) {
            case APPEND_FAT_STATE_READ_FAT_DONE:
                if (NULL == result.buffer) {
                    goto fail;
                }

                uint32_t cur_bit_size;
                if (0 == vsf_local.cur_fat_bit) {
                    cur_bit_size = sector_bit - vsf_local.cur_fat_bit_offset_in_sector;
                    cur_bit_size = vsf_min(cur_bit_size, fat_bit);
                } else {
                    cur_bit_size = fat_bit - vsf_local.cur_fat_bit;
                }
                vsf_local.cur_fat_bit += cur_bit_size;

                uint_fast16_t u32off = vsf_local.cur_fat_bit_offset_in_sector >> 5;
                uint_fast16_t bitoff = vsf_local.cur_fat_bit_offset_in_sector & 0x1F, cur_bitsize;
                uint_fast32_t u32mask, u32value;
                while (cur_bit_size > 0) {
                    u32value = le32_to_cpup(&(((uint32_t *)result.buffer)[u32off]));
                    cur_bitsize = 32 - bitoff;
                    cur_bitsize = vsf_min(cur_bitsize, cur_bit_size);
                    u32mask = ((1ULL << cur_bitsize) - 1) << bitoff;
                    u32value &= ~u32mask;
                    u32value |= vsf_local.cur_next_cluster & ((1ULL << cur_bitsize) - 1);
                    ((uint32_t *)result.buffer)[u32off] = cpu_to_le32(u32value);

                    vsf_local.cur_next_cluster >>= cur_bitsize;
                    cur_bit_size -= cur_bitsize;
                }

                vsf_eda_set_user_value(APPEND_FAT_STATE_WRITE_FAT_DONE);
                uint32_t cur_sector = fsinfo->fat_sector + vsf_local.cur_fat_idx * fsinfo->fat_size + vsf_local.cur_fat_sector_offset;
                __vk_malfs_write(malfs_info, cur_sector, 1, result.buffer);
                break;
            case APPEND_FAT_STATE_WRITE_FAT_DONE:
                if (vsf_local.cur_fat_bit < fat_bit) {
                    vsf_local.cur_fat_sector_offset++;
                    vsf_local.cur_fat_bit_offset_in_sector = 0;
                    goto read_next_fat_sector;
                }

                vsf_local.cur_fat_idx++;
                goto write_next_fat;
            }
        }
    }
    return;
fail:
    vsf_eda_return(VSF_ERR_FAIL);
    vsf_peda_end();
}

__vsf_component_peda_private_entry(__vk_fatfs_append_fat_entry,,
    uint32_t cluster;
    uint32_t *entry;
    ,
    uint32_t cur_fat_bit;
    uint32_t cur_fat_sector;
    uint32_t cur_cluster;
    uint32_t entry_tmp;
) {
    vsf_peda_begin();
    enum {
        APPEND_FAT_STATE_READ_FAT_DONE,
        APPEND_FAT_STATE_WRITE_EOF_FAT_DONE,
        APPEND_FAT_STATE_WRITE_PREV_FAT_DONE,
    };
    __vk_fatfs_info_t *fsinfo = (__vk_fatfs_info_t *)&vsf_this;
    __vk_malfs_info_t *malfs_info = &fsinfo->use_as____vk_malfs_info_t;
    uint_fast8_t fat_bit = __vk_fatfs_fat_bitsize[fsinfo->type];
    uint_fast32_t sector_bit = 1 << (fsinfo->sector_size_bits + 3);

    switch (evt) {
    case VSF_EVT_INIT:
        vsf_local.entry_tmp = 0;
        vsf_local.cur_cluster = 0;
        vsf_local.cur_fat_bit = 0;
        vsf_local.cur_fat_sector = fsinfo->fat_sector;

        vsf_eda_set_user_value(APPEND_FAT_STATE_READ_FAT_DONE);
    read_next_fat_sector:
        __vk_malfs_read(malfs_info, vsf_local.cur_fat_sector, 1, NULL);
        break;
    case VSF_EVT_RETURN: {
            union {
                uintptr_t value;
                vsf_err_t err;
                uint8_t *buffer;
            } result;
            result.value = vsf_eda_get_return_value();

            switch (vsf_eda_get_user_value()) {
            case APPEND_FAT_STATE_READ_FAT_DONE: {
                    uint32_t pos = 0, cur_bit_size;

                    if (NULL == result.buffer) {
                        goto fail;
                    }

                    while (pos < sector_bit) {
                        cur_bit_size = sector_bit - pos;
                        cur_bit_size = vsf_min(cur_bit_size, fat_bit);

                        uint_fast16_t bit_size_tmp = cur_bit_size;
                        uint_fast16_t u32off = pos >> 5;
                        uint_fast16_t bitoff = pos & 0x1F, cur_bitsize;
                        uint_fast32_t u32mask, u32value = 0;
                        while (bit_size_tmp > 0) {
                            u32value = le32_to_cpup(&(((uint32_t *)result.buffer)[u32off]));
                            cur_bitsize = 32 - bitoff;
                            cur_bitsize = vsf_min(cur_bitsize, bit_size_tmp);
                            u32mask = ((1ULL << cur_bitsize) - 1) << bitoff;
                            u32value &= u32mask;

                            bit_size_tmp -= cur_bitsize;
                        }
                        vsf_local.entry_tmp += u32value << vsf_local.cur_fat_bit;

                        vsf_local.cur_fat_bit += cur_bit_size;
                        if (vsf_local.cur_fat_bit == fat_bit) {
                            if (0 == vsf_local.entry_tmp) {
                                vsf_err_t err;
                                *vsf_local.entry = vsf_local.cur_cluster;
                                vsf_eda_set_user_value(APPEND_FAT_STATE_WRITE_EOF_FAT_DONE);
                                __vsf_component_call_peda(__vk_fatfs_set_fat_entry, err, fsinfo,
                                    .cluster = vsf_local.cur_cluster,
                                    .next_cluster = 0x0FFFFFFF,
                                );
                                if (err != VSF_ERR_NONE) {
                                    goto fail;
                                }
                                return;
                            }
                            vsf_local.cur_fat_bit = 0;
                            vsf_local.entry_tmp = 0;
                            pos += fat_bit;
                            vsf_local.cur_cluster++;
                        }
                    }

                    if (++vsf_local.cur_fat_sector >= fsinfo->fat_size) {
                        vsf_eda_return(VSF_ERR_FAIL);
                        return;
                    }
                    goto read_next_fat_sector;
                }
                break;
            case APPEND_FAT_STATE_WRITE_EOF_FAT_DONE:
                if (result.err != VSF_ERR_NONE) {
                    goto fail;
                }

                if (0 == vsf_local.cluster) {
                    goto succeed;
                }

                vsf_err_t err;
                vsf_eda_set_user_value(APPEND_FAT_STATE_WRITE_PREV_FAT_DONE);
                __vsf_component_call_peda(__vk_fatfs_set_fat_entry, err, fsinfo,
                    .cluster = vsf_local.cluster,
                    .next_cluster = vsf_local.cur_cluster,
                );
                if (err != VSF_ERR_NONE) {
                    goto fail;
                }
                break;
            case APPEND_FAT_STATE_WRITE_PREV_FAT_DONE:
                if (result.err != VSF_ERR_NONE) {
                fail:
                    vsf_eda_return(VSF_ERR_FAIL);
                    return;
                }
            succeed:
                vsf_eda_return(VSF_ERR_NONE);
                break;
            }
        }
    }
    vsf_peda_end();
}

__vsf_component_peda_private_entry(__vk_fatfs_dentry_setsize,,
    uint64_t size;
) {
    vsf_peda_begin();
    enum {
        DENTRY_SETSIZE_STATE_READ,
        DENTRY_SETSIZE_STATE_WRITE,
    };
    vk_fatfs_file_t *file = (vk_fatfs_file_t *)&vsf_this;
    __vk_fatfs_info_t *fsinfo = (__vk_fatfs_info_t *)file->info;
    __vk_malfs_info_t *malfs_info = &fsinfo->use_as____vk_malfs_info_t;
    uint32_t sector;
    uint16_t offset;
    if (file->dentry.entry_num1) {
        sector = file->dentry.sector1;
        offset = file->dentry.entry_offset_in_sector1 << 5;
        offset += 0x20 * (file->dentry.entry_num1 - 1);
    } else {
        sector = file->dentry.sector0;
        offset = file->dentry.entry_offset_in_sector0 << 5;
        offset += 0x20 * (file->dentry.entry_num0 - 1);
    }

    switch (evt) {
    case VSF_EVT_INIT:
        vsf_eda_set_user_value(DENTRY_SETSIZE_STATE_READ);
        __vk_malfs_read(malfs_info, sector, 1, NULL);
        break;
    case VSF_EVT_RETURN: {
            union {
                uintptr_t value;
                vsf_err_t err;
                uint8_t *buffer;
            } result;

            result.value = vsf_eda_get_return_value();
            switch (vsf_eda_get_user_value()) {
            case DENTRY_SETSIZE_STATE_READ: {
                    fatfs_dentry_t *dentry = (fatfs_dentry_t *)(result.buffer + offset);
                    dentry->fat.FileSize = (uint32_t)vsf_local.size;
                    dentry->fat.FstClusHi = file->first_cluster >> 16;
                    dentry->fat.FstClusLo = file->first_cluster;
                    vsf_eda_set_user_value(DENTRY_SETSIZE_STATE_WRITE);
                    __vk_malfs_write(malfs_info, sector, 1, result.buffer);
                }
                break;
            case DENTRY_SETSIZE_STATE_WRITE:
                vsf_eda_return(VSF_ERR_NONE);
                break;
            }
        }
        break;
    }
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_fatfs_setpos, vk_file_setpos,
    implement(vk_fatfs_setpos_local)
) {
    vsf_peda_begin();
    vk_fatfs_file_t *file = (vk_fatfs_file_t *)&vsf_this;
    __vk_fatfs_info_t *fsinfo = (__vk_fatfs_info_t *)file->info;
    uint32_t clustersize = 1 << (fsinfo->cluster_size_bits + fsinfo->sector_size_bits);
    vsf_err_t err = VSF_ERR_NONE;

    switch (evt) {
    case VSF_EVT_INIT:
        vsf_local.cur_offset = vsf_local.offset;
        vsf_local.cur_cluster = file->first_cluster;
        if (!file->first_cluster) {
            if ((fsinfo->type != VSF_FAT_12) && (fsinfo->type != VSF_FAT_16)) {
                VSF_FS_ASSERT(false);
                err = VSF_ERR_FAIL;
                goto __fail_and_exit;
            }
            vsf_local.cur_sector = fsinfo->root_sector;
        } else {
            vsf_local.cur_sector = __vk_fatfs_clus2sec(fsinfo, vsf_local.cur_cluster);
        }

        // fall through
    case VSF_EVT_RETURN:
        if (vsf_local.cur_offset > clustersize) {
            vsf_local.cur_offset -= clustersize;

            __vsf_component_call_peda(__vk_fatfs_get_fat_entry, err, fsinfo,
                .cluster = vsf_local.cur_cluster,
                .entry = &vsf_local.cur_cluster,
            )
            if (VSF_ERR_NONE != err) {
                goto __fail_and_exit;
            }
        } else {
            file->cur.cluster = vsf_local.cur_cluster;
            file->cur.sector_offset_in_cluster = vsf_local.cur_offset / (1 << fsinfo->sector_size_bits);
            file->cur.offset_in_sector = vsf_local.cur_offset % (1 << fsinfo->sector_size_bits);
            VSF_FS_ASSERT(vsf_local.result != NULL);
            *vsf_local.result = vsf_local.offset;
            vsf_eda_return(VSF_ERR_NONE);
        }
    }

    return;
__fail_and_exit:
    vsf_eda_return(err);
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_fatfs_setsize, vk_file_setsize,
    implement(vk_fatfs_setsize_local)
) {
    vsf_peda_begin();
    enum {
        SETSIZE_STATE_APPEND_DONE,
        SETSIZE_STATE_APPEND,
        SETSIZE_STATE_FREE_PREPARE,
        SETSIZE_STATE_FREE_NEXT,
        SETSIZE_STATE_FREE,
        SETSIZE_STATE_UPDATE_DENTRY_DONE,
    };
    vk_fatfs_file_t *fatfs_file = (vk_fatfs_file_t *)&vsf_this;
    __vk_fatfs_info_t *fsinfo = (__vk_fatfs_info_t *)fatfs_file->info;
    uint32_t clustersize = 1 << (fsinfo->cluster_size_bits + fsinfo->sector_size_bits);
    uint64_t size = vsf_local.size;
    vsf_err_t err;
    VSF_FS_ASSERT(!(fatfs_file->attr & VSF_FILE_ATTR_DIRECTORY));

    switch (evt) {
    case VSF_EVT_INIT: {
            uint64_t round_size = (size + clustersize - 1) & ~(clustersize - 1);
            uint64_t round_file_size = (fatfs_file->size + clustersize - 1) & ~(clustersize - 1);
            int64_t pos;

            vsf_local.orig_pos = fatfs_file->pos;
            vsf_local.orig_fatfs_pos = fatfs_file->cur;
            if (round_size == round_file_size) {
                goto update_dentry;
            } else {
                if (round_size > round_file_size) {
                    vsf_local.clusters = (round_size - round_file_size) / clustersize;
                    vsf_eda_set_user_value(SETSIZE_STATE_APPEND);
                    pos = fatfs_file->size;
                } else {
                    if (0 == size) {
                        fatfs_file->first_cluster = 0;
                        vsf_local.next_cluster = 0;
                    } else {
                        vsf_local.next_cluster = 0x0FFFFFFF;
                    }
                    vsf_eda_set_user_value(SETSIZE_STATE_FREE_PREPARE);
                    pos = size;
                }

                err = vk_file_seek(&fatfs_file->use_as__vk_file_t, pos, VSF_FILE_SEEK_SET);
                if (err != VSF_ERR_NONE) {
                    goto fail;
                }
            }
        }
        break;
    case VSF_EVT_RETURN:
        switch (vsf_eda_get_user_value()) {
        case SETSIZE_STATE_APPEND_DONE:
            err = vsf_eda_get_return_value();
            if (err != VSF_ERR_NONE) {
                goto fail;
            }
            if (0 == fatfs_file->first_cluster) {
                fatfs_file->first_cluster = fatfs_file->cur.cluster;
                vsf_local.orig_fatfs_pos.cluster = fatfs_file->first_cluster;
                vsf_local.orig_fatfs_pos.sector_offset_in_cluster = 0;
                vsf_local.orig_fatfs_pos.offset_in_sector = 0;
            }
            // fall through
        case SETSIZE_STATE_APPEND:
            if (vsf_local.clusters > 0) {
                vsf_local.clusters--;
                vsf_eda_set_user_value(SETSIZE_STATE_APPEND_DONE);
                __vsf_component_call_peda(__vk_fatfs_append_fat_entry, err, fsinfo,
                    .cluster = fatfs_file->cur.cluster,
                    .entry = &fatfs_file->cur.cluster,
                );
                if (err != VSF_ERR_NONE) {
                    goto fail;
                }
            } else {
                goto update_dentry;
            }
            break;
        case SETSIZE_STATE_FREE: {
                uint32_t cur_cluster = vsf_local.cur_cluster;
                uint32_t next_cluster = vsf_local.next_cluster;
                vsf_local.cur_cluster = fatfs_file->cur.cluster;
                vsf_local.next_cluster = 0;

                vsf_eda_set_user_value(SETSIZE_STATE_FREE_NEXT);
                __vsf_component_call_peda(__vk_fatfs_set_fat_entry, err, fsinfo,
                    .cluster = cur_cluster,
                    .next_cluster = next_cluster,
                );
                if (err != VSF_ERR_NONE) {
                    goto fail;
                }
            }
            break;
        case SETSIZE_STATE_FREE_PREPARE:
            if (vsf_local.orig_pos > size) {
                vsf_local.orig_pos = fatfs_file->pos;
                vsf_local.orig_fatfs_pos = fatfs_file->cur;
            }
            vsf_local.cur_cluster = fatfs_file->cur.cluster;
        case SETSIZE_STATE_FREE_NEXT:
            if (    __vk_fatfs_fat_entry_is_valid(fsinfo, vsf_local.cur_cluster)
                &&  !__vk_fatfs_fat_entry_is_eof(fsinfo, vsf_local.cur_cluster)) {
                vsf_eda_set_user_value(SETSIZE_STATE_FREE);
                __vsf_component_call_peda(__vk_fatfs_get_fat_entry, err, fsinfo,
                    .cluster = vsf_local.cur_cluster,
                    .entry = &fatfs_file->cur.cluster,
                );
                if (err != VSF_ERR_NONE) {
                fail:
                    vsf_eda_return(VSF_ERR_FAIL);
                    return;
                }
            } else {
            update_dentry:
                vsf_eda_set_user_value(SETSIZE_STATE_UPDATE_DENTRY_DONE);
                __vsf_component_call_peda(__vk_fatfs_dentry_setsize, err, fatfs_file,
                    .size   = vsf_local.size,
                );
            }
            break;
        case SETSIZE_STATE_UPDATE_DENTRY_DONE:
            fatfs_file->pos = vsf_local.orig_pos;
            fatfs_file->cur = vsf_local.orig_fatfs_pos;
            vsf_eda_return(VSF_ERR_NONE);
            break;
        }
        break;
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
            goto __fail_and_exit;
        }
        if (!dir->cur.cluster) {
            if ((fsinfo->type != VSF_FAT_12) && (fsinfo->type != VSF_FAT_16)) {
                VSF_FS_ASSERT(false);
                err = VSF_ERR_FAIL;
                goto __fail_and_exit;
            }
            vsf_local.cur_sector = fsinfo->root_sector;
        } else {
            vsf_local.cur_sector = __vk_fatfs_clus2sec(fsinfo, dir->cur.cluster);
        }
        vsf_local.cur_sector += dir->cur.sector_offset_in_cluster;
        vsf_local.dparser.lfn = 0;
        vsf_eda_set_user_value(LOOKUP_STATE_READ_SECTOR);

        // fall through
    case VSF_EVT_RETURN:
        switch (vsf_eda_get_user_value()) {
        case LOOKUP_STATE_READ_SECTOR:
        read_fat_sector:
            vsf_eda_set_user_value(LOOKUP_STATE_PARSE_SECTOR);
            __vk_malfs_read(malfs_info, vsf_local.cur_sector, 1, NULL);
            break;
        case LOOKUP_STATE_PARSE_SECTOR: {
                fatfs_dentry_t *dentry = (fatfs_dentry_t *)vsf_eda_get_return_value();
                vk_fatfs_dentry_parser_t *dparser = &vsf_local.dparser;

                if (NULL == dentry) {
                    goto __fail_and_exit;
                }

                if (fsinfo->type == VSF_FAT_EX) {
                    err = VSF_ERR_NOT_SUPPORT;
                    goto __fail_and_exit;
                }

                dparser->entry = (uint8_t *)dentry + dir->cur.offset_in_sector;
                dparser->entry_num = 1 << (fsinfo->sector_size_bits - 5);
                dparser->entry_num -= dir->cur.offset_in_sector >> 5;
                dparser->filename = vsf_local.filename;
                while (dparser->entry_num) {
                    uint16_t entry_num = dparser->entry_num;
                    bool parsed = vk_fatfs_parse_dentry_fat(dparser);
                    uint32_t parsed_entry = entry_num - dparser->entry_num;
                    uint32_t parsed_size = parsed_entry << 5;
                    dir->cur.offset_in_sector += parsed_size;
                    dir->pos += parsed_size;

                    if (parsed) {
                        if (!name || vk_file_is_match((char *)name, dparser->filename)) {
                            vk_fatfs_file_t *fatfs_file = (vk_fatfs_file_t *)vk_file_alloc(sizeof(vk_fatfs_file_t));
                            if (NULL == fatfs_file) {
                            fail_mem:
                                err = VSF_ERR_NOT_ENOUGH_RESOURCES;
                                goto __fail_and_exit;
                            }

                            uint_fast16_t filename_len;
                            if (dparser->is_unicode) {
                                uint16_t *uchar = (uint16_t *)dparser->filename;
                                filename_len = 0;
                                while (*uchar++ != 0) {
                                    filename_len += 2;
                                }
                                filename_len += 2;
                                // TODO: convert to utf8 length
                                VSF_FS_ASSERT(false);
                            } else {
                                filename_len = strlen(dparser->filename) + 1;
                            }

                            fatfs_file->name = vsf_heap_malloc(filename_len);
                            if (NULL == fatfs_file->name) {
                                vk_file_free(&fatfs_file->use_as__vk_file_t);
                                goto fail_mem;
                            }
                            // TODO: convert to utf8
                            memcpy(fatfs_file->name, dparser->filename, filename_len);

                            dentry = (fatfs_dentry_t *)dparser->entry;
                            fatfs_file->attr |= (vk_file_attr_t)__vk_fatfs_parse_file_attr(dentry->fat.Attr);
                            fatfs_file->fsop = &vk_fatfs_op;
                            fatfs_file->size = dentry->fat.FileSize;
                            fatfs_file->info = fsinfo;
                            fatfs_file->first_cluster = dentry->fat.FstClusLo + (dentry->fat.FstClusHi << 16);
                            fatfs_file->cur.cluster = fatfs_file->first_cluster;
                            fatfs_file->cur.sector_offset_in_cluster = 0;
                            fatfs_file->cur.offset_in_sector = 0;

                            if (vsf_local.dentry.entry_num) {
                                fatfs_file->dentry.sector0 = vsf_local.dentry.sector;
                                fatfs_file->dentry.entry_offset_in_sector0 = vsf_local.dentry.entry_offset_in_sector;
                                fatfs_file->dentry.entry_num0 = vsf_local.dentry.entry_num;
                                fatfs_file->dentry.sector1 = vsf_local.cur_sector;
                                fatfs_file->dentry.entry_offset_in_sector1 = (1 << (fsinfo->sector_size_bits - 5)) - entry_num;
                                fatfs_file->dentry.entry_num1 = parsed_entry;
                            } else {
                                fatfs_file->dentry.sector0 = vsf_local.cur_sector;
                                fatfs_file->dentry.entry_offset_in_sector0 = (1 << (fsinfo->sector_size_bits - 5)) - entry_num;
                                fatfs_file->dentry.entry_num0 = parsed_entry;
                            }

                            *vsf_local.result = &fatfs_file->use_as__vk_file_t;
                            goto __fail_and_exit;
                        } else {
                            vsf_local.dentry.entry_num = 0;
                        }
                        dparser->entry += 32;
                    } else if (dparser->entry_num > 0) {
                        goto __not_available;
                    } else {
                        vsf_local.dentry.sector = vsf_local.cur_sector;
                        vsf_local.dentry.entry_offset_in_sector = entry_num;
                        vsf_local.dentry.entry_num = parsed_entry;
                        break;
                    }
                }

                if (    (!dir->cur.cluster && (vsf_local.cur_sector < fsinfo->root_size))
                    ||  (dir->cur.sector_offset_in_cluster < ((1 << fsinfo->cluster_size_bits) - 1))) {
                    // not found in current sector, find next sector
                    vsf_local.cur_sector++;
                    dir->cur.sector_offset_in_cluster++;
                    dir->cur.offset_in_sector = 0;
                    goto read_fat_sector;
                } else {
                    vsf_err_t err;
                    // not found in current cluster, find next cluster if exists
                    vsf_eda_set_user_value(LOOKUP_STATE_READ_FAT);
                    __vsf_component_call_peda(__vk_fatfs_get_fat_entry, err, fsinfo,
                        .cluster = dir->cur.cluster,
                        .entry = &dir->cur.cluster,
                    )
                    VSF_UNUSED_PARAM(err);
                }
            }
            break;
        case LOOKUP_STATE_READ_FAT:
            if (    ((vsf_err_t)vsf_eda_get_return_value() != VSF_ERR_NONE)
                ||  !__vk_fatfs_fat_entry_is_valid(fsinfo, dir->cur.cluster)
                ||  __vk_fatfs_fat_entry_is_eof(fsinfo, dir->cur.cluster)) {
            __not_available:
                dir->cur.cluster = dir->first_cluster;
                dir->cur.sector_offset_in_cluster = 0;
                dir->cur.offset_in_sector = 0;
                dir->pos = 0;
                err = VSF_ERR_NOT_AVAILABLE;
                goto __fail_and_exit;
            }

            // remove MSB 4-bit for 32-bit FAT entry
            dir->cur.cluster &= 0x0FFFFFFF;
            vsf_local.cur_sector = __vk_fatfs_clus2sec(fsinfo, dir->cur.cluster);
            dir->cur.sector_offset_in_cluster = 0;
            dir->cur.offset_in_sector = 0;
            goto read_fat_sector;
        }
        break;
    }
    return;
__fail_and_exit:
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
    vsf_eda_return(VSF_ERR_NONE);
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_fatfs_read, vk_file_read,
    implement(vk_fatfs_rw_local)
) {
    vsf_peda_begin();
    enum {
        READ_STATE_READ,
        READ_STATE_READ_DONE,
        READ_STATE_GET_NEXT_FAT_ENTRY_DONE,
    };

    vk_fatfs_file_t *fatfs_file = (vk_fatfs_file_t *)&vsf_this;
    __vk_fatfs_info_t *fsinfo = (__vk_fatfs_info_t *)fatfs_file->info;
    __vk_malfs_info_t *malfs_info = &fsinfo->use_as____vk_malfs_info_t;

    switch (evt) {
    case VSF_EVT_INIT:
        if (vsf_local.size > (fatfs_file->size - fatfs_file->pos)) {
            vsf_local.size = fatfs_file->size - fatfs_file->pos;
        }
        vsf_local.offset = fatfs_file->pos;
        vsf_local.cur_size = 0;
        vsf_local.cur_sector = __vk_fatfs_clus2sec(fsinfo, fatfs_file->cur.cluster);
        vsf_local.cur_sector += fatfs_file->cur.sector_offset_in_cluster;
        vsf_local.offset_in_sector = vsf_local.offset & ((1 << fsinfo->sector_size_bits) - 1);
        vsf_eda_set_user_value(READ_STATE_READ);
        // fall through
    case VSF_EVT_RETURN: {
            union {
                uintptr_t value;
                vsf_err_t err;
                int_fast32_t size;
                uint8_t *buffer;
            } result;

            result.value = vsf_eda_get_return_value();
            switch (vsf_eda_get_user_value()) {
            case READ_STATE_READ:
            read_next:
                if (vsf_local.size) {
                    uint8_t *buffer;

                    if (vsf_local.offset_in_sector != 0) {
                        // read first non-page-aligned data
                        vsf_local.cur_run_size = (1 << fsinfo->sector_size_bits) - vsf_local.offset_in_sector;
                        vsf_local.cur_run_size = vsf_min(vsf_local.cur_run_size, vsf_local.size);
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
                        vsf_local.cur_run_sector = (1 << fsinfo->cluster_size_bits) -
                            ((vsf_local.cur_sector - fsinfo->data_sector) & ((1 << fsinfo->cluster_size_bits) - 1));
                        vsf_local.cur_run_sector = vsf_min(vsf_local.cur_run_sector,
                            vsf_local.size >> fsinfo->sector_size_bits);
                        vsf_local.cur_run_size = vsf_local.cur_run_sector << fsinfo->sector_size_bits;
                        buffer = vsf_local.buff + vsf_local.cur_size;
                    }
                    vsf_eda_set_user_value(READ_STATE_READ_DONE);
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

                if (vsf_local.offset_in_sector != 0) {
                    uint8_t *src = result.buffer + vsf_local.offset_in_sector;
                    memcpy(vsf_local.buff, src, vsf_local.cur_run_size);
                    vsf_local.offset_in_sector = 0;
                } else if (vsf_local.size < (1 << fsinfo->sector_size_bits)) {
                    uint8_t *dst = vsf_local.buff + vsf_local.cur_size;
                    memcpy(dst, result.buffer, vsf_local.cur_run_size);
                }
                vsf_local.cur_size += vsf_local.cur_run_size;
                vsf_local.offset += vsf_local.cur_run_size;
                vsf_local.size -= vsf_local.cur_run_size;

                fatfs_file->cur.offset_in_sector += vsf_local.cur_run_size;
                if (fatfs_file->cur.offset_in_sector == (1 << fsinfo->sector_size_bits)) {
                    fatfs_file->cur.offset_in_sector = 0;
                    fatfs_file->cur.sector_offset_in_cluster++;
                }

                // get next cluster if necessary
                // check if next sector will be in new cluster
                if (!(      (vsf_local.cur_sector + vsf_local.cur_run_sector - fsinfo->data_sector)
                        &   ((1 << fsinfo->cluster_size_bits) - 1)
                )) {
                    vsf_err_t err;
                    vsf_eda_set_user_value(READ_STATE_GET_NEXT_FAT_ENTRY_DONE);
                    __vsf_component_call_peda(__vk_fatfs_get_fat_entry, err, fsinfo,
                        .cluster = fatfs_file->cur.cluster,
                        .entry = &fatfs_file->cur.cluster,
                    );
                    VSF_UNUSED_PARAM(err);
                    break;
                }
                vsf_local.cur_sector += vsf_local.cur_run_sector;
                goto read_next;
            case READ_STATE_GET_NEXT_FAT_ENTRY_DONE:
                if (result.err < 0) {
                    vsf_eda_return(result.err);
                    break;
                }
                if (!__vk_fatfs_fat_entry_is_valid(fsinfo, fatfs_file->cur.cluster)) {
                    vsf_eda_return(VSF_ERR_FAIL);
                    break;
                }

                // remove MSB 4-bit for 32-bit FAT entry
                fatfs_file->cur.cluster &= 0x0FFFFFFF;
                fatfs_file->cur.sector_offset_in_cluster = 0;
                vsf_local.cur_sector = __vk_fatfs_clus2sec(fsinfo, fatfs_file->cur.cluster);
                goto read_next;
            }
        }
    }
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_fatfs_write, vk_file_write,
    implement(vk_fatfs_rw_local)
) {
    vsf_peda_begin();
    enum {
        WRITE_STATE_READ_DONE,
        WRITE_STATE_ALLOC_FIRST_CLUSTER,
        WRITE_STATE_WRITE,
        WRITE_STATE_WRITE_DONE,
        WRITE_STATE_GET_NEXT_FAT_ENTRY_DONE,
        WRITE_STATE_DENTRY_SETSIZE_DONE,
    };

    vk_fatfs_file_t *fatfs_file = (vk_fatfs_file_t *)&vsf_this;
    __vk_fatfs_info_t *fsinfo = (__vk_fatfs_info_t *)fatfs_file->info;
    __vk_malfs_info_t *malfs_info = &fsinfo->use_as____vk_malfs_info_t;

    switch (evt) {
    case VSF_EVT_INIT:
        vsf_local.offset = fatfs_file->pos;
        vsf_local.cur_size = 0;
        vsf_local.cur_sector = __vk_fatfs_clus2sec(fsinfo, fatfs_file->cur.cluster);
        vsf_local.cur_sector += fatfs_file->cur.sector_offset_in_cluster;
        vsf_local.offset_in_sector = vsf_local.offset & ((1 << fsinfo->sector_size_bits) - 1);

        if (0 == fatfs_file->size) {
            uint32_t clustersize = 1 << (fsinfo->cluster_size_bits + fsinfo->sector_size_bits);
            uint32_t size = vsf_min(vsf_local.size, clustersize);
            vsf_eda_set_user_value(WRITE_STATE_ALLOC_FIRST_CLUSTER);
            vk_file_setsize(&fatfs_file->use_as__vk_file_t, size);
            break;
        } else {
            vsf_eda_set_user_value(WRITE_STATE_WRITE);
        }
        // fall through
    case VSF_EVT_RETURN: {
            union {
                uintptr_t value;
                vsf_err_t err;
                int_fast32_t size;
                uint8_t *buffer;
            } result;

            result.value = vsf_eda_get_return_value();
            switch (vsf_eda_get_user_value()) {
            case WRITE_STATE_READ_DONE:
                if (NULL == result.buffer) {
                    vsf_eda_return(VSF_ERR_FAIL);
                    break;
                }
                if (vsf_local.offset_in_sector != 0) {
                    uint8_t *dst = result.buffer + vsf_local.offset_in_sector;
                    memcpy(dst, vsf_local.buff, vsf_local.cur_run_size);
                    vsf_local.offset_in_sector = 0;
                } else if (vsf_local.size < (1 << fsinfo->sector_size_bits)) {
                    uint8_t *src = vsf_local.buff + vsf_local.cur_size;
                    memcpy(result.buffer, src, vsf_local.cur_run_size);
                }
                goto write_page;
            case WRITE_STATE_ALLOC_FIRST_CLUSTER:
                vsf_local.cur_sector = __vk_fatfs_clus2sec(fsinfo, fatfs_file->cur.cluster);
                vsf_local.cur_sector += fatfs_file->cur.sector_offset_in_cluster;
                // fall through
            case WRITE_STATE_WRITE:
            write_next:
                if (vsf_local.size) {
                    if (vsf_local.offset_in_sector != 0) {
                        // read first non-page-aligned data
                        vsf_local.cur_run_size = (1 << fsinfo->sector_size_bits) - vsf_local.offset_in_sector;
                        vsf_local.cur_run_size = vsf_min(vsf_local.cur_run_size, vsf_local.size);
                        vsf_local.cur_run_sector = 1;
                        goto read_page;
                    } else if (vsf_local.size < (1 << fsinfo->sector_size_bits)) {
                        // read last non-page-aligned data
                        vsf_local.cur_run_size = vsf_local.size;
                        vsf_local.cur_run_sector = 1;
                    read_page:
                        vsf_eda_set_user_value(WRITE_STATE_READ_DONE);
                        __vk_malfs_read(malfs_info, vsf_local.cur_sector, vsf_local.cur_run_sector, NULL);
                    } else {
                        // read page-aligned data in cluster
                        // get remain sector in clusrer
                        vsf_local.cur_run_sector = (1 << fsinfo->cluster_size_bits) -
                            ((vsf_local.cur_sector - fsinfo->data_sector) & ((1 << fsinfo->cluster_size_bits) - 1));
                        vsf_local.cur_run_sector = vsf_min(vsf_local.cur_run_sector,
                            vsf_local.size >> fsinfo->sector_size_bits);
                        vsf_local.cur_run_size = vsf_local.cur_run_sector << fsinfo->sector_size_bits;
                        result.buffer = vsf_local.buff + vsf_local.cur_size;
                    write_page:
                        vsf_eda_set_user_value(WRITE_STATE_WRITE_DONE);
                        __vk_malfs_write(malfs_info, vsf_local.cur_sector, vsf_local.cur_run_sector, result.buffer);
                    }
                    break;
                } else if (vsf_local.offset > fatfs_file->size) {
                    vsf_err_t err;
                    vsf_eda_set_user_value(WRITE_STATE_DENTRY_SETSIZE_DONE);
                    __vsf_component_call_peda(__vk_fatfs_dentry_setsize, err, fatfs_file,
                        .size   = vsf_local.offset,
                    );
                    VSF_UNUSED_PARAM(err);
                } else {
                    vsf_eda_return(vsf_local.cur_size);
                }
                break;
            case WRITE_STATE_WRITE_DONE:
                if (NULL == result.buffer) {
                    vsf_eda_return(VSF_ERR_FAIL);
                    break;
                }

                vsf_local.cur_size += vsf_local.cur_run_size;
                vsf_local.offset += vsf_local.cur_run_size;
                vsf_local.size -= vsf_local.cur_run_size;

                fatfs_file->cur.offset_in_sector += vsf_local.cur_run_size;
                if (fatfs_file->cur.offset_in_sector == (1 << fsinfo->sector_size_bits)) {
                    fatfs_file->cur.offset_in_sector = 0;
                    fatfs_file->cur.sector_offset_in_cluster++;
                }

                // get/append next cluster if necessary
                // check if next sector will be in new cluster
                if (!(      (vsf_local.cur_sector + vsf_local.cur_run_sector - fsinfo->data_sector)
                        &   ((1 << fsinfo->cluster_size_bits) - 1)
                )) {
                    vsf_err_t err;
                    vsf_eda_set_user_value(WRITE_STATE_GET_NEXT_FAT_ENTRY_DONE);
                    if (vsf_local.offset < fatfs_file->size) {
                        __vsf_component_call_peda(__vk_fatfs_get_fat_entry, err, fsinfo,
                            .cluster = fatfs_file->cur.cluster,
                            .entry = &fatfs_file->cur.cluster,
                        );
                    } else {
                        __vsf_component_call_peda(__vk_fatfs_append_fat_entry, err, fsinfo,
                            .cluster = fatfs_file->cur.cluster,
                            .entry = &fatfs_file->cur.cluster,
                        );
                    }
                    VSF_UNUSED_PARAM(err);
                    break;
                }
                vsf_local.cur_sector += vsf_local.cur_run_sector;
                goto write_next;
            case WRITE_STATE_GET_NEXT_FAT_ENTRY_DONE:
                if (result.err < 0) {
                    vsf_eda_return(result.err);
                    break;
                }
                if (!__vk_fatfs_fat_entry_is_valid(fsinfo, fatfs_file->cur.cluster)) {
                    vsf_trace_warning("fatfs: corrupted file %s" VSF_TRACE_CFG_LINEEND, fatfs_file->name);
                    break;
                }
                if (0 == fatfs_file->first_cluster) {
                    fatfs_file->first_cluster = fatfs_file->cur.cluster;
                    fatfs_file->cur.cluster = fatfs_file->first_cluster;
                    fatfs_file->cur.sector_offset_in_cluster = 0;
                    fatfs_file->cur.offset_in_sector = 0;
                }

                // remove MSB 4-bit for 32-bit FAT entry
                fatfs_file->cur.cluster &= 0x0FFFFFFF;
                fatfs_file->cur.sector_offset_in_cluster = 0;
                vsf_local.cur_sector = __vk_fatfs_clus2sec(fsinfo, fatfs_file->cur.cluster);
                goto write_next;
            case WRITE_STATE_DENTRY_SETSIZE_DONE:
                fatfs_file->size = vsf_local.offset;
                vsf_eda_return(vsf_local.cur_size);
                break;
            }
        }
    }
    vsf_peda_end();
}

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic pop
#endif

#endif
