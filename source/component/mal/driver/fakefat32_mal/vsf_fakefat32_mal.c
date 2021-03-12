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

#include "../../vsf_mal_cfg.h"

#if VSF_USE_MAL == ENABLED && VSF_MAL_USE_FAKEFAT32_MAL == ENABLED

#define __VSF_MAL_CLASS_INHERIT__
#define __VSF_FS_CLASS_INHERIT__
#define __VSF_FAKEFAT32_MAL_CLASS_IMPLEMENT

// for ctype.h
#include "utilities/vsf_utilities.h"
#include "../../vsf_mal.h"
#include "./vsf_fakefat32_mal.h"

/*============================ MACROS ========================================*/

#define FAT32_FILE_ATTR_LFN                 0x0F
#define FAT32_FILE_ATTR_READONLY            (1 << 0)
#define FAT32_FILE_ATTR_HIDDEN              (1 << 1)
#define FAT32_FILE_ATTR_SYSTEM              (1 << 2)
#define FAT32_FILE_ATTR_VOLUMID             (1 << 3)
#define FAT32_FILE_ATTR_DIRECTORY           (1 << 4)
#define FAT32_FILE_ATTR_ARCHIVE             (1 << 5)

#define FAT32_NAMEATTR_NAMELOWERCASE        0x08
#define FAT32_NAMEATTR_EXTLOWERCASE         0x10

#define FAT32_FAT_FILEEND                   0x0FFFFFFF
#define FAT32_FAT_START                     0x0FFFFFF8
#define FAT32_FAT_INVALID                   0xFFFFFFFF

#define FAKEFAT32_RES_SECTORS               get_unaligned_le16(&__fakefat32_mbr[0x0E])
#define FAKEFAT32_FAT_NUM                   __fakefat32_mbr[0x10]
#define FAKEFAT32_HIDDEN_SECTORS            get_unaligned_le32(&__fakefat32_mbr[0x1C])
#define FAKEFAT32_ROOT_CLUSTER              get_unaligned_le32(&__fakefat32_mbr[0x2C])
#define FAKEFAT32_FSINFO_SECTOR             __fakefat32_mbr[0x30]
#define FAKEFAT32_BACKUP_SECTOR             get_unaligned_le16(&__fakefat32_mbr[0x32])

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

static uint_fast32_t __vk_fakefat32_mal_blksz(vk_mal_t *mal, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op);
static bool __vk_fakefat32_mal_buffer(vk_mal_t *mal, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op, vsf_mem_t *mem);
dcl_vsf_peda_methods(static, __vk_fakefat32_mal_init)
dcl_vsf_peda_methods(static, __vk_fakefat32_mal_fini)
dcl_vsf_peda_methods(static, __vk_fakefat32_mal_read)
dcl_vsf_peda_methods(static, __vk_fakefat32_mal_write)

dcl_vsf_peda_methods(static, __vk_fakefat32_dir_read)
dcl_vsf_peda_methods(static, __vk_fakefat32_dir_write)

/*============================ GLOBAL VARIABLES ==============================*/

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-function-type"
#endif

const vk_mal_drv_t vk_fakefat32_mal_drv = {
    .blksz          = __vk_fakefat32_mal_blksz,
    .buffer         = __vk_fakefat32_mal_buffer,
    .init           = (vsf_peda_evthandler_t)vsf_peda_func(__vk_fakefat32_mal_init),
    .fini           = (vsf_peda_evthandler_t)vsf_peda_func(__vk_fakefat32_mal_fini),
    .read           = (vsf_peda_evthandler_t)vsf_peda_func(__vk_fakefat32_mal_read),
    .write          = (vsf_peda_evthandler_t)vsf_peda_func(__vk_fakefat32_mal_write),
};

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

/*============================ LOCAL VARIABLES ===============================*/

