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

#ifndef __VSF_FS_H__
#define __VSF_FS_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_fs_cfg.h"

#if VSF_USE_FS == ENABLED

#include "kernel/vsf_kernel.h"

#if     defined(__VSF_FS_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_FS_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#if VSF_FS_CFG_CACHE == ENABLED
#   warning cache is not supported now, disable VSF_FS_CFG_CACHE
#   undef VSF_FS_CFG_CACHE
#   define VSF_FS_CFG_CACHE             DISABLED
#endif

#ifndef VSF_FS_CFG_LOCK
#   define VSF_FS_CFG_LOCK              ENABLED
#endif

#ifndef VSF_FS_CFG_TIME
#   define VSF_FS_CFG_TIME              ENABLED
#endif

#if defined(VSF_FS_CFG_MALLOC) && !defined(VSF_FS_CFG_FREE)
#   error VSF_FS_CFG_FREE must be defined
#endif
#if !defined(VSF_FS_CFG_MALLOC) && defined(VSF_FS_CFG_FREE)
#   error VSF_FS_CFG_MALLOC must be defined
#endif

#ifndef VSF_FS_CFG_MALLOC
#   if VSF_USE_HEAP != ENABLED
#       error heap is used if VSF_FS_CFG_MALLOC is not defined
#   endif
#   define VSF_FS_CFG_MALLOC            vsf_heap_malloc
#   define VSF_FS_CFG_FREE              vsf_heap_free
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

vsf_dcl_class(vk_fs_t)
vsf_dcl_class(vk_file_t)
vsf_dcl_class(vk_vfs_file_t)
#if VSF_USE_SIMPLE_STREAM == ENABLED
vsf_dcl_class(vk_file_stream_t)
#endif

vsf_dcl_class(vk_fs_fop_t)
vsf_dcl_class(vk_fs_dop_t)
vsf_dcl_class(vk_fs_op_t)

vsf_class(vk_fs_fop_t) {
    protected_member(
        uint8_t close_local_size;
        uint8_t read_local_size;
        uint8_t write_local_size;
        uint8_t setsize_local_size;
        uint8_t setpos_local_size;
        vsf_peda_evthandler_t fn_close;
        vsf_peda_evthandler_t fn_read;
        vsf_peda_evthandler_t fn_write;
        vsf_peda_evthandler_t fn_setsize;
        vsf_peda_evthandler_t fn_setpos;
    )
};

vsf_class(vk_fs_dop_t) {
    protected_member(
        uint8_t lookup_local_size;
        uint8_t create_local_size;
        uint8_t unlink_local_size;
        uint8_t chmod_local_size;
        vsf_peda_evthandler_t fn_lookup;
        vsf_peda_evthandler_t fn_create;
        vsf_peda_evthandler_t fn_unlink;
        vsf_peda_evthandler_t fn_chmod;
    )
};

// TODO: remove fop and dop, put everything here for optimization
vsf_class(vk_fs_op_t) {
    protected_member(
        // if succeed, VSF_VFS_FILE_ATTR_MOUNTED should be set in file->attr
        uint8_t mount_local_size;
        uint8_t unmount_local_size;
#if VSF_FS_CFG_USE_CACHE == ENABLED
        uint8_t sync_local_size;
#endif
        uint8_t rename_local_size;
        vsf_peda_evthandler_t fn_mount;
        vsf_peda_evthandler_t fn_unmount;
        vsf_peda_evthandler_t fn_rename;
#if VSF_FS_CFG_USE_CACHE == ENABLED
        vsf_peda_evthandler_t sync;
#endif
        vk_fs_fop_t fop;
        vk_fs_dop_t dop;
    )
};

typedef struct vk_fs_info_t vk_fs_info_t;
vsf_class(vk_file_t) {
    public_member(
        vk_file_attr_t attr;
        char *name;
        uint64_t size;

#if VSF_FS_CFG_TIME == ENABLED
        struct {
            uint64_t create;
            uint64_t access;
            uint64_t modify;
        } time;
#endif
    )

    protected_member(
        const vk_fs_op_t *fsop;
        vk_fs_info_t *fsinfo;

        vk_file_t *parent;
        uint64_t pos;
    )

    private_member(
        uint32_t ref;
    )
};

struct vk_fs_info_t {
    vk_file_t root;
};

#if defined(__VSF_FS_CLASS_INHERIT__) || defined(__VSF_FS_CLASS_IMPLEMENT)
typedef enum vk_vfs_file_attr_t {
    VSF_VFS_FILE_ATTR_MOUNTED   = VSF_FILE_ATTR_EXT,
} vk_vfs_file_attr_t;

vsf_class(vk_vfs_file_t) {
    implement(vk_file_t)
    implement(vsf_dlist_node_t)

    protected_member(
        union {
            struct {
                void *param;
                struct {
                    void (*fn_read)(uintptr_t target, vsf_evt_t evt);
                    void (*fn_write)(uintptr_t target, vsf_evt_t evt);
                } callback;
            } f;
            struct {
                vsf_dlist_t child_list;
            } d;
            struct {
                const vk_fs_op_t *op;
                void *data;
                vk_file_t *root;
            } subfs;
        };
    )
};
#endif

#if VSF_USE_SIMPLE_STREAM == ENABLED
vsf_class(vk_file_stream_t) {
    public_member(
        vk_file_t *file;
    )
    private_member(
        uint32_t size;
        uint32_t rw_size;
        vsf_stream_t *stream;
        uint32_t cur_size;
        uint8_t *cur_buff;
        vsf_eda_t *cur_eda;
    )
};
#endif

