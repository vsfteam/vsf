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
#   include "../../include/poll.h"
#   include "../../include/fcntl.h"
#   include "../../include/errno.h"
#   include "../../include/termios.h"
#else
#   include <unistd.h>
#   include <sys/stat.h>
#   include <sys/mount.h>
#   include <poll.h>
#   include <fcntl.h>
#   include <errno.h>
#   include <termios.h>
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
    .fn_read            = __vsf_linux_stream_read,
    .fn_close           = __vsf_linux_pipe_close,
};

const vsf_linux_fd_op_t vsf_linux_pipe_tx_fdop = {
    .priv_size          = sizeof(vsf_linux_pipe_tx_priv_t),
    .fn_fcntl           = __vsf_linux_pipe_fcntl,
    .fn_write           = __vsf_linux_stream_write,
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

    while ((count > 0) && (priv->pos < file->size)) {
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

vsf_linux_fd_t * __vsf_linux_fd_get_ex(vsf_linux_process_t *process, int fd)
{
    if (NULL == process) {
        process = vsf_linux_get_cur_process();
    }

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
    if ((NULL == sfd) || (sfd->op != &__vsf_linux_fs_fdop)) {
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

static int __vsf_linux_fd_add(vsf_linux_process_t *process, vsf_linux_fd_t *sfd, int fd_desired)
{
    if (NULL == process) {
        process = vsf_linux_get_cur_process();
    }

    vsf_protect_t orig = vsf_protect_sched();
        if (fd_desired >= 0) {
#ifdef VSF_LINUX_CFG_FD_BITMAP_SIZE
            if (vsf_bitmap_get(&process->fd_bitmap, fd_desired)) {
                vsf_unprotect_sched(orig);
                return -1;
            }
            vsf_bitmap_set(&process->fd_bitmap, fd_desired);
            sfd->fd = fd_desired;
#else
            vsf_unprotect_sched(orig);
            return -1;
#endif
        } else {
#ifdef VSF_LINUX_CFG_FD_BITMAP_SIZE
            sfd->fd = vsf_bitmap_ffz(&process->fd_bitmap, VSF_LINUX_CFG_FD_BITMAP_SIZE);
            VSF_LINUX_ASSERT(sfd->fd >= 0);
            vsf_bitmap_set(&process->fd_bitmap, sfd->fd);
#else
            sfd->fd = process->cur_fd++;
#endif
        }
        vsf_dlist_add_to_tail(vsf_linux_fd_t, fd_node, &process->fd_list, sfd);
    vsf_unprotect_sched(orig);

    return sfd->fd;
}

int __vsf_linux_fd_create_ex(vsf_linux_process_t *process, vsf_linux_fd_t **sfd,
        const vsf_linux_fd_op_t *op, int fd_desired, bool allocate_priv)
{
    int priv_size = (op != NULL) ? op->priv_size : sizeof(vsf_linux_fd_priv_t);
    int ret;
    vsf_linux_fd_t *new_sfd;

    new_sfd = calloc(1, sizeof(vsf_linux_fd_t));
    if (!new_sfd) {
        errno = ENOMEM;
        return -1;
    }
    new_sfd->op = op;
    if (allocate_priv) {
        new_sfd->priv = calloc(1, priv_size);
        if (!new_sfd->priv) {
            ret = -1;
            goto free_sfd_and_exit;
        }
        ((vsf_linux_fd_priv_t *)new_sfd->priv)->ref = 1;
    }

    if (sfd != NULL) {
        *sfd = new_sfd;
    }

    ret = __vsf_linux_fd_add(process, new_sfd, fd_desired);
    if (ret < 0) {
free_sfd_and_exit:
        free(new_sfd);
    }
    return ret;
}

int vsf_linux_fd_create(vsf_linux_fd_t **sfd, const vsf_linux_fd_op_t *op)
{
    return __vsf_linux_fd_create_ex(NULL, sfd, op, -1, true);
}

void __vsf_linux_fd_delete_ex(vsf_linux_process_t *process, int fd)
{
    if (NULL == process) {
        process = vsf_linux_get_cur_process();
    }
    vsf_linux_fd_t *sfd = __vsf_linux_fd_get_ex(process, fd);

    vsf_protect_t orig = vsf_protect_sched();
        vsf_dlist_remove(vsf_linux_fd_t, fd_node, &process->fd_list, sfd);
#ifdef VSF_LINUX_CFG_FD_BITMAP_SIZE
        vsf_bitmap_clear(&process->fd_bitmap, sfd->fd);
#endif
    vsf_unprotect_sched(orig);

    free(sfd);
}

void vsf_linux_fd_delete(int fd)
{
    return __vsf_linux_fd_delete_ex(NULL, fd);
}

bool vsf_linux_fd_is_block(vsf_linux_fd_t *sfd)
{
    vsf_linux_process_t *process;
#if VSF_LINUX_USE_TERMIOS == ENABLED
    if (sfd->fd < 3) {
        process = vsf_linux_get_cur_process();
        VSF_LINUX_ASSERT(process != NULL);
        struct termios *term = &process->term[sfd->fd];
        return term->c_cc[VMIN] > 0;
    }
#endif
    return !(sfd->status_flags & O_NONBLOCK);
}

void vsf_linux_fd_trigger_init(vsf_trig_t *trig)
{
    vsf_eda_trig_init(trig, false, true);
}

// vsf_linux_fd_tx_[pend/ready/busy/trigger] MUST be called scheduler protected
int vsf_linux_fd_tx_pend(vsf_linux_fd_t *sfd, vsf_trig_t *trig, vsf_protect_t orig)
{
    if (sfd->priv->txevt) {
        sfd->priv->txevt = false;
        vsf_unprotect_sched(orig);
    } else {
        sfd->priv->txpend = trig;
        vsf_unprotect_sched(orig);
        vsf_thread_trig_pend(trig, -1);
    }
    return 0;
}

int vsf_linux_fd_rx_pend(vsf_linux_fd_t *sfd, vsf_trig_t *trig, vsf_protect_t orig)
{
    if (sfd->priv->rxevt) {
        sfd->priv->rxevt = false;
        vsf_unprotect_sched(orig);
    } else {
        sfd->priv->rxpend = trig;
        vsf_unprotect_sched(orig);
        vsf_thread_trig_pend(trig, -1);
    }
    return 0;
}

int vsf_linux_fd_tx_trigger(vsf_linux_fd_t *sfd, vsf_protect_t orig)
{
    vsf_trig_t *trig = sfd->priv->txpend;
    if (trig != NULL) {
        vsf_unprotect_sched(orig);
        sfd->priv->txpend = NULL;
        vsf_eda_trig_set_isr(trig);
    } else {
        sfd->priv->txevt = true;
        vsf_unprotect_sched(orig);
    }
    return 0;
}

int vsf_linux_fd_rx_trigger(vsf_linux_fd_t *sfd, vsf_protect_t orig)
{
    vsf_trig_t *trig = sfd->priv->rxpend;
    if (trig != NULL) {
        vsf_unprotect_sched(orig);
        sfd->priv->rxpend = NULL;
        vsf_eda_trig_set_isr(trig);
    } else {
        sfd->priv->rxevt = true;
        vsf_unprotect_sched(orig);
    }
    return 0;
}

int vsf_linux_fd_tx_ready(vsf_linux_fd_t *sfd, vsf_protect_t orig)
{
    if (!sfd->priv->txrdy) {
        sfd->priv->txrdy = true;
        return vsf_linux_fd_tx_trigger(sfd, orig);
    } else {
        vsf_unprotect_sched(orig);
    }
    return 0;
}

int vsf_linux_fd_rx_ready(vsf_linux_fd_t *sfd, vsf_protect_t orig)
{
    if (!sfd->priv->rxrdy) {
        sfd->priv->rxrdy = true;
        return vsf_linux_fd_rx_trigger(sfd, orig);
    } else {
        vsf_unprotect_sched(orig);
    }
    return 0;
}

void vsf_linux_fd_tx_busy(vsf_linux_fd_t *sfd, vsf_protect_t orig)
{
    if (sfd->priv->txpend != NULL) {
        VSF_LINUX_ASSERT(false);
    } else {
        sfd->priv->txevt = sfd->priv->txrdy = false;
    }
    vsf_unprotect_sched(orig);
}

void vsf_linux_fd_rx_busy(vsf_linux_fd_t *sfd, vsf_protect_t orig)
{
    if (sfd->priv->rxpend != NULL) {
        VSF_LINUX_ASSERT(false);
    } else {
        sfd->priv->rxevt = sfd->priv->rxrdy = false;
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
            if (sfd->priv->rxevt || sfd->priv->txevt) {
                if ((fds[i].events & POLLIN) && sfd->priv->rxevt) {
                    sfd->priv->rxevt = sfd->priv->rxrdy;
                    fds[i].revents |= POLLIN;
                }
                if ((fds[i].events & POLLOUT) && sfd->priv->txevt) {
                    sfd->priv->txevt = sfd->priv->txrdy;
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
                sfd->priv->rxpend = &trig;
            }
            if (fds[i].events & POLLOUT) {
                sfd->priv->txpend = &trig;
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
                    if (NULL == sfd->priv->rxpend) {
                        sfd->priv->rxevt = true;
                    } else {
                        sfd->priv->rxpend = NULL;
                    }
                }
                if (fds[i].events & POLLOUT) {
                    if (NULL == sfd->priv->txpend) {
                        sfd->priv->txevt = true;
                    } else {
                        sfd->priv->txpend = NULL;
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
    if ((NULL == pathname) || ('\0' == *pathname)) {
        return -1;
    }

    int fd = __vsf_linux_fs_create(pathname, mode, VSF_FILE_ATTR_DIRECTORY | VSF_FILE_ATTR_READ | VSF_FILE_ATTR_WRITE, 0);
    if (fd >= 0) {
        close(fd);
        fd = 0;
    }
    return fd;
}

int mkdirs(const char *pathname, mode_t mode)
{
    char *path_in_ram;
    if ((NULL == pathname) || ('\0' == *pathname)) {
        return -1;
    }
    path_in_ram = strdup(pathname);
    if (NULL == path_in_ram) {
        return -1;
    }

    int ret = -1;
    struct stat statbuf;
    for (   char *tmp = path_in_ram + (*pathname == '/' ? 1 : 0), *end;
            (end = strchr(tmp, '/')) != NULL;
            tmp = end + 1) {
        *end = '\0';
        if (    (stat(path_in_ram, &statbuf) < 0)
            &&  (mkdir(path_in_ram, mode) < 0)) {
            goto _exit_failure;
        }
        *end = '/';
    }
    ret = mkdir(path_in_ram, mode);
_exit_failure:
    free(path_in_ram);
    return ret;
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
    close(newfd);
    return fcntl(oldfd, F_DUPFD, newfd);
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
    if ((NULL == pathname) || ('\0' == *pathname)) {
        return -1;
    }
    vsf_linux_process_t *process = vsf_linux_get_cur_process();
    return vsf_linux_chdir(process, (char *)pathname);
}

int creat(const char *pathname, mode_t mode)
{
    if ((NULL == pathname) || ('\0' == *pathname)) {
        return -1;
    }
    return __vsf_linux_fs_create(pathname, mode, VSF_FILE_ATTR_READ | VSF_FILE_ATTR_WRITE, 0);
}

int open(const char *pathname, int flags, ...)
{
    vk_file_t *file;
    vsf_linux_fd_t *sfd;
    char fullpath[MAX_PATH];
    int fd;

    if ((NULL == pathname) || ('\0' == *pathname)) {
        return -1;
    }
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
        sfd->status_flags = flags;
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

    vsf_linux_fd_priv_t *priv = sfd->priv;
    vsf_protect_t orig = vsf_protect_sched();
        bool is_to_close = --priv->ref == 0;
    vsf_unprotect_sched(orig);

    int err = 0;
    if (is_to_close) {
        err = sfd->op->fn_close(sfd);
        free(sfd->priv);
    }
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

    // process generic commands
    switch (cmd) {
    case F_DUPFD: {
            vsf_linux_fd_t *sfd_new;
            int ret = __vsf_linux_fd_create_ex(NULL, &sfd_new, sfd->op, arg, false);
            if (ret < 0) {
                return ret;
            }

            vsf_linux_fd_priv_t *priv = sfd->priv;
            vsf_protect_t orig = vsf_protect_sched();
                priv->ref++;
            vsf_unprotect_sched(orig);
            sfd_new->priv = priv;
            return ret;
        }
        break;
    case F_GETFD:
        return sfd->fd_flags;
        break;
    case F_SETFD: {
            long tmp_arg = arg;
            arg = arg ^ sfd->fd_flags;
            sfd->fd_flags = tmp_arg;
        }
        break;
    case F_GETFL:
        return sfd->status_flags;
        break;
    case F_SETFL: {
            long tmp_arg = arg;
            arg = arg ^ sfd->status_flags;
            sfd->status_flags = tmp_arg;
        }
        break;
    }

    return sfd->op->fn_fcntl(sfd, cmd, arg);
}

ssize_t read(int fd, void *buf, size_t count)
{
    vsf_linux_fd_t *sfd = vsf_linux_fd_get(fd);
    if (!sfd || (sfd->status_flags & O_WRONLY)) { return -1; }
    return sfd->op->fn_read(sfd, buf, count);
}

ssize_t write(int fd, const void *buf, size_t count)
{
    vsf_linux_fd_t *sfd = vsf_linux_fd_get(fd);
    if (!sfd || (sfd->status_flags & O_RDONLY)) { return -1; }
    return sfd->op->fn_write(sfd, buf, count);
}

ssize_t pread(int fd, void *buf, size_t count, off_t offset)
{
    off_t orig = lseek(fd, 0, SEEK_CUR);
    lseek(fd, offset, SEEK_SET);
    ssize_t size = read(fd, buf, count);
    lseek(fd, orig, SEEK_SET);
    return size;
}

ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset)
{
    off_t orig = lseek(fd, 0, SEEK_CUR);
    lseek(fd, offset, SEEK_SET);
    ssize_t size = write(fd, buf, count);
    lseek(fd, orig, SEEK_SET);
    return size;
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

int fsync(int fd)
{
    return 0;
}

int fdatasync(int fd)
{
    return 0;
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
    if ((NULL == pathname) || ('\0' == *pathname)) {
        return -1;
    }

    int fd = open(pathname, 0);
    if (fd < 0) { return -1; }

    int ret = fstat(fd, buf);
    close(fd);
    return ret;
}

int chmod(const char *pathname, mode_t mode)
{
    return 0;
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

int __vsf_linux_create_open_path(char *path)
{
    int fd = open(path, 0);
    if (fd < 0) {
        fd = creat(path, 0);
        if (fd < 0) {
            printf("fail to create %s.\r\n", path);
        }
    }
    return fd;
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

int vsf_linux_fs_bind_target_ex(const char *pathname, void *target,
        vsf_param_eda_evthandler_t peda_read,
        vsf_param_eda_evthandler_t peda_write,
        uint_fast32_t feature, uint64_t size)
{
    int fd = __vsf_linux_create_open_path((char *)pathname);
    if (fd >= 0) {
        int err = vsf_linux_fd_bind_target(fd, target, peda_read, peda_write);
        if (!err) {
            vsf_linux_fd_set_feature(fd, feature);
            vsf_linux_fd_set_size(fd, size);
            printf("%s bound.\r\n", pathname);
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
    return vsf_linux_fs_bind_target_ex(pathname, target, peda_read, peda_write, 0, 0);
}

__vsf_component_peda_ifs_entry(__vk_vfs_buffer_write, vk_file_write)
{
    vsf_peda_begin();

    vk_vfs_file_t *vfs_file = (vk_vfs_file_t *)&vsf_this;
    void *buffer = vfs_file->f.param;
    int realsize;

    if (vsf_local.offset >= vfs_file->size) {
        realsize = -1;
    } else if (vsf_local.offset + vsf_local.size > vfs_file->size) {
        realsize = vfs_file->size - vsf_local.offset;
    } else {
        realsize = vsf_local.size;
    }

    if (realsize > 0) {
        memcpy((uint8_t *)buffer + vsf_local.offset, vsf_local.buff, realsize);
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

    if (vsf_local.offset >= vfs_file->size) {
        realsize = -1;
    } else if (vsf_local.offset + vsf_local.size > vfs_file->size) {
        realsize = vfs_file->size - vsf_local.offset;
    } else {
        realsize = vsf_local.size;
    }

    if (realsize > 0) {
        memcpy(vsf_local.buff, (uint8_t *)buffer + vsf_local.offset, realsize);
    }
    vsf_eda_return(realsize);

    vsf_peda_end();
}

int vsf_linux_fs_bind_buffer(const char *pathname, void *buffer,
        uint_fast32_t feature, uint64_t size)
{
    return vsf_linux_fs_bind_target_ex(pathname, buffer,
        (vsf_peda_evthandler_t)vsf_peda_func(__vk_vfs_buffer_read),
        (vsf_peda_evthandler_t)vsf_peda_func(__vk_vfs_buffer_write),
        feature, size);
}

// stream
static int __vsf_linux_stream_fcntl(vsf_linux_fd_t *sfd, int cmd, long arg)
{
    VSF_LINUX_ASSERT(false);
    return 0;
}

static void __vsf_linux_stream_evt(vsf_linux_fd_t *sfd, vsf_protect_t orig, bool is_rx, bool is_ready)
{
    vsf_linux_stream_priv_t *priv = (vsf_linux_stream_priv_t *)sfd->priv;
        if (priv->on_evt != NULL) {
            priv->on_evt(sfd, orig, is_ready);
        } else if (is_ready) {
            if (is_rx) {
                vsf_linux_fd_rx_ready(sfd, orig);
            } else {
                vsf_linux_fd_tx_ready(sfd, orig);
            }
        } else {
            if (is_rx) {
                vsf_linux_fd_rx_busy(sfd, orig);
            } else {
                vsf_linux_fd_tx_busy(sfd, orig);
            }
        }
    vsf_unprotect_sched(orig);
}

static void __vsf_linux_stream_evthandler(void *param, vsf_stream_evt_t evt)
{
    vsf_linux_fd_t *sfd = param;
    vsf_linux_stream_priv_t *priv = (vsf_linux_stream_priv_t *)sfd->priv;
    vsf_stream_t *stream;

    switch (evt) {
    case VSF_STREAM_ON_RX:
        stream = priv->stream_rx;
        stream->rx.param = NULL;
        stream->rx.evthandler = NULL;
        __vsf_linux_stream_evt(sfd, vsf_protect_sched(), true, true);
        break;
    case VSF_STREAM_ON_TX:
        stream = priv->stream_tx;
        stream->tx.param = NULL;
        stream->tx.evthandler = NULL;
        __vsf_linux_stream_evt(sfd, vsf_protect_sched(), false, true);
        break;
    }
}

static ssize_t __vsf_linux_stream_read(vsf_linux_fd_t *sfd, void *buf, size_t count)
{
    vsf_linux_stream_priv_t *priv = (vsf_linux_stream_priv_t *)sfd->priv;
    vsf_stream_t *stream = priv->stream_rx;
    VSF_LINUX_ASSERT(stream != NULL);
    uint_fast32_t size = count, cursize;
    vsf_protect_t orig;

    while (size > 0) {
        orig = vsf_protect_sched();
        if (!sfd->priv->rxrdy && (0 == vsf_stream_get_rbuf(stream, NULL))) {
            if (vsf_linux_fd_is_block(sfd) && vsf_stream_is_tx_connected(stream)) {
                vsf_trig_t trig;
                vsf_linux_fd_trigger_init(&trig);
                VSF_LINUX_ASSERT((NULL == stream->rx.param) && (NULL == stream->rx.evthandler));
                stream->rx.param = sfd;
                stream->rx.evthandler = __vsf_linux_stream_evthandler;
                vsf_linux_fd_rx_pend(sfd, &trig, orig);
            } else {
                vsf_unprotect_sched(orig);
                goto do_return;
            }
        } else {
            vsf_unprotect_sched(orig);
        }

        cursize = vsf_stream_read(stream, buf, size);
        if ((sfd->fd == STDIN_FILENO) && isatty(sfd->fd)) {
#if VSF_LINUX_USE_TERMIOS == ENABLED
            vsf_linux_process_t *process = vsf_linux_get_cur_process();
            VSF_LINUX_ASSERT(process != NULL);
            struct termios *term = &process->term[STDIN_FILENO];

            if (term->c_lflag & ECHO) {
#else
            if (true) {
#endif
                // TODO: do not use static value here for multi process support
                static int skip_echo = 0;
                char ch;
                for (uint_fast32_t i = 0; i < cursize; i++) {
                    ch = ((char *)buf)[i];
                    switch (ch) {
                    case '\033':skip_echo = 2;                      break;
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
        }

        size -= cursize;
        buf = (uint8_t *)buf + cursize;

        // TODO: need to read all data
        break;
    }

do_return:
    orig = vsf_protect_sched();
    VSF_LINUX_ASSERT(NULL == sfd->priv->rxpend);
    if (!vsf_stream_get_data_size(stream)) {
        __vsf_linux_stream_evt(sfd, orig, true, false);
    } else {
        vsf_linux_fd_rx_trigger(sfd, orig);
    }
    return count - size;
}

static ssize_t __vsf_linux_stream_write(vsf_linux_fd_t *sfd, const void *buf, size_t count)
{
    vsf_linux_stream_priv_t *priv = (vsf_linux_stream_priv_t *)sfd->priv;
    vsf_stream_t *stream = priv->stream_tx;
    VSF_LINUX_ASSERT(stream != NULL);
    uint_fast32_t size = count, cursize;
    vsf_protect_t orig;

    while (size > 0) {
        orig = vsf_protect_sched();
        if (!sfd->priv->txrdy) {
            vsf_trig_t trig;
            vsf_linux_fd_trigger_init(&trig);
            VSF_LINUX_ASSERT((NULL == stream->tx.param) || (NULL == stream->tx.evthandler));
            stream->tx.param = sfd;
            stream->tx.evthandler = __vsf_linux_stream_evthandler;
            vsf_linux_fd_tx_pend(sfd, &trig, orig);
        } else {
            vsf_unprotect_sched(orig);
        }

        cursize = vsf_stream_write(stream, (uint8_t *)buf, size);
        size -= cursize;
        buf = (uint8_t *)buf + cursize;
    }

    orig = vsf_protect_sched();
    VSF_LINUX_ASSERT(NULL == sfd->priv->txpend);
    if (!vsf_stream_get_free_size(stream)) {
        __vsf_linux_stream_evt(sfd, orig, false, false);
    } else {
        vsf_linux_fd_tx_trigger(sfd, orig);
    }
    return count;
}

static int __vsf_linux_stream_close(vsf_linux_fd_t *sfd)
{
    if (!isatty(sfd->fd)) {
        vsf_linux_stream_priv_t *priv = (vsf_linux_stream_priv_t *)sfd->priv;
        if (priv->stream_rx != NULL) {
            vsf_stream_disconnect_rx(priv->stream_rx);
        }
        if (priv->stream_tx != NULL) {
            vsf_stream_disconnect_tx(priv->stream_tx);
        }
    }
    return 0;
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

static void __vsf_linux_tx_stream_init(vsf_linux_fd_t *sfd, vsf_stream_t *stream)
{
    stream->tx.evthandler = NULL;
    stream->tx.param = NULL;
    vsf_stream_connect_tx(stream);

    vsf_protect_t orig = vsf_protect_sched();
    if (vsf_stream_get_free_size(stream)) {
        vsf_linux_fd_tx_ready(sfd, orig);
    } else {
        vsf_unprotect_sched(orig);
    }
}

static void __vsf_linux_rx_stream_init(vsf_linux_fd_t *sfd, vsf_stream_t *stream)
{
    stream->rx.evthandler = NULL;
    stream->rx.param = NULL;
    vsf_stream_connect_rx(stream);

    vsf_protect_t orig = vsf_protect_sched();
    if (vsf_stream_get_data_size(stream)) {
        vsf_linux_fd_tx_ready(sfd, orig);
    } else {
        vsf_unprotect_sched(orig);
    }
}

vsf_linux_fd_t * vsf_linux_stream(vsf_stream_t *stream_rx, vsf_stream_t *stream_tx)
{
    vsf_linux_fd_t *sfd = __vsf_linux_stream(stream_rx, stream_tx);
    if (sfd != NULL) {
        if (stream_rx != NULL) {
            __vsf_linux_rx_stream_init(sfd, stream_rx);
        }
        if (stream_tx != NULL) {
            __vsf_linux_tx_stream_init(sfd, stream_tx);
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

vsf_stream_t * vsf_linux_get_rx_stream(vsf_linux_fd_t *sfd)
{
    if (sfd->op == &__vsf_linux_stream_fdop) {
        return ((vsf_linux_stream_priv_t *)sfd->priv)->stream_rx;
    }
    return NULL;
}

vsf_stream_t * vsf_linux_get_tx_stream(vsf_linux_fd_t *sfd)
{
    if (sfd->op == &__vsf_linux_stream_fdop) {
        return ((vsf_linux_stream_priv_t *)sfd->priv)->stream_tx;
    }
    return NULL;
}

// pipe
static int __vsf_linux_pipe_fcntl(vsf_linux_fd_t *sfd, int cmd, long arg)
{
    return 0;
}

static int __vsf_linux_pipe_close(vsf_linux_fd_t *sfd)
{
    bool is_rx_pipe = &vsf_linux_pipe_rx_fdop == sfd->op;
    if (is_rx_pipe) {
        vsf_linux_pipe_rx_priv_t *priv_rx = (vsf_linux_pipe_rx_priv_t *)sfd->priv;
        VSF_STREAM_READ(priv_rx->stream_rx, NULL, 0xFFFFFFFF);
    }
    int ret = __vsf_linux_stream_close(sfd);
    if (is_rx_pipe) {
        vsf_linux_pipe_rx_priv_t *priv_rx = (vsf_linux_pipe_rx_priv_t *)sfd->priv;
        if (priv_rx->is_to_free_stream) {
            free(priv_rx->stream_rx);
        }
    }
    return ret;
}

vsf_linux_fd_t * vsf_linux_rx_pipe(vsf_queue_stream_t *queue_stream)
{
    vsf_linux_fd_t *sfd_rx = NULL;
    if (vsf_linux_fd_create(&sfd_rx, &vsf_linux_pipe_rx_fdop) >= 0) {
        vsf_linux_pipe_rx_priv_t *priv_rx = (vsf_linux_pipe_rx_priv_t *)sfd_rx->priv;

        if (NULL == queue_stream) {
            queue_stream = malloc(sizeof(vsf_queue_stream_t));
            if (NULL == queue_stream) {
                vsf_linux_fd_delete(sfd_rx->fd);
                return NULL;
            }
            priv_rx->is_to_free_stream = true;
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

        __vsf_linux_rx_stream_init(sfd_rx, priv_rx->stream_rx);
    }
    return sfd_rx;
}

vsf_linux_fd_t * vsf_linux_tx_pipe(vsf_queue_stream_t *queue_stream)
{
    VSF_LINUX_ASSERT(queue_stream != NULL);
    vsf_linux_fd_t *sfd_tx = NULL;
    if (vsf_linux_fd_create(&sfd_tx, &vsf_linux_pipe_tx_fdop) >= 0) {
        vsf_linux_pipe_tx_priv_t *priv_tx = (vsf_linux_pipe_tx_priv_t *)sfd_tx->priv;
        priv_tx->stream_tx = &queue_stream->use_as__vsf_stream_t;

        __vsf_linux_tx_stream_init(sfd_tx, priv_tx->stream_tx);
    }
    return sfd_tx;
}

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

#endif      // VSF_USE_LINUX
