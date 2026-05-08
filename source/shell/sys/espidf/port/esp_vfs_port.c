/*****************************************************************************
 *   Copyright(C)2009-2026 by VSF Team                                       *
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

#include "../vsf_espidf_cfg.h"

#if VSF_USE_ESPIDF == ENABLED && VSF_ESPIDF_CFG_USE_VFS == ENABLED

#define __VSF_FS_CLASS_IMPLEMENT
#define __VSF_LINUX_FS_CLASS_INHERIT__

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

struct _reent;
struct utimbuf;

#include "component/fs/vsf_fs.h"
#include "shell/sys/linux/kernel/fs/vsf_linux_fs.h"

#include "esp_vfs.h"
#include "esp_vfs_ops.h"

#if !defined(VSF_USE_LINUX) || VSF_USE_LINUX != ENABLED
#   error "esp_vfs port requires VSF_USE_LINUX == ENABLED"
#endif

struct _reent;

#define VFS_MAX_REGISTERED  VSF_ESPIDF_CFG_VFS_MAX_REGISTERED
#define VFS_MAX_FDS         16
#define VFS_SUBPATH_MAX     128

typedef enum {
    BRIDGE_OP_LOOKUP,
    BRIDGE_OP_READ,
    BRIDGE_OP_WRITE,
    BRIDGE_OP_CLOSE,
    BRIDGE_OP_SETPOS,
} bridge_op_t;

typedef struct {
    const esp_vfs_fs_ops_t *ops;
    void                  *ctx;
    int                    local_fd;
    char                   subpath[VFS_SUBPATH_MAX];
} bridge_file_t;

typedef struct {
    vk_file_t              root;
    const esp_vfs_fs_ops_t *ops;
    void                  *ctx;
    int                    flags;
    bool                   mounted;
    pthread_t              thread;
    vsf_sem_t              sem;
    void                  *caller;
    bridge_op_t            op;
    union {
        struct { const char *name; vk_file_t *result; } lookup;
        struct { bridge_file_t *file; uint8_t *buff; uint32_t size; ssize_t rsize; } rw;
        struct { bridge_file_t *file; } close;
        struct { bridge_file_t *file; uint64_t offset; uint64_t *poff; off_t roff; } setpos;
    } param;
    struct { vsf_err_t err; } result;
    const vk_fs_op_t      *fs_op;
} bridge_fsinfo_t;

typedef struct {
    bool                   in_use;
    char                   base_path[ESP_VFS_PATH_MAX + 1];
    const esp_vfs_fs_ops_t *ops;
    void                  *ctx;
    int                    flags;
    bool                   own_ops;
    bridge_fsinfo_t        fsinfo;
} vfs_entry_t;

static vfs_entry_t s_vfs_entries[VFS_MAX_REGISTERED];

/* worker thread */
static void *__bridge_thread(void *arg)
{
    bridge_fsinfo_t *fsinfo = (bridge_fsinfo_t *)arg;

    while (true) {
        vsf_thread_sem_pend(&fsinfo->sem, -1);

        switch (fsinfo->op) {
        case BRIDGE_OP_LOOKUP: {
            const char *name = fsinfo->param.lookup.name;

            int local_fd = -1;
            bool is_dir = false;
            bool exists = true;
            char fullpath[VFS_SUBPATH_MAX];
            fullpath[0] = '/';
            strncpy(fullpath + 1, name, sizeof(fullpath) - 2);
            fullpath[sizeof(fullpath) - 1] = '\0';

            if (fsinfo->ops->open_p) {
                local_fd = fsinfo->ops->open_p(fsinfo->ctx, fullpath, O_RDWR, 0);
            }
            if (local_fd >= 0) {
                is_dir = false;
            } else if (errno == EISDIR || errno == ENOTDIR) {
                is_dir = true;
            } else {
                exists = false;
            }

            if (!exists) {
                fsinfo->result.err = VSF_ERR_NOT_AVAILABLE;
                break;
            }

#if VSF_USE_HEAP == ENABLED
            bridge_file_t *bf = vsf_heap_malloc(sizeof(bridge_file_t));
            if (bf == NULL) { fsinfo->result.err = VSF_ERR_NOT_ENOUGH_RESOURCES; break; }
            bf->ops = fsinfo->ops;
            bf->ctx = fsinfo->ctx;
            bf->local_fd = is_dir ? -1 : local_fd;
            strncpy(bf->subpath, name, sizeof(bf->subpath) - 1);

            vk_vfs_file_t *vfs_file = (vk_vfs_file_t *)vk_file_alloc(
                sizeof(vk_vfs_file_t) + (name ? strlen(name) : 0) + 1);
            if (vfs_file == NULL) { vsf_heap_free(bf); fsinfo->result.err = VSF_ERR_NOT_ENOUGH_RESOURCES; break; }
            memset(vfs_file, 0, sizeof(vk_vfs_file_t));

            const char *base = name;
            while (*base == '/') base++;
            vfs_file->use_as__vk_file_t.name = (char *)&vfs_file[1];
            strcpy(vfs_file->use_as__vk_file_t.name, base);

            if (is_dir) {
                vfs_file->use_as__vk_file_t.attr = VSF_FILE_ATTR_DIRECTORY | VSF_FILE_ATTR_READ | VSF_FILE_ATTR_WRITE | VSF_VFS_FILE_ATTR_MOUNTED;
                vfs_file->subfs.op = fsinfo->fs_op;
                vfs_file->subfs.data = fsinfo;
                vfs_file->subfs.root = &fsinfo->root;
            } else {
                vfs_file->use_as__vk_file_t.attr = VSF_FILE_ATTR_READ | VSF_FILE_ATTR_WRITE;
            }
            vfs_file->use_as__vk_file_t.fsop = fsinfo->fs_op;
            vfs_file->use_as__vk_file_t.fsinfo = (vk_fs_info_t *)fsinfo;
            vfs_file->f.param = bf;
            fsinfo->param.lookup.result = &vfs_file->use_as__vk_file_t;
            fsinfo->result.err = VSF_ERR_NONE;
#else
            fsinfo->result.err = VSF_ERR_NOT_ENOUGH_RESOURCES;
#endif
            break;
        }
        case BRIDGE_OP_READ: {
            bridge_file_t *bf = fsinfo->param.rw.file;
            if (bf->ops->read_p)
                fsinfo->param.rw.rsize = bf->ops->read_p(bf->ctx, bf->local_fd, fsinfo->param.rw.buff, fsinfo->param.rw.size);
            else
                fsinfo->param.rw.rsize = -1;
            fsinfo->result.err = fsinfo->param.rw.rsize >= 0 ? VSF_ERR_NONE : VSF_ERR_FAIL;
            break;
        }
        case BRIDGE_OP_WRITE: {
            bridge_file_t *bf = fsinfo->param.rw.file;
            if (bf->ops->write_p)
                fsinfo->param.rw.rsize = bf->ops->write_p(bf->ctx, bf->local_fd, fsinfo->param.rw.buff, fsinfo->param.rw.size);
            else
                fsinfo->param.rw.rsize = -1;
            fsinfo->result.err = fsinfo->param.rw.rsize >= 0 ? VSF_ERR_NONE : VSF_ERR_FAIL;
            break;
        }
        case BRIDGE_OP_CLOSE: {
            bridge_file_t *bf = fsinfo->param.close.file;
            if (bf->ops->close_p)
                bf->ops->close_p(bf->ctx, bf->local_fd);
#if VSF_USE_HEAP == ENABLED
            vsf_heap_free(bf);
#endif
            fsinfo->result.err = VSF_ERR_NONE;
            break;
        }
        case BRIDGE_OP_SETPOS: {
            bridge_file_t *bf = fsinfo->param.setpos.file;
            off_t result = -1;
            if (bf->ops->lseek_p)
                result = bf->ops->lseek_p(bf->ctx, bf->local_fd, (off_t)fsinfo->param.setpos.offset, SEEK_SET);
            fsinfo->param.setpos.roff = result;
            fsinfo->result.err = result >= 0 ? VSF_ERR_NONE : VSF_ERR_FAIL;
            break;
        }
        }

        VSF_FS_ASSERT(fsinfo->caller != NULL);
        vsf_eda_post_evt(fsinfo->caller, VSF_EVT_RETURN);
    }
}

