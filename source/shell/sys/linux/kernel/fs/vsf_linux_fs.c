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
#   include "../../include/poll.h"
#   include "../../include/fcntl.h"
#   include "../../include/errno.h"
#else
#   include <unistd.h>
#   include <sys/stat.h>
#   include <sys/mount.h>
#   include <poll.h>
#   include <fcntl.h>
#   include <errno.h>
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

#include "./vsf_linux_fs.h"

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_suppress=pe111
#endif

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_linux_stream_priv_t {
    vsf_stream_t *stream;
} vsf_linux_stream_priv_t;

typedef struct vsf_linux_pipe_buffer_t {
    vsf_slist_node_t buffer_node;
    size_t size;
    size_t pos;
} vsf_linux_pipe_buffer_t;

/*============================ PROTOTYPES ====================================*/

static int __vsf_linux_fs_fcntl(vsf_linux_fd_t *sfd, int cmd, long arg);
static ssize_t __vsf_linux_fs_read(vsf_linux_fd_t *sfd, void *buf, size_t count);
static ssize_t __vsf_linux_fs_write(vsf_linux_fd_t *sfd, const void *buf, size_t count);
static int __vsf_linux_fs_close(vsf_linux_fd_t *sfd);

static int __vsf_linux_stream_fcntl(vsf_linux_fd_t *sfd, int cmd, long arg);
static ssize_t __vsf_linux_stream_read(vsf_linux_fd_t *sfd, void *buf, size_t count);
static ssize_t __vsf_linux_stream_write(vsf_linux_fd_t *sfd, const void *buf, size_t count);
static int __vsf_linux_stream_close(vsf_linux_fd_t *sfd);

static int __vsf_linux_pipe_fcntl(vsf_linux_fd_t *sfd, int cmd, long arg);
static ssize_t __vsf_linux_pipe_read(vsf_linux_fd_t *sfd, void *buf, size_t count);
static ssize_t __vsf_linux_pipe_write(vsf_linux_fd_t *sfd, const void *buf, size_t count);
static int __vsf_linux_pipe_close(vsf_linux_fd_t *sfd);

/*============================ LOCAL VARIABLES ===============================*/

const vsf_linux_fd_op_t __vsf_linux_fs_fdop = {
    .priv_size          = sizeof(vsf_linux_fs_priv_t),
    .fn_fcntl           = __vsf_linux_fs_fcntl,
    .fn_read            = __vsf_linux_fs_read,
    .fn_write           = __vsf_linux_fs_write,
    .fn_close           = __vsf_linux_fs_close,
};

/*============================ GLOBAL VARIABLES ==============================*/

static const vsf_linux_fd_op_t __vsf_linux_stream_fdop = {
    .priv_size          = sizeof(vsf_linux_stream_priv_t),
    .fn_fcntl           = __vsf_linux_stream_fcntl,
    .fn_read            = __vsf_linux_stream_read,
    .fn_write           = __vsf_linux_stream_write,
    .fn_close           = __vsf_linux_stream_close,
};

const vsf_linux_fd_op_t vsf_linux_pipe_rx_fdop = {
    .priv_size          = sizeof(vsf_linux_pipe_rx_priv_t),
    .fn_fcntl           = __vsf_linux_pipe_fcntl,
    .fn_read            = __vsf_linux_pipe_read,
    .fn_close           = __vsf_linux_pipe_close,
};

const vsf_linux_fd_op_t vsf_linux_pipe_tx_fdop = {
    .priv_size          = sizeof(vsf_linux_pipe_tx_priv_t),
    .fn_fcntl           = __vsf_linux_pipe_fcntl,
    .fn_write           = __vsf_linux_pipe_write,
    .fn_close           = __vsf_linux_pipe_close,
};

/*============================ IMPLEMENTATION ================================*/

static vk_file_t * __vsf_linux_fs_get_file(const char *pathname)
{
    vk_file_t *file;

    vk_file_open(NULL, pathname, 0, &file);
    return file;
}

static void __vsf_linux_fs_close_do(vk_file_t *file)
{
    vk_file_close(file);
}

static int __vsf_linux_fs_fcntl(vsf_linux_fd_t *sfd, int cmd, long arg)
{
    return 0;
}

static ssize_t __vsf_linux_fs_read(vsf_linux_fd_t *sfd, void *buf, size_t count)
{
    vsf_linux_fs_priv_t *priv = (vsf_linux_fs_priv_t *)sfd->priv;
    vk_file_t *file = priv->file;
    ssize_t result = 0;
    int32_t rsize;

    while (count > 0) {
        vk_file_read(file, priv->pos, count, (uint8_t *)buf);
        rsize = (int32_t)vsf_eda_get_return_value();
        if (rsize < 0) {
            return -1;
        } else if (!rsize) {
            break;
        }

        count -= rsize;
        result += rsize;
        priv->pos += rsize;
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
        vk_file_write(file, priv->pos, count, (uint8_t *)buf);
        wsize = (int32_t)vsf_eda_get_return_value();
        if (wsize < 0) {
            return -1;
        } else if (!wsize) {
            break;
        }

        count -= wsize;
        result += wsize;
        priv->pos += wsize;
        buf = (uint8_t *)buf + wsize;
    }
    return result;
}

