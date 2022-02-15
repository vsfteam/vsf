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

#if VSF_USE_FS == ENABLED && VSF_FS_USE_LITTLEFS == ENABLED

#define __VSF_FS_CLASS_INHERIT__
#define __VSF_LITTLEFS_CLASS_IMPLEMENT
#include "../../vsf_fs.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum vk_lfs_operation_t {
    VK_LFS_MOUNT,
    VK_LFS_UNMOUNT,
    VK_LFS_LOOKUP,
    VK_LFS_CLOSE_FILE,
    VK_LFS_CLOSE_DIR,
    VK_LFS_READ_FILE,
    VK_LFS_WRITE_FILE,
} vk_lfs_operation_t;

/*============================ PROTOTYPES ====================================*/

dcl_vsf_peda_methods(static, __vk_lfs_mount)
dcl_vsf_peda_methods(static, __vk_lfs_unmount)
dcl_vsf_peda_methods(static, __vk_lfs_lookup)
dcl_vsf_peda_methods(static, __vk_lfs_read)
dcl_vsf_peda_methods(static, __vk_lfs_write)
dcl_vsf_peda_methods(static, __vk_lfs_close)

/*============================ GLOBAL VARIABLES ==============================*/

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-function-type"
#endif

const vk_fs_op_t vk_lfs_op = {
    .fn_mount       = (vsf_peda_evthandler_t)vsf_peda_func(__vk_lfs_mount),
    .fn_unmount     = (vsf_peda_evthandler_t)vsf_peda_func(__vk_lfs_unmount),
#if VSF_FS_CFG_USE_CACHE == ENABLED
    .fn_sync        = (vsf_peda_evthandler_t)vsf_peda_func(vk_fsop_succeed),
#endif
    .fn_rename      = (vsf_peda_evthandler_t)vsf_peda_func(vk_fsop_not_support),
    .fop            = {
        .fn_read    = (vsf_peda_evthandler_t)vsf_peda_func(__vk_lfs_read),
        .fn_write   = (vsf_peda_evthandler_t)vsf_peda_func(__vk_lfs_write),
        .fn_close   = (vsf_peda_evthandler_t)vsf_peda_func(__vk_lfs_close),
        .fn_setsize = (vsf_peda_evthandler_t)vsf_peda_func(vk_fsop_not_support),
        .fn_setpos  = (vsf_peda_evthandler_t)vsf_peda_func(vk_fsop_not_support),
    },
    .dop            = {
        .fn_lookup  = (vsf_peda_evthandler_t)vsf_peda_func(__vk_lfs_lookup),
        .fn_create  = (vsf_peda_evthandler_t)vsf_peda_func(vk_fsop_not_support),
        .fn_unlink  = (vsf_peda_evthandler_t)vsf_peda_func(vk_fsop_not_support),
        .fn_chmod   = (vsf_peda_evthandler_t)vsf_peda_func(vk_fsop_not_support),
    },
};

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

