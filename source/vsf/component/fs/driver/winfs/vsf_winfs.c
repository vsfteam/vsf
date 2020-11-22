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

#if VSF_USE_FS == ENABLED && VSF_FS_USE_WINFS == ENABLED

#define __VSF_FS_CLASS_INHERIT__
#define __VSF_WINFS_CLASS_IMPLEMENT

#include "../../vsf_fs.h"
#include <windows.h>

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

dcl_vsf_peda_methods(static, __vk_winfs_mount)
dcl_vsf_peda_methods(static, __vk_winfs_lookup)
dcl_vsf_peda_methods(static, __vk_winfs_read)
dcl_vsf_peda_methods(static, __vk_winfs_write)
dcl_vsf_peda_methods(static, __vk_winfs_close)

extern vk_file_t * __vk_file_get_fs_parent(vk_file_t *file);

/*============================ GLOBAL VARIABLES ==============================*/

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-function-type"
#endif

const vk_fs_op_t vk_winfs_op = {
    .fn_mount       = (vsf_peda_evthandler_t)vsf_peda_func(__vk_winfs_mount),
    .fn_unmount     = (vsf_peda_evthandler_t)vsf_peda_func(vk_dummyfs_succeed),
#if VSF_FS_CFG_USE_CACHE == ENABLED
    .fn_sync        = vk_file_dummy,
#endif
    .fop            = {
        .fn_read    = (vsf_peda_evthandler_t)vsf_peda_func(__vk_winfs_read),
        .fn_write   = (vsf_peda_evthandler_t)vsf_peda_func(__vk_winfs_write),
        .fn_close   = (vsf_peda_evthandler_t)vsf_peda_func(__vk_winfs_close),
        .fn_resize  = (vsf_peda_evthandler_t)vsf_peda_func(vk_dummyfs_not_support),
    },
    .dop            = {
        .fn_lookup  = (vsf_peda_evthandler_t)vsf_peda_func(__vk_winfs_lookup),
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

static uint_fast16_t __vk_winfs_file_get_path(vk_file_t *file, char *path, uint_fast16_t len)
{
    vk_file_t *tmp = file;
    uint_fast16_t real_len = 0, cur_len;

    while (tmp != NULL) {
        if (&vk_vfs_op == tmp->fsop) {
            tmp = ((vk_vfs_file_t *)tmp)->subfs.root;
        }
        real_len += strlen(tmp->name) + 1;
        tmp = tmp->parent;
    }

    if (real_len > len) {
        return 0;
    }

    tmp = file;
    len = real_len - 1;
    path[len] = '\0';
    while (tmp != NULL) {
        if (&vk_vfs_op == tmp->fsop) {
            tmp = ((vk_vfs_file_t *)tmp)->subfs.root;
        }
        cur_len = strlen(tmp->name);
        len -= cur_len;
        memcpy(&path[len], tmp->name, cur_len);
        tmp = tmp->parent;
        if (tmp != NULL) {
            path[len-- - 1] = '/';
        }
    }
    return real_len;
}

static bool __vk_winfs_set_pos(vk_winfs_file_t *file, uint_fast64_t pos)
{
    LARGE_INTEGER li;
    li.QuadPart = pos;
    li.LowPart = SetFilePointer(file->f.hFile, li.LowPart, &li.HighPart, FILE_BEGIN);
    if ((INVALID_SET_FILE_POINTER == li.LowPart) && (GetLastError() != NO_ERROR)) {
        return VSF_ERR_FAIL;
    }
    return VSF_ERR_NONE;
}

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#elif   __IS_COMPILER_LLVM__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#endif

__vsf_component_peda_ifs_entry(__vk_winfs_mount, vk_fs_mount)
{
    vsf_peda_begin();
    vk_vfs_file_t *dir = (vk_vfs_file_t *)&vsf_this;
    vk_winfs_info_t *fsinfo = dir->subfs.data;
    VSF_FS_ASSERT((fsinfo != NULL) && (fsinfo->root.name != NULL));

    WIN32_FIND_DATAA FindFileData;
    HANDLE hFind = FindFirstFileA(fsinfo->root.name, &FindFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        vsf_eda_return(VSF_ERR_NOT_AVAILABLE);
        return;
    }
    if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
        vsf_eda_return(VSF_ERR_INVALID_PARAMETER);
        return;
    }
    FindClose(hFind);

    dir->subfs.root = &fsinfo->root.use_as__vk_file_t;
    vsf_eda_return(VSF_ERR_NONE);
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_winfs_lookup, vk_file_lookup)
{
    vsf_peda_begin();
    vk_winfs_file_t *dir = (vk_winfs_file_t *)&vsf_this;
    const char *name = vsf_local.name;
    uint_fast32_t idx = vsf_local.idx;
    vsf_err_t err = VSF_ERR_NONE;

    vsf_protect_t orig = vsf_protect_sched();
        __vsf_dlist_foreach_unsafe(vk_winfs_file_t, child_node, &dir->d.child_list) {
            if (    (name && vk_file_is_match((char *)name, _->name))
                ||  (!name && (_->idx == idx))) {
                vsf_unprotect_sched(orig);
                *vsf_local.result = &_->use_as__vk_file_t;
                goto do_return;
            }
        }
    vsf_unprotect_sched(orig);

    char path[MAX_PATH];
    uint_fast16_t len = __vk_winfs_file_get_path(&dir->use_as__vk_file_t, path, sizeof(path));
    uint_fast16_t namelen;

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
        if ((len + namelen + 1) > MAX_PATH) {
            err = VSF_ERR_FAIL;
            goto do_return;
        }
        path[len - 1] = '/';
        memcpy(&path[len], name, namelen);
        path[len + namelen] = '\0';
    } else {
        WIN32_FIND_DATAA FindFileData;
        HANDLE hFind;

        if ((len + 2) > MAX_PATH) {
            err = VSF_ERR_FAIL;
            goto do_return;
        }
        strcat(path, "/*");

        hFind = FindFirstFileA(path, &FindFileData);
        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                if (    !strcmp(FindFileData.cFileName, ".")
                    ||  !strcmp(FindFileData.cFileName, "..")) {
                    idx++;
                }
                if (!idx) {
                    break;
                }

                if (!FindNextFileA(hFind, &FindFileData)) {
                    FindClose(hFind);
                    goto do_not_available;
                }
            } while (idx--);
            FindClose(hFind);

            namelen = strlen(FindFileData.cFileName);
            if ((len + namelen + 1) > MAX_PATH) {
                err = VSF_ERR_FAIL;
                goto do_return;
            }
            path[len] = '\0';
            strcat(path, FindFileData.cFileName);
        } else {
            goto do_not_available;
        }
    }

    DWORD dwAttribute = GetFileAttributesA(path);
    if (INVALID_FILE_ATTRIBUTES == dwAttribute) {
    do_not_available:
        err = VSF_ERR_NOT_AVAILABLE;
        goto do_return;
    }

    vk_winfs_file_t *winfs_file = (vk_winfs_file_t *)vk_file_alloc(sizeof(*winfs_file));
    if (NULL == winfs_file) {
        err = VSF_ERR_NOT_ENOUGH_RESOURCES;
        goto do_return;
    }
    winfs_file->name = vsf_heap_malloc(namelen + 1);
    if (NULL == winfs_file->name) {
        err = VSF_ERR_NOT_ENOUGH_RESOURCES;
        goto do_free_and_return;
    }
    strcpy(winfs_file->name, vk_file_getfilename(path));
    winfs_file->fsop = &vk_winfs_op;

    if (dwAttribute & FILE_ATTRIBUTE_DIRECTORY) {
        // TODO: Open Directory
    } else {
        winfs_file->f.hFile = CreateFileA(path, GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL, NULL);
        if (INVALID_HANDLE_VALUE == winfs_file->f.hFile) {
            err = VSF_ERR_NOT_AVAILABLE;
            goto do_free_and_return;
        }

        DWORD sizehigh;
        winfs_file->size = GetFileSize(winfs_file->f.hFile, &sizehigh) | ((uint64_t)sizehigh << 32);
    }

    if (dwAttribute & FILE_ATTRIBUTE_ARCHIVE) {
        winfs_file->attr |= VSF_WINFS_FILE_ATTR_ARCHIVE;
    }
    if (dwAttribute & FILE_ATTRIBUTE_DIRECTORY) {
        winfs_file->attr |= VSF_FILE_ATTR_DIRECTORY;
    }
    if (dwAttribute & FILE_ATTRIBUTE_HIDDEN) {
        winfs_file->attr |= VSF_FILE_ATTR_HIDDEN;
    }
    if (dwAttribute & FILE_ATTRIBUTE_SYSTEM) {
    }
    winfs_file->attr |= VSF_FILE_ATTR_READ | VSF_FILE_ATTR_WRITE;
    if (dwAttribute & FILE_ATTRIBUTE_READONLY) {
        winfs_file->attr &= ~VSF_FILE_ATTR_WRITE;
    }
    orig = vsf_protect_sched();
        vsf_dlist_add_to_head(vk_winfs_file_t, child_node, &dir->d.child_list, winfs_file);
    vsf_unprotect_sched(orig);
    *vsf_local.result = &winfs_file->use_as__vk_file_t;
    goto do_return;

