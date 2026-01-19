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

#define __VSF_SIMPLE_STREAM_CLASS_INHERIT__
#define __VSF_FS_CLASS_INHERIT__
#define __VSF_LINUX_FS_CLASS_IMPLEMENT
#define __VSF_LINUX_CLASS_IMPLEMENT

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../../include/unistd.h"
#   include "../../include/sys/stat.h"
#   include "../../include/sys/mount.h"
#   include "../../include/sys/epoll.h"
#   include "../../include/sys/statvfs.h"
#   include "../../include/sys/sendfile.h"
#   include "../../include/sys/statfs.h"
#   include "../../include/sys/file.h"
#   include "../../include/sys/ioctl.h"
#   include "../../include/sys/mman.h"
#   include "../../include/poll.h"
#   include "../../include/fcntl.h"
#   include "../../include/errno.h"
#   include "../../include/termios.h"
#   include "../../include/mntent.h"
#   include "../../include/glob.h"
#   include "../../include/fnmatch.h"
#   include "../../include/linux/limits.h"
#else
#   include <unistd.h>
#   include <sys/stat.h>
#   include <sys/mount.h>
#   include <sys/epoll.h>
#   include <sys/statvfs.h>
#   include <sys/sendfile.h>
#   include <sys/statfs.h>
#   include <sys/file.h>
#   include <sys/ioctl.h>
#   include <sys/mman.h>
#   include <poll.h>
#   include <fcntl.h>
#   include <errno.h>
#   include <termios.h>
#   include <mntent.h>
#   include <glob.h>
#   include <fnmatch.h>
// for PATH_MAX
#   include <linux/limits.h>
#endif

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_STDLIB == ENABLED
#   include "../include/simple_libc/stdlib.h"
#else
#   include <stdlib.h>
#endif
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_STRING == ENABLED
#   include "../include/simple_libc/string.h"
#else
#   include <string.h>
#endif
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_STDIO == ENABLED
#   include "../include/simple_libc/stdio.h"
#else
#   include <stdio.h>
#endif
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../include/sys/eventfd.h"
#   include "../include/sys/signalfd.h"
#else
#   include <sys/eventfd.h>
#   include <sys/signalfd.h>
#endif

#include "./vsf_linux_fs.h"
#include "../socket/vsf_linux_socket.h"

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_suppress=pe111
#endif

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#endif

/*============================ MACROS ========================================*/

#if VSF_USE_SIMPLE_STREAM != ENABLED
#   error vsf_linux_fs need VSF_USE_SIMPLE_STREAM
#endif

#if VSF_FS_CFG_VFS_FILE_HAS_OP != ENABLED
#   error vsf_linux_fs need VSF_FS_CFG_VFS_FILE_HAS_OP
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_linux_eventfd_priv_t {
    implement(vsf_linux_fd_priv_t)
    eventfd_t counter;
} vsf_linux_eventfd_priv_t;

typedef struct vsf_linux_signalfd_priv_t {
    implement(vsf_linux_fd_priv_t)
    sigset_t *sigset;
} vsf_linux_signalfd_priv_t;

typedef struct vsf_linux_fd_trigger_t {
    vsf_linux_trigger_t *trigger;
} vsf_linux_fd_trigger_t;

typedef struct vsf_linux_epoll_node_t {
    vsf_linux_fd_priv_t *fd_priv;
    vsf_linux_fd_priv_callback_t *events_callback;
    uint32_t events;
    epoll_data_t data;
    vsf_dlist_node_t rdy_node;
    vsf_dlist_node_t fd_node;
} vsf_linux_epoll_node_t;

typedef struct vsf_linux_epollfd_priv_t {
    implement(vsf_linux_fd_priv_t)
    vsf_dlist_t rdy_list;
    vsf_dlist_t fd_list;
    vsf_linux_trigger_t *trigger;
} vsf_linux_epollfd_priv_t;

/*============================ PROTOTYPES ====================================*/

static int __vsf_linux_fs_fcntl(vsf_linux_fd_t *sfd, int cmd, uintptr_t arg);
static ssize_t __vsf_linux_fs_read(vsf_linux_fd_t *sfd, void *buf, size_t count);
static ssize_t __vsf_linux_fs_write(vsf_linux_fd_t *sfd, const void *buf, size_t count);
static int __vsf_linux_fs_close(vsf_linux_fd_t *sfd);
static int __vsf_linux_fs_eof(vsf_linux_fd_t *sfd);
static int __vsf_linux_fs_setsize(vsf_linux_fd_t *sfd, off64_t size);
static int __vsf_linux_fs_stat(vsf_linux_fd_t *sfd, struct stat *buf);
static void * __vsf_linux_fs_mmap(vsf_linux_fd_t *sfd, off64_t offset, size_t len, uint_fast32_t feature);

static int __vsf_linux_eventfd_fcntl(vsf_linux_fd_t *sfd, int cmd, uintptr_t arg);
static ssize_t __vsf_linux_eventfd_read(vsf_linux_fd_t *sfd, void *buf, size_t count);
static ssize_t __vsf_linux_eventfd_write(vsf_linux_fd_t *sfd, const void *buf, size_t count);
static int __vsf_linux_eventfd_close(vsf_linux_fd_t *sfd);
static int __vsf_linux_eventfd_eof(vsf_linux_fd_t *sfd);

static int __vsf_linux_epollfd_close(vsf_linux_fd_t *sfd);

static void __vsf_linux_stream_init(vsf_linux_fd_t *sfd);
static int __vsf_linux_stream_fcntl(vsf_linux_fd_t *sfd, int cmd, uintptr_t arg);
ssize_t __vsf_linux_stream_read(vsf_linux_fd_t *sfd, void *buf, size_t count);
ssize_t __vsf_linux_stream_write(vsf_linux_fd_t *sfd, const void *buf, size_t count);
static int __vsf_linux_stream_close(vsf_linux_fd_t *sfd);
static int __vsf_linux_stream_eof(vsf_linux_fd_t *sfd);

int __vsf_linux_pipe_fcntl(vsf_linux_fd_t *sfd, int cmd, uintptr_t arg);
static int __vsf_linux_pipe_stat(vsf_linux_fd_t *sfd, struct stat *buf);
static int __vsf_linux_pipe_close(vsf_linux_fd_t *sfd);

static void __vsf_linux_term_init(vsf_linux_fd_t *sfd);
static void __vsf_linux_term_fini(vsf_linux_fd_t *sfd);
static int __vsf_linux_term_fcntl(vsf_linux_fd_t *sfd, int cmd, uintptr_t arg);
static int __vsf_linux_term_stat(vsf_linux_fd_t *sfd, struct stat *buf);
static ssize_t __vsf_linux_term_read(vsf_linux_fd_t *sfd, void *buf, size_t count);
static ssize_t __vsf_linux_term_write(vsf_linux_fd_t *sfd, const void *buf, size_t count);
static int __vsf_linux_term_close(vsf_linux_fd_t *sfd);

static void __vsf_linux_key_fini(vsf_linux_fd_t *sfd);
static int __vsf_linux_key_close(vsf_linux_fd_t *sfd);
static int __vsf_linux_key_stat(vsf_linux_fd_t *sfd, struct stat *buf);

#if VSF_LINUX_CFG_STDIO_FALLBACK == ENABLED
ssize_t __vsf_linux_stdio_fallback_read(vsf_linux_fd_t *sfd, void *buf, size_t count);
ssize_t __vsf_linux_stdio_fallback_write(vsf_linux_fd_t *sfd, const void *buf, size_t count);
#endif

/*============================ GLOBAL VARIABLES ==============================*/

const vsf_linux_fd_op_t __vsf_linux_fs_fdop = {
    .priv_size          = sizeof(vsf_linux_fs_priv_t),
    .feature            = VSF_LINUX_FDOP_FEATURE_FS,
    .fn_fcntl           = __vsf_linux_fs_fcntl,
    .fn_read            = __vsf_linux_fs_read,
    .fn_write           = __vsf_linux_fs_write,
    .fn_close           = __vsf_linux_fs_close,
    .fn_eof             = __vsf_linux_fs_eof,
    .fn_setsize         = __vsf_linux_fs_setsize,
    .fn_stat            = __vsf_linux_fs_stat,
    .fn_mmap            = __vsf_linux_fs_mmap,
};

const vsf_linux_fd_op_t __vsf_linux_eventfd_fdop = {
    .priv_size          = sizeof(vsf_linux_eventfd_priv_t),
    .fn_fcntl           = __vsf_linux_eventfd_fcntl,
    .fn_read            = __vsf_linux_eventfd_read,
    .fn_write           = __vsf_linux_eventfd_write,
    .fn_close           = __vsf_linux_eventfd_close,
    .fn_eof             = __vsf_linux_eventfd_eof,
};

const vsf_linux_fd_op_t __vsf_linux_signalfd_fdop = {
    .priv_size          = sizeof(vsf_linux_signalfd_priv_t),
};

const vsf_linux_fd_op_t __vsf_linux_epollfd_fdop = {
    .priv_size          = sizeof(vsf_linux_epollfd_priv_t),
    .fn_close           = __vsf_linux_epollfd_close,
};

const vsf_linux_fd_op_t __vsf_linux_stream_fdop = {
    .priv_size          = sizeof(vsf_linux_stream_priv_t),
    .feature            = VSF_LINUX_FDOP_FEATURE_FS,
    .fn_init            = __vsf_linux_stream_init,
    .fn_fcntl           = __vsf_linux_stream_fcntl,
    .fn_read            = __vsf_linux_stream_read,
    .fn_write           = __vsf_linux_stream_write,
    .fn_close           = __vsf_linux_stream_close,
};

const vsf_linux_fd_op_t vsf_linux_pipe_fdop = {
    .priv_size          = sizeof(vsf_linux_pipe_priv_t),
    .feature            = VSF_LINUX_FDOP_FEATURE_FS,
    .fn_fcntl           = __vsf_linux_pipe_fcntl,
    .fn_read            = __vsf_linux_stream_read,
    .fn_write           = __vsf_linux_stream_write,
    .fn_close           = __vsf_linux_pipe_close,
    .fn_eof             = __vsf_linux_stream_eof,
    .fn_stat            = __vsf_linux_pipe_stat,
};

const vsf_linux_fd_op_t vsf_linux_term_fdop = {
    .priv_size          = sizeof(vsf_linux_term_priv_t),
    .feature            = VSF_LINUX_FDOP_FEATURE_FS,
    .fn_init            = __vsf_linux_term_init,
    .fn_fini            = __vsf_linux_term_fini,
    .fn_fcntl           = __vsf_linux_term_fcntl,
    .fn_read            = __vsf_linux_term_read,
    .fn_write           = __vsf_linux_term_write,
    .fn_close           = __vsf_linux_term_close,
    .fn_stat            = __vsf_linux_term_stat,
};

const vsf_linux_fd_op_t vsf_linux_key_fdop = {
    .priv_size          = sizeof(vsf_linux_key_priv_t),
    .fn_fini            = __vsf_linux_key_fini,
    .fn_close           = __vsf_linux_key_close,
    .fn_stat            = __vsf_linux_key_stat,
};

/*============================ LOCAL VARIABLES ===============================*/

#if VSF_LINUX_CFG_STDIO_FALLBACK == ENABLED
const vsf_linux_fd_op_t __vsf_linux_stdio_fallback_fdop = {
    .priv_size          = sizeof(vsf_linux_fd_priv_t),
    .fn_read            = __vsf_linux_stdio_fallback_read,
    .fn_write           = __vsf_linux_stdio_fallback_write,
};

static vsf_linux_fd_priv_t __vsf_linux_stdio_fallback_priv[3] = {
    [STDIN_FILENO]      = {
        .flags          = O_RDONLY,
    },
    [STDOUT_FILENO]     = {
        .flags          = O_WRONLY,
    },
    [STDERR_FILENO]     = {
        .flags          = O_RDWR,
    },
};
static vsf_linux_fd_t __vsf_linux_stdio_fallback[3] = {
    [STDIN_FILENO]      = {
        .fd             = STDIN_FILENO,
        .op             = &__vsf_linux_stdio_fallback_fdop,
        .priv           = &__vsf_linux_stdio_fallback_priv[STDIN_FILENO],
    },
    [STDOUT_FILENO]     = {
        .fd             = STDOUT_FILENO,
        .op             = &__vsf_linux_stdio_fallback_fdop,
        .priv           = &__vsf_linux_stdio_fallback_priv[STDOUT_FILENO],
    },
    [STDERR_FILENO]     = {
        .fd             = STDERR_FILENO,
        .op             = &__vsf_linux_stdio_fallback_fdop,
        .priv           = &__vsf_linux_stdio_fallback_priv[STDERR_FILENO],
    },
};
#endif

/*============================ IMPLEMENTATION ================================*/

#if VSF_LINUX_CFG_STDIO_FALLBACK == ENABLED
VSF_CAL_WEAK(__vsf_linux_stdio_fallback_read)
ssize_t __vsf_linux_stdio_fallback_read(vsf_linux_fd_t *sfd, void *buf, size_t count)
{
    VSF_LINUX_ASSERT(false);
    return -1;
}

VSF_CAL_WEAK(__vsf_linux_stdio_fallback_write)
ssize_t __vsf_linux_stdio_fallback_write(vsf_linux_fd_t *sfd, const void *buf, size_t count)
{
    extern uint_fast32_t __vsf_trace_output(const char *buff, uint_fast32_t size);
    return (ssize_t)__vsf_trace_output(buf, count);
}
#endif

int __vsf_linux_default_fcntl(vsf_linux_fd_t *sfd, int cmd, uintptr_t arg)
{
    switch (cmd) {
    case FIONBIO:
        if (*(int *)arg) {
            sfd->priv->flags |= O_NONBLOCK;
        } else {
            sfd->priv->flags &= ~O_NONBLOCK;
        }
        // fall through
    case F_SETFD:
    case F_SETFL:
        return 0;
    default:
        return -1;
    }
}

vk_file_t * __vsf_linux_fs_get_file_ex(vk_file_t *dir, const char *pathname)
{
    vk_file_t *file;
    vk_file_open(dir, pathname, &file);
    return file;
}

static vk_file_t * __vsf_linux_fs_get_file(const char *pathname)
{
    return __vsf_linux_fs_get_file_ex(NULL, pathname);
}

static void __vsf_linux_fs_close_do(vk_file_t *file)
{
    vk_file_close(file);
}

static int __vsf_linux_fs_fcntl(vsf_linux_fd_t *sfd, int cmd, uintptr_t arg)
{
    return __vsf_linux_default_fcntl(sfd, cmd, arg);
}

static ssize_t __vsf_linux_fs_read(vsf_linux_fd_t *sfd, void *buf, size_t count)
{
    vsf_linux_fs_priv_t *priv = (vsf_linux_fs_priv_t *)sfd->priv;
    vk_file_t *file = priv->file;
    ssize_t result = 0;
    int32_t rsize;

    while (count > 0) {
#if VSF_LINUX_CFG_FS_CACHE_SIZE > 0
        if (    (count < VSF_LINUX_CFG_FS_CACHE_THRESHOLD)
            ||  ((priv->cache_size > 0) && (priv->cache_offset < priv->cache_size))) {
            if (NULL == priv->cache_buffer) {
                priv->cache_buffer = vsf_heap_malloc(VSF_LINUX_CFG_FS_CACHE_SIZE);
                if (NULL == priv->cache_buffer) {
                    return -1;
                }
                priv->cache_size = 0;
                priv->cache_pos = VSF_LINUX_CFG_FS_CACHE_SIZE;
            }
            if ((priv->cache_size == 0) || (priv->cache_offset >= priv->cache_size)) {
                priv->cache_pos = vk_file_tell(file);
                vk_file_read(file, priv->cache_buffer, VSF_LINUX_CFG_FS_CACHE_SIZE);
                rsize = (int32_t)vsf_eda_get_return_value();
                if (rsize < 0) {
                    priv->cache_size = 0;
                    return result > 0 ? result : EOF;
                } else if (!rsize) {
                    break;
                } else {
                    priv->cache_size = rsize;
                    priv->cache_offset = 0;
                    rsize = vsf_min(rsize, count);
                }
            } else {
                uint64_t remain_size = priv->cache_size - priv->cache_offset;
                rsize = (int32_t)vsf_min(remain_size, count);
            }
            memcpy(buf, priv->cache_buffer + priv->cache_offset, rsize);
            priv->cache_offset += rsize;
        } else
#endif
        {
            vk_file_read(file, (uint8_t *)buf, count);
            rsize = (int32_t)vsf_eda_get_return_value();
            if (rsize < 0) {
                return result > 0 ? result : EOF;
            } else if (!rsize) {
                break;
            }
        }

        count -= rsize;
        result += rsize;
        buf = (uint8_t *)buf + rsize;
    }
    return result;
}

