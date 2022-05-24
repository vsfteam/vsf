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
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../include/sys/eventfd.h"
#else
#   include <sys/eventfd.h>
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
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_linux_eventfd_priv_t {
    public_member(
        implement(vsf_linux_fd_priv_t)
    )
    eventfd_t counter;
} vsf_linux_eventfd_priv_t;

/*============================ PROTOTYPES ====================================*/

static int __vsf_linux_fs_fcntl(vsf_linux_fd_t *sfd, int cmd, long arg);
static ssize_t __vsf_linux_fs_read(vsf_linux_fd_t *sfd, void *buf, size_t count);
static ssize_t __vsf_linux_fs_write(vsf_linux_fd_t *sfd, const void *buf, size_t count);
static int __vsf_linux_fs_close(vsf_linux_fd_t *sfd);
static int __vsf_linux_fs_eof(vsf_linux_fd_t *sfd);

static int __vsf_linux_eventfd_fcntl(vsf_linux_fd_t *sfd, int cmd, long arg);
static ssize_t __vsf_linux_eventfd_read(vsf_linux_fd_t *sfd, void *buf, size_t count);
static ssize_t __vsf_linux_eventfd_write(vsf_linux_fd_t *sfd, const void *buf, size_t count);
static int __vsf_linux_eventfd_close(vsf_linux_fd_t *sfd);
static int __vsf_linux_eventfd_eof(vsf_linux_fd_t *sfd);

static int __vsf_linux_stream_fcntl(vsf_linux_fd_t *sfd, int cmd, long arg);
static ssize_t __vsf_linux_stream_read(vsf_linux_fd_t *sfd, void *buf, size_t count);
static ssize_t __vsf_linux_stream_write(vsf_linux_fd_t *sfd, const void *buf, size_t count);
static int __vsf_linux_stream_close(vsf_linux_fd_t *sfd);
static int __vsf_linux_stream_rx_eof(vsf_linux_fd_t *sfd);
static int __vsf_linux_stream_tx_eof(vsf_linux_fd_t *sfd);

static int __vsf_linux_pipe_fcntl(vsf_linux_fd_t *sfd, int cmd, long arg);
static int __vsf_linux_pipe_close(vsf_linux_fd_t *sfd);

/*============================ LOCAL VARIABLES ===============================*/

const vsf_linux_fd_op_t __vsf_linux_fs_fdop = {
    .priv_size          = sizeof(vsf_linux_fs_priv_t),
    .fn_fcntl           = __vsf_linux_fs_fcntl,
    .fn_read            = __vsf_linux_fs_read,
    .fn_write           = __vsf_linux_fs_write,
    .fn_close           = __vsf_linux_fs_close,
    .fn_eof             = __vsf_linux_fs_eof,
};

/*============================ GLOBAL VARIABLES ==============================*/

const vsf_linux_fd_op_t __vsf_linux_eventfd_fdop = {
    .priv_size          = sizeof(vsf_linux_eventfd_priv_t),
    .fn_fcntl           = __vsf_linux_eventfd_fcntl,
    .fn_read            = __vsf_linux_eventfd_read,
    .fn_write           = __vsf_linux_eventfd_write,
    .fn_close           = __vsf_linux_eventfd_close,
    .fn_eof             = __vsf_linux_eventfd_eof,
};

const vsf_linux_fd_op_t __vsf_linux_stream_fdop = {
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
    .fn_eof             = __vsf_linux_stream_rx_eof,
};

const vsf_linux_fd_op_t vsf_linux_pipe_tx_fdop = {
    .priv_size          = sizeof(vsf_linux_pipe_tx_priv_t),
    .fn_fcntl           = __vsf_linux_pipe_fcntl,
    .fn_write           = __vsf_linux_stream_write,
    .fn_close           = __vsf_linux_pipe_close,
    .fn_eof             = __vsf_linux_stream_tx_eof,
};

/*============================ IMPLEMENTATION ================================*/

