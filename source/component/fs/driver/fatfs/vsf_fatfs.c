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

#ifdef __VSF64__
#   if defined(VSF_OS_CFG_EDA_FRAME_POOL_EXTRA_SIZE) && VSF_OS_CFG_EDA_FRAME_POOL_EXTRA_SIZE < 128
#       error VSF_OS_CFG_EDA_FRAME_POOL_EXTRA_SIZE MUST be >= 128
#   endif
#else
#   if defined(VSF_OS_CFG_EDA_FRAME_POOL_EXTRA_SIZE) && VSF_OS_CFG_EDA_FRAME_POOL_EXTRA_SIZE < 64
#       error VSF_OS_CFG_EDA_FRAME_POOL_EXTRA_SIZE MUST be >= 64
#   endif
#endif

#ifndef VSF_FATFS_CFG_MAX_FILENAME
#   define VSF_FATFS_CFG_MAX_FILENAME   (31 * 13 + 1)
#endif

#ifndef VSF_FATFS_CFG_DEFAULT_DATE
#   define VSF_FATFS_CFG_DEFAULT_DATE    ((0 << 9) | (1 << 5) | 1)
#endif

#define FAT_ATTR_LFN                    0x0F
#define FAT_ATTR_READ_ONLY              0x01
#define FAT_ATTR_HIDDEN                 0x02
#define FAT_ATTR_SYSTEM                 0x04
#define FAT_ATTR_VOLUME_ID              0x08
#define FAT_ATTR_DIRECTORY              0x10
#define FAT_ATTR_ARCHIVE                0x20

// exFAT Directory Entry Type

#define EXFAT_ET_BITMAP                 0x81
#define EXFAT_ET_UPCASE                 0x82
#define EXFAT_ET_VOLUME_LABEL           0x83
#define EXFAT_ET_FILE_DIR               0x85
#define EXFAT_ET_STREAM                 0xC0
#define EXFAT_ET_FILENAME               0xC1

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
} VSF_CAL_PACKED fatfs_bpb_t;

typedef struct fatfs_ebpb_t {
    uint8_t DrvNo;
    uint8_t Reserved;
    uint8_t BootSig;
    uint32_t VolID;
    uint8_t VolLab[11];
    uint8_t FilSysType[8];
} VSF_CAL_PACKED fatfs_ebpb_t;

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
            } VSF_CAL_PACKED bpb;
            fatfs_ebpb_t ebpb;
            uint8_t Bootstrap[420];
        } VSF_CAL_PACKED fat32;
        struct {
            fatfs_ebpb_t ebpb;
            uint8_t Bootstrap[448];
        } VSF_CAL_PACKED fat1216;
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
                } VSF_CAL_PACKED Ver;
                uint16_t VolState;
                uint8_t SecBits;
                uint8_t SPCBits;
                uint8_t NumFATs;
                uint8_t DrvNo;
                uint8_t AllocPercnet;
                uint8_t Reserved_All0[397];
            } VSF_CAL_PACKED bpb;
        } VSF_CAL_PACKED exfat;
    } VSF_CAL_PACKED;
    uint16_t Magic;
} VSF_CAL_PACKED fatfs_dbr_t;

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
        } VSF_CAL_PACKED fat;
        struct {
            uint8_t EntryType;
            union {
                struct {
                    uint8_t CharacterCount;
                    uint16_t VolumeLabel[11];
                } VSF_CAL_PACKED VolumeLabel;
                struct {
                    uint8_t BitmapFlags;
                    uint8_t Reserved[18];
                    uint32_t FirstCluster;
                    uint64_t DataLength;
                } VSF_CAL_PACKED Bitmap;
                struct {
                    uint8_t Reserved1[3];
                    uint32_t TableChecksum;
                    uint8_t Reserved[12];
                    uint32_t FirstCluster;
                    uint64_t DataLength;
                } VSF_CAL_PACKED UpCase;
                struct {
                    uint8_t SecondaryCount;
                    uint16_t SetChecksum;
                    uint16_t FileAttributes;
                    uint16_t Reserved1;
                    uint32_t CreateTimestamp;
                    uint32_t LastModifiedTimestamp;
                    uint32_t LastAccessedTimestamp;
                    uint8_t Create10msIncrement;
                    uint8_t LastModified10msIncrement;
                    uint8_t CreateUtcOffset;
                    uint8_t LastModifiedUtcOffset;
                    uint8_t LaseAccessedUtcOffset;
                    uint8_t Reserved2[7];
                } VSF_CAL_PACKED FilDir;
                struct {
                    uint8_t GeneralSecondaryFlags;
                    uint8_t Reserved1;
                    uint8_t NameLength;
                    uint16_t NameHash;
                    uint16_t Reserved2;
                    uint64_t ValidDataLength;
                    uint32_t Reserved3;
                    uint32_t FirstCluster;
                    uint64_t DataLength;
                } VSF_CAL_PACKED Stream;
                struct {
                    uint8_t GeneralSecondaryFlags;
                    uint16_t FileName[15];
                } VSF_CAL_PACKED FileName;
                uint8_t Buffer[31];
            } VSF_CAL_PACKED;
        } VSF_CAL_PACKED exfat;
    } VSF_CAL_PACKED;
} VSF_CAL_PACKED fatfs_dentry_t;

typedef struct vk_fatfs_rw_local {
    uint64_t offset;
    vk_fat_sector_type_t cur_sector;
    uint32_t cur_size;
    uint32_t cur_run_size;
    uint32_t cur_run_sector;
    uint32_t boundary_cluster;          // write: cluster before a boundary cursor, for append-after-EOC
    uint32_t run_last_cluster;          // read: cluster the prospective run currently ends in
    uint32_t peek_entry;                // read: FAT peek target for contiguous-run extension
    uint16_t offset_in_sector;
    uint8_t state;
} vk_fatfs_rw_local;

typedef struct vk_fatfs_setsize_local {
    int32_t clusters;
    uint32_t cur_cluster;
    uint32_t next_cluster;
#if VSF_FS_USE_EXFATFS == ENABLED
    uint32_t freed_cluster;
    vk_fat_sector_type_t cur_sector;
    bool free_bitmap;
#endif
    vk_fatfs_file_pos_t orig_fatfs_pos;
    uint64_t orig_pos;
    uint8_t state;
} vk_fatfs_setsize_local;

typedef struct vk_fatfs_lookup_local {
    vk_fat_sector_type_t cur_sector;
    vk_fatfs_file_pos_t fatfs_file_pos_save;
    vk_fatfs_dentry_parser_t *dparser;

    vk_fat_sector_type_t sector;
    uint16_t node_parsed_num;
    int16_t vital_entry_num;
#if VSF_FS_USE_EXFATFS == ENABLED
    int16_t root_entry_num;
#endif
    uint8_t entry_num;
    uint8_t state;
} vk_fatfs_lookup_local;

typedef struct vk_fatfs_setpos_local {
    uint64_t cur_offset;
    uint32_t cur_cluster;
    vk_fat_sector_type_t cur_sector;
    uint8_t *fat_buff;                // bulk walk: malfs-cached FAT sector
    uint32_t fat_buff_sector;
    uint32_t fetch_sector;
    uint8_t fat_pending_fetch;
} vk_fatfs_setpos_local;

// dop.fn_create/fn_unlink scratch areas. single-threaded like the rest of
// the malfs stack(vsf_malfs has no lock), callers serialize per volume
typedef struct vk_fatfs_create_local {
    // interface part: name/attr from vk_file_create
    vk_fat_sector_type_t cur_sector;
    vk_fat_sector_type_t slot_sector;
    vk_fat_sector_type_t zero_sector;
    uint32_t cur_cluster;
    uint32_t last_cluster;              // chain tail, where an extension links on
    uint32_t slot_cluster;
    uint32_t slot_sector_in_cluster;
    uint32_t sector_offset_in_cluster;  // scan cursor
    uint32_t zero_remain;
    uint16_t offset_in_sector;          // scan cursor
    uint16_t slot_offset;
    uint8_t *zero_buff;
    uint8_t sfn[11];
    uint8_t lcase;
    bool slot_found;
    bool scan_terminated;
#if VSF_FS_USE_EXFATFS == ENABLED
    // exfat entry-set writing
    uint16_t *utf16_name;               // owned, namelen chars, original case
    uint8_t *set_buff;                  // owned, set_entry_num * 32 bytes
    uint16_t set_off;                   // bytes already written from set_buff
    uint16_t run_len;                   // current consecutive free-entry run
    uint16_t name_hash;
    vk_file_t *result_file;             // duplicate-name probe via lookup
    uint8_t namelen;
    uint8_t set_entry_num;
#endif
    uint8_t state;
} vk_fatfs_create_local;

typedef struct vk_fatfs_unlink_local {
    // interface part: name from vk_file_unlink
    vk_file_t *file;
    vk_fat_sector_type_t cur_sector;
    uint32_t cur_cluster;
    uint32_t next_cluster;
    uint32_t file_cluster;
#if VSF_FS_USE_EXFATFS == ENABLED
    uint32_t free_cluster_num;
    bool no_fat_chain;
#endif
    uint32_t freed_cluster;             // exfat bitmap clear target
    uint16_t entry_offset;              // entry index in cur_sector
    uint16_t entry_num;                 // entries left to mark deleted
    uint8_t state;
} vk_fatfs_unlink_local;

#if defined(VSF_OS_CFG_EDA_FRAME_POOL_EXTRA_SIZE)
VSF_STATIC_ASSERT(
    (   ((sizeof(vsf_peda_local(vk_file_create)) + 7) & ~0x07)
        + sizeof(vk_fatfs_create_local))
        <= (VSF_OS_CFG_EDA_FRAME_POOL_EXTRA_SIZE - sizeof(uintalu_t)),
    fatfs_create_frame_too_large
);
#endif

/*============================ PROTOTYPES ====================================*/

static vsf_err_t __vk_fatfs_probe(uint8_t *sector0_buf, uint_fast32_t sector_size);
dcl_vsf_peda_methods(static, __vk_fatfs_mount)
dcl_vsf_peda_methods(static, __vk_fatfs_unmount)
dcl_vsf_peda_methods(static, __vk_fatfs_lookup)
dcl_vsf_peda_methods(static, __vk_fatfs_create)
dcl_vsf_peda_methods(static, __vk_fatfs_unlink)
dcl_vsf_peda_methods(static, __vk_fatfs_read)
dcl_vsf_peda_methods(static, __vk_fatfs_write)
dcl_vsf_peda_methods(static, __vk_fatfs_close)
dcl_vsf_peda_methods(static, __vk_fatfs_setpos)
dcl_vsf_peda_methods(static, __vk_fatfs_setsize)
dcl_vsf_peda_methods(static, __vk_fatfs_flush_fsinfo)

/*============================ GLOBAL VARIABLES ==============================*/

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-function-type"
#endif

const vk_fs_op_t vk_fatfs_op = {
    .fn_probe               = __vk_fatfs_probe,
    .mount_local_size       = 1,
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
        .create_local_size  = sizeof(vk_fatfs_create_local),
        .fn_create          = (vsf_peda_evthandler_t)vsf_peda_func(__vk_fatfs_create),
        .unlink_local_size  = sizeof(vk_fatfs_unlink_local),
        .fn_unlink          = (vsf_peda_evthandler_t)vsf_peda_func(__vk_fatfs_unlink),
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

#if VSF_FS_USE_EXFATFS == ENABLED
static uint16_t __vk_exfatfs_entry_checksum(uint16_t checksum, uint8_t *buffer, uint_fast16_t size, bool skip_checksum);

// utf8 name to utf16(exfat on-disk encoding). the ascii subset only - the
// full up-case table lives on the volume and is not loaded, so anything
// outside ascii is refused rather than hashed/stored wrongly. the stored
// copy keeps the original case, exfat is case-preserving
static int __vk_fatfs_name_to_utf16(const char *name, uint16_t *utf16)
{
    int len = 0;

    while (*name != '\0') {
        uint8_t ch = (uint8_t)*name++;
        if ((ch & 0x80) || (ch < 0x20) || strchr("\"*/:<>?\\|", ch)) {
            return -1;
        }
        if (len >= 255) {
            return -1;
        }
        utf16[len++] = ch;
    }
    return len > 0 ? len : -1;
}

// exfat NameHash: the entry-set checksum recurrence run over the UTF16LE
// bytes of the up-cased name
static uint16_t __vk_fatfs_exfat_name_hash(uint16_t *utf16, uint_fast16_t len)
{
    uint16_t hash = 0;

    for (uint_fast16_t i = 0; i < len; i++) {
        uint16_t ch = utf16[i];
        if ((ch >= 'a') && (ch <= 'z')) {
            ch -= 'a' - 'A';
        }
        uint8_t bytes[2] = { (uint8_t)ch, (uint8_t)(ch >> 8) };
        hash = __vk_exfatfs_entry_checksum(hash, bytes, 2, false);
    }
    return hash;
}
#endif

#if VSF_USE_HEAP == ENABLED
// build the on-disk 11-byte short name for a create request. the write side
// does not generate LFN entries, so the caller-visible name must round-trip
// exactly through the 8.3 parse side: refuse mixed case, out-of-8.3 shape
// and characters that are illegal in short names(no mangling - a mangled
// name would open under a different spelling than the one asked for)
static bool __vk_fatfs_name_to_sfn(const char *name, uint8_t sfn[11], uint8_t *lcase)
{
    static const char illegal[] = "<>:\"/\\|?*.";    // '.' included: the split
                                                    //  below owns the only dot
    const char *ext = vk_file_getfileext((char *)name);
    bool base_lower = false, base_upper = false, ext_lower = false, ext_upper = false;
    uint_fast32_t base_len = strlen(name), ext_len = 0;
    uint_fast8_t i;

    if ((0 == base_len) || ('.' == name[base_len - 1]) || (' ' == name[base_len - 1])) {
        return false;
    }
    if (ext != NULL) {
        ext_len = strlen(ext);
        base_len -= ext_len + 1;
    }
    if (!base_len || (base_len > 8) || (ext_len > 3)) {
        return false;
    }

    for (i = 0; i < base_len; i++) {
        char ch = name[i];
        if (islower((int)ch)) {
            base_lower = true;
        } else if (isupper((int)ch)) {
            base_upper = true;
        } else if ((ch <= ' ') || (ch >= 0x7F) || strchr(illegal, ch)) {
            return false;
        }
    }
    for (i = 0; i < ext_len; i++) {
        char ch = ext[i];
        if (islower((int)ch)) {
            ext_lower = true;
        } else if (isupper((int)ch)) {
            ext_upper = true;
        } else if ((ch <= ' ') || (ch >= 0x7F) || strchr(illegal, ch)) {
            return false;
        }
    }
    if ((base_lower && base_upper) || (ext_lower && ext_upper)) {
        // mixed case needs an LFN entry to survive a round-trip
        return false;
    }

    memset(sfn, ' ', 11);
    for (i = 0; i < base_len; i++) {
        sfn[i] = toupper((int)name[i]);
    }
    for (i = 0; i < ext_len; i++) {
        sfn[8 + i] = toupper((int)ext[i]);
    }
    // NTRes case bits, mirrored by the parse side(lowercase restore)
    *lcase = (base_lower ? 0x08 : 0) | (ext_lower ? 0x10 : 0);
    return true;
}
#endif

static vk_fat_sector_type_t __vk_fatfs_clus2sec(__vk_fatfs_info_t *fsinfo, uint_fast32_t cluster)
{
    cluster -= 2;
    return fsinfo->data_sector + (cluster << fsinfo->cluster_size_bits);
}

static bool __vk_fatfs_try_unicode2ascii(uint16_t *uchar)
{
    bool is_unicode = false;
    uint16_t *uchar_tmp = uchar;

    while (*uchar_tmp != 0) {
        if (*uchar_tmp++ >= 128) {
            is_unicode = true;
            break;
        }
    }
    if (!is_unicode) {
        char *ptr = (char *)uchar;
        uchar_tmp = (uint16_t *)uchar;
        while (*uchar_tmp != 0) {
            *ptr++ = *uchar_tmp++;
        }
        *ptr++ = '\0';
    }
    return is_unicode;
}

// convert nul-terminated utf16(exFAT/FAT-LFN, little-endian) to utf8.
// pass NULL out to just get the needed size(NUL included). invalid surrogate
// halves are emitted as U+FFFD
static uint_fast16_t __vk_fatfs_utf16_to_utf8(const uint16_t *utf16, char *out)
{
    uint_fast16_t len = 0;

    while (*utf16 != 0) {
        uint32_t code = *utf16++;
        uint_fast8_t size;

        if (    ((code >= 0xD800) && (code <= 0xDBFF))
            &&  ((utf16[0] >= 0xDC00) && (utf16[0] <= 0xDFFF))) {
            code = 0x10000 + ((code - 0xD800) << 10) + (uint32_t)(utf16[0] - 0xDC00);
            utf16++;
        } else if ((code >= 0xD800) && (code <= 0xDFFF)) {
            code = 0xFFFD;      // stray surrogate half
        }

        if (code < 0x80) {
            size = 1;
            if (out != NULL) {
                out[len] = code;
            }
        } else if (code < 0x800) {
            size = 2;
            if (out != NULL) {
                out[len] = 0xC0 | (code >> 6);
                out[len + 1] = 0x80 | (code & 0x3F);
            }
        } else if (code < 0x10000) {
            size = 3;
            if (out != NULL) {
                out[len] = 0xE0 | (code >> 12);
                out[len + 1] = 0x80 | ((code >> 6) & 0x3F);
                out[len + 2] = 0x80 | (code & 0x3F);
            }
        } else {
            size = 4;
            if (out != NULL) {
                out[len] = 0xF0 | (code >> 18);
                out[len + 1] = 0x80 | ((code >> 12) & 0x3F);
                out[len + 2] = 0x80 | ((code >> 6) & 0x3F);
                out[len + 3] = 0x80 | (code & 0x3F);
            }
        }
        len += size;
    }
    if (out != NULL) {
        out[len] = '\0';
    }
    return len + 1;
}

static vsf_err_t __vk_fatfs_parse_dbr(__vk_fatfs_info_t *info, uint8_t *buff)
{
    fatfs_dbr_t *dbr = (fatfs_dbr_t *)buff;
    uint_fast16_t sector_size;
    uint_fast32_t tmp32;

    if (dbr->Magic != cpu_to_be16(0x55AA)) {
        return VSF_ERR_FAIL;
    }

    buff += 3 + 8;      // skip jmp and oem
    for (tmp32 = 0; (tmp32 < 53) && !*buff; tmp32++, buff++);
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
            ||  !dbr->bpb.SecPerClus
            ||  !reserved_size
            ||  !info->fat_num
            ||  !sector_num
            ||  !info->fat_size) {
            return VSF_ERR_FAIL;
        }

        root_entry = le16_to_cpu(dbr->bpb.RootEntCnt);
        if (root_entry) {
            info->root_size = ((root_entry << 5) + sector_size - 1) / sector_size;
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
            // FSInfo location for the free-cluster-count cache; 0(absent)
            // leaves the count unknown and unmaintained
            info->fsinfo_sector = le16_to_cpu(dbr->fat32.bpb.FSInfo);
        } else {
            // FAT12 or FAT16
            info->type = (cluster_num < 4085) ? VSF_FAT_12 : VSF_FAT_16;

            // root has no cluster
            info->root.first_cluster = 0;
            info->cluster_num = cluster_num + 2;
        }
        info->root.cur.cluster = info->root.first_cluster;
#if VSF_FS_USE_EXFATFS == ENABLED
    } else if (!strncmp((char const *)dbr->oem, "EXFAT   ", sizeof(dbr->oem))) {
        // bpb all 0, exFAT
        info->type = VSF_FAT_EX;

        info->sector_size_bits = dbr->exfat.bpb.SecBits;
        info->cluster_size_bits = dbr->exfat.bpb.SPCBits;
        info->fat_num = dbr->exfat.bpb.NumFATs;
        info->fat_size = le32_to_cpu(dbr->exfat.bpb.FATSecCount);
        info->root_size = 0;
        info->fat_sector = le32_to_cpu(dbr->exfat.bpb.FATSecStart);
        info->data_sector = le32_to_cpu(dbr->exfat.bpb.ClusSecStart);
        info->cluster_num = le32_to_cpu(dbr->exfat.bpb.ClusSecCount) + 2;
        info->root.first_cluster = le32_to_cpu(dbr->exfat.bpb.RootClus);
        info->root.cur.cluster = info->root.first_cluster;
        info->root_sector = __vk_fatfs_clus2sec(info, info->root.first_cluster);

        // sector_size_bits size MUST be between [9, 12] which is [512, 4096] bytes
        // cluster_size_bits size MUST be between [1, 25]
        // RootClus CANNOT be less than 2
        if (    (info->sector_size_bits < 9) || (info->sector_size_bits > 12)
            ||  (info->cluster_size_bits < 1) || (info->cluster_size_bits > 25)
            ||  (info->root.first_cluster < 2)) {
            return VSF_ERR_FAIL;
        }
#endif
    } else {
        return VSF_ERR_FAIL;
    }

    return VSF_ERR_NONE;
}