static ssize_t __vsf_linux_fs_write(vsf_linux_fd_t *sfd, const void *buf, size_t count)
{
    vsf_linux_fs_priv_t *priv = (vsf_linux_fs_priv_t *)sfd->priv;
    vk_file_t *file = priv->file;
    ssize_t result = 0;
    int32_t wsize;

    while (count > 0) {
        vk_file_write(file, (uint8_t *)buf, count);
        wsize = (int32_t)vsf_eda_get_return_value();
        if (wsize < 0) {
            return -1;
        } else if (!wsize) {
            break;
        }

        count -= wsize;
        result += wsize;
        buf = (uint8_t *)buf + wsize;
    }
    return result;
}

static int __vsf_linux_fs_close(vsf_linux_fd_t *sfd)
{
    vsf_linux_fs_priv_t *priv = (vsf_linux_fs_priv_t *)sfd->priv;
    priv->file->attr &= ~VSF_FILE_ATTR_EXCL;
#if VSF_LINUX_CFG_FS_CACHE_SIZE > 0
    if (priv->cache_buffer != NULL) {
        vsf_heap_free(priv->cache_buffer);
        priv->cache_buffer = NULL;
    }
#endif
    __vsf_linux_fs_close_do(priv->file);
    return 0;
}

static int __vsf_linux_fs_eof(vsf_linux_fd_t *sfd)
{
    vsf_linux_fs_priv_t *priv = (vsf_linux_fs_priv_t *)sfd->priv;
    return !(priv->file->size - vk_file_tell(priv->file));
}

static int __vsf_linux_fs_setsize(vsf_linux_fd_t *sfd, off64_t size)
{
    vsf_linux_fs_priv_t *priv = (vsf_linux_fs_priv_t *)sfd->priv;
    vk_file_t *file = priv->file;

    if (file->size != size) {
        vk_file_setsize(file, size);
        return vsf_eda_get_return_value();
    }
    return 0;
}

static int __vsf_linux_fs_stat(vsf_linux_fd_t *sfd, struct stat *buf)
{
    vsf_linux_fs_priv_t *priv = (vsf_linux_fs_priv_t *)sfd->priv;
    vk_file_t *file = priv->file;

    // in VSF, VSF_FILE_ATTR_LNK is a standalone bit, so MUST be checked first
    if (file->attr & VSF_FILE_ATTR_LNK) {
        buf->st_mode = S_IFLNK;
    } else if (file->attr & VSF_FILE_ATTR_DIRECTORY) {
        buf->st_mode = S_IFDIR;
    } else if (file->attr & VSF_FILE_ATTR_CHR) {
        buf->st_mode = S_IFCHR;
    } else if (file->attr & VSF_FILE_ATTR_BLK) {
        buf->st_mode = S_IFBLK;
    } else if (file->attr & VSF_FILE_ATTR_SOCK) {
        buf->st_mode = S_IFSOCK;
    } else {
        buf->st_mode = S_IFREG;
    }
    buf->st_mode |= 0777;
    buf->st_size = file->size;
    return 0;
}

static void * __vsf_linux_fs_mmap(vsf_linux_fd_t *sfd, off64_t offset, size_t len, uint_fast32_t feature)
{
    vsf_linux_fs_priv_t *priv = (vsf_linux_fs_priv_t *)sfd->priv;
    vk_file_t *file = priv->file;

    if (!(file->attr & VSF_FILE_ATTR_DIRECTORY)) {
        if ((feature & PROT_WRITE) && !(file->attr & VSF_FILE_ATTR_WRITE)) {
            return MAP_FAILED;
        }

        if (file->fsop == &vk_vfs_op) {
            vk_vfs_file_t *vfs_file = (vk_vfs_file_t *)file;
            return vfs_file->f.param;
        }
#if VSF_FS_USE_MEMFS == ENABLED
        else if (file->fsop == &vk_vfs_op) {
            vk_memfs_file_t *mem_file = (vk_memfs_file_t *)file;
            return mem_file->f.buff;
        }
#endif
    }
    return MAP_FAILED;
}

// eventfd

int eventfd(int count, int flags)
{
    vsf_linux_fd_t *sfd = NULL;
    if (vsf_linux_fd_create(&sfd, &__vsf_linux_eventfd_fdop) < 0) {
        return -1;
    }

    vsf_linux_eventfd_priv_t *priv = (vsf_linux_eventfd_priv_t *)sfd->priv;
    priv->counter = count;
    priv->flags = flags;
    vsf_linux_fd_clear_status(&priv->use_as__vsf_linux_fd_priv_t, POLLOUT, vsf_protect_sched());
    return sfd->fd;
}

int eventfd_read(int fd, eventfd_t *value)
{
    return read(fd, value, sizeof(eventfd_t)) != sizeof(eventfd_t) ? -1 : 0;
}

int eventfd_write(int fd, eventfd_t value)
{
    return write(fd, &value, sizeof(eventfd_t)) != sizeof(eventfd_t) ? -1 : 0;
}

// eventfd_inc_isr can be called in isr to increase eventfd
void eventfd_inc_isr(vsf_linux_fd_priv_t *eventfd_priv)
{
    vsf_eda_post_evt_msg(vsf_linux_get_kernel_task(), __VSF_EVT_LINUX_EVENTFD_INC, eventfd_priv);
}

void __vsf_linux_eventfd_inc(vsf_linux_fd_priv_t *priv)
{
    vsf_linux_eventfd_priv_t *eventfd_priv = (vsf_linux_eventfd_priv_t *)priv;
    vsf_protect_t orig = vsf_protect_sched();
    eventfd_priv->counter += 1;
    vsf_linux_fd_set_status(&eventfd_priv->use_as__vsf_linux_fd_priv_t, POLLIN, orig);
}

static int __vsf_linux_eventfd_fcntl(vsf_linux_fd_t *sfd, int cmd, uintptr_t arg)
{
    return __vsf_linux_default_fcntl(sfd, cmd, arg);
}

static ssize_t __vsf_linux_eventfd_read(vsf_linux_fd_t *sfd, void *buf, size_t count)
{
    vsf_linux_eventfd_priv_t *priv = (vsf_linux_eventfd_priv_t *)sfd->priv;
    if (count != sizeof(priv->counter)) {
        return -1;
    }

    eventfd_t counter;
    vsf_linux_trigger_t trig;
    vsf_linux_trigger_init(&trig);
    vsf_protect_t orig = vsf_protect_sched();
again:
    if (!priv->counter) {
        if (vsf_linux_fd_is_block(sfd)) {
            if (!vsf_linux_fd_pend_events(&priv->use_as__vsf_linux_fd_priv_t, POLLIN, &trig, orig)) {
                // triggered by signal
                return -1;
            }
            orig = vsf_protect_sched();
        } else {
            vsf_unprotect_sched(orig);
            errno = EAGAIN;
            return -1;
        }
    }
    counter = priv->counter;
    if (!counter) {
        goto again;
    }
    if (priv->flags & EFD_SEMAPHORE) {
        priv->counter--;
    } else {
        priv->counter = 0;
    }
    vsf_linux_fd_clear_status(&priv->use_as__vsf_linux_fd_priv_t, POLLIN, orig);
    put_unaligned_cpu64(counter, buf);
    return sizeof(counter);
}

static ssize_t __vsf_linux_eventfd_write(vsf_linux_fd_t *sfd, const void *buf, size_t count)
{
    vsf_linux_eventfd_priv_t *priv = (vsf_linux_eventfd_priv_t *)sfd->priv;
    if (count != sizeof(priv->counter)) {
        return -1;
    }

    eventfd_t counter = get_unaligned_cpu64((const void *)buf);
    vsf_protect_t orig = vsf_protect_sched();
    priv->counter += counter;
    vsf_linux_fd_set_status(&priv->use_as__vsf_linux_fd_priv_t, POLLIN, orig);
    return sizeof(counter);
}

static int __vsf_linux_eventfd_close(vsf_linux_fd_t *sfd)
{
    return 0;
}

static int __vsf_linux_eventfd_eof(vsf_linux_fd_t *sfd)
{
    vsf_linux_eventfd_priv_t *priv = (vsf_linux_eventfd_priv_t *)sfd->priv;
    eventfd_t counter;
    vsf_protect_t orig = vsf_protect_sched();
        counter = priv->counter;
    vsf_unprotect_sched(orig);
    return !counter;
}

// signalfd

int signalfd(int fd, const sigset_t *mask, int flags)
{
    vsf_linux_fd_t *sfd = NULL;
    if (vsf_linux_fd_create(&sfd, &__vsf_linux_signalfd_fdop) < 0) {
        return -1;
    }

    vsf_linux_signalfd_priv_t *priv = (vsf_linux_signalfd_priv_t *)sfd->priv;
    priv->sigset = (sigset_t *)mask;
    priv->flags = flags;
    vsf_linux_fd_clear_status(&priv->use_as__vsf_linux_fd_priv_t, POLLOUT, vsf_protect_sched());
    return sfd->fd;
}

// epoll

static int __vsf_linux_epollfd_close(vsf_linux_fd_t *sfd)
{
    vsf_linux_epollfd_priv_t *epoll_priv = (vsf_linux_epollfd_priv_t *)sfd->priv;
    __vsf_dlist_foreach_next_unsafe(vsf_linux_epoll_node_t, fd_node, &epoll_priv->fd_list) {
        vsf_linux_fd_release_calback(_->fd_priv, _->events_callback);
        free(_);
    }
    return 0;
}

static vsf_linux_epoll_node_t * __vsf_linux_epoll_get_node(vsf_linux_epollfd_priv_t *epoll_priv, vsf_linux_fd_priv_t *fd_priv)
{
    vsf_linux_epoll_node_t *result = NULL;
    __vsf_dlist_foreach_unsafe(vsf_linux_epoll_node_t, fd_node, &epoll_priv->fd_list) {
        if (_->fd_priv == fd_priv) {
            result = _;
            break;
        }
    }
    return result;
}

// __vsf_linux_epoll_on_events is called scheduler protected
static void __vsf_linux_epoll_on_events(vsf_linux_fd_priv_t *priv, void *param, short events, vsf_protect_t orig)
{
    vsf_linux_epollfd_priv_t *epoll_priv = (vsf_linux_epollfd_priv_t *)param;
    vsf_linux_trigger_t *trig = epoll_priv->trigger;
    vsf_linux_epoll_node_t *node = __vsf_linux_epoll_get_node(epoll_priv, priv);
    VSF_LINUX_ASSERT(node != NULL);

    if (!vsf_dlist_is_in(vsf_linux_epoll_node_t, rdy_node, &epoll_priv->rdy_list, node)) {
        if (events & node->events) {
            vsf_dlist_add_to_tail(vsf_linux_epoll_node_t, rdy_node, &epoll_priv->rdy_list, node);
            epoll_priv->trigger = NULL;
            vsf_unprotect_sched(orig);

            if (trig != NULL) {
                vsf_linux_trigger_signal(trig, 0);
            }
            return;
        }
    }
    vsf_unprotect_sched(orig);
}

int __vsf_linux_epoll_tick(vsf_linux_epollfd_priv_t *epoll_priv, struct epoll_event *events, int maxevents, vsf_timeout_tick_t timeout)
{
    vsf_protect_t orig;
    vsf_linux_trigger_t trig;
    vsf_linux_epoll_node_t *node;
    vsf_linux_fd_priv_t *priv;
    short events_triggered;
    int ret;

    vsf_linux_trigger_init(&trig);
    while (1) {
        ret = 0;
        orig = vsf_protect_sched();
        while (!vsf_dlist_is_empty(&epoll_priv->rdy_list) && (maxevents > ret)) {
            vsf_dlist_remove_head(vsf_linux_epoll_node_t, rdy_node, &epoll_priv->rdy_list, node);
            VSF_LINUX_ASSERT(node != NULL);

            priv = node->fd_priv;
            events_triggered = node->events & priv->events;
            priv->events &= ~(events_triggered & ~priv->sticky_events);
            priv->events |= priv->status & events_triggered;

            events->events = events_triggered;
            events->data = node->data;
            ret++;
        }
        if (ret || (0 == timeout)) {
            vsf_unprotect_sched(orig);
            return ret;
        }

        epoll_priv->trigger = &trig;
        vsf_unprotect_sched(orig);

        int r = vsf_linux_trigger_pend(&trig, timeout);
        // timeout or interrupted by signal
        if (r != 0) {
            return 0;
        }
    }

    return 0;
}

int epoll_create1(int flags)
{
    vsf_linux_fd_t *sfd;
    int fd = vsf_linux_fd_create(&sfd, &__vsf_linux_epollfd_fdop);
    if (fd < 0) {
        return -1;
    }

    if (flags & EPOLL_CLOEXEC) {
        sfd->fd_flags |= FD_CLOEXEC;
    }
    return fd;
}

int epoll_create(int size)
{
    return epoll_create1(0);
}

int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event)
{
    vsf_linux_fd_t *epoll_sfd = vsf_linux_fd_get(epfd);
    VSF_LINUX_ASSERT(epoll_sfd != NULL);
    vsf_linux_epollfd_priv_t *epoll_priv = (vsf_linux_epollfd_priv_t *)epoll_sfd->priv;
    VSF_LINUX_ASSERT(epoll_priv != NULL);
    vsf_linux_epoll_node_t *node;
    vsf_protect_t orig;

    vsf_linux_fd_t *sfd = vsf_linux_fd_get(fd);
    VSF_LINUX_ASSERT(sfd != NULL);
    vsf_linux_fd_priv_t *priv = sfd->priv;
    VSF_LINUX_ASSERT(priv != NULL);
    short events_triggered;

    switch (op) {
    case EPOLL_CTL_ADD:
        node = calloc(1, sizeof(vsf_linux_epoll_node_t));
        if (NULL == node) {
            return -1;
        }

        node->fd_priv = priv;
        node->events = event->events;
        node->data = event->data;

        orig = vsf_protect_sched();
        vsf_dlist_add_to_head(vsf_linux_epoll_node_t, fd_node, &epoll_priv->fd_list, node);
        vsf_linux_fd_priv_callback_t * callback = vsf_linux_fd_claim_calback(priv);
        VSF_LINUX_ASSERT(callback != NULL);
        callback->pendind_events = node->events;
        callback->param = epoll_priv;
        callback->cb = __vsf_linux_epoll_on_events;
        node->events_callback = callback;
        events_triggered = priv->events & node->events;
        if (events_triggered) {
            __vsf_linux_epoll_on_events(priv, epoll_priv, events_triggered, orig);
        } else {
            vsf_unprotect_sched(orig);
        }
        break;
    case EPOLL_CTL_MOD:
        orig = vsf_protect_sched();
        node = __vsf_linux_epoll_get_node(epoll_priv, priv);
        if (NULL == node) {
            vsf_unprotect_sched(orig);
            return -1;
        }
        node->data = event->data;
        node->events = event->events;

        events_triggered = priv->events & node->events;
        if (events_triggered) {
            __vsf_linux_epoll_on_events(priv, epoll_priv, events_triggered, orig);
        } else {
            vsf_unprotect_sched(orig);
        }
        break;
    case EPOLL_CTL_DEL:
        orig = vsf_protect_sched();
        node = __vsf_linux_epoll_get_node(epoll_priv, priv);
        if (NULL == node) {
            vsf_unprotect_sched(orig);
            return -1;
        }

        vsf_dlist_remove(vsf_linux_epoll_node_t, fd_node, &epoll_priv->fd_list, node);
        if (vsf_dlist_is_in(vsf_linux_epoll_node_t, rdy_node, &epoll_priv->rdy_list, node)) {
            vsf_dlist_remove(vsf_linux_epoll_node_t, rdy_node, &epoll_priv->rdy_list, node);
        }
        vsf_linux_fd_release_calback(priv, node->events_callback);
        vsf_unprotect_sched(orig);
        free(node);
        break;
    default:
        return -1;
    }
    return 0;
}

int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout)
{
    vsf_linux_fd_t *epoll_sfd = vsf_linux_fd_get(epfd);
    VSF_LINUX_ASSERT(epoll_sfd != NULL);
    vsf_linux_epollfd_priv_t *epoll_priv = (vsf_linux_epollfd_priv_t *)epoll_sfd->priv;
    VSF_LINUX_ASSERT(epoll_priv != NULL);
    vsf_timeout_tick_t timeout_tick = (timeout < 0) ? -1 : vsf_systimer_ms_to_tick(timeout);
    return __vsf_linux_epoll_tick(epoll_priv, events, maxevents, timeout_tick);
}

int epoll_pwait(int epfd, struct epoll_event *events, int maxevents, int timeout, const sigset_t *sigmask)
{
    sigset_t origmask;
    int result;

    sigprocmask(SIG_SETMASK, sigmask, &origmask);
    result = epoll_wait(epfd, events, maxevents, timeout);
    sigprocmask(SIG_SETMASK, &origmask, NULL);
    return result;
}