/* peda declarations */
dcl_vsf_peda_methods(static, __bridge_mount)
dcl_vsf_peda_methods(static, __bridge_lookup)
dcl_vsf_peda_methods(static, __bridge_read)
dcl_vsf_peda_methods(static, __bridge_write)
dcl_vsf_peda_methods(static, __bridge_close)
dcl_vsf_peda_methods(static, __bridge_setpos)
dcl_vsf_peda_methods(static, __bridge_noop)

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-function-type"
#endif

static const vk_fs_op_t __bridge_fs_op = {
    .fn_probe = NULL,
    .mount_local_size = 1,
    .fn_mount = (vsf_peda_evthandler_t)vsf_peda_func(__bridge_mount),
    .fn_unmount = (vsf_peda_evthandler_t)vsf_peda_func(__bridge_noop),
    .fn_rename = (vsf_peda_evthandler_t)vsf_peda_func(__bridge_noop),
    .fop = {
        .fn_read = (vsf_peda_evthandler_t)vsf_peda_func(__bridge_read),
        .fn_write = (vsf_peda_evthandler_t)vsf_peda_func(__bridge_write),
        .fn_close = (vsf_peda_evthandler_t)vsf_peda_func(__bridge_close),
        .fn_setsize = (vsf_peda_evthandler_t)vsf_peda_func(__bridge_noop),
        .fn_setpos = (vsf_peda_evthandler_t)vsf_peda_func(__bridge_setpos),
    },
    .dop = {
        .fn_lookup = (vsf_peda_evthandler_t)vsf_peda_func(__bridge_lookup),
        .fn_create = (vsf_peda_evthandler_t)vsf_peda_func(__bridge_noop),
        .fn_unlink = (vsf_peda_evthandler_t)vsf_peda_func(__bridge_noop),
        .fn_chmod = (vsf_peda_evthandler_t)vsf_peda_func(__bridge_noop),
    },
};

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

