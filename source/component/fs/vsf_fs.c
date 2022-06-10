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

#include "./vsf_fs_cfg.h"

#if VSF_USE_FS == ENABLED

#if VSF_USE_SIMPLE_STREAM == ENABLED
#   define __VSF_SIMPLE_STREAM_CLASS_INHERIT__
#endif
#define __VSF_FS_CLASS_IMPLEMENT
#define __VSF_EDA_CLASS_INHERIT__
#include "./vsf_fs.h"

/*============================ MACROS ========================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL != ENABLED
#   error VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL is needed to use scsi
#endif

#if VSF_KERNEL_USE_SIMPLE_SHELL != ENABLED
#   error VSF_KERNEL_USE_SIMPLE_SHELL must be enabled
#endif

//#define VSF_FS_REF_TRACE            ENABLED

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct __vk_fs_t {
    struct {
        vsf_crit_t lock;
    } open;
    vk_vfs_file_t rootfs;
} __vk_fs_t;

#if VSF_USE_SIMPLE_STREAM == ENABLED
enum {
    VSF_EVT_FILE_READ       = VSF_EVT_USER + 0,
    VSF_EVT_FILE_WRITE      = VSF_EVT_USER + 1,
};
#endif

/*============================ PROTOTYPES ====================================*/

dcl_vsf_peda_methods(static, __vk_vfs_mount)
dcl_vsf_peda_methods(static, __vk_vfs_unmount)
dcl_vsf_peda_methods(static, __vk_vfs_lookup)
dcl_vsf_peda_methods(static, __vk_vfs_create)
dcl_vsf_peda_methods(static, __vk_vfs_unlink)
dcl_vsf_peda_methods(static, __vk_vfs_rename)
dcl_vsf_peda_methods(static, __vk_vfs_close)
dcl_vsf_peda_methods(static, __vk_vfs_read)
dcl_vsf_peda_methods(static, __vk_vfs_write)
dcl_vsf_peda_methods(static, __vk_vfs_setpos)
dcl_vsf_peda_methods(static, __vk_vfs_setsize)

static vsf_err_t __vk_file_lookup(vk_file_t *dir, const char *name, vk_file_t **file);

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static NO_INIT __vk_fs_t __vk_fs;

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-function-type"
#endif

vk_fs_op_t vk_vfs_op = {
    .fn_mount       = (vsf_peda_evthandler_t)vsf_peda_func(__vk_vfs_mount),
    .fn_unmount     = (vsf_peda_evthandler_t)vsf_peda_func(__vk_vfs_unmount),
#if VSF_FS_CFG_USE_CACHE == ENABLED
    .fn_sync        = (vsf_peda_evthandler_t)vsf_peda_func(vk_fsop_succeed),
#endif
    .fn_rename      = (vsf_peda_evthandler_t)vsf_peda_func(__vk_vfs_rename),
    .fop            = {
        .fn_close   = (vsf_peda_evthandler_t)vsf_peda_func(__vk_vfs_close),
        .fn_read    = (vsf_peda_evthandler_t)vsf_peda_func(__vk_vfs_read),
        .fn_write   = (vsf_peda_evthandler_t)vsf_peda_func(__vk_vfs_write),
        .fn_setsize = (vsf_peda_evthandler_t)vsf_peda_func(__vk_vfs_setsize),
        .fn_setpos  = (vsf_peda_evthandler_t)vsf_peda_func(__vk_vfs_setpos),
    },
    .dop            = {
        .fn_lookup  = (vsf_peda_evthandler_t)vsf_peda_func(__vk_vfs_lookup),
        .fn_create  = (vsf_peda_evthandler_t)vsf_peda_func(__vk_vfs_create),
        .fn_unlink  = (vsf_peda_evthandler_t)vsf_peda_func(__vk_vfs_unlink),
        .fn_chmod   = (vsf_peda_evthandler_t)vsf_peda_func(vk_fsop_not_support),
    },
};

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

/*============================ IMPLEMENTATION ================================*/

// dummy
__vsf_component_peda_public_entry(vk_fsop_succeed)
{
    vsf_peda_begin();
    vsf_eda_return(VSF_ERR_NONE);
    vsf_peda_end();
}

__vsf_component_peda_public_entry(vk_fsop_not_support)
{
    vsf_peda_begin();
    vsf_eda_return(VSF_ERR_NOT_SUPPORT);
    vsf_peda_end();
}

static void __vk_file_ref(vk_file_t *file)
{
    vsf_protect_t orig = vsf_protect_sched();
        file->ref++;
    vsf_unprotect_sched(orig);
#if VSF_FS_REF_TRACE == ENABLED
    vsf_trace_debug("%s: %d\r\n", file->name ? file->name : "ROOT", file->ref);
#endif
}

static uint_fast32_t __vk_file_deref(vk_file_t *file)
{
    uint_fast32_t ref;
    vsf_protect_t orig = vsf_protect_sched();
        ref = --file->ref;
    vsf_unprotect_sched(orig);
#if VSF_FS_REF_TRACE == ENABLED
    vsf_trace_debug("%s: %d\r\n", file->name ? file->name : "ROOT", file->ref);
#endif
    return ref;
}

static void __vk_file_ref_parent(vk_file_t *file)
{
    file = file->parent;
    while (file != NULL) {
        __vk_file_ref(file);
        file = file->parent;
    }
}

static void __vk_file_deref_parent(vk_file_t *file)
{
    file = file->parent;
    while (file != NULL) {
        __vk_file_deref(file);
        file = file->parent;
    }
}

uint32_t vk_file_get_ref(vk_file_t *file)
{
    return (NULL == file) ? 0 : file->ref;
}

vk_file_t * vk_file_alloc(uint_fast16_t size)
{
    vk_file_t *file = VSF_FS_CFG_MALLOC(size);
    if (file != NULL) {
        memset(file, 0, size);
        file->attr |= VSF_FILE_ATTR_DYN;
    }
    return file;
}

void vk_file_free(vk_file_t *file)
{
    if (file->attr & VSF_FILE_ATTR_DYN) {
        VSF_FS_CFG_FREE(file);
    }
}

char * vk_file_getfileext(char *fname)
{
    char *ext = (char *)strrchr(fname, '.');
    if (ext != NULL) {
        ext++;
    }
    return ext;
}