int epoll_pwait2(int epfd, struct epoll_event *events, int maxevents, const struct timespec *timeout_ts, const sigset_t *sigmask)
{
    int timeout = (timeout_ts == NULL) ? -1 : (timeout_ts->tv_sec * 1000 + timeout_ts->tv_nsec / 1000000);
    return epoll_pwait(epfd, events, maxevents, timeout, sigmask);
}

vsf_linux_fd_t * __vsf_linux_fd_get_ex(vsf_linux_process_t *process, int fd)
{
    process = vsf_linux_get_real_process(process);
#if VSF_LINUX_CFG_STDIO_FALLBACK == ENABLED
    if (NULL == process) {
        VSF_LINUX_ASSERT(fd <= 2);
        return &__vsf_linux_stdio_fallback[fd];
    }
#else
    VSF_LINUX_ASSERT(process != NULL);
#endif

    vsf_dlist_t *fd_list = &process->fd_list;
    vsf_protect_t orig = vsf_protect_sched();
    __vsf_dlist_foreach_unsafe(vsf_linux_fd_t, fd_node, fd_list) {
        if (_->fd == fd) {
            vsf_unprotect_sched(orig);
            return _;
        }
    }
    vsf_unprotect_sched(orig);
    return NULL;
}

vsf_linux_fd_t * vsf_linux_fd_get(int fd)
{
    return __vsf_linux_fd_get_ex(NULL, fd);
}

vk_file_t * __vsf_linux_get_fs_ex(vsf_linux_process_t *process, int fd)
{
    vsf_linux_fd_t *sfd = __vsf_linux_fd_get_ex(process, fd);
    if ((NULL == sfd) || !(sfd->op->feature & VSF_LINUX_FDOP_FEATURE_FS)) {
        return NULL;
    }

    return ((vsf_linux_fs_priv_t *)sfd->priv)->file;
}

vk_vfs_file_t * __vsf_linux_get_vfs(int fd)
{
    vk_file_t *file = __vsf_linux_get_fs_ex(NULL, fd);
    if (file->fsop != &vk_vfs_op) {
        return NULL;
    }
    return (vk_vfs_file_t *)file;
}

int vsf_linux_fd_get_feature(int fd, uint_fast32_t *feature)
{
    vk_vfs_file_t *vfs_file = __vsf_linux_get_vfs(fd);
    if (vfs_file != NULL) {
        if (feature != NULL) {
            *feature = vfs_file->attr;
        }
        return 0;
    }
    return -1;
}

int vsf_linux_fd_set_feature(int fd, uint_fast32_t feature)
{
    vk_vfs_file_t *vfs_file = __vsf_linux_get_vfs(fd);
    if (vfs_file != NULL) {
        vfs_file->attr = feature;
        return 0;
    }
    return -1;
}

int vsf_linux_fd_add_feature(int fd, uint_fast32_t feature)
{
    uint_fast32_t orig_feature;
    if (vsf_linux_fd_get_feature(fd, &orig_feature) < 0) {
        return -1;
    }
    return vsf_linux_fd_set_feature(fd, orig_feature | feature);
}

int vsf_linux_fd_set_size(int fd, uint64_t size)
{
    vk_vfs_file_t *vfs_file = __vsf_linux_get_vfs(fd);
    if (vfs_file != NULL) {
        vfs_file->size = size;
        return 0;
    }
    return -1;
}

static int __vsf_linux_fd_add_ex(vsf_linux_process_t *process, vsf_linux_fd_t *sfd, int fd_min)
{
    process = vsf_linux_get_real_process(process);
    VSF_LINUX_ASSERT(process != NULL);

    vsf_bitmap(vsf_linux_fd_bitmap) fd_bitmap, *bitmap_ptr = &process->fd_bitmap;
    vsf_protect_t orig = vsf_protect_sched();
        if (fd_min > 0) {
            memcpy(&fd_bitmap, bitmap_ptr, sizeof(fd_bitmap));
            vsf_bitmap_set_range(&fd_bitmap, 0, fd_min - 1);
            bitmap_ptr = &fd_bitmap;
        }
        sfd->fd = vsf_bitmap_ffz(bitmap_ptr, VSF_LINUX_CFG_FD_BITMAP_SIZE);
        VSF_LINUX_ASSERT(sfd->fd >= 0);
        vsf_bitmap_set(&process->fd_bitmap, sfd->fd);
        vsf_dlist_add_to_tail(vsf_linux_fd_t, fd_node, &process->fd_list, sfd);
    vsf_unprotect_sched(orig);

    return sfd->fd;
}

static int __vsf_linux_fd_ref_priv(vsf_linux_fd_priv_t *priv)
{
    vsf_protect_t orig = vsf_protect_sched();
        priv->ref++;
#if VSF_LINUX_CFG_FD_TRACE == ENABLED
        vsf_trace_debug("%s: priv 0x%p ref %d" VSF_TRACE_CFG_LINEEND,
            __FUNCTION__, priv, priv->ref);
#endif
    vsf_unprotect_sched(orig);
    return 0;
}

static int __vsf_linux_fd_deref_priv(vsf_linux_fd_t *sfd)
{
    vsf_linux_fd_priv_t *priv = sfd->priv;
    bool is_to_close;

    vsf_protect_t orig = vsf_protect_sched();
        is_to_close = --priv->ref == 0;
#if VSF_LINUX_CFG_FD_TRACE == ENABLED
        vsf_linux_process_t *process = vsf_linux_get_real_process(NULL);
        vsf_trace_debug("%s: process 0x%p fd %d 0x%p priv 0x%p ref %d" VSF_TRACE_CFG_LINEEND,
            __FUNCTION__, process, sfd->fd, sfd, priv, priv->ref);
#endif
    vsf_unprotect_sched(orig);

    if (sfd->op->fn_fini != NULL) {
        sfd->op->fn_fini(sfd);
    }

    int err = 0;
    if (is_to_close) {
        if (sfd->op->fn_close != NULL) {
            err = sfd->op->fn_close(sfd);
        }
        // priv of fd does not belong to the process
        if (!(priv->flags & __VSF_FILE_ATTR_SHARE_PRIV)) {
            vsf_linux_free_res(priv);
        }
    }
    return err;
}

int __vsf_linux_fd_create_ex(vsf_linux_process_t *process, vsf_linux_fd_t **sfd,
        const vsf_linux_fd_op_t *op, int fd_min, vsf_linux_fd_priv_t *priv)
{
    int priv_size = ((op != NULL) && (op->priv_size > 0)) ? op->priv_size : sizeof(vsf_linux_fd_priv_t);
    int ret;
    vsf_linux_fd_t *new_sfd;

    process = vsf_linux_get_real_process(process);
    VSF_LINUX_ASSERT(process != NULL);

    new_sfd = __calloc_ex(process, 1, sizeof(vsf_linux_fd_t));
    if (!new_sfd) {
        errno = ENOMEM;
        return -1;
    }
    new_sfd->op = op;
    new_sfd->unget_buff = EOF;
    if (NULL == priv) {
        // priv of fd does not belong to the process
        VSF_LINUX_ASSERT(priv_size >= sizeof(vsf_linux_fd_priv_t));
        new_sfd->priv = vsf_linux_malloc_res(priv_size);
        if (!new_sfd->priv) {
            ret = -1;
            goto free_sfd_and_exit;
        }
        memset(new_sfd->priv, 0, priv_size);
        new_sfd->priv->ref = 1;
    } else {
        __vsf_linux_fd_ref_priv(priv);
        new_sfd->priv = priv;
    }

    if (sfd != NULL) {
        *sfd = new_sfd;
    }

    ret = __vsf_linux_fd_add_ex(process, new_sfd, fd_min);
    if (ret < 0) {
free_sfd_and_exit:
        __free_ex(process, new_sfd);
    }
#if VSF_LINUX_CFG_FD_TRACE == ENABLED
    vsf_trace_debug("%s: process 0x%p fd %d 0x%p priv 0x%p ref %d" VSF_TRACE_CFG_LINEEND,
        __FUNCTION__, NULL == process ? vsf_linux_get_cur_process() : process,
        new_sfd->fd, new_sfd, new_sfd->priv, NULL == new_sfd->priv ? 0 : new_sfd->priv->ref);
#endif
    return ret;
}

int vsf_linux_fd_create(vsf_linux_fd_t **sfd, const vsf_linux_fd_op_t *op)
{
    return __vsf_linux_fd_create_ex(NULL, sfd, op, -1, NULL);
}

void ____vsf_linux_fd_delete_ex(vsf_linux_process_t *process, vsf_linux_fd_t *sfd)
{
    process = vsf_linux_get_real_process(process);
    VSF_LINUX_ASSERT(process != NULL);

    vsf_protect_t orig = vsf_protect_sched();
        vsf_dlist_remove(vsf_linux_fd_t, fd_node, &process->fd_list, sfd);
        vsf_bitmap_clear(&process->fd_bitmap, sfd->fd);
    vsf_unprotect_sched(orig);

    __free_ex(process, sfd);
}

void __vsf_linux_fd_delete_ex(vsf_linux_process_t *process, int fd)
{
    process = vsf_linux_get_real_process(process);
    VSF_LINUX_ASSERT(process != NULL);

    vsf_linux_fd_t *sfd = __vsf_linux_fd_get_ex(process, fd);
    VSF_LINUX_ASSERT(sfd != NULL);
    ____vsf_linux_fd_delete_ex(process, sfd);
}

void vsf_linux_fd_delete(int fd)
{
    __vsf_linux_fd_delete_ex(NULL, fd);
}

int vsf_linux_statat(int dirfd, const char *pathname, struct stat *buf, int flags)
{
    int fd;
    if (dirfd < 0) {
        fd = open(pathname, flags);
    } else {
        fd = openat(dirfd, pathname, flags);
    }
    if (fd < 0) {
        errno = ENOENT;
        return -1;
    }

    int result = 0;
    if (buf != NULL) {
        result = fstat(fd, buf);
    }
    close(fd);
    return result;
}

bool vsf_linux_fd_is_block(vsf_linux_fd_t *sfd)
{
    return !(sfd->priv->flags & O_NONBLOCK);
}

static void __vsf_linux_fd_on_events(vsf_linux_fd_priv_t *priv, void *param, short events, vsf_protect_t orig)
{
    struct pollfd *pfd = (struct pollfd *)param;
    vsf_linux_fd_trigger_t *fd_trigger = (vsf_linux_fd_trigger_t *)pfd->trig;
    vsf_linux_trigger_t *trig = fd_trigger->trigger;
    if (trig != NULL) {
        pfd->events_triggered = events & pfd->events;
        priv->events &= ~(pfd->events_triggered & ~priv->sticky_events);
        if (pfd->events_triggered) {
            fd_trigger->trigger = NULL;
            vsf_unprotect_sched(orig);

            vsf_linux_trigger_signal(trig, 0);
            return;
        }
    }
    vsf_unprotect_sched(orig);
}

// vsf_linux_fd_[pend/set/clear]_[status/events] MUST be called scheduler protected
vsf_linux_fd_priv_callback_t * vsf_linux_fd_claim_calback(vsf_linux_fd_priv_t *priv)
{
    vsf_linux_fd_priv_callback_t *callback = NULL;
    if (NULL == priv->events_callback[0].cb) {
        callback = &priv->events_callback[0];
    } else if (NULL == priv->events_callback[1].cb) {
        callback = &priv->events_callback[1];
    }
    if (callback != NULL) {
        callback->cb = (void (*)(vsf_linux_fd_priv_t *priv, void *param, short events, vsf_protect_t orig))1;
    }
    return callback;
}

void vsf_linux_fd_release_calback(vsf_linux_fd_priv_t *priv, vsf_linux_fd_priv_callback_t *callback)
{
    callback->cb = NULL;
}

short vsf_linux_fd_pend_events(vsf_linux_fd_priv_t *priv, short events, vsf_linux_trigger_t *trig, vsf_protect_t orig)
{
    uint16_t events_triggered = priv->events & events;
    if (events_triggered) {
        priv->events &= ~(events_triggered & ~priv->sticky_events);
        vsf_unprotect_sched(orig);
    } else {
        vsf_linux_fd_trigger_t fd_trigger = {
            .trigger    = trig,
        };
        struct pollfd pfd = {
            .events     = events,
            .trig       = &fd_trigger,
        };

        vsf_linux_fd_priv_callback_t * callback = vsf_linux_fd_claim_calback(priv);
        VSF_LINUX_ASSERT(callback != NULL);
        callback->pendind_events = events;
        callback->param = &pfd;
        callback->cb = __vsf_linux_fd_on_events;
        vsf_unprotect_sched(orig);

        int ret = vsf_linux_trigger_pend(trig, -1);
        orig = vsf_protect_sched();
            vsf_linux_fd_release_calback(priv, callback);
        vsf_unprotect_sched(orig);
        if (!ret) {
            events_triggered = pfd.events_triggered;
        }
    }
    return events_triggered;
}

void vsf_linux_fd_set_events(vsf_linux_fd_priv_t *priv, short events, vsf_protect_t orig)
{
    priv->events |= events;
    if (    (priv->events_callback[0].cb != NULL)
        &&  (priv->events_callback[0].pendind_events & priv->events)) {
        priv->events_callback[0].cb(priv, priv->events_callback[0].param, events, orig);
    } else if (
            (priv->events_callback[1].cb != NULL)
        &&  (priv->events_callback[1].pendind_events & priv->events)) {
        priv->events_callback[1].cb(priv, priv->events_callback[1].param, events, orig);
    } else {
        vsf_unprotect_sched(orig);
    }
}

void vsf_linux_fd_set_status(vsf_linux_fd_priv_t *priv, short status, vsf_protect_t orig)
{
    short events_triggered = (priv->status & status) ^ status;
    priv->status |= status;
    if (events_triggered) {
        vsf_linux_fd_set_events(priv, events_triggered, orig);
    } else {
        vsf_unprotect_sched(orig);
    }
}

void vsf_linux_fd_clear_status(vsf_linux_fd_priv_t *priv, short status, vsf_protect_t orig)
{
    priv->events &= ~(status & ~priv->sticky_events);
    priv->status &= ~status;
    vsf_unprotect_sched(orig);
}

short vsf_linux_fd_get_status(vsf_linux_fd_priv_t *priv, short status)
{
    return priv->status & status;
}

int __vsf_linux_poll_tick(struct pollfd *fds, nfds_t nfds, vsf_timeout_tick_t timeout)
{
    vsf_protect_t orig;
    vsf_linux_fd_t *sfd;
    vsf_linux_fd_priv_t *priv;
    int ret = 0;
    nfds_t i;
    vsf_linux_trigger_t trig;
    vsf_linux_fd_trigger_t fd_trigger = {
        .trigger    = &trig,
    };
    short events_triggered;

    vsf_linux_trigger_init(&trig);
    while (1) {
        orig = vsf_protect_sched();
        for (i = 0; i < nfds; i++) {
            sfd = vsf_linux_fd_get(fds[i].fd);
            // fd maybe closed by other thread while being polled
            if (NULL == sfd) {
                fds[i].revents = 0;
                continue;
            }
            priv = sfd->priv;
            VSF_LINUX_ASSERT(priv != NULL);
            events_triggered = priv->events & fds[i].events;
            if (events_triggered) {
                fds[i].revents = events_triggered;
                priv->events &= ~(events_triggered & ~priv->sticky_events);
                priv->events |= priv->status & events_triggered;
                ret++;
            } else {
                fds[i].revents = 0;
            }
        }
        if (ret || (0 == timeout)) {
            vsf_unprotect_sched(orig);
            return ret;
        }

        for (i = 0; i < nfds; i++) {
            sfd = vsf_linux_fd_get(fds[i].fd);
            if (NULL == sfd) {
                continue;
            }
            priv = sfd->priv;
            fds[i].events_triggered = 0;
            fds[i].trig = &fd_trigger;
            fds[i].callback = vsf_linux_fd_claim_calback(priv);
            VSF_LINUX_ASSERT(fds[i].callback != NULL);
            fds[i].callback->pendind_events = fds[i].events;
            fds[i].callback->param = &fds[i];
            fds[i].callback->cb = __vsf_linux_fd_on_events;
        }
        vsf_unprotect_sched(orig);

        int r = vsf_linux_trigger_pend(&trig, timeout);

        for (i = 0; i < nfds; i++) {
            sfd = vsf_linux_fd_get(fds[i].fd);
            // fd maybe closed by other thread while being polled
            if (NULL == sfd) {
                continue;
            }
            priv = sfd->priv;
            VSF_LINUX_ASSERT(priv != NULL);
            orig = vsf_protect_sched();
                vsf_linux_fd_release_calback(priv, fds[i].callback);
                priv->events |= fds[i].events_triggered;
            vsf_unprotect_sched(orig);
        }

        // timeout or interrupted by signal
        if (r != 0) {
            return 0;
        }
    }
    return 0;
}