static vsf_err_t __vk_fatfs_probe(uint8_t *sector0_buf, uint_fast32_t sector_size)
{
    __vk_fatfs_info_t info = {
        .block_size = sector_size,
    };
    return __vk_fatfs_parse_dbr(&info, sector0_buf);
}

static bool __vk_fatfs_fat_entry_is_valid(__vk_fatfs_info_t *fsinfo, uint_fast32_t cluster)
{
    uint_fast8_t fat_bit = __vk_fatfs_fat_bitsize[fsinfo->type];
    uint_fast32_t mask = ((1ULL << fat_bit) - 1) & 0x0FFFFFFF;

    cluster &= (32 == fat_bit) ? 0x0FFFFFFF : mask;
    return (cluster >= 2) && (cluster <= mask);
}

#if VSF_USE_HEAP == ENABLED
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
#endif

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

#if VSF_FS_USE_EXFATFS == ENABLED
bool vk_fatfs_parse_dentry_exfat(vk_fatfs_dentry_parser_t *parser)
{
    fatfs_dentry_t *entry = (fatfs_dentry_t *)parser->entry;
    bool parsed = false;

    parser->node_parsed_num = 0;
    parser->vital_entry_num = -1;
    parser->exfat.root_entry_num = -1;
    while (parser->entry_num-- > 0) {
        switch (entry->exfat.EntryType) {
        case 0:
            parser->entry_num++;
            return false;
        case EXFAT_ET_FILE_DIR:
            parser->exfat.root_entry_num = parser->entry_num + 1;
            parser->attr = le16_to_cpu(entry->exfat.FilDir.FileAttributes) & 0xFF;
            parser->exfat.namelen = 0;
            parser->exfat.entry_num = entry->exfat.FilDir.SecondaryCount + 1;
            break;
        case EXFAT_ET_STREAM:
            if (parser->exfat.entry_num > 0) {
                parser->vital_entry_num = parser->entry_num + 1;
                parser->first_cluster = le32_to_cpu(entry->exfat.Stream.FirstCluster);
                parser->size = le64_to_cpu(entry->exfat.Stream.ValidDataLength);
                parser->data_length = le64_to_cpu(entry->exfat.Stream.DataLength);
                parser->exfat.namelen = entry->exfat.Stream.NameLength;
                parser->exfat.namepos = 0;
                parser->exfat.stream_flags = entry->exfat.Stream.GeneralSecondaryFlags;
            }
            break;
        case EXFAT_ET_FILENAME:
            if ((parser->exfat.entry_num > 0) && (parser->exfat.namelen > 0)) {
                uint8_t copy_size = parser->exfat.namelen - parser->exfat.namepos;
                copy_size = vsf_min(copy_size, 15);
                memcpy(&parser->filename[parser->exfat.namepos << 1], entry->exfat.FileName.FileName, copy_size << 1);
                parser->exfat.namepos += copy_size;
                if (parser->exfat.namepos == parser->exfat.namelen) {
                    *(uint16_t *)&parser->filename[parser->exfat.namepos << 1] = 0;
                    parser->is_unicode = __vk_fatfs_try_unicode2ascii((uint16_t *)parser->filename);
                }
            }
            break;
        }

        if (parser->exfat.entry_num > 0) {
            parser->node_parsed_num++;
            parser->exfat.entry_num--;
            if (0 == parser->exfat.entry_num) {
                parsed = true;
                goto done;
            }
        }
        entry++;
    }

done:
    parser->entry = (uint8_t *)entry;
    return parsed;
}
#endif