/* peda implementations */

__vsf_component_peda_public_entry(__bridge_noop)
{
    vsf_peda_begin();
    vsf_eda_return(VSF_ERR_NONE);
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__bridge_mount, vk_fs_mount)
{
    vsf_peda_begin();
    vk_vfs_file_t *dir = (vk_vfs_file_t *)&vsf_this;
    bridge_fsinfo_t *fsinfo = (bridge_fsinfo_t *)dir->subfs.data;

    dir->subfs.root = &fsinfo->root;
    fsinfo->mounted = true;
    vsf_eda_return(VSF_ERR_NONE);
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__bridge_lookup, vk_file_lookup)
{
    vsf_peda_begin();
    vk_file_t *root = (vk_file_t *)&vsf_this;
    bridge_fsinfo_t *fsinfo = (bridge_fsinfo_t *)root->fsinfo;

    switch (evt) {
    case VSF_EVT_INIT: {
        const char *name = vsf_local.name;
        if ((name == NULL) || (name[0] == '\0')) {
            vsf_eda_return(VSF_ERR_NOT_AVAILABLE);
            break;
        }
        fsinfo->caller = vsf_eda_get_cur();
        fsinfo->op = BRIDGE_OP_LOOKUP;
        fsinfo->param.lookup.name = name;
        vsf_eda_sem_post(&fsinfo->sem);
        break;
    }
    case VSF_EVT_RETURN:
        if (fsinfo->result.err == VSF_ERR_NONE) {
            *vsf_local.result = fsinfo->param.lookup.result;
        }
        vsf_eda_return(fsinfo->result.err);
        break;
    }
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__bridge_read, vk_file_read)
{
    vsf_peda_begin();
    vk_vfs_file_t *file = (vk_vfs_file_t *)&vsf_this;
    bridge_fsinfo_t *fsinfo = (bridge_fsinfo_t *)file->use_as__vk_file_t.fsinfo;
    bridge_file_t *bf = (bridge_file_t *)file->f.param;

    switch (evt) {
    case VSF_EVT_INIT:
        fsinfo->caller = vsf_eda_get_cur();
        fsinfo->op = BRIDGE_OP_READ;
        fsinfo->param.rw.file = bf;
        fsinfo->param.rw.buff = vsf_local.buff;
        fsinfo->param.rw.size = vsf_local.size;
        vsf_eda_sem_post(&fsinfo->sem);
        break;
    case VSF_EVT_RETURN:
        if (fsinfo->result.err == VSF_ERR_NONE && fsinfo->param.rw.rsize >= 0) {
            file->use_as__vk_file_t.pos += (uint64_t)fsinfo->param.rw.rsize;
        }
        vsf_eda_return(fsinfo->param.rw.rsize >= 0
            ? (uintptr_t)fsinfo->param.rw.rsize : VSF_ERR_FAIL);
        break;
    }
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__bridge_write, vk_file_write)
{
    vsf_peda_begin();
    vk_vfs_file_t *file = (vk_vfs_file_t *)&vsf_this;
    bridge_fsinfo_t *fsinfo = (bridge_fsinfo_t *)file->use_as__vk_file_t.fsinfo;
    bridge_file_t *bf = (bridge_file_t *)file->f.param;

    switch (evt) {
    case VSF_EVT_INIT:
        fsinfo->caller = vsf_eda_get_cur();
        fsinfo->op = BRIDGE_OP_WRITE;
        fsinfo->param.rw.file = bf;
        fsinfo->param.rw.buff = vsf_local.buff;
        fsinfo->param.rw.size = vsf_local.size;
        vsf_eda_sem_post(&fsinfo->sem);
        break;
    case VSF_EVT_RETURN:
        if (fsinfo->result.err == VSF_ERR_NONE && fsinfo->param.rw.rsize >= 0) {
            file->use_as__vk_file_t.pos += (uint64_t)fsinfo->param.rw.rsize;
        }
        vsf_eda_return(fsinfo->param.rw.rsize >= 0
            ? (uintptr_t)fsinfo->param.rw.rsize : VSF_ERR_FAIL);
        break;
    }
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__bridge_close, vk_file_close)
{
    vsf_peda_begin();
    vk_vfs_file_t *file = (vk_vfs_file_t *)&vsf_this;
    bridge_fsinfo_t *fsinfo = (bridge_fsinfo_t *)file->use_as__vk_file_t.fsinfo;
    bridge_file_t *bf = (bridge_file_t *)file->f.param;

    switch (evt) {
    case VSF_EVT_INIT:
        if (bf != NULL) {
            fsinfo->caller = vsf_eda_get_cur();
            fsinfo->op = BRIDGE_OP_CLOSE;
            fsinfo->param.close.file = bf;
            vsf_eda_sem_post(&fsinfo->sem);
            break;
        }
        vsf_eda_return(VSF_ERR_NONE);
        break;
    case VSF_EVT_RETURN:
        file->f.param = NULL;
        vsf_eda_return(fsinfo->result.err);
        break;
    }
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__bridge_setpos, vk_file_setpos)
{
    vsf_peda_begin();
    vk_vfs_file_t *file = (vk_vfs_file_t *)&vsf_this;
    bridge_fsinfo_t *fsinfo = (bridge_fsinfo_t *)file->use_as__vk_file_t.fsinfo;
    bridge_file_t *bf = (bridge_file_t *)file->f.param;

    switch (evt) {
    case VSF_EVT_INIT:
        fsinfo->caller = vsf_eda_get_cur();
        fsinfo->op = BRIDGE_OP_SETPOS;
        fsinfo->param.setpos.file = bf;
        fsinfo->param.setpos.offset = vsf_local.offset;
        fsinfo->param.setpos.poff = vsf_local.result;
        vsf_eda_sem_post(&fsinfo->sem);
        break;
    case VSF_EVT_RETURN:
        if (fsinfo->result.err == VSF_ERR_NONE && fsinfo->param.setpos.roff >= 0) {
            file->use_as__vk_file_t.pos = (uint64_t)fsinfo->param.setpos.roff;
            if (vsf_local.result) *vsf_local.result = (uint64_t)fsinfo->param.setpos.roff;
        }
        vsf_eda_return(fsinfo->result.err);
        break;
    }
    vsf_peda_end();
}

