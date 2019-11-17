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

#ifndef __VSF_FS_H__
#define __VSF_FS_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_fs_cfg.h"

#if VSF_USE_FS == ENABLED

#if     defined(VSF_FS_IMPLEMENT)
#   undef VSF_FS_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT
#elif   defined(VSF_FS_INHERIT)
#   undef VSF_FS_INHERIT
#   define __PLOOC_CLASS_INHERIT
#endif

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_simple_class(vsf_file_t)

enum vsf_file_attr_t {
    VSF_FILE_ATTR_READONLY      = 1 << 0,
    VSF_FILE_ATTR_HIDDEN        = 1 << 1,
    VSF_FILE_ATTR_SYSTEM        = 1 << 2,
    VSF_FILE_ATTR_VOLUMID       = 1 << 3,
    VSF_FILE_ATTR_DIRECTORY     = 1 << 4,
    VSF_FILE_ATTR_ARCHIVE       = 1 << 5,
    VSF_FILE_ATTR_WRITEONLY     = 1 << 6,
    VSF_FILE_ATTR_EXT           = 1 << 8,
};
typedef enum vsf_file_attr_t vsf_file_attr_t;

struct vsf_fs_fop_t {
    void (*close)(vsf_file_t *file, vsf_evt_t evt);
    void (*read)(vsf_file_t *file, vsf_evt_t evt);
    void (*write)(vsf_file_t *file, vsf_evt_t evt);
};
typedef struct vsf_fs_fop_t vsf_fs_fop_t;

struct vsf_fs_dop_t {
    void (*get_child)(vsf_file_t *file, vsf_evt_t evt);
    void (*add_file)(vsf_file_t *file, vsf_evt_t evt);
    void (*remove_file)(vsf_file_t *file, vsf_evt_t evt);
};
typedef struct vsf_fs_dop_t vsf_fs_dop_t;

struct vsf_fs_op_t {
    void (*format)(vsf_file_t *file, vsf_evt_t evt);
    void (*mount)(vsf_file_t *file, vsf_evt_t evt);
    void (*umount)(vsf_file_t *file, vsf_evt_t evt);
    vsf_fs_fop_t fop;
    vsf_fs_dop_t dop;
};
typedef struct vsf_fs_op_t vsf_fs_op_t;

def_simple_class(vsf_file_t) {
    public_member(
        char *name;
        uint64_t size;
        vsf_file_attr_t attr;
        const vsf_fs_op_t *fs_drv;
        vsf_file_t *parent;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/

#include "./driver/fatfs/vsf_fatfs.h"
#include "./driver/memfs/vsf_memfs.h"

/*============================ PROTOTYPES ====================================*/

extern char * vsf_file_getfileext(char *fname);
extern bool vsf_file_is_div(char ch);

#endif      // VSF_USE_FS
#endif      // __VSF_FS_H__