static vk_file_t * __vsf_linux_fs_get_file_ex(vk_file_t *dir, const char *pathname)
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
        vk_file_read(file, (uint8_t *)buf, count);
        rsize = (int32_t)vsf_eda_get_return_value();
        if (rsize < 0) {
            return result > 0 ? result : EOF;
        } else if (!rsize) {
            break;
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
    __vsf_linux_fs_close_do(priv->file);
    return 0;
}

static int __vsf_linux_fs_eof(vsf_linux_fd_t *sfd)
{
    vsf_linux_fs_priv_t *priv = (vsf_linux_fs_priv_t *)sfd->priv;
    return !(priv->file->size - vk_file_tell(priv->file));
}

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

static int __vsf_linux_eventfd_fcntl(vsf_linux_fd_t *sfd, int cmd, long arg)
{
    return 0;
}

static ssize_t __vsf_linux_eventfd_read(vsf_linux_fd_t *sfd, void *buf, size_t count)
{
    vsf_linux_eventfd_priv_t *priv = (vsf_linux_eventfd_priv_t *)sfd->priv;
    if (count != sizeof(priv->counter)) {
        return -1;
    }

    eventfd_t counter;
    vsf_protect_t orig = vsf_protect_sched();
again:
    if (!priv->counter) {
        if (vsf_linux_fd_is_block(sfd)) {
            vsf_linux_trigger_t trig;
            vsf_linux_trigger_init(&trig);

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

static int __vsf_linux_fd_add_ex(vsf_linux_process_t *process, vsf_linux_fd_t *sfd, int fd_desired)
{
    if (NULL == process) {
        process = vsf_linux_get_cur_process();
    }

    vsf_protect_t orig = vsf_protect_sched();
        if (fd_desired >= 0) {
            if (vsf_bitmap_get(&process->fd_bitmap, fd_desired)) {
                vsf_unprotect_sched(orig);
                return -1;
            }
            vsf_bitmap_set(&process->fd_bitmap, fd_desired);
            sfd->fd = fd_desired;
        } else {
            sfd->fd = vsf_bitmap_ffz(&process->fd_bitmap, VSF_LINUX_CFG_FD_BITMAP_SIZE);
            VSF_LINUX_ASSERT(sfd->fd >= 0);
            vsf_bitmap_set(&process->fd_bitmap, sfd->fd);
        }
        vsf_dlist_add_to_tail(vsf_linux_fd_t, fd_node, &process->fd_list, sfd);
    vsf_unprotect_sched(orig);

    return sfd->fd;
}

int __vsf_linux_fd_create_ex(vsf_linux_process_t *process, vsf_linux_fd_t **sfd,
        const vsf_linux_fd_op_t *op, int fd_desired, vsf_linux_fd_priv_t *priv)
{
    int priv_size = ((op != NULL) && (op->priv_size > 0)) ? op->priv_size : sizeof(vsf_linux_fd_priv_t);
    int ret;
    vsf_linux_fd_t *new_sfd;

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
        new_sfd->priv = __calloc_ex(vsf_linux_resources_process(), 1, priv_size);
        if (!new_sfd->priv) {
            ret = -1;
            goto free_sfd_and_exit;
        }
        new_sfd->priv->ref = 1;
    } else {
        vsf_protect_t orig = vsf_protect_sched();
            priv->ref++;
#if VSF_LINUX_CFG_FD_TRACE == ENABLED
            vsf_trace_debug("%s: priv 0x%p ref %d" VSF_TRACE_CFG_LINEEND,
                __FUNCTION__, priv, priv->ref);
#endif
        vsf_unprotect_sched(orig);
        new_sfd->priv = priv;
    }

    if (sfd != NULL) {
        *sfd = new_sfd;
    }

    ret = __vsf_linux_fd_add_ex(process, new_sfd, fd_desired);
    if (ret < 0) {
free_sfd_and_exit:
        __free_ex(process, new_sfd);
    }
#if VSF_LINUX_CFG_FD_TRACE == ENABLED
    vsf_trace_debug("%s: process 0x%p fd %d priv 0x%p ref %d" VSF_TRACE_CFG_LINEEND,
        __FUNCTION__, NULL == process ? vsf_linux_get_cur_process() : process,
        new_sfd->fd, new_sfd->priv, NULL == new_sfd->priv ? 0 : new_sfd->priv->ref);
#endif
    return ret;
}

int vsf_linux_fd_create(vsf_linux_fd_t **sfd, const vsf_linux_fd_op_t *op)
{
    return __vsf_linux_fd_create_ex(NULL, sfd, op, -1, NULL);
}

void __vsf_linux_fd_delete_ex(vsf_linux_process_t *process, int fd)
{
    if (NULL == process) {
        process = vsf_linux_get_cur_process();
    }
    vsf_linux_fd_t *sfd = __vsf_linux_fd_get_ex(process, fd);

    vsf_protect_t orig = vsf_protect_sched();
        vsf_dlist_remove(vsf_linux_fd_t, fd_node, &process->fd_list, sfd);
        vsf_bitmap_clear(&process->fd_bitmap, sfd->fd);
    vsf_unprotect_sched(orig);

    __free_ex(process, sfd);
}

void vsf_linux_fd_delete(int fd)
{
    __vsf_linux_fd_delete_ex(NULL, fd);
}

bool vsf_linux_fd_is_block(vsf_linux_fd_t *sfd)
{
    vsf_linux_process_t *process;
    if (sfd->priv->flags & O_NONBLOCK) {
        return false;
    }

#if VSF_LINUX_USE_TERMIOS == ENABLED
    if (sfd->fd < 3) {
        process = vsf_linux_get_cur_process();
        VSF_LINUX_ASSERT(process != NULL);
        struct termios *term = &process->term[sfd->fd];
        return term->c_cc[VMIN] > 0;
    }
#endif
    return true;
}

// vsf_linux_fd_[pend/set/clear]_[status/events] MUST be called scheduler protected
short vsf_linux_fd_pend_events(vsf_linux_fd_priv_t *priv, short events, vsf_linux_trigger_t *trig, vsf_protect_t orig)
{
    uint16_t events_triggered = priv->events & events;
    if (events_triggered) {
        priv->events &= ~(events_triggered & ~priv->sticky_events);
        vsf_unprotect_sched(orig);
    } else {
        priv->trigger = trig;
        priv->events_pending = events;
        vsf_unprotect_sched(orig);

        int ret = vsf_linux_trigger_pend(trig, -1);
        if (!ret) {
            events_triggered = priv->events_triggered;
        } else {
            orig = vsf_protect_sched();
                priv->trigger = NULL;
            vsf_unprotect_sched(orig);
        }
        priv->events_triggered = 0;
    }
    return events_triggered;
}

void vsf_linux_fd_set_events(vsf_linux_fd_priv_t *priv, short events, vsf_protect_t orig)
{
    vsf_linux_trigger_t *trig = priv->trigger;
    priv->events |= events;
    if (trig != NULL) {
        priv->events_triggered = events & priv->events_pending;
        priv->events &= ~(priv->events_triggered & ~priv->sticky_events);
        if (priv->events_triggered) {
            priv->trigger = NULL;
            vsf_unprotect_sched(orig);

            vsf_linux_trigger_signal(trig, 0);
            return;
        }
    }
    vsf_unprotect_sched(orig);
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
            priv->events_pending = fds[i].events;
            if (priv->trigger != NULL) {
                VSF_LINUX_ASSERT(priv->trigger == &trig);
            } else {
                priv->trigger = &trig;
            }
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
                priv->trigger = NULL;
                priv->events |= priv->events_triggered;
                priv->events_triggered = 0;
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

static int __vsf_linux_fs_create(const char *pathname, mode_t mode, vk_file_attr_t attr)
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
    vk_file_create(dir, name_tmp, attr);
    if (VSF_ERR_NONE != (vsf_err_t)vsf_eda_get_return_value()) {
        err = -1;
    }
    __vsf_linux_fs_close_do(dir);
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
    char fullpath[MAX_PATH], *name_tmp;
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

int mkdir(const char *pathname, mode_t mode)
{
    if ((NULL == pathname) || ('\0' == *pathname)) {
        return -1;
    }

    return __vsf_linux_fs_create(pathname, mode, VSF_FILE_ATTR_DIRECTORY | VSF_FILE_ATTR_READ | VSF_FILE_ATTR_WRITE);
}

int mkdirat(int dirfd, const char *pathname, mode_t mode)
{
    vk_file_t *dir = __vsf_linux_get_fs_ex(NULL, dirfd);
    if (NULL == dir) {
        errno = ENOTDIR;
        return -1;
    }

    vk_file_create(dir, pathname, VSF_FILE_ATTR_DIRECTORY | VSF_FILE_ATTR_READ | VSF_FILE_ATTR_WRITE);
    if (VSF_ERR_NONE != (vsf_err_t)vsf_eda_get_return_value()) {
        return -1;
    }

    return 0;
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
    return vsf_linux_chdir(process, (char *)pathname);
}

int creat(const char *pathname, mode_t mode)
{
    return open(pathname, O_WRONLY | O_CREAT | O_TRUNC, mode);
}

int vsf_linux_open(vk_file_t *dir, const char *pathname, int flags, mode_t mode)
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
    // check not supported flags here
    if (flags & (O_NONBLOCK)) {
        VSF_LINUX_ASSERT(false);
        return -1;
    }

__open_again:
    file = __vsf_linux_fs_get_file(fullpath);
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
                if (    (stat(path_in_ram, &statbuf) < 0)
                    &&  (mkdir(path_in_ram, mode) < 0)) {
                    goto __exit_failure;
                }
                *end = '/';
            }
            if (flags & O_DIRECTORY) {
                if (mkdir(path_in_ram, mode) < 0) {
                    goto __exit_failure;
                }
            } else {
                if (__vsf_linux_fs_create(path_in_ram, mode, VSF_FILE_ATTR_READ | VSF_FILE_ATTR_WRITE) < 0) {
                    goto __exit_failure;
                }
            }
            flags &= ~O_CREAT;
            free(path_in_ram);
            goto __open_again;
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
        return -1;
    }

    fd = vsf_linux_fd_create(&sfd, &__vsf_linux_fs_fdop);
    if (fd < 0) {
        __vsf_linux_fs_close_do(file);
    } else {
        vsf_linux_fs_priv_t *priv = (vsf_linux_fs_priv_t *)sfd->priv;
        sfd->priv->flags = flags;
        priv->file = file;

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
        if ((flags & O_TRUNC) && (flags & (O_RDWR | O_WRONLY))) {
            ftruncate(fd, 0);
        }
    }
    return fd;
}