/* helpers */
static vfs_entry_t *find_entry(const char *path) {
    if (!path) return NULL;
    for (int i = 0; i < VFS_MAX_REGISTERED; i++)
        if (s_vfs_entries[i].in_use && !strcmp(s_vfs_entries[i].base_path, path))
            return &s_vfs_entries[i];
    return NULL;
}
static vfs_entry_t *alloc_entry(void) {
    for (int i = 0; i < VFS_MAX_REGISTERED; i++)
        if (!s_vfs_entries[i].in_use) return &s_vfs_entries[i];
    return NULL;
}

/* registration */
esp_err_t esp_vfs_register_fs(const char *base_path, const esp_vfs_fs_ops_t *vfs, int flags, void *ctx)
{
    if (!base_path || !vfs || base_path[0] != '/') return ESP_ERR_INVALID_ARG;
    if (strlen(base_path) > ESP_VFS_PATH_MAX) return ESP_ERR_INVALID_ARG;
    if (find_entry(base_path)) return ESP_ERR_INVALID_STATE;

    vfs_entry_t *e = alloc_entry();
    if (!e) return ESP_ERR_NO_MEM;
    memset(e, 0, sizeof(*e));
    e->in_use = true;
    strncpy(e->base_path, base_path, ESP_VFS_PATH_MAX);
    e->flags = flags;
    e->ctx = ctx;

    if (flags & ESP_VFS_FLAG_STATIC) {
        e->ops = vfs;
        e->own_ops = false;
    } else {
#if VSF_USE_HEAP == ENABLED
        esp_vfs_fs_ops_t *copy = vsf_heap_malloc(sizeof(*copy));
        if (!copy) { memset(e, 0, sizeof(*e)); return ESP_ERR_NO_MEM; }
        memcpy(copy, vfs, sizeof(*copy));
        e->ops = copy;
        e->own_ops = true;
#else
        e->ops = vfs;
        e->own_ops = false;
#endif
    }

    e->fsinfo.ops = e->ops;
    e->fsinfo.ctx = ctx;
    e->fsinfo.flags = flags;
    e->fsinfo.fs_op = &__bridge_fs_op;

    vsf_eda_sem_init(&e->fsinfo.sem);
    if (pthread_create(&e->fsinfo.thread, NULL, __bridge_thread, &e->fsinfo) != 0) {
        if (e->own_ops) vsf_heap_free((void *)e->ops);
        memset(e, 0, sizeof(*e));
        return ESP_FAIL;
    }

    (void)mkdir(base_path, 0777);
    if (mount(NULL, base_path, &__bridge_fs_op, 0, &e->fsinfo) != 0) {
        (void)rmdir(base_path);
        if (e->own_ops) vsf_heap_free((void *)e->ops);
        memset(e, 0, sizeof(*e));
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t esp_vfs_unregister_fs(const char *base_path)
{
    if (!base_path) return ESP_ERR_INVALID_ARG;
    vfs_entry_t *e = find_entry(base_path);
    if (!e) return ESP_ERR_INVALID_STATE;
    if (e->fsinfo.mounted) { (void)umount(base_path); e->fsinfo.mounted = false; }
    (void)rmdir(base_path);
    if (e->own_ops) { vsf_heap_free((void *)e->ops); e->own_ops = false; }
    memset(e, 0, sizeof(*e));
    return ESP_OK;
}

/* legacy wrappers */
esp_err_t esp_vfs_register(const char *base_path, const esp_vfs_t *vfs, void *ctx)
{
    if (!base_path || !vfs) return ESP_ERR_INVALID_ARG;
    esp_vfs_fs_ops_t ops; memset(&ops, 0, sizeof(ops));
    if (vfs->flags & ESP_VFS_FLAG_CONTEXT_PTR) {
        ops.write_p = (esp_vfs_write_ctx_op_t)vfs->write_p;
        ops.lseek_p = (esp_vfs_lseek_ctx_op_t)vfs->lseek_p;
        ops.read_p = (esp_vfs_read_ctx_op_t)vfs->read_p;
        ops.open_p = (esp_vfs_open_ctx_op_t)vfs->open_p;
        ops.close_p = (esp_vfs_close_ctx_op_t)vfs->close_p;
        ops.fstat_p = (esp_vfs_fstat_ctx_op_t)vfs->fstat_p;
    } else {
        ops.write = vfs->write; ops.lseek = vfs->lseek; ops.read = vfs->read;
        ops.open = vfs->open; ops.close = vfs->close; ops.fstat = vfs->fstat;
    }
    return esp_vfs_register_fs(base_path, &ops, vfs->flags, ctx);
}

esp_err_t esp_vfs_unregister(const char *base_path) { return esp_vfs_unregister_fs(base_path); }
esp_err_t esp_vfs_register_fd_range(const esp_vfs_t *v, void *c, int min, int max) { (void)v;(void)c;(void)min;(void)max; return ESP_ERR_NOT_SUPPORTED; }
esp_err_t esp_vfs_register_fs_with_id(const esp_vfs_fs_ops_t *v, int f, void *c, esp_vfs_id_t *id) { (void)v;(void)f;(void)c;(void)id; return ESP_ERR_NOT_SUPPORTED; }
esp_err_t esp_vfs_unregister_fs_with_id(esp_vfs_id_t id) { (void)id; return ESP_ERR_NOT_SUPPORTED; }
esp_err_t esp_vfs_register_with_id(const esp_vfs_t *v, void *c, esp_vfs_id_t *id) { (void)v;(void)c;(void)id; return ESP_ERR_NOT_SUPPORTED; }
esp_err_t esp_vfs_unregister_with_id(esp_vfs_id_t id) { (void)id; return ESP_ERR_NOT_SUPPORTED; }
esp_err_t esp_vfs_register_fd(esp_vfs_id_t id, int *fd) { (void)id;(void)fd; return ESP_ERR_NOT_SUPPORTED; }
esp_err_t esp_vfs_register_fd_with_local_fd(esp_vfs_id_t id, int l, bool p, int *fd) { (void)id;(void)l;(void)p;(void)fd; return ESP_ERR_NOT_SUPPORTED; }
esp_err_t esp_vfs_unregister_fd(esp_vfs_id_t id, int fd) { (void)id;(void)fd; return ESP_ERR_NOT_SUPPORTED; }

/* posix dispatch wrappers */
ssize_t esp_vfs_write(struct _reent *r, int fd, const void *d, size_t s) { (void)r; return write(fd, d, s); }
off_t esp_vfs_lseek(struct _reent *r, int fd, off_t s, int m) { (void)r; return lseek(fd, s, m); }
ssize_t esp_vfs_read(struct _reent *r, int fd, void *d, size_t s) { (void)r; return read(fd, d, s); }
int esp_vfs_open(struct _reent *r, const char *p, int f, int m) { (void)r; return open(p, f, m); }
int esp_vfs_close(struct _reent *r, int fd) { (void)r; return close(fd); }
int esp_vfs_fstat(struct _reent *r, int fd, struct stat *st) { (void)r; return fstat(fd, st); }
int esp_vfs_stat(struct _reent *r, const char *p, struct stat *st) { (void)r; return stat(p, st); }
int esp_vfs_link(struct _reent *r, const char *a, const char *b) { (void)r; return link(a, b); }
int esp_vfs_unlink(struct _reent *r, const char *p) { (void)r; return unlink(p); }
int esp_vfs_rename(struct _reent *r, const char *a, const char *b) { (void)r; return rename(a, b); }
int esp_vfs_utime(const char *p, const struct utimbuf *t) { (void)p;(void)t; errno = ENOSYS; return -1; }
int esp_vfs_fsync(int fd) { return fsync(fd); }
int esp_vfs_fcntl_r(struct _reent *r, int fd, int c, int a) { (void)r; return fcntl(fd, c, a); }
int esp_vfs_ioctl(int fd, int c, ...) { (void)fd;(void)c; errno = ENOSYS; return -1; }
int esp_vfs_truncate(const char *p, off_t l) { return truncate(p, l); }
int esp_vfs_ftruncate(int fd, off_t l) { return ftruncate(fd, l); }
int esp_vfs_access(const char *p, int a) { return access(p, a); }
int esp_vfs_rmdir(const char *n) { return rmdir(n); }
int esp_vfs_mkdir(const char *n, mode_t m) { return mkdir(n, m); }
DIR *esp_vfs_opendir(const char *n) { return opendir(n); }
int esp_vfs_closedir(DIR *d) { return closedir(d); }
int esp_vfs_readdir_r(DIR *d, struct dirent *e, struct dirent **o) { return readdir_r(d, e, o); }
struct dirent *esp_vfs_readdir(DIR *d) { return readdir(d); }
long esp_vfs_telldir(DIR *d) { return telldir(d); }
void esp_vfs_seekdir(DIR *d, long l) { seekdir(d, l); }
void esp_vfs_rewinddir(DIR *d) { rewinddir(d); }
int esp_vfs_select(int n, fd_set *r, fd_set *w, fd_set *e, struct timeval *t) { return select(n, r, w, e, t); }
void esp_vfs_select_triggered(esp_vfs_select_sem_t s) { (void)s; }
void esp_vfs_select_triggered_isr(esp_vfs_select_sem_t s, int *w) { (void)s;(void)w; }
ssize_t esp_vfs_pread(int fd, void *d, size_t s, off_t o) { return pread(fd, d, s, o); }
ssize_t esp_vfs_pwrite(int fd, const void *s, size_t sz, off_t o) { return pwrite(fd, s, sz, o); }
void esp_vfs_dump_fds(FILE *f) { (void)f; }
void esp_vfs_dump_registered_paths(FILE *f) { (void)f; }

#endif