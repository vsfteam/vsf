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

#include "./vsf_fs_cfg.h"

#if VSF_USE_FS == ENABLED

#define VSF_FS_IMPLEMENT
#define __VSF_EDA_CLASS_INHERIT
// TODO: use dedicated include
#include "vsf.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct __vk_fs_t {
    struct {
        char *name;
        vk_file_t *dir;
        vk_file_t **file;
        vsf_err_t err;
        vsf_crit_t lock;
    } open;
    vk_vfs_file_t rootfs;
};
typedef struct __vk_fs_t __vk_fs_t;

/*============================ PROTOTYPES ====================================*/

static void __vk_vfs_mount(uintptr_t, vsf_evt_t);
static void __vk_vfs_unmount(uintptr_t, vsf_evt_t);
static void __vk_vfs_lookup(uintptr_t, vsf_evt_t);
static void __vk_vfs_create(uintptr_t, vsf_evt_t);
static void __vk_vfs_unlink(uintptr_t, vsf_evt_t);

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static NO_INIT __vk_fs_t __vk_fs;

static vk_fs_op_t __vk_vfs_op = {
    .mount          = __vk_vfs_mount,
    .unmount        = __vk_vfs_unmount,
#if VSF_FS_CFG_USE_CACHE == ENABLED
    .sync           = vk_dummyfs_succeed,
#endif
    .fop            = {
        .close      = vk_dummyfs_succeed,
        .read       = vk_dummyfs_not_support,
        .write      = vk_dummyfs_not_support,
        .truncate   = vk_dummyfs_not_support,
    },
    .dop            = {
        .lookup     = __vk_vfs_lookup,
        .create     = __vk_vfs_create,
        .unlink     = __vk_vfs_unlink,
        .chmod      = vk_dummyfs_not_support,
        .rename     = vk_dummyfs_not_support,
    },
};

/*============================ IMPLEMENTATION ================================*/

static void __vk_file_ref(vk_file_t *file)
{
    vsf_protect_t orig = vsf_protect_sched();
        file->ref++;
    vsf_unprotect_sched(orig);
}

static uint_fast32_t __vk_file_deref(vk_file_t *file)
{
    uint_fast32_t ref;
    vsf_protect_t orig = vsf_protect_sched();
        ref = --file->ref;
    vsf_unprotect_sched(orig);
    return ref;
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
    char *ext = strrchr(fname, '.');
    if (ext != NULL) {
        ext++;
    }
    return ext;
}

