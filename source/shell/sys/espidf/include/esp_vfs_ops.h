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
 * esp_vfs_ops.h -- ESP-IDF VFS operations types (clean-room).
 *
 * This header defines the modular esp_vfs_fs_ops_t type introduced in
 * ESP-IDF v5.2+. The older monolithic esp_vfs_t (in esp_vfs.h) is
 * deprecated in favour of this struct. All production drivers (FATFS,
 * SPIFFS, UART, nullfs, semihost, etc.) use esp_vfs_fs_ops_t with
 * per-category sub-structs (dir, termios, select).
 */

#ifndef __VSF_ESPIDF_ESP_VFS_OPS_H__
#define __VSF_ESPIDF_ESP_VFS_OPS_H__

/*============================ INCLUDES ======================================*/

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <dirent.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ TYPES =========================================*/

/*
 * VFS identification number returned by esp_vfs_register_with_id() /
 * esp_vfs_register_fs_with_id().
 */
typedef int esp_vfs_id_t;

/*
 * Select notification semaphore, used by drivers to signal I/O readiness
 * back to the VFS select/poll dispatcher.
 */
typedef struct {
    bool  is_sem_local;
    void *sem;
} esp_vfs_select_sem_t;

/*============================ BASIC FILE OPERATION TYPES ====================*/

typedef ssize_t (*esp_vfs_write_ctx_op_t) (void *ctx, int fd,
                                           const void *data, size_t size);
typedef ssize_t (*esp_vfs_write_op_t)     (          int fd,
                                           const void *data, size_t size);
typedef off_t   (*esp_vfs_lseek_ctx_op_t) (void *ctx, int fd,
                                           off_t size, int mode);
typedef off_t   (*esp_vfs_lseek_op_t)     (          int fd,
                                           off_t size, int mode);
typedef ssize_t (*esp_vfs_read_ctx_op_t)  (void *ctx, int fd,
                                           void *dst, size_t size);
typedef ssize_t (*esp_vfs_read_op_t)      (          int fd,
                                           void *dst, size_t size);
typedef ssize_t (*esp_vfs_pread_ctx_op_t) (void *ctx, int fd,
                                           void *dst, size_t size, off_t offset);
typedef ssize_t (*esp_vfs_pread_op_t)     (          int fd,
                                           void *dst, size_t size, off_t offset);
typedef ssize_t (*esp_vfs_pwrite_ctx_op_t)(void *ctx, int fd,
                                           const void *src, size_t size,
                                           off_t offset);
typedef ssize_t (*esp_vfs_pwrite_op_t)    (          int fd,
                                           const void *src, size_t size,
                                           off_t offset);
typedef int     (*esp_vfs_open_ctx_op_t)  (void *ctx, const char *path,
                                           int flags, int mode);
typedef int     (*esp_vfs_open_op_t)      (          const char *path,
                                           int flags, int mode);
typedef int     (*esp_vfs_close_ctx_op_t) (void *ctx, int fd);
typedef int     (*esp_vfs_close_op_t)     (          int fd);
typedef int     (*esp_vfs_fstat_ctx_op_t) (void *ctx, int fd, struct stat *st);
typedef int     (*esp_vfs_fstat_op_t)     (          int fd, struct stat *st);
typedef int     (*esp_vfs_fcntl_ctx_op_t) (void *ctx, int fd,
                                           int cmd, int arg);
typedef int     (*esp_vfs_fcntl_op_t)     (          int fd,
                                           int cmd, int arg);
typedef int     (*esp_vfs_ioctl_ctx_op_t) (void *ctx, int fd,
                                           int cmd, va_list args);
typedef int     (*esp_vfs_ioctl_op_t)     (          int fd,
                                           int cmd, va_list args);
typedef int     (*esp_vfs_fsync_ctx_op_t) (void *ctx, int fd);
typedef int     (*esp_vfs_fsync_op_t)     (          int fd);

/*============================ DIRECTORY OPERATION TYPES =====================*/