static uint8_t __fakefat32_mbr[512] = {
//00   01   02   03   04   05   06   07   08   09   0A   0B   0C   0D   0E   0F
0xEB,0x58,0x90,0x4D,0x53,0x44,0x4F,0x53,0x35,0x2E,0x30,0x00,0x00,0x00,0x10,0x00,//00
0x02,0x00,0x00,0x00,0x00,0xF8,0x00,0x00,0x3F,0x00,0xFF,0x00,0x40,0x00,0x00,0x00,//01
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x00,//02
0x01,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//03
0x00,0x00,0x29,0x00,0x00,0x00,0x00,0x4E,0x4F,0x20,0x4E,0x41,0x4D,0x45,0x20,0x20,//04
0x20,0x20,0x46,0x41,0x54,0x33,0x32,0x20,0x20,0x20,0x33,0xC9,0x8E,0xD1,0xBC,0xF4,//05
0x7B,0x8E,0xC1,0x8E,0xD9,0xBD,0x00,0x7C,0x88,0x4E,0x02,0x8A,0x56,0x40,0xB4,0x08,//06
0xCD,0x13,0x73,0x05,0xB9,0xFF,0xFF,0x8A,0xF1,0x66,0x0F,0xB6,0xC6,0x40,0x66,0x0F,//07
0xB6,0xD1,0x80,0xE2,0x3F,0xF7,0xE2,0x86,0xCD,0xC0,0xED,0x06,0x41,0x66,0x0F,0xB7,//08
0xC9,0x66,0xF7,0xE1,0x66,0x89,0x46,0xF8,0x83,0x7E,0x16,0x00,0x75,0x38,0x83,0x7E,//09
0x2A,0x00,0x77,0x32,0x66,0x8B,0x46,0x1C,0x66,0x83,0xC0,0x0C,0xBB,0x00,0x80,0xB9,//0A
0x01,0x00,0xE8,0x2B,0x00,0xE9,0x48,0x03,0xA0,0xFA,0x7D,0xB4,0x7D,0x8B,0xF0,0xAC,//0B
0x84,0xC0,0x74,0x17,0x3C,0xFF,0x74,0x09,0xB4,0x0E,0xBB,0x07,0x00,0xCD,0x10,0xEB,//0C
0xEE,0xA0,0xFB,0x7D,0xEB,0xE5,0xA0,0xF9,0x7D,0xEB,0xE0,0x98,0xCD,0x16,0xCD,0x19,//0D
0x66,0x60,0x66,0x3B,0x46,0xF8,0x0F,0x82,0x4A,0x00,0x66,0x6A,0x00,0x66,0x50,0x06,//0E
0x53,0x66,0x68,0x10,0x00,0x01,0x00,0x80,0x7E,0x02,0x00,0x0F,0x85,0x20,0x00,0xB4,//0F
0x41,0xBB,0xAA,0x55,0x8A,0x56,0x40,0xCD,0x13,0x0F,0x82,0x1C,0x00,0x81,0xFB,0x55,//10
0xAA,0x0F,0x85,0x14,0x00,0xF6,0xC1,0x01,0x0F,0x84,0x0D,0x00,0xFE,0x46,0x02,0xB4,//11
0x42,0x8A,0x56,0x40,0x8B,0xF4,0xCD,0x13,0xB0,0xF9,0x66,0x58,0x66,0x58,0x66,0x58,//12
0x66,0x58,0xEB,0x2A,0x66,0x33,0xD2,0x66,0x0F,0xB7,0x4E,0x18,0x66,0xF7,0xF1,0xFE,//13
0xC2,0x8A,0xCA,0x66,0x8B,0xD0,0x66,0xC1,0xEA,0x10,0xF7,0x76,0x1A,0x86,0xD6,0x8A,//14
0x56,0x40,0x8A,0xE8,0xC0,0xE4,0x06,0x0A,0xCC,0xB8,0x01,0x02,0xCD,0x13,0x66,0x61,//15
0x0F,0x82,0x54,0xFF,0x81,0xC3,0x00,0x02,0x66,0x40,0x49,0x0F,0x85,0x71,0xFF,0xC3,//16
0x4E,0x54,0x4C,0x44,0x52,0x20,0x20,0x20,0x20,0x20,0x20,0x00,0x00,0x00,0x00,0x00,//17
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//18
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//19
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0D,0x0A,0x52,0x65,//1A
0x6D,0x6F,0x76,0x65,0x20,0x64,0x69,0x73,0x6B,0x73,0x20,0x6F,0x72,0x20,0x6F,0x74,//1B
0x68,0x65,0x72,0x20,0x6D,0x65,0x64,0x69,0x61,0x2E,0xFF,0x0D,0x0A,0x44,0x69,0x73,//1C
0x6B,0x20,0x65,0x72,0x72,0x6F,0x72,0xFF,0x0D,0x0A,0x50,0x72,0x65,0x73,0x73,0x20,//1D
0x61,0x6E,0x79,0x20,0x6B,0x65,0x79,0x20,0x74,0x6F,0x20,0x72,0x65,0x73,0x74,0x61,//1E
0x72,0x74,0x0D,0x0A,0x00,0x00,0x00,0x00,0x00,0xAC,0xCB,0xD8,0x00,0x00,0x55,0xAA,//1F
};

/*============================ IMPLEMENTATION ================================*/

static char __vk_find_first_alphabet(const char *str)
{
    if (NULL == str) {
        return 0;
    }

    while ((*str != '\0') && !isalpha(*str)) {
        str++;
    }
    return *str;
}

static char * __vk_strncpy_fill(char *dst, const char *src, char fill, uint32_t n)
{
    if (n != 0) {
        char *d = dst;
        const char *s = src;

        if (NULL == s) {
            memset(dst, fill, n);
            return dst;
        }

        do {
            if ((*d++ = *s++) == 0) {
                d--;
                n++;
                while (--n != 0) {
                    *d++ = fill;
                }
                break;
            }
        } while (--n != 0);
    }
    return dst;
}

static char * __vk_memncpy_toupper(char *dst, const char *src, uint32_t n)
{
    if (n != 0) {
        char *d = dst;
        const char *s = src;

        if (NULL == s) {
            return dst;
        }

        do {
            if ((*d++ = (char)toupper(*s++)) == 0) {
                break;
            }
        } while (--n != 0);
    }
    return dst;
}