char * vk_file_getfilename(char *path)
{
    char *name0 = (char *)strrchr(path, '\\');
    char *name1 = (char *)strrchr(path, '/');
    char *name = (char *)vsf_max((uintptr_t)name0, (uintptr_t)name1);
    if (NULL == name) {
        name = path;
    } else if (vk_file_is_div(*name)) {
        name++;
    }
    return name;
}

bool vk_file_is_div(char ch)
{
    return ('\\' == ch) || ('/' == ch);
}

bool vk_file_is_match(char *path, char *name)
{
    if (strstr(path, name) == path) {
        char ch = path[strlen(name)];
        if (('\0' == ch) || vk_file_is_div(ch)) {
            return true;
        }
    }
    return false;
}

vk_file_t * vk_file_get_parent(vk_file_t *file)
{
    vk_file_t *parent = file->parent;
    if (parent != NULL) {
#if VSF_FS_REF_TRACE == ENABLED
        vsf_trace_debug("get_parent %s" VSF_TRACE_CFG_LINEEND, parent->name ? parent->name : "ROOT");
#endif
        __vk_file_ref(parent);
        __vk_file_ref_parent(parent);
    }
    return parent;
}

void vk_fs_init(void)
{
    static bool __is_initialized = false;

    if (!__is_initialized) {
        __is_initialized = true;

        memset(&__vk_fs, 0, sizeof(__vk_fs));
        __vk_fs.rootfs.attr = VSF_FILE_ATTR_DIRECTORY;
        __vk_fs.rootfs.fsop = &vk_vfs_op;
        __vk_fs.rootfs.fsinfo = (vk_fs_info_t *)&__vk_fs.rootfs;
        __vk_file_ref(&__vk_fs.rootfs.use_as__vk_file_t);
        vsf_eda_crit_init(&__vk_fs.open.lock);
    }
}

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#endif

__vsf_component_peda_private_entry(__vk_file_lookup,
    const char *name;
    vk_file_t **result;
) {
    vsf_peda_begin();
    vsf_err_t err = VSF_ERR_NONE;
    vk_file_t *dir = (vk_file_t *)&vsf_this;
    switch (evt) {
    case VSF_EVT_INIT:
        __vsf_component_call_peda_ifs(vk_file_lookup, err, dir->fsop->dop.fn_lookup, dir->fsop->dop.lookup_local_size, dir,
            .name       = vsf_local.name,
            .result     = vsf_local.result);
        if (VSF_ERR_NONE == err) {
            break;
        }
        err = VSF_ERR_NOT_ENOUGH_RESOURCES;
    case VSF_EVT_RETURN:
        if ((VSF_ERR_NONE == err) && (*vsf_local.result != NULL)) {
            (*vsf_local.result)->parent = dir;
            if (dir->attr & VSF_VFS_FILE_ATTR_MOUNTED) {
                dir = &((vk_fs_info_t *)(((vk_vfs_file_t *)dir)->subfs.data))->root;
            }
            (*vsf_local.result)->fsinfo = dir->fsinfo;
        }
        vsf_eda_return(err);
        break;
    }
    vsf_peda_end();
}

__vsf_component_peda_private_entry(__vk_file_open,
    const char  *name;
    vk_file_t   *dir;
    vk_file_t   **file;
    vsf_err_t   err;
    bool        is_to_setpos;
) {
    vsf_peda_begin();
    vk_file_t *cur_dir = vsf_local.dir;
    vk_file_t *cur_file = *vsf_local.file;

    switch (evt) {
    case VSF_EVT_INIT:
        vsf_local.err = VSF_ERR_NONE;
        if (VSF_ERR_NONE != vsf_eda_crit_enter(&__vk_fs.open.lock)) {
            break;
        }
    case VSF_EVT_SYNC:
    do_lookup_child:
        if (vsf_local.name != NULL) {
            if (*vsf_local.name != '\0') {
                if (vk_file_is_div(*vsf_local.name)) {
                    if (!(cur_dir->attr & VSF_FILE_ATTR_DIRECTORY)) {
                        vsf_local.err = VSF_ERR_INVALID_PARAMETER;
                        goto do_fail;
                    }
                    vsf_local.name++;
                    if ('\0' == *vsf_local.name) {
                        goto do_return;
                    }
                }
            } else {
                goto do_return;
            }
        }

        *vsf_local.file = NULL;
        if (VSF_ERR_NONE != __vk_file_lookup(cur_dir, vsf_local.name, vsf_local.file)) {
            vsf_local.err = VSF_ERR_NOT_ENOUGH_RESOURCES;
        do_fail:
            if (cur_dir != NULL) {
                vsf_local.dir = NULL;
                if (VSF_ERR_NONE != vk_file_close(cur_dir)) {
                    goto do_return;
                }
            } else {
                goto do_return;
            }
        }
        break;
    do_return:
        vsf_eda_crit_leave(&__vk_fs.open.lock);
        vsf_eda_return(vsf_local.err);
        break;
    case VSF_EVT_RETURN:
        if (vsf_local.err != VSF_ERR_NONE) {
            goto do_fail;
        }
        if (vsf_local.is_to_setpos) {
            vsf_local.is_to_setpos = false;
            VSF_FS_ASSERT(0 == cur_file->pos);
        } else {
            if (NULL == cur_file) {
                vsf_local.err = VSF_ERR_NOT_AVAILABLE;
                goto do_fail;
            }
            __vk_file_ref(cur_file);
            if (cur_file->pos != 0) {
                vsf_local.is_to_setpos = true;
                vk_file_seek(cur_file, 0, VSF_FILE_SEEK_SET);
                break;
            }
        }
        if (vsf_local.name != NULL) {
            vsf_local.name += strlen(cur_file->name);
            cur_dir = vsf_local.dir = cur_file;
            goto do_lookup_child;
        } else {
            goto do_return;
        }
    }

    vsf_peda_end();
}

