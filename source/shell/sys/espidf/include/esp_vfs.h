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

/*
 * esp_vfs.h -- ESP-IDF VFS compatibility header (clean-room).
 *
 * POSIX filesystem API surface (open/close/read/write/lseek/stat/
 * opendir/readdir/mkdir/unlink/rename/...) is provided natively by VSF's
 * linux shell (shell/sys/linux) which wraps component/fs as synchronous
 * POSIX calls. Users include <stdio.h> / <unistd.h> / <sys/stat.h> etc.
 * and get matching behaviour.
 *
 * Runtime VFS driver registration (esp_vfs_register_fs / esp_vfs_register)
 * maintains an internal registry. When active (VSF_USE_ESPIDF && USE_VFS),
 * the open() syscall checks the registry for prefix matches before falling
 * through to VSF's native VFS.
 */

#ifndef __VSF_ESPIDF_ESP_VFS_H__
#define __VSF_ESPIDF_ESP_VFS_H__

/*============================ INCLUDES ======================================*/

#include "esp_err.h"
#include "esp_vfs_ops.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// POSIX surface
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef ESP_VFS_PATH_MAX
#   define ESP_VFS_PATH_MAX         15
#endif

#define MAX_FDS                     FD_SETSIZE

#define ESP_VFS_FLAG_DEFAULT        (1 << 0)
#define ESP_VFS_FLAG_CONTEXT_PTR    (1 << 1)
#define ESP_VFS_FLAG_READONLY_FS    (1 << 2)
#define ESP_VFS_FLAG_STATIC         (1 << 3)

/*============================ TYPES =========================================*/

/*
 * Legacy VFS definition structure (deprecated in favour of esp_vfs_fs_ops_t).
 *
 * Kept for backward compatibility with ESP-IDF code that initialises
 * esp_vfs_t directly. New code should use esp_vfs_fs_ops_t (esp_vfs_ops.h).
 */
typedef struct {
    int flags;
    union {
        ssize_t (*write_p)(void *ctx, int fd, const void *data, size_t size);
        ssize_t (*write)  (          int fd, const void *data, size_t size);
    };
    union {
        off_t   (*lseek_p)(void *ctx, int fd, off_t size, int mode);
        off_t   (*lseek)  (          int fd, off_t size, int mode);
    };
    union {
        ssize_t (*read_p) (void *ctx, int fd, void *dst, size_t size);
        ssize_t (*read)   (          int fd, void *dst, size_t size);
    };
    union {
        ssize_t (*pread_p)(void *ctx, int fd, void *dst, size_t size, off_t offset);
        ssize_t (*pread)  (          int fd, void *dst, size_t size, off_t offset);
    };
    union {
        ssize_t (*pwrite_p)(void *ctx, int fd, const void *src, size_t size, off_t offset);
        ssize_t (*pwrite)  (          int fd, const void *src, size_t size, off_t offset);
    };
    union {
        int     (*open_p) (void *ctx, const char *path, int flags, int mode);
        int     (*open)   (          const char *path, int flags, int mode);
    };
    union {
        int     (*close_p)(void *ctx, int fd);
        int     (*close)  (          int fd);
    };
    union {
        int     (*fstat_p)(void *ctx, int fd, struct stat *st);
        int     (*fstat)  (          int fd, struct stat *st);
    };
    union {
        int     (*stat_p) (void *ctx, const char *path, struct stat *st);
        int     (*stat)   (          const char *path, struct stat *st);
    };
    union {
        int     (*link_p) (void *ctx, const char *n1, const char *n2);
        int     (*link)   (          const char *n1, const char *n2);
    };
    union {
        int     (*unlink_p)(void *ctx, const char *path);
        int     (*unlink)  (          const char *path);
    };
    union {
        int     (*rename_p)(void *ctx, const char *src, const char *dst);
        int     (*rename)  (          const char *src, const char *dst);
    };
    union {
        DIR *  (*opendir_p) (void *ctx, const char *name);
        DIR *  (*opendir)   (          const char *name);
    };
    union {
        struct dirent * (*readdir_p) (void *ctx, DIR *pdir);
        struct dirent * (*readdir)   (          DIR *pdir);
    };
    union {
        int     (*readdir_r_p)(void *ctx, DIR *pdir, struct dirent *entry,
                               struct dirent **out);
        int     (*readdir_r)  (          DIR *pdir, struct dirent *entry,
                               struct dirent **out);
    };
    union {
        long    (*telldir_p)(void *ctx, DIR *pdir);
        long    (*telldir)  (          DIR *pdir);
    };
    union {
        void    (*seekdir_p)(void *ctx, DIR *pdir, long offset);
        void    (*seekdir)  (          DIR *pdir, long offset);
    };
    union {
        int     (*closedir_p)(void *ctx, DIR *pdir);
        int     (*closedir)  (          DIR *pdir);
    };
    union {
        int     (*mkdir_p)(void *ctx, const char *name, mode_t mode);
        int     (*mkdir)  (          const char *name, mode_t mode);
    };
    union {
        int     (*rmdir_p)(void *ctx, const char *name);
        int     (*rmdir)  (          const char *name);
    };
    union {
        int     (*fcntl_p)(void *ctx, int fd, int cmd, int arg);
        int     (*fcntl)  (          int fd, int cmd, int arg);
    };
    union {
        int     (*ioctl_p)(void *ctx, int fd, int cmd, va_list args);
        int     (*ioctl)  (          int fd, int cmd, va_list args);
    };
    union {
        int     (*fsync_p)(void *ctx, int fd);
        int     (*fsync)  (          int fd);
    };
    union {
        int     (*access_p)(void *ctx, const char *path, int amode);
        int     (*access)  (          const char *path, int amode);
    };
    union {
        int     (*truncate_p) (void *ctx, const char *path, off_t length);
        int     (*truncate)   (          const char *path, off_t length);
    };
    union {
        int     (*ftruncate_p)(void *ctx, int fd, off_t length);
        int     (*ftruncate)  (          int fd, off_t length);
    };
    union {
        int     (*utime_p)(void *ctx, const char *path, const struct utimbuf *times);
        int     (*utime)  (          const char *path, const struct utimbuf *times);
    };
} esp_vfs_t;