int open(const char *pathname, int flags, ...)
{
    va_list ap;
    mode_t mode;

    va_start(ap, flags);
        mode = va_arg(ap, mode_t);
    va_end(ap);
    return vsf_linux_open(NULL, pathname, flags, mode);
}

int openat(int dirfd, const char *pathname, int flags, ...)
{
    va_list ap;
    mode_t mode;

    va_start(ap, flags);
        mode = va_arg(ap, mode_t);
    va_end(ap);

    vk_file_t *dir = __vsf_linux_get_fs_ex(NULL, dirfd);
    if ((NULL == dir) || !(dir->attr & VSF_FILE_ATTR_DIRECTORY)) {
        errno = ENOTDIR;
        return -1;
    }
    return vsf_linux_open(dir, pathname, flags, mode);
}

int __vsf_linux_fd_close_ex(vsf_linux_process_t *process, int fd)
{
    vsf_linux_fd_t *sfd = __vsf_linux_fd_get_ex(process, fd);
    if (!sfd) { return -1; }

    vsf_linux_fd_priv_t *priv = sfd->priv;
    vsf_protect_t orig = vsf_protect_sched();
        bool is_to_close = --priv->ref == 0;
#if VSF_LINUX_CFG_FD_TRACE == ENABLED
        vsf_trace_debug("%s: process 0x%p fd %d priv 0x%p ref %d" VSF_TRACE_CFG_LINEEND,
            __FUNCTION__, vsf_linux_get_cur_process(), sfd->fd, priv, priv->ref);
#endif
    vsf_unprotect_sched(orig);

    int err = 0;
    if (is_to_close) {
        err = sfd->op->fn_close(sfd);
        // priv of fd does not belong to the process
        __free_ex(vsf_linux_resources_process(), sfd->priv);
    }
    __vsf_linux_fd_delete_ex(process, fd);
    return err;
}