typedef           int  (*esp_vfs_stat_ctx_op_t)     (void *ctx,
                                              const char *path, struct stat *st);
typedef           int  (*esp_vfs_stat_op_t)         (
                                              const char *path, struct stat *st);
typedef           int  (*esp_vfs_link_ctx_op_t)     (void *ctx,
                                              const char *n1, const char *n2);
typedef           int  (*esp_vfs_link_op_t)         (
                                              const char *n1, const char *n2);
typedef           int  (*esp_vfs_unlink_ctx_op_t)   (void *ctx,
                                              const char *path);
typedef           int  (*esp_vfs_unlink_op_t)       (
                                              const char *path);
typedef           int  (*esp_vfs_rename_ctx_op_t)   (void *ctx,
                                              const char *src, const char *dst);
typedef           int  (*esp_vfs_rename_op_t)       (
                                              const char *src, const char *dst);
typedef           DIR* (*esp_vfs_opendir_ctx_op_t)  (void *ctx,
                                              const char *name);
typedef           DIR* (*esp_vfs_opendir_op_t)      (
                                              const char *name);
typedef struct dirent* (*esp_vfs_readdir_ctx_op_t)  (void *ctx, DIR *pdir);
typedef struct dirent* (*esp_vfs_readdir_op_t)      (          DIR *pdir);
typedef           int  (*esp_vfs_readdir_r_ctx_op_t)(void *ctx, DIR *pdir,
                                              struct dirent *entry,
                                              struct dirent **out);
typedef           int  (*esp_vfs_readdir_r_op_t)    (          DIR *pdir,
                                              struct dirent *entry,
                                              struct dirent **out);
typedef          long  (*esp_vfs_telldir_ctx_op_t)  (void *ctx, DIR *pdir);
typedef          long  (*esp_vfs_telldir_op_t)      (          DIR *pdir);
typedef          void  (*esp_vfs_seekdir_ctx_op_t)  (void *ctx, DIR *pdir,
                                              long offset);
typedef          void  (*esp_vfs_seekdir_op_t)      (          DIR *pdir,
                                              long offset);
typedef           int  (*esp_vfs_closedir_ctx_op_t) (void *ctx, DIR *pdir);
typedef           int  (*esp_vfs_closedir_op_t)     (          DIR *pdir);
typedef           int  (*esp_vfs_mkdir_ctx_op_t)    (void *ctx,
                                              const char *name, mode_t mode);
typedef           int  (*esp_vfs_mkdir_op_t)        (
                                              const char *name, mode_t mode);
typedef           int  (*esp_vfs_rmdir_ctx_op_t)    (void *ctx,
                                              const char *name);
typedef           int  (*esp_vfs_rmdir_op_t)        (
                                              const char *name);
typedef           int  (*esp_vfs_access_ctx_op_t)   (void *ctx,
                                              const char *path, int amode);
typedef           int  (*esp_vfs_access_op_t)       (
                                              const char *path, int amode);
typedef           int  (*esp_vfs_truncate_ctx_op_t) (void *ctx,
                                              const char *path, off_t length);
typedef           int  (*esp_vfs_truncate_op_t)     (
                                              const char *path, off_t length);
typedef           int  (*esp_vfs_ftruncate_ctx_op_t)(void *ctx, int fd,
                                              off_t length);
typedef           int  (*esp_vfs_ftruncate_op_t)    (          int fd,
                                              off_t length);

