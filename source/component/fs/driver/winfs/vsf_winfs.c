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
dcl_vsf_peda_methods(static, __vk_winfs_create)
dcl_vsf_peda_methods(static, __vk_winfs_unlink)
dcl_vsf_peda_methods(static, __vk_winfs_rename)
dcl_vsf_peda_methods(static, __vk_winfs_setpos)
dcl_vsf_peda_methods(static, __vk_winfs_setsize)

/*============================ GLOBAL VARIABLES ==============================*/

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-function-type"
#endif

const vk_fs_op_t vk_winfs_op = {
    .fn_mount       = (vsf_peda_evthandler_t)vsf_peda_func(__vk_winfs_mount),
    .fn_unmount     = (vsf_peda_evthandler_t)vsf_peda_func(vk_fsop_succeed),
#if VSF_FS_CFG_USE_CACHE == ENABLED
    .fn_sync        = (vsf_peda_evthandler_t)vsf_peda_func(vk_fsop_succeed),
#endif
    .fn_rename      = (vsf_peda_evthandler_t)vsf_peda_func(__vk_winfs_rename),
    .fop            = {
        .fn_read    = (vsf_peda_evthandler_t)vsf_peda_func(__vk_winfs_read),
        .fn_write   = (vsf_peda_evthandler_t)vsf_peda_func(__vk_winfs_write),
        .fn_close   = (vsf_peda_evthandler_t)vsf_peda_func(__vk_winfs_close),
        .fn_setsize = (vsf_peda_evthandler_t)vsf_peda_func(__vk_winfs_setsize),
        .fn_setpos  = (vsf_peda_evthandler_t)vsf_peda_func(__vk_winfs_setpos),
    },
    .dop            = {
        .fn_lookup  = (vsf_peda_evthandler_t)vsf_peda_func(__vk_winfs_lookup),
        .fn_create  = (vsf_peda_evthandler_t)vsf_peda_func(__vk_winfs_create),
        .fn_unlink  = (vsf_peda_evthandler_t)vsf_peda_func(__vk_winfs_unlink),
        .fn_chmod   = (vsf_peda_evthandler_t)vsf_peda_func(vk_fsop_not_support),
    },
};

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

static vsf_err_t __vk_winfs_set_pos(vk_winfs_file_t *file, uint_fast64_t pos)
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
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#endif

__vsf_component_peda_ifs_entry(__vk_winfs_mount, vk_fs_mount)
{
    vsf_peda_begin();
    vk_vfs_file_t *dir = (vk_vfs_file_t *)&vsf_this;
    vk_winfs_info_t *fsinfo = dir->subfs.data;
    VSF_FS_ASSERT((fsinfo != NULL) && (fsinfo->root.name != NULL));

    size_t namelen = strlen(fsinfo->root.name);
    char find_str[namelen + 2 + 1];
    strcpy(find_str, fsinfo->root.name);
    if ('\\' == find_str[namelen - 1]) {
        find_str[namelen + 0] = '*';
        find_str[namelen + 1] = '\0';
    } else {
        find_str[namelen + 0] = '\\';
        find_str[namelen + 1] = '*';
        find_str[namelen + 2] = '\0';
    }

    int path_unicode_len = MultiByteToWideChar(CP_UTF8, 0, find_str, -1, NULL, 0);
    wchar_t path_unicode[path_unicode_len];
    MultiByteToWideChar(CP_UTF8, 0, find_str, -1, path_unicode, path_unicode_len);

    WIN32_FIND_DATAW FindFileData;
    HANDLE hFind = FindFirstFileW(path_unicode, &FindFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        vsf_eda_return(VSF_ERR_NOT_AVAILABLE);
        return;
    }
    if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
        vsf_eda_return(VSF_ERR_INVALID_PARAMETER);
        return;
    }
    FindClose(hFind);

    fsinfo->root.fullpath = fsinfo->root.name;
    fsinfo->root.d.hFind = INVALID_HANDLE_VALUE;
    dir->subfs.root = &fsinfo->root.use_as__vk_file_t;
    vsf_eda_return(VSF_ERR_NONE);
    vsf_peda_end();
}

static void __vk_winfs_prepare_find(vk_winfs_file_t *dir, WIN32_FIND_DATAW *FindFileData)
{
    uint_fast16_t pathlen = strlen(dir->fullpath);
    char path[pathlen + 3 /* "\\*\n" */];
    strcpy(path, dir->fullpath);
    strcat(path, "\\*");

    int path_unicode_len = MultiByteToWideChar(CP_UTF8, 0, path, -1, NULL, 0);
    wchar_t path_unicode[path_unicode_len];
    MultiByteToWideChar(CP_UTF8, 0, path, -1, path_unicode, path_unicode_len);

    dir->d.hFind = FindFirstFileW(path_unicode, FindFileData);
}