int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *execeptfds, struct timeval *timeout)
{
    int fd_num = 0;

    VSF_LINUX_ASSERT((nfds >= 0) && (nfds < FD_SETSIZE));

    fd_set mask;
    FD_ZERO(&mask);
    for (int i = 0; i < nfds; i++) {
        if (    ((readfds != NULL) && (FD_ISSET(i, readfds)))
            ||  ((writefds != NULL) && (FD_ISSET(i, writefds)))
            ||  ((execeptfds != NULL) && (FD_ISSET(i, execeptfds)))) {
            FD_SET(i, &mask);
            fd_num++;
        }
    }

    struct pollfd fds[fd_num];
    if (fd_num > 0) {
        memset(fds, 0, fd_num * sizeof(struct pollfd));

        for (int i = 0, idx = 0; i < nfds; i++) {
            if (FD_ISSET(i, &mask)) {
                fds[idx].fd = i;
#if POLLPRI != POLLIN
                if ((readfds != NULL) && (FD_ISSET(i, readfds))) {
                    fds[idx].events |= POLLIN;
                }
                if ((execeptfds != NULL) && (FD_ISSET(i, execeptfds))) {
                    fds[idx].events |= POLLPRI;
                }
#else
                if (    ((readfds != NULL) && (FD_ISSET(i, readfds)))
                    ||  ((execeptfds != NULL) && (FD_ISSET(i, execeptfds)))) {
                    fds[idx].events |= POLLIN;
                }
#endif
                if ((writefds != NULL) && (FD_ISSET(i, writefds))) {
                    fds[idx].events |= POLLOUT;
                }
                idx++;
            }
        }

        if (readfds != NULL) {
            FD_ZERO(readfds);
        }
        if (writefds != NULL) {
            FD_ZERO(writefds);
        }
        if (execeptfds != NULL) {
            FD_ZERO(execeptfds);
        }
    }

    vsf_timeout_tick_t timeout_tick = -1;
    if (timeout != NULL) {
        timeout_tick = vsf_systimer_ms_to_tick(1000 * timeout->tv_sec);
        timeout_tick += vsf_systimer_us_to_tick(timeout->tv_usec);
    }
    int ret = __vsf_linux_poll_tick(fds, fd_num, timeout_tick);
    if (ret > 0) {
        for (int i = 0; i < fd_num; i++) {
            if (fds[i].revents & POLLIN) {
                FD_SET(fds[i].fd, readfds);
            }
            if (fds[i].revents & POLLOUT) {
                FD_SET(fds[i].fd, writefds);
            }
#if POLLPRI != POLLIN
            if (fds[i].revents & POLLPRI) {
                FD_SET(fds[i].fd, execeptfds);
            }
#endif
        }
    }
    return ret;
}

int pselect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, const struct timespec *timeout_ts, const sigset_t *sigmask)
{
    sigset_t origmask;
    struct timeval timeout;
    int ready;

    timeout.tv_sec = timeout_ts->tv_sec;
    timeout.tv_usec = timeout_ts->tv_nsec / 1000;
    sigprocmask(SIG_SETMASK, sigmask, &origmask);
    ready = select(nfds, readfds, writefds, exceptfds, &timeout);
    sigprocmask(SIG_SETMASK, &origmask, NULL);
    return ready;
}

int poll(struct pollfd *fds, nfds_t nfds, int timeout)
{
    vsf_timeout_tick_t timeout_tick = (timeout < 0) ? -1 : vsf_systimer_ms_to_tick(timeout);
    return __vsf_linux_poll_tick(fds, nfds, timeout_tick);
}

int ppoll(struct pollfd *fds, nfds_t nfds, const struct timespec *timeout_ts, const sigset_t *sigmask)
{
    sigset_t origmask;
    int timeout, ready;

    timeout = (timeout_ts == NULL) ? -1 : (timeout_ts->tv_sec * 1000 + timeout_ts->tv_nsec / 1000000);
    sigprocmask(SIG_SETMASK, sigmask, &origmask);
    ready = poll(fds, nfds, timeout);
    sigprocmask(SIG_SETMASK, &origmask, NULL);
    return ready;
}

static int __vsf_linux_fs_create(int dirfd, const char *pathname, mode_t mode, vk_file_attr_t attr)
{
    vk_file_t *dir = dirfd < 0 ? NULL : __vsf_linux_get_fs_ex(NULL, dirfd);
    char fullpath[PATH_MAX], *name_tmp;
    int err = 0;

    if (NULL == dir) {
        if (vsf_linux_generate_path(fullpath, sizeof(fullpath), NULL, (char *)pathname)) {
            return -1;
        }
    } else {
        strcpy(fullpath, pathname);
    }

    if (fullpath[0] == '/') {
        if (fullpath[1] == '\0') {
            errno = EEXIST;
            return -1;
        }
        if (dirfd >= 0) {
            errno = EINVAL;
            return -1;
        }
    }
    size_t fullpath_len = strlen(fullpath);
    if (fullpath[fullpath_len - 1] == '/') {
        fullpath[fullpath_len - 1] = '\0';
    }

    name_tmp = vk_file_getfilename((char *)fullpath);
    if ('\0' == name_tmp[0]) {
        errno = EINVAL;
        return -1;
    }
    char filename[strlen(name_tmp) + 1];
    strcpy(filename, name_tmp);

    fullpath[name_tmp - fullpath] = '\0';
    dir = __vsf_linux_fs_get_file_ex(dir, fullpath);
    if (!dir) {
        return -1;
    }

    vk_file_t *file;
    vk_file_open(dir, filename, &file);
    if (VSF_ERR_NONE == (vsf_err_t)vsf_eda_get_return_value()) {
        errno = EEXIST;
        return -1;
    } else {
        vk_file_create(dir, filename, attr);
        if (VSF_ERR_NONE != (vsf_err_t)vsf_eda_get_return_value()) {
            err = -1;
        }
    }
    __vsf_linux_fs_close_do(dir);
    return err;
}

static int __vsf_linux_fs_remove(const char *pathname, vk_file_attr_t attr)
{
    char fullpath[PATH_MAX];
    if (vsf_linux_generate_path(fullpath, sizeof(fullpath), NULL, (char *)pathname)) {
        return -1;
    }

    vk_file_t *file = __vsf_linux_fs_get_file(fullpath), *dir;
    if (!file) {
        errno = ENOENT;
        return -1;
    }

    // check file attr
    if (attr && !(file->attr & attr)) {
        return -1;
    }

    dir = vk_file_get_parent(file);
    __vsf_linux_fs_close_do(file);

    pathname = vk_file_getfilename((char *)pathname);
    vk_file_unlink(dir, pathname);
    vsf_err_t err = (vsf_err_t)vsf_eda_get_return_value();
    __vsf_linux_fs_close_do(dir);
    return VSF_ERR_NONE == err ? 0 : -1;
}

int __vsf_linux_fs_rename(const char *pathname_old, const char *pathname_new)
{
    char fullpath[PATH_MAX], *name_tmp;
    vk_file_t *olddir, *newdir, *newfile;
    int err = 0;

    if (vsf_linux_generate_path(fullpath, sizeof(fullpath), NULL, (char *)pathname_old)) {
        return -1;
    }
    olddir = __vsf_linux_fs_get_file(fullpath);
    if (NULL == olddir) {
        return -1;
    }
    if (vk_file_get_ref(olddir) > 1) {
        goto do_close_olddir_and_fail;
    }
    __vsf_linux_fs_close_do(olddir);

    name_tmp = vk_file_getfilename((char *)fullpath);
    fullpath[name_tmp - fullpath] = '\0';
    olddir = __vsf_linux_fs_get_file(fullpath);
    if (!olddir) {
        return -1;
    }

    if (    !(olddir->attr & VSF_FILE_ATTR_WRITE)
        ||  vsf_linux_generate_path(fullpath, sizeof(fullpath), NULL, (char *)pathname_new)) {
        goto do_close_olddir_and_fail;
    }
    pathname_old = vk_file_getfilename((char *)pathname_old);
    pathname_new = vk_file_getfilename((char *)pathname_new);

    newfile = __vsf_linux_fs_get_file(fullpath);
    name_tmp = vk_file_getfilename((char *)fullpath);
    fullpath[name_tmp - fullpath] = '\0';
    newdir = __vsf_linux_fs_get_file(fullpath);
    if (newfile != NULL) {
        uint32_t newfile_ref = vk_file_get_ref(newfile);
        __vsf_linux_fs_close_do(newfile);
        if (newfile_ref > 1) {
            goto do_close_newdir_olddir_and_fail;
        }

        vk_file_unlink(newdir, pathname_new);
        vsf_err_t err = (vsf_err_t)vsf_eda_get_return_value();
        if (err != VSF_ERR_NONE) {
            goto do_close_newdir_olddir_and_fail;
        }
    }

    if (!newdir) {
        goto do_close_olddir_and_fail;
    }
    if (!(newdir->attr & VSF_FILE_ATTR_WRITE)) {
        goto do_close_newdir_olddir_and_fail;
    }

    vk_file_rename(olddir, pathname_old, newdir, pathname_new);
    if (VSF_ERR_NONE != (vsf_err_t)vsf_eda_get_return_value()) {
        err = -1;
    }

    __vsf_linux_fs_close_do(olddir);
    __vsf_linux_fs_close_do(newdir);
    return err;

do_close_newdir_olddir_and_fail:
    __vsf_linux_fs_close_do(newdir);
do_close_olddir_and_fail:
    __vsf_linux_fs_close_do(olddir);
    return -1;
}

int mkdirat(int dirfd, const char *pathname, mode_t mode)
{
    if ((NULL == pathname) || ('\0' == *pathname)) {
        return -1;
    }
    return __vsf_linux_fs_create(dirfd, pathname, mode, VSF_FILE_ATTR_DIRECTORY | VSF_FILE_ATTR_READ | VSF_FILE_ATTR_WRITE);
}

int mkdir(const char *pathname, mode_t mode)
{
    return mkdirat(-1, pathname, mode);
}

int mkdirs(const char *pathname, mode_t mode)
{
    int fd = open(pathname, O_CREAT | O_DIRECTORY, mode);
    if (fd < 0) {
        return -1;
    }
    close(fd);
    return 0;
}

int rmdir(const char *pathname)
{
    if ((NULL == pathname) || ('\0' == *pathname)) {
        return -1;
    }
    return __vsf_linux_fs_remove(pathname, VSF_FILE_ATTR_DIRECTORY);
}

int dup(int oldfd)
{
    return fcntl(oldfd, F_DUPFD, -1);
}

int dup2(int oldfd, int newfd)
{
    return fcntl(oldfd, F_DUPFD, newfd);
}

int dup3(int oldfd, int newfd, int flags)
{
    int res = dup2(oldfd, newfd);
    if (0 == res) {
        fcntl(newfd, F_SETFD, flags);
    }
    return res;
}

int vsf_linux_chdir(vsf_linux_process_t *process, char *pathname)
{
    VSF_LINUX_ASSERT(process != NULL);
    process = vsf_linux_get_real_process(process);

    char fullpath[PATH_MAX];
    if (vsf_linux_generate_path(fullpath, sizeof(fullpath), NULL, (char *)pathname)) {
        return -1;
    }

    int len = strlen(fullpath);
    if (fullpath[len - 1] != '/') {
        VSF_LINUX_ASSERT(len + 1 < PATH_MAX);
        fullpath[len] = '/';
        fullpath[len + 1] = '\0';
    }

    vk_file_t *file = __vsf_linux_fs_get_file(fullpath);
    if (NULL == file) {
        return -1;
    }
    vk_file_attr_t attr = file->attr;
    __vsf_linux_fs_close_do(file);
    if (!(attr & VSF_FILE_ATTR_DIRECTORY)) {
        return -1;
    }

    __free_ex(process, process->working_dir);
    process->working_dir = __malloc_ex(process, strlen(fullpath) + 1);
    if (process->working_dir != NULL) {
        strcpy(process->working_dir, fullpath);
        return 0;
    }
    VSF_LINUX_ASSERT(process->working_dir != NULL);
    return -1;
}

int chroot(const char *path)
{
    return 0;
}

int chdir(const char *pathname)
{
    if ((NULL == pathname) || ('\0' == *pathname)) {
        return -1;
    }
    vsf_linux_process_t *process = vsf_linux_get_cur_process();
    VSF_LINUX_ASSERT(process != NULL);
    return vsf_linux_chdir(process, (char *)pathname);
}

int fchdir(int fd)
{
    vk_file_t *file = __vsf_linux_get_fs_ex(NULL, fd);
    if (NULL == file) {
        return -1;
    }

    char pathname[PATH_MAX], *pos = pathname + PATH_MAX - 1;
    size_t namelen;
    pos[0] = '\0';
    while ((file != NULL) && (file->name != NULL)) {
        namelen = strlen(file->name);
        pos -= namelen + 1;
        if (pos < pathname) {
            __vsf_linux_fs_close_do(file);
            return -1;
        }

        pos[0] = '/';
        memcpy(pos + 1, file->name, namelen);
        file = file->parent;
    }
    __vsf_linux_fs_close_do(file);
    return chdir((const char *)pos);
}

int creat(const char *pathname, mode_t mode)
{
    return open(pathname, O_WRONLY | O_CREAT | O_TRUNC, mode);
}

int vsf_linux_open(int dirfd, const char *pathname, int flags, mode_t mode)
{
    vk_file_t *dir = dirfd < 0 ? NULL : __vsf_linux_get_fs_ex(NULL, dirfd);
    vk_file_t *file;
    vk_vfs_file_t *vfs_file = NULL;
    vsf_linux_fd_t *sfd;
    vsf_linux_fd_op_t *fdop;
    char fullpath[PATH_MAX], *dir_path = NULL;
#if VSF_LINUX_CFG_LINK_FILE == ENABLED
    char linkpath[PATH_MAX];
#endif
    int fd;

    if ((NULL == pathname) || ('\0' == *pathname)) {
        return -1;
    }
#if VSF_LINUX_CFG_LINK_FILE == ENABLED
__open_again:
#endif
    if (NULL == dir) {
        if (vsf_linux_generate_path(fullpath, sizeof(fullpath), dir_path, (char *)pathname)) {
            return -1;
        }
    } else {
        // TODO: support relative path
        strcpy(fullpath, pathname);
    }

__open_again_do:
    file = __vsf_linux_fs_get_file_ex(dir, fullpath);
    if (!file) {
        if (flags & O_CREAT) {
            char *path_in_ram = strdup(pathname);
            if (NULL == path_in_ram) {
                return -1;
            }

            struct stat statbuf;
            for (   char *tmp = path_in_ram + (*pathname == '/' ? 1 : 0), *end;
                    (end = strchr(tmp, '/')) != NULL;
                    tmp = end + 1) {
                if (*end == '\0') {
                    break;
                }
                *end = '\0';
                if (    (vsf_linux_statat(dirfd, path_in_ram, &statbuf, 0) < 0)
                    &&  (mkdirat(dirfd, path_in_ram, mode) < 0)) {
                    goto __exit_failure;
                }
                *end = '/';
            }
            int create_flags =  VSF_FILE_ATTR_READ | VSF_FILE_ATTR_WRITE
                        |   ((flags & O_DIRECTORY) ? VSF_FILE_ATTR_DIRECTORY : 0);
            if (__vsf_linux_fs_create(dirfd, path_in_ram, mode, create_flags) < 0) {
                goto __exit_failure;
            }
            flags &= ~O_CREAT;
            free(path_in_ram);
            goto __open_again_do;
        __exit_failure:
            free(path_in_ram);
            return -1;
        }
        errno = ENOENT;
        return -1;
    } else if ((flags & (O_EXCL | O_CREAT)) == (O_EXCL | O_CREAT)) {
        // O_EXECL | O_CREAT MUST be for non-existing files
        errno = EEXIST;
        return -1;
    } else if (file->attr & VSF_FILE_ATTR_EXCL) {
        errno = EBUSY;
        return -1;
    }

    if (    (file->fsop == &vk_vfs_op)
        &&  !(file->attr & VSF_FILE_ATTR_DIRECTORY)
        &&  ((vfs_file = (vk_vfs_file_t *)file)->f.op != NULL)) {
        fdop = (vsf_linux_fd_op_t *)vfs_file->f.op;
    } else {
        fdop = (vsf_linux_fd_op_t *)&__vsf_linux_fs_fdop;
        // check not supported flags here
        if (flags & (O_NONBLOCK)) {
            VSF_LINUX_ASSERT(false);
            return -1;
        }
    }

    if ((vfs_file != NULL) && (file->attr & __VSF_FILE_ATTR_SHARE_PRIV)) {
        fd = __vsf_linux_fd_create_ex(NULL, &sfd, (const vsf_linux_fd_op_t *)fdop, -1, vfs_file->f.param);
    } else {
        fd = vsf_linux_fd_create(&sfd, (const vsf_linux_fd_op_t *)fdop);
    }
    if (fd < 0) {
        __vsf_linux_fs_close_do(file);
    } else {
        vsf_linux_fs_priv_t *priv = (vsf_linux_fs_priv_t *)sfd->priv;
        priv->flags = flags;
        priv->file = file;

        if (sfd->op->fn_init != NULL) {
            sfd->op->fn_init(sfd);
        }

        if ((flags & O_DIRECTORY) && !(file->attr & VSF_FILE_ATTR_DIRECTORY)) {
            close(fd);
            errno = ENOTDIR;
            return -1;
        }
        if (flags & O_APPEND) {
            vk_file_seek(file, 0, VSF_FILE_SEEK_END);
        }
        if (flags & O_EXCL) {
            file->attr |= VSF_FILE_ATTR_EXCL;
        }
        if (flags & O_CLOEXEC) {
            sfd->fd_flags |= FD_CLOEXEC;
        }
        if (flags & (1 << __FD_OPENBYLINK)) {
            sfd->fd_flags |= 1 << __FD_OPENBYLINK;
        }
        if ((flags & O_TRUNC) && (flags & (O_RDWR | O_WRONLY))) {
            ftruncate(fd, 0);
        }
        file->pos = 0;
    }

#if VSF_LINUX_CFG_LINK_FILE == ENABLED
    if (!(flags & O_NOFOLLOW)) {
        struct stat s;
        if (!fstat(fd, &s) && S_ISLNK(s.st_mode)) {
            ssize_t linklen = read(fd, linkpath, sizeof(linkpath));
            close(fd);
            if (linklen < 0) {
                return -1;
            }
            linkpath[linklen] = '\0';

            if (linkpath[0] == '/') {
                pathname = linkpath;
                dir_path = NULL;
            } else {
                char *filename = (char *)strrchr((const char *)fullpath, '/');
                VSF_LINUX_ASSERT(filename != NULL);
                filename[0] = '\0';

                pathname = linkpath;
                dir_path = fullpath;
            }
            close(fd);

            flags |= 1 << __FD_OPENBYLINK;
            vfs_file = NULL;
            goto __open_again;
        }
    }
#endif
    return fd;
}