static void __vk_lfs_thread(vsf_thread_cb_t *thread)
{
    vk_lfs_info_t *fsinfo = container_of(thread, vk_lfs_info_t, thread_cb);
    lfs_t *lfs = &fsinfo->lfs;
    int ret;

    ret = lfs_mount(lfs, &fsinfo->config);
    if (ret < 0) {
        lfs_format(lfs, &fsinfo->config);
        ret = lfs_mount(lfs, &fsinfo->config);
        VSF_FS_ASSERT(LFS_ERR_OK == ret);
        ret = lfs_dir_open(lfs, &fsinfo->root.lfs_dir, "/");
        VSF_FS_ASSERT(LFS_ERR_OK == ret);
    }

    while (true) {
        fsinfo->result.ret = ret;
        VSF_FS_ASSERT(fsinfo->caller != NULL);
        vsf_eda_post_evt(fsinfo->caller, VSF_EVT_RETURN);
        if (    ((ret < 0) && (VK_LFS_MOUNT == fsinfo->op))
            ||  (VK_LFS_UNMOUNT == fsinfo->op)) {
            break;
        }

        vsf_thread_sem_pend(&fsinfo->sem, -1);

        switch (fsinfo->op) {
        case VK_LFS_UNMOUNT:
            lfs_dir_close(lfs, &fsinfo->root.lfs_dir);
            ret = lfs_unmount(lfs);
            break;
        case VK_LFS_LOOKUP: {
                vk_lfs_file_t *dir = fsinfo->param.open.dir;
                const char *name = fsinfo->param.open.name;
                struct lfs_info lfsinfo;

                char path[LFS_NAME_MAX];
                path[0] = '\0';
                while (dir->name != NULL) {
                    VSF_FS_ASSERT(strlen(path) + strlen(dir->name) <= sizeof(path) - 1);
                    strcat(path, dir->name);
                }

                ret = 0;
                if (name != NULL) {
                    // by name
                    VSF_FS_ASSERT(strlen(path) + strlen(name) <= sizeof(path) - 1);
                    strcat(path, name);
                } else {
                scan_next:
                    lfs_dir_read(lfs, &dir->lfs_dir, &lfsinfo);
                    if (0 == strlen(lfsinfo.name)) {
                        lfs_dir_rewind(lfs, &dir->lfs_dir);
                        ret = -1;
                        break;
                    }

                    if (    !strcmp(lfsinfo.name, ".")
                        ||  !strcmp(lfsinfo.name, "..")) {
                        goto scan_next;
                    }

                    VSF_FS_ASSERT(strlen(path) + strlen(lfsinfo.name) <= sizeof(path) - 1);
                    name = &path[strlen(path)];
                    strcat(path, lfsinfo.name);
                }

                if (0 == ret) {
                    vk_lfs_file_t *output_file = (vk_lfs_file_t *)vk_file_alloc(sizeof(vk_lfs_file_t));
                    if (NULL == output_file) {
                        ret = -1;
                        break;
                    }
                    output_file->name = vsf_heap_malloc(strlen(name) + 1);
                    if (NULL == output_file) {
                        vk_file_free(&output_file->use_as__vk_file_t);
                        ret = -1;
                        break;
                    }
                    strcpy(output_file->name, name);

                    lfs_stat(lfs, path, &lfsinfo);
                    switch (lfsinfo.type) {
                    case LFS_TYPE_REG:
                        output_file->attr = VSF_FILE_ATTR_READ | VSF_FILE_ATTR_WRITE;
                        lfs_file_open(lfs, &output_file->lfs_file, path, LFS_O_RDWR);
                        output_file->size = lfs_file_size(lfs, &output_file->lfs_file);
                        break;
                    case LFS_TYPE_DIR:
                        output_file->attr = VSF_FILE_ATTR_DIRECTORY;
                        lfs_dir_open(lfs, &output_file->lfs_dir, path);
                        break;
                    }

                    output_file->fsop = &vk_lfs_op;
                    output_file->info = fsinfo;
                    fsinfo->result.open.file = output_file;
                }
            }
            break;
        case VK_LFS_CLOSE_FILE:
            ret = lfs_file_close(lfs, fsinfo->param.close_file.file);
            break;
        case VK_LFS_CLOSE_DIR:
            ret = lfs_dir_close(lfs, fsinfo->param.close_dir.dir);
            break;
        case VK_LFS_READ_FILE: {
                lfs_file_t *file = fsinfo->param.read_file.file;
                void *buffer = fsinfo->param.read_file.buffer;
                lfs_size_t size = fsinfo->param.read_file.size;
                fsinfo->result.read_file.size = lfs_file_read(lfs, file, buffer, size);
            }
            break;
        case VK_LFS_WRITE_FILE: {
                lfs_file_t *file = fsinfo->param.read_file.file;
                void *buffer = fsinfo->param.read_file.buffer;
                lfs_size_t size = fsinfo->param.read_file.size;
                fsinfo->result.read_file.size = lfs_file_write(lfs, file, buffer, size);
            }
            break;
        }
    }
}

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#endif