__vsf_component_peda_ifs_entry(__vk_winfs_lookup, vk_file_lookup)
{
    vsf_peda_begin();
    vk_winfs_file_t *dir = (vk_winfs_file_t *)&vsf_this;
    const char *name = vsf_local.name;
    uint_fast32_t idx = dir->pos;
    vsf_err_t err = VSF_ERR_NONE;

    uint_fast16_t namelen;
    WIN32_FIND_DATAW FindFileData;

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
    } else {
        if (INVALID_HANDLE_VALUE == dir->d.hFind) {
            __vk_winfs_prepare_find(dir, &FindFileData);
            if (INVALID_HANDLE_VALUE == dir->d.hFind) {
                goto return_not_available;
            }
        } else if (!FindNextFileW(dir->d.hFind, &FindFileData)) {
        return_not_available:
            if (dir->d.hFind != INVALID_HANDLE_VALUE) {
                FindClose(dir->d.hFind);
                dir->d.hFind = INVALID_HANDLE_VALUE;
            }
            dir->pos = 0;

            err = VSF_ERR_NOT_AVAILABLE;
            vsf_eda_return(err);
            return;
        }

        idx = 0;
        do {
            if (    !wcscmp(FindFileData.cFileName, L".")
                ||  !wcscmp(FindFileData.cFileName, L"..")) {
                idx++;
            }
            if (!idx) {
                break;
            }

            if (!FindNextFileW(dir->d.hFind, &FindFileData)) {
                goto return_not_available;
            }
        } while (idx--);
        namelen = WideCharToMultiByte(CP_UTF8, 0, FindFileData.cFileName, -1, NULL, 0, 0, false) - 1;
    }

    uint_fast16_t pathlen = strlen(dir->fullpath);
    char path[pathlen + 1 /* '\\' */ + namelen + 1 /* '\0' */];
    strcpy(path, dir->fullpath);
    path[pathlen] = '\\';
    if (NULL == name) {
        VSF_FS_ASSERT(dir->d.hFind != INVALID_HANDLE_VALUE);
        name = &path[pathlen + 1];
        WideCharToMultiByte(CP_UTF8, 0, FindFileData.cFileName, -1, (char *)name, namelen, 0, false);
    } else {
        memcpy(&path[pathlen + 1], name, namelen);
    }
    path[pathlen + namelen + 1] = '\0';

    int path_unicode_len = MultiByteToWideChar(CP_UTF8, 0, path, -1, NULL, 0);
    wchar_t path_unicode[path_unicode_len];
    MultiByteToWideChar(CP_UTF8, 0, path, -1, path_unicode, path_unicode_len);
    DWORD dwAttribute = GetFileAttributesW(path_unicode);
    if (INVALID_FILE_ATTRIBUTES == dwAttribute) {
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
    memcpy(winfs_file->name, name, namelen);
    winfs_file->name[namelen] = '\0';
    winfs_file->fullpath = vsf_heap_malloc(strlen(dir->fullpath) + 1 /* '\\' */ + namelen + 1 /* '\0' */);
    if (NULL == winfs_file->fullpath) {
        err = VSF_ERR_NOT_ENOUGH_RESOURCES;
        goto do_free_and_return;
    }
    strcpy(winfs_file->fullpath, dir->fullpath);
    strcat(winfs_file->fullpath, "\\");
    strcat(winfs_file->fullpath, winfs_file->name);
    winfs_file->fsop = &vk_winfs_op;

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
    if (dwAttribute & FILE_ATTRIBUTE_DIRECTORY) {
        winfs_file->d.hFind = INVALID_HANDLE_VALUE;
    } else {
        DWORD access_mode = GENERIC_READ, share_mode = FILE_SHARE_READ;
        if (winfs_file->attr & VSF_FILE_ATTR_WRITE) {
            access_mode |= GENERIC_WRITE;
            share_mode |= FILE_SHARE_WRITE;
        }
        winfs_file->f.hFile = CreateFileW(path_unicode, access_mode, share_mode, NULL, OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL, NULL);
        if (INVALID_HANDLE_VALUE == winfs_file->f.hFile) {
            err = VSF_ERR_NOT_AVAILABLE;
            goto do_free_and_return;
        }

        DWORD sizehigh;
        winfs_file->size = GetFileSize(winfs_file->f.hFile, &sizehigh) | ((uint64_t)sizehigh << 32);
    }

    *vsf_local.result = &winfs_file->use_as__vk_file_t;
    goto do_return;

do_free_and_return:
    if (winfs_file->name != NULL) {
        vsf_heap_free(winfs_file->name);
    }
    if (winfs_file->fullpath != NULL) {
        vsf_heap_free(winfs_file->fullpath);
    }
    vk_file_free(&winfs_file->use_as__vk_file_t);
do_return:
    vsf_eda_return(err);
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_winfs_setpos, vk_file_setpos)
{
    vsf_peda_begin();
    vk_winfs_file_t *file = (vk_winfs_file_t *)&vsf_this;
    uint_fast64_t offset = vsf_local.offset;
    if (file->attr & VSF_FILE_ATTR_DIRECTORY) {
        if (file->d.hFind != INVALID_HANDLE_VALUE) {
            FindClose(file->d.hFind);
        }
        if (0 == offset) {
            file->d.hFind = INVALID_HANDLE_VALUE;
        } else {
            WIN32_FIND_DATAW FindFileData;
            __vk_winfs_prepare_find(file, &FindFileData);
            VSF_FS_ASSERT(file->d.hFind != INVALID_HANDLE_VALUE);

            while (--offset > 0) {
                if (!FindNextFileW(file->d.hFind, &FindFileData)) {
                    FindClose(file->d.hFind);
                    file->d.hFind = INVALID_HANDLE_VALUE;
                    goto assert_fail;
                }
            }
        }
    } else {
        if (VSF_ERR_NONE != __vk_winfs_set_pos(file, offset)) {
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

__vsf_component_peda_ifs_entry(__vk_winfs_setsize, vk_file_setsize)
{
    vsf_peda_begin();
    vk_winfs_file_t *file = (vk_winfs_file_t *)&vsf_this;
    FILE_END_OF_FILE_INFO finfo;
    VSF_FS_ASSERT(!(file->attr & VSF_FILE_ATTR_DIRECTORY));

    finfo.EndOfFile.QuadPart = (long long )vsf_local.size;
    if (!SetFileInformationByHandle(file->f.hFile, FileEndOfFileInfo, &finfo, sizeof(finfo))) {
        VSF_FS_ASSERT(false);
        vsf_eda_return(VSF_ERR_FAIL);
        return;
    }
    vsf_eda_return(VSF_ERR_NONE);
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_winfs_read, vk_file_read)
{
    vsf_peda_begin();
    vk_winfs_file_t *file = (vk_winfs_file_t *)&vsf_this;
    uint_fast32_t size = vsf_local.size;
    uint8_t *buff = vsf_local.buff;
    DWORD rsize = 0;

    if (ReadFile(file->f.hFile, buff, size, &rsize, NULL)) {
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
    uint_fast32_t size = vsf_local.size;
    uint8_t *buff = vsf_local.buff;
    DWORD wsize = 0;

    if (WriteFile(file->f.hFile, buff, size, &wsize, NULL)) {
        if (file->pos + wsize > file->size) {
            file->size = file->pos + wsize;
        }
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

    VSF_FS_ASSERT(file->name != NULL);
    vsf_heap_free(file->name);
    VSF_FS_ASSERT(file->fullpath != NULL);
    vsf_heap_free(file->fullpath);

    if (file->attr & VSF_FILE_ATTR_DIRECTORY) {
        if (file->d.hFind != INVALID_HANDLE_VALUE) {
            FindClose(file->d.hFind);
            file->d.hFind = INVALID_HANDLE_VALUE;
        }
    } else {
        CloseHandle(file->f.hFile);
    }

    vsf_eda_return(VSF_ERR_NONE);
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_winfs_create, vk_file_create)
{
    vsf_peda_begin();
    vk_winfs_file_t *dir = (vk_winfs_file_t *)&vsf_this;
    uint_fast16_t pathlen = strlen(dir->fullpath);
    int namelen = strlen(vsf_local.name);
    vsf_err_t err;

    char path[pathlen + 1 /* possible '\\' */ + namelen + 1 /* '\0' */];
    strcpy(path, dir->fullpath);
    if ((path[pathlen - 1] != '\\') && (path[pathlen - 1] != '/')) {
        path[pathlen] = '\\';
        path[pathlen + 1] = '\0';
    }
    strcat(path, vsf_local.name);
    int path_unicode_len = MultiByteToWideChar(CP_UTF8, 0, path, -1, NULL, 0);
    wchar_t path_unicode[path_unicode_len];
    MultiByteToWideChar(CP_UTF8, 0, path, -1, path_unicode, path_unicode_len);

    if (vsf_local.attr & VSF_FILE_ATTR_DIRECTORY) {
        err = CreateDirectoryW(path_unicode, NULL) ? VSF_ERR_NONE : VSF_ERR_FAIL;
    } else {
        HANDLE hFile = CreateFileW(path_unicode, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
        err = (INVALID_HANDLE_VALUE == hFile) ? VSF_ERR_FAIL : VSF_ERR_NONE;
        CloseHandle(hFile);
    }
    vsf_eda_return(err);

    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_winfs_unlink, vk_file_unlink)
{
    vsf_peda_begin();
    vk_winfs_file_t *dir = (vk_winfs_file_t *)&vsf_this;
    uint_fast16_t pathlen = strlen(dir->fullpath);
    int namelen = strlen(vsf_local.name);
    vsf_err_t err;

    char path[pathlen + 1 /* possible '\\' */ + namelen + 1 /* '\0' */];
    strcpy(path, dir->fullpath);
    if ((path[pathlen - 1] != '\\') && (path[pathlen - 1] != '/')) {
        path[pathlen] = '\\';
        path[pathlen + 1] = '\0';
    }
    strcat(path, vsf_local.name);
    int path_unicode_len = MultiByteToWideChar(CP_UTF8, 0, path, -1, NULL, 0);
    wchar_t path_unicode[path_unicode_len];
    MultiByteToWideChar(CP_UTF8, 0, path, -1, path_unicode, path_unicode_len);

    DWORD dwAttribute = GetFileAttributesW(path_unicode);
    if (INVALID_FILE_ATTRIBUTES == dwAttribute) {
    do_not_available:
        err = VSF_ERR_NOT_AVAILABLE;
        goto do_return;
    }

    if (dwAttribute & FILE_ATTRIBUTE_DIRECTORY) {
        err = RemoveDirectoryW(path_unicode) ? VSF_ERR_NONE : VSF_ERR_FAIL;
    } else {
        err = DeleteFileW(path_unicode) ? VSF_ERR_NONE : VSF_ERR_FAIL;
    }
do_return:
    vsf_eda_return(err);
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_winfs_rename, vk_file_rename)
{
    vsf_peda_begin();
    char *newname;

    if (vsf_local.newname != NULL) {
        newname = vsf_heap_malloc(strlen(vsf_local.newname) + 1);
        if (NULL == newname) {
            vsf_eda_return(VSF_ERR_NOT_ENOUGH_RESOURCES);
            return;
        }
        strcpy(newname, vsf_local.newname);
    } else {
        newname = (char *)vsf_local.oldname;
    }

    uint_fast16_t pathlen_old = strlen(((vk_winfs_file_t *)vsf_local.olddir)->fullpath);
    uint_fast16_t pathlen_new = strlen(((vk_winfs_file_t *)vsf_local.newdir)->fullpath);
    uint_fast16_t namelen_old = strlen(vsf_local.oldname);
    uint_fast16_t namelen_new = strlen(vsf_local.newname);

    char path_old[pathlen_old + 1 /* '\\' */ + namelen_old + 1 /* '\0' */];
    strcpy(path_old, ((vk_winfs_file_t *)vsf_local.olddir)->fullpath);
    strcat(path_old, "\\");
    strcat(path_old, vsf_local.oldname);
    int path_old_unicode_len = MultiByteToWideChar(CP_UTF8, 0, path_old, -1, NULL, 0);
    wchar_t path_old_unicode[path_old_unicode_len];
    MultiByteToWideChar(CP_UTF8, 0, path_old, -1, path_old_unicode, path_old_unicode_len);

    char path_new[pathlen_new + 1 /* '\\' */ + namelen_new + 1 /* '\0' */];
    strcpy(path_new, ((vk_winfs_file_t *)vsf_local.newdir)->fullpath);
    strcat(path_new, "\\");
    strcat(path_new, vsf_local.newname);
    int path_new_unicode_len = MultiByteToWideChar(CP_UTF8, 0, path_new, -1, NULL, 0);
    wchar_t path_new_unicode[path_new_unicode_len];
    MultiByteToWideChar(CP_UTF8, 0, path_new, -1, path_new_unicode, path_new_unicode_len);

    if (!MoveFileW(path_old_unicode, path_new_unicode)) {
        vsf_trace_error("winfs: fail to rename %s to %s %d\r\n", path_old, path_new, GetLastError());
        if (vsf_local.newname != NULL) {
            vsf_heap_free(newname);
        }
        vsf_eda_return(VSF_ERR_FAIL);
        return;
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