static uint_fast8_t __vk_fakefat32_calc_attr(uint_fast16_t attr)
{
    uint_fast8_t fat32_attr = 0;
    if (attr & VSF_FAT_FILE_ATTR_VOLUMID) {
        fat32_attr |= FAT32_FILE_ATTR_VOLUMID;
    }
    if ((attr & VSF_FILE_ATTR_READ) && !(attr & VSF_FILE_ATTR_WRITE)) {
        fat32_attr |= FAT32_FILE_ATTR_READONLY;
    }
    if (attr & VSF_FILE_ATTR_HIDDEN) {
        fat32_attr |= FAT32_FILE_ATTR_HIDDEN;
    }
    if (attr & VSF_FAT_FILE_ATTR_SYSTEM) {
        fat32_attr |= FAT32_FILE_ATTR_SYSTEM;
    }
    if (attr & VSF_FILE_ATTR_DIRECTORY) {
        fat32_attr |= FAT32_FILE_ATTR_DIRECTORY;
    }
    if (attr & VSF_FAT_FILE_ATTR_ARCHIVE) {
        fat32_attr |= FAT32_FILE_ATTR_ARCHIVE;
    }
    return fat32_attr;
}

static uint_fast32_t __vk_fakefat32_calc_fat_sectors(vk_fakefat32_mal_t *pthis)
{
    // simple but safe
    return (((pthis->sector_number - FAKEFAT32_HIDDEN_SECTORS) / pthis->sectors_per_cluster) + 1 + FAKEFAT32_ROOT_CLUSTER) * 4 / pthis->sector_size + 1;
}

static vk_fakefat32_file_t * __vk_fakefat32_get_file_by_cluster(
            vk_fakefat32_mal_t *pthis, vk_fakefat32_file_t *cur_file, uint_fast16_t file_num,
            uint32_t cluster)
{
    vk_fakefat32_file_t *result = NULL;
    uint_fast32_t cluster_size = pthis->sector_size * pthis->sectors_per_cluster;
    uint_fast32_t cluster_start, cluster_end, clusters;

    for (int i = 0; i < file_num; i++, cur_file++) {
        clusters = (cur_file->size + cluster_size - 1) / cluster_size;
        cluster_start = cur_file->first_cluster;
        cluster_end = cluster_start + clusters;
        if ((cluster >= cluster_start) && (cluster < cluster_end)) {
            return cur_file;
        }

        if ((cur_file->d.child != NULL) && (cur_file->attr & VSF_FILE_ATTR_DIRECTORY)) {
            result = __vk_fakefat32_get_file_by_cluster(pthis, (vk_fakefat32_file_t *)cur_file->d.child, cur_file->d.child_num, cluster);
            if (result != NULL) {
                return result;
            }
        }
    }
    return NULL;
}

static bool __vk_fakefat32_file_is_lfn(vk_fakefat32_file_t *file)
{
    return vk_fatfs_is_lfn(file->name);
}

static uint32_t __vk_fakefat32_calc_lfn_len(vk_fakefat32_file_t *file)
{
    return file->name ? strlen(file->name) : 0;
}

static void __vk_fakefat32_get_sfn(vk_fakefat32_file_t* file, char sfn[11])
{
    char *ext = vk_file_getfileext(file->name);
    uint_fast8_t extlen = ext ? strlen(ext) : 0;

    memset(sfn, ' ', 11);
    if (!strcmp(file->name, ".")) {
        sfn[0] = '.';
        return;
    } else if (!strcmp(file->name, "..")) {
        sfn[0] = sfn[1] = '.';
        return;
    }

    if (!__vk_fakefat32_file_is_lfn(file)) {
        uint_fast16_t file_name_len = strlen(file->name);
        if (ext) {
            file_name_len -= extlen + 1;
        }
        __vk_memncpy_toupper(sfn, file->name, file_name_len);
    } else {
        uint_fast16_t n = strlen(file->name);
        n = min(n, 6);

        __vk_memncpy_toupper(sfn, file->name, n);
        sfn[n] = '~';
        // TODO: fix file index here, now use 1
        // BUG here if multiple same short names under one directory
        sfn[n + 1] = '1';
    }
    if (ext) {
        extlen = min(extlen, 3);
        __vk_memncpy_toupper((char *)&sfn[8], ext, extlen);
    }
}

static uint_fast32_t __vk_fakefat32_calc_dir_clusters(
            vk_fakefat32_mal_t *pthis, vk_fakefat32_file_t *file)
{
    uint_fast32_t cluster_size = pthis->sector_size * pthis->sectors_per_cluster;
    uint_fast32_t size = 0;
    uint_fast16_t child_num;

    if (!(file->attr & VSF_FILE_ATTR_DIRECTORY) || (NULL == file->d.child)) {
        return 0;
    }

    child_num = file->d.child_num;
    file = (vk_fakefat32_file_t *)file->d.child;
    for (int i = 0; i < child_num; i++, file++) {
        if (    (file->attr != (vk_file_attr_t)VSF_FAT_FILE_ATTR_VOLUMID)
            &&  __vk_fakefat32_file_is_lfn(file)) {
            // one long name can contain 13 unicode max
            size += 0x20 * ((__vk_fakefat32_calc_lfn_len(file) + 12) / 13);
        }
        size += 0x20;
    }
    return (size + cluster_size - 1) / cluster_size;
}

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-function-type"
#endif