// entry_num is the number of entries remained in buffer,
//     after return, entry_num will be counted down by number of parsed entries
// lfn is unicode encoded, but we just support ascii
// if a filename parsed, parser->entry will point to the sfn
bool vk_fatfs_parse_dentry_fat(vk_fatfs_dentry_parser_t *parser)
{
    fatfs_dentry_t *entry = (fatfs_dentry_t *)parser->entry;
    bool parsed = false;

    parser->node_parsed_num = 0;
    parser->vital_entry_num = -1;
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

                parser->node_parsed_num++;
                parser->fat.lfn = index & 0x0F;
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

                parser->node_parsed_num++;
                parser->vital_entry_num = parser->entry_num + 1;
                parser->attr = entry->fat.Attr;
                parser->size = le32_to_cpu(entry->fat.FileSize);
                parser->first_cluster = entry->fat.FstClusLo + (entry->fat.FstClusHi << 16);

                if (parser->fat.lfn == 1) {
                    parser->is_unicode = __vk_fatfs_try_unicode2ascii((uint16_t *)parser->filename);
                    parser->fat.lfn = 0;
                    parsed = true;
                    break;
                }

                parser->fat.lfn = 0;
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
        } else if (parser->fat.lfn > 0) {
            // an erased entry with previous parsed lfn entry?
            parser->fat.lfn = 0;
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

__vsf_component_peda_ifs_entry(__vk_fatfs_mount, vk_fs_mount,
    uint8_t state;
)
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
        vsf_local.state = MOUNT_STATE_PARSE_DBR;
        __vk_malfs_read(malfs_info, 0, 1, NULL);
        break;
    case VSF_EVT_RETURN: {
            uint8_t *buff = (uint8_t *)vsf_eda_get_return_value();
            if (NULL == buff) {
                vsf_eda_return(VSF_ERR_FAIL);
                return;
            }

            switch (vsf_local.state) {
            case MOUNT_STATE_PARSE_DBR:
                if (VSF_ERR_NONE != __vk_fatfs_parse_dbr(fsinfo, buff)) {
                return_fail:
                    vsf_eda_return(VSF_ERR_FAIL);
                    return;
                }
                vsf_local.state = MOUNT_STATE_PARSE_ROOT;
                __vk_malfs_read(malfs_info, fsinfo->root_sector, 1, NULL);
                break;
            case MOUNT_STATE_PARSE_ROOT: {
                    fatfs_dentry_t *dentry = (fatfs_dentry_t *)buff;
                    malfs_info->volume_name = NULL;
                    if (VSF_FAT_EX == fsinfo->type) {
#if VSF_FS_USE_EXFATFS == ENABLED
                        for (int i = 0; i < 3; i++, dentry++) {
                            switch (dentry->exfat.EntryType) {
                            case EXFAT_ET_VOLUME_LABEL:
                                if (dentry->exfat.VolumeLabel.CharacterCount > 11) {
                                    goto return_fail;
                                }
                                *(uint16_t *)&fsinfo->fat_volume_name[22] = 0;
                                memcpy(fsinfo->fat_volume_name, dentry->exfat.VolumeLabel.VolumeLabel,
                                        2 * dentry->exfat.VolumeLabel.CharacterCount);
                                __vk_fatfs_try_unicode2ascii((uint16_t *)fsinfo->fat_volume_name);
                                malfs_info->volume_name = fsinfo->fat_volume_name;
                                break;
                            case EXFAT_ET_BITMAP:
                                fsinfo->bitmap_sector = le64_to_cpu(dentry->exfat.Bitmap.FirstCluster);
                                fsinfo->bitmap_sector = __vk_fatfs_clus2sec(fsinfo, fsinfo->bitmap_sector);
                                fsinfo->bitmap_size = le32_to_cpu(dentry->exfat.Bitmap.DataLength);
                                break;
                            }
                        }
#else
                        goto return_fail;
#endif
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
    uint32_t entry_tmp;
    uint32_t cur_fat_bit;
    uint8_t state;
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
        vsf_local.state = LOOKUP_FAT_STATE_START;
        // fall through
    case VSF_EVT_RETURN:
        switch (vsf_local.state) {
        case LOOKUP_FAT_STATE_START:
        read_fat_sector:
            if (vsf_local.cur_fat_bit < fat_bit) {
                start_bit = fsinfo->fat_sector + (start_bit >> (fsinfo->sector_size_bits + 3));
                start_bit += vsf_local.cur_fat_bit ? 1 : 0;

                vsf_local.state = LOOKUP_FAT_STATE_PARSE;
                __vk_malfs_read(malfs_info, start_bit, 1, NULL);
            } else {
                vsf_eda_return(VSF_ERR_NONE);
                break;
            }
            break;
        case LOOKUP_FAT_STATE_PARSE: {
                uint8_t *buff = (uint8_t *)vsf_eda_get_return_value();
                if (NULL == buff) {
                    vsf_eda_return(VSF_ERR_FAIL);
                    break;
                }

                if (vsf_local.cur_fat_bit) {
                    vsf_local.entry_tmp |= get_unaligned_le32(buff) << vsf_local.cur_fat_bit;
                    vsf_local.entry_tmp &= (1 << fat_bit) - 1;
                    vsf_eda_return(VSF_ERR_NONE);
                    break;
                }

                vsf_local.cur_fat_bit += vsf_min(fat_bit, sector_bit - start_bit_sec);
                vsf_local.entry_tmp = get_unaligned_le32(&buff[start_bit_sec >> 3]);
                vsf_local.entry_tmp = (vsf_local.entry_tmp >> (start_bit & 7)) & ((1ULL << vsf_local.cur_fat_bit) - 1);
#if VSF_FS_USE_EXFATFS == ENABLED
                if (VSF_FAT_EX == fsinfo->type) {
                    if (!vsf_local.entry_tmp) {
                        (*vsf_local.entry)++;
                    } else {
                        *vsf_local.entry = vsf_local.entry_tmp;
                    }
                } else
#endif
                {
                    *vsf_local.entry = vsf_local.entry_tmp;
                }
                goto read_fat_sector;
            }
        }
    }
    vsf_peda_end();
}

// Mark the optional FAT32 free-cluster hint as unknown after mutations.
__vsf_component_peda_private_entry(__vk_fatfs_flush_fsinfo,,
    uint8_t state;
) {
    vsf_peda_begin();
    enum {
        FLUSH_FSINFO_STATE_READ,
        FLUSH_FSINFO_STATE_WRITE,
    };
    __vk_fatfs_info_t *fsinfo = (__vk_fatfs_info_t *)&vsf_this;
    __vk_malfs_info_t *malfs_info = &fsinfo->use_as____vk_malfs_info_t;

    switch (evt) {
    case VSF_EVT_INIT:
        if (    (VSF_FAT_32 != fsinfo->type) || !fsinfo->fsinfo_sector
            ||  (fsinfo->fsinfo_sector >= fsinfo->fat_sector)) {
            vsf_eda_return(VSF_ERR_NONE);
            break;
        }
        vsf_local.state = FLUSH_FSINFO_STATE_READ;
        __vk_malfs_read(malfs_info, fsinfo->fsinfo_sector, 1, NULL);
        break;
    case VSF_EVT_RETURN: {
            uint8_t *buff = (uint8_t *)vsf_eda_get_return_value();
            switch (vsf_local.state) {
            case FLUSH_FSINFO_STATE_READ:
                if (NULL == buff) {
                    vsf_eda_return(VSF_ERR_FAIL);
                    break;
                }
                if (    (get_unaligned_le32(&buff[0]) == 0x41615252)
                    &&  (get_unaligned_le32(&buff[484]) == 0x61417272)) {
                    put_unaligned_le32(0xFFFFFFFF, &buff[488]);
                }
                vsf_local.state = FLUSH_FSINFO_STATE_WRITE;
                __vk_malfs_write(malfs_info, fsinfo->fsinfo_sector, 1, buff);
                break;
            case FLUSH_FSINFO_STATE_WRITE:
                vsf_eda_return(
                    ((int32_t)(uintptr_t)buff == (1 << fsinfo->sector_size_bits))
                        ? VSF_ERR_NONE : VSF_ERR_FAIL);
                break;
            }
        }
        break;
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
    uint8_t state;
) {
    vsf_peda_begin();
    enum {
        APPEND_FAT_STATE_READ_FAT_DONE,
        APPEND_FAT_STATE_WRITE_FAT_DONE,
        APPEND_FAT_STATE_FLUSH_FSINFO_DONE,
    };
    __vk_fatfs_info_t *fsinfo = (__vk_fatfs_info_t *)&vsf_this;
    __vk_malfs_info_t *malfs_info = &fsinfo->use_as____vk_malfs_info_t;
    uint_fast8_t fat_bit = __vk_fatfs_fat_bitsize[fsinfo->type];
    uint32_t sector_bit_size_bits = fsinfo->sector_size_bits + 3;
    uint_fast32_t sector_bit = 1 << sector_bit_size_bits;
    uint64_t sector_bit_offset;
    uint32_t cur_sector;
    uint_fast16_t u32off, bitoff;

    switch (evt) {
    case VSF_EVT_INIT:
        vsf_local.cur_fat_idx = 0;

    write_next_fat:
        if (vsf_local.cur_fat_idx >= fsinfo->fat_num) {
            vsf_eda_return(VSF_ERR_NONE);
            return;
        }

        sector_bit_offset = vsf_local.cluster * fat_bit;
        vsf_local.cur_fat_sector_offset = (uint32_t)(sector_bit_offset >> sector_bit_size_bits);
        vsf_local.cur_fat_bit_offset_in_sector = sector_bit_offset & (sector_bit - 1);
        vsf_local.cur_fat_bit = 0;
        // the real link value is always written, also for contiguous exfat
        // chains: the stream entry advertises FAT-chain mode(flags=1), and
        // standard readers treat a 0 FAT entry as a free cluster(the old
        // cluster+1==next_cluster shortcut to 0 was only readable by this
        // driver's own get_fat_entry)
        vsf_local.cur_next_cluster = vsf_local.next_cluster;

    read_next_fat_sector:
        vsf_local.state = APPEND_FAT_STATE_READ_FAT_DONE;
        cur_sector = fsinfo->fat_sector + vsf_local.cur_fat_idx * fsinfo->fat_size + vsf_local.cur_fat_sector_offset;
        __vk_malfs_read(malfs_info, cur_sector, 1, NULL);
        break;
    case VSF_EVT_RETURN: {
            union {
                uintptr_t value;
                vsf_err_t err;
                uint8_t *buffer;
            } result;
            result.value = vsf_eda_get_return_value();

            switch (vsf_local.state) {
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

                u32off = vsf_local.cur_fat_bit_offset_in_sector >> 5;
                bitoff = vsf_local.cur_fat_bit_offset_in_sector & 0x1F;
                uint_fast16_t cur_bitsize;
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

                vsf_local.state = APPEND_FAT_STATE_WRITE_FAT_DONE;
                cur_sector = fsinfo->fat_sector + vsf_local.cur_fat_idx * fsinfo->fat_size + vsf_local.cur_fat_sector_offset;
                __vk_malfs_write(malfs_info, cur_sector, 1, result.buffer);
                break;
            case APPEND_FAT_STATE_WRITE_FAT_DONE:
                if ((int32_t)result.value != (1 << fsinfo->sector_size_bits)) {
                    goto fail;
                }
                if (vsf_local.cur_fat_bit < fat_bit) {
                    vsf_local.cur_fat_sector_offset++;
                    vsf_local.cur_fat_bit_offset_in_sector = 0;
                    goto read_next_fat_sector;
                }

                vsf_local.cur_fat_idx++;
                goto write_next_fat;
            case APPEND_FAT_STATE_FLUSH_FSINFO_DONE:
                vsf_eda_return(VSF_ERR_FAIL);
                break;
            }
        }
    }
    return;
fail:
    vsf_local.state = APPEND_FAT_STATE_FLUSH_FSINFO_DONE;
    {
        vsf_err_t err;
        __vsf_component_call_peda(__vk_fatfs_flush_fsinfo, err, fsinfo);
        if (err != VSF_ERR_NONE) {
            vsf_eda_return(VSF_ERR_FAIL);
        }
    }
    return;
    vsf_peda_end();
}

#if VSF_FS_USE_EXFATFS == ENABLED
__vsf_component_peda_private_entry(__vk_exfatfs_convert_to_fat_chain,,
    uint32_t cur_cluster;
    uint32_t remaining;
) {
    vsf_peda_begin();
    vk_fatfs_file_t *file = (vk_fatfs_file_t *)&vsf_this;
    __vk_fatfs_info_t *fsinfo = (__vk_fatfs_info_t *)file->info;

    switch (evt) {
    case VSF_EVT_INIT:
        if (!(file->dentry.stream_flags & 0x02) || (0 == file->dentry.data_length)) {
            vsf_eda_return(VSF_ERR_NONE);
            break;
        }
        {
            uint32_t cluster_size = 1 << (fsinfo->sector_size_bits + fsinfo->cluster_size_bits);
            uint64_t count = (file->dentry.data_length - 1) / cluster_size + 1;
            if (    (file->dentry.data_length < file->size)
                ||  (count > fsinfo->cluster_num - 2)
                ||  (file->first_cluster < 2)
                ||  (file->first_cluster + count - 1 >= fsinfo->cluster_num)) {
                vsf_eda_return(VSF_ERR_FAIL);
                break;
            }
            vsf_local.remaining = count;
        }
        vsf_local.cur_cluster = file->first_cluster;

    write_next:
        {
            vsf_err_t err;
            __vsf_component_call_peda(__vk_fatfs_set_fat_entry, err, fsinfo,
                .cluster = vsf_local.cur_cluster,
                .next_cluster = (vsf_local.remaining > 1)
                    ? vsf_local.cur_cluster + 1 : 0xFFFFFFFF,
            );
            if (err != VSF_ERR_NONE) {
                vsf_eda_return(err);
            }
        }
        break;
    case VSF_EVT_RETURN:
        if ((vsf_err_t)vsf_eda_get_return_value() != VSF_ERR_NONE) {
            vsf_eda_return(VSF_ERR_FAIL);
            break;
        }
        if (--vsf_local.remaining == 0) {
            file->dentry.stream_flags &= ~0x02;
            vsf_eda_return(VSF_ERR_NONE);
            break;
        }
        vsf_local.cur_cluster++;
        if (vsf_local.cur_cluster >= fsinfo->cluster_num) {
            vsf_eda_return(VSF_ERR_FAIL);
            break;
        }
        goto write_next;
    }
    vsf_peda_end();
}
#endif

__vsf_component_peda_private_entry(__vk_fatfs_append_fat_entry,,
    uint32_t cluster;
    uint32_t *entry;
    ,
    uint32_t cur_fat_bit;
    uint32_t cur_cluster;
    union {
        struct {
            uint32_t cur_fat_sector;
            uint32_t entry_tmp;
        };
        struct {
            uint32_t cur_bitmap_sector;
            uint32_t bitmap_remain_size;
        };
    };
    uint8_t state;
) {
    vsf_peda_begin();
    enum {
        APPEND_FAT_STATE_READ_FAT_DONE,
        APPEND_FAT_STATE_WRITE_EOF_FAT_DONE,
        APPEND_FAT_STATE_WRITE_PREV_FAT_DONE,
        APPEND_FAT_STATE_READ_BITMAP_DONE,
        APPEND_FAT_STATE_WRITE_BITMAP_DONE,
    };
    __vk_fatfs_info_t *fsinfo = (__vk_fatfs_info_t *)&vsf_this;
    __vk_malfs_info_t *malfs_info = &fsinfo->use_as____vk_malfs_info_t;
    uint_fast8_t fat_bit = __vk_fatfs_fat_bitsize[fsinfo->type];
    uint_fast32_t sector_bit = 1 << (fsinfo->sector_size_bits + 3);

    switch (evt) {
    case VSF_EVT_INIT:
        vsf_local.cur_cluster = 0;
#if VSF_FS_USE_EXFATFS == ENABLED
        if (VSF_FAT_EX == fsinfo->type) {
            vsf_local.cur_bitmap_sector = fsinfo->bitmap_sector;
            vsf_local.bitmap_remain_size = fsinfo->bitmap_size;
            vsf_local.state = APPEND_FAT_STATE_READ_BITMAP_DONE;
        read_next_bitmap_sector:
            __vk_malfs_read(malfs_info, vsf_local.cur_bitmap_sector, 1, NULL);
        } else
#endif
        {
            vsf_local.entry_tmp = 0;
            vsf_local.cur_fat_bit = 0;
            vsf_local.cur_fat_sector = fsinfo->fat_sector;
            vsf_local.state = APPEND_FAT_STATE_READ_FAT_DONE;
        read_next_fat_sector:
            __vk_malfs_read(malfs_info, vsf_local.cur_fat_sector, 1, NULL);
        }
        break;
    case VSF_EVT_RETURN: {
            union {
                uintptr_t value;
                vsf_err_t err;
                int32_t written_size;
                uint8_t *buffer;
            } result;
            result.value = vsf_eda_get_return_value();

            switch (vsf_local.state) {
#if VSF_FS_USE_EXFATFS == ENABLED
            case APPEND_FAT_STATE_READ_BITMAP_DONE:
                if (NULL == result.buffer) {
                    goto fail;
                }

                uint32_t curlen = 1 << fsinfo->sector_size_bits;
                curlen = vsf_min(curlen, vsf_local.bitmap_remain_size);
                int_fast32_t bit = vsf_bitmap_ffz(result.buffer, curlen << 3);
                if (bit < 0) {
                    vsf_local.bitmap_remain_size -= curlen;
                    if (vsf_local.bitmap_remain_size > 0) {
                        vsf_local.cur_bitmap_sector++;
                        goto read_next_bitmap_sector;
                    } else {
                        goto fail;
                    }
                }

                vsf_bitmap_set(result.buffer, bit);
                vsf_local.cur_cluster = ((fsinfo->bitmap_size - vsf_local.bitmap_remain_size) << 3) + bit + 2;
                vsf_local.state = APPEND_FAT_STATE_WRITE_BITMAP_DONE;
                __vk_malfs_write(malfs_info, vsf_local.cur_bitmap_sector, 1, result.buffer);
                break;
            case APPEND_FAT_STATE_WRITE_BITMAP_DONE:
                if (result.written_size != (1 << fsinfo->sector_size_bits)) {
                    goto fail;
                }
                goto cluster_found;
#endif
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
                            cluster_found: {
                                vsf_err_t err;
#if VSF_FS_USE_EXFATFS == ENABLED
                                uint32_t last_cluster = (VSF_FAT_EX == fsinfo->type) ? 0xFFFFFFFF : 0x0FFFFFFF;
#else
                                uint32_t last_cluster = 0x0FFFFFFF;
#endif
                                *vsf_local.entry = vsf_local.cur_cluster;
                                vsf_local.state = APPEND_FAT_STATE_WRITE_EOF_FAT_DONE;
                                __vsf_component_call_peda(__vk_fatfs_set_fat_entry, err, fsinfo,
                                    .cluster = vsf_local.cur_cluster,
                                    .next_cluster = last_cluster,
                                );
                                if (err != VSF_ERR_NONE) {
                                    goto fail;
                                }
                                return;
                            }}
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
                vsf_local.state = APPEND_FAT_STATE_WRITE_PREV_FAT_DONE;
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

#if VSF_FS_USE_EXFATFS == ENABLED
static uint16_t __vk_exfatfs_entry_checksum(uint16_t checksum, uint8_t *buffer, uint_fast16_t size, bool skip_checksum)
{
    for (uint_fast16_t i = 0; i < size; i++) {
        if (skip_checksum && ((2 == i) || (3 == i))) {
            continue;
        }
        checksum = ((checksum & 1) ? 0x8000 : 0) + (checksum >> 1) + (uint16_t)buffer[i];
    }
    return checksum;
}

__vsf_component_peda_private_entry(__vk_exfatfs_update_entry_checksum,,
    vk_fat_sector_type_t sector;
    uint8_t entry_num;
    uint8_t state;
    uint16_t checksum;
) {
    vsf_peda_begin();
    enum {
        EXFAT_UPDATE_ENTRY_CHECKSUM_READ_ROOT,
        EXFAT_UPDATE_ENTRY_CHECKSUM_READ_ENTRIES,
        EXFAT_UPDATE_ENTRY_CHECKSUM_READ_ROOT_FOR_WRITE,
        EXFAT_UPDATE_ENTRY_CHECKSUM_WRITE_ROOT,
    };
    vk_fatfs_file_t *file = (vk_fatfs_file_t *)&vsf_this;
    __vk_fatfs_info_t *fsinfo = (__vk_fatfs_info_t *)file->info;
    __vk_malfs_info_t *malfs_info = &fsinfo->use_as____vk_malfs_info_t;
    uint32_t entry_num_in_sector = 1 << (fsinfo->sector_size_bits - 5);
    fatfs_dentry_t *dentry;
    uint_fast16_t cur_entry_num;

    switch (evt) {
    case VSF_EVT_INIT:
        vsf_local.state = EXFAT_UPDATE_ENTRY_CHECKSUM_READ_ROOT;
        vsf_local.sector = file->dentry.sector0;
    read_next_sector:
        __vk_malfs_read(malfs_info, vsf_local.sector, 1, NULL);
        break;
    case VSF_EVT_RETURN: {
            union {
                uintptr_t value;
                vsf_err_t err;
                uint8_t *buffer;
                int32_t written_size;
            } result;

            result.value = vsf_eda_get_return_value();
            switch (vsf_local.state) {
            case EXFAT_UPDATE_ENTRY_CHECKSUM_READ_ROOT:
                if (NULL == result.buffer) {
                    vsf_eda_return(VSF_ERR_FAIL);
                } else {
                    dentry = (fatfs_dentry_t *)result.buffer + file->dentry.root_entry_offset;
                    cur_entry_num = entry_num_in_sector - file->dentry.root_entry_offset;
                    cur_entry_num = vsf_min(cur_entry_num, file->dentry.entry_num);
                    vsf_local.checksum = __vk_exfatfs_entry_checksum(0, (uint8_t *)dentry, cur_entry_num << 5, true);
                    if (cur_entry_num == file->dentry.entry_num) {
                        dentry->exfat.FilDir.SetChecksum = cpu_to_le16(vsf_local.checksum);
                        goto write_root;
                    } else {
                        vsf_local.entry_num = file->dentry.entry_num - cur_entry_num;
                        vsf_local.sector++;
                        vsf_local.state = EXFAT_UPDATE_ENTRY_CHECKSUM_READ_ENTRIES;
                        goto read_next_sector;
                    }
                }
                break;
            case EXFAT_UPDATE_ENTRY_CHECKSUM_READ_ENTRIES:
                if (NULL == result.buffer) {
                    vsf_eda_return(VSF_ERR_FAIL);
                } else {
                    cur_entry_num = vsf_min(entry_num_in_sector, vsf_local.entry_num);
                    vsf_local.checksum = __vk_exfatfs_entry_checksum(vsf_local.checksum, result.buffer, cur_entry_num << 5, false);
                    vsf_local.entry_num -= cur_entry_num;
                    if (0 == vsf_local.entry_num) {
                        vsf_local.sector = file->dentry.sector0;
                        vsf_local.state = EXFAT_UPDATE_ENTRY_CHECKSUM_READ_ROOT_FOR_WRITE;
                    } else {
                        vsf_local.sector++;
                    }
                    goto read_next_sector;
                }
                break;
            case EXFAT_UPDATE_ENTRY_CHECKSUM_READ_ROOT_FOR_WRITE:
                if (NULL == result.buffer) {
                    vsf_eda_return(VSF_ERR_FAIL);
                } else {
                write_root:
                    vsf_local.state = EXFAT_UPDATE_ENTRY_CHECKSUM_WRITE_ROOT;
                    __vk_malfs_write(malfs_info, file->dentry.sector0, 1, result.buffer);
                }
                break;
            case EXFAT_UPDATE_ENTRY_CHECKSUM_WRITE_ROOT:
                if (result.written_size != (1 << fsinfo->sector_size_bits)) {
                    vsf_eda_return(VSF_ERR_FAIL);
                } else {
                    vsf_eda_return(VSF_ERR_NONE);
                }
                break;
            }
        }
        break;
    }
    vsf_peda_end();
}

#endif

__vsf_component_peda_private_entry(__vk_fatfs_dentry_setsize,,
    uint64_t size;
    uint64_t data_length;
#if VSF_FS_USE_EXFATFS == ENABLED
    bool need_update_checksum;
#endif
    vsf_err_t op_err;
    uint8_t state;
) {
    vsf_peda_begin();
    enum {
        DENTRY_SETSIZE_STATE_READ,
        DENTRY_SETSIZE_STATE_WRITE,
#if VSF_FS_USE_EXFATFS == ENABLED
        DENTRY_SETSIZE_STATE_UPDATE_CHECKSUM,
#endif
        DENTRY_SETSIZE_STATE_FLUSH_FSINFO,
    };
    vk_fatfs_file_t *file = (vk_fatfs_file_t *)&vsf_this;
    __vk_fatfs_info_t *fsinfo = (__vk_fatfs_info_t *)file->info;
    __vk_malfs_info_t *malfs_info = &fsinfo->use_as____vk_malfs_info_t;
    uint32_t sector = file->dentry.vital_sector ? file->dentry.sector1 : file->dentry.sector0;

    switch (evt) {
    case VSF_EVT_INIT:
#if VSF_FS_USE_EXFATFS == ENABLED
        vsf_local.need_update_checksum = false;
#endif
        vsf_local.state = DENTRY_SETSIZE_STATE_READ;
        __vk_malfs_read(malfs_info, sector, 1, NULL);
        break;
    case VSF_EVT_RETURN: {
            union {
                uintptr_t value;
                vsf_err_t err;
                uint8_t *buffer;
            } result;

            result.value = vsf_eda_get_return_value();
            switch (vsf_local.state) {
            case DENTRY_SETSIZE_STATE_READ:
                if (NULL == result.buffer) {
                    vsf_eda_return(VSF_ERR_FAIL);
                } else {
                    fatfs_dentry_t *dentry = (fatfs_dentry_t *)result.buffer + file->dentry.vital_entry_offset;
#if VSF_FS_USE_EXFATFS == ENABLED
                    if (VSF_FAT_EX == fsinfo->type) {
                        dentry->exfat.Stream.GeneralSecondaryFlags = 1; // AllocationPossible, !NoFatChain
                        dentry->exfat.Stream.ValidDataLength = cpu_to_le64(vsf_local.size);
                        dentry->exfat.Stream.DataLength = cpu_to_le64(vsf_local.data_length);
                        dentry->exfat.Stream.FirstCluster = cpu_to_le32(file->first_cluster);

                        uint32_t entry_num_in_sector = 1 << (fsinfo->sector_size_bits - 5);
                        if ((file->dentry.root_entry_offset + file->dentry.entry_num) < entry_num_in_sector) {
                            dentry = (fatfs_dentry_t *)result.buffer + file->dentry.root_entry_offset;
                            uint16_t checksum = __vk_exfatfs_entry_checksum(0, (uint8_t *)dentry, file->dentry.entry_num << 5, true);
                            dentry->exfat.FilDir.SetChecksum = cpu_to_le16(checksum);
                        } else {
                            vsf_local.need_update_checksum = true;
                        }
                    } else
#endif
                    {
                        dentry->fat.FileSize = (uint32_t)cpu_to_le32(vsf_local.size);
                        dentry->fat.FstClusHi = cpu_to_le16(file->first_cluster >> 16);
                        dentry->fat.FstClusLo = cpu_to_le16(file->first_cluster);
                    }
                    vsf_local.state = DENTRY_SETSIZE_STATE_WRITE;
                    __vk_malfs_write(malfs_info, sector, 1, result.buffer);
                }
                break;
            case DENTRY_SETSIZE_STATE_WRITE:
#if VSF_FS_USE_EXFATFS == ENABLED
                if (vsf_local.need_update_checksum) {
                    vsf_err_t err;
                    vsf_local.op_err = VSF_ERR_NONE;
                    vsf_local.state = DENTRY_SETSIZE_STATE_UPDATE_CHECKSUM;
                    __vsf_component_call_peda(__vk_exfatfs_update_entry_checksum, err, file);
                    if (err != VSF_ERR_NONE) {
                        VSF_FS_ASSERT(false);
                        vsf_eda_return(VSF_ERR_FAIL);
                    }
                    break;
                }
#endif
                // fall through: data committed, sync the FSInfo hint
#if VSF_FS_USE_EXFATFS == ENABLED
            case DENTRY_SETSIZE_STATE_UPDATE_CHECKSUM:
                vsf_local.op_err = (DENTRY_SETSIZE_STATE_UPDATE_CHECKSUM == vsf_local.state)
                    ? result.err : VSF_ERR_NONE;
#else
                vsf_local.op_err = VSF_ERR_NONE;
#endif
                {
                    vsf_err_t err;
                    vsf_local.state = DENTRY_SETSIZE_STATE_FLUSH_FSINFO;
                    __vsf_component_call_peda(__vk_fatfs_flush_fsinfo, err, fsinfo);
                    if (VSF_ERR_NONE != err) {
                        // flush start failure is non-fatal: the data is
                        // already committed, FSInfo is only a hint
                        vsf_eda_return(vsf_local.op_err);
                    }
                }
                break;
            case DENTRY_SETSIZE_STATE_FLUSH_FSINFO:
                // flush outcome ignored(see above)
                vsf_eda_return(vsf_local.op_err);
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
    __vk_malfs_info_t *malfs_info = &fsinfo->use_as____vk_malfs_info_t;
    uint32_t clustersize = 1 << (fsinfo->cluster_size_bits + fsinfo->sector_size_bits);
    uint_fast8_t fat_bit = __vk_fatfs_fat_bitsize[fsinfo->type];
    vsf_err_t err = VSF_ERR_NONE;

    switch (evt) {
    case VSF_EVT_INIT:
        vsf_local.cur_offset = vsf_local.offset;
        vsf_local.cur_cluster = file->first_cluster;
        vsf_local.fat_buff = NULL;
        vsf_local.fat_pending_fetch = 0;
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
        if (VSF_EVT_RETURN == evt) {
            if (vsf_local.fat_pending_fetch) {
                // sector fetch completed: keep the malfs cache buffer and
                // walk it in pure CPU(safe: no other malfs op can evict it
                // while this peda step runs to completion)
                uint8_t *buff = (uint8_t *)vsf_eda_get_return_value();
                if (NULL == buff) {
                    err = VSF_ERR_FAIL;
                    goto __fail_and_exit;
                }
                vsf_local.fat_buff = buff;
                vsf_local.fat_buff_sector = vsf_local.fetch_sector;
                vsf_local.fat_pending_fetch = 0;
            }
        }
        // parse consecutive FAT entries from one cached sector and fetch
        // another sector only when the walk leaves the current one
        while (vsf_local.cur_offset > clustersize) {
            uint_fast32_t bit = (uint_fast32_t)vsf_local.cur_cluster * fat_bit;
            uint_fast32_t sector = fsinfo->fat_sector + (bit >> (fsinfo->sector_size_bits + 3));
            uint_fast32_t in_sec = bit & ((1 << (fsinfo->sector_size_bits + 3)) - 1);
            uint32_t entry;

            if (    (NULL != vsf_local.fat_buff)
                &&  (sector == vsf_local.fat_buff_sector)
                &&  ((in_sec + fat_bit) <= (1u << (fsinfo->sector_size_bits + 3)))) {
                uint8_t *p = &vsf_local.fat_buff[in_sec >> 3];
                if (12 == fat_bit) {
                    entry = (((uint32_t)p[0] | ((uint32_t)p[1] << 8)) >> (in_sec & 7)) & 0xFFF;
                } else if (16 == fat_bit) {
                    entry = (uint32_t)p[0] | ((uint32_t)p[1] << 8);
                } else {
                    entry = get_unaligned_le32(p);
                    if (32 == fat_bit) {
                        entry &= 0x0FFFFFFF;
                    }
#if VSF_FS_USE_EXFATFS == ENABLED
                    if ((VSF_FAT_EX == fsinfo->type) && (0 == entry)) {
                        // exFAT: a zero entry means the chain continues at
                        // the numerically next cluster
                        entry = vsf_local.cur_cluster + 1;
                    }
#endif
                }
                vsf_local.cur_cluster = entry;
                vsf_local.cur_offset -= clustersize;
                continue;
            }

            if ((in_sec + fat_bit) > (1u << (fsinfo->sector_size_bits + 3))) {
                // FAT12 entry straddling the sector edge: one legacy hop
                // (get_fat_entry handles the crossing); cur_cluster is
                // updated on its completion, cur_offset is consumed here
                __vsf_component_call_peda(__vk_fatfs_get_fat_entry, err, fsinfo,
                    .cluster = vsf_local.cur_cluster,
                    .entry = &vsf_local.cur_cluster,
                )
                if (VSF_ERR_NONE != err) {
                    goto __fail_and_exit;
                }
                vsf_local.cur_offset -= clustersize;
                return;
            } else {
                // fetch the FAT sector holding this entry; the hop itself
                // is retried from the buffer on return(cur_offset NOT
                // consumed yet)
                vsf_local.fetch_sector = sector;
                vsf_local.fat_pending_fetch = 1;
                __vk_malfs_read(malfs_info, sector, 1, NULL);
                return;
            }
        }
        file->cur.cluster = vsf_local.cur_cluster;
        file->cur.sector_offset_in_cluster = vsf_local.cur_offset / (1 << fsinfo->sector_size_bits);
        file->cur.offset_in_sector = vsf_local.cur_offset % (1 << fsinfo->sector_size_bits);
        VSF_FS_ASSERT(vsf_local.result != NULL);
        *vsf_local.result = vsf_local.offset;
        vsf_eda_return(VSF_ERR_NONE);
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
        SETSIZE_STATE_CONVERT_DONE,
        SETSIZE_STATE_APPEND_DONE,
        SETSIZE_STATE_APPEND,
        SETSIZE_STATE_FREE_PREPARE,
        SETSIZE_STATE_FREE_NEXT,
        SETSIZE_STATE_FREE,
        SETSIZE_STATE_FREE_SET_DONE,
        SETSIZE_STATE_BITMAP_READ,
        SETSIZE_STATE_BITMAP_WRITE,
        SETSIZE_STATE_UPDATE_DENTRY_DONE,
    };
    vk_fatfs_file_t *fatfs_file = (vk_fatfs_file_t *)&vsf_this;
    __vk_fatfs_info_t *fsinfo = (__vk_fatfs_info_t *)fatfs_file->info;
    __vk_malfs_info_t *malfs_info = &fsinfo->use_as____vk_malfs_info_t;
    uint32_t clustersize = 1 << (fsinfo->cluster_size_bits + fsinfo->sector_size_bits);
    uint64_t size = vsf_local.size;
    vsf_err_t err;
    VSF_FS_ASSERT(!(fatfs_file->attr & VSF_FILE_ATTR_DIRECTORY));

    switch (evt) {
    case VSF_EVT_INIT: {
#if VSF_FS_USE_EXFATFS == ENABLED
            if (    (VSF_FAT_EX == fsinfo->type)
                &&  (fatfs_file->dentry.stream_flags & 0x02)
                &&  fatfs_file->dentry.data_length) {
                vsf_local.state = SETSIZE_STATE_CONVERT_DONE;
                __vsf_component_call_peda(__vk_exfatfs_convert_to_fat_chain, err, fatfs_file);
                if (err != VSF_ERR_NONE) {
                    goto fail;
                }
                break;
            }
#endif
        setsize_start:
            ;
            uint64_t round_size = (size + clustersize - 1) & ~(clustersize - 1);
#if VSF_FS_USE_EXFATFS == ENABLED
            uint64_t allocated_size = (VSF_FAT_EX == fsinfo->type)
                ? fatfs_file->dentry.data_length : fatfs_file->size;
#else
            uint64_t allocated_size = fatfs_file->size;
#endif
            uint64_t round_file_size = (allocated_size + clustersize - 1) & ~(clustersize - 1);
            int64_t pos;

            vsf_local.orig_pos = fatfs_file->pos;
            vsf_local.orig_fatfs_pos = fatfs_file->cur;
            if (round_size == round_file_size) {
                goto update_dentry;
            } else {
                if (round_size > round_file_size) {
                    vsf_local.clusters = (round_size - round_file_size) / clustersize;
                    vsf_local.state = SETSIZE_STATE_APPEND;
                    pos = allocated_size;
                } else {
                    if (0 == size) {
                        fatfs_file->first_cluster = 0;
                        vsf_local.next_cluster = 0;
                    } else {
#if VSF_FS_USE_EXFATFS == ENABLED
                        vsf_local.next_cluster = (VSF_FAT_EX == fsinfo->type) ? 0xFFFFFFFF : 0x0FFFFFFF;;
#else
                        vsf_local.next_cluster = 0x0FFFFFFF;;
#endif
                    }
                    vsf_local.state = SETSIZE_STATE_FREE_PREPARE;
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
        switch (vsf_local.state) {
#if VSF_FS_USE_EXFATFS == ENABLED
        case SETSIZE_STATE_CONVERT_DONE:
            err = vsf_eda_get_return_value();
            if (err != VSF_ERR_NONE) {
                goto fail;
            }
            goto setsize_start;
#endif
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
                vsf_local.state = SETSIZE_STATE_APPEND_DONE;
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
#if VSF_FS_USE_EXFATFS == ENABLED
                vsf_local.freed_cluster = cur_cluster;
                vsf_local.free_bitmap = (0 == next_cluster);
#endif
                vsf_local.state = SETSIZE_STATE_FREE_SET_DONE;
                __vsf_component_call_peda(__vk_fatfs_set_fat_entry, err, fsinfo,
                    .cluster = cur_cluster,
                    .next_cluster = next_cluster,
                );
                if (err != VSF_ERR_NONE) {
                    goto fail;
                }
            }
            break;
        case SETSIZE_STATE_FREE_SET_DONE:
            err = vsf_eda_get_return_value();
            if (err != VSF_ERR_NONE) {
                goto fail;
            }
#if VSF_FS_USE_EXFATFS == ENABLED
            if ((VSF_FAT_EX == fsinfo->type) && vsf_local.free_bitmap) {
                vsf_local.cur_sector = fsinfo->bitmap_sector
                    + ((vsf_local.freed_cluster - 2) >> (fsinfo->sector_size_bits + 3));
                if ((uint32_t)vsf_local.cur_sector >= fsinfo->bitmap_sector + fsinfo->bitmap_size) {
                    goto fail;
                }
                vsf_local.state = SETSIZE_STATE_BITMAP_READ;
                __vk_malfs_read(malfs_info, vsf_local.cur_sector, 1, NULL);
                break;
            }
#endif
            goto free_next;
#if VSF_FS_USE_EXFATFS == ENABLED
        case SETSIZE_STATE_BITMAP_READ: {
                uint8_t *buff = (uint8_t *)vsf_eda_get_return_value();
                if (NULL == buff) {
                    goto fail;
                }
                vsf_bitmap_clear(buff, (vsf_local.freed_cluster - 2)
                    & ((1 << (fsinfo->sector_size_bits + 3)) - 1));
                vsf_local.state = SETSIZE_STATE_BITMAP_WRITE;
                __vk_malfs_write(malfs_info, vsf_local.cur_sector, 1, buff);
            }
            break;
        case SETSIZE_STATE_BITMAP_WRITE:
            if ((int32_t)vsf_eda_get_return_value() != (1 << fsinfo->sector_size_bits)) {
                goto fail;
            }
            goto free_next;
#endif
        case SETSIZE_STATE_FREE_PREPARE:
            if (vsf_local.orig_pos > size) {
                vsf_local.orig_pos = fatfs_file->pos;
                vsf_local.orig_fatfs_pos = fatfs_file->cur;
            }
            vsf_local.cur_cluster = fatfs_file->cur.cluster;
        case SETSIZE_STATE_FREE_NEXT:
        free_next:
            if (    __vk_fatfs_fat_entry_is_valid(fsinfo, vsf_local.cur_cluster)
                &&  !__vk_fatfs_fat_entry_is_eof(fsinfo, vsf_local.cur_cluster)) {
                vsf_local.state = SETSIZE_STATE_FREE;
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
                vsf_local.state = SETSIZE_STATE_UPDATE_DENTRY_DONE;
                __vsf_component_call_peda(__vk_fatfs_dentry_setsize, err, fatfs_file,
                    .size   = vsf_local.size,
                    .data_length = vsf_local.size,
                );
            }
            break;
        case SETSIZE_STATE_UPDATE_DENTRY_DONE:
            err = vsf_eda_get_return_value();
            if (err != VSF_ERR_NONE) {
                goto fail;
            }
#if VSF_FS_USE_EXFATFS == ENABLED
            fatfs_file->dentry.data_length = vsf_local.size;
            fatfs_file->dentry.stream_flags &= ~0x02;
#endif
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
#if VSF_USE_HEAP != ENABLED
    VSF_FS_ASSERT(false);
    vsf_eda_return(VSF_ERR_NOT_SUPPORT);
#else
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
        vsf_local.dparser = vsf_heap_malloc(sizeof(*vsf_local.dparser) + VSF_FATFS_CFG_MAX_FILENAME);
        if (NULL == vsf_local.dparser) {
            err = VSF_ERR_NOT_ENOUGH_RESOURCES;
            goto __fail_and_exit;
        }
        vsf_local.dparser->filename = (char *)&vsf_local.dparser[1];
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
        vsf_local.dparser->zero_before_first_call = 0;
        if (name != NULL) {
            vsf_local.fatfs_file_pos_save = dir->cur;
            dir->cur.cluster = dir->first_cluster;
            dir->cur.sector_offset_in_cluster = 0;
            dir->cur.offset_in_sector = 0;
        }
        vsf_local.state = LOOKUP_STATE_READ_SECTOR;

        // fall through
    case VSF_EVT_RETURN:
        switch (vsf_local.state) {
        case LOOKUP_STATE_READ_SECTOR:
        read_fat_sector:
            vsf_local.state = LOOKUP_STATE_PARSE_SECTOR;
            __vk_malfs_read(malfs_info, vsf_local.cur_sector, 1, NULL);
            break;
        case LOOKUP_STATE_PARSE_SECTOR: {
                fatfs_dentry_t *dentry = (fatfs_dentry_t *)vsf_eda_get_return_value();
                vk_fatfs_dentry_parser_t *dparser = vsf_local.dparser;
                uint32_t entry_num_in_sector = 1 << (fsinfo->sector_size_bits - 5);

                if (NULL == dentry) {
                    goto __fail_and_exit;
                }

                dparser->entry = (uint8_t *)dentry + dir->cur.offset_in_sector;
                dparser->entry_num = entry_num_in_sector;
                dparser->entry_num -= dir->cur.offset_in_sector >> 5;
                while (dparser->entry_num) {
                    uint16_t entry_num = dparser->entry_num;
                    bool parsed =
#if VSF_FS_USE_EXFATFS == ENABLED
                        (fsinfo->type == VSF_FAT_EX) ? vk_fatfs_parse_dentry_exfat(dparser) :
#endif
                        vk_fatfs_parse_dentry_fat(dparser);
                    uint32_t parsed_size = (entry_num - dparser->entry_num) << 5;
                    dir->cur.offset_in_sector += parsed_size;
                    if (NULL == name) {
                        dir->pos += parsed_size;
                    }

                    if (parsed) {
                        char *filename_str;
                        uint_fast16_t filename_len;

                        if (dparser->is_unicode) {
                            // exFAT/LFN names are utf16: convert to utf8 for
                            //  both matching and storage
                            filename_len = __vk_fatfs_utf16_to_utf8((uint16_t *)dparser->filename, NULL);
                            filename_str = vsf_heap_malloc(filename_len);
                            if (NULL == filename_str) {
                            fail_mem:
                                err = VSF_ERR_NOT_ENOUGH_RESOURCES;
                                goto __fail_and_exit;
                            }
                            __vk_fatfs_utf16_to_utf8((uint16_t *)dparser->filename, filename_str);
                        } else {
                            filename_str = dparser->filename;
                            filename_len = strlen(dparser->filename) + 1;
                        }

                        if (!name || vk_file_is_match((char *)name, filename_str)) {
                            vk_fatfs_file_t *fatfs_file = (vk_fatfs_file_t *)vk_file_alloc(sizeof(vk_fatfs_file_t));
                            if (NULL == fatfs_file) {
                                if (filename_str != dparser->filename) {
                                    vsf_heap_free(filename_str);
                                }
                                goto fail_mem;
                            }

                            fatfs_file->name = vsf_heap_malloc(filename_len);
                            if (NULL == fatfs_file->name) {
                                if (filename_str != dparser->filename) {
                                    vsf_heap_free(filename_str);
                                }
                                vk_file_free(&fatfs_file->use_as__vk_file_t);
                                goto fail_mem;
                            }
                            memcpy(fatfs_file->name, filename_str, filename_len);

                            fatfs_file->attr |= (vk_file_attr_t)__vk_fatfs_parse_file_attr(dparser->attr);
                            fatfs_file->fsop = &vk_fatfs_op;
                            fatfs_file->size = dparser->size;
                            fatfs_file->info = fsinfo;
                            fatfs_file->first_cluster = dparser->first_cluster;
#if VSF_FS_USE_EXFATFS == ENABLED
                            if (VSF_FAT_EX == fsinfo->type) {
                                fatfs_file->dentry.stream_flags = dparser->exfat.stream_flags;
                                fatfs_file->dentry.data_length = dparser->data_length;
                            }
#endif
                            fatfs_file->cur.cluster = fatfs_file->first_cluster;
                            fatfs_file->cur.sector_offset_in_cluster = 0;
                            fatfs_file->cur.offset_in_sector = 0;

                            if (vsf_local.entry_num) {
                                fatfs_file->dentry.sector0 = vsf_local.sector;
                                fatfs_file->dentry.entry_offset_in_sector0 = entry_num_in_sector - vsf_local.entry_num;
                                fatfs_file->dentry.entry_num = vsf_local.entry_num + dparser->node_parsed_num;
                                fatfs_file->dentry.sector1 = vsf_local.cur_sector;
                                if (vsf_local.vital_entry_num >= 0) {
                                    fatfs_file->dentry.vital_sector = 0;
                                    fatfs_file->dentry.vital_entry_offset = entry_num_in_sector - vsf_local.vital_entry_num;
                                } else {
                                    fatfs_file->dentry.vital_sector = 1;
                                    VSF_FS_ASSERT(dparser->vital_entry_num >= 0);
                                    fatfs_file->dentry.vital_entry_offset = entry_num_in_sector - dparser->vital_entry_num;
                                }
#if VSF_FS_USE_EXFATFS == ENABLED
                                if (fsinfo->type == VSF_FAT_EX) {
                                    fatfs_file->dentry.root_entry_offset = entry_num_in_sector - vsf_local.root_entry_num;
                                }
#endif
                            } else {
                                fatfs_file->dentry.sector0 = vsf_local.cur_sector;
                                fatfs_file->dentry.entry_offset_in_sector0 = entry_num_in_sector - dparser->entry_num - dparser->node_parsed_num;
                                fatfs_file->dentry.entry_num = dparser->node_parsed_num;
                                fatfs_file->dentry.vital_sector = 0;
                                VSF_FS_ASSERT(dparser->vital_entry_num >= 0);
                                fatfs_file->dentry.vital_entry_offset = entry_num_in_sector - dparser->vital_entry_num;
#if VSF_FS_USE_EXFATFS == ENABLED
                                if (fsinfo->type == VSF_FAT_EX) {
                                    fatfs_file->dentry.root_entry_offset = entry_num_in_sector - dparser->exfat.root_entry_num;
                                }
#endif
                            }

                            *vsf_local.result = &fatfs_file->use_as__vk_file_t;
                            goto __fail_and_exit;
                        } else {
                            vsf_local.entry_num = 0;
                        }
                        dparser->entry += 32;
                    } else if (dparser->entry_num > 0) {
                        goto __not_available;
                    } else {
                        vsf_local.sector = vsf_local.cur_sector;
                        vsf_local.entry_num = dparser->node_parsed_num;
                        vsf_local.vital_entry_num = dparser->vital_entry_num;
#if VSF_FS_USE_EXFATFS == ENABLED
                        vsf_local.root_entry_num = dparser->exfat.root_entry_num;
#endif
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
                    vsf_local.state = LOOKUP_STATE_READ_FAT;
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
                if (NULL == name) {
                    dir->cur.cluster = dir->first_cluster;
                    dir->cur.sector_offset_in_cluster = 0;
                    dir->cur.offset_in_sector = 0;
                    dir->pos = 0;
                }
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
    if (name != NULL) {
        dir->cur = vsf_local.fatfs_file_pos_save;
    }
    if (vsf_local.dparser != NULL) {
        vsf_heap_free(vsf_local.dparser);
    }
    vsf_eda_return(err);
#endif
    vsf_peda_end();
}

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_warning=pe111
#endif

// FAT32: scan the whole parent directory once for a duplicate short name
// and the first free slot(0xE5/0x00), grow the chain by one zeroed cluster
// when there is no usable slot, then write the entry read-modify-write.
// names that cannot round-trip through 8.3 are refused rather than mangled.
// exFAT: duplicate check goes through lookup(lfn-aware), the scan only has
// to find a consecutive free run for the File/Stream/Name entry set, which
// is written sector by sector(it may straddle sectors/cluster boundary)
__vsf_component_peda_ifs_entry(__vk_fatfs_create, vk_file_create,
    implement(vk_fatfs_create_local)
) {
    vsf_peda_begin();
#if VSF_USE_HEAP != ENABLED
    VSF_FS_ASSERT(false);
    vsf_eda_return(VSF_ERR_NOT_SUPPORT);
#else
    enum {
        CREATE_STATE_SCAN_SECTOR,
        CREATE_STATE_SCAN_FAT,
        CREATE_STATE_ALLOC,
        CREATE_STATE_ZERO,
        CREATE_STATE_READ_SLOT,
        CREATE_STATE_WRITE_SLOT,
        CREATE_STATE_FLUSH_FSINFO,
#if VSF_FS_USE_EXFATFS == ENABLED
        CREATE_STATE_EX_LOOKUP,
        CREATE_STATE_EX_WRITE_READ,
        CREATE_STATE_EX_WRITE_WRITE,
        CREATE_STATE_EX_WRITE_NEXT,
#endif
    };
    vk_fatfs_file_t *dir = (vk_fatfs_file_t *)&vsf_this;
    __vk_fatfs_info_t *fsinfo = (__vk_fatfs_info_t *)dir->info;
    __vk_malfs_info_t *malfs_info = &fsinfo->use_as____vk_malfs_info_t;
    uint32_t sector_size = 1 << fsinfo->sector_size_bits;
    vsf_err_t err;

    switch (evt) {
    case VSF_EVT_INIT:
        vsf_local.zero_buff = NULL;
#if VSF_FS_USE_EXFATFS == ENABLED
        vsf_local.utf16_name = NULL;
        vsf_local.set_buff = NULL;
#endif
        if (vsf_local.attr & VSF_FILE_ATTR_DIRECTORY) {
            // directory create needs the dot entries written into a fresh
            // cluster, separate path
            err = VSF_ERR_NOT_SUPPORT;
            goto __exit;
        }
#if VSF_FS_USE_EXFATFS == ENABLED
        if (VSF_FAT_EX == fsinfo->type) {
            int namelen;

            vsf_local.utf16_name = vsf_heap_malloc(255 * sizeof(uint16_t));
            if (NULL == vsf_local.utf16_name) {
                err = VSF_ERR_NOT_ENOUGH_RESOURCES;
                goto __exit;
            }
            namelen = __vk_fatfs_name_to_utf16(vsf_local.name, vsf_local.utf16_name);
            if (namelen < 0) {
                err = VSF_ERR_NOT_SUPPORT;
                goto __exit;
            }
            vsf_local.namelen = (uint8_t)namelen;
            vsf_local.name_hash = __vk_fatfs_exfat_name_hash(vsf_local.utf16_name, namelen);
            vsf_local.set_entry_num = 2 + ((namelen + 14) / 15);

            // duplicate-name check through the lfn/unicode-aware lookup
            vsf_local.state = CREATE_STATE_EX_LOOKUP;
            __vsf_component_call_peda_ifs(vk_file_lookup, err, dir->fsop->dop.fn_lookup, dir->fsop->dop.lookup_local_size,
                &dir->use_as__vk_file_t,
                .name       = vsf_local.name,
                .result     = &vsf_local.result_file,
            );
            if (VSF_ERR_NONE != err) {
                vsf_eda_return(err);
            }
            break;
        }
#endif
        // FAT12/16 have a flat root, this cut only writes FAT32 chains
        if (    (VSF_FAT_32 != fsinfo->type)
            ||  !__vk_fatfs_name_to_sfn(vsf_local.name, vsf_local.sfn, &vsf_local.lcase)) {
            err = VSF_ERR_NOT_SUPPORT;
            goto __exit;
        }

        vsf_local.slot_found = false;
        vsf_local.scan_terminated = false;
        vsf_local.cur_cluster = dir->first_cluster;
        vsf_local.sector_offset_in_cluster = 0;
        vsf_local.offset_in_sector = 0;
        vsf_local.cur_sector = __vk_fatfs_clus2sec(fsinfo, vsf_local.cur_cluster);
        vsf_local.state = CREATE_STATE_SCAN_SECTOR;
        __vk_malfs_read(malfs_info, vsf_local.cur_sector, 1, NULL);
        break;
    case VSF_EVT_RETURN: {
            union {
                uintptr_t value;
                vsf_err_t err;
                int32_t written_size;
                uint8_t *buffer;
            } result;
            result.value = vsf_eda_get_return_value();

            switch (vsf_local.state) {
#if VSF_FS_USE_EXFATFS == ENABLED
            case CREATE_STATE_EX_LOOKUP: {
                    vk_fatfs_file_t *file = (vk_fatfs_file_t *)vsf_local.result_file;
                    vsf_err_t lookup_err = (vsf_err_t)result.value;

                    if (VSF_ERR_NONE == lookup_err) {
                        if (file != NULL) {
                            vsf_heap_free(file->name);
                            vk_file_free(&file->use_as__vk_file_t);
                        }
                        err = VSF_ERR_ALREADY_EXISTS;
                        goto __exit;
                    }
                    if (VSF_ERR_NOT_AVAILABLE != lookup_err) {
                        err = lookup_err;
                        goto __exit;
                    }

                    vsf_local.slot_found = false;
                    vsf_local.scan_terminated = false;
                    vsf_local.run_len = 0;
                    vsf_local.cur_cluster = dir->first_cluster;
                    vsf_local.sector_offset_in_cluster = 0;
                    vsf_local.offset_in_sector = 0;
                    vsf_local.cur_sector = __vk_fatfs_clus2sec(fsinfo, vsf_local.cur_cluster);
                    vsf_local.state = CREATE_STATE_SCAN_SECTOR;
                    __vk_malfs_read(malfs_info, vsf_local.cur_sector, 1, NULL);
                }
                break;
#endif
            case CREATE_STATE_SCAN_SECTOR: {
                    fatfs_dentry_t *dentry = (fatfs_dentry_t *)result.buffer;
                    if (NULL == dentry) {
                        err = VSF_ERR_FAIL;
                        goto __exit;
                    }

#if VSF_FS_USE_EXFATFS == ENABLED
                    if (VSF_FAT_EX == fsinfo->type) {
                        // free = never-used(0x00) or deleted(bit7 cleared,
                        // except the 0x20 padding); the first consecutive run
                        // long enough for the entry set wins, no name checks
                        // (lookup did that). a 0x00 entry virtually extends
                        // the run to the end of the cluster: the set must
                        // land IN the 0x00 region, the parse side stops at
                        // the first 0x00 and never sees entries behind it
                        while (vsf_local.offset_in_sector < sector_size) {
                            uint8_t entry_type = ((uint8_t *)dentry)[vsf_local.offset_in_sector];
                            bool is_free = (0 == entry_type)
                                || ((!(entry_type & 0x80)) && (0x20 != entry_type));

                            if (is_free) {
                                if ((0 == vsf_local.run_len) && !vsf_local.slot_found) {
                                    vsf_local.slot_sector = vsf_local.cur_sector;
                                    vsf_local.slot_cluster = vsf_local.cur_cluster;
                                    vsf_local.slot_sector_in_cluster = vsf_local.sector_offset_in_cluster;
                                    vsf_local.slot_offset = vsf_local.offset_in_sector;
                                }
                                if (0 == entry_type) {
                                    uint32_t entries_in_cluster =
                                          ((1 << fsinfo->cluster_size_bits) - 1 - vsf_local.sector_offset_in_cluster)
                                        * (sector_size / sizeof(fatfs_dentry_t))
                                        + ((sector_size - vsf_local.offset_in_sector) / sizeof(fatfs_dentry_t));
                                    if (    !vsf_local.slot_found
                                        &&  ((vsf_local.run_len + entries_in_cluster) >= vsf_local.set_entry_num)) {
                                        vsf_local.slot_found = true;
                                    }
                                    vsf_local.scan_terminated = true;
                                    break;
                                }
                                vsf_local.run_len++;
                                if (!vsf_local.slot_found
                                &&  (vsf_local.run_len >= vsf_local.set_entry_num)) {
                                    vsf_local.slot_found = true;
                                    break;
                                }
                            } else {
                                vsf_local.run_len = 0;
                            }
                            vsf_local.offset_in_sector += sizeof(*dentry);
                        }
                    } else
#endif
                    {
                        while (vsf_local.offset_in_sector < sector_size) {
                            fatfs_dentry_t *cur = (fatfs_dentry_t *)
                                ((uint8_t *)dentry + vsf_local.offset_in_sector);
                            if (0 == cur->fat.Name[0]) {
                                // end of directory: no live entry can follow
                                if (!vsf_local.slot_found) {
                                    vsf_local.slot_sector = vsf_local.cur_sector;
                                    vsf_local.slot_cluster = vsf_local.cur_cluster;
                                    vsf_local.slot_sector_in_cluster = vsf_local.sector_offset_in_cluster;
                                    vsf_local.slot_offset = vsf_local.offset_in_sector;
                                    vsf_local.slot_found = true;
                                }
                                vsf_local.scan_terminated = true;
                                break;
                            } else if ((char)0xE5 == cur->fat.Name[0]) {
                                if (!vsf_local.slot_found) {
                                    vsf_local.slot_sector = vsf_local.cur_sector;
                                    vsf_local.slot_cluster = vsf_local.cur_cluster;
                                    vsf_local.slot_sector_in_cluster = vsf_local.sector_offset_in_cluster;
                                    vsf_local.slot_offset = vsf_local.offset_in_sector;
                                    vsf_local.slot_found = true;
                                }
                            } else if (    (cur->fat.Attr != FAT_ATTR_LFN)
                                        && (cur->fat.Attr != FAT_ATTR_VOLUME_ID)
                                        &&  !memcmp(cur->fat.Name, vsf_local.sfn, 11)) {
                                err = VSF_ERR_ALREADY_EXISTS;
                                goto __exit;
                            }
                            vsf_local.offset_in_sector += sizeof(*cur);
                        }
                    }

#if VSF_FS_USE_EXFATFS == ENABLED
                    if ((VSF_FAT_EX == fsinfo->type) && vsf_local.slot_found) {
                        // a long-enough free run is all exfat needs, later
                        // entries cannot conflict(lookup already checked)
                        goto exfat_build_and_write;
                    }
#endif
                    if (    !vsf_local.scan_terminated
                        &&  (vsf_local.sector_offset_in_cluster < ((1 << fsinfo->cluster_size_bits) - 1))) {
                        vsf_local.sector_offset_in_cluster++;
                        vsf_local.cur_sector++;
                        vsf_local.offset_in_sector = 0;
                        vsf_local.state = CREATE_STATE_SCAN_SECTOR;
                        __vk_malfs_read(malfs_info, vsf_local.cur_sector, 1, NULL);
                        break;
                    }
                    // hop the chain; a terminated scan only walks on to
                    // identify the chain tail
                    vsf_local.last_cluster = vsf_local.cur_cluster;
                    vsf_local.state = CREATE_STATE_SCAN_FAT;
                    __vsf_component_call_peda(__vk_fatfs_get_fat_entry, err, fsinfo,
                        .cluster     = vsf_local.cur_cluster,
                        .entry       = &vsf_local.cur_cluster,
                    );
                    if (VSF_ERR_NONE != err) {
                        goto __exit;
                    }
                }
                break;
#if VSF_FS_USE_EXFATFS == ENABLED
            exfat_build_and_write: {
                    fatfs_dentry_t *set;

                    // assemble the File/Stream/Name set, then write it out
                    // sector by sector(it may straddle sector and cluster
                    // boundaries)
                    vsf_local.set_buff = vsf_heap_malloc(vsf_local.set_entry_num * sizeof(fatfs_dentry_t));
                    if (NULL == vsf_local.set_buff) {
                        err = VSF_ERR_NOT_ENOUGH_RESOURCES;
                        goto __exit;
                    }
                    set = (fatfs_dentry_t *)vsf_local.set_buff;
                    memset(set, 0, vsf_local.set_entry_num * sizeof(fatfs_dentry_t));

                    set[0].exfat.EntryType = EXFAT_ET_FILE_DIR;
                    set[0].exfat.FilDir.SecondaryCount = vsf_local.set_entry_num - 1;
                    set[0].exfat.FilDir.FileAttributes = cpu_to_le16(FAT_ATTR_ARCHIVE);

                    set[1].exfat.EntryType = EXFAT_ET_STREAM;
                    set[1].exfat.Stream.GeneralSecondaryFlags = 1;  // AllocationPossible, FAT chain
                    set[1].exfat.Stream.NameLength = vsf_local.namelen;
                    set[1].exfat.Stream.NameHash = cpu_to_le16(vsf_local.name_hash);
                    // FirstCluster/ValidDataLength/DataLength stay 0: the
                    // first write allocates and fills them via setsize

                    for (uint8_t i = 0; i < (vsf_local.set_entry_num - 2); i++) {
                        set[2 + i].exfat.EntryType = EXFAT_ET_FILENAME;
                    }
                    for (uint8_t i = 0; i < vsf_local.namelen; i++) {
                        set[2 + i / 15].exfat.FileName.FileName[i % 15] = cpu_to_le16(vsf_local.utf16_name[i]);
                    }
                    // checksum over the whole set, excluding its own field
                    set[0].exfat.FilDir.SetChecksum = cpu_to_le16(__vk_exfatfs_entry_checksum(
                        0, vsf_local.set_buff, vsf_local.set_entry_num << 5, true));

                    vsf_local.set_off = 0;
                    vsf_local.sector_offset_in_cluster = vsf_local.slot_sector_in_cluster;
                    vsf_local.state = CREATE_STATE_EX_WRITE_READ;
                    __vk_malfs_read(malfs_info, vsf_local.slot_sector, 1, NULL);
                }
                break;
#endif
            case CREATE_STATE_SCAN_FAT: {
                    bool need_extend = !vsf_local.slot_found;

                    if (    (VSF_ERR_NONE != (vsf_err_t)result.value)
                        ||  !__vk_fatfs_fat_entry_is_valid(fsinfo, vsf_local.cur_cluster)
                        ||  __vk_fatfs_fat_entry_is_eof(fsinfo, vsf_local.cur_cluster)) {
                        // chain end. FAT32: a new entry must not take the very
                        // last slot without leaving a 0x00 terminator behind;
                        // exFAT has no terminator requirement, growing is only
                        // needed when no free run was found at all
                        if (    !need_extend
                            &&  (VSF_FAT_32 == fsinfo->type)
                            &&  (vsf_local.slot_cluster == vsf_local.last_cluster)
                            &&  (vsf_local.slot_sector_in_cluster == ((1 << fsinfo->cluster_size_bits) - 1))
                            &&  (vsf_local.slot_offset == sector_size - sizeof(fatfs_dentry_t))) {
                            need_extend = true;
                        }
                        if (need_extend) {
                            vsf_local.state = CREATE_STATE_ALLOC;
                            __vsf_component_call_peda(__vk_fatfs_append_fat_entry, err, fsinfo,
                                .cluster     = vsf_local.last_cluster,
                                .entry       = &vsf_local.cur_cluster,
                            );
                            if (VSF_ERR_NONE != err) {
                                goto __exit;
                            }
                        }
#if VSF_FS_USE_EXFATFS == ENABLED
                        else if (VSF_FAT_EX == fsinfo->type) {
                            goto exfat_build_and_write;
                        }
#endif
                        else {
                            vsf_local.state = CREATE_STATE_READ_SLOT;
                            __vk_malfs_read(malfs_info, vsf_local.slot_sector, 1, NULL);
                        }
                    } else {
                    vsf_local.cur_cluster &= 0x0FFFFFFF;
                    vsf_local.last_cluster = vsf_local.cur_cluster;
                    if (!vsf_local.scan_terminated) {
                        vsf_local.cur_sector = __vk_fatfs_clus2sec(fsinfo, vsf_local.cur_cluster);
                        vsf_local.sector_offset_in_cluster = 0;
                        vsf_local.offset_in_sector = 0;
                        vsf_local.state = CREATE_STATE_SCAN_SECTOR;
                        __vk_malfs_read(malfs_info, vsf_local.cur_sector, 1, NULL);
                    } else {
                        vsf_local.state = CREATE_STATE_SCAN_FAT;
                        __vsf_component_call_peda(__vk_fatfs_get_fat_entry, err, fsinfo,
                            .cluster     = vsf_local.cur_cluster,
                            .entry       = &vsf_local.cur_cluster,
                        );
                        if (VSF_ERR_NONE != err) {
                            goto __exit;
                        }
                    }
                }
                }
                break;
            case CREATE_STATE_ALLOC:
                if (VSF_ERR_NONE != (vsf_err_t)result.value) {
                    err = VSF_ERR_FAIL;
                    goto __exit;
                }
                // cur_cluster is the freshly linked cluster: zero it out so
                // the new entry lands in clean space with a terminator after
                vsf_local.zero_buff = vsf_heap_malloc(sector_size);
                if (NULL == vsf_local.zero_buff) {
                    err = VSF_ERR_NOT_ENOUGH_RESOURCES;
                    goto __exit;
                }
                memset(vsf_local.zero_buff, 0, sector_size);
                vsf_local.zero_sector = __vk_fatfs_clus2sec(fsinfo, vsf_local.cur_cluster);
                vsf_local.zero_remain = 1 << fsinfo->cluster_size_bits;
                vsf_local.state = CREATE_STATE_ZERO;
                __vk_malfs_write(malfs_info, vsf_local.zero_sector, 1, vsf_local.zero_buff);
                break;
            case CREATE_STATE_ZERO:
                if (result.written_size != (int32_t)sector_size) {
                    err = VSF_ERR_FAIL;
                    goto __exit;
                }
                vsf_local.zero_sector++;
                if (--vsf_local.zero_remain > 0) {
                    vsf_local.state = CREATE_STATE_ZERO;
                    __vk_malfs_write(malfs_info, vsf_local.zero_sector, 1, vsf_local.zero_buff);
                    break;
                }
                vsf_heap_free(vsf_local.zero_buff);
                vsf_local.zero_buff = NULL;
                if (!vsf_local.slot_found) {
                    // the extension itself provides the slot
                    vsf_local.slot_sector = __vk_fatfs_clus2sec(fsinfo, vsf_local.cur_cluster);
                    vsf_local.slot_offset = 0;
                    vsf_local.slot_sector_in_cluster = 0;
                    vsf_local.slot_cluster = vsf_local.cur_cluster;
                }
#if VSF_FS_USE_EXFATFS == ENABLED
                if (VSF_FAT_EX == fsinfo->type) {
                    goto exfat_build_and_write;
                }
#endif
                vsf_local.state = CREATE_STATE_READ_SLOT;
                __vk_malfs_read(malfs_info, vsf_local.slot_sector, 1, NULL);
                break;
            case CREATE_STATE_READ_SLOT: {
                    fatfs_dentry_t *dentry = (fatfs_dentry_t *)result.buffer;
                    if (NULL == dentry) {
                        err = VSF_ERR_FAIL;
                        goto __exit;
                    }
                    dentry = (fatfs_dentry_t *)((uint8_t *)dentry + vsf_local.slot_offset);
                    memset(dentry, 0, sizeof(*dentry));
                    // Name[8] and Ext[3] are adjacent in the packed entry
                    memcpy(dentry->fat.Name, vsf_local.sfn, 11);
                    dentry->fat.Attr = FAT_ATTR_ARCHIVE;
                    dentry->fat.LCase = vsf_local.lcase;
                    dentry->fat.CrtData = cpu_to_le16(VSF_FATFS_CFG_DEFAULT_DATE);
                    dentry->fat.LstAccData = dentry->fat.CrtData;
                    dentry->fat.WrtData = dentry->fat.CrtData;
                    vsf_local.state = CREATE_STATE_WRITE_SLOT;
                    __vk_malfs_write(malfs_info, vsf_local.slot_sector, 1, result.buffer);
                }
                break;
            case CREATE_STATE_WRITE_SLOT:
                if (result.written_size != (int32_t)sector_size) {
                    err = VSF_ERR_FAIL;
                    goto __exit;
                }
            flush_fsinfo:
                vsf_local.state = CREATE_STATE_FLUSH_FSINFO;
                __vsf_component_call_peda(__vk_fatfs_flush_fsinfo, err, fsinfo);
                if (VSF_ERR_NONE != err) {
                    err = VSF_ERR_NONE;
                    goto __exit;
                }
                break;
            case CREATE_STATE_FLUSH_FSINFO:
                err = VSF_ERR_NONE;
                goto __exit;
#if VSF_FS_USE_EXFATFS == ENABLED
            case CREATE_STATE_EX_WRITE_READ: {
                    uint8_t *buff = result.buffer;
                    uint32_t n;

                    if (NULL == buff) {
                        err = VSF_ERR_FAIL;
                        goto __exit;
                    }
                    n = (uint32_t)(vsf_local.set_entry_num << 5) - vsf_local.set_off;
                    n = vsf_min(n, sector_size - vsf_local.slot_offset);
                    memcpy(buff + vsf_local.slot_offset, vsf_local.set_buff + vsf_local.set_off, n);
                    vsf_local.set_off += n;
                    vsf_local.zero_remain = n;          // bytes in flight
                    vsf_local.state = CREATE_STATE_EX_WRITE_WRITE;
                    __vk_malfs_write(malfs_info, vsf_local.slot_sector, 1, buff);
                }
                break;
            case CREATE_STATE_EX_WRITE_WRITE:
                if (result.written_size != (int32_t)sector_size) {
                    err = VSF_ERR_FAIL;
                    goto __exit;
                }
                vsf_local.slot_offset += vsf_local.zero_remain;
                if (vsf_local.set_off >= (uint16_t)(vsf_local.set_entry_num << 5)) {
                    // entry set fully on disk
                    vsf_heap_free(vsf_local.set_buff);
                    vsf_local.set_buff = NULL;
                    vsf_heap_free(vsf_local.utf16_name);
                    vsf_local.utf16_name = NULL;
                    goto flush_fsinfo;
                }
                if (vsf_local.slot_offset >= sector_size) {
                    vsf_local.slot_offset = 0;
                    if (++vsf_local.sector_offset_in_cluster >= (1 << fsinfo->cluster_size_bits)) {
                        // the set continues in the next directory cluster
                        vsf_local.sector_offset_in_cluster = 0;
                        vsf_local.state = CREATE_STATE_EX_WRITE_NEXT;
                        __vsf_component_call_peda(__vk_fatfs_get_fat_entry, err, fsinfo,
                            .cluster     = vsf_local.slot_cluster,
                            .entry       = &vsf_local.slot_cluster,
                        );
                        if (VSF_ERR_NONE != err) {
                            goto __exit;
                        }
                        break;
                    }
                    vsf_local.slot_sector++;
                }
                vsf_local.state = CREATE_STATE_EX_WRITE_READ;
                __vk_malfs_read(malfs_info, vsf_local.slot_sector, 1, NULL);
                break;
            case CREATE_STATE_EX_WRITE_NEXT:
                if (    (VSF_ERR_NONE != (vsf_err_t)result.value)
                    ||  !__vk_fatfs_fat_entry_is_valid(fsinfo, vsf_local.slot_cluster)
                    ||  __vk_fatfs_fat_entry_is_eof(fsinfo, vsf_local.slot_cluster)) {
                    err = VSF_ERR_FAIL;
                    goto __exit;
                }
                vsf_local.slot_cluster &= 0x0FFFFFFF;
                vsf_local.slot_sector = __vk_fatfs_clus2sec(fsinfo, vsf_local.slot_cluster);
                vsf_local.state = CREATE_STATE_EX_WRITE_READ;
                __vk_malfs_read(malfs_info, vsf_local.slot_sector, 1, NULL);
                break;
#endif
            }
        }
        break;
    }
    return;
__exit:
    if (vsf_local.zero_buff != NULL) {
        vsf_heap_free(vsf_local.zero_buff);
    }
#if VSF_FS_USE_EXFATFS == ENABLED
    if (vsf_local.set_buff != NULL) {
        vsf_heap_free(vsf_local.set_buff);
    }
    if (vsf_local.utf16_name != NULL) {
        vsf_heap_free(vsf_local.utf16_name);
    }
#endif
    vsf_eda_return(err);
#endif
    vsf_peda_end();
}

// FAT32 unlink: lookup locates the entry set(SFN plus any LFN run before it,
// possibly crossing a cluster boundary), every entry gets the 0xE5 deleted
// mark, then the cluster chain is walked once and freed in the FAT
__vsf_component_peda_ifs_entry(__vk_fatfs_unlink, vk_file_unlink,
    implement(vk_fatfs_unlink_local)
) {
    vsf_peda_begin();
#if VSF_USE_HEAP != ENABLED
    VSF_FS_ASSERT(false);
    vsf_eda_return(VSF_ERR_NOT_SUPPORT);
#else
    enum {
        UNLINK_STATE_LOOKUP,
        UNLINK_STATE_MARK_READ,
        UNLINK_STATE_MARK_WRITE,
        UNLINK_STATE_MARK_NEXT,
        UNLINK_STATE_FREE,
        UNLINK_STATE_FREE_SET,
        UNLINK_STATE_BITMAP_READ,
        UNLINK_STATE_BITMAP_WRITE,
        UNLINK_STATE_FLUSH_FSINFO,
    };
    vk_fatfs_file_t *dir = (vk_fatfs_file_t *)&vsf_this;
    __vk_fatfs_info_t *fsinfo = (__vk_fatfs_info_t *)dir->info;
    __vk_malfs_info_t *malfs_info = &fsinfo->use_as____vk_malfs_info_t;
    uint32_t sector_size = 1 << fsinfo->sector_size_bits;
    vsf_err_t err;

    switch (evt) {
    case VSF_EVT_INIT:
        // FAT12/16 have a flat root directory, not written by this cut
        if ((VSF_FAT_32 != fsinfo->type) && (VSF_FAT_EX != fsinfo->type)) {
            vsf_eda_return(VSF_ERR_NOT_SUPPORT);
            break;
        }

        vsf_local.file = NULL;
        vsf_local.state = UNLINK_STATE_LOOKUP;
        __vsf_component_call_peda_ifs(vk_file_lookup, err, dir->fsop->dop.fn_lookup, dir->fsop->dop.lookup_local_size,
            &dir->use_as__vk_file_t,
            .name       = vsf_local.name,
            .result     = &vsf_local.file,
        );
        if (VSF_ERR_NONE != err) {
            vsf_eda_return(err);
        }
        break;
    case VSF_EVT_RETURN: {
            union {
                uintptr_t value;
                vsf_err_t err;
                int32_t written_size;
                uint8_t *buffer;
            } result;
            result.value = vsf_eda_get_return_value();

            switch (vsf_local.state) {
            case UNLINK_STATE_LOOKUP: {
                    vk_fatfs_file_t *file = (vk_fatfs_file_t *)vsf_local.file;
                    if ((NULL == file) || (VSF_ERR_NONE != (vsf_err_t)result.value)) {
                        err = VSF_ERR_NOT_AVAILABLE;
                        goto __exit;
                    }
                    // the set layout comes from lookup's own bookkeeping:
                    // entry_num entries starting at(sector0, entry_offset)
                    vsf_local.cur_sector = file->dentry.sector0;
                    vsf_local.entry_offset = file->dentry.entry_offset_in_sector0;
                    vsf_local.entry_num = file->dentry.entry_num;
                    vsf_local.file_cluster = file->first_cluster;
#if VSF_FS_USE_EXFATFS == ENABLED
                    if (    (VSF_FAT_EX == fsinfo->type)
                        &&  (file->dentry.stream_flags & 0x02)) {
                        uint32_t cluster_size = sector_size << fsinfo->cluster_size_bits;
                        vsf_local.no_fat_chain = true;
                        vsf_local.free_cluster_num = file->dentry.data_length / cluster_size;
                        if (file->dentry.data_length % cluster_size) {
                            vsf_local.free_cluster_num++;
                        }
                    }
#endif
                    vsf_heap_free(file->name);
                    vk_file_free(&file->use_as__vk_file_t);
                    vsf_local.file = NULL;

                    if (0 == vsf_local.entry_num) {
                        err = VSF_ERR_FAIL;
                        goto __exit;
                    }
                    vsf_local.state = UNLINK_STATE_MARK_READ;
                    __vk_malfs_read(malfs_info, vsf_local.cur_sector, 1, NULL);
                }
                break;
            case UNLINK_STATE_MARK_READ: {
                    uint8_t *buff = result.buffer;
                    uint32_t num;
                    if (NULL == buff) {
                        err = VSF_ERR_FAIL;
                        goto __exit;
                    }
                    num = vsf_min(vsf_local.entry_num,
                        (1 << (fsinfo->sector_size_bits - 5)) - vsf_local.entry_offset);
                    for (uint32_t i = 0; i < num; i++) {
                        if (VSF_FAT_EX == fsinfo->type) {
                            // exFAT deletion mark: clear the InUse bit(bit7)
                            // of EntryType and leave the payload unchanged
                            buff[(vsf_local.entry_offset + i) << 5] &= ~0x80;
                        } else {
                            buff[(vsf_local.entry_offset + i) << 5] = 0xE5;
                        }
                    }
                    vsf_local.entry_num -= num;
                    vsf_local.state = UNLINK_STATE_MARK_WRITE;
                    __vk_malfs_write(malfs_info, vsf_local.cur_sector, 1, buff);
                }
                break;
            case UNLINK_STATE_MARK_WRITE:
                if (result.written_size != (int32_t)sector_size) {
                    err = VSF_ERR_FAIL;
                    goto __exit;
                }
                if (vsf_local.entry_num > 0) {
                    uint32_t sec_in_clus = ((uint32_t)(vsf_local.cur_sector - fsinfo->data_sector))
                        & ((1 << fsinfo->cluster_size_bits) - 1);
                    if (sec_in_clus < ((1 << fsinfo->cluster_size_bits) - 1)) {
                        vsf_local.cur_sector++;
                        vsf_local.state = UNLINK_STATE_MARK_READ;
                        __vk_malfs_read(malfs_info, vsf_local.cur_sector, 1, NULL);
                        break;
                    }
                    // the entry set continues in the next cluster of the
                    // directory chain
                    vsf_local.state = UNLINK_STATE_MARK_NEXT;
                    __vsf_component_call_peda(__vk_fatfs_get_fat_entry, err, fsinfo,
                        .cluster     = (uint32_t)(((vsf_local.cur_sector - fsinfo->data_sector)
                                        >> fsinfo->cluster_size_bits) + 2),
                        .entry       = &vsf_local.cur_cluster,
                    );
                    if (VSF_ERR_NONE != err) {
                        goto __exit;
                    }
                    break;
                }
                // entries cleared, release the file's cluster chain
                vsf_local.cur_cluster = vsf_local.file_cluster;
                // fall through
            free_next:
#if VSF_FS_USE_EXFATFS == ENABLED
                if ((VSF_FAT_EX == fsinfo->type) && vsf_local.no_fat_chain) {
                    if (0 == vsf_local.free_cluster_num) {
                        goto release_done;
                    }
                    if (    !__vk_fatfs_fat_entry_is_valid(fsinfo, vsf_local.cur_cluster)
                        ||  __vk_fatfs_fat_entry_is_eof(fsinfo, vsf_local.cur_cluster)) {
                        err = VSF_ERR_FAIL;
                        goto __exit;
                    }
                    vsf_local.free_cluster_num--;
                    vsf_local.next_cluster = vsf_local.cur_cluster + 1;
                    goto free_current;
                }
#endif
                if (    __vk_fatfs_fat_entry_is_valid(fsinfo, vsf_local.cur_cluster)
                    &&  !__vk_fatfs_fat_entry_is_eof(fsinfo, vsf_local.cur_cluster)) {
                    vsf_local.state = UNLINK_STATE_FREE;
                    __vsf_component_call_peda(__vk_fatfs_get_fat_entry, err, fsinfo,
                        .cluster     = vsf_local.cur_cluster,
                        .entry       = &vsf_local.next_cluster,
                    );
                    if (VSF_ERR_NONE != err) {
                        goto __exit;
                    }
                } else {
                release_done:
                    // chain released, sync the FSInfo hint(failure of the
                    // flush does not fail the unlink)
                    vsf_local.state = UNLINK_STATE_FLUSH_FSINFO;
                    __vsf_component_call_peda(__vk_fatfs_flush_fsinfo, err, fsinfo);
                    if (VSF_ERR_NONE != err) {
                        err = VSF_ERR_NONE;
                        goto __exit;
                    }
                }
                break;
            case UNLINK_STATE_FLUSH_FSINFO:
                err = VSF_ERR_NONE;
                goto __exit;
            case UNLINK_STATE_MARK_NEXT:
                if (    (VSF_ERR_NONE != (vsf_err_t)result.value)
                    ||  !__vk_fatfs_fat_entry_is_valid(fsinfo, vsf_local.cur_cluster)
                    ||  __vk_fatfs_fat_entry_is_eof(fsinfo, vsf_local.cur_cluster)) {
                    // chain broken in the middle of an entry set
                    err = VSF_ERR_FAIL;
                    goto __exit;
                }
                vsf_local.cur_cluster &= 0x0FFFFFFF;
                vsf_local.cur_sector = __vk_fatfs_clus2sec(fsinfo, vsf_local.cur_cluster);
                vsf_local.entry_offset = 0;
                vsf_local.state = UNLINK_STATE_MARK_READ;
                __vk_malfs_read(malfs_info, vsf_local.cur_sector, 1, NULL);
                break;
            case UNLINK_STATE_FREE:
                if (VSF_ERR_NONE != (vsf_err_t)result.value) {
                    err = VSF_ERR_FAIL;
                    goto __exit;
                }
            free_current:
                // clear the current cluster and advance
                {
                    uint32_t cluster = vsf_local.cur_cluster;
                    vsf_local.cur_cluster = vsf_local.next_cluster;
                    vsf_local.freed_cluster = cluster;
                    vsf_local.state = UNLINK_STATE_FREE_SET;
                    __vsf_component_call_peda(__vk_fatfs_set_fat_entry, err, fsinfo,
                        .cluster         = cluster,
                        .next_cluster    = 0,
                    );
                    if (VSF_ERR_NONE != err) {
                        goto __exit;
                    }
                }
                break;
            case UNLINK_STATE_FREE_SET:
                if (VSF_ERR_NONE != (vsf_err_t)result.value) {
                    err = VSF_ERR_FAIL;
                    goto __exit;
                }
#if VSF_FS_USE_EXFATFS == ENABLED
                if (VSF_FAT_EX == fsinfo->type) {
                    // exfat allocations are tracked in the cluster bitmap:
                    // clear the bit for the just-freed cluster before
                    // walking on(cur_sector is idle in this phase)
                    vsf_local.cur_sector = fsinfo->bitmap_sector
                        + ((vsf_local.freed_cluster - 2) >> (fsinfo->sector_size_bits + 3));
                    if ((uint32_t)vsf_local.cur_sector < (fsinfo->bitmap_sector + fsinfo->bitmap_size)) {
                        vsf_local.state = UNLINK_STATE_BITMAP_READ;
                        __vk_malfs_read(malfs_info, vsf_local.cur_sector, 1, NULL);
                        break;
                    }
                }
#endif
                goto free_next;
#if VSF_FS_USE_EXFATFS == ENABLED
            case UNLINK_STATE_BITMAP_READ: {
                    uint8_t *buff = result.buffer;
                    if (NULL == buff) {
                        err = VSF_ERR_FAIL;
                        goto __exit;
                    }
                    vsf_bitmap_clear(buff, (vsf_local.freed_cluster - 2)
                        & ((1 << (fsinfo->sector_size_bits + 3)) - 1));
                    vsf_local.state = UNLINK_STATE_BITMAP_WRITE;
                    __vk_malfs_write(malfs_info, vsf_local.cur_sector, 1, buff);
                }
                break;
            case UNLINK_STATE_BITMAP_WRITE:
                if (result.written_size != (int32_t)sector_size) {
                    err = VSF_ERR_FAIL;
                    goto __exit;
                }
                goto free_next;
#endif
            }
        }
        break;
    }
    return;
__exit:
    vsf_eda_return(err);
#endif
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_fatfs_close, vk_file_close)
{
    vsf_peda_begin();
#if VSF_USE_HEAP != ENABLED
    VSF_FS_ASSERT(false);
    vsf_eda_return(VSF_ERR_NOT_SUPPORT);
#else
    vk_fatfs_file_t *fatfs_file = (vk_fatfs_file_t *)&vsf_this;

    // TODO: flush file buffer if enabled
    if (fatfs_file->name != NULL) {
        vsf_heap_free(fatfs_file->name);
    }
    vsf_eda_return(VSF_ERR_NONE);
#endif
    vsf_peda_end();
}

// FATFS-local cap for merging FAT-consecutive clusters into one MAL read.
// This batching policy is deliberately independent of the malfs bounce path.
#define __VK_FATFS_READ_RUN_SECTORS_MAX      32

__vsf_component_peda_ifs_entry(__vk_fatfs_read, vk_file_read,
    implement(vk_fatfs_rw_local)
) {
    vsf_peda_begin();
    enum {
        READ_STATE_READ,
        READ_STATE_READ_DONE,
        READ_STATE_GET_NEXT_FAT_ENTRY_DONE,
        READ_STATE_BOUNDARY_WALK_DONE,
        READ_STATE_EXTEND_DONE,
    };

    vk_fatfs_file_t *fatfs_file = (vk_fatfs_file_t *)&vsf_this;
    __vk_fatfs_info_t *fsinfo = (__vk_fatfs_info_t *)fatfs_file->info;
    __vk_malfs_info_t *malfs_info = &fsinfo->use_as____vk_malfs_info_t;
    uint32_t sector_size_bytes = 1 << fsinfo->sector_size_bits;

    switch (evt) {
    case VSF_EVT_INIT:
        if (vsf_local.size > (fatfs_file->size - fatfs_file->pos)) {
            vsf_local.size = fatfs_file->size - fatfs_file->pos;
        }
        vsf_local.offset = fatfs_file->pos;
        vsf_local.cur_size = 0;
        vsf_local.offset_in_sector = vsf_local.offset & (sector_size_bytes - 1);
        if (    vsf_local.size
            &&  !fatfs_file->cur.offset_in_sector
            &&  (fatfs_file->cur.sector_offset_in_cluster == (1 << fsinfo->cluster_size_bits))) {
            // lazy boundary cursor: position sits exactly on a cluster
            // boundary, cur.cluster is the preceding cluster and the FAT
            // walk to the target cluster is deferred to the consumer
            vsf_err_t err;
            vsf_local.state = READ_STATE_BOUNDARY_WALK_DONE;
            __vsf_component_call_peda(__vk_fatfs_get_fat_entry, err, fsinfo,
                .cluster = fatfs_file->cur.cluster,
                .entry = &fatfs_file->cur.cluster,
            );
            VSF_UNUSED_PARAM(err);
            break;
        }
        vsf_local.cur_sector = __vk_fatfs_clus2sec(fsinfo, fatfs_file->cur.cluster);
        vsf_local.cur_sector += fatfs_file->cur.sector_offset_in_cluster;
        vsf_local.state = READ_STATE_READ;
        // fall through
    case VSF_EVT_RETURN: {
            union {
                uintptr_t value;
                vsf_err_t err;
                int_fast32_t size;
                uint8_t *buffer;
            } result;

            result.value = vsf_eda_get_return_value();
            switch (vsf_local.state) {
            case READ_STATE_READ:
            read_next:
                if (vsf_local.size) {
                    uint8_t *buffer;
                    uint32_t cluster_remainder_secs;

                    if (vsf_local.offset_in_sector != 0) {
                        // read first non-page-aligned data
                        vsf_local.cur_run_size = sector_size_bytes - vsf_local.offset_in_sector;
                        vsf_local.cur_run_size = vsf_min(vsf_local.cur_run_size, vsf_local.size);
                        vsf_local.cur_run_sector = 1;
                        buffer = NULL;
                    } else if (vsf_local.size < sector_size_bytes) {
                        // read last non-page-aligned data
                        vsf_local.cur_run_size = vsf_local.size;
                        vsf_local.cur_run_sector = 1;
                        buffer = NULL;
                    } else {
                        // read page-aligned data in cluster
                        // get remain sector in clusrer
                        cluster_remainder_secs = (1 << fsinfo->cluster_size_bits) -
                            ((vsf_local.cur_sector - fsinfo->data_sector) & ((1 << fsinfo->cluster_size_bits) - 1));
                        vsf_local.cur_run_sector = vsf_min(cluster_remainder_secs,
                            vsf_local.size >> fsinfo->sector_size_bits);
                        vsf_local.cur_run_size = vsf_local.cur_run_sector << fsinfo->sector_size_bits;
                        buffer = vsf_local.buff + vsf_local.cur_size;

                        // when the run reaches the cluster end and data remains,
                        // extend it across FAT-consecutive clusters to reduce
                        // the number of MAL operations
                        if (    (vsf_local.cur_run_sector == cluster_remainder_secs)
                            &&  (vsf_local.size > vsf_local.cur_run_size)
                            &&  (vsf_local.cur_run_sector < __VK_FATFS_READ_RUN_SECTORS_MAX)) {
                            vsf_err_t err;
                            vsf_local.run_last_cluster = fatfs_file->cur.cluster;
                            // pre-seed for exFAT's zero-means-next-cluster
                            // convention(get_fat_entry increments *entry)
                            vsf_local.peek_entry = vsf_local.run_last_cluster;
                            vsf_local.state = READ_STATE_EXTEND_DONE;
                            __vsf_component_call_peda(__vk_fatfs_get_fat_entry, err, fsinfo,
                                .cluster = vsf_local.run_last_cluster,
                                .entry = &vsf_local.peek_entry,
                            );
                            VSF_UNUSED_PARAM(err);
                            break;
                        }
                    }
                    vsf_local.state = READ_STATE_READ_DONE;
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
                } else if (vsf_local.size < sector_size_bytes) {
                    uint8_t *dst = vsf_local.buff + vsf_local.cur_size;
                    memcpy(dst, result.buffer, vsf_local.cur_run_size);
                }
                vsf_local.cur_size += vsf_local.cur_run_size;
                vsf_local.offset += vsf_local.cur_run_size;
                vsf_local.size -= vsf_local.cur_run_size;

                {
                    uint32_t new_pos = fatfs_file->cur.offset_in_sector + vsf_local.cur_run_size;
                    uint32_t cluster_secs = 1 << fsinfo->cluster_size_bits;
                    uint32_t chain_pos = fatfs_file->cur.sector_offset_in_cluster
                        + (new_pos >> fsinfo->sector_size_bits);
                    if (chain_pos >= cluster_secs) {
                        // batched multi-cluster run: the extension peeks
                        // verified every hop, so step the cursor across the
                        // verified clusters directly. a run ending exactly on
                        // a cluster boundary stays in the lazy-boundary form
                        // (cur.cluster = preceding cluster) so the existing
                        // walk-on-demand semantics are preserved
                        uint32_t steps = chain_pos >> fsinfo->cluster_size_bits;
                        uint32_t rem = chain_pos & (cluster_secs - 1);
                        if (rem) {
                            fatfs_file->cur.cluster += steps;
                            fatfs_file->cur.sector_offset_in_cluster = rem;
                            fatfs_file->cur.offset_in_sector = 0;
                        } else {
                            fatfs_file->cur.cluster += steps - 1;
                            fatfs_file->cur.sector_offset_in_cluster = cluster_secs;
                            fatfs_file->cur.offset_in_sector = 0;
                        }
                    } else {
                        fatfs_file->cur.offset_in_sector = new_pos % sector_size_bytes;
                        fatfs_file->cur.sector_offset_in_cluster = chain_pos;
                    }
                }

                // get next cluster if necessary
                // check if next sector will be in new cluster
                // walk only while more data follows in this call: a read
                // ending exactly at a cluster boundary keeps the lazy
                // boundary cursor instead(no EOC value left in the cursor)
                if (    vsf_local.size
                    &&  !fatfs_file->cur.offset_in_sector
                    &&  !(fatfs_file->cur.sector_offset_in_cluster & ((1 << fsinfo->cluster_size_bits) - 1))) {
                    vsf_err_t err;
                    vsf_local.state = READ_STATE_GET_NEXT_FAT_ENTRY_DONE;
                    __vsf_component_call_peda(__vk_fatfs_get_fat_entry, err, fsinfo,
                        .cluster = fatfs_file->cur.cluster,
                        .entry = &fatfs_file->cur.cluster,
                    );
                    VSF_UNUSED_PARAM(err);
                    break;
                }
                vsf_local.cur_sector += vsf_local.cur_run_sector;
                goto read_next;
            case READ_STATE_BOUNDARY_WALK_DONE:
                if (result.err < 0) {
                    vsf_eda_return(result.err);
                    break;
                }
                if (    !__vk_fatfs_fat_entry_is_valid(fsinfo, fatfs_file->cur.cluster)
                    ||  __vk_fatfs_fat_entry_is_eof(fsinfo, fatfs_file->cur.cluster)) {
                    // size said data follows, but the chain ends: corrupted file
                    vsf_eda_return(VSF_ERR_FAIL);
                    break;
                }

                // remove MSB 4-bit for 32-bit FAT entry
                fatfs_file->cur.cluster &= 0x0FFFFFFF;
                fatfs_file->cur.sector_offset_in_cluster = 0;
                vsf_local.cur_sector = __vk_fatfs_clus2sec(fsinfo, fatfs_file->cur.cluster);
                vsf_local.state = READ_STATE_READ;
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
            case READ_STATE_EXTEND_DONE:
                if (result.err < 0) {
                    vsf_eda_return(result.err);
                    break;
                }
                {
                    uint32_t cluster_secs = 1 << fsinfo->cluster_size_bits;
                    uint32_t expected = vsf_local.run_last_cluster + 1;
                    if ((vsf_local.peek_entry & 0x0FFFFFFF) == expected) {
                        // FAT-consecutive: merge the next cluster into the
                        // pending run and keep extending while the chain is
                        // straight, the request crosses the new end and the
                        // run stays under the batch cap
                        uint32_t remain_secs = (vsf_local.size >> fsinfo->sector_size_bits)
                            - vsf_local.cur_run_sector;
                        uint32_t add = vsf_min(cluster_secs, remain_secs);
                        add = vsf_min(add, __VK_FATFS_READ_RUN_SECTORS_MAX - vsf_local.cur_run_sector);
                        vsf_local.cur_run_sector += add;
                        vsf_local.cur_run_size = vsf_local.cur_run_sector << fsinfo->sector_size_bits;
                        vsf_local.run_last_cluster = expected;

                        if (    (add == cluster_secs)
                            &&  ((vsf_local.size >> fsinfo->sector_size_bits) > vsf_local.cur_run_sector)
                            &&  (vsf_local.cur_run_sector < __VK_FATFS_READ_RUN_SECTORS_MAX)) {
                            vsf_err_t err;
                            vsf_local.peek_entry = expected;
                            __vsf_component_call_peda(__vk_fatfs_get_fat_entry, err, fsinfo,
                                .cluster = expected,
                                .entry = &vsf_local.peek_entry,
                            );
                            VSF_UNUSED_PARAM(err);
                            break;
                        }
                    }
                }
                vsf_local.state = READ_STATE_READ_DONE;
                __vk_malfs_read(malfs_info, vsf_local.cur_sector, vsf_local.cur_run_sector,
                    vsf_local.buff + vsf_local.cur_size);
                break;
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
        WRITE_STATE_CONVERT_DONE,
        WRITE_STATE_READ_DONE,
        WRITE_STATE_ALLOC_FIRST_CLUSTER,
        WRITE_STATE_WRITE,
        WRITE_STATE_WRITE_DONE,
        WRITE_STATE_GET_NEXT_FAT_ENTRY_DONE,
        WRITE_STATE_DENTRY_SETSIZE_DONE,
        WRITE_STATE_BOUNDARY_GET_DONE,
        WRITE_STATE_BOUNDARY_APPEND_DONE,
    };

    vk_fatfs_file_t *fatfs_file = (vk_fatfs_file_t *)&vsf_this;
    __vk_fatfs_info_t *fsinfo = (__vk_fatfs_info_t *)fatfs_file->info;
    __vk_malfs_info_t *malfs_info = &fsinfo->use_as____vk_malfs_info_t;
    uint32_t sector_size_bytes = 1 << fsinfo->sector_size_bits;

    switch (evt) {
    case VSF_EVT_INIT:
        vsf_local.offset = fatfs_file->pos;
#if VSF_FS_USE_EXFATFS == ENABLED
        if (    (VSF_FAT_EX == fsinfo->type)
            &&  (fatfs_file->dentry.stream_flags & 0x02)
            &&  fatfs_file->dentry.data_length && vsf_local.size
            &&  (vsf_local.offset + vsf_local.size > fatfs_file->size)) {
            vsf_err_t err;
            vsf_local.state = WRITE_STATE_CONVERT_DONE;
            __vsf_component_call_peda(__vk_exfatfs_convert_to_fat_chain, err, fatfs_file);
            if (err != VSF_ERR_NONE) {
                vsf_eda_return(err);
            }
            break;
        }
#endif
    write_start:
        vsf_local.cur_size = 0;
        vsf_local.offset_in_sector = vsf_local.offset & (sector_size_bytes - 1);

        if (0 == fatfs_file->size) {
            uint32_t clustersize = 1 << (fsinfo->cluster_size_bits + fsinfo->sector_size_bits);
            uint32_t size = vsf_min(vsf_local.size, clustersize);
            vsf_local.state = WRITE_STATE_ALLOC_FIRST_CLUSTER;
            vk_file_setsize(&fatfs_file->use_as__vk_file_t, size);
            break;
        } else if (   vsf_local.size
                &&  !fatfs_file->cur.offset_in_sector
                &&  (fatfs_file->cur.sector_offset_in_cluster == (1 << fsinfo->cluster_size_bits))) {
            // lazy boundary cursor: position sits exactly on a cluster
            // boundary, cur.cluster is the preceding cluster. get before
            // append so an already linked tail cluster is reused
            vsf_err_t err;
            vsf_local.boundary_cluster = fatfs_file->cur.cluster;
            vsf_local.state = WRITE_STATE_BOUNDARY_GET_DONE;
            __vsf_component_call_peda(__vk_fatfs_get_fat_entry, err, fsinfo,
                .cluster = fatfs_file->cur.cluster,
                .entry = &fatfs_file->cur.cluster,
            );
            VSF_UNUSED_PARAM(err);
            break;
        } else {
            vsf_local.cur_sector = __vk_fatfs_clus2sec(fsinfo, fatfs_file->cur.cluster);
            vsf_local.cur_sector += fatfs_file->cur.sector_offset_in_cluster;
            vsf_local.state = WRITE_STATE_WRITE;
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
            switch (vsf_local.state) {
#if VSF_FS_USE_EXFATFS == ENABLED
            case WRITE_STATE_CONVERT_DONE:
                if (result.err != VSF_ERR_NONE) {
                    vsf_eda_return(result.err);
                    break;
                }
                goto write_start;
#endif
            case WRITE_STATE_READ_DONE:
                if (NULL == result.buffer) {
                    vsf_eda_return(VSF_ERR_FAIL);
                    break;
                }
                if (vsf_local.offset_in_sector != 0) {
                    uint8_t *dst = result.buffer + vsf_local.offset_in_sector;
                    memcpy(dst, vsf_local.buff, vsf_local.cur_run_size);
                    vsf_local.offset_in_sector = 0;
                } else if (vsf_local.size < sector_size_bytes) {
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
                        vsf_local.cur_run_size = sector_size_bytes - vsf_local.offset_in_sector;
                        vsf_local.cur_run_size = vsf_min(vsf_local.cur_run_size, vsf_local.size);
                        vsf_local.cur_run_sector = 1;
                        goto read_page;
                    } else if (vsf_local.size < sector_size_bytes) {
                        // read last non-page-aligned data
                        vsf_local.cur_run_size = vsf_local.size;
                        vsf_local.cur_run_sector = 1;
                    read_page:
                        vsf_local.state = WRITE_STATE_READ_DONE;
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
                        vsf_local.state = WRITE_STATE_WRITE_DONE;
                        __vk_malfs_write(malfs_info, vsf_local.cur_sector, vsf_local.cur_run_sector, result.buffer);
                    }
                    break;
                } else if (vsf_local.offset > fatfs_file->size) {
                    vsf_err_t err;
                    uint64_t data_length = vsf_local.offset;
#if VSF_FS_USE_EXFATFS == ENABLED
                    if (    (VSF_FAT_EX == fsinfo->type)
                        &&  (data_length < fatfs_file->dentry.data_length)) {
                        data_length = fatfs_file->dentry.data_length;
                    }
#endif
                    vsf_local.state = WRITE_STATE_DENTRY_SETSIZE_DONE;
                    __vsf_component_call_peda(__vk_fatfs_dentry_setsize, err, fatfs_file,
                        .size   = vsf_local.offset,
                        .data_length = data_length,
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

                {
                    uint32_t new_pos = fatfs_file->cur.offset_in_sector + vsf_local.cur_run_size;
                    fatfs_file->cur.offset_in_sector = new_pos % sector_size_bytes;
                    fatfs_file->cur.sector_offset_in_cluster += new_pos / sector_size_bytes;
                }

                // get/append next cluster if necessary
                // check if next sector will be in new cluster
                // prefetch only while more data follows in this call: a
                // write ending exactly at a cluster boundary keeps the lazy
                // boundary cursor(cluster = preceding cluster, sector
                // offset == cluster size) without allocating an unused tail
                // cluster beyond the file size
                if (    vsf_local.size
                    &&  !fatfs_file->cur.offset_in_sector
                    &&  !(fatfs_file->cur.sector_offset_in_cluster & ((1 << fsinfo->cluster_size_bits) - 1))) {
                    vsf_err_t err;
                    vsf_local.state = WRITE_STATE_GET_NEXT_FAT_ENTRY_DONE;
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
                    vsf_eda_return(VSF_ERR_FAIL);
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
            case WRITE_STATE_BOUNDARY_GET_DONE:
                if (result.err < 0) {
                    vsf_eda_return(result.err);
                    break;
                }
                if (    __vk_fatfs_fat_entry_is_valid(fsinfo, fatfs_file->cur.cluster)
                    &&  !__vk_fatfs_fat_entry_is_eof(fsinfo, fatfs_file->cur.cluster)) {
                    fatfs_file->cur.cluster &= 0x0FFFFFFF;
                    fatfs_file->cur.sector_offset_in_cluster = 0;
                    vsf_local.cur_sector = __vk_fatfs_clus2sec(fsinfo, fatfs_file->cur.cluster);
                    vsf_local.state = WRITE_STATE_WRITE;
                    goto write_next;
                }
                if (vsf_local.offset < fatfs_file->size) {
                    // size said data follows, but the chain ends: corrupted file
                    vsf_eda_return(VSF_ERR_FAIL);
                    break;
                }
                {
                    vsf_err_t err;
                    fatfs_file->cur.cluster = vsf_local.boundary_cluster;
                    vsf_local.state = WRITE_STATE_BOUNDARY_APPEND_DONE;
                    __vsf_component_call_peda(__vk_fatfs_append_fat_entry, err, fsinfo,
                        .cluster = fatfs_file->cur.cluster,
                        .entry = &fatfs_file->cur.cluster,
                    );
                    VSF_UNUSED_PARAM(err);
                }
                break;
            case WRITE_STATE_BOUNDARY_APPEND_DONE:
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
                vsf_local.state = WRITE_STATE_WRITE;
                goto write_next;
            case WRITE_STATE_DENTRY_SETSIZE_DONE:
                if (result.err != VSF_ERR_NONE) {
                    vsf_eda_return(result.err);
                    break;
                }
#if VSF_FS_USE_EXFATFS == ENABLED
                if (fatfs_file->dentry.data_length < vsf_local.offset) {
                    fatfs_file->dentry.data_length = vsf_local.offset;
                }
                fatfs_file->dentry.stream_flags &= ~0x02;
#endif
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