#if defined(__VSF_FS_CLASS_INHERIT__) || defined(__VSF_FS_CLASS_IMPLEMENT)
__vsf_component_peda_ifs(vk_fs_mount)
__vsf_component_peda_ifs(vk_fs_unmount)
__vsf_component_peda_ifs(vk_fs_sync)

__vsf_component_peda_ifs(vk_file_create,
    const char          *name;
    vk_file_attr_t      attr;
)
__vsf_component_peda_ifs(vk_file_unlink,
    const char          *name;
)
__vsf_component_peda_ifs(vk_file_lookup,
    const char          *name;
    vk_file_t           **result;
)
__vsf_component_peda_ifs(vk_file_setsize,
    uint64_t            size;
)
__vsf_component_peda_ifs(vk_file_rename,
    vk_file_t           *olddir;
    const char          *oldname;
    vk_file_t           *newdir;
    const char          *newname;
)
__vsf_component_peda_ifs(vk_file_read,
    uint8_t             *buff;
    uint32_t            size;
)
__vsf_component_peda_ifs(vk_file_write,
    uint8_t             *buff;
    uint32_t            size;
)
__vsf_component_peda_ifs(vk_file_setpos,
    uint64_t            offset;
    uint64_t            *result;
)
__vsf_component_peda_ifs(vk_file_close)
__vsf_component_peda_ifs(vk_file_sync)
#endif

/*============================ GLOBAL VARIABLES ==============================*/

extern vk_fs_op_t vk_vfs_op;

/*============================ PROTOTYPES ====================================*/

extern void vk_fs_init(void);

extern vsf_err_t vk_fs_mount(vk_file_t *dir, const vk_fs_op_t *fsop, void *fsdata);
extern vsf_err_t vk_fs_unmount(vk_file_t *dir);
#if VSF_FS_CFG_USE_CACHE == ENABLED
extern vsf_err_t vk_fs_sync(vk_file_t *dir);
#endif

extern vsf_err_t vk_file_open(vk_file_t *dir, const char *name, vk_file_t **file);
extern vsf_err_t vk_file_create(vk_file_t *dir, const char *name, vk_file_attr_t attr);
extern vsf_err_t vk_file_unlink(vk_file_t *dir, const char *name);
extern vsf_err_t vk_file_rename(vk_file_t *olddir, const char *oldname, vk_file_t *newdir, const char *newname);

extern uint32_t vk_file_get_ref(vk_file_t *file);
extern vsf_err_t vk_file_close(vk_file_t *file);
extern uint64_t vk_file_tell(vk_file_t *file);
extern vsf_err_t vk_file_seek(vk_file_t *file, int64_t offset, vk_file_whence_t whence);
extern vsf_err_t vk_file_read(vk_file_t *file, uint8_t *buff, uint_fast32_t size);
extern vsf_err_t vk_file_write(vk_file_t *file, uint8_t *buff, uint_fast32_t size);
extern vsf_err_t vk_file_setsize(vk_file_t *file, uint64_t size);
#if VSF_FS_CFG_USE_CACHE == ENABLED
extern vsf_err_t vk_file_sync(vk_file_t *file);
#endif

extern char * vk_file_getfileext(char *fname);
extern char * vk_file_getfilename(char *path);
extern vk_file_t * vk_file_get_parent(vk_file_t *file);

#if defined(__VSF_FS_CLASS_INHERIT__) || defined(__VSF_FS_CLASS_IMPLEMENT)
extern bool vk_file_is_match(char *path, char *name);
extern bool vk_file_is_div(char ch);

extern vk_file_t * vk_file_alloc(uint_fast16_t size);
extern void vk_file_free(vk_file_t *file);

extern void vk_fs_return(vk_file_t *file, vsf_err_t err);

dcl_vsf_peda_methods(extern, vk_fsop_succeed)
dcl_vsf_peda_methods(extern, vk_fsop_not_support)
#endif

#if VSF_USE_SIMPLE_STREAM == ENABLED
extern vsf_err_t vk_file_read_stream(vk_file_stream_t *pthis, vsf_stream_t *stream, uint_fast32_t size);
extern vsf_err_t vk_file_write_stream(vk_file_stream_t *pthis, vsf_stream_t *stream, uint_fast32_t size);
#endif

#ifdef __cplusplus
}
#endif

/*============================ INCLUDES ======================================*/

#ifndef __VSF_HEADER_ONLY_SHOW_FS_INFO__
#   if VSF_FS_USE_FATFS == ENABLED
#       include "./driver/fatfs/vsf_fatfs.h"
#   endif
#   if VSF_FS_USE_MEMFS == ENABLED
#       include "./driver/memfs/vsf_memfs.h"
#   endif
#   if VSF_FS_USE_WINFS == ENABLED
#       include "./driver/winfs/vsf_winfs.h"
#   endif
#   if VSF_FS_USE_LITTLEFS == ENABLED
#       include "./driver/littlefs/vsf_littlefs.h"
#   endif
#   if VSF_FS_USE_LINFS == ENABLED
#       include "./driver/linfs/vsf_linfs.h"
#   endif
#endif

#undef __VSF_FS_CLASS_IMPLEMENT
#undef __VSF_FS_CLASS_INHERIT__

#endif      // VSF_USE_FS
#endif      // __VSF_FS_H__