__vsf_component_peda_private_entry(__vk_file_close)
{
    vsf_peda_begin();
    vsf_err_t err;
    vk_file_t *file = (vk_file_t *)&vsf_this;
    VSF_FS_ASSERT(file != NULL);

    switch (evt) {
    case VSF_EVT_INIT:
    do_close:
        if (__vk_file_deref(file) > 0) {
            __vk_file_deref_parent(file);
            goto do_return;
        }

        VSF_FS_ASSERT(file->fsop != NULL);
        VSF_FS_ASSERT(file->fsop->fop.fn_close != NULL);
        __vsf_component_call_peda_ifs(vk_file_close, err, file->fsop->fop.fn_close, file->fsop->fop.close_local_size, file);
        if (VSF_ERR_NONE != err) {
        do_return:
            vsf_eda_return();
        }
        break;
    case VSF_EVT_RETURN: {
            vk_file_t *parent = file->parent;
            vk_file_free(file);
            file = parent;
            if (file != NULL) {
                vsf_eda_target_set((uintptr_t)file);
                goto do_close;
            } else {
                goto do_return;
            }
        }
        break;
    }
    vsf_peda_end();
}

__vsf_component_peda_private_entry(__vk_file_read,
    uint8_t *buff;
    uint_fast32_t size;
) {
    vsf_peda_begin();
    vsf_err_t err;
    vk_file_t *file = (vk_file_t *)&vsf_this;
    VSF_FS_ASSERT(file != NULL);

    switch (evt) {
    case VSF_EVT_INIT:
        __vsf_component_call_peda_ifs(vk_file_read, err, file->fsop->fop.fn_read, file->fsop->fop.read_local_size, file,
            .size       = vsf_local.size,
            .buff       = vsf_local.buff,
        );
        if (VSF_ERR_NONE != err) {
            vsf_eda_return(VSF_ERR_NOT_ENOUGH_RESOURCES);
        }
        break;
    case VSF_EVT_RETURN: {
            int rsize = vsf_eda_get_return_value();
            if (rsize > 0) {
                file->pos += rsize;
            }
            vsf_eda_return(rsize);
        }
        break;
    }
    vsf_peda_end();
}

__vsf_component_peda_private_entry(__vk_file_write,
    uint8_t *buff;
    uint_fast32_t size;
) {
    vsf_peda_begin();
    vsf_err_t err;
    vk_file_t *file = (vk_file_t *)&vsf_this;
    VSF_FS_ASSERT(file != NULL);

    switch (evt) {
    case VSF_EVT_INIT:
        __vsf_component_call_peda_ifs(vk_file_write, err, file->fsop->fop.fn_write, file->fsop->fop.write_local_size, file,
            .size       = vsf_local.size,
            .buff       = vsf_local.buff,
        );
        if (VSF_ERR_NONE != err) {
            vsf_eda_return(VSF_ERR_NOT_ENOUGH_RESOURCES);
        }
        break;
    case VSF_EVT_RETURN: {
            int rsize = vsf_eda_get_return_value();
            if (rsize > 0) {
                file->pos += rsize;
            }
            vsf_eda_return(rsize);
        }
        break;
    }
    vsf_peda_end();
}

__vsf_component_peda_private_entry(__vk_file_setpos,
    uint64_t offset;
    uint64_t result;
) {
    vsf_peda_begin();
    vsf_err_t err;
    vk_file_t *file = (vk_file_t *)&vsf_this;
    uint64_t pos = vsf_local.offset;
    VSF_FS_ASSERT(file != NULL);

    switch (evt) {
    case VSF_EVT_INIT:
        if (!(file->attr & VSF_FILE_ATTR_DIRECTORY)) {
            if (pos > file->size) {
                vsf_eda_return(VSF_ERR_INVALID_PARAMETER);
                return;
            } else if (pos == file->pos) {
                vsf_eda_return(VSF_ERR_NONE);
                return;
            }
        }

        __vsf_component_call_peda_ifs(vk_file_setpos, err, file->fsop->fop.fn_setpos, file->fsop->fop.setpos_local_size, file,
            .offset     = pos,
            .result     = &vsf_local.result,
        );
        if (VSF_ERR_NONE != err) {
            vsf_eda_return(VSF_ERR_NOT_ENOUGH_RESOURCES);
        }
        break;
    case VSF_EVT_RETURN: {
            vsf_err_t ret = vsf_eda_get_return_value();
            if (VSF_ERR_NONE == ret) {
                file->pos = vsf_local.result;
            }
            vsf_eda_return(ret);
        }
        break;
    }
    vsf_peda_end();
}