do_free_and_return:
    if (winfs_file->name != NULL) {
        vsf_heap_free(winfs_file->name);
    }
    vk_file_free(&winfs_file->use_as__vk_file_t);
do_return:
    vsf_eda_return(err);
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_winfs_read, vk_file_read)
{
    vsf_peda_begin();
    vk_winfs_file_t *file = (vk_winfs_file_t *)&vsf_this;
    uint_fast64_t offset = vsf_local.offset;
    uint_fast32_t size = vsf_local.size;
    uint8_t *buff = vsf_local.buff;
    DWORD rsize = 0;

    if (    (VSF_ERR_NONE != __vk_winfs_set_pos(file, offset))
        ||  ReadFile(file->f.hFile, buff, size, &rsize, NULL)) {
        vsf_eda_return(rsize);
    } else {
        vsf_eda_return(VSF_ERR_FAIL);
    }
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_winfs_write, vk_file_write)
{
    vsf_peda_begin();
    vk_winfs_file_t *file = (vk_winfs_file_t *)&vsf_this;
    uint_fast64_t offset = vsf_local.offset;
    uint_fast32_t size = vsf_local.size;
    uint8_t *buff = vsf_local.buff;
    DWORD wsize = 0;

    if (    (VSF_ERR_NONE != __vk_winfs_set_pos(file, offset))
        ||  WriteFile(file->f.hFile, buff, size, &wsize, NULL)) {
        vsf_eda_return(wsize);
    } else {
        vsf_eda_return(VSF_ERR_FAIL);
    }
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_winfs_close, vk_file_close)
{
    vsf_peda_begin();
    vk_winfs_file_t *file = (vk_winfs_file_t *)&vsf_this;
    vk_winfs_file_t *parent = (vk_winfs_file_t *)__vk_file_get_fs_parent(&file->use_as__vk_file_t);

    VSF_FS_ASSERT(file->name != NULL);
    vsf_heap_free(file->name);

    if (file->attr & VSF_FILE_ATTR_DIRECTORY) {
        // TODO: Close Directory
    } else {
        CloseHandle(file->f.hFile);
    }
    vsf_protect_t orig = vsf_protect_sched();
        vsf_dlist_remove(vk_winfs_file_t, child_node, &parent->d.child_list, file);
    vsf_unprotect_sched(orig);
    vsf_eda_return(VSF_ERR_NONE);
    vsf_peda_end();
}

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#elif   __IS_COMPILER_LLVM__
#   pragma clang diagnostic pop
#endif

#endif