static vsf_err_t __vk_fakefat32_init_recursion(vk_fakefat32_mal_t *pthis, vk_fakefat32_file_t *file, uint32_t *cur_cluster)
{
    uint_fast32_t cluster_size = pthis->sector_size * pthis->sectors_per_cluster;
    uint_fast32_t clusters;

//    file->fsop = &vsf_memfs_op;
    file->fsop = (vk_fs_op_t *)1;
    file->d.child_size = sizeof(vk_fakefat32_file_t);
    file->mal = pthis;

    if (file->attr & VSF_FILE_ATTR_DIRECTORY) {
        if (!strcmp(file->name, ".")) {
            clusters = 0;
            file->first_cluster = ((vk_fakefat32_file_t *)file->parent)->first_cluster;
        } else if (!strcmp(file->name, "..")) {
            clusters = 0;
        } else {
            clusters = __vk_fakefat32_calc_dir_clusters(pthis, file);
            file->size = clusters * cluster_size;
        }
        file->callback.read = (vsf_peda_evthandler_t)vsf_peda_func(__vk_fakefat32_dir_read);
        file->callback.write = (vsf_peda_evthandler_t)vsf_peda_func(__vk_fakefat32_dir_write);
    } else if (file->attr == (vk_file_attr_t)VSF_FAT_FILE_ATTR_VOLUMID) {
        clusters = 0;
    } else {
        clusters = ((uint64_t)file->size + cluster_size - 1) / cluster_size;
    }

    if (clusters) {
        file->first_cluster = *cur_cluster;
        file->record.FstClusHI = (file->first_cluster >> 16) & 0xFFFF;
        file->record.FstClusLO = (file->first_cluster >>  0) & 0xFFFF;
        *cur_cluster += clusters;
    }

    if ((file->d.child != NULL) && (file->attr & VSF_FILE_ATTR_DIRECTORY)) {
        vk_fakefat32_file_t *parent = file;
        uint_fast32_t child_num = file->d.child_num;

        file = (vk_fakefat32_file_t *)file->d.child;
        for (uint_fast32_t i = 0; i < child_num; i++, file++) {
            file->parent = (vk_file_t *)parent;
            if (__vk_fakefat32_init_recursion(pthis, file, cur_cluster)) {
                return VSF_ERR_FAIL;
            }
        }
    }
    return VSF_ERR_NONE;
}

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

static vsf_err_t __vk_fakefat32_init(vk_fakefat32_mal_t *pthis)
{
    if (!pthis->root.fsop) {
        uint32_t cur_cluster = FAKEFAT32_ROOT_CLUSTER;

        pthis->root.attr = (vk_file_attr_t)(VSF_FILE_ATTR_DIRECTORY | VSF_FILE_ATTR_READ | VSF_FILE_ATTR_WRITE);
        pthis->root.parent = NULL;
        return __vk_fakefat32_init_recursion(pthis, &pthis->root, &cur_cluster);
    }
    return VSF_ERR_NONE;
}

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#elif   __IS_COMPILER_LLVM__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#endif

