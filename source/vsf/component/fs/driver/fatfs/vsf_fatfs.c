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

#if VSF_USE_FS == ENABLED && VSF_USE_MEMFS == ENABLED

#define VSF_FS_INHERIT
#define VSF_MEMFS_IMPLEMENT

// TODO: use dedicated include
#include "vsf.h"

/*============================ MACROS ========================================*/

#define FAT32_FILE_ATTR_LFN                                                     \
        (VSF_FILE_ATTR_READONLY | VSF_FILE_ATTR_HIDDEN | VSF_FILE_ATTR_SYSTEM | VSF_FILE_ATTR_VOLUMID)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct vsf_fatfs_dentry_t {
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
            uint16_t FstClusHI;
            uint16_t WrtTime;
            uint16_t WrtData;
            uint16_t FstClusLO;
            uint32_t FileSize;
        } fat;
        struct {
            union {
                struct {
                    uint8_t Type;
                    uint8_t NumExt;
                    uint16_t Chksum;
                    uint16_t Attr;
                    uint16_t Reserved1;
                    uint16_t CrtTime;
                    uint16_t CrtData;
                    uint16_t WrtTime;
                    uint16_t WrtData;
                    uint16_t AccTime;
                    uint16_t AddData;
                    uint8_t CrtTimeMs;
                    uint8_t WrtTimeMs;
                    uint8_t AccTimeMs;
                    uint8_t Reserved2[9];
                } file;
                struct {
                    uint8_t Type;
                    uint8_t Flag;
                    uint16_t Uni[15];
                } fname;
                struct {
                    uint8_t Type;
                    uint8_t Reserved1[3];
                    uint32_t Chksum;
                    uint8_t Reserved2[12];
                    uint32_t FstClu;
                    uint64_t Size;
                } casetbl;
                struct {
                    uint8_t Order;
                    uint16_t Uni0[5];
                    uint8_t Attr;
                    uint8_t SysID;
                    uint8_t Chksum;
                    uint16_t Uni5[6];
                    uint16_t FstClus;
                    uint16_t Uni11[2];
                } edir;
                struct {
                    uint8_t Type;
                    uint8_t Flag;
                    uint8_t Reserved1;
                    uint8_t NameLen;
                    uint16_t NameHash;
                    uint16_t Reserved2;
                    uint64_t ValidSize;
                    uint32_t Reserved3;
                    uint32_t FstClus;
                    uint64_t Size;
                } stream;
                struct {
                    uint8_t Type;
                    uint8_t Flag;
                    uint8_t Reserved[18];
                    uint32_t FstClus;
                    uint64_t Size;
                } bmap;
                struct {
                    uint8_t Type;
                    uint8_t LblLen;
                    uint16_t Uni[11];
                    uint8_t Reserved[8];
                } vol;
            };
        } exfat;
    };
} PACKED;
typedef struct vsf_fatfs_dentry_t vsf_fatfs_dentry_t;

/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

bool vsf_fatfs_is_lfn(char *name)
{
    char *ext = NULL;
    bool has_lower = false, has_upper = false;
    uint_fast32_t i, name_len = 0, ext_len = 0;

    if (name != NULL) {
        name_len = strlen(name);
        ext = vsf_file_getfileext(name);
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

// entry_num is the number of entry remain in buffer,
//     and the entry_num of entry for current filename parsed
// lfn is unicode encoded, but we just support ascii
// if a filename parsed, parser->entry will point to the sfn
bool vsf_fatfs_parse_dentry_fat(vsf_fatfs_dentry_parser_t *parser)
{
    vsf_fatfs_dentry_t *entry = (vsf_fatfs_dentry_t *)parser->entry;
    bool parsed = false;

    while (parser->entry_num-- > 0) {
        if (!entry->fat.Name[0]) {
            break;
        } else if (entry->fat.Name[0] != (char)0xE5) {
            char *ptr;
            int i;

            if (entry->fat.Attr == FAT32_FILE_ATTR_LFN) {
                uint_fast8_t index = entry->fat.Name[0];
                uint_fast8_t pos = ((index & 0x0F) - 1) * 13;
                uint8_t *buf;

                parser->lfn = index & 0x0F;
                ptr = parser->filename + pos;
                buf = (uint8_t *)entry + 1;
                for (i = 0; i < 5; i++, buf += 2) {
                    if ((buf[0] == '\0') && (buf[1] == '\0')) {
                        goto parsed;
                    }
                    *ptr++ = (char)buf[0];
                }

                buf = (uint8_t *)entry + 14;
                for (i = 0; i < 6; i++, buf += 2) {
                    if ((buf[0] == '\0') && (buf[1] == '\0')) {
                        goto parsed;
                    }
                    *ptr++ = (char)buf[0];
                }

                buf = (uint8_t *)entry + 28;
                for (i = 0; i < 2; i++, buf += 2) {
                    if ((buf[0] == '\0') && (buf[1] == '\0')) {
                        goto parsed;
                    }
                    *ptr++ = (char)buf[0];
                }

            parsed:
                if ((index & 0xF0) == 0x40) {
                    *ptr = '\0';
                }
            } else if (entry->fat.Attr != VSF_FILE_ATTR_VOLUMID) {
                bool lower;
                if (parser->lfn == 1) {
                    // previous lfn parsed, igure sfn and return
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

#endif