char * vk_file_getfilename(char *path)
{
    char *name0 = strrchr(path, '\\');
    char *name1 = strrchr(path, '/');
    char *name = (char *)max((uintptr_t)name0, (uintptr_t)name1);
    if ((name != NULL) && (vk_file_is_div(*name))) {
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

void vk_fs_init(void)
{
    memset(&__vk_fs, 0, sizeof(__vk_fs));
    __vk_fs.rootfs.attr = VSF_FILE_ATTR_DIRECTORY;
    __vk_fs.rootfs.fsop = &__vk_vfs_op;
    vsf_eda_crit_init(&__vk_fs.open.lock);
}

vk_file_ctx_t * vk_file_get_ctx(vk_file_t *file)
{
    VSF_FS_ASSERT(file != NULL);
    return &file->ctx;
}

vsf_err_t vk_file_get_errcode(vk_file_t *file)
{
    return file->ctx.err;
}

vk_file_t * vk_file_get_parent(vk_file_t *file)
{
    vk_file_t *parent = file->parent;
    if (parent != NULL) {
        __vk_file_ref(parent);
    }
    return parent;
}

vsf_err_t vk_fs_mount(vk_file_t *dir, const vk_fs_op_t *fsop, void *fsdata)
{
    VSF_FS_ASSERT(  (dir->attr & VSF_FILE_ATTR_DIRECTORY)
                &&  (dir->fsop == &__vk_vfs_op)
                &&  !(dir->attr & VSF_VFS_FILE_ATTR_MOUNTED));

    ((vk_vfs_file_t *)dir)->subfs.op = fsop;
    ((vk_vfs_file_t *)dir)->subfs.data = fsdata;
    return vsf_eda_call_param_eda(dir->fsop->mount, dir);
}

vsf_err_t vk_fs_unmount(vk_file_t *dir)
{
    VSF_FS_ASSERT(  (dir->attr & VSF_FILE_ATTR_DIRECTORY)
                &&  (dir->fsop == &__vk_vfs_op)
                &&  (dir->attr & VSF_VFS_FILE_ATTR_MOUNTED));

    dir->attr &= ~VSF_VFS_FILE_ATTR_MOUNTED;
    return vsf_eda_call_param_eda(dir->fsop->unmount, dir);
}

#if VSF_FS_CFG_USE_CACHE == ENABLED
vsf_err_t vk_fs_sync(vk_file_t *dir)
{
    VSF_FS_ASSERT(dir != NULL);
    VSF_FS_ASSERT(dir->fsop != NULL);
    VSF_FS_ASSERT(dir->fsop->sync != NULL);
    return vsf_eda_call_param_eda(dir->fsop->sync, dir);
}
#endif

static void __vk_file_open_imp(vk_file_t *dir, vsf_evt_t evt)
{
    vk_file_t *cur_dir = __vk_fs.open.dir;
    vk_file_t *cur_file = *__vk_fs.open.file;

    switch (evt) {
    case VSF_EVT_INIT:
        __vk_fs.open.err = VSF_ERR_NONE;
        if (VSF_ERR_NONE != vsf_eda_crit_enter(&__vk_fs.open.lock)) {
            break;
        }
    case VSF_EVT_SYNC:
    do_lookup_child:
        if (*__vk_fs.open.name != '\0') {
            if (vk_file_is_div(*__vk_fs.open.name)) {
                if (!(cur_dir->attr & VSF_FILE_ATTR_DIRECTORY)) {
                    __vk_fs.open.err = VSF_ERR_INVALID_PARAMETER;
                    goto do_fail;
                }
                __vk_fs.open.name++;
                if ('\0' == *__vk_fs.open.name) {
                    goto do_return;
                }
            }

            if (VSF_ERR_NONE != vk_file_lookup(cur_dir, __vk_fs.open.name, 0, __vk_fs.open.file)) {
                __vk_fs.open.err = VSF_ERR_NOT_ENOUGH_RESOURCES;
            do_fail:
                if ((cur_dir == dir) || (VSF_ERR_NONE != vk_file_close(cur_dir))) {
                    goto do_return;
                }
            }
            break;
        }
    do_return:
        if (    (VSF_ERR_NONE == __vk_fs.open.err)
            &&  (*__vk_fs.open.file == &__vk_fs.rootfs.use_as__vk_file_t)) {
            // root is not returned by lookup, so ref manually
            __vk_file_ref(*__vk_fs.open.file);
        }
        vsf_eda_crit_leave(&__vk_fs.open.lock);
        vsf_eda_return();
        break;
    case VSF_EVT_RETURN:
        if (__vk_fs.open.err != VSF_ERR_NONE) {
            goto do_fail;
        }
        if (NULL == cur_file) {
            __vk_fs.open.err = VSF_ERR_NOT_AVAILABLE;
            goto do_fail;
        }
        __vk_fs.open.name += strlen(cur_file->name);
        cur_dir = __vk_fs.open.dir = cur_file;
        goto do_lookup_child;
    }
}

vsf_err_t vk_file_open(vk_file_t *dir, char *name, vk_file_t **file)
{
    VSF_FS_ASSERT((name != NULL) && (*name != '\0') && (file != NULL));

    if (NULL == dir) {
        dir = &__vk_fs.rootfs.use_as__vk_file_t;
    }

    VSF_FS_ASSERT(  (dir != NULL) && (dir->attr & VSF_FILE_ATTR_DIRECTORY)
                &&  (dir->fsop != NULL) && (dir->fsop->dop.lookup != NULL));
    __vk_fs.open.name = name;
    __vk_fs.open.dir = dir;
    *file = dir;
    __vk_fs.open.file = file;
    return vsf_eda_call_param_eda(__vk_file_open_imp, dir);
}

static void __vk_file_close_imp(vk_file_t *file, vsf_evt_t evt)
{
    VSF_FS_ASSERT(file != NULL);

    switch (evt) {
    case VSF_EVT_INIT:
    do_close:
        if (__vk_file_deref(file) > 0) {
            goto do_return;
        }

        VSF_FS_ASSERT(file->fsop != NULL);
        VSF_FS_ASSERT(file->fsop->fop.close != NULL);
        if (VSF_ERR_NONE != vsf_eda_call_param_eda(file->fsop->fop.close, file)) {
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
}

vsf_err_t vk_file_close(vk_file_t *file)
{
    VSF_FS_ASSERT(file != NULL);
    return vsf_eda_call_param_eda(__vk_file_close_imp, file);
}

static void __vk_file_lookup_imp(uintptr_t target, vsf_evt_t evt)
{
    vk_file_t *dir = (vk_file_t *)target;
    switch (evt) {
    case VSF_EVT_INIT:
        if (VSF_ERR_NONE == vsf_eda_call_param_eda(dir->fsop->dop.lookup, dir)) {
            break;
        }
        dir->ctx.err = VSF_ERR_NOT_ENOUGH_RESOURCES;
    case VSF_EVT_RETURN:
        if ((VSF_ERR_NONE == dir->ctx.err) && (*dir->ctx.lookup.result != NULL)) {
            __vk_file_ref(*dir->ctx.lookup.result);
        }
        vsf_eda_return();
        break;
    }
}

vsf_err_t vk_file_lookup(vk_file_t *dir, char *name, uint_fast16_t idx, vk_file_t **file)
{
    VSF_FS_ASSERT(dir != NULL);
    VSF_FS_ASSERT(dir->fsop != NULL);
    VSF_FS_ASSERT(dir->fsop->dop.lookup != NULL);

    dir->ctx.lookup.name = name;
    dir->ctx.lookup.idx = idx;
    dir->ctx.lookup.result = file;
    return vsf_eda_call_param_eda(__vk_file_lookup_imp, dir);
}

vsf_err_t vk_file_read(vk_file_t *file, uint_fast64_t addr, uint_fast32_t size, uint8_t *buff, int32_t *rsize)
{
    VSF_FS_ASSERT(file != NULL);
    VSF_FS_ASSERT(file->attr & VSF_FILE_ATTR_READ);
    VSF_FS_ASSERT(file->fsop != NULL);
    VSF_FS_ASSERT(file->fsop->fop.read != NULL);

    file->ctx.io.offset = addr;
    file->ctx.io.size = size;
    file->ctx.io.buff = buff;
    file->ctx.io.result = rsize;
    return vsf_eda_call_param_eda(file->fsop->fop.read, file);
}

vsf_err_t vk_file_write(vk_file_t *file, uint_fast64_t addr, uint_fast32_t size, uint8_t *buff, int32_t *wsize)
{
    VSF_FS_ASSERT(file != NULL);
    VSF_FS_ASSERT(file->attr & VSF_FILE_ATTR_WRITE);
    VSF_FS_ASSERT(file->fsop != NULL);
    VSF_FS_ASSERT(file->fsop->fop.write != NULL);

    file->ctx.io.offset = addr;
    file->ctx.io.size = size;
    file->ctx.io.buff = buff;
    file->ctx.io.result = wsize;
    return vsf_eda_call_param_eda(file->fsop->fop.write, file);
}

vsf_err_t vk_file_create(vk_file_t *dir, char *name, vk_file_attr_t attr, uint_fast64_t size)
{
    VSF_FS_ASSERT((dir != NULL) && (name != NULL) && (*name != '\0'));
    VSF_FS_ASSERT(dir->attr & VSF_FILE_ATTR_DIRECTORY);
    VSF_FS_ASSERT(dir->fsop != NULL);
    VSF_FS_ASSERT(dir->fsop->dop.create != NULL);

    dir->ctx.create.name = name;
    dir->ctx.create.attr = attr;
    dir->ctx.create.size = size;
    return vsf_eda_call_param_eda(dir->fsop->dop.create, dir);
}

vsf_err_t vk_file_unlink(vk_file_t *dir, char *name)
{
    VSF_FS_ASSERT((dir != NULL) && (name != NULL) && (*name != '\0'));
    VSF_FS_ASSERT(dir->attr & VSF_FILE_ATTR_DIRECTORY);
    VSF_FS_ASSERT(dir->fsop != NULL);
    VSF_FS_ASSERT(dir->fsop->dop.unlink != NULL);

    dir->ctx.unlink.name = name;
    return vsf_eda_call_param_eda(dir->fsop->dop.unlink, dir);
}

#if VSF_FS_CFG_USE_CACHE == ENABLED
vsf_err_t vk_file_sync(vk_file_t *file)
{
    VSF_FS_ASSERT(file != NULL);
    VSF_FS_ASSERT(file->fsop != NULL);
    VSF_FS_ASSERT(file->fsop->fop.sync != NULL);
    return vsf_eda_call_param_eda(dir->fsop->fop.sync, file);
}
#endif

// dummy
void vk_file_set_io_result(vk_file_t *file, vsf_err_t err, int_fast32_t size)
{
    vk_file_set_result(file, err);
    if (file->ctx.io.result != NULL) {
        *file->ctx.io.result = size;
    }
}

void vk_file_set_result(vk_file_t *file, vsf_err_t err)
{
    file->ctx.err = err;
}

void vk_file_return(vk_file_t *file, vsf_err_t err)
{
    vk_file_set_result(file, err);
    vsf_eda_return();
}

void vk_dummyfs_succeed(uintptr_t target, vsf_evt_t evt)
{
    vk_file_return((vk_file_t *)target, VSF_ERR_NONE);
}

void vk_dummyfs_not_support(uintptr_t target, vsf_evt_t evt)
{
    vk_file_return((vk_file_t *)target, VSF_ERR_NOT_SUPPORT);
}

// vfs
static void __vk_vfs_mount(uintptr_t target, vsf_evt_t evt)
{
    vk_vfs_file_t *dir = (vk_vfs_file_t *)target;
    switch (evt) {
    case VSF_EVT_INIT:
        if (VSF_ERR_NONE == vsf_eda_call_param_eda(dir->subfs.op->mount, dir)) {
            break;
        }

        dir->ctx.err = VSF_ERR_NOT_ENOUGH_RESOURCES;
    case VSF_EVT_RETURN:
        if (VSF_ERR_NONE == dir->ctx.err) {
            dir->attr |= VSF_VFS_FILE_ATTR_MOUNTED;
        }
        vsf_eda_return();
        break;
    }
}

static void __vk_vfs_unmount(uintptr_t target, vsf_evt_t evt)
{
    vk_vfs_file_t *dir = (vk_vfs_file_t *)target;
    switch (evt) {
    case VSF_EVT_INIT:
        if (VSF_ERR_NONE == vsf_eda_call_param_eda(dir->subfs.op->unmount, dir)) {
            break;
        }

        dir->ctx.err = VSF_ERR_NOT_ENOUGH_RESOURCES;
    case VSF_EVT_RETURN:
        dir->attr &= ~VSF_VFS_FILE_ATTR_MOUNTED;
        vsf_eda_return();
        break;
    }
}

static vk_vfs_file_t * __vk_vfs_lookup_imp(vk_vfs_file_t *dir, char *name, uint32_t *idx)
{
    vk_vfs_file_t *child;
    uint32_t tmp_idx;

    if (NULL == idx) {
        VSF_FS_ASSERT(name != NULL);
        idx = &tmp_idx;
    }

    vsf_dlist_peek_head(vk_vfs_file_t, use_as__vsf_dlist_node_t, &dir->d.child_list, child);
    while (child != NULL) {
        if (    (name && vk_file_is_match(name, child->name))
            ||  (!name && !*idx)) {
            break;
        }
        (*idx)--;
        vsf_dlist_peek_next(vk_vfs_file_t, use_as__vsf_dlist_node_t, child, child);
    }
    return child;
}

static void __vk_vfs_lookup(uintptr_t target, vsf_evt_t evt)
{
    vk_vfs_file_t *dir = (vk_vfs_file_t *)target;
    switch (evt) {
    case VSF_EVT_INIT:
        if (dir->attr & VSF_VFS_FILE_ATTR_MOUNTED) {
            dir->subfs.root->ctx.lookup = dir->ctx.lookup;
            if (VSF_ERR_NONE == vsf_eda_call_param_eda(dir->subfs.op->dop.lookup, dir->subfs.root)) {
                break;
            }

            dir->ctx.err = VSF_ERR_NOT_ENOUGH_RESOURCES;
        } else {
            *dir->ctx.lookup.result = (vk_file_t *)__vk_vfs_lookup_imp(dir, dir->ctx.lookup.name, &dir->ctx.lookup.idx);
            dir->ctx.err = (*dir->ctx.lookup.result != NULL) ? VSF_ERR_NONE : VSF_ERR_NOT_AVAILABLE;
        }
    case VSF_EVT_RETURN:
        if (dir->attr & VSF_VFS_FILE_ATTR_MOUNTED) {
            dir->ctx.err = dir->subfs.root->ctx.err;
            dir->ctx.lookup = dir->subfs.root->ctx.lookup;
        }
        vsf_eda_return();
        break;
    }
}

static void __vk_vfs_create(uintptr_t target, vsf_evt_t evt)
{
    vk_vfs_file_t *dir = (vk_vfs_file_t *)target;
    vk_vfs_file_t *new_file = __vk_vfs_lookup_imp(dir, dir->ctx.create.name, NULL);

    if (new_file != NULL) {
        dir->ctx.err = VSF_ERR_ALREADY_EXISTS;
        goto do_return;
    }

    new_file = (vk_vfs_file_t *)vk_file_alloc(sizeof(vk_vfs_file_t));
    if (NULL == new_file) {
        dir->ctx.err = VSF_ERR_NOT_ENOUGH_RESOURCES;
        goto do_return;
    }

    new_file->name = dir->ctx.create.name;
    new_file->attr |= dir->ctx.create.attr;
    new_file->fsop = &__vk_vfs_op;
    vsf_dlist_add_to_tail(vk_vfs_file_t, use_as__vsf_dlist_node_t, &dir->d.child_list, new_file);
    dir->ctx.err = VSF_ERR_NONE;

do_return:
    vsf_eda_return();
}

static void __vk_vfs_unlink(uintptr_t target, vsf_evt_t evt)
{
    vk_vfs_file_t *dir = (vk_vfs_file_t *)target;
    switch (evt) {
    case VSF_EVT_INIT:
        if (dir->attr & VSF_VFS_FILE_ATTR_MOUNTED) {
            dir->subfs.root->ctx.unlink = dir->ctx.unlink;
            if (VSF_ERR_NONE == vsf_eda_call_param_eda(dir->subfs.op->dop.unlink, dir->subfs.root)) {
                break;
            }

            dir->ctx.err = VSF_ERR_NOT_ENOUGH_RESOURCES;
        } else {
            vk_vfs_file_t *child = __vk_vfs_lookup_imp(dir, dir->ctx.unlink.name, NULL);
            if (child != NULL) {
                vsf_dlist_remove(vk_vfs_file_t, use_as__vsf_dlist_node_t, &dir->d.child_list, child);
            }
            dir->ctx.err = (NULL == child) ? VSF_ERR_NOT_AVAILABLE : VSF_ERR_NONE;
        }
    case VSF_EVT_RETURN:
        if (dir->attr & VSF_VFS_FILE_ATTR_MOUNTED) {
            dir->ctx.err = dir->subfs.root->ctx.err;
            dir->ctx.unlink = dir->subfs.root->ctx.unlink;
        }
        vsf_eda_return();
        break;
    }
}

#endif