typedef struct {
    union {
        esp_vfs_stat_ctx_op_t      stat_p;
        esp_vfs_stat_op_t          stat;
    };
    union {
        esp_vfs_link_ctx_op_t      link_p;
        esp_vfs_link_op_t          link;
    };
    union {
        esp_vfs_unlink_ctx_op_t    unlink_p;
        esp_vfs_unlink_op_t        unlink;
    };
    union {
        esp_vfs_rename_ctx_op_t    rename_p;
        esp_vfs_rename_op_t        rename;
    };
    union {
        esp_vfs_opendir_ctx_op_t   opendir_p;
        esp_vfs_opendir_op_t       opendir;
    };
    union {
        esp_vfs_readdir_ctx_op_t   readdir_p;
        esp_vfs_readdir_op_t       readdir;
    };
    union {
        esp_vfs_readdir_r_ctx_op_t readdir_r_p;
        esp_vfs_readdir_r_op_t     readdir_r;
    };
    union {
        esp_vfs_telldir_ctx_op_t   telldir_p;
        esp_vfs_telldir_op_t       telldir;
    };
    union {
        esp_vfs_seekdir_ctx_op_t   seekdir_p;
        esp_vfs_seekdir_op_t       seekdir;
    };
    union {
        esp_vfs_closedir_ctx_op_t  closedir_p;
        esp_vfs_closedir_op_t      closedir;
    };
    union {
        esp_vfs_mkdir_ctx_op_t     mkdir_p;
        esp_vfs_mkdir_op_t         mkdir;
    };
    union {
        esp_vfs_rmdir_ctx_op_t     rmdir_p;
        esp_vfs_rmdir_op_t         rmdir;
    };
    union {
        esp_vfs_access_ctx_op_t    access_p;
        esp_vfs_access_op_t        access;
    };
    union {
        esp_vfs_truncate_ctx_op_t  truncate_p;
        esp_vfs_truncate_op_t      truncate;
    };
    union {
        esp_vfs_ftruncate_ctx_op_t ftruncate_p;
        esp_vfs_ftruncate_op_t     ftruncate;
    };
} esp_vfs_dir_ops_t;

/*============================ MAIN VFS OPERATIONS STRUCT ====================*/

typedef struct {
    union {
        esp_vfs_write_ctx_op_t  write_p;
        esp_vfs_write_op_t      write;
    };
    union {
        esp_vfs_lseek_ctx_op_t  lseek_p;
        esp_vfs_lseek_op_t      lseek;
    };
    union {
        esp_vfs_read_ctx_op_t   read_p;
        esp_vfs_read_op_t       read;
    };
    union {
        esp_vfs_pread_ctx_op_t  pread_p;
        esp_vfs_pread_op_t      pread;
    };
    union {
        esp_vfs_pwrite_ctx_op_t pwrite_p;
        esp_vfs_pwrite_op_t     pwrite;
    };
    union {
        esp_vfs_open_ctx_op_t   open_p;
        esp_vfs_open_op_t       open;
    };
    union {
        esp_vfs_close_ctx_op_t  close_p;
        esp_vfs_close_op_t      close;
    };
    union {
        esp_vfs_fstat_ctx_op_t  fstat_p;
        esp_vfs_fstat_op_t      fstat;
    };
    union {
        esp_vfs_fcntl_ctx_op_t  fcntl_p;
        esp_vfs_fcntl_op_t      fcntl;
    };
    union {
        esp_vfs_ioctl_ctx_op_t  ioctl_p;
        esp_vfs_ioctl_op_t      ioctl;
    };
    union {
        esp_vfs_fsync_ctx_op_t  fsync_p;
        esp_vfs_fsync_op_t      fsync;
    };

    const esp_vfs_dir_ops_t *dir;
} esp_vfs_fs_ops_t;

/*============================ REGISTRATION PROTOTYPES =======================*/

extern esp_err_t esp_vfs_register_fs(const char *base_path,
                                     const esp_vfs_fs_ops_t *vfs,
                                     int flags, void *ctx);
extern esp_err_t esp_vfs_register_fs_with_id(const esp_vfs_fs_ops_t *vfs,
                                             int flags, void *ctx,
                                             esp_vfs_id_t *id);
extern esp_err_t esp_vfs_unregister_fs(const char *base_path);
extern esp_err_t esp_vfs_unregister_fs_with_id(esp_vfs_id_t id);

#ifdef __cplusplus
}
#endif

#endif      // __VSF_ESPIDF_ESP_VFS_OPS_H__
