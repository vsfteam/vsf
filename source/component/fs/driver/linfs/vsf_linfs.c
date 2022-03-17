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

#if VSF_USE_FS == ENABLED && VSF_FS_USE_LINFS == ENABLED

#define __VSF_FS_CLASS_INHERIT__
#define __VSF_LINFS_CLASS_IMPLEMENT
#define __VSF_HEADER_ONLY_SHOW_FS_INFO__
#include "../../vsf_fs.h"
#include "./vsf_linfs.h"

#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

dcl_vsf_peda_methods(static, __vk_linfs_mount)
dcl_vsf_peda_methods(static, __vk_linfs_lookup)
dcl_vsf_peda_methods(static, __vk_linfs_read)
dcl_vsf_peda_methods(static, __vk_linfs_write)
dcl_vsf_peda_methods(static, __vk_linfs_close)
dcl_vsf_peda_methods(static, __vk_linfs_setpos)

/*============================ GLOBAL VARIABLES ==============================*/

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-function-type"
#endif

const vk_fs_op_t vk_linfs_op = {
    .fn_mount       = (vsf_peda_evthandler_t)vsf_peda_func(__vk_linfs_mount),
    .fn_unmount     = (vsf_peda_evthandler_t)vsf_peda_func(vk_fsop_succeed),
#if VSF_FS_CFG_USE_CACHE == ENABLED
    .fn_sync        = (vsf_peda_evthandler_t)vsf_peda_func(vk_fsop_succeed),
#endif
    .fn_rename      = (vsf_peda_evthandler_t)vsf_peda_func(vk_fsop_not_support),
    .fop            = {
        .fn_read    = (vsf_peda_evthandler_t)vsf_peda_func(__vk_linfs_read),
        .fn_write   = (vsf_peda_evthandler_t)vsf_peda_func(__vk_linfs_write),
        .fn_close   = (vsf_peda_evthandler_t)vsf_peda_func(__vk_linfs_close),
        .fn_setsize = (vsf_peda_evthandler_t)vsf_peda_func(vk_fsop_not_support),
        .fn_setpos  = (vsf_peda_evthandler_t)vsf_peda_func(__vk_linfs_setpos),
    },
    .dop            = {
        .fn_lookup  = (vsf_peda_evthandler_t)vsf_peda_func(__vk_linfs_lookup),
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

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#endif

__vsf_component_peda_ifs_entry(__vk_linfs_mount, vk_fs_mount)
{
    vsf_peda_begin();
    vk_vfs_file_t *dir = (vk_vfs_file_t *)&vsf_this;
    vk_linfs_info_t *fsinfo = dir->subfs.data;
    VSF_FS_ASSERT((fsinfo != NULL) && (fsinfo->root.name != NULL));

    struct stat statbuf;
    if (0 != stat(fsinfo->root.name, &statbuf)) {
        vsf_eda_return(VSF_ERR_NOT_AVAILABLE);
        return;
    }
    if (!S_ISDIR(statbuf.st_mode)) {
        vsf_eda_return(VSF_ERR_INVALID_PARAMETER);
        return;
    }

    fsinfo->root.fullpath = fsinfo->root.name;
    fsinfo->root.d.dir = NULL;
    dir->subfs.root = &fsinfo->root.use_as__vk_file_t;
    vsf_eda_return(VSF_ERR_NONE);
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_linfs_lookup, vk_file_lookup)
{
    vsf_peda_begin();
    vk_linfs_file_t *dir = (vk_linfs_file_t *)&vsf_this;
    const char *name = vsf_local.name;
    uint_fast32_t idx = dir->pos;
    vsf_err_t err = VSF_ERR_NONE;

    char path[PATH_MAX];
    uint_fast16_t len = strlen(dir->fullpath);
    uint_fast16_t namelen;
    strcpy(path, dir->fullpath);

    *vsf_local.result = NULL;
    if (name != NULL) {
        const char *ptr = name;
        while (*ptr != '\0') {
            if (vk_file_is_div(*ptr)) {
                break;
            }
            ptr++;
        }
        if ('\0' == *ptr) {
            namelen = strlen(name);
        } else {
            namelen = ptr - name;
        }
        if ((len + namelen + 2) > PATH_MAX) {
            err = VSF_ERR_FAIL;
            goto do_return;
        }
        path[len] = '/';
        memcpy(&path[len + 1], name, namelen);
        path[len + namelen + 1] = '\0';
    } else {
        struct dirent *entry;
        if (NULL == dir->d.dir) {
            dir->d.dir = opendir(path);
            VSF_FS_ASSERT(dir->d.dir != NULL);
        }
    read_next:
        entry = readdir((DIR *)dir->d.dir);
        if (NULL == entry) {
            closedir((DIR *)dir->d.dir);
            dir->d.dir = NULL;
            dir->pos = 0;

            err = VSF_ERR_NOT_AVAILABLE;
            vsf_eda_return(err);
            return;
        }

        if (    !strcmp(entry->d_name, ".")
            ||  !strcmp(entry->d_name, "..")) {
            goto read_next;
        }

        namelen = strlen(entry->d_name);
        if ((len + namelen + 2) > PATH_MAX) {
            err = VSF_ERR_FAIL;
            goto do_return;
        }
        path[len + 1] = '\0';
        strcat(path, "/");
        strcat(path, entry->d_name);
    }

    struct stat statbuf;
    if (0 != stat(path, &statbuf)) {
    do_not_available:
        err = VSF_ERR_NOT_AVAILABLE;
        goto do_return;
    }

    vk_linfs_file_t *linfs_file = (vk_linfs_file_t *)vk_file_alloc(sizeof(*linfs_file));
    if (NULL == linfs_file) {
        err = VSF_ERR_NOT_ENOUGH_RESOURCES;
        goto do_return;
    }
    linfs_file->name = vsf_heap_malloc(namelen + 1);
    if (NULL == linfs_file->name) {
        err = VSF_ERR_NOT_ENOUGH_RESOURCES;
        goto do_free_and_return;
    }
    strcpy(linfs_file->name, vk_file_getfilename(path));
    linfs_file->fullpath = vsf_heap_malloc(strlen(dir->fullpath) + 1 /* '/' */ + namelen + 1 /* '\0' */);
    if (NULL == linfs_file->fullpath) {
        err = VSF_ERR_NOT_ENOUGH_RESOURCES;
        goto do_free_and_return;
    }
    strcpy(linfs_file->fullpath, dir->fullpath);
    strcat(linfs_file->fullpath, "/");
    strcat(linfs_file->fullpath, linfs_file->name);
    linfs_file->fsop = &vk_linfs_op;

    if (S_ISDIR(statbuf.st_mode)) {
        linfs_file->attr |= VSF_FILE_ATTR_DIRECTORY;
    } else {
        linfs_file->f.fd = open(path, 0);
        if (-1 == linfs_file->f.fd) {
            err = VSF_ERR_NOT_AVAILABLE;
            goto do_free_and_return;
        }
        linfs_file->size = lseek(linfs_file->f.fd, 0, SEEK_END);
        lseek(linfs_file->f.fd, 0, SEEK_SET);
    }
    linfs_file->attr |= VSF_FILE_ATTR_READ | VSF_FILE_ATTR_WRITE;
    *vsf_local.result = &linfs_file->use_as__vk_file_t;
    goto do_return;

do_free_and_return:
    if (linfs_file->name != NULL) {
        vsf_heap_free(linfs_file->name);
    }
    vk_file_free(&linfs_file->use_as__vk_file_t);
do_return:
    vsf_eda_return(err);
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_linfs_setpos, vk_file_setpos)
{
    vsf_peda_begin();
    vk_linfs_file_t *file = (vk_linfs_file_t *)&vsf_this;
    uint_fast64_t offset = vsf_local.offset;
    if (file->attr & VSF_FILE_ATTR_DIRECTORY) {
        if (file->d.dir != NULL) {
            closedir((DIR *)file->d.dir);
        }
        if (0 == offset) {
            file->d.dir = NULL;
        } else {
            file->d.dir = opendir(file->fullpath);
            VSF_FS_ASSERT(file->d.dir != NULL);

            struct dirent *entry;
            while (--offset > 0) {
                if (NULL == (entry = readdir((DIR *)file->d.dir))) {
                    closedir((DIR *)file->d.dir);
                    file->d.dir = NULL;
                    goto assert_fail;
                }
            }
        }
    } else {
        if (lseek(file->f.fd, vsf_local.offset, SEEK_SET) < 0) {
        assert_fail:
            VSF_FS_ASSERT(false);
            vsf_eda_return(VSF_ERR_FAIL);
            return;
        }
    }
    VSF_FS_ASSERT(vsf_local.result != NULL);
    *vsf_local.result = offset;
    vsf_eda_return(VSF_ERR_NONE);
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_linfs_read, vk_file_read)
{
    vsf_peda_begin();
    vk_linfs_file_t *file = (vk_linfs_file_t *)&vsf_this;
    vsf_eda_return(read(file->f.fd, vsf_local.buff, vsf_local.size));
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_linfs_write, vk_file_write)
{
    vsf_peda_begin();
    vk_linfs_file_t *file = (vk_linfs_file_t *)&vsf_this;
    ssize_t wsize = write(file->f.fd, vsf_local.buff, vsf_local.size);
    if (file->pos + wsize > file->size) {
        file->size = file->pos + wsize;
    }
    vsf_eda_return(wsize);
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_linfs_close, vk_file_close)
{
    vsf_peda_begin();
    vk_linfs_file_t *file = (vk_linfs_file_t *)&vsf_this;

    VSF_FS_ASSERT(file->name != NULL);
    vsf_heap_free(file->name);
    VSF_FS_ASSERT(file->fullpath != NULL);
    vsf_heap_free(file->fullpath);

    if (file->attr & VSF_FILE_ATTR_DIRECTORY) {
        if (file->d.dir != NULL) {
            closedir((DIR *)file->d.dir);
            file->d.dir = NULL;
        }
    } else {
        close(file->f.fd);
    }

    vsf_eda_return(VSF_ERR_NONE);
    vsf_peda_end();
}

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic pop
#endif

#endif