vsf_component_peda_ifs_entry(__vk_fakefat32_dir_read, vk_memfs_callback_read)
{
    vsf_peda_begin();
    vk_fakefat32_file_t *file = (vk_fakefat32_file_t *)&vsf_this;
    uint_fast64_t addr = vsf_local.offset;
    uint8_t *buff = vsf_local.buff;

    uint_fast32_t page_size = file->mal->sector_size;
    vk_fakefat32_file_t *file_dir = file;
    uint_fast8_t lfn_index_offset = 0;
    uint_fast16_t child_num;

    memset(buff, 0, page_size);
    if (!(file->attr & VSF_FILE_ATTR_DIRECTORY)) {
        vsf_eda_return(VSF_ERR_FAIL);
        return;
    }

    child_num = file->d.child_num;
    file = (vk_fakefat32_file_t *)file->d.child;
    if (NULL == file) {
        goto return_success;
    }

    for (int i = 0; i < child_num; i++, file++) {
        if (addr) {
            uint_fast32_t current_entry_size;

            if (    (file->attr != (vk_file_attr_t)VSF_FAT_FILE_ATTR_VOLUMID)
                &&  __vk_fakefat32_file_is_lfn(file)) {
                uint_fast32_t lfn_len = __vk_fakefat32_calc_lfn_len(file);
                uint_fast8_t lfn_entry_num = (uint8_t)((lfn_len + 12) / 13);
                current_entry_size = (1 + lfn_entry_num) * 0x20;
            } else {
                current_entry_size = 0x20;
            }
            if (addr < current_entry_size) {
                lfn_index_offset = (uint8_t)(addr / 0x20);
                break;
            }
            addr -= current_entry_size;
        } else if (page_size) {
            char sfn[11];
            bool is_lfn = false;

            if ((vk_file_attr_t)VSF_FAT_FILE_ATTR_VOLUMID == file->attr) {
                // ONLY file->name is valid for volume_id
                // volume_id is 11 characters max
                __vk_strncpy_fill((char *)buff, file->name, ' ', 11);
                buff[11] = __vk_fakefat32_calc_attr((uint_fast16_t)file->attr);
                memset(&buff[12], 0, 20);
                goto fakefat32_dir_read_next;
            }

            // generate short 8.3 filename
            __vk_fakefat32_get_sfn(file, sfn);

            if (__vk_fakefat32_file_is_lfn(file)) {
                // process entries for long file name
                uint_fast32_t lfn_len = __vk_fakefat32_calc_lfn_len(file);
                uint_fast8_t lfn_entry_num = (uint8_t)((lfn_len + 12) / 13);
                uint_fast8_t lfn_index;
                uint_fast32_t i, j;

                is_lfn = true;
                lfn_index = lfn_entry_num;
                if (lfn_index >= 0x40) {
                    lfn_index = 0x3F;
                }
                if (!lfn_index_offset) {
                    lfn_index |= 0x40;
                } else {
                    lfn_index -= lfn_index_offset;
                    lfn_index_offset = 0;
                }

                while (page_size && lfn_index) {
                    uint8_t checksum = 0;

                    memset(buff, 0xFF, 0x20);
                    buff[0x0B] = FAT32_FILE_ATTR_LFN;
                    buff[0x0C] = buff[0x1A] = buff[0x1B] = 0;

                    for (i = 0; i < 11; i++) {
                        checksum = ((checksum << 7) | (checksum >> 1)) + sfn[i];
                    }
                    buff[0x0D] = checksum;

                    buff[0] = lfn_index;
                    j = 1;
                    i = 13 * ((lfn_index & ~0x40) - 1);
                    while ((i < lfn_len) && (j < 32)) {
                        buff[j] = file->name[i];
                        buff[j + 1] = 0;
                        j += 2;
                        if (0x0B == j) {
                            j = 0x0E;
                        } else if (0x1A == j) {
                            j = 0x1C;
                        }
                        i++;
                    }

                    if (j < 32) {
                        buff[j] = buff[j + 1] = 0;
                    }

                    lfn_index &= ~0x40;
                    lfn_index--;
                    buff += 0x20;
                    page_size -= 0x20;
                }

                if (!page_size) {
                    break;
                }
            }

            // 8.3 file name
            memcpy(&buff[0], sfn, 11);

            // File Attribute
            buff[11] = __vk_fakefat32_calc_attr((uint_fast16_t)file->attr);

            // File Nt Attribute
            buff[12] = 0;
            if (!is_lfn) {
                char *ext = vk_file_getfileext(file->name);
                if (islower(__vk_find_first_alphabet(file->name))) {
                    buff[12] |= FAT32_NAMEATTR_NAMELOWERCASE;
                }
                if (islower(__vk_find_first_alphabet(ext))) {
                    buff[12] |= FAT32_NAMEATTR_EXTLOWERCASE;
                }
            }

            // File Size
            if (file->attr & VSF_FILE_ATTR_DIRECTORY) {
                // File Size for directory SHOULD be 0
                put_unaligned_le32(0, &buff[28]);
            } else {
                put_unaligned_le32(file->size, &buff[28]);
            }

            // fix for current directory
            if (!strcmp(file->name, ".")) {
                file->record.FstClusHI = (file_dir->first_cluster >> 16) & 0xFFFF;
                file->record.FstClusLO = (file_dir->first_cluster >>  0) & 0xFFFF;
            } else if ( !strcmp(file->name, "..")
                    &&  (file_dir->parent != NULL)
                    &&  (file_dir->parent->parent != NULL)) {// if parent->parent is NULL, parent is root dir
                // fix for parent directory
                file->record.FstClusHI = (((vk_fakefat32_file_t *)(file_dir->parent))->first_cluster >> 16) & 0xFFFF;
                file->record.FstClusLO = (((vk_fakefat32_file_t *)(file_dir->parent))->first_cluster >>  0) & 0xFFFF;
            }

            // record
            memcpy(&buff[13], &file->record, sizeof(file->record));

        fakefat32_dir_read_next:
            buff += 0x20;
            page_size -= 0x20;
        }
    }
return_success:
    vsf_eda_return(vsf_local.size);
    vsf_peda_end();
}

vsf_component_peda_ifs_entry(__vk_fakefat32_dir_write, vk_memfs_callback_write)
{
    vsf_peda_begin();
    vk_fakefat32_file_t *file = (vk_fakefat32_file_t *)&vsf_this;
    uint8_t *buff = vsf_local.buff;
    uint_fast16_t child_num;

    uint_fast32_t page_size = file->mal->sector_size;
    vk_fakefat32_file_t *file_temp, *file_match;
    uint8_t *entry;
    uint_fast32_t want_size;
    uint_fast16_t want_first_cluster;
    vk_fatfs_dentry_parser_t dparser;

    child_num = file->d.child_num;
    file = (vk_fakefat32_file_t *)file->d.child;
    dparser.entry = buff;
    dparser.entry_num = page_size >> 5;
    dparser.lfn = 0;
    dparser.filename = (char *)buff;
    while (dparser.entry_num) {
        if (vk_fatfs_parse_dentry_fat(&dparser)) {
            entry = dparser.entry;
            file_temp = file;
            file_match = NULL;
            for (int i = 0; i < child_num; i++, file_temp++) {
                if (!strcmp(file_temp->name, dparser.filename)) {
                    file_match = file_temp;
                    break;
                }
            }
            // seems host add some file, just ignore it
            if (NULL == file_match) {
                goto fakefat32_dir_write_next;
            }

            want_size = get_unaligned_le32(&entry[28]);
            want_first_cluster =
                get_unaligned_le16(&entry[26]) + (get_unaligned_le16(&entry[20]) << 16);

            // host can change the size and first_cluster
            // ONLY one limitation:
            //         host MUST guarantee that the space is continuous
            if (!(file_temp->attr & VSF_FILE_ATTR_DIRECTORY) && (file_temp->size != want_size)) {
/*                if ((file_match->callback.change_size != NULL) &&
                    file_match->callback.change_size(file_match, want_size)) {
                    return VSF_ERR_FAIL;
                }
*/                file_temp->size = want_size;
            }
            file_match->first_cluster = want_first_cluster;
            memcpy(&file_match->record, &entry[13], sizeof(file_match->record));

fakefat32_dir_write_next:
            dparser.entry += 32;
            dparser.filename = (char *)buff;
        } else {
            break;
        }
    }
    vsf_eda_return(vsf_local.size);
    vsf_peda_end();
}