int close(int fd)
{
    return __vsf_linux_fd_close_ex(NULL, fd);
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
    case F_DUPFD:
        return __vsf_linux_fd_create_ex(NULL, NULL, sfd->op, arg, sfd->priv);
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
        return sfd->priv->flags;
        break;
    case F_SETFL: {
            long tmp_arg = arg;
            arg = arg ^ sfd->priv->flags;
            sfd->priv->flags = tmp_arg;
        }
        break;
    }

    return sfd->op->fn_fcntl(sfd, cmd, arg);
}

ssize_t read(int fd, void *buf, size_t count)
{
    vsf_linux_fd_t *sfd = vsf_linux_fd_get(fd);
    if (!sfd || (sfd->priv->flags & O_WRONLY)) { return -1; }
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

ssize_t pread(int fd, void *buf, size_t count, off_t offset)
{
    off_t orig = lseek(fd, 0, SEEK_CUR);
    lseek(fd, offset, SEEK_SET);
    ssize_t size = read(fd, buf, count);
    lseek(fd, orig, SEEK_SET);
    return size;
}

ssize_t preadv(int fd, const struct iovec *iov, int iovcnt, off_t offset)
{
    ssize_t size = 0, cur_size;
    for (int i = 0; i < iovcnt; i++) {
        cur_size = pread(fd, iov->iov_base, iov->iov_len, offset);
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

ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset)
{
    off_t orig = lseek(fd, 0, SEEK_CUR);
    lseek(fd, offset, SEEK_SET);
    ssize_t size = write(fd, buf, count);
    lseek(fd, orig, SEEK_SET);
    return size;
}

ssize_t pwritev(int fd, const struct iovec *iov, int iovcnt, off_t offset)
{
    ssize_t size = 0, cur_size;
    for (int i = 0; i < iovcnt; i++) {
        cur_size = pwrite(fd, iov->iov_base, iov->iov_len, offset);
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

off_t lseek(int fd, off_t offset, int whence)
{
    vsf_linux_fd_t *sfd = vsf_linux_fd_get(fd);
    VSF_LINUX_ASSERT(sfd->op == &__vsf_linux_fs_fdop);
    vsf_linux_fs_priv_t *priv = (vsf_linux_fs_priv_t *)sfd->priv;

    vk_file_seek(priv->file, offset, whence);
    return vk_file_tell(priv->file);
}

int futimes(int fd, const struct timeval tv[2])
{
    return 0;
}

int utimes(const char *filename, const struct timeval times[2])
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
    VSF_LINUX_ASSERT(&__vsf_linux_fs_fdop == sfd->op);
    vsf_linux_fs_priv_t *priv = (vsf_linux_fs_priv_t *)sfd->priv;
    vk_file_t *file = priv->file;

    vk_file_setsize(file, length);
    return vsf_eda_get_return_value();
}

int truncate(const char *path, off_t length)
{
    char fullpath[MAX_PATH];
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

int fstat(int fd, struct stat *buf)
{
    vsf_linux_fd_t *sfd = vsf_linux_fd_get(fd);

    memset(buf, 0, sizeof(*buf));
    if (&__vsf_linux_fs_fdop == sfd->op) {
        vsf_linux_fs_priv_t *priv = (vsf_linux_fs_priv_t *)sfd->priv;
        vk_file_t *file = priv->file;

        if (file->attr & VSF_FILE_ATTR_DIRECTORY) {
            buf->st_mode = S_IFDIR;
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
    } else if ( (&vsf_linux_pipe_rx_fdop == sfd->op)
            ||  (&vsf_linux_pipe_tx_fdop == sfd->op)) {
        buf->st_mode = S_IFIFO;
        return 0;
#if VSF_LINUX_USE_SOCKET == ENABLED
    } else if (
#   if VSF_LINUX_SOCKET_USE_UNIX == ENABLED
                (&vsf_linux_socket_unix_op.fdop == sfd->op)
#   endif
#   if VSF_LINUX_SOCKET_USE_UNIX == ENABLED && VSF_LINUX_SOCKET_USE_INET == ENABLED
            ||
#   endif
#   if VSF_LINUX_SOCKET_USE_INET == ENABLED
                (&vsf_linux_socket_inet_op.fdop == sfd->op)
#   endif
        ) {
        buf->st_mode = S_IFSOCK;
        return 0;
#endif
    }
    return -1;
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

int fstatat(int dirfd, const char *pathname, struct stat *buf, int flags)
{
    if ((NULL == pathname) || ('\0' == *pathname)) {
        return -1;
    }

    int fd = openat(dirfd, pathname, 0);
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
    return -1;
}

ssize_t readlink(const char *pathname, char *buf, size_t bufsiz)
{
    return -1;
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

struct dirent * readdir(DIR *dir)
{
    vsf_linux_fs_priv_t *priv = (vsf_linux_fs_priv_t *)dir->priv;
    vk_file_t *file = priv->file, *child;
    off_t offset = vk_file_tell(file);

    if (priv->child != NULL) {
        vk_file_close(priv->child);
        priv->child = NULL;
    }

    vk_file_open(file, NULL, &priv->child);
    if (NULL == priv->child) {
        return NULL;
    }
    priv->dir.d_ino++;

    child = priv->child;
    priv->dir.d_name = child->name;
    priv->dir.d_off = offset;
    priv->dir.d_reclen = sizeof(struct dirent);
    priv->dir.d_type = child->attr & VSF_FILE_ATTR_DIRECTORY ? DT_DIR :
                child->attr & VSF_FILE_ATTR_EXECUTE ? DT_EXE : DT_REG;
    return &priv->dir;
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

        n = malloc(sizeof(struct dirent) + strlen(d->d_name) + 1);
        if (NULL == n) {
            break;
        }
        *n = *d;
        n->d_name = (char *)&n[1];
        strcpy(n->d_name, d->d_name);

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
    return vsf_linux_fs_bind_target_ex(pathname, buffer,
        (vsf_peda_evthandler_t)vsf_peda_func(__vk_vfs_buffer_read),
        (vsf_peda_evthandler_t)vsf_peda_func(__vk_vfs_buffer_write),
        feature, size);
}

// stream
static int __vsf_linux_stream_fcntl(vsf_linux_fd_t *sfd, int cmd, long arg)
{
    return 0;
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

static ssize_t __vsf_linux_stream_read(vsf_linux_fd_t *sfd, void *buf, size_t count)
{
    vsf_linux_stream_priv_t *priv = (vsf_linux_stream_priv_t *)sfd->priv;
    vsf_stream_t *stream = priv->stream_rx;
    VSF_LINUX_ASSERT(stream != NULL);
    uint_fast32_t size = count, cursize;
    vsf_protect_t orig;

    while (size > 0) {
        orig = vsf_protect_sched();
        if (!vsf_linux_fd_get_status(&priv->use_as__vsf_linux_fd_priv_t, POLLIN) && (0 == vsf_stream_get_rbuf(stream, NULL))) {
            if (vsf_linux_fd_is_block(sfd) && vsf_stream_is_tx_connected(stream)) {
                vsf_linux_trigger_t trig;
                vsf_linux_trigger_init(&trig);

                if (!vsf_linux_fd_pend_events(&priv->use_as__vsf_linux_fd_priv_t, POLLIN, &trig, orig)) {
                    // triggered by signal
                    return -1;
                }
            } else {
                vsf_unprotect_sched(orig);
                goto do_return;
            }
        } else {
            vsf_unprotect_sched(orig);
        }

        cursize = vsf_stream_read(stream, buf, size);
        if ((sfd->fd == STDIN_FILENO) && vsf_linux_is_stdio_stream(sfd->fd)) {
#if VSF_LINUX_USE_TERMIOS == ENABLED
            vsf_linux_process_t *process = vsf_linux_get_cur_process();
            VSF_LINUX_ASSERT(process != NULL);
            struct termios *term = &process->term[STDIN_FILENO];

            if (term->c_lflag & ECHO) {
#else
            if (true) {
#endif
                // TODO: do not use static value here for multi process support
                static char esc_type = '\0';
                char ch;
                for (uint_fast32_t i = 0; i < cursize; i++) {
                    ch = ((char *)buf)[i];
                    switch (esc_type) {
                    case '\033':
                        if ((ch == '[') || (ch == 'O')){
                            esc_type = ch;
                        } else {
                            esc_type = '\0';
                            goto char_input;
                        }
                        break;
                    case '[':
                        if (    ((ch >= 'a') && (ch <= 'z'))
                            ||  ((ch >= 'A') && (ch <= 'Z'))
                            ||  (ch == '~')) {
                            esc_type = '\0';
                        }
                        break;
                    case 'O':
                        esc_type = '\0';
                        break;
                    case '\0':
                    char_input:
                        switch (ch) {
                        case '\033':esc_type = '\033';                  break;
                        case 0x7F:
                        case '\b':  write(STDOUT_FILENO, "\b \b", 3);   break;
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
        buf = (uint8_t *)buf + cursize;

        // TODO: need to read all data
        break;
    }

do_return:
    orig = vsf_protect_sched();
    VSF_LINUX_ASSERT((NULL == sfd->priv->trigger) || !(sfd->priv->events_pending & POLLIN));
    if (!vsf_stream_get_data_size(stream)) {
        __vsf_linux_stream_evt(priv, orig, POLLIN, false);
    } else {
        vsf_linux_fd_set_events(&priv->use_as__vsf_linux_fd_priv_t, POLLIN, orig);
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
        if (!vsf_linux_fd_get_status(&priv->use_as__vsf_linux_fd_priv_t, POLLOUT)) {
            vsf_linux_trigger_t trig;
            vsf_linux_trigger_init(&trig);

            if (!vsf_linux_fd_pend_events(&priv->use_as__vsf_linux_fd_priv_t, POLLOUT, &trig, orig)) {
                // triggered by signal
                return -1;
            }
        } else {
            vsf_unprotect_sched(orig);
        }

        cursize = vsf_stream_write(stream, (uint8_t *)buf, size);
        size -= cursize;
        buf = (uint8_t *)buf + cursize;
    }

    orig = vsf_protect_sched();
    VSF_LINUX_ASSERT((NULL == sfd->priv->trigger) || !(sfd->priv->events_pending & POLLOUT));
    if (!vsf_stream_get_free_size(stream)) {
        __vsf_linux_stream_evt(priv, orig, POLLOUT, false);
    } else {
        vsf_linux_fd_set_events(&priv->use_as__vsf_linux_fd_priv_t, POLLOUT, orig);
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

static int __vsf_linux_stream_rx_eof(vsf_linux_fd_t *sfd)
{
    vsf_linux_stream_priv_t *priv = (vsf_linux_stream_priv_t *)sfd->priv;
    return !vsf_stream_is_tx_connected(priv->stream_rx);
}

static int __vsf_linux_stream_tx_eof(vsf_linux_fd_t *sfd)
{
    vsf_linux_stream_priv_t *priv = (vsf_linux_stream_priv_t *)sfd->priv;
    return !vsf_stream_is_rx_connected(priv->stream_rx);
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

void __vsf_linux_tx_stream_init(vsf_linux_fd_t *sfd, vsf_stream_t *stream)
{
    stream->tx.evthandler = __vsf_linux_stream_evthandler;
    stream->tx.param = sfd->priv;
    vsf_stream_connect_tx(stream);

    vsf_protect_t orig = vsf_protect_sched();
    if (vsf_stream_get_free_size(stream)) {
        vsf_linux_fd_set_status(sfd->priv, POLLOUT, orig);
    } else {
        vsf_unprotect_sched(orig);
    }
}

void __vsf_linux_rx_stream_init(vsf_linux_fd_t *sfd, vsf_stream_t *stream)
{
    stream->rx.evthandler = __vsf_linux_stream_evthandler;
    stream->rx.param = sfd->priv;
    vsf_stream_connect_rx(stream);

    vsf_protect_t orig = vsf_protect_sched();
    if (vsf_stream_get_data_size(stream)) {
        vsf_linux_fd_set_status(sfd->priv, POLLIN, orig);
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
    if (    (sfd->op == &__vsf_linux_stream_fdop)
        ||  (sfd->op == &vsf_linux_pipe_rx_fdop)) {
        return ((vsf_linux_stream_priv_t *)sfd->priv)->stream_rx;
    }
    return NULL;
}

vsf_stream_t * vsf_linux_get_tx_stream(vsf_linux_fd_t *sfd)
{
    if (    (sfd->op == &__vsf_linux_stream_fdop)
        ||  (sfd->op == &vsf_linux_pipe_tx_fdop)) {
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
    union {
        vsf_linux_pipe_rx_priv_t *rx;
        vsf_linux_pipe_tx_priv_t *tx;
        void *ptr;
    } priv;
    priv.ptr = sfd->priv;
    vsf_protect_t orig = vsf_protect_sched();
    if (is_rx_pipe) {
        if (priv.rx->is_to_free_stream) {
            if (priv.rx->pipe_tx_priv != NULL) {
                priv.rx->pipe_tx_priv->stream_tx = NULL;
                priv.rx->pipe_tx_priv->pipe_rx_priv = NULL;
            }
            vsf_unprotect_sched(orig);

            // pipe internals does not belong to process
            __free_ex(vsf_linux_resources_process(), priv.rx->stream_rx);
        } else {
            if (priv.rx->pipe_tx_priv != NULL) {
                priv.rx->pipe_tx_priv->pipe_rx_priv = NULL;
            }
            vsf_unprotect_sched(orig);
        }
    } else {
        // pipe_tx is closed, set stick_events in pipe_rx with POLLLIN
        if (priv.tx->pipe_rx_priv != NULL) {
            priv.tx->pipe_rx_priv->sticky_events = POLLIN;
            priv.tx->pipe_rx_priv->pipe_tx_priv = NULL;
        }
        vsf_unprotect_sched(orig);
    }
    return ret;
}

vsf_linux_fd_t * vsf_linux_rx_pipe(vsf_queue_stream_t *queue_stream)
{
    vsf_linux_fd_t *sfd_rx = NULL;
    if (vsf_linux_fd_create(&sfd_rx, &vsf_linux_pipe_rx_fdop) >= 0) {
        vsf_linux_pipe_rx_priv_t *priv_rx = (vsf_linux_pipe_rx_priv_t *)sfd_rx->priv;

        if (NULL == queue_stream) {
            // pipe internals does not belong to process
            queue_stream = __malloc_ex(vsf_linux_resources_process(), sizeof(vsf_queue_stream_t));
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

vsf_linux_fd_t * vsf_linux_tx_pipe(vsf_linux_pipe_rx_priv_t *priv_rx)
{
    VSF_LINUX_ASSERT(priv_rx != NULL);
    vsf_stream_t *stream_rx = priv_rx->stream_rx;
    VSF_LINUX_ASSERT(stream_rx != NULL);
    vsf_linux_fd_t *sfd_tx = NULL;
    if (vsf_linux_fd_create(&sfd_tx, &vsf_linux_pipe_tx_fdop) >= 0) {
        vsf_linux_pipe_tx_priv_t *priv_tx = (vsf_linux_pipe_tx_priv_t *)sfd_tx->priv;
        priv_tx->stream_tx = stream_rx;

        __vsf_linux_tx_stream_init(sfd_tx, priv_tx->stream_tx);

        priv_rx->pipe_tx_priv = priv_tx;
        priv_tx->pipe_rx_priv = priv_rx;
    }
    return sfd_tx;
}

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

#endif      // VSF_USE_LINUX