static int __vsf_linux_fs_close(vsf_linux_fd_t *sfd)
{
    vsf_linux_fs_priv_t *priv = (vsf_linux_fs_priv_t *)sfd->priv;
    __vsf_linux_fs_close_do(priv->file);
    return 0;
}

vsf_linux_fd_t * vsf_linux_fd_get(int fd)
{
    vsf_dlist_t *fd_list = &vsf_linux_get_cur_process()->fd_list;
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

static vk_vfs_file_t * __vsf_linux_get_vfs(int fd)
{
    vsf_linux_fd_t *sfd = vsf_linux_fd_get(fd);
    if ((NULL == sfd) || (sfd->op != &__vsf_linux_fs_fdop)) {
        return NULL;
    }

    vk_file_t *file = ((vsf_linux_fs_priv_t *)sfd->priv)->file;
    if ((file->fsop != &vk_vfs_op) || (file->attr & VSF_FILE_ATTR_DIRECTORY)) {
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
    uint32_t orig_feature;
    if (vsf_linux_fd_get_feature(fd, &orig_feature) < 0) {
        return -1;
    }
    return vsf_linux_fd_set_feature(fd, orig_feature | feature);
}

int vsf_linux_fd_add(vsf_linux_fd_t *sfd)
{
    vsf_linux_process_t *process = vsf_linux_get_cur_process();

    sfd->rxpend = sfd->txpend = NULL;

    vsf_protect_t orig = vsf_protect_sched();
#ifdef VSF_LINUX_CFG_FD_BITMAP_SIZE
        sfd->fd = vsf_bitmap_ffz(&process->fd_bitmap, VSF_LINUX_CFG_FD_BITMAP_SIZE);
        VSF_LINUX_ASSERT(sfd->fd >= 0);
        vsf_bitmap_set(&process->fd_bitmap, sfd->fd);
#else
        sfd->fd = process->cur_fd++;
#endif
        vsf_dlist_add_to_tail(vsf_linux_fd_t, fd_node, &process->fd_list, sfd);
    vsf_unprotect_sched(orig);

    return sfd->fd;
}

int vsf_linux_fd_create(vsf_linux_fd_t **sfd, const vsf_linux_fd_op_t *op)
{
    int priv_size = (op != NULL) ? op->priv_size : 0;
    vsf_linux_fd_t *new_sfd;
#if     VSF_LINUX_USE_SIMPLE_LIBC == ENABLED && VSF_LINUX_USE_SIMPLE_STDLIB == ENABLED\
    &&  VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_FD == ENABLED
    vsf_linux_process_t *process = vsf_linux_get_cur_process();
    if (process->cur_fd <= 2) {
        new_sfd = vsf_heap_malloc(sizeof(vsf_linux_fd_t) + priv_size);
        if (new_sfd != NULL) {
            memset(new_sfd, 0, sizeof(vsf_linux_fd_t) + priv_size);
        }
    } else {
        new_sfd = calloc(1, sizeof(vsf_linux_fd_t) + priv_size);
    }
#else
    new_sfd = calloc(1, sizeof(vsf_linux_fd_t) + priv_size);
#endif
    if (!new_sfd) {
        errno = ENOMEM;
        return -1;
    }
    new_sfd->op = op;

    if (sfd != NULL) {
        *sfd = new_sfd;
    }
    return vsf_linux_fd_add(new_sfd);
}

void vsf_linux_fd_delete(int fd)
{
    vsf_linux_fd_t *sfd = vsf_linux_fd_get(fd);
    vsf_linux_process_t *process = vsf_linux_get_cur_process();

    vsf_protect_t orig = vsf_protect_sched();
        vsf_dlist_remove(vsf_linux_fd_t, fd_node, &process->fd_list, sfd);
#ifdef VSF_LINUX_CFG_FD_BITMAP_SIZE
        vsf_bitmap_clear(&process->fd_bitmap, sfd->fd);
#endif
    vsf_unprotect_sched(orig);

#if     VSF_LINUX_USE_SIMPLE_LIBC == ENABLED && VSF_LINUX_USE_SIMPLE_STDLIB == ENABLED\
    &&  VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_FD == ENABLED
    extern const vsf_linux_fd_op_t __vsf_linux_heap_fdop;
    if ((sfd->op == &__vsf_linux_heap_fdop) || (fd <= 2)) {
        vsf_heap_free(sfd);
    } else {
        free(sfd);
    }
#else
    free(sfd);
#endif
}

void vsf_linux_fd_trigger_init(vsf_trig_t *trig)
{
    vsf_eda_trig_init(trig, false, true);
}

// vsf_linux_fd_tx_[pend/ready/busy/trigger] MUST be called scheduler protected
int vsf_linux_fd_tx_pend(vsf_linux_fd_t *sfd, vsf_trig_t *trig, vsf_protect_t orig)
{
    if (sfd->txevt) {
        sfd->txevt = false;
        vsf_unprotect_sched(orig);
    } else {
        sfd->txpend = trig;
        vsf_unprotect_sched(orig);
        vsf_thread_trig_pend(trig, -1);
    }
    return 0;
}

int vsf_linux_fd_rx_pend(vsf_linux_fd_t *sfd, vsf_trig_t *trig, vsf_protect_t orig)
{
    if (sfd->rxevt) {
        sfd->rxevt = false;
        vsf_unprotect_sched(orig);
    } else {
        sfd->rxpend = trig;
        vsf_unprotect_sched(orig);
        vsf_thread_trig_pend(trig, -1);
    }
    return 0;
}

int vsf_linux_fd_tx_trigger(vsf_linux_fd_t *sfd, vsf_protect_t orig)
{
    vsf_trig_t *trig = sfd->txpend;
    if (trig != NULL) {
        vsf_unprotect_sched(orig);
        sfd->txpend = NULL;
        vsf_eda_trig_set_isr(trig);
    } else {
        sfd->txevt = true;
        vsf_unprotect_sched(orig);
    }
    return 0;
}

int vsf_linux_fd_rx_trigger(vsf_linux_fd_t *sfd, vsf_protect_t orig)
{
    vsf_trig_t *trig = sfd->rxpend;
    if (trig != NULL) {
        vsf_unprotect_sched(orig);
        sfd->rxpend = NULL;
        vsf_eda_trig_set_isr(trig);
    } else {
        sfd->rxevt = true;
        vsf_unprotect_sched(orig);
    }
    return 0;
}

int vsf_linux_fd_tx_ready(vsf_linux_fd_t *sfd, vsf_protect_t orig)
{
    if (!sfd->txrdy) {
        sfd->txrdy = true;
        return vsf_linux_fd_tx_trigger(sfd, orig);
    } else {
        vsf_unprotect_sched(orig);
    }
    return 0;
}

int vsf_linux_fd_rx_ready(vsf_linux_fd_t *sfd, vsf_protect_t orig)
{
    if (!sfd->rxrdy) {
        sfd->rxrdy = true;
        return vsf_linux_fd_rx_trigger(sfd, orig);
    } else {
        vsf_unprotect_sched(orig);
    }
    return 0;
}

void vsf_linux_fd_tx_busy(vsf_linux_fd_t *sfd, vsf_protect_t orig)
{
    if (sfd->txpend != NULL) {
        VSF_LINUX_ASSERT(false);
    } else {
        sfd->txevt = sfd->txrdy = false;
    }
    vsf_unprotect_sched(orig);
}

void vsf_linux_fd_rx_busy(vsf_linux_fd_t *sfd, vsf_protect_t orig)
{
    if (sfd->rxpend != NULL) {
        VSF_LINUX_ASSERT(false);
    } else {
        sfd->rxevt = sfd->rxrdy = false;
    }
    vsf_unprotect_sched(orig);
}

int __vsf_linux_poll_tick(struct pollfd *fds, nfds_t nfds, vsf_timeout_tick_t timeout)
{
    vsf_protect_t orig;
    vsf_linux_fd_t *sfd;
    int ret = 0;
    nfds_t i;
    vsf_trig_t trig;

    vsf_linux_fd_trigger_init(&trig);
    while (1) {
        orig = vsf_protect_sched();
        for (i = 0; i < nfds; i++) {
            sfd = vsf_linux_fd_get(fds[i].fd);
            // fd maybe closed by other thread while being polled
            if (NULL == sfd) {
                continue;
            }
            if (sfd->rxevt || sfd->txevt) {
                if ((fds[i].events & POLLIN) && sfd->rxevt) {
                    sfd->rxevt = sfd->rxrdy;
                    fds[i].revents |= POLLIN;
                }
                if ((fds[i].events & POLLOUT) && sfd->txevt) {
                    sfd->txevt = sfd->txrdy;
                    fds[i].revents |= POLLOUT;
                }
                if (fds[i].revents) {
                    ret++;
                }
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
            if (fds[i].events & POLLIN) {
                sfd->rxpend = &trig;
            }
            if (fds[i].events & POLLOUT) {
                sfd->txpend = &trig;
            }
        }
        vsf_unprotect_sched(orig);

        vsf_sync_reason_t r = vsf_thread_trig_pend(&trig, timeout);
        if (VSF_SYNC_TIMEOUT == r) {
            return 0;
        } else if (r != VSF_SYNC_GET) {
            return -1;
        }

        for (i = 0; i < nfds; i++) {
            sfd = vsf_linux_fd_get(fds[i].fd);
            // fd maybe closed by other thread while being polled
            if (NULL == sfd) {
                continue;
            }
            orig = vsf_protect_sched();
                if (fds[i].events & POLLIN) {
                    if (NULL == sfd->rxpend) {
                        sfd->rxevt = true;
                    } else {
                        sfd->rxpend = NULL;
                    }
                }
                if (fds[i].events & POLLOUT) {
                    if (NULL == sfd->txpend) {
                        sfd->txevt = true;
                    } else {
                        sfd->txpend = NULL;
                    }
                }
            vsf_unprotect_sched(orig);
        }
    }
    return 0;
}

int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *execeptfds, struct timeval *timeout)
{
    int fd_num = 0;

    VSF_LINUX_ASSERT((nfds > 0) && (nfds < FD_SETSIZE));
    if (execeptfds != NULL) {
        VSF_LINUX_ASSERT(false);
        return -1;
    }

    fd_set mask;
    FD_ZERO(&mask);
    for (int i = 0; i < nfds; i++) {
        if (    ((readfds != NULL) && (FD_ISSET(i, readfds)))
            ||  ((writefds != NULL) && (FD_ISSET(i, writefds)))) {
            FD_SET(i, &mask);
            fd_num++;
        }
    }

    struct pollfd *fds = NULL;
    if (fd_num > 0) {
        fds = malloc(fd_num * sizeof(struct pollfd));
        if (NULL == fds) {
            return -1;
        }
        memset(fds, 0, fd_num * sizeof(struct pollfd));

        for (int i = 0, idx = 0; i < nfds; i++) {
            if (FD_ISSET(i, &mask)) {
                fds[idx].fd = i;
                if ((readfds != NULL) && (FD_ISSET(i, readfds))) {
                    fds[idx].events |= POLLIN;
                }
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
        }
    }
    if (fds != NULL) {
        free(fds);
    }
    return ret;
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

static int __vsf_linux_fs_create(const char *pathname, mode_t mode, vk_file_attr_t attr, uint_fast64_t size)
{
    char fullpath[MAX_PATH], *name_tmp;
    int err = 0;
    if (vsf_linux_generate_path(fullpath, sizeof(fullpath), NULL, (char *)pathname)) {
        return -1;
    }

    name_tmp = vk_file_getfilename((char *)fullpath);
    fullpath[name_tmp - fullpath] = '\0';
    vk_file_t *dir = __vsf_linux_fs_get_file(fullpath);
    if (!dir) {
        return -1;
    }

    name_tmp = vk_file_getfilename((char *)pathname);
    vk_file_create(dir, name_tmp, attr, size);
    if (VSF_ERR_NONE != (vsf_err_t)vsf_eda_get_return_value()) {
        err = -1;
    }
    __vsf_linux_fs_close_do(dir);
    if (!err) {
        err = open(pathname, 0);
    }

    return err;
}

static int __vsf_linux_fs_remove(const char *pathname, vk_file_attr_t attr)
{
    char fullpath[MAX_PATH];
    if (vsf_linux_generate_path(fullpath, sizeof(fullpath), NULL, (char *)pathname)) {
        return -1;
    }

    vk_file_t *file = __vsf_linux_fs_get_file(fullpath), *dir;
    if (!file) {
        errno = ENOENT;
        return -1;
    }

    // check file attr
    int fattr = file->attr & S_IFMT;
    if (0 == fattr) {
        fattr |= S_IFREG;
    }
    if (0 == attr) {
        attr |= S_IFREG;
    }
    if (!(fattr & attr)) {
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

int mkdir(const char *pathname, mode_t mode)
{
    int fd = __vsf_linux_fs_create(pathname, mode, VSF_FILE_ATTR_DIRECTORY | VSF_FILE_ATTR_READ | VSF_FILE_ATTR_WRITE, 0);
    if (fd >= 0) {
        close(fd);
        fd = 0;
    }
    return fd;
}

int rmdir(const char *pathname)
{
    return __vsf_linux_fs_remove(pathname, VSF_FILE_ATTR_DIRECTORY);
}

int vsf_linux_chdir(vsf_linux_process_t *process, char *pathname)
{
    VSF_LINUX_ASSERT(process != NULL);
    char fullpath[MAX_PATH];
    if (vsf_linux_generate_path(fullpath, sizeof(fullpath), NULL, (char *)pathname)) {
        return -1;
    }

    int len = strlen(fullpath);
    if (fullpath[len - 1] != '/') {
        VSF_LINUX_ASSERT(len + 1 < MAX_PATH);
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

    vsf_heap_free(process->working_dir);
    process->working_dir = vsf_heap_malloc(strlen(fullpath) + 1);
    if (process->working_dir != NULL) {
        strcpy(process->working_dir, fullpath);
        return 0;
    }
    VSF_LINUX_ASSERT(process->working_dir != NULL);
    return -1;
}

int chdir(const char *pathname)
{
    vsf_linux_process_t *process = vsf_linux_get_cur_process();
    return vsf_linux_chdir(process, (char *)pathname);
}

int creat(const char *pathname, mode_t mode)
{
    return __vsf_linux_fs_create(pathname, mode, VSF_FILE_ATTR_READ | VSF_FILE_ATTR_WRITE, 0);
}

int open(const char *pathname, int flags, ...)
{
    vk_file_t *file;
    vsf_linux_fd_t *sfd;
    char fullpath[MAX_PATH];
    int fd;

    if (vsf_linux_generate_path(fullpath, sizeof(fullpath), NULL, (char *)pathname)) {
        return -1;
    }

    file = __vsf_linux_fs_get_file(fullpath);
    if (!file) {
        if (flags & O_CREAT) {
            va_list ap;
            mode_t mode;

            va_start(ap, flags);
                mode = va_arg(ap, mode_t);
            va_end(ap);

            return creat(fullpath, mode);
        }
        return -1;
    }

    fd = vsf_linux_fd_create(&sfd, &__vsf_linux_fs_fdop);
    if (fd < 0) {
        __vsf_linux_fs_close_do(file);
    } else {
        vsf_linux_fs_priv_t *priv = (vsf_linux_fs_priv_t *)sfd->priv;
        sfd->flags = flags;
        priv->file = file;
        if ((flags & O_TRUNC) && !(file->attr & VSF_FILE_ATTR_DIRECTORY)) {
            // todo:
        }
    }
    return fd;
}

int close(int fd)
{
    vsf_linux_fd_t *sfd = vsf_linux_fd_get(fd);
    if (!sfd) { return -1; }

    int err = sfd->op->fn_close(sfd);
    vsf_linux_fd_delete(fd);
    return err;
}

int fcntl(int fd, int cmd, ...)
{
    vsf_linux_fd_t *sfd = vsf_linux_fd_get(fd);
    va_list ap;
    long arg;

    if (!sfd) { return -1; }
    va_start(ap, cmd);
        arg = va_arg(ap, long);
    va_end(ap);
    return sfd->op->fn_fcntl(sfd, cmd, arg);
}

ssize_t read(int fd, void *buf, size_t count)
{
    vsf_linux_fd_t *sfd = vsf_linux_fd_get(fd);
    if (!sfd || (sfd->flags & O_WRONLY)) { return -1; }
    return sfd->op->fn_read(sfd, buf, count);
}

ssize_t write(int fd, const void *buf, size_t count)
{
    vsf_linux_fd_t *sfd = vsf_linux_fd_get(fd);
    if (!sfd || (sfd->flags & O_RDONLY)) { return -1; }
    return sfd->op->fn_write(sfd, buf, count);
}

off_t lseek(int fd, off_t offset, int whence)
{
    vsf_linux_fd_t *sfd = vsf_linux_fd_get(fd);
    VSF_LINUX_ASSERT(sfd->op == &__vsf_linux_fs_fdop);
    vsf_linux_fs_priv_t *priv = (vsf_linux_fs_priv_t *)sfd->priv;
    uint_fast64_t new_pos;

    switch (whence) {
    case SEEK_SET:  new_pos = 0;                break;
    case SEEK_CUR:  new_pos = priv->pos;        break;
    case SEEK_END:  new_pos = priv->file->size; break;
    default:        return -1;
    }

    new_pos += offset;
    if (new_pos > priv->file->size) {
        return -1;
    }
    priv->pos = new_pos;
    return (off_t)new_pos;
}

int fstat(int fd, struct stat *buf)
{
    vsf_linux_fd_t *sfd = vsf_linux_fd_get(fd);

    memset(buf, 0, sizeof(*buf));
    if (&__vsf_linux_fs_fdop == sfd->op) {
        vsf_linux_fs_priv_t *priv = (vsf_linux_fs_priv_t *)sfd->priv;
        vk_file_t *file = priv->file;

        buf->st_mode = file->attr;
        if (!(file->attr & S_IFMT)) {
            buf->st_mode |= S_IFREG;
        }
        buf->st_size = file->size;
        return 0;
    } else {
        return -1;
    }
}

int stat(const char *pathname, struct stat *buf)
{
    int fd = open(pathname, 0);
    if (fd < 0) { return -1; }

    int ret = fstat(fd, buf);
    close(fd);
    return ret;
}

int access(const char *pathname, int mode)
{
    int fd = open(pathname, mode);
    if (fd < 0) { return -1; }

    int ret = 0;
    vk_vfs_file_t *vfs_file = __vsf_linux_get_vfs(fd);
    if (    ((mode & R_OK) && !(vfs_file->attr & VSF_FILE_ATTR_READ))
        ||  ((mode & W_OK) && !(vfs_file->attr & VSF_FILE_ATTR_WRITE))
        ||  ((mode & X_OK) && !(vfs_file->attr & VSF_FILE_ATTR_EXECUTE))) {
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
    return __vsf_linux_fs_remove(pathname, S_IFMT & ~S_IFDIR);
}

DIR * opendir(const char *name)
{
    int fd = open(name, 0);
    if (fd < 0) {
        return NULL;
    }

    vsf_linux_fd_t *sfd = vsf_linux_fd_get(fd);
    vsf_linux_fs_priv_t *priv = (vsf_linux_fs_priv_t *)sfd->priv;
    if (!(priv->file->attr & VSF_FILE_ATTR_DIRECTORY)) {
        close(fd);
        sfd = NULL;
    }
    return sfd;
}

struct dirent * readdir(DIR *dir)
{
    vsf_linux_fs_priv_t *priv = (vsf_linux_fs_priv_t *)dir->priv;
    vk_file_t *file = priv->file, *child;

    if (priv->child != NULL) {
        vk_file_close(priv->child);
        priv->child = NULL;
    }

    vk_file_open(file, NULL, priv->dir.d_ino++, &priv->child);
    if (NULL == priv->child) {
        return NULL;
    }

    child = priv->child;
    priv->dir.d_name = child->name;
    priv->dir.d_reclen = vk_file_get_name_length(child);
    priv->dir.d_type = child->attr & VSF_FILE_ATTR_DIRECTORY ? DT_DIR :
                child->attr & VSF_FILE_ATTR_EXECUTE ? DT_EXE : DT_REG;
    return &priv->dir;
}

int closedir(DIR *dir)
{
    vsf_linux_fs_priv_t *priv = (vsf_linux_fs_priv_t *)dir->priv;

    if (priv->child != NULL) {
        vk_file_close(priv->child);
        priv->child = NULL;
    }
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
        vk_malfs_mounter_t mounter;
        mounter.dir = dir;
        mounter.mal = (vk_mal_t *)data;
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

int vsf_linux_fd_get_target(int fd, void **target)
{
    vk_vfs_file_t *vfs_file = __vsf_linux_get_vfs(fd);
    if ((NULL == vfs_file) || (vfs_file->attr & VSF_FILE_ATTR_DIRECTORY)) {
        return -1;
    }

    if (target != NULL) {
        *target = vfs_file->f.param;
    }
    return 0;
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

int vsf_linux_fd_bind_target(int fd, void *target,
        vsf_param_eda_evthandler_t peda_read,
        vsf_param_eda_evthandler_t peda_write)
{
    vk_vfs_file_t *vfs_file = __vsf_linux_get_vfs(fd);
    if (NULL == vfs_file) {
        return -1;
    }

    vfs_file->f.param = target;
    vfs_file->f.callback.fn_read = peda_read;
    vfs_file->f.callback.fn_write = peda_write;
    return 0;
}

// stream
static int __vsf_linux_stream_fcntl(vsf_linux_fd_t *sfd, int cmd, long arg)
{
    VSF_LINUX_ASSERT(false);
    return 0;
}

static void __vsf_linux_stream_evthandler(void *param, vsf_stream_evt_t evt)
{
    vsf_linux_fd_t *sfd = param;
    switch (evt) {
    case VSF_STREAM_ON_RX:
        vsf_linux_fd_rx_ready(sfd, vsf_protect_sched());
        break;
    case VSF_STREAM_ON_TX:
        vsf_linux_fd_tx_ready(sfd, vsf_protect_sched());
        break;
    }
}

static ssize_t __vsf_linux_stream_read(vsf_linux_fd_t *sfd, void *buf, size_t count)
{
    vsf_linux_stream_priv_t *priv = (vsf_linux_stream_priv_t *)sfd->priv;
    vsf_stream_t *stream = priv->stream;
    uint_fast32_t size = count, cursize;
    vsf_protect_t orig;

    while (size > 0) {
        orig = vsf_protect_sched();
        if (!sfd->rxrdy && (0 == vsf_stream_get_rbuf(stream, NULL))) {
            vsf_trig_t trig;
            vsf_linux_fd_trigger_init(&trig);
            vsf_linux_fd_rx_pend(sfd, &trig, orig);
        } else {
            vsf_unprotect_sched(orig);
        }

        cursize = vsf_stream_read(stream, buf, size);
        if (sfd->fd == STDIN_FILENO) {
            static int skip_echo = 0;
            char ch;
            for (uint_fast32_t i = 0; i < cursize; i++) {
                ch = ((char *)buf)[i];
                switch (ch) {
                case '\033':skip_echo = 2;                      break;
                case '\r':  write(STDOUT_FILENO, "\r\n", 2);    break;
                case 0x7F:
                case '\b':  write(STDOUT_FILENO, "\b \b", 3);   break;
                default:
                    if (skip_echo) {
                        skip_echo--;
                        break;
                    }
                    write(STDOUT_FILENO, &ch, 1);
                    break;
                }
            }
        }

        size -= cursize;
        buf = (uint8_t *)buf + cursize;
    }

    orig = vsf_protect_sched();
    VSF_LINUX_ASSERT(NULL == sfd->rxpend);
    if (!vsf_stream_get_data_size(stream)) {
        vsf_linux_fd_rx_busy(sfd, orig);
    } else {
        vsf_linux_fd_rx_trigger(sfd, orig);
    }
    return count;
}

static ssize_t __vsf_linux_stream_write(vsf_linux_fd_t *sfd, const void *buf, size_t count)
{
    vsf_linux_stream_priv_t *priv = (vsf_linux_stream_priv_t *)sfd->priv;
    vsf_stream_t *stream = priv->stream;
    uint_fast32_t size = count, cursize;
    vsf_protect_t orig;

    while (size > 0) {
        orig = vsf_protect_sched();
        if (!sfd->txrdy) {
            vsf_trig_t trig;
            vsf_linux_fd_trigger_init(&trig);
            vsf_linux_fd_tx_pend(sfd, &trig, orig);
        } else {
            vsf_unprotect_sched(orig);
        }

        cursize = vsf_stream_write(stream, (uint8_t *)buf, size);
        size -= cursize;
        buf = (uint8_t *)buf + cursize;
    }

    orig = vsf_protect_sched();
    VSF_LINUX_ASSERT(NULL == sfd->txpend);
    if (!vsf_stream_get_free_size(stream)) {
        vsf_linux_fd_tx_busy(sfd, orig);
    } else {
        vsf_linux_fd_tx_trigger(sfd, orig);
    }
    return count;
}

static int __vsf_linux_stream_close(vsf_linux_fd_t *sfd)
{
    return 0;
}

static vsf_linux_fd_t * __vsf_linux_stream(vsf_stream_t *stream)
{
    vsf_linux_fd_t *sfd = NULL;
    vsf_linux_stream_priv_t *stream_priv;

    if (vsf_linux_fd_create(&sfd, &__vsf_linux_stream_fdop) >= 0) {
        stream_priv = (vsf_linux_stream_priv_t *)sfd->priv;
        stream_priv->stream = stream;
    }
    return sfd;
}

vsf_linux_fd_t * vsf_linux_rx_stream(vsf_stream_t *stream)
{
    vsf_linux_fd_t *sfd = __vsf_linux_stream(stream);
    if (sfd != NULL) {
        stream->rx.evthandler = __vsf_linux_stream_evthandler;
        stream->rx.param = sfd;
        vsf_stream_connect_rx(stream);

        vsf_protect_t orig = vsf_protect_sched();
        if (vsf_stream_get_data_size(stream)) {
            vsf_linux_fd_tx_ready(sfd, orig);
        } else {
            vsf_unprotect_sched(orig);
        }
    }
    return sfd;
}

vsf_linux_fd_t * vsf_linux_tx_stream(vsf_stream_t *stream)
{
    vsf_linux_fd_t *sfd = __vsf_linux_stream(stream);
    if (sfd != NULL) {
        stream->tx.evthandler = __vsf_linux_stream_evthandler;
        stream->tx.param = sfd;
        vsf_stream_connect_tx(stream);

        vsf_protect_t orig = vsf_protect_sched();
        if (vsf_stream_get_free_size(stream)) {
            vsf_linux_fd_tx_ready(sfd, orig);
        } else {
            vsf_unprotect_sched(orig);
        }
    }
    return sfd;
}

vsf_stream_t * vsf_linux_get_stream(vsf_linux_fd_t *sfd)
{
    if (sfd->op == &__vsf_linux_stream_fdop) {
        return ((vsf_linux_stream_priv_t *)sfd->priv)->stream;
    }
    return NULL;
}

// pipe
static int __vsf_linux_pipe_fcntl(vsf_linux_fd_t *sfd, int cmd, long arg)
{
    return 0;
}

static ssize_t __vsf_linux_pipe_read(vsf_linux_fd_t *sfd_rx, void *buf, size_t count)
{
    vsf_linux_pipe_rx_priv_t *priv_rx = (vsf_linux_pipe_rx_priv_t *)sfd_rx->priv;
    vsf_linux_pipe_buffer_t *buffer;
    ssize_t read_cnt = 0, cur_size;

    vsf_trig_t trig;
    vsf_linux_fd_trigger_init(&trig);

    vsf_protect_t orig;
    while (true) {
        orig = vsf_protect_sched();
        while (!vsf_slist_queue_is_empty(&priv_rx->buffer_queue)) {
            vsf_slist_queue_peek(vsf_linux_pipe_buffer_t, buffer_node, &priv_rx->buffer_queue, buffer);
            vsf_unprotect_sched(orig);

            cur_size = buffer->size - buffer->pos;
            cur_size = min(cur_size, count);
            memcpy(buf, (uint8_t *)&buffer[1] + buffer->pos, cur_size);
            buf = (uint8_t *)buf + cur_size;
            count -= cur_size;
            read_cnt += cur_size;
            buffer->pos += cur_size;
            if (buffer->pos >= buffer->size) {
                orig = vsf_protect_sched();
                    vsf_slist_queue_dequeue(vsf_linux_pipe_buffer_t, buffer_node, &priv_rx->buffer_queue, buffer);
                vsf_unprotect_sched(orig);

                vsf_heap_free(buffer);
            }

            orig = vsf_protect_sched();
            if (!count) {
                break;
            }
        }

        if (!read_cnt) {
            vsf_linux_fd_rx_pend(sfd_rx, &trig, orig);
            continue;
        }

        VSF_LINUX_ASSERT(NULL == sfd_rx->rxpend);
        if (vsf_slist_queue_is_empty(&priv_rx->buffer_queue)) {
            if (priv_rx->on_evt != NULL) {
                priv_rx->on_evt(sfd_rx, orig, false);
            } else {
                vsf_linux_fd_rx_busy(sfd_rx, orig);
            }
        } else {
            if (priv_rx->on_evt != NULL) {
                priv_rx->on_evt(sfd_rx, orig, true);
            } else {
                vsf_linux_fd_rx_ready(sfd_rx, orig);
            }
        }
        break;
    }
    return read_cnt;
}

static ssize_t __vsf_linux_pipe_write(vsf_linux_fd_t *sfd_tx, const void *buf, size_t count)
{
    vsf_linux_pipe_tx_priv_t *priv_tx = (vsf_linux_pipe_tx_priv_t *)sfd_tx->priv;
    vsf_linux_fd_t *sfd_rx = priv_tx->sfd_rx;
    vsf_linux_pipe_rx_priv_t *priv_rx;

    if (NULL == sfd_rx) {
        return -1;
    }

    priv_rx = (vsf_linux_pipe_rx_priv_t *)sfd_rx->priv;

    vsf_linux_pipe_buffer_t *buffer = vsf_heap_malloc(sizeof(*buffer) + count);
    if (NULL == buffer) {
        return -1;
    }
    vsf_slist_init_node(vsf_linux_pipe_buffer_t, buffer_node, buffer);
    buffer->pos = 0;
    buffer->size = count;
    memcpy(&buffer[1], buf, count);

    vsf_protect_t orig = vsf_protect_sched();
    bool is_empty = vsf_slist_queue_is_empty(&priv_rx->buffer_queue);
    vsf_slist_queue_enqueue(vsf_linux_pipe_buffer_t, buffer_node, &priv_rx->buffer_queue, buffer);
    if (is_empty) {
        if (priv_rx->on_evt != NULL) {
            priv_rx->on_evt(sfd_rx, orig, true);
        } else {
            vsf_linux_fd_rx_ready(sfd_rx, orig);
        }
    } else {
        vsf_unprotect_sched(orig);
    }
    return count;
}

static int __vsf_linux_pipe_close(vsf_linux_fd_t *sfd)
{
    if (&vsf_linux_pipe_rx_fdop == sfd->op) {
        vsf_linux_pipe_rx_priv_t *priv_rx = (vsf_linux_pipe_rx_priv_t *)sfd->priv;
        vsf_linux_pipe_buffer_t *buffer;

        vsf_protect_t orig = vsf_protect_sched();
        while (!vsf_slist_queue_is_empty(&priv_rx->buffer_queue)) {
            vsf_slist_queue_dequeue(vsf_linux_pipe_buffer_t, buffer_node, &priv_rx->buffer_queue, buffer);
            vsf_unprotect_sched(orig);
            vsf_heap_free(buffer);
            orig = vsf_protect_sched();
        }
        vsf_unprotect_sched(orig);
    }
    return 0;
}

vsf_linux_fd_t * vsf_linux_rx_pipe(void)
{
    vsf_linux_fd_t *sfd_rx;
    if (vsf_linux_fd_create(&sfd_rx, &vsf_linux_pipe_rx_fdop) >= 0) {
        vsf_linux_pipe_rx_priv_t *priv_rx = (vsf_linux_pipe_rx_priv_t *)sfd_rx->priv;
        vsf_slist_queue_init(&priv_rx->buffer_queue);
    }
    return sfd_rx;
}

vsf_linux_fd_t * vsf_linux_tx_pipe(vsf_linux_fd_t *sfd_rx)
{
    vsf_linux_fd_t *sfd_tx;
    if (vsf_linux_fd_create(&sfd_tx, &vsf_linux_pipe_tx_fdop) >= 0) {
        vsf_linux_pipe_tx_priv_t *priv_tx = (vsf_linux_pipe_tx_priv_t *)sfd_tx->priv;
        priv_tx->sfd_rx = sfd_rx;
    }
    return sfd_tx;
}

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

#endif      // VSF_USE_LINUX