static vsf_err_t __vk_fakefat32_read(vk_fakefat32_mal_t *pthis, uint_fast64_t addr, uint8_t *buff)
{
    uint_fast32_t page_size = pthis->sector_size;
    uint_fast32_t block_addr = addr / page_size;
    uint_fast32_t fat_sectors = __vk_fakefat32_calc_fat_sectors(pthis);
    uint_fast32_t cluster_size = pthis->sectors_per_cluster * pthis->sector_size;
    uint_fast32_t root_cluster = FAKEFAT32_ROOT_CLUSTER;

    if (block_addr < (FAKEFAT32_HIDDEN_SECTORS + FAKEFAT32_RES_SECTORS + FAKEFAT32_FAT_NUM * fat_sectors)) {
        memset(buff, 0, page_size);
    }

    // first sector and first backup copy of boot sector
    if ((FAKEFAT32_HIDDEN_SECTORS > 0) && (0 == block_addr)) {
        // DPT
        // Only PTE1 is valid

        // Disk ID
        put_unaligned_le32(pthis->disk_id, &buff[0x1B8]);

        // PTE1
        // Status: Active
        buff[0x1BE] = 0x80;
        // CHS address of first sector
        buff[0x1BF] = 0x01;
        buff[0x1C0] = 0x01;
        buff[0x1C1] = 0x00;
        // Partition type: FAT32
        buff[0x1C2] = 0x0B;
        // CHS address of last sector
        buff[0x1C3] = 0x01;
        buff[0x1C4] = 0xFF;
        buff[0x1C5] = 0xFF;
        // LBA of first sector in the partition
        put_unaligned_le32(FAKEFAT32_HIDDEN_SECTORS, &buff[0x1C6]);
        // Number of sectors in partition
        put_unaligned_le32(pthis->sector_number - FAKEFAT32_HIDDEN_SECTORS, &buff[0x1CA]);

        // Boot signature
        put_unaligned_le16(0xAA55, &buff[510]);
    } else if ((FAKEFAT32_HIDDEN_SECTORS > 0) && (block_addr < FAKEFAT32_HIDDEN_SECTORS)) {
        // other data in hidden sectors, all 0
    } else if ((FAKEFAT32_HIDDEN_SECTORS == block_addr) || ((FAKEFAT32_HIDDEN_SECTORS + FAKEFAT32_BACKUP_SECTOR) == block_addr)) {
        // MBR
        memcpy(buff, __fakefat32_mbr, sizeof(__fakefat32_mbr));

        // Sector size in bytes
        put_unaligned_le16(pthis->sector_size, &buff[0x0B]);
        // Number of sectors per cluster
        buff[0x0D] = pthis->sectors_per_cluster;
        // Total number of sectors
        put_unaligned_le32(pthis->sector_number - FAKEFAT32_HIDDEN_SECTORS, &buff[0x20]);
        // Number of sectors of one FAT
        put_unaligned_le32(fat_sectors, &buff[0x24]);
        // Volume ID
        put_unaligned_le32(pthis->volume_id, &buff[0x43]);
    } else if ( ((FAKEFAT32_HIDDEN_SECTORS + FAKEFAT32_FSINFO_SECTOR) == block_addr)
            ||  ((FAKEFAT32_HIDDEN_SECTORS + FAKEFAT32_BACKUP_SECTOR + FAKEFAT32_FSINFO_SECTOR) == block_addr)) {
        // FSInfo
        // refer to FAT32 FSInfo sector Structure in FAT32 white paper

        // The lead signature is used to validate that this is in fact an
        // FSInfo sector.
        put_unaligned_le32(0x41615252, &buff[0]);
        // Another signature that is more localized in the sector to the
        // location of the fields that are used.
        put_unaligned_le32(0x61417272, &buff[484]);
        // Contains the last known free cluster count on the volume.
        // If the value is 0xFFFFFFFF, then the free count is unknown and
        // must be computed.
        // Any other value can be used, but is not necessarily correct.
        // Is should be range checked at least to make sure it is <= volume
        // cluser count.
        put_unaligned_le32(0xFFFFFFFF, &buff[488]);
        // This trail signature is used to validate that this is in fact
        // an FSInfo sector.
        // Note that the high 2 bytes of this value match the signature
        // bytes used at the same offsets in sector 0.
        put_unaligned_le16(0xAA55, &buff[510]);
    } else if ( ((FAKEFAT32_HIDDEN_SECTORS + FAKEFAT32_FSINFO_SECTOR + 1) == block_addr)
            ||  ((FAKEFAT32_HIDDEN_SECTORS + FAKEFAT32_BACKUP_SECTOR + FAKEFAT32_FSINFO_SECTOR + 1) == block_addr)) {
        // empty sector, with only Boot sector signature
        put_unaligned_le16(0xAA55, &buff[510]);
    } else if (block_addr < (FAKEFAT32_HIDDEN_SECTORS + FAKEFAT32_RES_SECTORS)) {
        // other reserved sectors, all data is 0
    } else if (block_addr < (FAKEFAT32_FAT_NUM * fat_sectors + FAKEFAT32_RES_SECTORS + FAKEFAT32_HIDDEN_SECTORS)) {
        // FAT
        uint_fast32_t fat_sector = (block_addr - FAKEFAT32_HIDDEN_SECTORS - FAKEFAT32_RES_SECTORS) % fat_sectors;
        uint_fast32_t max_cluster = page_size / 4;
        uint_fast32_t remain_size = page_size;
        uint_fast32_t cluster_index = fat_sector * max_cluster;
        uint32_t *buff32 = (uint32_t *)buff;

        while (remain_size && (cluster_index < root_cluster)) {
            *buff32++ = (0 == cluster_index) ? FAT32_FAT_START : FAT32_FAT_INVALID;
            remain_size -= 4;
            cluster_index++;
        }

        while (remain_size) {
            vk_fakefat32_file_t *file = NULL;

            file = __vk_fakefat32_get_file_by_cluster(pthis, &pthis->root, 1, cluster_index);
            if (NULL == file) {
                // file not found
                *buff32++ = 0;
                remain_size -= 4;
                cluster_index++;
            } else {
                // file found
                uint_fast32_t cluster_offset = cluster_index - file->first_cluster;
                uint_fast32_t file_clusters = ((uint64_t)file->size + cluster_size - 1) / cluster_size;

                while (remain_size && (cluster_offset < file_clusters)) {
                    if (cluster_offset == (file_clusters - 1)) {
                        // last cluster
                        *buff32++ = FAT32_FAT_FILEEND;
                    } else {
                        *buff32++ = cluster_index + 1;
                    }

                    remain_size -= 4;
                    cluster_offset++;
                    cluster_index++;
                }
            }
        }
    } else {
        // Clusters
        uint_fast32_t sectors_to_root = block_addr - FAKEFAT32_HIDDEN_SECTORS - FAKEFAT32_RES_SECTORS - FAKEFAT32_FAT_NUM * fat_sectors;
        uint_fast32_t cluster_index = root_cluster + sectors_to_root / pthis->sectors_per_cluster;
        vk_fakefat32_file_t *file = NULL;

        file = __vk_fakefat32_get_file_by_cluster(pthis, &pthis->root, 1, cluster_index);
        if ((file != NULL) && (file->attr & VSF_FILE_ATTR_READ)) {
            uint_fast32_t addr_offset = pthis->sector_size *
                    (sectors_to_root - pthis->sectors_per_cluster * (file->first_cluster - root_cluster));

            if ((file->f.buff != NULL) && !(file->attr & VSF_FILE_ATTR_DIRECTORY)) {
                memcpy(buff, &file->f.buff[addr_offset], page_size);
            } else if (file->callback.read != NULL) {
                vsf_err_t err;
                __vsf_component_call_peda_ifs(vk_memfs_callback_read, err, file->callback.read, 0, file,
                    .offset     = addr_offset,
                    .size       = page_size,
                    .buff       = buff,
                );
                UNUSED_PARAM(err);
                return VSF_ERR_NOT_READY;
            }
        }
    }
    return VSF_ERR_NONE;
}

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#elif   __IS_COMPILER_LLVM__
#   pragma clang diagnostic pop
#endif