__vsf_component_peda_private_entry(__vk_file_setsize,
    uint64_t size;
) {
    vsf_peda_begin();
    vsf_err_t err;
    vk_file_t *file = (vk_file_t *)&vsf_this;
    uint64_t size = vsf_local.size;
    VSF_FS_ASSERT(file != NULL);

    switch (evt) {
    case VSF_EVT_INIT:
        if (file->attr & VSF_FILE_ATTR_DIRECTORY) {
            vsf_eda_return(VSF_ERR_INVALID_PARAMETER);
            return;
        }

        __vsf_component_call_peda_ifs(vk_file_setsize, err, file->fsop->fop.fn_setsize, file->fsop->fop.setsize_local_size, file,
            .size       = size,
        );
        if (VSF_ERR_NONE != err) {
            vsf_eda_return(VSF_ERR_NOT_ENOUGH_RESOURCES);
        }
        break;
    case VSF_EVT_RETURN: {
            vsf_err_t ret = vsf_eda_get_return_value();
            if (VSF_ERR_NONE == ret) {
                file->size = size;
                if (file->pos > file->size) {
                    file->pos = file->size;
                }
            }
            vsf_eda_return(ret);
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

static vsf_err_t __vk_file_lookup(vk_file_t *dir, const char *name, vk_file_t **file)
{
    vsf_err_t err;
    if (NULL == dir) {
        VSF_FS_ASSERT((name != NULL) && (*name != '\0'));
        dir = &__vk_fs.rootfs.use_as__vk_file_t;
        if (vk_file_is_div(*name)) {
            name++;
        }
        if (('\0' == *name) && (file != NULL)) {
            *file = dir;
            __vsf_component_call_peda(vk_fsop_succeed, err, dir)
            return err;
        }
    }

    VSF_FS_ASSERT(dir->fsop != NULL);
    VSF_FS_ASSERT(dir->fsop->dop.fn_lookup != NULL);

    __vsf_component_call_peda(__vk_file_lookup, err, dir,
        .name   = name,
        .result = file,
    )
    return err;
}

vsf_err_t vk_fs_mount(vk_file_t *dir, const vk_fs_op_t *fsop, void *fsdata)
{
    vsf_err_t err;
    VSF_FS_ASSERT(  (dir->attr & VSF_FILE_ATTR_DIRECTORY)
                &&  (dir->fsop == &vk_vfs_op)
                &&  !(dir->attr & VSF_VFS_FILE_ATTR_MOUNTED));

    vk_fs_info_t *fsinfo = fsdata;
    fsinfo->root.fsinfo = fsinfo;

    ((vk_vfs_file_t *)dir)->subfs.op = fsop;
    ((vk_vfs_file_t *)dir)->subfs.data = fsdata;
    __vsf_component_call_peda_ifs(vk_fs_mount, err, dir->fsop->fn_mount, dir->fsop->mount_local_size, dir);
    return err;
}

vsf_err_t vk_fs_unmount(vk_file_t *dir)
{
    vsf_err_t err;
    VSF_FS_ASSERT(  (dir->attr & VSF_FILE_ATTR_DIRECTORY)
                &&  (dir->fsop == &vk_vfs_op)
                &&  (dir->attr & VSF_VFS_FILE_ATTR_MOUNTED));

    dir->attr &= ~VSF_VFS_FILE_ATTR_MOUNTED;
    __vsf_component_call_peda_ifs(vk_fs_unmount, err, dir->fsop->fn_unmount, dir->fsop->unmount_local_size, dir);
    return err;
}

#if VSF_FS_CFG_USE_CACHE == ENABLED
vsf_err_t vk_fs_sync(vk_file_t *dir)
{
    vsf_err_t err;
    VSF_FS_ASSERT(dir != NULL);
    VSF_FS_ASSERT(dir->fsop != NULL);
    VSF_FS_ASSERT(dir->fsop->sync != NULL);

    __vsf_component_call_peda_ifs(vk_fs_sync, err, dir->fsop->sync, dir->fsop->sync_local_size, dir);
    return err;
}
#endif

vsf_err_t vk_file_open(vk_file_t *dir, const char *name, vk_file_t **file)
{
    vsf_err_t err;
#if VSF_FS_REF_TRACE == ENABLED
    char intbuf[32];
    vsf_trace_debug("open %s" VSF_TRACE_CFG_LINEEND, name ? name : itoa(idx, intbuf, 10));
#endif
    VSF_FS_ASSERT(file != NULL);

    if (NULL == dir) {
        dir = &__vk_fs.rootfs.use_as__vk_file_t;
    }

    VSF_FS_ASSERT(  (dir != NULL) && (dir->attr & VSF_FILE_ATTR_DIRECTORY)
                &&  (dir->fsop != NULL) && (dir->fsop->dop.fn_lookup != NULL));
    *file = dir;
    __vk_file_ref(dir);
    __vk_file_ref_parent(dir);

    __vsf_component_call_peda(__vk_file_open, err, dir,
        .name   = name,
        .dir    = dir,
        .file   = file,
    )
    return err;
}

vsf_err_t vk_file_close(vk_file_t *file)
{
    vsf_err_t err;
#if VSF_FS_REF_TRACE == ENABLED
    vsf_trace_debug("close %s" VSF_TRACE_CFG_LINEEND, file->name ? file->name : "ROOT");
#endif
    VSF_FS_ASSERT(file != NULL);
    __vsf_component_call_peda(__vk_file_close, err, file)
    return err;
}

vsf_err_t vk_file_read(vk_file_t *file, uint8_t *buff, uint_fast32_t size)
{
    vsf_err_t err;
    VSF_FS_ASSERT(file != NULL);
    VSF_FS_ASSERT(!(file->attr & VSF_FILE_ATTR_DIRECTORY));
    VSF_FS_ASSERT(file->attr & VSF_FILE_ATTR_READ);
    VSF_FS_ASSERT(file->fsop != NULL);
    VSF_FS_ASSERT(file->fsop->fop.fn_read != NULL);

    __vsf_component_call_peda(__vk_file_read, err, file,
        .size       = size,
        .buff       = buff,
    );
    return err;
}

vsf_err_t vk_file_write(vk_file_t *file, uint8_t *buff, uint_fast32_t size)
{
    vsf_err_t err;
    VSF_FS_ASSERT(file != NULL);
    VSF_FS_ASSERT(!(file->attr & VSF_FILE_ATTR_DIRECTORY));
    VSF_FS_ASSERT(file->attr & VSF_FILE_ATTR_WRITE);
    VSF_FS_ASSERT(file->fsop != NULL);
    VSF_FS_ASSERT(file->fsop->fop.fn_write != NULL);

    __vsf_component_call_peda(__vk_file_write, err, file,
        .size       = size,
        .buff       = buff,
    );
    return err;
}

vsf_err_t vk_file_setsize(vk_file_t *file, uint64_t size)
{
    vsf_err_t err;
    VSF_FS_ASSERT(file != NULL);
    VSF_FS_ASSERT(!(file->attr & VSF_FILE_ATTR_DIRECTORY));
    VSF_FS_ASSERT(file->attr & VSF_FILE_ATTR_WRITE);
    VSF_FS_ASSERT(file->fsop != NULL);
    VSF_FS_ASSERT(file->fsop->fop.fn_setsize != NULL);

    __vsf_component_call_peda(__vk_file_setsize, err, file,
        .size       = size,
    );
    return err;
}

uint64_t vk_file_tell(vk_file_t *file)
{
    VSF_FS_ASSERT(file != NULL);
    return file->pos;
}

vsf_err_t vk_file_seek(vk_file_t *file, int64_t offset, vk_file_whence_t whence)
{
    vsf_err_t err;
    VSF_FS_ASSERT(file != NULL);
    uint64_t pos = file->pos;

    switch (whence) {
    case VSF_FILE_SEEK_SET:
        VSF_FS_ASSERT(offset >= 0);
        pos = offset;
        break;
    case VSF_FILE_SEEK_CUR:
        pos = file->pos + offset;
        break;
    case VSF_FILE_SEEK_END:
        VSF_FS_ASSERT(!(file->attr & VSF_FILE_ATTR_DIRECTORY));
        pos = file->size + offset;
        break;
    }
    __vsf_component_call_peda(__vk_file_setpos, err, file,
        .offset     = pos,
    );
    return err;
}

vsf_err_t vk_file_create(vk_file_t *dir, const char *name, vk_file_attr_t attr)
{
    vsf_err_t err;
    if (NULL == dir) {
        dir = &__vk_fs.rootfs.use_as__vk_file_t;
    }

    VSF_FS_ASSERT((dir != NULL) && (name != NULL) && (*name != '\0'));
    VSF_FS_ASSERT(dir->attr & VSF_FILE_ATTR_DIRECTORY);
    VSF_FS_ASSERT(dir->fsop != NULL);
    VSF_FS_ASSERT(dir->fsop->dop.fn_create != NULL);

    __vsf_component_call_peda_ifs(vk_file_create, err, dir->fsop->dop.fn_create, dir->fsop->dop.create_local_size, dir,
        .name       = name,
        .attr       = attr,
    );
    return err;
}

vsf_err_t vk_file_unlink(vk_file_t *dir, const char *name)
{
    vsf_err_t err;
    if (NULL == dir) {
        dir = &__vk_fs.rootfs.use_as__vk_file_t;
    }

    VSF_FS_ASSERT((dir != NULL) && (name != NULL) && (*name != '\0'));
    VSF_FS_ASSERT(dir->attr & VSF_FILE_ATTR_DIRECTORY);
    VSF_FS_ASSERT(dir->fsop != NULL);
    VSF_FS_ASSERT(dir->fsop->dop.fn_unlink != NULL);

    __vsf_component_call_peda_ifs(vk_file_unlink, err, dir->fsop->dop.fn_unlink, dir->fsop->dop.unlink_local_size, dir,
        .name       = name,
    );
    return err;
}

vsf_err_t vk_file_rename(vk_file_t *olddir, const char *oldname, vk_file_t *newdir, const char *newname)
{
    vsf_err_t err;
    if (NULL == olddir) {
        olddir = &__vk_fs.rootfs.use_as__vk_file_t;
    }

    VSF_FS_ASSERT(olddir != NULL);
    VSF_FS_ASSERT(olddir->attr & VSF_FILE_ATTR_DIRECTORY);
    VSF_FS_ASSERT((NULL == newdir) || (newdir->attr & VSF_FILE_ATTR_DIRECTORY));
    VSF_FS_ASSERT((oldname != NULL) && (*oldname != '\0'));

    vk_file_t *root = &olddir->fsinfo->root;
    VSF_FS_ASSERT(root != NULL);
    vk_file_t *newroot = (newdir != NULL) ? &newdir->fsinfo->root : root;
    VSF_FS_ASSERT(newroot != NULL);
    if (root != newroot) {
        return VSF_ERR_INVALID_PARAMETER;
    }

    __vsf_component_call_peda_ifs(vk_file_rename, err, root->fsop->fn_rename, root->fsop->rename_local_size, root,
        .olddir     = olddir,
        .oldname    = oldname,
        .newdir     = newdir,
        .newname    = newname,
    );
    return err;
}

#if VSF_FS_CFG_USE_CACHE == ENABLED
vsf_err_t vk_file_sync(vk_file_t *file)
{
    vsf_err_t err;
    VSF_FS_ASSERT(file != NULL);
    VSF_FS_ASSERT(file->fsop != NULL);
    VSF_FS_ASSERT(file->fsop->fop.fn_sync != NULL);

    __vsf_component_call_peda_ifs(vk_file_sync, err, dir->fsop->fop.fn_sync, dir->fsop->fop.sync_local_size, file);
    return err;
}
#endif

// vfs

static vk_vfs_file_t * __vk_vfs_lookup_imp(vk_vfs_file_t *dir, const char *name)
{
    vk_vfs_file_t *child;
    uint32_t idx = dir->pos;

    vsf_protect_t orig = vsf_protect_sched();
        vsf_dlist_peek_head(vk_vfs_file_t, use_as__vsf_dlist_node_t, &dir->d.child_list, child);
        while (child != NULL) {
            if (    (name && vk_file_is_match((char *)name, child->name))
                ||  (!name && !idx)) {
                break;
            }
            idx--;
            vsf_dlist_peek_next(vk_vfs_file_t, use_as__vsf_dlist_node_t, child, child);
        }
    vsf_unprotect_sched(orig);
    if (NULL == name) {
        if (child != NULL) {
            dir->pos++;
        } else {
            dir->pos = 0;
        }
    }
    return child;
}

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#endif

__vsf_component_peda_ifs_entry(__vk_vfs_mount, vk_fs_mount)
{
    vsf_peda_begin();
    vsf_err_t err = VSF_ERR_NONE;
    vk_vfs_file_t *dir = (vk_vfs_file_t *)&vsf_this;

    switch (evt) {
    case VSF_EVT_INIT:
        __vsf_component_call_peda_ifs(vk_fs_mount, err, dir->subfs.op->fn_mount, dir->subfs.op->mount_local_size, dir);
        if (VSF_ERR_NONE == err) {
            break;
        }

        err = VSF_ERR_NOT_ENOUGH_RESOURCES;
        vsf_eda_return(err);
        break;
    case VSF_EVT_RETURN:
        err = vsf_eda_get_return_value();
        if (VSF_ERR_NONE == err) {
            vk_file_t *root = dir->subfs.root;
            dir->attr |= VSF_VFS_FILE_ATTR_MOUNTED;
            root->attr |= VSF_FILE_ATTR_DIRECTORY | VSF_FILE_ATTR_READ;
            root->fsop = dir->subfs.op;
        }
        vsf_eda_return(err);
        break;
    }
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_vfs_unmount, vk_fs_unmount)
{
    vsf_peda_begin();
    vsf_err_t err = VSF_ERR_NONE;
    vk_vfs_file_t *dir = (vk_vfs_file_t *)&vsf_this;
    switch (evt) {
    case VSF_EVT_INIT:
        __vsf_component_call_peda_ifs(vk_fs_unmount, err, dir->subfs.op->fn_unmount, dir->subfs.op->unmount_local_size, dir);
        if (VSF_ERR_NONE == err) {
            break;
        }

        err = VSF_ERR_NOT_ENOUGH_RESOURCES;
    case VSF_EVT_RETURN:
        dir->attr &= ~VSF_VFS_FILE_ATTR_MOUNTED;
        vsf_eda_return(err);
        break;
    }
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_vfs_lookup, vk_file_lookup)
{
    vsf_peda_begin();
    vsf_err_t err = VSF_ERR_NONE;
    vk_vfs_file_t *dir = (vk_vfs_file_t *)&vsf_this;
    switch (evt) {
    case VSF_EVT_INIT:
        if (dir->attr & VSF_VFS_FILE_ATTR_MOUNTED) {
            __vsf_component_call_peda_ifs(vk_file_lookup, err, dir->subfs.op->dop.fn_lookup, dir->subfs.op->dop.lookup_local_size, dir->subfs.root,
                .name       = vsf_local.name,
                .result     = vsf_local.result,
            );
            if (VSF_ERR_NONE == err) {
                break;
            }

            err = VSF_ERR_NOT_ENOUGH_RESOURCES;
        } else {
            vk_vfs_file_t *result = __vk_vfs_lookup_imp(dir, vsf_local.name);
            *vsf_local.result = &result->use_as__vk_file_t;

            if (result != NULL) {
                err = VSF_ERR_NONE;
                if (result->attr & VSF_VFS_FILE_ATTR_MOUNTED) {
                    VSF_FS_ASSERT(result->subfs.root != NULL);
                    result->pos = result->subfs.root->pos;
                }
            } else {
                err = VSF_ERR_NOT_AVAILABLE;
            }
        }
    case VSF_EVT_RETURN:
        if (dir->attr & VSF_VFS_FILE_ATTR_MOUNTED) {
            err = (vsf_err_t)vsf_eda_get_return_value();
        }
        vsf_eda_return(err);
        break;
    }
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_vfs_create, vk_file_create)
{
    vsf_peda_begin();
    vsf_err_t err = VSF_ERR_NONE;
    vk_vfs_file_t *dir = (vk_vfs_file_t *)&vsf_this;
    switch (evt) {
    case VSF_EVT_INIT:
        if (dir->attr & VSF_VFS_FILE_ATTR_MOUNTED) {
            __vsf_component_call_peda_ifs(vk_file_create, err, dir->subfs.op->dop.fn_create, dir->subfs.op->dop.create_local_size, dir->subfs.root,
                .name       = vsf_local.name,
                .attr       = vsf_local.attr,
            );
            if (VSF_ERR_NONE == err) {
                break;
            }

            goto do_no_resources;
        } else {
            vk_vfs_file_t *new_file = __vk_vfs_lookup_imp(dir, vsf_local.name);
            if (new_file != NULL) {
                err = VSF_ERR_ALREADY_EXISTS;
                goto do_return;
            }

            new_file = (vk_vfs_file_t *)vk_file_alloc(sizeof(vk_vfs_file_t));
            if (NULL == new_file) {
            do_no_resources:
                err = VSF_ERR_NOT_ENOUGH_RESOURCES;
                goto do_return;
            }

            new_file->name = vsf_heap_malloc(strlen(vsf_local.name) + 1);
            if (NULL == new_file->name) {
                vk_file_free(&new_file->use_as__vk_file_t);
                goto do_no_resources;
            }
            strcpy(new_file->name, vsf_local.name);

            new_file->attr |= vsf_local.attr;
            new_file->fsop = &vk_vfs_op;

            vsf_protect_t orig = vsf_protect_sched();
                vsf_dlist_add_to_tail(vk_vfs_file_t, use_as__vsf_dlist_node_t, &dir->d.child_list, new_file);
            vsf_unprotect_sched(orig);

#if VSF_FS_REF_TRACE == ENABLED
            vsf_trace_debug("create vfs %s" VSF_TRACE_CFG_LINEEND, new_file->name);
#endif
            // avoid to be freed while closing, can only be freed in unlink
            __vk_file_ref(&new_file->use_as__vk_file_t);
            goto do_return;
        }
    case VSF_EVT_RETURN:
        if (dir->attr & VSF_VFS_FILE_ATTR_MOUNTED) {
            err = (vsf_err_t)vsf_eda_get_return_value();
        }
    do_return:
        vsf_eda_return(err);
        break;
    }
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_vfs_unlink, vk_file_unlink)
{
    vsf_peda_begin();
    vsf_err_t err;
    vk_vfs_file_t *dir = (vk_vfs_file_t *)&vsf_this;
    switch (evt) {
    case VSF_EVT_INIT:
        if (dir->attr & VSF_VFS_FILE_ATTR_MOUNTED) {
            __vsf_component_call_peda_ifs(vk_file_unlink, err, dir->subfs.op->dop.fn_unlink, dir->subfs.op->dop.unlink_local_size, dir->subfs.root,
                .name = vsf_local.name
            );
            if (VSF_ERR_NONE == err) {
                break;
            }

            err = VSF_ERR_NOT_ENOUGH_RESOURCES;
        } else {
            vk_vfs_file_t *child = __vk_vfs_lookup_imp(dir, vsf_local.name);
            if (child != NULL) {
                VSF_FS_ASSERT(1 == child->ref);
#if VSF_FS_REF_TRACE == ENABLED
                vsf_trace_debug("unlink vfs %s" VSF_TRACE_CFG_LINEEND, child->name);
#endif
                // deref the ref referenced in __vk_vfs_create
                __vk_file_deref(&child->use_as__vk_file_t);
                vsf_protect_t orig = vsf_protect_sched();
                    vsf_dlist_remove(vk_vfs_file_t, use_as__vsf_dlist_node_t, &dir->d.child_list, child);
                vsf_unprotect_sched(orig);
                vk_file_free(&child->use_as__vk_file_t);
            }
            err = (NULL == child) ? VSF_ERR_NOT_AVAILABLE : VSF_ERR_NONE;
        }
    case VSF_EVT_RETURN:
        if (dir->attr & VSF_VFS_FILE_ATTR_MOUNTED) {
            err = (vsf_err_t)vsf_eda_get_return_value();
        } else {
            err = VSF_ERR_FAIL;
        }
        vsf_eda_return(err);
        break;
    }
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_vfs_rename, vk_file_rename)
{
    vsf_peda_begin();
    vsf_err_t err;
    vk_vfs_file_t *root = (vk_vfs_file_t *)&vsf_this;
    switch (evt) {
    case VSF_EVT_INIT:
        if (root->attr & VSF_VFS_FILE_ATTR_MOUNTED) {
            __vsf_component_call_peda_ifs(vk_file_rename, err, root->subfs.op->fn_rename, root->subfs.op->rename_local_size, root->subfs.root,
                .olddir     = vsf_local.olddir,
                .oldname    = vsf_local.oldname,
                .newdir     = vsf_local.newdir,
                .newname    = vsf_local.newname,
            );
            if (VSF_ERR_NONE == err) {
                break;
            }

            err = VSF_ERR_NOT_ENOUGH_RESOURCES;
        } else {
            vk_vfs_file_t *olddir = (vk_vfs_file_t *)vsf_local.olddir;
            vk_vfs_file_t *newdir = (vk_vfs_file_t *)vsf_local.newdir;
            vk_vfs_file_t *oldfile = __vk_vfs_lookup_imp(olddir, vsf_local.oldname);
            vk_vfs_file_t *newfile;
            char *newname;

            if (vsf_local.newname != NULL) {
                newfile = __vk_vfs_lookup_imp(newdir, vsf_local.newname);
            } else {
                newfile = __vk_vfs_lookup_imp(newdir, vsf_local.oldname);
            }

            if (newfile != NULL) {
                err = VSF_ERR_ALREADY_EXISTS;
                goto do_exit;
            } else if (oldfile != NULL) {
                err = VSF_ERR_NOT_AVAILABLE;
                goto do_exit;
            }

            if (vsf_local.newname != NULL) {
                newname = vsf_heap_malloc(strlen(vsf_local.newname) + 1);
                if (NULL == newname) {
                    err = VSF_ERR_NOT_ENOUGH_RESOURCES;
                    goto do_exit;
                }
                strcpy(newname, vsf_local.newname);

                free(oldfile->name);
                oldfile->name = newname;
            } else {
                newname = (char *)vsf_local.oldname;
            }

            if (newdir != NULL) {
                vsf_protect_t orig = vsf_protect_sched();
                    vsf_dlist_remove(vk_vfs_file_t, use_as__vsf_dlist_node_t, &olddir->d.child_list, oldfile);
                    vsf_dlist_add_to_tail(vk_vfs_file_t, use_as__vsf_dlist_node_t, &newdir->d.child_list, oldfile);
                vsf_unprotect_sched(orig);
            }

            vsf_eda_return(VSF_ERR_NONE);
            return;
        }
    case VSF_EVT_RETURN:
        if (root->attr & VSF_VFS_FILE_ATTR_MOUNTED) {
            err = (vsf_err_t)vsf_eda_get_return_value();
        } else {
            err = VSF_ERR_FAIL;
        }
    do_exit:
        vsf_eda_return(err);
        break;
    }
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_vfs_setpos, vk_file_setpos)
{
    vsf_peda_begin();
    vk_vfs_file_t *file = (vk_vfs_file_t *)&vsf_this;

    switch (evt) {
    case VSF_EVT_INIT:
        VSF_FS_ASSERT(file != NULL);

        if (file->attr & VSF_VFS_FILE_ATTR_MOUNTED) {
            vsf_err_t err;
            __vsf_component_call_peda_ifs(vk_file_setpos, err, file->subfs.op->fop.fn_setpos, file->subfs.op->fop.setpos_local_size, file->subfs.root,
                .offset     = vsf_local.offset,
                .result     = vsf_local.result,
            );
            if (VSF_ERR_NONE != err) {
                vsf_eda_return(VSF_ERR_NOT_ENOUGH_RESOURCES);
            }
            break;
        } else {
            *vsf_local.result = vsf_local.offset;
        }
        // fall through
    case VSF_EVT_RETURN:
        vsf_eda_return(VSF_ERR_NONE);
        break;
    }
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_vfs_setsize, vk_file_setsize)
{
    vsf_peda_begin();
    vk_vfs_file_t *file = (vk_vfs_file_t *)&vsf_this;
    VSF_FS_ASSERT((file != NULL) && !(file->attr & VSF_FILE_ATTR_DIRECTORY));
    VSF_FS_ASSERT((NULL == file->f.callback.fn_read) && (NULL == file->f.callback.fn_write));

    file->f.param = vsf_heap_realloc(file->f.param, vsf_local.size);
    if (NULL == file->f.param) {
        file->size = 0;
        vsf_eda_return(VSF_ERR_NOT_ENOUGH_RESOURCES);
        return;
    }

    if (vsf_local.size > file->size) {
        memset((uint8_t *)file->f.param + file->size, 0, vsf_local.size - file->size);
    }
    file->size = vsf_local.size;
    vsf_eda_return(VSF_ERR_NONE);
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_vfs_close, vk_file_close)
{
    vsf_peda_begin();
    vk_vfs_file_t *file = (vk_vfs_file_t *)&vsf_this;

    if (    (NULL == file->f.callback.fn_read)
        &&  (NULL== file->f.callback.fn_write)
        &&  (NULL != file->f.param)) {
        vsf_heap_free(file->f.param);
        file->f.param = NULL;
    }
    if (file->name != NULL) {
        vsf_heap_free(file->name);
    }

    vsf_eda_return(VSF_ERR_NONE);
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_vfs_read, vk_file_read)
{
    vsf_peda_begin();
    vk_vfs_file_t *file = (vk_vfs_file_t *)&vsf_this;

    switch (evt) {
    case VSF_EVT_INIT:
        if (file->f.callback.fn_read != NULL) {
            vsf_err_t err;
            __vsf_component_call_peda_ifs(vk_file_read, err, file->f.callback.fn_read, 0, file,
                .size       = vsf_local.size,
                .buff       = vsf_local.buff,
            );
            if (VSF_ERR_NONE != err) {
                vsf_eda_return(VSF_ERR_NOT_ENOUGH_RESOURCES);
            }
        } else if (NULL == file->f.callback.fn_write) {
            uint_fast64_t offset = file->pos;
            uint_fast32_t size = vsf_local.size;
            uint8_t *buff = vsf_local.buff;
            int_fast32_t rsize = 0;

            if (offset < file->size) {
                rsize = vsf_min(size, file->size - offset);
                VSF_FS_ASSERT(file->f.param != NULL);
                memcpy(buff, &((uint8_t *)file->f.param)[offset], rsize);
            }
            vsf_eda_return(rsize);
        } else {
            vsf_eda_return(VSF_ERR_NOT_ACCESSABLE);
        }
        break;
    case VSF_EVT_RETURN:
        vsf_eda_return(vsf_eda_get_return_value());
        break;
    }
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_vfs_write, vk_file_write)
{
    vsf_peda_begin();
    vk_vfs_file_t *file = (vk_vfs_file_t *)&vsf_this;

    switch (evt) {
    case VSF_EVT_INIT:
        if (file->f.callback.fn_write != NULL) {
            vsf_err_t err;
            __vsf_component_call_peda_ifs(vk_file_write, err, file->f.callback.fn_write, 0, file,
                .size       = vsf_local.size,
                .buff       = vsf_local.buff,
            );
            if (VSF_ERR_NONE != err) {
                vsf_eda_return(VSF_ERR_NOT_ENOUGH_RESOURCES);
            }
        } else if (NULL == file->f.callback.fn_read) {
            uint_fast64_t offset = file->pos;
            uint_fast32_t size = vsf_local.size;
            uint8_t *buff = vsf_local.buff;
            int_fast32_t wsize = 0;

            if ((offset + size) > file->size) {
                file->size = offset + size;
                file->f.param = vsf_heap_realloc(file->f.param, file->size);
                if (NULL == file->f.param) {
                    file->size = 0;
                    vsf_eda_return(VSF_ERR_NOT_ENOUGH_RESOURCES);
                    return;
                }
            }

            if (offset < file->size) {
                wsize = vsf_min(size, file->size - offset);
                memcpy(&((uint8_t *)file->f.param)[offset], buff, wsize);
            }
            vsf_eda_return(wsize);
        } else {
            vsf_eda_return(VSF_ERR_NOT_ACCESSABLE);
        }
        break;
    case VSF_EVT_RETURN:
        vsf_eda_return(vsf_eda_get_return_value());
        break;
    }
    vsf_peda_end();
}

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic pop
#endif

#if VSF_USE_SIMPLE_STREAM == ENABLED
static void __vk_file_stream_tx_evthandler(vsf_stream_t *stream, void *param, vsf_stream_evt_t evt)
{
    vk_file_stream_t *pthis = (vk_file_stream_t *)param;

    switch (evt) {
    case VSF_STREAM_ON_CONNECT:
    case VSF_STREAM_ON_OUT:
        if (!pthis->cur_size && pthis->size > 0) {
            pthis->cur_size = vsf_stream_get_wbuf(stream, &pthis->cur_buff);
            vsf_eda_post_evt(pthis->cur_eda, VSF_EVT_FILE_READ);
        }
        break;
    }
}

static void __vk_file_stream_rx_evthandler(vsf_stream_t *stream, void *param, vsf_stream_evt_t evt)
{
    vk_file_stream_t *pthis = (vk_file_stream_t *)param;

    switch (evt) {
    case VSF_STREAM_ON_CONNECT:
    case VSF_STREAM_ON_IN:
        if (!pthis->cur_size && pthis->size > 0) {
            pthis->cur_size = vsf_stream_get_rbuf(stream, &pthis->cur_buff);
            vsf_eda_post_evt(pthis->cur_eda, VSF_EVT_FILE_WRITE);
        }
        break;
    }
}

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#endif

__vsf_component_peda_private_entry(__vk_file_read_stream)
{
    vsf_peda_begin();
    vk_file_stream_t *pthis = (vk_file_stream_t *)&vsf_this;
    vk_file_t *file = pthis->file;
    vsf_stream_t *stream = pthis->stream;

    switch (evt) {
    case VSF_EVT_INIT:
        pthis->rw_size = 0;
        pthis->cur_eda = vsf_eda_get_cur();
        stream->tx.param = pthis;
        stream->tx.evthandler = __vk_file_stream_tx_evthandler;
        vsf_stream_connect_tx(stream);
        break;
    case VSF_EVT_RETURN: {
            int32_t result = (int32_t)vsf_eda_get_return_value();
            if (result > 0) {
                pthis->cur_size = 0;
                pthis->size -= result;
                pthis->rw_size += result;
                vsf_stream_write(stream, NULL, result);
            }
            if ((result < 0) || !pthis->size) {
                vsf_stream_disconnect_tx(stream);
                vsf_eda_return();
            }
        }
        break;
    case VSF_EVT_FILE_READ:
        pthis->cur_size = vsf_min(pthis->cur_size, pthis->size);
        vk_file_read(file, pthis->cur_buff, pthis->cur_size);
        break;
    }
    vsf_peda_end();
}

__vsf_component_peda_private_entry(__vk_file_write_stream)
{
    vsf_peda_begin();
    vk_file_stream_t *pthis = (vk_file_stream_t *)&vsf_this;
    vk_file_t *file = pthis->file;
    vsf_stream_t *stream = pthis->stream;

    switch (evt) {
    case VSF_EVT_INIT:
        pthis->rw_size = 0;
        pthis->cur_eda = vsf_eda_get_cur();
        stream->rx.param = pthis;
        stream->rx.evthandler = __vk_file_stream_rx_evthandler;
        vsf_stream_connect_rx(stream);
        break;
    case VSF_EVT_RETURN: {
            int32_t result = (int32_t)vsf_eda_get_return_value();
            if (result > 0) {
                pthis->cur_size = 0;
                pthis->size -= result;
                pthis->rw_size += result;
                vsf_stream_read(stream, NULL, result);
            }
            if ((result < 0) || !pthis->size) {
                vsf_stream_disconnect_rx(stream);
                vsf_eda_return();
            }
        }
        break;
    case VSF_EVT_FILE_WRITE:
        pthis->cur_size = vsf_min(pthis->cur_size, pthis->size);
        vk_file_write(file, pthis->cur_buff, pthis->cur_size);
        break;
    }
    vsf_peda_end();
}

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic pop
#endif

vsf_err_t vk_file_read_stream(vk_file_stream_t *pthis, vsf_stream_t *stream, uint_fast32_t size)
{
    vsf_err_t err;
    VSF_FS_ASSERT(pthis != NULL);
    pthis->size = size;
    pthis->stream = stream;
    pthis->cur_size = 0;
    __vsf_component_call_peda(__vk_file_read_stream, err, pthis)
    return err;
}

vsf_err_t vk_file_write_stream(vk_file_stream_t *pthis, vsf_stream_t *stream, uint_fast32_t size)
{
    vsf_err_t err;
    VSF_FS_ASSERT(pthis != NULL);
    pthis->size = size;
    pthis->stream = stream;
    pthis->cur_size = 0;
    __vsf_component_call_peda(__vk_file_write_stream, err, pthis)
    return err;
}
#endif

#endif