/*============================ PROTOTYPES ====================================*/

// ---- Registration / unregistration (old esp_vfs_t API) ----

extern esp_err_t esp_vfs_register(const char *base_path,
                                  const esp_vfs_t *vfs,
                                  void *ctx);
extern esp_err_t esp_vfs_unregister(const char *base_path);
extern esp_err_t esp_vfs_register_fd_range(const esp_vfs_t *vfs, void *ctx,
                                           int min_fd, int max_fd);

// ---- Registration / unregistration (new esp_vfs_fs_ops_t API) ----
//      Declared in esp_vfs_ops.h:
//      esp_vfs_register_fs(), esp_vfs_register_fs_with_id(),
//      esp_vfs_unregister_fs(), esp_vfs_unregister_fs_with_id()

// ---- VFS-ID-based registration ----

extern esp_err_t esp_vfs_register_with_id(const esp_vfs_t *vfs, void *ctx,
                                          esp_vfs_id_t *vfs_id);
extern esp_err_t esp_vfs_unregister_with_id(esp_vfs_id_t vfs_id);
extern esp_err_t esp_vfs_register_fd(esp_vfs_id_t vfs_id, int *fd);
extern esp_err_t esp_vfs_register_fd_with_local_fd(esp_vfs_id_t vfs_id,
                                                   int local_fd, bool permanent,
                                                   int *fd);
extern esp_err_t esp_vfs_unregister_fd(esp_vfs_id_t vfs_id, int fd);

// ---- POSIX syscall dispatch (used by newlib syscall table; thin wrappers) ----

extern ssize_t     esp_vfs_write(struct _reent *r, int fd,
                                 const void *data, size_t size);
extern off_t       esp_vfs_lseek(struct _reent *r, int fd,
                                 off_t size, int mode);
extern ssize_t     esp_vfs_read(struct _reent *r, int fd,
                                void *dst, size_t size);
extern int         esp_vfs_open(struct _reent *r, const char *path,
                                int flags, int mode);
extern int         esp_vfs_close(struct _reent *r, int fd);
extern int         esp_vfs_fstat(struct _reent *r, int fd, struct stat *st);
extern int         esp_vfs_stat(struct _reent *r, const char *path,
                                struct stat *st);
extern int         esp_vfs_link(struct _reent *r, const char *n1,
                                const char *n2);
extern int         esp_vfs_unlink(struct _reent *r, const char *path);
extern int         esp_vfs_rename(struct _reent *r, const char *src,
                                  const char *dst);
extern int         esp_vfs_utime(const char *path,
                                 const struct utimbuf *times);
extern int         esp_vfs_fsync(int fd);
extern int         esp_vfs_fcntl_r(struct _reent *r, int fd,
                                   int cmd, int arg);
extern int         esp_vfs_ioctl(int fd, int cmd, ...);
extern int         esp_vfs_truncate(const char *path, off_t length);
extern int         esp_vfs_ftruncate(int fd, off_t length);
extern int         esp_vfs_access(const char *path, int amode);
extern int         esp_vfs_rmdir(const char *name);
extern int         esp_vfs_mkdir(const char *name, mode_t mode);
extern DIR *       esp_vfs_opendir(const char *name);
extern int         esp_vfs_closedir(DIR *pdir);
extern int         esp_vfs_readdir_r(DIR *pdir, struct dirent *entry,
                                     struct dirent **out_dirent);
extern struct dirent * esp_vfs_readdir(DIR *pdir);
extern long        esp_vfs_telldir(DIR *pdir);
extern void        esp_vfs_seekdir(DIR *pdir, long loc);
extern void        esp_vfs_rewinddir(DIR *pdir);

// ---- select / pread / pwrite ----

extern int         esp_vfs_select(int nfds, fd_set *readfds,
                                  fd_set *writefds, fd_set *errorfds,
                                  struct timeval *timeout);
extern void        esp_vfs_select_triggered(esp_vfs_select_sem_t sem);
extern void        esp_vfs_select_triggered_isr(esp_vfs_select_sem_t sem,
                                                int *woken);
extern ssize_t     esp_vfs_pread(int fd, void *dst, size_t size,
                                 off_t offset);
extern ssize_t     esp_vfs_pwrite(int fd, const void *src, size_t size,
                                  off_t offset);

// ---- Debug ----

extern void        esp_vfs_dump_fds(FILE *fp);
extern void        esp_vfs_dump_registered_paths(FILE *fp);

#ifdef __cplusplus
}
#endif

#endif      // __VSF_ESPIDF_ESP_VFS_H__