int __open_va(const char *pathname, int flags, va_list ap)
{
    mode_t mode = va_arg(ap, mode_t);
    return vsf_linux_open(-1, pathname, flags, mode);
}

int open(const char *pathname, int flags, ...)
{
    int ret;

    va_list ap;
    va_start(ap, flags);
        ret = __open_va(pathname, flags, ap);
    va_end(ap);
    return ret;
}

int __openat_va(int dirfd, const char *pathname, int flags, va_list ap)
{
    mode_t mode = va_arg(ap, mode_t);
    return vsf_linux_open(dirfd, pathname, flags, mode);
}

int openat(int dirfd, const char *pathname, int flags, ...)
{
    int ret;

    va_list ap;
    va_start(ap, flags);
        ret = __openat_va(dirfd, pathname, flags, ap);
    va_end(ap);
    return ret;
}

int ____vsf_linux_fd_close_ex(vsf_linux_process_t *process, vsf_linux_fd_t *sfd)
{
    VSF_LINUX_ASSERT(sfd != NULL);
    int err = __vsf_linux_fd_deref_priv(sfd);
    ____vsf_linux_fd_delete_ex(process, sfd);
    return err;
}

int __vsf_linux_fd_close_ex(vsf_linux_process_t *process, int fd)
{
    vsf_linux_fd_t *sfd = __vsf_linux_fd_get_ex(process, fd);
    if (!sfd) { return -1; }
    return ____vsf_linux_fd_close_ex(process, sfd);
}

int close(int fd)
{
    return __vsf_linux_fd_close_ex(NULL, fd);
}

int __fcntl_va(int fd, int cmd, va_list ap)
{
    vsf_linux_fd_t *sfd = vsf_linux_fd_get(fd);
    int result = -1;
    uintptr_t arg;

    if (!sfd) { return -1; }
    arg = va_arg(ap, uintptr_t);

    // process generic commands
    switch (cmd) {
    case F_DUPFD: {
            vsf_linux_fd_t *sfd_target = vsf_linux_fd_get((int)arg);
            if (sfd_target != NULL) {
                // do not close original fd, because maybe the original fd is used as FILE
                __vsf_linux_fd_deref_priv(sfd_target);
                __vsf_linux_fd_ref_priv(sfd->priv);

                vsf_dlist_node_t node;
                vsf_protect_t orig = vsf_protect_sched();
                    node = sfd_target->fd_node;
                    *sfd_target = *sfd;
                    sfd_target->fd = (int)arg;
                    sfd_target->fd_node = node;
                vsf_unprotect_sched(orig);
                return sfd_target->fd;
            } else {
                return __vsf_linux_fd_create_ex(NULL, NULL, sfd->op, arg, sfd->priv);
            }
        }
    case F_GETFD:
        return sfd->fd_flags;
        break;
    case F_SETFD: {
            long tmp_arg = arg;
            arg = arg ^ sfd->fd_flags;
            sfd->fd_flags |= tmp_arg;
            result = 0;
        }
        break;
    case F_GETFL:
        return sfd->priv->flags;
        break;
    case F_SETFL: {
            long tmp_arg = arg;
            arg = arg ^ sfd->priv->flags;
            sfd->priv->flags = tmp_arg;
            result = 0;
        }
        break;
    }

    return NULL == sfd->op->fn_fcntl ? result : sfd->op->fn_fcntl(sfd, cmd, arg);
}

int fcntl(int fd, int cmd, ...)
{
    int ret;

    va_list ap;
    va_start(ap, cmd);
        ret = __fcntl_va(fd, cmd, ap);
    va_end(ap);
    return ret;
}

int ioctl(int fd, unsigned long request, ...)
{
    int ret;

    va_list ap;
    va_start(ap, request);
        ret = __fcntl_va(fd, request, ap);
    va_end(ap);
    return ret;
}

ssize_t read(int fd, void *buf, size_t count)
{
    vsf_linux_fd_t *sfd = vsf_linux_fd_get(fd);
    if (!sfd || (sfd->priv->flags & O_WRONLY)) { return -1; }
    sfd->cur_rdflags = 0;
    return sfd->op->fn_read(sfd, buf, count);
}

ssize_t readv(int fd, const struct iovec *iov, int iovcnt)
{
    ssize_t size = 0, cur_size;
    for (int i = 0; i < iovcnt; i++) {
        cur_size = read(fd, iov->iov_base, iov->iov_len);
        if (cur_size > 0) {
            size += cur_size;
        }
        if (cur_size != iov->iov_len) {
            return 0 == size ? cur_size : size;
        }
        iov++;
    }
    return size;
}

ssize_t write(int fd, const void *buf, size_t count)
{
    vsf_linux_fd_t *sfd = vsf_linux_fd_get(fd);
    if (!sfd || (sfd->priv->flags & O_RDONLY)) { return -1; }
    sfd->cur_wrflags = 0;
    return sfd->op->fn_write(sfd, buf, count);
}

ssize_t writev(int fd, const struct iovec *iov, int iovcnt)
{
    ssize_t size = 0, cur_size;
    for (int i = 0; i < iovcnt; i++) {
        cur_size = write(fd, iov->iov_base, iov->iov_len);
        if (cur_size > 0) {
            size += cur_size;
        }
        if (cur_size != iov->iov_len) {
            return 0 == size ? cur_size : size;
        }
        iov++;
    }
    return size;
}

ssize_t pread64(int fd, void *buf, size_t count, off64_t offset)
{
    off_t orig = lseek(fd, 0, SEEK_CUR);
    lseek64(fd, offset, SEEK_SET);
    ssize_t size = read(fd, buf, count);
    lseek64(fd, orig, SEEK_SET);
    return size;
}

ssize_t pread(int fd, void *buf, size_t count, off_t offset)
{
    return pread64(fd, buf, count, (off64_t)offset);
}

ssize_t preadv64(int fd, const struct iovec *iov, int iovcnt, off64_t offset)
{
    ssize_t size = 0, cur_size;
    for (int i = 0; i < iovcnt; i++) {
        cur_size = pread64(fd, iov->iov_base, iov->iov_len, offset);
        if (cur_size > 0) {
            size += cur_size;
            offset += cur_size;
        }
        if (cur_size != iov->iov_len) {
            return 0 == size ? cur_size : size;
        }
        iov++;
    }
    return size;
}

ssize_t preadv(int fd, const struct iovec *iov, int iovcnt, off_t offset)
{
    return preadv64(fd, iov, iovcnt, (off64_t)offset);
}

ssize_t pwrite64(int fd, const void *buf, size_t count, off64_t offset)
{
    off_t orig = lseek(fd, 0, SEEK_CUR);
    lseek64(fd, offset, SEEK_SET);
    ssize_t size = write(fd, buf, count);
    lseek64(fd, orig, SEEK_SET);
    return size;
}

ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset)
{
    return pwrite64(fd, buf, count, (off64_t)offset);
}

ssize_t pwritev64(int fd, const struct iovec *iov, int iovcnt, off64_t offset)
{
    ssize_t size = 0, cur_size;
    for (int i = 0; i < iovcnt; i++) {
        cur_size = pwrite64(fd, iov->iov_base, iov->iov_len, offset);
        if (cur_size > 0) {
            size += cur_size;
            offset += cur_size;
        }
        if (cur_size != iov->iov_len) {
            return 0 == size ? cur_size : size;
        }
        iov++;
    }
    return size;
}

ssize_t pwritev(int fd, const struct iovec *iov, int iovcnt, off_t offset)
{
    return pwritev64(fd, iov, iovcnt, (off64_t)offset);
}

ssize_t sendfile(int out_fd, int in_fd, off_t *offset, size_t count)
{
    char buffer[256];
    ssize_t cur_size, total_size = 0;

    if (offset != NULL) {
        lseek(in_fd, *offset, SEEK_SET);
    }
    while (count > 0) {
        cur_size = vsf_min(count, sizeof(buffer));
        cur_size = read(in_fd, buffer, cur_size);
        if (cur_size <= 0) {
            break;
        }
        total_size += cur_size;
        if (offset != NULL) {
            *offset  += cur_size;
        }
        count -= cur_size;
        write(out_fd, buffer, cur_size);
    }
    return total_size;
}

off64_t lseek64(int fd, off64_t offset, int whence)
{
    vsf_linux_fd_t *sfd = vsf_linux_fd_get(fd);
    if ((NULL == sfd) || (sfd->op != &__vsf_linux_fs_fdop)) {
        return (off64_t)-1;
    }

    vsf_linux_fs_priv_t *priv = (vsf_linux_fs_priv_t *)sfd->priv;
#if VSF_LINUX_CFG_FS_CACHE_SIZE > 0
    if (    (whence == SEEK_CUR)
        &&  (priv->cache_size > 0)
        &&  (priv->cache_offset < priv->cache_size)
        &&  (offset + priv->cache_offset >= 0)
        &&  (offset + priv->cache_offset < priv->cache_size)) {
        priv->cache_offset += offset;
        return priv->cache_pos + priv->cache_offset;
    }
#endif
    vk_file_seek(priv->file, offset, whence);
#if VSF_LINUX_CFG_FS_CACHE_SIZE > 0
    priv->cache_size = 0;
#endif
    return vk_file_tell(priv->file);
}

off_t lseek(int fd, off_t offset, int whence)
{
    return (off_t)lseek64(fd, (off64_t)offset, whence);
}

int futimes(int fd, const struct timeval tv[2])
{
    return 0;
}

int utimes(const char *filename, const struct timeval times[2])
{
    return 0;
}

void sync(void)
{
}

int syncfs(int fd)
{
    return 0;
}

int fsync(int fd)
{
    return 0;
}

int fdatasync(int fd)
{
    return 0;
}

int ftruncate(int fd, off_t length)
{
    vsf_linux_fd_t *sfd = vsf_linux_fd_get(fd);
    if (NULL == sfd->op->fn_setsize) {
        return -ENOTSUP;
    }

    return sfd->op->fn_setsize(sfd, length);
}

int truncate(const char *path, off_t length)
{
    char fullpath[PATH_MAX];
    if (vsf_linux_generate_path(fullpath, sizeof(fullpath), NULL, (char *)path)) {
        return -1;
    }

    int fd = open(fullpath, O_WRONLY);
    if (fd < 0) {
        return -1;
    }

    int ret = ftruncate(fd, length);
    close(fd);
    return ret;
}

int ftruncate64(int fd, off64_t length)
{
    vsf_linux_fd_t *sfd = vsf_linux_fd_get(fd);
    if (NULL == sfd->op->fn_setsize) {
        return -ENOTSUP;
    }

    return sfd->op->fn_setsize(sfd, length);
}

int truncate64(const char *path, off64_t length)
{
    char fullpath[PATH_MAX];
    if (vsf_linux_generate_path(fullpath, sizeof(fullpath), NULL, (char *)path)) {
        return -1;
    }

    int fd = open(fullpath, O_WRONLY);
    if (fd < 0) {
        return -1;
    }

    int ret = ftruncate64(fd, length);
    close(fd);
    return ret;
}

int fstat(int fd, struct stat *buf)
{
    vsf_linux_fd_t *sfd = vsf_linux_fd_get(fd);
    if (NULL == sfd->op->fn_stat) {
        return -1;
    }

    memset(buf, 0, sizeof(*buf));
    return sfd->op->fn_stat(sfd, buf);
}

int stat(const char *pathname, struct stat *buf)
{
    return vsf_linux_statat(-1, pathname, buf, 0);
}

int lstat(const char *pathname, struct stat *buf)
{
    return vsf_linux_statat(-1, pathname, buf, O_NOFOLLOW);
}

int fstatat(int dirfd, const char *pathname, struct stat *buf, int flags)
{
    if ((NULL == pathname) || ('\0' == *pathname)) {
        return -1;
    }

    return vsf_linux_statat(dirfd, pathname, buf, 0);
}

int futimens(int fd, const struct timespec times[2])
{
    return 0;
}

int utimensat(int dirfd, const char *pathname, const struct timespec times[2], int flags)
{
    return vsf_linux_statat(dirfd, pathname, NULL, 0);
}

int chmod(const char *pathname, mode_t mode)
{
    return vsf_linux_statat(-1, pathname, NULL, 0);
}

int fchmod(int fd, mode_t mode)
{
    return 0;
}

mode_t umask(mode_t mask)
{
    return 0777;
}

int access(const char *pathname, int mode)
{
    int fd = open(pathname, 0);
    if (fd < 0) { return -1; }

    int ret = 0;
    vk_file_t *file = __vsf_linux_get_fs_ex(NULL, fd);
    if (    ((mode & R_OK) && !(file->attr & VSF_FILE_ATTR_READ))
        ||  ((mode & W_OK) && !(file->attr & VSF_FILE_ATTR_WRITE))
        ||  (   (mode & X_OK)
            &&  !(file->attr & VSF_FILE_ATTR_DIRECTORY)
            &&  !(file->attr & VSF_FILE_ATTR_EXECUTE))) {
        ret = -1;
    }

    close(fd);
    return ret;
}

int link(const char *oldpath, const char *newpath)
{
    return -1;
}

int unlink(const char *pathname)
{
    return __vsf_linux_fs_remove(pathname, 0);
}

int unlinkat(int dirfd, const char *pathname, int flags)
{
    vk_file_t *dir = __vsf_linux_get_fs_ex(NULL, dirfd);
    if ((NULL == dir) || !(dir->attr & VSF_FILE_ATTR_DIRECTORY)) {
        errno = ENOTDIR;
        return -1;
    }

    vk_file_unlink(dir, pathname);
    vsf_err_t err = (vsf_err_t)vsf_eda_get_return_value();
    return VSF_ERR_NONE == err ? 0 : -1;
}

