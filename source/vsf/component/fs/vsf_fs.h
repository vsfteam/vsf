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

#include "kernel/vsf_kernel.h"

#if     defined(VSF_FS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT
#elif   defined(VSF_FS_INHERIT)
#   define __PLOOC_CLASS_INHERIT
#endif

#include "utilities/ooc_class.h"

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

declare_simple_class(vk_fs_t)
declare_simple_class(vk_file_t)
declare_simple_class(vk_vfs_file_t)
#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
declare_simple_class(vk_file_stream_t)
#endif

enum vk_file_attr_t {
    VSF_FILE_ATTR_READ          = 1 << 0,
    VSF_FILE_ATTR_WRITE         = 1 << 1,
    VSF_FILE_ATTR_EXECUTE       = 1 << 2,
    VSF_FILE_ATTR_HIDDEN        = 1 << 3,
    VSF_FILE_ATTR_DIRECTORY     = 1 << 4,
    VSF_FILE_ATTR_DYN           = 1 << 7,
    VSF_FILE_ATTR_EXT           = 1 << 8,
};
typedef enum vk_file_attr_t vk_file_attr_t;

struct vk_fs_fop_t {
    void (*close)(uintptr_t, vsf_evt_t);
    void (*read)(uintptr_t, vsf_evt_t);
    void (*write)(uintptr_t, vsf_evt_t);
    void (*resize)(uintptr_t, vsf_evt_t);
};
typedef struct vk_fs_fop_t vk_fs_fop_t;

struct vk_fs_dop_t {
    void (*lookup)(uintptr_t, vsf_evt_t);
    void (*create)(uintptr_t, vsf_evt_t);
    void (*unlink)(uintptr_t, vsf_evt_t);
    void (*chmod)(uintptr_t, vsf_evt_t);
    void (*rename)(uintptr_t, vsf_evt_t);
};
typedef struct vk_fs_dop_t vk_fs_dop_t;

struct vk_fs_op_t {
    // if succeed, VSF_VFS_FILE_ATTR_MOUNTED should be set in file->attr
    void (*mount)(uintptr_t, vsf_evt_t);
    void (*unmount)(uintptr_t, vsf_evt_t);
#if VSF_FS_CFG_USE_CACHE == ENABLED
    void (*sync)(uintptr_t, vsf_evt_t);
#endif
    vk_fs_fop_t fop;
    vk_fs_dop_t dop;
};
typedef struct vk_fs_op_t vk_fs_op_t;

struct vk_file_ctx_t {
    vsf_err_t err;
    union {
        struct {
            uint64_t offset;
            uint32_t size;
            uint8_t *buff;
            int32_t *result;
        } io;
        struct {
            const char *name;
            vk_file_attr_t attr;
            uint64_t size;
        } create;
        struct {
            const char *name;
        } unlink;
        struct {
            const char *name;
            uint32_t idx;
            vk_file_t **result;
        } lookup;
        struct {
            const char *from_name;
            const char *to_name;
        } rename;
    };
};
typedef struct vk_file_ctx_t vk_file_ctx_t;

// (bytelen << 6) | index
enum vk_file_name_coding_t {
    VSF_FILE_NAME_CODING_UNKNOWN    = 0,
    VSF_FILE_NAME_CODING_ASCII      = (1 << 6) | 0,
    VSF_FILE_NAME_CODING_UCS2       = (2 << 6) | 1,
};
typedef enum vk_file_name_coding_t vk_file_name_coding_t;

def_simple_class(vk_file_t) {
    public_member(
        vk_file_attr_t attr;
        vk_file_name_coding_t coding;
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
        vk_file_t *parent;
        // TODO: use pointer here?
        vk_file_ctx_t ctx;
    )

    private_member(
        uint32_t ref;
    )
};

#if defined(VSF_FS_INHERIT) || defined(VSF_FS_IMPLEMENT)
enum vk_vfs_file_attr_t {
    VSF_VFS_FILE_ATTR_MOUNTED   = VSF_FILE_ATTR_EXT,
};
typedef enum vk_file_attr_t vk_file_attr_t;

