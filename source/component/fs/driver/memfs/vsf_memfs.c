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

#if VSF_USE_FS == ENABLED && VSF_FS_USE_MEMFS == ENABLED

#define __VSF_FS_CLASS_INHERIT__
#define __VSF_MEMFS_CLASS_IMPLEMENT
#include "../../vsf_fs.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

dcl_vsf_peda_methods(static, __vk_memfs_mount)
dcl_vsf_peda_methods(static, __vk_memfs_lookup)
dcl_vsf_peda_methods(static, __vk_memfs_read)
dcl_vsf_peda_methods(static, __vk_memfs_write)

/*============================ GLOBAL VARIABLES ==============================*/

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-function-type"
#endif

const vk_fs_op_t vk_memfs_op = {
    .fn_mount       = (vsf_peda_evthandler_t)vsf_peda_func(__vk_memfs_mount),
    .fn_unmount     = (vsf_peda_evthandler_t)vsf_peda_func(vk_dummyfs_succeed),
#if VSF_FS_CFG_USE_CACHE == ENABLED
    .fn_sync        = (vsf_peda_evthandler_t)vsf_peda_func(vk_dummyfs_succeed),
#endif
    .fop            = {
        .fn_read    = (vsf_peda_evthandler_t)vsf_peda_func(__vk_memfs_read),
        .fn_write   = (vsf_peda_evthandler_t)vsf_peda_func(__vk_memfs_write),
        .fn_close   = (vsf_peda_evthandler_t)vsf_peda_func(vk_dummyfs_succeed),
        .fn_resize  = (vsf_peda_evthandler_t)vsf_peda_func(vk_dummyfs_not_support),
    },
    .dop            = {
        .fn_lookup  = (vsf_peda_evthandler_t)vsf_peda_func(__vk_memfs_lookup),
        .fn_create  = (vsf_peda_evthandler_t)vsf_peda_func(vk_dummyfs_not_support),
        .fn_unlink  = (vsf_peda_evthandler_t)vsf_peda_func(vk_dummyfs_not_support),
        .fn_chmod   = (vsf_peda_evthandler_t)vsf_peda_func(vk_dummyfs_not_support),
        .fn_rename  = (vsf_peda_evthandler_t)vsf_peda_func(vk_dummyfs_not_support),
    },
};

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

static void __vk_memfs_init(vk_memfs_info_t *info, vk_memfs_file_t *file)
{
    file->fsop = &vk_memfs_op;
    if (file->attr & VSF_FILE_ATTR_DIRECTORY) {
        vk_memfs_file_t *child = (vk_memfs_file_t *)file->d.child;
        file->d.child_size = info->root.d.child_size;
        for (uint_fast16_t i = 0; i < file->d.child_num; i++) {
            __vk_memfs_init(info, child);
            child = (vk_memfs_file_t *)((uintptr_t)child + file->d.child_size);
        }
    }
}

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#elif   __IS_COMPILER_LLVM__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#endif