int symlink(const char *target, const char *linkpath)
{
#if VSF_LINUX_CFG_LINK_FILE == ENABLED
    vk_file_attr_t attr = VSF_FILE_ATTR_LNK | VSF_FILE_ATTR_READ | VSF_FILE_ATTR_WRITE;

    struct stat s;
    if (stat(target, &s) < 0) {
        return -1;
    }

    switch (s.st_mode & S_IFMT) {
    // link to a directory is not currently supported
    case S_IFDIR:       attr |= VSF_FILE_ATTR_DIRECTORY;    return -1;
    case S_IFLNK:
    case S_IFREG:                                           break;
    case S_IFIFO:       attr |= VSF_FILE_ATTR_FIFO;         break;
    case S_IFCHR:       attr |= VSF_FILE_ATTR_CHR;          break;
    case S_IFBLK:       attr |= VSF_FILE_ATTR_BLK;          break;
    case S_IFSOCK:      attr |= VSF_FILE_ATTR_SOCK;         break;
    }

    if (__vsf_linux_fs_create(-1, linkpath, 0, attr) < 0) {
        return -1;
    }

    int fd = open(linkpath, O_NOFOLLOW);
    if (fd < 0) {
        return -1;
    }

    size_t targetlen = strlen(target);
    ssize_t res = write(fd, target, targetlen);
    if (res != targetlen) {
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
#else
    return -1;
#endif
}

ssize_t readlink(const char *pathname, char *buf, size_t bufsiz)
{
#if VSF_LINUX_CFG_LINK_FILE == ENABLED
    int fd = open(pathname, O_NOFOLLOW);
    if (fd < 0) {
        return -1;
    }

    ssize_t res = read(fd, buf, bufsiz);
    close(fd);
    return res;
#else
    return -1;
#endif
}

DIR *fdopendir(int fd)
{
    vsf_linux_fd_t *sfd = vsf_linux_fd_get(fd);
    vsf_linux_fs_priv_t *priv = (vsf_linux_fs_priv_t *)sfd->priv;
    if (!(priv->file->attr & VSF_FILE_ATTR_DIRECTORY)) {
        close(fd);
        sfd = NULL;
    }
    return sfd;
}

DIR * opendir(const char *name)
{
    int fd = open(name, O_DIRECTORY);
    if (fd < 0) {
        return NULL;
    }

    return fdopendir(fd);
}

struct dirent64 * readdir64(DIR *dir)
{
    vsf_linux_fs_priv_t *priv = (vsf_linux_fs_priv_t *)dir->priv;
    vk_file_t *file = priv->file, *child;
    off_t offset = vk_file_tell(file);

    vk_file_open(file, NULL, &child);
    if (NULL == child) {
        return NULL;
    }
    priv->dir64.d_ino++;

    strncpy(priv->dir64.d_name, child->name, sizeof(priv->dir.d_name));
    priv->dir64.d_off = offset;
    priv->dir64.d_reclen = sizeof(struct dirent);
    priv->dir64.d_type = child->attr & VSF_FILE_ATTR_DIRECTORY ? DT_DIR :
                child->attr & VSF_FILE_ATTR_EXECUTE ? DT_EXE : DT_REG;
    vk_file_close(child);
    return &priv->dir64;
}

struct dirent * readdir(DIR *dir)
{
    vsf_linux_fs_priv_t *priv = (vsf_linux_fs_priv_t *)dir->priv;
    vk_file_t *file = priv->file, *child;
    off_t offset = vk_file_tell(file);

    vk_file_open(file, NULL, &child);
    if (NULL == child) {
        return NULL;
    }
    priv->dir.d_ino++;

    strncpy(priv->dir.d_name, child->name, sizeof(priv->dir.d_name));
    priv->dir.d_off = offset;
    priv->dir.d_reclen = sizeof(struct dirent);
    priv->dir.d_type = child->attr & VSF_FILE_ATTR_DIRECTORY ? DT_DIR :
                child->attr & VSF_FILE_ATTR_EXECUTE ? DT_EXE : DT_REG;
    vk_file_close(child);
    return &priv->dir;
}

int readdir_r(DIR *dirp, struct dirent *entry, struct dirent **result)
{
    *result = readdir(dirp);
    return 0;
}

int readdir64_r(DIR *dirp, struct dirent64 *entry, struct dirent64 **result)
{
    *result = readdir64(dirp);
    return 0;
}

int scandir(const char *dir, struct dirent ***namelist,
              int (*filter)(const struct dirent *),
              int (*compare)(const struct dirent **, const struct dirent **))
{
    DIR *dp = opendir(dir);
    if (NULL == dp) {
        return -1;
    }

    int cnt = 0;
    struct dirent *d, *n, **result = NULL;
    while ((d = readdir(dp)) != NULL) {
        if ((filter != NULL) && !filter(d)) {
            continue;
        }

        result = realloc(result, (++cnt) * sizeof(struct dirent *));
        if (NULL == result) {
            return -1;
        }

        n = malloc(sizeof(struct dirent));
        if (NULL == n) {
            break;
        }
        *n = *d;

        result[cnt - 1] = n;
    }

    if ((cnt > 0) && (compare != NULL)) {
        qsort(result, cnt, sizeof(struct dirent *),
                (int (*)(const void *, const void*))compare);
    }
    *namelist = result;
    return cnt;
}

int alphasort(const struct dirent **a, const struct dirent **b)
{
    return strcmp((*a)->d_name, (*b)->d_name);
}

int versionsort(const struct dirent **a, const struct dirent **b)
{
    return strverscmp((*a)->d_name, (*b)->d_name);
}

int scandir64(const char *dir, struct dirent64 ***namelist,
              int (*filter)(const struct dirent64 *),
              int (*compare)(const struct dirent64 **, const struct dirent64 **))
{
    DIR *dp = opendir(dir);
    if (NULL == dp) {
        return -1;
    }

    int cnt = 0;
    struct dirent64 *d, *n, **result = NULL;
    while ((d = readdir64(dp)) != NULL) {
        if ((filter != NULL) && !filter(d)) {
            continue;
        }

        result = realloc(result, (++cnt) * sizeof(struct dirent *));
        if (NULL == result) {
            return -1;
        }

        n = malloc(sizeof(struct dirent));
        if (NULL == n) {
            break;
        }
        *n = *d;

        result[cnt - 1] = n;
    }

    if ((cnt > 0) && (compare != NULL)) {
        qsort(result, cnt, sizeof(struct dirent *),
                (int (*)(const void *, const void*))compare);
    }
    *namelist = result;
    return cnt;
}

int alphasort64(const struct dirent64 **a, const struct dirent64 **b)
{
    return strcmp((*a)->d_name, (*b)->d_name);
}

int versionsort64(const struct dirent64 **a, const struct dirent64 **b)
{
    return strverscmp((*a)->d_name, (*b)->d_name);
}

long telldir(DIR *dir)
{
    vsf_linux_fs_priv_t *priv = (vsf_linux_fs_priv_t *)dir->priv;
    vk_file_t *file = priv->file;
    vk_file_seek(file, 0, VSF_FILE_SEEK_CUR);
    return vk_file_tell(file);
}

void seekdir(DIR *dir, long loc)
{
    vsf_linux_fs_priv_t *priv = (vsf_linux_fs_priv_t *)dir->priv;
    vk_file_t *file = priv->file;
    vk_file_seek(file, loc, VSF_FILE_SEEK_SET);
}

void rewinddir(DIR *dir)
{
    seekdir(dir, 0);
}

int closedir(DIR *dir)
{
    return close(dir->fd);
}

int mount(const char *source, const char *target,
    const vk_fs_op_t *filesystem, unsigned long mountflags, const void *data)
{
    int fd = open(target, 0);
    if (fd < 0) { return fd; }

    vsf_linux_fd_t *sfd = vsf_linux_fd_get(fd);
    vsf_linux_fs_priv_t *priv = (vsf_linux_fs_priv_t *)sfd->priv;
    vk_file_t *dir = priv->file;
    vsf_err_t err;

    if (filesystem != NULL) {
        vk_fs_mount(dir, filesystem, (void *)data);
        err = (vsf_err_t)vsf_eda_get_return_value();
    } else {
#if VSF_FS_USE_MALFS == ENABLED
        vsf_linux_fsdata_auto_t *fsdata = (vsf_linux_fsdata_auto_t *)data;
        vk_malfs_mounter_t mounter = {
            .err    = VSF_ERR_NONE,
        };
        mounter.dir = dir;
        mounter.mal = fsdata->mal;
        mounter.mutex = &fsdata->mutex;
        vk_malfs_mount_mbr(&mounter);
        err = mounter.err;
#else
        err = VSF_ERR_NOT_SUPPORT;
#endif
    }

    close(fd);
    if (VSF_ERR_NONE != err) {
        return -1;
    }
    return 0;
}

int umount(const char *target)
{
    int fd = open(target, 0);
    if (fd < 0) { return fd; }

    vsf_linux_fd_t *sfd = vsf_linux_fd_get(fd);
    vsf_linux_fs_priv_t *priv = (vsf_linux_fs_priv_t *)sfd->priv;
    vk_file_t *dir = priv->file;
    vk_fs_unmount(dir);
    close(fd);
    if (VSF_ERR_NONE != (vsf_err_t)vsf_eda_get_return_value()) {
        return -1;
    }
    return 0;
}

int umount2(const char *target, int flags)
{
    // TODO: process flags
    return umount(target);
}

int statfs(const char *path, struct statfs *buf)
{
    VSF_LINUX_ASSERT(false);
    return -1;
}

int fstatfs(int fd, struct statfs *buf)
{
    VSF_LINUX_ASSERT(false);
    return -1;
}

int fstatvfs(int fd, struct statvfs *buf)
{
    VSF_LINUX_ASSERT(false);
    return -1;
}

int statvfs(const char *path, struct statvfs *buf)
{
    VSF_LINUX_ASSERT(false);
    return -1;
}

int fstatvfs64(int fd, struct statvfs64 *buf)
{
    VSF_LINUX_ASSERT(false);
    return -1;
}

int statvfs64(const char *path, struct statvfs64 *buf)
{
    VSF_LINUX_ASSERT(false);
    return -1;
}

int __vsf_linux_create_open_path(char *path)
{
    int fd = open(path, 0);
    if (fd < 0) {
        fd = creat(path, 0);
        if (fd < 0) {
            fprintf(stderr, "fail to create %s.\r\n", path);
        }
    }
    return fd;
}

int __vsf_linux_create_open_dir_path(char *path)
{
    int fd = open(path, 0);
    if (fd < 0) {
        if (!mkdirs(path, 0)) {
            fd = open(path, 0);
        } else {
            fprintf(stderr, "fail to create %s.\r\n", path);
        }
    }
    return fd;
}

// compatible with directory
int vsf_linux_fd_get_target(int fd, void **target)
{
    vk_vfs_file_t *vfs_file = __vsf_linux_get_vfs(fd);
    if (NULL == vfs_file) {
        return -1;
    }

    if (target != NULL) {
        *target = vfs_file->f.param;
    }
    return 0;
}

int vsf_linux_fd_bind_target_ex(int fd,
        void *target, const vsf_linux_fd_op_t *op,
        vsf_param_eda_evthandler_t peda_read,
        vsf_param_eda_evthandler_t peda_write,
        uint_fast32_t feature, uint64_t size)
{
    vk_vfs_file_t *vfs_file = __vsf_linux_get_vfs(fd);
    if (NULL == vfs_file) {
        return -1;
    }

    vfs_file->f.param = target;
    vfs_file->f.callback.fn_read = peda_read;
    vfs_file->f.callback.fn_write = peda_write;
    vfs_file->attr = feature;
    vfs_file->size = size;
    vfs_file->f.op = (void *)op;
    return 0;
}

int vsf_linux_fd_bind_target(int fd, void *target,
        vsf_param_eda_evthandler_t peda_read,
        vsf_param_eda_evthandler_t peda_write)
{
    return vsf_linux_fd_bind_target_ex(fd, target, NULL, peda_read, peda_write, 0, 0);
}

int vsf_linux_fs_get_target(const char *pathname, void **target)
{
    int fd = open(pathname, 0);
    if (fd < 0) {
        return -1;
    }

    int err = vsf_linux_fd_get_target(fd, target);
    close(fd);
    return err;
}

int vsf_linux_fs_bind_target_relative(vk_vfs_file_t *dir, const char *pathname,
        void *target, const vsf_linux_fd_op_t *op,
        uint_fast32_t feature, uint64_t size)
{
    VSF_LINUX_ASSERT(dir->attr & VSF_FILE_ATTR_DIRECTORY);
    vk_file_create(&dir->use_as__vk_file_t, pathname, feature);
    if (VSF_ERR_NONE != (vsf_err_t)vsf_eda_get_return_value()) {
        return -1;
    }
    vk_file_t *f;
    vk_file_open(&dir->use_as__vk_file_t, pathname, &f);
    if (VSF_ERR_NONE != (vsf_err_t)vsf_eda_get_return_value()) {
        return -1;
    }

    ((vk_vfs_file_t *)f)->size = size;
    ((vk_vfs_file_t *)f)->f.op = (void *)op;
    ((vk_vfs_file_t *)f)->f.param = target;
    vsf_trace_info("%s bound under %s.\r\n", pathname, dir->name);
    return 0;
}

int vsf_linux_fs_bind_target_ex(const char *pathname,
        void *target, const vsf_linux_fd_op_t *op,
        vsf_param_eda_evthandler_t peda_read,
        vsf_param_eda_evthandler_t peda_write,
        uint_fast32_t feature, uint64_t size)
{
    int fd = __vsf_linux_create_open_path((char *)pathname);
    if (fd >= 0) {
        int err = vsf_linux_fd_bind_target_ex(fd, target, op, peda_read, peda_write, feature, size);
        if (!err) {
            vsf_trace_info("%s bound.\r\n", pathname);
        }
        close(fd);
        return err;
    }
    return -1;
}

int vsf_linux_fs_bind_target(const char *pathname, void *target,
        vsf_param_eda_evthandler_t peda_read,
        vsf_param_eda_evthandler_t peda_write)
{
    return vsf_linux_fs_bind_target_ex(pathname, target, NULL, peda_read, peda_write, 0, 0);
}

int vsf_linux_fs_bind_fdpriv(const char *pathname, vsf_linux_fd_priv_t *priv,
        const vsf_linux_fd_op_t *op, uint_fast32_t feature, uint64_t size)
{
    return vsf_linux_fs_bind_target_ex(pathname, priv, op, NULL, NULL, feature, size);
}

int vsf_linux_fs_bind_dir_target(const char *pathname, void *target)
{
    int fd = __vsf_linux_create_open_dir_path((char *)pathname);
    if (fd >= 0) {
        vk_vfs_file_t *vfs_dir = __vsf_linux_get_vfs(fd);
        if (NULL == vfs_dir) {
            return -1;
        }

        vfs_dir->d.param = target;

        vsf_trace_info("%s bound.\r\n", pathname);
        close(fd);
        return 0;
    }
    return -1;
}

__vsf_component_peda_ifs_entry(__vk_vfs_buffer_write, vk_file_write)
{
    vsf_peda_begin();

    vk_vfs_file_t *vfs_file = (vk_vfs_file_t *)&vsf_this;
    void *buffer = vfs_file->f.param;
    int realsize;

    if (vfs_file->pos >= vfs_file->size) {
        realsize = -1;
    } else if (vfs_file->pos + vsf_local.size > vfs_file->size) {
        realsize = vfs_file->size - vfs_file->pos;
    } else {
        realsize = vsf_local.size;
    }

    if (realsize > 0) {
        memcpy((uint8_t *)buffer + vfs_file->pos, vsf_local.buff, realsize);
    }
    vsf_eda_return(realsize);

    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_vfs_buffer_read, vk_file_read)
{
    vsf_peda_begin();

    vk_vfs_file_t *vfs_file = (vk_vfs_file_t *)&vsf_this;
    void *buffer = vfs_file->f.param;
    int realsize;

    if (vfs_file->pos >= vfs_file->size) {
        realsize = -1;
    } else if (vfs_file->pos + vsf_local.size > vfs_file->size) {
        realsize = vfs_file->size - vfs_file->pos;
    } else {
        realsize = vsf_local.size;
    }

    if (realsize > 0) {
        memcpy(vsf_local.buff, (uint8_t *)buffer + vfs_file->pos, realsize);
    }
    vsf_eda_return(realsize);

    vsf_peda_end();
}

int vsf_linux_fs_bind_buffer(const char *pathname, void *buffer,
        uint_fast32_t feature, uint64_t size)
{
    return vsf_linux_fs_bind_target_ex(pathname, buffer, NULL,
        (vsf_peda_evthandler_t)vsf_peda_func(__vk_vfs_buffer_read),
        (vsf_peda_evthandler_t)vsf_peda_func(__vk_vfs_buffer_write),
        feature, size);
}

// stream

static int __vsf_linux_stream_fcntl(vsf_linux_fd_t *sfd, int cmd, uintptr_t arg)
{
    return __vsf_linux_default_fcntl(sfd, cmd, arg);
}

void __vsf_linux_stream_evt(vsf_linux_stream_priv_t *priv, vsf_protect_t orig, short event, bool is_ready)
{
    if (priv->on_evt != NULL) {
        priv->on_evt(priv, orig, event, is_ready);
    } else if (is_ready) {
        vsf_linux_fd_set_status(&priv->use_as__vsf_linux_fd_priv_t, event, orig);
    } else {
        vsf_linux_fd_clear_status(&priv->use_as__vsf_linux_fd_priv_t, event, orig);
    }
}

static void __vsf_linux_stream_evthandler(vsf_stream_t *stream, void *param, vsf_stream_evt_t evt)
{
    vsf_linux_stream_priv_t *priv = param;

    switch (evt) {
    case VSF_STREAM_ON_DISCONNECT:
        if (priv->stream_rx == stream) {
            goto on_stream_rx;
        } else if (priv->stream_tx == stream) {
            goto on_stream_tx;
        } else {
            VSF_LINUX_ASSERT(false);
        }
    case VSF_STREAM_ON_RX:
    on_stream_rx:
        __vsf_linux_stream_evt(priv, vsf_protect_sched(), POLLIN, true);
        break;
    case VSF_STREAM_ON_TX:
    on_stream_tx:
        __vsf_linux_stream_evt(priv, vsf_protect_sched(), POLLOUT, true);
        break;
    }
}

ssize_t __vsf_linux_stream_read(vsf_linux_fd_t *sfd, void *buf, size_t count)
{
    vsf_linux_stream_priv_t *priv = (vsf_linux_stream_priv_t *)sfd->priv;
    vsf_stream_t *stream = priv->stream_rx;
    VSF_LINUX_ASSERT(stream != NULL);
    uint_fast32_t size = count, cursize;
    vsf_protect_t orig;
    ssize_t result;

    vsf_linux_trigger_t trig;
    vsf_linux_trigger_init(&trig);
    while (size > 0) {
        orig = vsf_protect_sched();
        if (!vsf_linux_fd_get_status(&priv->use_as__vsf_linux_fd_priv_t, POLLIN) && (0 == vsf_stream_get_rbuf(stream, NULL))) {
            if (vsf_linux_fd_is_block(sfd) && vsf_stream_is_tx_connected(stream)) {
                if (!vsf_linux_fd_pend_events(&priv->use_as__vsf_linux_fd_priv_t, POLLIN, &trig, orig)) {
                    // triggered by signal
                    return -1;
                }
            } else {
                vsf_unprotect_sched(orig);
                errno = EAGAIN;
                goto do_return;
            }
        } else {
            vsf_unprotect_sched(orig);
        }

        cursize = vsf_stream_read(stream, buf, size);
        if ((buf != NULL) && isatty(sfd->fd) && !(priv->flags & O_NOCTTY)) {
            vsf_linux_term_priv_t *term_priv = (vsf_linux_term_priv_t *)priv;
            if (term_priv->termios.c_lflag & ECHO) {
                char ch;
                for (uint_fast32_t i = 0; i < cursize; i++) {
                    ch = ((char *)buf)[i];
                    switch (term_priv->esc_type) {
                    case '\033':
                        if ((ch == '[') || (ch == 'O')){
                            term_priv->esc_type = ch;
                        } else {
                            term_priv->esc_type = '\0';
                            goto char_input;
                        }
                        break;
                    case '[':
                        if (    ((ch >= 'a') && (ch <= 'z'))
                            ||  ((ch >= 'A') && (ch <= 'Z'))
                            ||  (ch == '~')) {
                            term_priv->esc_type = '\0';
                        }
                        break;
                    case 'O':
                        term_priv->esc_type = '\0';
                        break;
                    case '\0':
                    char_input:
                        switch (ch) {
                        case '\033':
                            term_priv->esc_type = '\033';
                            break;
                        case 0x7F:
                        case '\b':
                            if (!term_priv->line_start) {
                                write(STDOUT_FILENO, "\b \b", 3);
                            }
                            break;
                        default:
                            write(STDOUT_FILENO, &ch, 1);
                            break;
                        }
                        break;
                    }
                }
            }
        }

        size -= cursize;
        if (buf != NULL) {
            buf = (uint8_t *)buf + cursize;
        }

        orig = vsf_protect_sched();
        if (!vsf_stream_get_data_size(stream)) {
            __vsf_linux_stream_evt(priv, orig, POLLIN, false);
        } else {
            vsf_linux_fd_set_events(&priv->use_as__vsf_linux_fd_priv_t, POLLIN, orig);
        }

        if (!size || !(sfd->fd_flags & __FD_READALL)) {
            break;
        }
    }

do_return:
    result = count - size;
    // return 0 means EOF
    return 0 == result ? -1 : result;
}

ssize_t __vsf_linux_stream_write(vsf_linux_fd_t *sfd, const void *buf, size_t count)
{
    vsf_linux_stream_priv_t *priv = (vsf_linux_stream_priv_t *)sfd->priv;
    vsf_stream_t *stream = priv->stream_tx;
    VSF_LINUX_ASSERT(stream != NULL);
    uint_fast32_t size = count, cursize;
    vsf_protect_t orig;

    vsf_linux_trigger_t trig;
    vsf_linux_trigger_init(&trig);
    while (size > 0) {
        orig = vsf_protect_sched();
        if (!vsf_linux_fd_get_status(&priv->use_as__vsf_linux_fd_priv_t, POLLOUT)) {
            if (!vsf_linux_fd_pend_events(&priv->use_as__vsf_linux_fd_priv_t, POLLOUT, &trig, orig)) {
                // triggered by signal
                return -1;
            }
        } else {
            vsf_unprotect_sched(orig);
        }

        cursize = vsf_stream_write(stream, (uint8_t *)buf, size);
        size -= cursize;
        if (buf != NULL) {
            buf = (uint8_t *)buf + cursize;
        }

        orig = vsf_protect_sched();
        if (!vsf_stream_get_free_size(stream)) {
            __vsf_linux_stream_evt(priv, orig, POLLOUT, false);
        } else {
            vsf_linux_fd_set_events(&priv->use_as__vsf_linux_fd_priv_t, POLLOUT, orig);
        }
    }

    return count;
}

void __vsf_linux_tx_stream_init(vsf_linux_stream_priv_t *priv_tx)
{
    vsf_stream_t *stream = priv_tx->stream_tx;
    stream->tx.evthandler = __vsf_linux_stream_evthandler;
    stream->tx.param = priv_tx;
    vsf_stream_connect_tx(stream);

    vsf_protect_t orig = vsf_protect_sched();
    if (vsf_stream_get_free_size(stream)) {
        vsf_linux_fd_set_status(&priv_tx->use_as__vsf_linux_fd_priv_t, POLLOUT, orig);
    } else {
        vsf_unprotect_sched(orig);
    }
}

void __vsf_linux_tx_stream_fini(vsf_linux_stream_priv_t *priv_tx)
{
    if (priv_tx->stream_tx != NULL) {
        vsf_stream_disconnect_tx(priv_tx->stream_tx);
        if (    !(priv_tx->flags & __VSF_FILE_ATTR_SHARE_PRIV)
            &&  priv_tx->stream_tx_allocated) {
            vsf_linux_free_res(priv_tx->stream_tx);
        }
    }
}

void __vsf_linux_tx_stream_drain(vsf_linux_stream_priv_t *priv_tx)
{
    vsf_stream_t *stream_tx = priv_tx->stream_tx;
    if (stream_tx != NULL) {
        while (vsf_stream_get_data_size(stream_tx)) {
            usleep(10 * 1000);
        }
    }
}

void __vsf_linux_tx_stream_drop(vsf_linux_stream_priv_t *priv_tx)
{
    vsf_stream_t *stream = priv_tx->stream_tx;
    if (stream != NULL) {
        uint_fast32_t data_size;
        while ((data_size = vsf_stream_get_data_size(stream))) {
            vsf_stream_read(stream, NULL, data_size);
        }
    }
}

void __vsf_linux_rx_stream_init(vsf_linux_stream_priv_t *priv_rx)
{
    vsf_stream_t *stream = priv_rx->stream_rx;
    stream->rx.evthandler = __vsf_linux_stream_evthandler;
    stream->rx.param = priv_rx;
    vsf_stream_connect_rx(stream);

    vsf_protect_t orig = vsf_protect_sched();
    if (vsf_stream_get_data_size(stream)) {
        vsf_linux_fd_set_status(&priv_rx->use_as__vsf_linux_fd_priv_t, POLLIN, orig);
    } else {
        vsf_unprotect_sched(orig);
    }
}

void __vsf_linux_rx_stream_fini(vsf_linux_stream_priv_t *priv_rx)
{
    if (priv_rx->stream_rx != NULL) {
        vsf_stream_disconnect_rx(priv_rx->stream_rx);
        if (    !(priv_rx->flags & __VSF_FILE_ATTR_SHARE_PRIV)
            &&  priv_rx->stream_rx_allocated) {
            vsf_linux_free_res(priv_rx->stream_rx);
        }
    }
}

void __vsf_linux_rx_stream_drop(vsf_linux_stream_priv_t *priv_rx)
{
    vsf_stream_t *stream = priv_rx->stream_rx;
    if (stream != NULL) {
        uint_fast32_t data_size;
        while ((data_size = vsf_stream_get_data_size(stream))) {
            vsf_stream_read(stream, NULL, data_size);
        }
    }
}

static void __vsf_linux_stream_init(vsf_linux_fd_t *sfd)
{
    vsf_linux_stream_priv_t *priv = (vsf_linux_stream_priv_t *)sfd->priv;

    if (priv->file != NULL) {
        vsf_stream_t **streams = (vsf_stream_t **)(((vk_vfs_file_t *)(priv->file))->f.param);
        VSF_LINUX_ASSERT(streams != NULL);

        priv->stream_rx = streams[0];
        priv->stream_tx = streams[1];
    }

    if (priv->stream_rx != NULL) {
        __vsf_linux_rx_stream_init(priv);
    }
    if (priv->stream_tx != NULL) {
        __vsf_linux_tx_stream_init(priv);
    }
}

static int __vsf_linux_stream_close(vsf_linux_fd_t *sfd)
{
    vsf_linux_stream_priv_t *priv = (vsf_linux_stream_priv_t *)sfd->priv;
    __vsf_linux_rx_stream_fini(priv);
    __vsf_linux_tx_stream_fini(priv);
    return 0;
}

static int __vsf_linux_stream_eof(vsf_linux_fd_t *sfd)
{
    vsf_linux_stream_priv_t *priv = (vsf_linux_stream_priv_t *)sfd->priv;
    return !vsf_stream_is_tx_connected(priv->stream_rx);
}

static vsf_linux_fd_t * __vsf_linux_stream(vsf_stream_t *stream_rx, vsf_stream_t *stream_tx)
{
    vsf_linux_fd_t *sfd = NULL;
    vsf_linux_stream_priv_t *priv;

    if (vsf_linux_fd_create(&sfd, &__vsf_linux_stream_fdop) >= 0) {
        priv = (vsf_linux_stream_priv_t *)sfd->priv;
        priv->stream_rx = stream_rx;
        priv->stream_tx = stream_tx;
    }
    return sfd;
}

vsf_linux_fd_t * vsf_linux_stream(vsf_stream_t *stream_rx, vsf_stream_t *stream_tx)
{
    vsf_linux_fd_t *sfd = __vsf_linux_stream(stream_rx, stream_tx);
    if (sfd != NULL) {
        if (stream_rx != NULL) {
            __vsf_linux_rx_stream_init((vsf_linux_stream_priv_t *)sfd->priv);
        }
        if (stream_tx != NULL) {
            __vsf_linux_tx_stream_init((vsf_linux_stream_priv_t *)sfd->priv);
        }
    }
    return sfd;
}

vsf_linux_fd_t * vsf_linux_rx_stream(vsf_stream_t *stream)
{
    return vsf_linux_stream(stream, NULL);
}

vsf_linux_fd_t * vsf_linux_tx_stream(vsf_stream_t *stream)
{
    return vsf_linux_stream(NULL, stream);
}

int vsf_linux_fs_bind_stream(const char *pathname, vsf_stream_t *stream_rx, vsf_stream_t *stream_tx)
{
    vsf_stream_t **stream_ctx = vsf_linux_malloc_res(2 * sizeof(vsf_stream_t *));
    if (NULL == stream_ctx) {
        return -1;
    }

    stream_ctx[0] = stream_rx;
    stream_ctx[1] = stream_tx;
    int result = vsf_linux_fs_bind_target_ex(pathname, stream_ctx, &__vsf_linux_stream_fdop,
                NULL, NULL,
                VSF_FILE_ATTR_READ | VSF_FILE_ATTR_WRITE, 0);
    if (result) {
        vsf_linux_free_res(stream_ctx);
    }
    return result;
}

// pipe

int __vsf_linux_pipe_fcntl(vsf_linux_fd_t *sfd, int cmd, uintptr_t arg)
{
    return __vsf_linux_stream_fcntl(sfd, cmd, arg);
}

static int __vsf_linux_pipe_stat(vsf_linux_fd_t *sfd, struct stat *buf)
{
    buf->st_mode = S_IFIFO;
    return 0;
}

static int __vsf_linux_pipe_close(vsf_linux_fd_t *sfd)
{
    vsf_linux_pipe_priv_t *priv = (vsf_linux_pipe_priv_t *)sfd->priv;
    if (priv->stream_rx != NULL) {
        VSF_STREAM_READ(priv->stream_rx, NULL, 0xFFFFFFFF);
    }

    int result = 0;
    if (!(priv->flags & __VSF_FILE_ATTR_SHARE_PRIV)) {
        vsf_protect_t orig = vsf_protect_sched();
        if (priv->pipe_remote != NULL) {
            // __vsf_linux_stream_close will free stream_rx if it is alloctead,
            //  if prpe is connected(i.e. pipe_remote is not NULL), transfer stream_rx to stream_tx of remote pipe
            priv->pipe_remote->stream_tx_allocated = priv->stream_rx_allocated;
            priv->stream_rx_allocated = false;

            // pipe_tx is closed, set stick_events in pipe_rx with POLLLIN
            priv->pipe_remote->sticky_events = POLLIN;
            priv->pipe_remote->pipe_remote = NULL;
        }
        vsf_unprotect_sched(orig);
        result = __vsf_linux_stream_close(sfd);
    }
    return result;
}

int __vsf_linux_rx_pipe_init(vsf_linux_pipe_priv_t *priv_rx, vsf_queue_stream_t *queue_stream)
{
    if (NULL == queue_stream) {
        // pipe internals does not belong to process
        queue_stream = vsf_linux_malloc_res(sizeof(vsf_queue_stream_t));
        if (NULL == queue_stream) {
            return -1;
        }
        priv_rx->stream_rx_allocated = true;
    }

    queue_stream->max_buffer_size = -1;
    queue_stream->max_entry_num = -1;
    queue_stream->op = &vsf_queue_stream_op;
    priv_rx->stream_rx = &queue_stream->use_as__vsf_stream_t;
    vsf_stream_init(priv_rx->stream_rx);
#if VSF_STREAM_CFG_THRESHOLD == ENABLED
    vsf_stream_set_tx_threshold(priv_rx->stream_rx, 0);
    vsf_stream_set_rx_threshold(priv_rx->stream_rx, 0);
#endif

    __vsf_linux_rx_stream_init(&priv_rx->use_as__vsf_linux_stream_priv_t);
    return 0;
}

static vsf_linux_fd_t * __vsf_linux_rx_pipe(vsf_queue_stream_t *queue_stream, const vsf_linux_fd_op_t *op)
{
    vsf_linux_fd_t *sfd_rx = NULL;
    if (    (vsf_linux_fd_create(&sfd_rx, op) >= 0)
        &&  (__vsf_linux_rx_pipe_init((vsf_linux_pipe_priv_t *)sfd_rx->priv, queue_stream))) {
        vsf_linux_fd_delete(sfd_rx->fd);
        sfd_rx = NULL;
    }
    return sfd_rx;
}

vsf_linux_fd_t * vsf_linux_rx_pipe(vsf_queue_stream_t *queue_stream)
{
    return __vsf_linux_rx_pipe(queue_stream, &vsf_linux_pipe_fdop);
}

int __vsf_linux_tx_pipe_init(vsf_linux_pipe_priv_t *priv_tx, vsf_linux_pipe_priv_t *priv_rx)
{
    VSF_LINUX_ASSERT(priv_rx->stream_rx != NULL);
    priv_tx->stream_tx = priv_rx->stream_rx;

    __vsf_linux_tx_stream_init(&priv_tx->use_as__vsf_linux_stream_priv_t);

    priv_rx->pipe_remote = priv_tx;
    priv_tx->pipe_remote = priv_rx;
    return 0;
}

static vsf_linux_fd_t * __vsf_linux_tx_pipe(vsf_linux_pipe_priv_t *priv_rx, const vsf_linux_fd_op_t *op)
{
    VSF_LINUX_ASSERT(priv_rx != NULL);
    vsf_linux_fd_t *sfd_tx = NULL;
    if (    (vsf_linux_fd_create(&sfd_tx, op) >= 0)
        &&  (__vsf_linux_tx_pipe_init((vsf_linux_pipe_priv_t *)sfd_tx->priv, priv_rx))) {
        vsf_linux_fd_delete(sfd_tx->fd);
        sfd_tx = NULL;
    }
    return sfd_tx;
}

vsf_linux_fd_t * vsf_linux_tx_pipe(vsf_linux_pipe_priv_t *priv_rx)
{
    return __vsf_linux_tx_pipe(priv_rx, &vsf_linux_pipe_fdop);
}

int vsf_linux_pipe(const vsf_linux_fd_op_t *op, vsf_queue_stream_t *queue_stream1, vsf_queue_stream_t *queue_stream2,
                    vsf_linux_fd_t **sfd1, vsf_linux_fd_t **sfd2)
{
    VSF_LINUX_ASSERT((op != NULL) && (sfd1 != NULL) && (sfd2 != NULL));

    *sfd1 = __vsf_linux_rx_pipe(queue_stream1, op);
    if (NULL == *sfd1) {
        return -1;
    }
    *sfd2 = __vsf_linux_rx_pipe(queue_stream2, op);
    if (NULL == *sfd2) {
        vsf_linux_fd_delete((*sfd1)->fd);
        return -1;
    }

    vsf_linux_pipe_priv_t *priv1 = (vsf_linux_pipe_priv_t *)(*sfd1)->priv;
    vsf_linux_pipe_priv_t *priv2 = (vsf_linux_pipe_priv_t *)(*sfd2)->priv;
    __vsf_linux_tx_pipe_init(priv1, priv2);
    __vsf_linux_tx_pipe_init(priv2, priv1);

    return 0;
}

int vsf_linux_fs_bind_pipe(const char *pathname1, const char *pathname2, bool exclusive)
{
    int ret = 0;
    vsf_linux_fd_t *sfd1, *sfd2;

    if (vsf_linux_pipe(&vsf_linux_term_fdop, NULL, NULL, &sfd1, &sfd2)) {
        return -1;
    }

    uint_fast32_t feature = exclusive ? VSF_FILE_ATTR_EXCL : 0;
    feature |= VSF_FILE_ATTR_READ | VSF_FILE_ATTR_WRITE | VSF_FILE_ATTR_TTY | __VSF_FILE_ATTR_SHARE_PRIV;

    vsf_linux_term_priv_t *priv;
    priv = (vsf_linux_term_priv_t *)sfd1->priv;
    priv->subop = &vsf_linux_pipe_fdop;
    priv->flags |= __VSF_FILE_ATTR_SHARE_PRIV;
    vsf_linux_term_fdop.fn_init(sfd1);
    ret |= vsf_linux_fs_bind_fdpriv(pathname1, &priv->use_as__vsf_linux_fd_priv_t, &vsf_linux_term_fdop, feature, 0);
    priv = (vsf_linux_term_priv_t *)sfd2->priv;
    priv->subop = &vsf_linux_pipe_fdop;
    priv->flags |= __VSF_FILE_ATTR_SHARE_PRIV;
    vsf_linux_term_fdop.fn_init(sfd2);
    ret |= vsf_linux_fs_bind_fdpriv(pathname2, &priv->use_as__vsf_linux_fd_priv_t, &vsf_linux_term_fdop, feature, 0);

    close(sfd1->fd);
    close(sfd2->fd);
    return ret;
}

int vsf_linux_create_pty(int num)
{
    VSF_LINUX_ASSERT(num <= VSF_LINUX_CFG_MAX_PTY);

    char ptyp[11] = "/dev/ptyp0", ttyp[11] = "/dev/ttyp0";
    for (int i = 0; i < num; i++) {
        ptyp[9] = ttyp[9] = '0' + i;
        if (vsf_linux_fs_bind_pipe(ptyp, ttyp, true) < 0) {
            return i;
        }
    }
    return num;
}

int mkfifo(const char *pathname, mode_t mode)
{
    return -1;
}

int mkfifoat(int dirfd, const char *pathname, mode_t mode)
{
    return -1;
}

int mknod(const char *pathname, mode_t mode, dev_t dev)
{
    return -1;
}

int mknodat(int dirfd, const char *pathname, mode_t mode, dev_t dev)
{
    return -1;
}

// term

void __vsf_linux_term_rx(vsf_linux_fd_priv_t *priv)
{
    vsf_linux_term_priv_t *term_priv = (vsf_linux_term_priv_t *)priv;
    vsf_protect_t orig = vsf_protect_sched();
    if (vsf_stream_get_data_size(term_priv->stream_rx)) {
        vsf_linux_fd_set_status(&term_priv->use_as__vsf_linux_fd_priv_t, POLLIN, orig);
    } else {
        vsf_unprotect_sched(orig);
    }
}

void __vsf_linux_term_notify_rx(vsf_linux_term_priv_t *priv)
{
    vsf_eda_post_evt_msg(vsf_linux_get_kernel_task(), __VSF_EVT_LINUX_TERM_RX, priv);
}

static void __vsf_linux_term_init(vsf_linux_fd_t *sfd)
{
    vsf_linux_term_priv_t *priv = (vsf_linux_term_priv_t *)sfd->priv;
    if (priv->file != NULL) {
        // excl should be forced on terminals
        priv->file->attr |= VSF_FILE_ATTR_EXCL;
    }
    // default is 115200_8N1
    static const struct termios __default_term = {
        .c_cflag        = CS8,
        .c_oflag        = OPOST | ONLCR,
        .c_lflag        = ECHO | ECHOE | ECHOK | ECHONL | ICANON,
        .c_cc[VMIN]     = 1,
        .c_cc[VERASE]   = 010,      // BS
        .c_cc[VWERASE]  = 027,      // ETB
        .c_cc[VKILL]    = 025,      // NAK
        .c_ispeed       = B115200,
        .c_ospeed       = B115200,
    };
    priv->termios = __default_term;
    sfd->fd_flags |= __FD_READALL;

    const vsf_linux_fd_op_t *subop = priv->subop;
    if ((subop != NULL) && (subop->fn_init != NULL) && !priv->subop_inited) {
        subop->fn_init(sfd);
        priv->subop_inited = true;
    }
}

static void __vsf_linux_term_fini(vsf_linux_fd_t *sfd)
{
    vsf_linux_term_priv_t *priv = (vsf_linux_term_priv_t *)sfd->priv;
    const vsf_linux_fd_op_t *subop = priv->subop;
    if ((subop != NULL) && (subop->fn_fini != NULL) && priv->subop_inited) {
        subop->fn_fini(sfd);
        priv->subop_inited = false;
    }
}

static int __vsf_linux_term_fcntl(vsf_linux_fd_t *sfd, int cmd, uintptr_t arg)
{
    vsf_linux_term_priv_t *priv = (vsf_linux_term_priv_t *)sfd->priv;
    const vsf_linux_fd_op_t *subop = priv->subop;
    int ret = 0;

    switch (cmd) {
    case TCGETS:    *(struct termios *)arg = priv->termios;     return 0;
    case TCSETS:    priv->termios = *(struct termios *)arg;     break;
    }

    if (subop->fn_fcntl != NULL) {
        ret = subop->fn_fcntl(sfd, cmd, arg);
    }

    if (!ret) {
        switch (cmd) {
        case TCSETS:
            if (priv->termios.c_cc[VMIN] > 0) {
                sfd->priv->flags &= ~O_NONBLOCK;
            } else {
                sfd->priv->flags |= O_NONBLOCK;
            }
            break;
        }
    }
    return ret;
}

int __vsf_linux_term_stat(vsf_linux_fd_t *sfd, struct stat *buf)
{
    buf->st_mode = S_IFCHR;
    return 0;
}

static ssize_t __vsf_linux_term_read(vsf_linux_fd_t *sfd, void *buf, size_t count)
{
    vsf_linux_term_priv_t *priv = (vsf_linux_term_priv_t *)sfd->priv;
    const vsf_linux_fd_op_t *subop = priv->subop;

    if ((subop != NULL) && (subop->fn_read != NULL)) {
        return subop->fn_read(sfd, buf, count);
    }
    return 0;
}

static ssize_t __vsf_linux_term_write(vsf_linux_fd_t *sfd, const void *buf, size_t count)
{
    vsf_linux_term_priv_t *priv = (vsf_linux_term_priv_t *)sfd->priv;
    const vsf_linux_fd_op_t *subop = priv->subop;

    if ((subop != NULL) && (subop->fn_write != NULL)) {
        return subop->fn_write(sfd, buf, count);
    }
    return 0;
}

static int __vsf_linux_term_close(vsf_linux_fd_t *sfd)
{
    vsf_linux_term_priv_t *priv = (vsf_linux_term_priv_t *)sfd->priv;
    const vsf_linux_fd_op_t *subop = priv->subop;
    int result = 0;

    if ((subop != NULL) && (subop->fn_close != NULL)) {
        result = subop->fn_close(sfd);
    }
    if (priv->file != NULL) {
        __vsf_linux_fs_close(sfd);
    }
    return result;
}

// key

static void __vsf_linux_key_fini(vsf_linux_fd_t *sfd)
{
    vsf_linux_key_priv_t *priv = (vsf_linux_key_priv_t *)sfd->priv;
    if (priv->fn_fini != NULL) {
        priv->fn_fini(sfd);
    }
}

static int __vsf_linux_key_close(vsf_linux_fd_t *sfd)
{
    vsf_linux_key_priv_t *priv = (vsf_linux_key_priv_t *)sfd->priv;
    if (priv->fn_close != NULL) {
        return priv->fn_close(sfd);
    }
    return 0;
}

static int __vsf_linux_key_stat(vsf_linux_fd_t *sfd, struct stat *buf)
{
    buf->st_mode = S_IFCHR;
    return 0;
}

// mntent

FILE * setmntent(const char *filename, const char *type)
{
    VSF_LINUX_ASSERT(false);
    return (FILE *)NULL;
}

struct mntent * getmntent(FILE *stream)
{
    VSF_LINUX_ASSERT(false);
    return (struct mntent *)NULL;
}

int addmntent(FILE *stream, const struct mntent *mnt)
{
    VSF_LINUX_ASSERT(false);
    return -1;
}

int endmntent(FILE *stream)
{
    VSF_LINUX_ASSERT(false);
    return -1;
}

char * hasmntopt(const struct mntent *mnt, const char *opt)
{
    VSF_LINUX_ASSERT(false);
    return (char *)NULL;
}

struct mntent * getmntent_r(FILE *stream, struct mntent *mntbuf, char *buf, int buflen)
{
    VSF_LINUX_ASSERT(false);
    return (struct mntent *)NULL;
}

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic pop
#endif

#if VSF_LINUX_APPLET_USE_MNTENT == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_mntent_vplt_t vsf_linux_mntent_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_mntent_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(setmntent),
    VSF_APPLET_VPLT_ENTRY_FUNC(getmntent),
    VSF_APPLET_VPLT_ENTRY_FUNC(addmntent),
    VSF_APPLET_VPLT_ENTRY_FUNC(endmntent),
    VSF_APPLET_VPLT_ENTRY_FUNC(hasmntopt),
    VSF_APPLET_VPLT_ENTRY_FUNC(getmntent_r),
};
#endif