static vsf_err_t __vk_fakefat32_write(vk_fakefat32_mal_t *pthis, uint_fast64_t addr, uint8_t *buff)
{
    uint_fast32_t page_size = pthis->sector_size;
    uint_fast32_t block_addr = addr / page_size;
    uint_fast32_t fat_sectors = __vk_fakefat32_calc_fat_sectors(pthis);
    uint_fast32_t sectors_to_root = block_addr - FAKEFAT32_HIDDEN_SECTORS - FAKEFAT32_RES_SECTORS - FAKEFAT32_FAT_NUM * fat_sectors;
    uint_fast32_t root_cluster = FAKEFAT32_ROOT_CLUSTER;

    uint_fast32_t cluster_index = FAKEFAT32_ROOT_CLUSTER + (block_addr -
                    FAKEFAT32_HIDDEN_SECTORS - FAKEFAT32_RES_SECTORS -
                    FAKEFAT32_FAT_NUM * fat_sectors) / pthis->sectors_per_cluster;
    vk_fakefat32_file_t *file = NULL;

    // Hidden sectors, Reserved sectors, FAT can not be written
    if (block_addr < (FAKEFAT32_HIDDEN_SECTORS + FAKEFAT32_RES_SECTORS + FAKEFAT32_FAT_NUM * fat_sectors)) {
        // first sector and first backup copy of boot sector
        if ((FAKEFAT32_HIDDEN_SECTORS == block_addr) || ((FAKEFAT32_HIDDEN_SECTORS + FAKEFAT32_BACKUP_SECTOR) == block_addr)) {
            memcpy(__fakefat32_mbr, buff, sizeof(__fakefat32_mbr));
        }
        return VSF_ERR_NONE;
    }

    file = __vk_fakefat32_get_file_by_cluster(pthis, &pthis->root, 1, cluster_index);
    if ((file != NULL) && (file->attr & VSF_FILE_ATTR_WRITE)) {
        uint_fast32_t addr_offset = pthis->sector_size *
            (sectors_to_root - pthis->sectors_per_cluster * (file->first_cluster - root_cluster));

        if ((file->f.buff != NULL) && !(file->attr & VSF_FILE_ATTR_DIRECTORY)) {
            memcpy(&file->f.buff[addr_offset], buff, page_size);
        } else if (file->callback.write != NULL) {
            vsf_err_t err;
            __vsf_component_call_peda_ifs(vk_memfs_callback_write, err, file->callback.write, 0, file,
                .offset     = addr_offset,
                .size       = page_size,
                .buff       = buff,
            );
            UNUSED_PARAM(err);
            return VSF_ERR_NOT_READY;
        }
    }
    return VSF_ERR_NONE;
}

