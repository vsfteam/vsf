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
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

static void __vk_memfs_mount(uintptr_t, vsf_evt_t);
static void __vk_memfs_lookup(uintptr_t, vsf_evt_t);
static void __vk_memfs_read(uintptr_t, vsf_evt_t);
static void __vk_memfs_write(uintptr_t, vsf_evt_t);

/*============================ GLOBAL VARIABLES ==============================*/

const vk_fs_op_t vk_memfs_op = {
    .mount          = __vk_memfs_mount,
    .unmount        = vk_dummyfs_succeed,
#if VSF_FS_CFG_USE_CACHE == ENABLED
    .sync           = vk_file_dummy,
#endif
    .fop            = {
        .read       = __vk_memfs_read,
        .write      = __vk_memfs_write,
        .close      = vk_dummyfs_succeed,
        .resize     = vk_dummyfs_not_support,
    },
    .dop            = {
        .lookup     = __vk_memfs_lookup,
        .create     = vk_dummyfs_not_support,
        .unlink     = vk_dummyfs_not_support,
        .chmod      = vk_dummyfs_not_support,
        .rename     = vk_dummyfs_not_support,
    },
};

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

static void __vk_memfs_mount(uintptr_t target, vsf_evt_t evt)
{
    vk_vfs_file_t *dir = (vk_vfs_file_t *)target;
    vk_memfs_info_t *fsinfo = dir->subfs.data;
    VSF_FS_ASSERT((fsinfo != NULL) && (fsinfo->root.d.child_size >= sizeof(vk_memfs_file_t)));
    fsinfo->root.attr = VSF_FILE_ATTR_DIRECTORY;
    __vk_memfs_init(fsinfo, &fsinfo->root);
    dir->subfs.root = &fsinfo->root.use_as__vk_file_t;
    vk_file_return(&dir->use_as__vk_file_t, VSF_ERR_NONE);
}

static void __vk_memfs_lookup(uintptr_t target, vsf_evt_t evt)
{
    vk_memfs_file_t *dir = (vk_memfs_file_t *)target;
    vk_memfs_file_t *child = dir->d.child;
    const char *name = dir->ctx.lookup.name;
    uint_fast32_t idx = dir->ctx.lookup.idx;
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
        *dir->ctx.lookup.result = &child->use_as__vk_file_t;
        vk_file_return(&dir->use_as__vk_file_t, VSF_ERR_NONE);
    } else {
        *dir->ctx.lookup.result = NULL;
        vk_file_return(&dir->use_as__vk_file_t, VSF_ERR_NOT_AVAILABLE);
    }
}

static void __vk_memfs_read(uintptr_t target, vsf_evt_t evt)
{
    vk_memfs_file_t *file = (vk_memfs_file_t *)target;

    switch (evt) {
    case VSF_EVT_INIT:
        if (file->f.buff != NULL) {
            uint_fast64_t offset = file->ctx.io.offset;
            uint_fast32_t size = file->ctx.io.size;
            uint8_t *buff = file->ctx.io.buff;
            int_fast32_t rsize = 0;

            if (offset < file->size) {
                rsize = min(size, file->size - offset);
                memcpy(buff, &file->f.buff[offset], rsize);
            }
            if (file->ctx.io.result != NULL) {
                *file->ctx.io.result = rsize;
            }
            vk_file_return(&file->use_as__vk_file_t, VSF_ERR_NONE);
        } else if (file->callback.read != NULL) {
            if (VSF_ERR_NONE != vsf_eda_call_param_eda(file->callback.read, file)) {
                vk_file_return(&file->use_as__vk_file_t, VSF_ERR_NOT_ENOUGH_RESOURCES);
            }
        } else {
            vk_file_return(&file->use_as__vk_file_t, VSF_ERR_NOT_ACCESSABLE);
        }
        break;
    case VSF_EVT_RETURN:
        vk_file_return(&file->use_as__vk_file_t, file->ctx.err);
        break;
    }
}

static void __vk_memfs_write(uintptr_t target, vsf_evt_t evt)
{
    vk_memfs_file_t *file = (vk_memfs_file_t *)target;

    switch (evt) {
    case VSF_EVT_INIT:
        if (file->f.buff != NULL) {
            uint_fast64_t offset = file->ctx.io.offset;
            uint_fast32_t size = file->ctx.io.size;
            uint8_t *buff = file->ctx.io.buff;
            int_fast32_t wsize = 0;

            if (offset < file->size) {
                wsize = min(size, file->size - offset);
                memcpy(&file->f.buff[offset], buff, wsize);
            }
            if (file->ctx.io.result != NULL) {
                *file->ctx.io.result = wsize;
            }
            vk_file_return(&file->use_as__vk_file_t, VSF_ERR_NONE);
        } else if (file->callback.write != NULL) {
            if (VSF_ERR_NONE != vsf_eda_call_param_eda(file->callback.write, file)) {
                vk_file_return(&file->use_as__vk_file_t, VSF_ERR_NOT_ENOUGH_RESOURCES);
            }
        } else {
            vk_file_return(&file->use_as__vk_file_t, VSF_ERR_NOT_ACCESSABLE);
        }
        break;
    case VSF_EVT_RETURN:
        vk_file_return(&file->use_as__vk_file_t, file->ctx.err);
        break;
    }
}

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