#if VSF_LINUX_APPLET_USE_POLL == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_poll_vplt_t vsf_linux_poll_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_poll_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(poll),
    VSF_APPLET_VPLT_ENTRY_FUNC(ppoll),
};
#endif

#if VSF_LINUX_APPLET_USE_SYS_EPOLL == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_sys_epoll_vplt_t vsf_linux_sys_epoll_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_sys_epoll_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(epoll_create),
    VSF_APPLET_VPLT_ENTRY_FUNC(epoll_create1),
    VSF_APPLET_VPLT_ENTRY_FUNC(epoll_ctl),
    VSF_APPLET_VPLT_ENTRY_FUNC(epoll_wait),
    VSF_APPLET_VPLT_ENTRY_FUNC(epoll_pwait),
    VSF_APPLET_VPLT_ENTRY_FUNC(epoll_pwait2),
};
#endif

#if VSF_LINUX_APPLET_USE_FCNTL == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_fcntl_vplt_t vsf_linux_fcntl_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_fcntl_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(__fcntl_va),
    VSF_APPLET_VPLT_ENTRY_FUNC(fcntl),
    VSF_APPLET_VPLT_ENTRY_FUNC(creat),
    VSF_APPLET_VPLT_ENTRY_FUNC(__open_va),
    VSF_APPLET_VPLT_ENTRY_FUNC(open),
    VSF_APPLET_VPLT_ENTRY_FUNC(__openat_va),
    VSF_APPLET_VPLT_ENTRY_FUNC(openat),
};
#endif