def_simple_class(vk_vfs_file_t) {
    implement(vk_file_t)
    implement(vsf_dlist_node_t)

    protected_member(
        union {
            struct {
                void *data;
                struct {
                    void (*read)(uintptr_t target, vsf_evt_t evt);
                    void (*write)(uintptr_t target, vsf_evt_t evt);
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

#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
def_simple_class(vk_file_stream_t) {
    public_member(
        vk_file_t *file;
    )
    protected_member(
        struct {
            uint64_t addr;
            uint32_t size;
            uint32_t rw_size;
            vsf_stream_t *stream;
            uint8_t *cur_buff;
            vsf_eda_t *cur_eda;
        } stream;
    )
};
#endif

/*============================ GLOBAL VARIABLES ==============================*/

extern vk_fs_op_t vk_vfs_op;

/*============================ INCLUDES ======================================*/

#include "./driver/fatfs/vsf_fatfs.h"
#include "./driver/memfs/vsf_memfs.h"
#include "./driver/winfs/vsf_winfs.h"

/*============================ PROTOTYPES ====================================*/

extern void vk_fs_init(void);

extern vsf_err_t vk_fs_mount(vk_file_t *dir, const vk_fs_op_t *fsop, void *fsdata);
extern vsf_err_t vk_fs_unmount(vk_file_t *dir);
#if VSF_FS_CFG_USE_CACHE == ENABLED
extern vsf_err_t vk_fs_sync(vk_file_t *dir);
#endif

extern vsf_err_t vk_file_open(vk_file_t *dir, const char *name, uint_fast16_t idx, vk_file_t **file);
extern vsf_err_t vk_file_create(vk_file_t *dir, const char *name, vk_file_attr_t attr, uint_fast64_t size);
extern vsf_err_t vk_file_unlink(vk_file_t *dir, const char *name);

extern vsf_err_t vk_file_close(vk_file_t *file);
extern vsf_err_t vk_file_read(vk_file_t *file, uint_fast64_t addr, uint_fast32_t size, uint8_t *buff, int32_t *rsize);
extern vsf_err_t vk_file_write(vk_file_t *file, uint_fast64_t addr, uint_fast32_t size, uint8_t *buff, int32_t *wsize);
#if VSF_FS_CFG_USE_CACHE == ENABLED
extern vsf_err_t vk_file_sync(vk_file_t *file);
#endif

extern uint_fast16_t vk_file_get_name_length(vk_file_t *file);

extern char * vk_file_getfileext(char *fname);
extern char * vk_file_getfilename(char *path);

extern vk_file_ctx_t * vk_file_get_ctx(vk_file_t *file);
extern vsf_err_t vk_file_get_errcode(vk_file_t *file);
extern vk_file_t * vk_file_get_parent(vk_file_t *file);

extern void vk_file_set_result(vk_file_t *file, vsf_err_t err);
extern void vk_file_set_io_result(vk_file_t *file, vsf_err_t err, int_fast32_t size);
extern void vk_file_return(vk_file_t *file, vsf_err_t err);

#if defined(VSF_FS_INHERIT) || defined(VSF_FS_IMPLEMENT)
extern bool vk_file_is_match(char *path, char *name);
extern bool vk_file_is_div(char ch);

extern vk_file_t * vk_file_alloc(uint_fast16_t size);
extern void vk_file_free(vk_file_t *file);

extern void vk_fs_return(vk_file_t *file, vsf_err_t err);
extern void vk_dummyfs_succeed(uintptr_t target, vsf_evt_t evt);
extern void vk_dummyfs_not_support(uintptr_t target, vsf_evt_t evt);
#endif

#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
extern vsf_err_t vk_file_read_stream(vk_file_stream_t *pthis, uint_fast64_t addr, uint_fast32_t size, vsf_stream_t *stream);
extern vsf_err_t vk_file_write_stream(vk_file_stream_t *pthis, uint_fast64_t addr, uint_fast32_t size, vsf_stream_t *stream);
#endif

#undef VSF_FS_IMPLEMENT
#undef VSF_FS_INHERIT

#endif      // VSF_USE_FS
#endif      // __VSF_FS_H__
