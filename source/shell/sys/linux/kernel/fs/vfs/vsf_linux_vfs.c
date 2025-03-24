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

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_USE_LINUX == ENABLED

#define __VSF_FS_CLASS_INHERIT__
#define __VSF_LINUX_FS_CLASS_INHERIT__
#define __VSF_LINUX_CLASS_IMPLEMENT

#include <vsf.h>

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../../../include/unistd.h"
#   include "../../../include/linux/limits.h"
#   include "shell/sys/linux/include/sys/stat.h"
#   include "shell/sys/linux/include/sys/sysinfo.h"
#else
#   include <unistd.h>
// for PATH_MAX
#   include <linux/limits.h>
#   include <sys/stat.h>
#   include <sys/sysinfo.h>
#endif

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_LIBC == ENABLED
#   include "../../../include/simple_libc/stdlib.h"
#   include "../../../include/simple_libc/stdio.h"
#else
#   include <stdlib.h>
#   include <stdio.h>
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_LINUX_USE_PROCFS == ENABLED

// /proc/self/exe

typedef struct vsf_linux_proc_self_exe_priv_t {
    implement(vsf_linux_fs_priv_t)
    char linkpath[PATH_MAX];
} vsf_linux_proc_self_exe_priv_t;

static void __vsf_linux_proc_self_exe_init(vsf_linux_fd_t *sfd)
{
    vsf_linux_proc_self_exe_priv_t *priv = (vsf_linux_proc_self_exe_priv_t *)sfd->priv;
    vsf_linux_process_t *process = vsf_linux_get_cur_process();

    strncpy(priv->linkpath, process->path, sizeof(priv->linkpath));
}

static ssize_t __vsf_linux_proc_self_exe_read(vsf_linux_fd_t *sfd, void *buf, size_t count)
{
    vsf_linux_proc_self_exe_priv_t *priv = (vsf_linux_proc_self_exe_priv_t *)sfd->priv;
    strncpy((char *)buf, priv->linkpath, count);
    return strlen((const char *)buf) + 1;
}

static ssize_t __vsf_linux_proc_self_exe_write(vsf_linux_fd_t *sfd, const void *buf, size_t count)
{
    vsf_linux_proc_self_exe_priv_t *priv = (vsf_linux_proc_self_exe_priv_t *)sfd->priv;
    strncpy(priv->linkpath, (const char *)buf, sizeof(priv->linkpath));
    return strlen(priv->linkpath) + 1;
}

static int __vsf_linux_proc_self_exe_stat(vsf_linux_fd_t *sfd, struct stat *buf)
{
    buf->st_mode = S_IFLNK;
    return 0;
}

static const vsf_linux_fd_op_t __vsf_linux_proc_self_exe_fdop = {
    .priv_size          = sizeof(vsf_linux_proc_self_exe_priv_t),
    .feature            = VSF_LINUX_FDOP_FEATURE_FS,
    .fn_init            = __vsf_linux_proc_self_exe_init,
    .fn_read            = __vsf_linux_proc_self_exe_read,
    .fn_write           = __vsf_linux_proc_self_exe_write,
    .fn_stat            = __vsf_linux_proc_self_exe_stat,
};

// /proc/meminfo

typedef struct __vsf_linux_proc_meminfo_ctx_t {
    vk_file_t *file;
    uint8_t *buf;
    size_t count;
    size_t pos;
} __vsf_linux_proc_meminfo_ctx_t;

static void __vsf_linux_proc_meminfo_show_value(__vsf_linux_proc_meminfo_ctx_t *ctx, const char *fmt, unsigned long value)
{
    uint8_t linebuf[64];
    int cnt = snprintf((char *)linebuf, sizeof(linebuf), fmt, value);
    VSF_LINUX_ASSERT(cnt < sizeof(linebuf));
    size_t cur_size = 0, cur_pos_range_min = ctx->file->pos, cur_pos_range_max = cur_pos_range_min + ctx->count;
    size_t cur_pos_min = ctx->pos, cur_pos_max = cur_pos_min + cnt;

    if ((cur_pos_max > cur_pos_range_min) && (cur_pos_min < cur_pos_range_max)) {
        cur_size = vsf_min(cur_pos_max, cur_pos_range_max) - vsf_max(cur_pos_min, cur_pos_range_min);
        memcpy(ctx->buf, &linebuf[cur_pos_range_min > cur_pos_min ? (cur_pos_range_min - cur_pos_min) : 0], cur_size);
        ctx->buf += cur_size;
        ctx->file->pos += cur_size;
        ctx->count -= cur_size;
    } else if (cur_pos_max <= cur_pos_range_min) {
        cur_size = cnt;
    }
    ctx->pos += cur_size;
}