#if VSF_LINUX_APPLET_USE_DIRENT == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_dirent_vplt_t vsf_linux_dirent_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_dirent_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(opendir),
    VSF_APPLET_VPLT_ENTRY_FUNC(fdopendir),
    VSF_APPLET_VPLT_ENTRY_FUNC(readdir),
    VSF_APPLET_VPLT_ENTRY_FUNC(readdir64),
    VSF_APPLET_VPLT_ENTRY_FUNC(readdir_r),
    VSF_APPLET_VPLT_ENTRY_FUNC(readdir64_r),
    VSF_APPLET_VPLT_ENTRY_FUNC(rewinddir),
    VSF_APPLET_VPLT_ENTRY_FUNC(telldir),
    VSF_APPLET_VPLT_ENTRY_FUNC(seekdir),
    VSF_APPLET_VPLT_ENTRY_FUNC(closedir),
    VSF_APPLET_VPLT_ENTRY_FUNC(scandir),
    VSF_APPLET_VPLT_ENTRY_FUNC(alphasort),
};
#endif

#if VSF_LINUX_APPLET_USE_SYS_EVENTFD == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_sys_eventfd_vplt_t vsf_linux_sys_eventfd_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_sys_eventfd_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(eventfd),
    VSF_APPLET_VPLT_ENTRY_FUNC(eventfd_read),
    VSF_APPLET_VPLT_ENTRY_FUNC(eventfd_write),
};
#endif

#if VSF_LINUX_APPLET_USE_SYS_SELECT == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_sys_select_vplt_t vsf_linux_sys_select_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_sys_select_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(select),
    VSF_APPLET_VPLT_ENTRY_FUNC(pselect),
};
#endif

#if VSF_LINUX_APPLET_USE_SYS_STAT == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_sys_stat_vplt_t vsf_linux_sys_stat_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_sys_stat_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(umask),
    VSF_APPLET_VPLT_ENTRY_FUNC(stat),
    VSF_APPLET_VPLT_ENTRY_FUNC(lstat),
    VSF_APPLET_VPLT_ENTRY_FUNC(fstat),
    VSF_APPLET_VPLT_ENTRY_FUNC(fstatat),
    VSF_APPLET_VPLT_ENTRY_FUNC(chmod),
    VSF_APPLET_VPLT_ENTRY_FUNC(fchmod),
    VSF_APPLET_VPLT_ENTRY_FUNC(mkdir),
    VSF_APPLET_VPLT_ENTRY_FUNC(mkfifo),
    VSF_APPLET_VPLT_ENTRY_FUNC(mkfifoat),
    VSF_APPLET_VPLT_ENTRY_FUNC(mknod),
    VSF_APPLET_VPLT_ENTRY_FUNC(mknodat),
    VSF_APPLET_VPLT_ENTRY_FUNC(lstat),
    VSF_APPLET_VPLT_ENTRY_FUNC(futimens),
    VSF_APPLET_VPLT_ENTRY_FUNC(utimensat),
};
#endif

#if VSF_LINUX_APPLET_USE_SYS_SENDFILE == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_sys_sendfile_vplt_t vsf_linux_sys_sendfile_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_sys_sendfile_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(sendfile),
};
#endif

#if VSF_LINUX_APPLET_USE_SYS_STATVFS == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_sys_statvfs_vplt_t vsf_linux_sys_statvfs_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_sys_statvfs_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(fstatvfs),
    VSF_APPLET_VPLT_ENTRY_FUNC(statvfs),
    VSF_APPLET_VPLT_ENTRY_FUNC(fstatvfs64),
    VSF_APPLET_VPLT_ENTRY_FUNC(statvfs64),
};
#endif

#if VSF_LINUX_APPLET_USE_SYS_FILE == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_sys_file_vplt_t vsf_linux_sys_file_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_sys_file_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(flock),
};
#endif

#if VSF_LINUX_APPLET_USE_SYS_IOCTL == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_sys_ioctl_vplt_t vsf_linux_sys_ioctl_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_sys_ioctl_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(ioctl),
};
#endif

#endif      // VSF_USE_LINUX