__vsf_component_peda_ifs_entry(__vk_memfs_mount, vk_fs_mount)
{
    vsf_peda_begin();
    vk_vfs_file_t *dir = (vk_vfs_file_t *)&vsf_this;
    vk_memfs_info_t *fsinfo = dir->subfs.data;
    VSF_FS_ASSERT((fsinfo != NULL) && (fsinfo->root.d.child_size >= sizeof(vk_memfs_file_t)));
    fsinfo->root.attr = VSF_FILE_ATTR_DIRECTORY;
    __vk_memfs_init(fsinfo, &fsinfo->root);
    dir->subfs.root = &fsinfo->root.use_as__vk_file_t;
    vsf_eda_return(VSF_ERR_NONE);
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_memfs_lookup, vk_file_lookup)
{
    vsf_peda_begin();
    vk_memfs_file_t *dir = (vk_memfs_file_t *)&vsf_this;
    vk_memfs_file_t *child = dir->d.child;
    const char *name = vsf_local.name;
    uint_fast32_t idx = vsf_local.idx;
    bool found = false;

    for (uint_fast16_t i = 0; i < dir->d.child_num; i++) {
        if (    (name && vk_file_is_match((char *)name, child->name))
            ||  (!name && !idx)) {
            found = true;
            break;
        }
        idx--;
        child = (vk_memfs_file_t *)((uintptr_t)child + dir->d.child_size);
    }

    if (found) {
        *vsf_local.result = &child->use_as__vk_file_t;
        vsf_eda_return(VSF_ERR_NONE);
    } else {
        *vsf_local.result = NULL;
        vsf_eda_return(VSF_ERR_NOT_AVAILABLE);
    }
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_memfs_read, vk_file_read)
{
    vsf_peda_begin();
    vk_memfs_file_t *file = (vk_memfs_file_t *)&vsf_this;

    switch (evt) {
    case VSF_EVT_INIT:
        if (file->f.buff != NULL) {
            uint_fast64_t offset = vsf_local.offset;
            uint_fast32_t size = vsf_local.size;
            uint8_t *buff = vsf_local.buff;
            int_fast32_t rsize = 0;

            if (offset < file->size) {
                rsize = min(size, file->size - offset);
                memcpy(buff, &file->f.buff[offset], rsize);
            }
            vsf_eda_return(rsize);
        } else if (file->callback.read != NULL) {
            vsf_err_t err;
            __vsf_component_call_peda_ifs(vk_memfs_callback_read, err, file->callback.read, 0, file,
                .offset     = vsf_local.offset,
                .size       = vsf_local.size,
                .buff       = vsf_local.buff,
            );
            if (VSF_ERR_NONE != err) {
                vsf_eda_return(VSF_ERR_NOT_ENOUGH_RESOURCES);
            }
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

__vsf_component_peda_ifs_entry(__vk_memfs_write, vk_file_write)
{
    vsf_peda_begin();
    vk_memfs_file_t *file = (vk_memfs_file_t *)&vsf_this;

    switch (evt) {
    case VSF_EVT_INIT:
        if (file->f.buff != NULL) {
            uint_fast64_t offset = vsf_local.offset;
            uint_fast32_t size = vsf_local.size;
            uint8_t *buff = vsf_local.buff;
            int_fast32_t wsize = 0;

            if (offset < file->size) {
                wsize = min(size, file->size - offset);
                memcpy(&file->f.buff[offset], buff, wsize);
            }
            vsf_eda_return(wsize);
        } else if (file->callback.write != NULL) {
            vsf_err_t err;
            __vsf_component_call_peda_ifs(vk_memfs_callback_write, err, file->callback.write, 0, file,
                .offset     = vsf_local.offset,
                .size       = vsf_local.size,
                .buff       = vsf_local.buff,
            );
            if (VSF_ERR_NONE != err) {
                vsf_eda_return(VSF_ERR_NOT_ENOUGH_RESOURCES);
            }
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
#elif   __IS_COMPILER_LLVM__
#   pragma clang diagnostic pop
#endif

void vk_memfs_init(vk_memfs_info_t *memfs)
{
    VSF_FS_ASSERT(memfs != NULL);
    __vk_memfs_init(memfs, &memfs->root);
}

vk_memfs_file_t * vk_memfs_open(vk_memfs_file_t *dir, const char *path)
{
    VSF_FS_ASSERT(dir != NULL);
    VSF_FS_ASSERT(path != NULL);

    while (path[0] != '\0') {
        if (!(dir->attr & VSF_FILE_ATTR_DIRECTORY)) {
            return NULL;
        }
        if (vk_file_is_div(path[0])) {
            path++;
            if ('\0' == path[0]) {
                break;
            }
        }

        uint16_t child_num = dir->d.child_num, child_size = dir->d.child_size;
        bool found = false;
        dir = dir->d.child;
        for (uint_fast16_t i = 0; i < child_num; i++) {
            if (vk_file_is_match((char *)path, dir->name)) {
                path += strlen(dir->name);
                found = true;
                break;
            }
            dir = (vk_memfs_file_t *)((uintptr_t)dir + child_size);
        }

        if (!found) {
            return NULL;
        }
    }
    return dir;
}

int_fast32_t vk_memfs_read(vk_memfs_file_t *file, uint_fast64_t addr, uint_fast32_t size, uint8_t *buff)
{
    VSF_FS_ASSERT(file != NULL);
    VSF_FS_ASSERT(!(file->attr & VSF_FILE_ATTR_DIRECTORY));
    if (addr >= file->size) {
        return -1;
    }

    int_fast32_t rsize = min(size, file->size - addr);
    memcpy(buff, &file->f.buff[addr], rsize);
    return rsize;
}

int_fast32_t vk_memfs_write(vk_memfs_file_t *file, uint_fast64_t addr, uint_fast32_t size, uint8_t *buff)
{
    VSF_FS_ASSERT(file != NULL);
    VSF_FS_ASSERT(!(file->attr & VSF_FILE_ATTR_DIRECTORY));
    if (addr >= file->size) {
        return -1;
    }

    int_fast32_t wsize = min(size, file->size - addr);
    memcpy(&file->f.buff[addr], buff, wsize);
    return wsize;
}

#endif