static ssize_t __vsf_linux_proc_meminfo_read(vsf_linux_fd_t *sfd, void *buf, size_t count)
{
    vsf_linux_fs_priv_t *priv = (vsf_linux_fs_priv_t *)sfd->priv;
    uint64_t pos_orig = priv->file->pos;
    __vsf_linux_proc_meminfo_ctx_t ctx = {
        .file   = priv->file,
        .buf    = (uint8_t *)buf,
        .count  = count,
        .pos    = 0,
    };

    struct sysinfo info;
    sysinfo(&info);
    info.totalram >>= 10;
    info.freeram >>= 10;

    __vsf_linux_proc_meminfo_show_value(&ctx, "MemTotal:       %d kB\n", info.totalram);
    __vsf_linux_proc_meminfo_show_value(&ctx, "MemFree:        %d kB\n", info.freeram);
    __vsf_linux_proc_meminfo_show_value(&ctx, "MemAvailable:   %d kB\n", info.freeram);
    __vsf_linux_proc_meminfo_show_value(&ctx, "Buffers:        %d kB\n", 0);
    __vsf_linux_proc_meminfo_show_value(&ctx, "Cached:         %d kB\n", 0);
    __vsf_linux_proc_meminfo_show_value(&ctx, "SwapCached:     %d kB\n", 0);
    __vsf_linux_proc_meminfo_show_value(&ctx, "SwapTotal:      %d kB\n", 0);
    __vsf_linux_proc_meminfo_show_value(&ctx, "SwapFree:       %d kB\n", 0);

    ssize_t result =  ctx.file->pos - pos_orig;
    return result > 0 ? result : 0;
}

static int __vsf_linux_proc_meminfo_stat(vsf_linux_fd_t *sfd, struct stat *buf)
{
    buf->st_mode = S_IFREG;
    return 0;
}

static const vsf_linux_fd_op_t __vsf_linux_proc_meminfo_fdop = {
    .priv_size          = sizeof(vsf_linux_fs_priv_t),
    .feature            = VSF_LINUX_FDOP_FEATURE_FS,
    .fn_read            = __vsf_linux_proc_meminfo_read,
    .fn_stat            = __vsf_linux_proc_meminfo_stat,
};

#   if VSF_LINUX_USE_SOCKET == ENABLED && VSF_LINUX_SOCKET_USE_INET == ENABLED
VSF_CAL_WEAK(__vsf_linux_socket_inet_procfs_bind)
int __vsf_linux_socket_inet_procfs_bind(void)
{
    return 0;
}
#   endif
#endif

int vsf_linux_vfs_init(void)
{
    int err;

    err = mkdir("/run", 0);
    if (err != 0) {
        fprintf(stderr, "fail to mkdir /run\n");
        return err;
    }

#if VSF_LINUX_USE_PROCFS == ENABLED
    err = mkdirs("/proc/self", 0);
    if (err != 0) {
        fprintf(stderr, "fail to mkdir /proc/self\n");
        return err;
    }
    err = vsf_linux_fs_bind_target_ex("/proc/self/exe", NULL, &__vsf_linux_proc_self_exe_fdop,
        NULL, NULL, VSF_FILE_ATTR_READ | VSF_FILE_ATTR_WRITE, 0);
    if (err != 0) {
        fprintf(stderr, "fail to create /proc/self/exe\n");
        return err;
    }
    err = vsf_linux_fs_bind_target_ex("/proc/meminfo", NULL, &__vsf_linux_proc_meminfo_fdop,
        NULL, NULL, VSF_FILE_ATTR_READ, 0);
    if (err != 0) {
        fprintf(stderr, "fail to create /proc/meminfo\n");
        return err;
    }
#   if VSF_LINUX_USE_SOCKET == ENABLED && VSF_LINUX_SOCKET_USE_INET == ENABLED
    err = __vsf_linux_socket_inet_procfs_bind();
    if (err != 0) {
        fprintf(stderr, "fail to bind /proc/net\n");
        return err;
    }
#   endif
#endif

#if VSF_LINUX_USE_DEVFS == ENABLED
    err = vsf_linux_devfs_init();
    if (err != 0) {
        return err;
    }
#endif

    return 0;
}

#endif