__vsf_component_peda_ifs_entry(__vk_lfs_mount, vk_fs_mount)
{
    vsf_peda_begin();
    vk_vfs_file_t *dir = (vk_vfs_file_t *)&vsf_this;
    vk_lfs_info_t *fsinfo = dir->subfs.data;
    VSF_FS_ASSERT(fsinfo != NULL);

    switch (evt) {
    case VSF_EVT_INIT:
        fsinfo->caller = vsf_eda_get_cur();

        vsf_eda_crit_init(&fsinfo->crit);
        vsf_eda_sem_init(&fsinfo->sem);
        fsinfo->thread_cb.entry = __vk_lfs_thread;
        fsinfo->thread_cb.stack_size = sizeof(fsinfo->thread_stack);
        fsinfo->thread_cb.stack = fsinfo->thread_stack;
        fsinfo->op = VK_LFS_MOUNT;
        vsf_thread_start(&fsinfo->thread, &fsinfo->thread_cb, vsf_prio_inherit);
        break;
    case VSF_EVT_RETURN:
        if (0 == fsinfo->result.ret) {
            fsinfo->root.attr = VSF_FILE_ATTR_DIRECTORY;
            fsinfo->root.name = NULL;
            fsinfo->root.info = fsinfo;
            dir->subfs.root = &fsinfo->root.use_as__vk_file_t;
        }
        vsf_eda_return(fsinfo->result.ret < 0 ? VSF_ERR_FAIL : VSF_ERR_NONE);
        break;
    }

    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_lfs_unmount, vk_fs_unmount)
{
    vsf_peda_begin();
    vk_vfs_file_t *dir = (vk_vfs_file_t *)&vsf_this;
    vk_lfs_info_t *fsinfo = dir->subfs.data;
    VSF_FS_ASSERT(fsinfo != NULL);

    switch (evt) {
    case VSF_EVT_INIT:
        if (VSF_ERR_NONE != vsf_eda_crit_enter(&fsinfo->crit)) {
            break;
        }
        // fall through
    case VSF_EVT_SYNC:
        fsinfo->caller = vsf_eda_get_cur();

        fsinfo->op = VK_LFS_UNMOUNT;
        vsf_eda_sem_post(&fsinfo->sem);
        break;
    case VSF_EVT_RETURN: {
            int ret = fsinfo->result.ret;
            vsf_eda_crit_leave(&fsinfo->crit);
            vsf_eda_return(ret < 0 ? VSF_ERR_FAIL : VSF_ERR_NONE);
        }
        break;
    }

    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_lfs_lookup, vk_file_lookup)
{
    vsf_peda_begin();
    vk_lfs_file_t *dir = (vk_lfs_file_t *)&vsf_this;
    vk_lfs_info_t *fsinfo = dir->info;
    const char *name = vsf_local.name;

    switch (evt) {
    case VSF_EVT_INIT:
        if (VSF_ERR_NONE != vsf_eda_crit_enter(&fsinfo->crit)) {
            break;
        }
        // fall through
    case VSF_EVT_SYNC:
        fsinfo->caller = vsf_eda_get_cur();

        fsinfo->param.open.dir = dir;
        fsinfo->param.open.name = name;
        fsinfo->op = VK_LFS_LOOKUP;
        vsf_eda_sem_post(&fsinfo->sem);
        break;
    case VSF_EVT_RETURN: {
            int ret = fsinfo->result.ret;
            vk_lfs_file_t *result_file = fsinfo->result.open.file;
            vsf_eda_crit_leave(&fsinfo->crit);

            if ((0 == ret) && (result_file != NULL)) {
                *vsf_local.result = &result_file->use_as__vk_file_t;
                dir->pos++;
            }
            vsf_eda_return(ret < 0 ? VSF_ERR_FAIL : VSF_ERR_NONE);
        }
        break;
    }
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_lfs_close, vk_file_close)
{
    vsf_peda_begin();
    vk_lfs_file_t *file = (vk_lfs_file_t *)&vsf_this;
    vk_lfs_info_t *fsinfo = file->info;

    switch (evt) {
    case VSF_EVT_INIT:
        if (VSF_ERR_NONE != vsf_eda_crit_enter(&fsinfo->crit)) {
            break;
        }
        // fall through
    case VSF_EVT_SYNC:
        fsinfo->caller = vsf_eda_get_cur();

        if (file->attr & VSF_FILE_ATTR_DIRECTORY) {
            fsinfo->param.close_dir.dir = &file->lfs_dir;
            fsinfo->op = VK_LFS_CLOSE_DIR;
        } else {
            fsinfo->param.close_file.file = &file->lfs_file;
            fsinfo->op = VK_LFS_CLOSE_FILE;
        }
        vsf_eda_sem_post(&fsinfo->sem);
        break;
    case VSF_EVT_RETURN: {
            int ret = fsinfo->result.ret;
            vsf_eda_crit_leave(&fsinfo->crit);

            if (file->name != NULL) {
                vsf_heap_free(file->name);
            }
            vsf_eda_return(ret < 0 ? VSF_ERR_FAIL : VSF_ERR_NONE);
        }
        break;
    }
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_lfs_read, vk_file_read)
{
    vsf_peda_begin();
    vk_lfs_file_t *file = (vk_lfs_file_t *)&vsf_this;
    vk_lfs_info_t *fsinfo = file->info;

    switch (evt) {
    case VSF_EVT_INIT:
        if (VSF_ERR_NONE != vsf_eda_crit_enter(&fsinfo->crit)) {
            break;
        }
        // fall through
    case VSF_EVT_SYNC:
        fsinfo->caller = vsf_eda_get_cur();

        fsinfo->param.read_file.file = &file->lfs_file;
        fsinfo->param.read_file.buffer = vsf_local.buff;
        fsinfo->param.read_file.size = vsf_local.size;
        fsinfo->op = VK_LFS_READ_FILE;
        vsf_eda_sem_post(&fsinfo->sem);
        break;
    case VSF_EVT_RETURN: {
            lfs_ssize_t size = fsinfo->result.read_file.size;
            int ret = fsinfo->result.ret;
            vsf_eda_crit_leave(&fsinfo->crit);
            vsf_eda_return(ret < 0 ? -1 : size);
        }
        break;
    }
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_lfs_write, vk_file_write)
{
    vsf_peda_begin();
    vk_lfs_file_t *file = (vk_lfs_file_t *)&vsf_this;
    vk_lfs_info_t *fsinfo = file->info;

    switch (evt) {
    case VSF_EVT_INIT:
        if (VSF_ERR_NONE != vsf_eda_crit_enter(&fsinfo->crit)) {
            break;
        }
        // fall through
    case VSF_EVT_SYNC:
        fsinfo->caller = vsf_eda_get_cur();

        fsinfo->param.write_file.file = &file->lfs_file;
        fsinfo->param.write_file.buffer = vsf_local.buff;
        fsinfo->param.write_file.size = vsf_local.size;
        fsinfo->op = VK_LFS_WRITE_FILE;
        vsf_eda_sem_post(&fsinfo->sem);
        break;
    case VSF_EVT_RETURN: {
            lfs_ssize_t size = fsinfo->result.read_file.size;
            int ret = fsinfo->result.ret;
            vsf_eda_crit_leave(&fsinfo->crit);
            vsf_eda_return(ret < 0 ? -1 : size);
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

#endif
