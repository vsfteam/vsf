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

#if VSF_USE_LINUX == ENABLED && VSF_LINUX_USE_SOCKET == ENABLED && VSF_LINUX_SOCKET_USE_UNIX == ENABLED

// for linux_trigger
#define __VSF_LINUX_CLASS_INHERIT__
#define __VSF_LINUX_FS_CLASS_INHERIT__
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../../../include/unistd.h"
#   include "../../../include/errno.h"
#   include "../../../include/fcntl.h"
#   include "../../../include/sys/un.h"
#   include "../../../include/sys/stat.h"
#   include "../../../include/poll.h"
#else
#   include <unistd.h>
#   include <errno.h>
#   include <fcntl.h>
#   include <sys/un.h>
#   include <sys/stat.h>
#   include <poll.h>
#endif
#include "../vsf_linux_socket.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_linux_socket_unix_priv_t vsf_linux_socket_unix_priv_t;
struct vsf_linux_socket_unix_priv_t {
    implement(vsf_linux_socket_priv_t)

    union {
        struct {
            uint32_t is_listening : 1;
        };
        uint32_t sock_flags;
    };

    vsf_linux_trigger_t *trig;
    vsf_dlist_node_t sock_node;
    vsf_linux_socket_unix_priv_t *remote;
    union {
        struct {
            vsf_dlist_t child_list;
            vsf_dlist_t accept_list;
            int backlog;
        } listener;
        struct {
            vsf_linux_socket_unix_priv_t *listener;
            vsf_linux_fd_t *sfd_rx;
            vsf_linux_fd_t *sfd_tx;
        } rw;
    };
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

static int __vsf_linux_socket_unix_fcntl(vsf_linux_fd_t *sfd, int cmd, long arg);
static ssize_t __vsf_linux_socket_unix_read(vsf_linux_fd_t *sfd, void *buf, size_t count);
static ssize_t __vsf_linux_socket_unix_write(vsf_linux_fd_t *sfd, const void *buf, size_t count);
static int __vsf_linux_socket_unix_close(vsf_linux_fd_t *sfd);

static int __vsf_linux_socket_unix_init(vsf_linux_fd_t *sfd);
static int __vsf_linux_socket_unix_fini(vsf_linux_socket_priv_t *socket_priv, int how);
static int __vsf_linux_socket_unix_connect(vsf_linux_socket_priv_t *socket_priv, const struct sockaddr *addr, socklen_t addrlen);
static int __vsf_linux_socket_unix_listen(vsf_linux_socket_priv_t *socket_priv, int backlog);
static int __vsf_linux_socket_unix_accept(vsf_linux_socket_priv_t *socket_priv, struct sockaddr *addr, socklen_t *addr_len);
static int __vsf_linux_socket_unix_bind(vsf_linux_socket_priv_t *socket_priv, const struct sockaddr *addr, socklen_t addrlen);
static int __vsf_linux_socket_unix_getsockopt(vsf_linux_socket_priv_t *socket_priv, int level, int optname, void *optval, socklen_t *optlen);
static int __vsf_linux_socket_unix_setsockopt(vsf_linux_socket_priv_t *socket_priv, int level, int optname,const void *optval, socklen_t optlen);

/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/

const vsf_linux_socket_op_t vsf_linux_socket_unix_op = {
    .fdop               = {
        .priv_size      = sizeof(vsf_linux_socket_unix_priv_t),
        .fn_fcntl       = __vsf_linux_socket_unix_fcntl,
        .fn_read        = __vsf_linux_socket_unix_read,
        .fn_write       = __vsf_linux_socket_unix_write,
        .fn_close       = __vsf_linux_socket_unix_close,
    },

    .fn_init            = __vsf_linux_socket_unix_init,
    .fn_fini            = __vsf_linux_socket_unix_fini,
    .fn_connect         = __vsf_linux_socket_unix_connect,
    .fn_listen          = __vsf_linux_socket_unix_listen,
    .fn_accept          = __vsf_linux_socket_unix_accept,
    .fn_bind            = __vsf_linux_socket_unix_bind,
    .fn_getsockopt      = __vsf_linux_socket_unix_getsockopt,
    .fn_setsockopt      = __vsf_linux_socket_unix_setsockopt,
};

/*============================ IMPLEMENTATION ================================*/

static int __vsf_linux_socket_unix_fcntl(vsf_linux_fd_t *sfd, int cmd, long arg)
{
    vsf_linux_socket_unix_priv_t *priv = (vsf_linux_socket_unix_priv_t *)sfd->priv;
    switch (cmd) {
    case F_SETFL:
        if (arg & O_NONBLOCK) {
            if (priv->rw.sfd_rx != NULL) {
                fcntl(priv->rw.sfd_rx->fd, F_SETFL, O_NONBLOCK);
            }
            if (priv->rw.sfd_tx != NULL) {
                fcntl(priv->rw.sfd_tx->fd, F_SETFL, O_NONBLOCK);
            }
        }
        return 0;
    default:
        return -1;
    }
}

static ssize_t __vsf_linux_socket_unix_read(vsf_linux_fd_t *sfd, void *buf, size_t count)
{
    vsf_linux_socket_unix_priv_t *priv = (vsf_linux_socket_unix_priv_t *)sfd->priv;
    VSF_LINUX_ASSERT(!priv->is_listening);

    // read socket returns 0 on disconnect
    if ((NULL == priv->rw.sfd_rx) || (NULL == priv->remote)) {
        return 0;
    }

    return read(priv->rw.sfd_rx->fd, buf, count);
}

static ssize_t __vsf_linux_socket_unix_write(vsf_linux_fd_t *sfd, const void *buf, size_t count)
{
    vsf_linux_socket_unix_priv_t *priv = (vsf_linux_socket_unix_priv_t *)sfd->priv;
    VSF_LINUX_ASSERT(!priv->is_listening);

    if ((NULL == priv->rw.sfd_tx) || (NULL == priv->remote)) {
        return -1;
    }

    return write(priv->rw.sfd_tx->fd, buf, count);
}

static int __vsf_linux_socket_unix_close(vsf_linux_fd_t *sfd)
{
    shutdown(sfd->fd, 0);
    return 0;
}

static int __vsf_linux_socket_unix_init(vsf_linux_fd_t *sfd)
{
    return 0;
}

static int __vsf_linux_socket_unix_fini(vsf_linux_socket_priv_t *socket_priv, int how)
{
    vsf_linux_socket_unix_priv_t *priv = (vsf_linux_socket_unix_priv_t *)socket_priv;

    if (priv->is_listening) {
        // TODO: close all child socket?
    } else if (priv->rw.listener != NULL) {
        vsf_protect_t orig = vsf_protect_sched();
        priv->rw.listener->listener.backlog++;
        vsf_dlist_remove(vsf_linux_socket_unix_priv_t, sock_node, &priv->rw.listener->listener.child_list, priv);
        vsf_unprotect_sched(orig);
    }

    vsf_protect_t orig = vsf_protect_sched();
    if (priv->remote != NULL) {
        priv->remote->remote = NULL;
        vsf_linux_fd_set_events(&priv->remote->use_as__vsf_linux_fd_priv_t, POLLIN, orig);
    } else {
        vsf_unprotect_sched(orig);
    }

    if (priv->rw.sfd_rx != NULL) {
        close(priv->rw.sfd_rx->fd);
        priv->rw.sfd_rx = NULL;
    }
    if (priv->rw.sfd_tx != NULL) {
        close(priv->rw.sfd_tx->fd);
        priv->rw.sfd_tx = NULL;
    }
    return 0;
}

static void __vsf_linux_socket_unix_pipe_on_rx_evt(vsf_linux_stream_priv_t *priv_rx, vsf_protect_t orig, short event, bool is_ready)
{
    vsf_linux_fd_priv_t *priv = ((vsf_linux_pipe_rx_priv_t *)priv_rx)->target;
    if (is_ready) {
        vsf_linux_fd_set_status(priv, event, orig);
    } else {
        vsf_linux_fd_clear_status(priv, event, orig);
    }
}

static int __vsf_linux_socket_unix_connect(vsf_linux_socket_priv_t *socket_priv, const struct sockaddr *addr, socklen_t addrlen)
{
    vsf_linux_socket_unix_priv_t *priv = (vsf_linux_socket_unix_priv_t *)socket_priv;
    struct sockaddr_un *addr_un = (struct sockaddr_un *)addr;
    VSF_LINUX_ASSERT(!priv->is_listening);
    VSF_LINUX_ASSERT(!priv->remote);

    int ret = vsf_linux_fs_get_target(addr_un->sun_path, (void **)&priv->remote);
    if (ret != 0) {
        return ret;
    }
    VSF_LINUX_ASSERT(priv->remote->is_listening);

    vsf_linux_fd_t *sfd_rx = vsf_linux_rx_pipe(NULL);
    if (NULL == sfd_rx) {
        return -1;
    }
    fcntl(sfd_rx->fd, F_SETFL, priv->flags);
    vsf_linux_pipe_rx_priv_t *priv_rx = (vsf_linux_pipe_rx_priv_t *)sfd_rx->priv;
    priv_rx->on_evt = __vsf_linux_socket_unix_pipe_on_rx_evt;
    priv_rx->target = priv;

    vsf_linux_trigger_t trig;
    vsf_linux_trigger_init(&trig);
    priv->trig = &trig;

    // 1. setup sfd_rx and trigger remote for conection request
    priv->rw.sfd_rx = sfd_rx;
    VSF_LINUX_ASSERT(!priv->remote->remote);
    vsf_protect_t orig = vsf_protect_sched();
    vsf_dlist_add_to_tail(vsf_linux_socket_unix_priv_t, sock_node, &priv->remote->listener.accept_list, priv);
    vsf_linux_fd_set_events(&priv->remote->use_as__vsf_linux_fd_priv_t, POLLIN, orig);

    // 2. pend to get remote response and get sfd_rx from remote
    if (    (vsf_linux_trigger_pend(&trig, -1) < 0)
        ||  (NULL == priv->remote)) {
        goto delete_sfd_rx_and_fail;
    }
    VSF_LINUX_ASSERT(priv->remote->trig != NULL);

    vsf_linux_fd_t *sfd_tx = vsf_linux_tx_pipe((vsf_linux_pipe_rx_priv_t *)priv->rw.sfd_tx->priv);
    if (NULL == sfd_tx) {
        priv->remote->remote = NULL;
        vsf_linux_trigger_signal(priv->remote->trig, 0);
        goto delete_sfd_rx_and_fail;
    }
    fcntl(sfd_tx->fd, F_SETFL, priv->flags);
    priv->rw.sfd_tx = sfd_tx;

    // 3. trigger remote again to complete conection
    vsf_linux_trigger_signal(priv->remote->trig, 0);
    vsf_linux_fd_set_status(&priv->use_as__vsf_linux_fd_priv_t, POLLOUT, vsf_protect_sched());
    return 0;

delete_sfd_rx_and_fail:
    vsf_linux_fd_delete(sfd_rx->fd);
    return -1;
}

static int __vsf_linux_socket_unix_listen(vsf_linux_socket_priv_t *socket_priv, int backlog)
{
    vsf_linux_socket_unix_priv_t *priv = (vsf_linux_socket_unix_priv_t *)socket_priv;
    VSF_LINUX_ASSERT(!priv->is_listening);

    priv->is_listening = true;
    priv->listener.backlog = backlog;
    return 0;
}

static int __vsf_linux_socket_unix_accept(vsf_linux_socket_priv_t *socket_priv, struct sockaddr *addr, socklen_t *addr_len)
{
    vsf_linux_socket_unix_priv_t *priv = (vsf_linux_socket_unix_priv_t *)socket_priv;
    VSF_LINUX_ASSERT(priv->is_listening);

    if (priv->listener.backlog > 0) {
        vsf_linux_socket_unix_priv_t *priv_remote;
        vsf_protect_t orig;

        vsf_linux_trigger_t trig;
        vsf_linux_trigger_init(&trig);

    wait_next:
        // 1. wait for connection request
        orig = vsf_protect_sched();
        vsf_dlist_remove_head(vsf_linux_socket_unix_priv_t, sock_node, &priv->listener.accept_list, priv_remote);
        if (NULL == priv_remote) {
            if (!vsf_linux_fd_pend_events(&priv->use_as__vsf_linux_fd_priv_t, POLLIN, &trig, orig)) {
                // triggered by signal
                return -1;
            }

            orig = vsf_protect_sched();
            vsf_dlist_remove_head(vsf_linux_socket_unix_priv_t, sock_node, &priv->listener.accept_list, priv_remote);
            vsf_unprotect_sched(orig);
        } else {
            vsf_unprotect_sched(orig);
        }
        VSF_LINUX_ASSERT(priv_remote != NULL);

        int sockfd_new = socket(socket_priv->domain, socket_priv->type, socket_priv->protocol);
        if (sockfd_new < 0) {
        fail:
            priv_remote->remote = NULL;
            VSF_LINUX_ASSERT(priv_remote->trig != NULL);
            vsf_linux_trigger_signal(priv_remote->trig, 0);
            goto wait_next;
        } else {
            vsf_linux_fd_t *sfd_new = vsf_linux_fd_get(sockfd_new);
            vsf_linux_socket_unix_priv_t *priv_new = (vsf_linux_socket_unix_priv_t *)sfd_new->priv;

            priv_new->rw.listener = priv;
            priv_new->trig = &trig;
            priv_new->remote = priv_remote;
            priv_remote->remote = priv_new;

            // 2. provide sfd_rx and trigger remote
            vsf_linux_fd_t *sfd_tx = vsf_linux_tx_pipe((vsf_linux_pipe_rx_priv_t *)priv_remote->rw.sfd_rx->priv);
            if (NULL == sfd_tx) {
            close_sockfd_new_and_fail:
                close(sockfd_new);
                goto fail;
            }
            vsf_linux_fd_t *sfd_rx = vsf_linux_rx_pipe(NULL);
            if (NULL == sfd_rx) {
            free_sfd_tx_and_close_sockfd_new_and_fail:
                vsf_linux_fd_delete(sfd_tx->fd);
                goto close_sockfd_new_and_fail;
            }
            vsf_linux_pipe_rx_priv_t *priv_rx = (vsf_linux_pipe_rx_priv_t *)sfd_rx->priv;
            priv_rx->on_evt = __vsf_linux_socket_unix_pipe_on_rx_evt;
            priv_rx->target = sfd_new->priv;

            // rw.sfd_tx is free in priv_remote
            priv_remote->rw.sfd_tx = sfd_rx;
            VSF_LINUX_ASSERT(priv_remote->trig != NULL);
            vsf_linux_trigger_signal(priv_remote->trig, 0);

            // 3. pend for connection complete
            if (vsf_linux_trigger_pend(&trig, -1) < 0) {
                vsf_linux_fd_delete(sfd_rx->fd);
                goto free_sfd_tx_and_close_sockfd_new_and_fail;
            }
            if (priv_new->remote != NULL) {
                // success
                priv_new->rw.sfd_rx = sfd_rx;
                priv_new->rw.sfd_tx = sfd_tx;

                orig = vsf_protect_sched();
                priv->listener.backlog--;
                vsf_dlist_add_to_tail(vsf_linux_socket_unix_priv_t, sock_node, &priv->listener.child_list, priv_new);
                vsf_linux_fd_set_status(sfd_new->priv, POLLOUT, orig);
                return sockfd_new;
            } else {
                vsf_linux_fd_delete(sfd_rx->fd);
                goto free_sfd_tx_and_close_sockfd_new_and_fail;
            }
        }
    }
    return -1;
}

static int __vsf_linux_socket_unix_bind(vsf_linux_socket_priv_t *socket_priv, const struct sockaddr *addr, socklen_t addrlen)
{
    struct sockaddr_un *addr_un = (struct sockaddr_un *)addr;
    int fd = open(addr_un->sun_path, O_CREAT, 0777);
    if (fd < 0) {
        return -1;
    }

    if (vsf_linux_fd_bind_target(fd, socket_priv, NULL, NULL) != 0) {
        close(fd);
        return -1;
    }

    vsf_linux_fd_add_feature(fd, VSF_FILE_ATTR_SOCK);
    close(fd);
    return 0;
}

static int __vsf_linux_socket_unix_getsockopt(vsf_linux_socket_priv_t *socket_priv, int level, int optname, void *optval, socklen_t *optlen)
{
    return 0;
}

static int __vsf_linux_socket_unix_setsockopt(vsf_linux_socket_priv_t *socket_priv, int level, int optname,const void *optval, socklen_t optlen)
{
    return 0;
}

#endif      // VSF_USE_LINUX && VSF_LINUX_USE_SOCKET