static uint_fast32_t __vk_fakefat32_mal_blksz(vk_mal_t *mal, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op)
{
    vk_fakefat32_mal_t *pthis = (vk_fakefat32_mal_t *)mal;
    return pthis->sector_size;
}

static bool __vk_fakefat32_mal_buffer(vk_mal_t *mal, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op, vsf_mem_t *mem)
{
    mem->buffer = NULL;
    mem->size = 0;
    return false;
}

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#elif   __IS_COMPILER_LLVM__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#endif

__vsf_component_peda_ifs_entry(__vk_fakefat32_mal_init, vk_mal_init)
{
    vsf_peda_begin();
    vk_fakefat32_mal_t *pthis = (vk_fakefat32_mal_t *)&vsf_this;

    VSF_MAL_ASSERT(pthis != NULL);
    vsf_eda_return(__vk_fakefat32_init(pthis));
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_fakefat32_mal_fini, vk_mal_fini)
{
    vsf_peda_begin();
    vk_fakefat32_mal_t *pthis = (vk_fakefat32_mal_t *)&vsf_this;

    VSF_MAL_ASSERT(pthis != NULL);
    vsf_eda_return(VSF_ERR_NONE);
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_fakefat32_mal_read, vk_mal_read)
{
    vsf_peda_begin();
    vk_fakefat32_mal_t *pthis = (vk_fakefat32_mal_t *)&vsf_this;

    VSF_MAL_ASSERT(pthis != NULL);

    switch (evt) {
    case VSF_EVT_RETURN: {
            int32_t result = (int32_t)vsf_eda_get_return_value();
            if (result >= 0) {
            read_finish:
                vsf_local.size -= pthis->sector_size;
                vsf_local.addr += pthis->sector_size;
                vsf_local.buff += pthis->sector_size;
                vsf_local.rsize += pthis->sector_size;
            } else {
                vsf_eda_return(result);
                break;
            }
            goto next;
        }
    case VSF_EVT_INIT:
        vsf_local.rsize = 0;
    next:
        if (vsf_local.size > 0) {
            vsf_err_t err = __vk_fakefat32_read(pthis, vsf_local.addr, vsf_local.buff);
            if (VSF_ERR_NONE == err) {
                goto read_finish;
            } else if (VSF_ERR_NOT_READY == err) {
                break;
            } else {
                vsf_eda_return(err);
                break;
            }
        }
        vsf_eda_return(vsf_local.rsize);
        break;
    }
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_fakefat32_mal_write, vk_mal_write)
{
    vsf_peda_begin();
    vk_fakefat32_mal_t *pthis = (vk_fakefat32_mal_t *)&vsf_this;

    VSF_MAL_ASSERT(pthis != NULL);

    switch (evt) {
    case VSF_EVT_RETURN: {
            int32_t result = (int32_t)vsf_eda_get_return_value();
            if (result >= 0) {
            write_finish:
                vsf_local.size -= pthis->sector_size;
                vsf_local.addr += pthis->sector_size;
                vsf_local.buff += pthis->sector_size;
                vsf_local.wsize += pthis->sector_size;
            } else {
                vsf_eda_return(result);
                break;
            }
            goto next;
        }
    case VSF_EVT_INIT:
        vsf_local.wsize = 0;
    next:
        if (vsf_local.size > 0) {
            vsf_err_t err = __vk_fakefat32_write(pthis, vsf_local.addr, vsf_local.buff);
            if (VSF_ERR_NONE == err) {
                goto write_finish;
            } else if (VSF_ERR_NOT_READY == err) {
                break;
            } else {
                vsf_eda_return(err);
                break;
            }
        }
        vsf_eda_return(vsf_local.wsize);
        break;
    }
    vsf_peda_end();
}

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#elif   __IS_COMPILER_LLVM__
#   pragma clang diagnostic pop
#endif

#endif
