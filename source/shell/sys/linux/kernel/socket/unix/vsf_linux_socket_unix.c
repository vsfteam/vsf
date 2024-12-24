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
// for protected members in vsf_linux_socket_priv_t
#define __VSF_LINUX_SOCKET_CLASS_INHERIT__
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
    union {
        struct {
            vsf_dlist_t child_list;
            vsf_dlist_t accept_list;
            int backlog;
        } listener;
        struct {
            vsf_linux_socket_unix_priv_t *listener;
        } rw;
    };
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern void __vsf_linux_rx_stream_fini(vsf_linux_stream_priv_t *priv_rx);
extern void __vsf_linux_tx_stream_fini(vsf_linux_stream_priv_t *priv_tx);
extern int __vsf_linux_rx_pipe_init(vsf_linux_pipe_priv_t *priv_rx, vsf_queue_stream_t *queue_stream);
extern int __vsf_linux_tx_pipe_init(vsf_linux_pipe_priv_t *priv_tx, vsf_linux_pipe_priv_t *priv_rx);

extern int __vsf_linux_pipe_fcntl(vsf_linux_fd_t *sfd, int cmd, uintptr_t arg);
extern ssize_t __vsf_linux_stream_read(vsf_linux_fd_t *sfd, void *buf, size_t count);
extern ssize_t __vsf_linux_stream_write(vsf_linux_fd_t *sfd, const void *buf, size_t count);
static int __vsf_linux_socket_unix_close(vsf_linux_fd_t *sfd);
extern int __vsf_linux_socket_stat(vsf_linux_fd_t *sfd, struct stat *buf);

static ssize_t __vsf_linux_socket_unix_read(vsf_linux_fd_t *sfd, void *buf, size_t count);
static ssize_t __vsf_linux_socket_unix_write(vsf_linux_fd_t *sfd, const void *buf, size_t count);
static int __vsf_linux_socket_unix_init(vsf_linux_fd_t *sfd);
static int __vsf_linux_socket_unix_socketpair(vsf_linux_fd_t *sfd1, vsf_linux_fd_t *sfd2);
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
        .fn_fcntl       = __vsf_linux_pipe_fcntl,
        .fn_read        = __vsf_linux_socket_unix_read,
        .fn_write       = __vsf_linux_socket_unix_write,
        .fn_close       = __vsf_linux_socket_unix_close,
        .fn_stat        = __vsf_linux_socket_stat,
    },

    .fn_init            = __vsf_linux_socket_unix_init,
    .fn_socketpair      = __vsf_linux_socket_unix_socketpair,
    .fn_fini            = __vsf_linux_socket_unix_fini,
    .fn_connect         = __vsf_linux_socket_unix_connect,
    .fn_listen          = __vsf_linux_socket_unix_listen,
    .fn_accept          = __vsf_linux_socket_unix_accept,
    .fn_bind            = __vsf_linux_socket_unix_bind,
    .fn_getsockopt      = __vsf_linux_socket_unix_getsockopt,
    .fn_setsockopt      = __vsf_linux_socket_unix_setsockopt,
};

/*============================ IMPLEMENTATION ================================*/

static ssize_t __vsf_linux_socket_unix_read(vsf_linux_fd_t *sfd, void *buf, size_t count)
{
    vsf_linux_socket_unix_priv_t *priv = (vsf_linux_socket_unix_priv_t *)sfd->priv;
    struct msghdr *msg = priv->msg_rx;
    ssize_t result = __vsf_linux_stream_read(sfd, buf, count);

    if (priv->target != NULL) {
        struct msghdr *rx_msg = priv->target;
        priv->target = NULL;
        if (msg != NULL) {
            *msg = *rx_msg;

            struct cmsghdr *cmsg = msg->msg_control;
            struct cmsghdr *rx_cmsg = rx_msg->msg_control;
            if ((cmsg != NULL) && (msg->msg_control != NULL)) {
                VSF_LINUX_ASSERT(msg->msg_controllen == rx_msg->msg_controllen);
                VSF_LINUX_ASSERT(cmsg->cmsg_len == rx_cmsg->cmsg_len);
                VSF_LINUX_ASSERT(cmsg->cmsg_level == rx_cmsg->cmsg_level);
                VSF_LINUX_ASSERT(cmsg->cmsg_type == rx_cmsg->cmsg_type);
                memcpy(cmsg, rx_cmsg, cmsg->cmsg_len);

                void *data = CMSG_DATA(cmsg), *rx_data = CMSG_DATA(rx_cmsg);
                int datalen = msg->msg_controllen - sizeof(*msg);
                vsf_linux_process_t *sender_process = priv->sender_process;
                priv->sender_process = NULL;

                switch (cmsg->cmsg_type) {
                case SCM_RIGHTS:
                    while (datalen > 0) {
                        int fd = *(int *)rx_data;
                        extern vsf_linux_fd_t * __vsf_linux_fd_get_ex(vsf_linux_process_t *process, int fd);
                        vsf_linux_fd_t *sfd = __vsf_linux_fd_get_ex(sender_process, fd);
                        if (NULL == sfd) {
                            result = -1;
                            break;
                        }

                        extern int __vsf_linux_fd_create_ex(vsf_linux_process_t *process, vsf_linux_fd_t **sfd,
                            const vsf_linux_fd_op_t *op, int fd_desired, vsf_linux_fd_priv_t *priv);
                        *(int *)data = __vsf_linux_fd_create_ex(NULL, NULL, sfd->op, -1, sfd->priv);
                        data = (void *)((uint8_t *)data + sizeof(int));
                        rx_data = (void *)((uint8_t *)rx_data + sizeof(int));
                        VSF_LINUX_ASSERT(datalen >= sizeof(int));
                        datalen -= sizeof(int);
                    }
                    break;
                default:
                    VSF_LINUX_ASSERT(false);
                    break;
                }
            }
        }
        __free_ex(vsf_linux_resources_process(), rx_msg);
    }

    return result;
}

static ssize_t __vsf_linux_socket_unix_write(vsf_linux_fd_t *sfd, const void *buf, size_t count)
{
    vsf_linux_socket_unix_priv_t *priv = (vsf_linux_socket_unix_priv_t *)sfd->priv;
    const struct msghdr *msg = priv->msg_tx;

    if ((msg->msg_control != NULL) && (msg->msg_controllen > 0)) {
        VSF_LINUX_ASSERT(NULL == priv->target);
        struct msghdr *tx_msg = __malloc_ex(vsf_linux_resources_process(), sizeof(*msg) + msg->msg_controllen);
        if (NULL == priv->target) {
            return -1;
        }

        *tx_msg = *msg;
        tx_msg->msg_control = (void *)&tx_msg[1];
        memcpy(tx_msg->msg_control, msg->msg_control, msg->msg_controllen);
        priv->target = tx_msg;
        priv->sender_process = vsf_linux_get_cur_process();
    }
    return __vsf_linux_stream_write(sfd, buf, count);
}

static int __vsf_linux_socket_unix_close(vsf_linux_fd_t *sfd)
{
    shutdown(sfd->fd, SHUT_RDWR);
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
        return 0;
    } else if (priv->rw.listener != NULL) {
        vsf_protect_t orig = vsf_protect_sched();
        priv->rw.listener->listener.backlog++;
        vsf_dlist_remove(vsf_linux_socket_unix_priv_t, sock_node, &priv->rw.listener->listener.child_list, priv);
        vsf_unprotect_sched(orig);
    }

    bool is_to_close = ((how & SHUT_RDWR) == SHUT_RDWR)
                ||  ((how & SHUT_RD) && (NULL == priv->stream_tx))
                ||  ((how & SHUT_WR) && (NULL == priv->stream_rx));
    if (is_to_close) {
        vsf_protect_t orig = vsf_protect_sched();
        if (priv->pipe_remote != NULL) {
            priv->pipe_remote->pipe_remote = NULL;
            vsf_linux_fd_set_events(&priv->pipe_remote->use_as__vsf_linux_fd_priv_t, POLLIN, orig);
        } else {
            vsf_unprotect_sched(orig);
        }
    }

    if (how & SHUT_RD) {
        __vsf_linux_rx_stream_fini(&priv->use_as__vsf_linux_stream_priv_t);
    }
    if (how & SHUT_WR) {
        __vsf_linux_tx_stream_fini(&priv->use_as__vsf_linux_stream_priv_t);
    }
    return 0;
}

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#endif

static int __vsf_linux_socket_unix_connect(vsf_linux_socket_priv_t *socket_priv, const struct sockaddr *addr, socklen_t addrlen)
{
    vsf_linux_socket_unix_priv_t *priv = (vsf_linux_socket_unix_priv_t *)socket_priv, *priv_remote;
    struct sockaddr_un *addr_un = (struct sockaddr_un *)addr;
    VSF_LINUX_ASSERT(!priv->is_listening);
    VSF_LINUX_ASSERT(!priv->pipe_remote);

    int ret = vsf_linux_fs_get_target(addr_un->sun_path, (void **)&priv->pipe_remote);
    if (ret != 0) {
        return ret;
    }
    priv_remote = (vsf_linux_socket_unix_priv_t *)priv->pipe_remote;
    VSF_LINUX_ASSERT(priv_remote->is_listening);

    if (__vsf_linux_rx_pipe_init(&priv->use_as__vsf_linux_pipe_priv_t, NULL)) {
        return -1;
    }

    vsf_linux_trigger_t trig;
    vsf_linux_trigger_init(&trig);
    priv->trig = &trig;

    // 1. trigger remote for connection request
    VSF_LINUX_ASSERT(!priv_remote->pipe_remote);
    vsf_protect_t orig = vsf_protect_sched();
    vsf_dlist_add_to_tail(vsf_linux_socket_unix_priv_t, sock_node, &priv_remote->listener.accept_list, priv);
    vsf_linux_fd_set_events(&priv_remote->use_as__vsf_linux_fd_priv_t, POLLIN, orig);

    // 2. pend to get remote response and get priv_remote from remote
    if (    (vsf_linux_trigger_pend(&trig, -1) < 0)
        ||  (NULL == priv->pipe_remote)) {
        return -1;
    }
    priv_remote = (vsf_linux_socket_unix_priv_t *)priv->pipe_remote;
    VSF_LINUX_ASSERT(priv_remote->trig != NULL);

    if (__vsf_linux_tx_pipe_init(&priv->use_as__vsf_linux_pipe_priv_t, &priv_remote->use_as__vsf_linux_pipe_priv_t)) {
        priv_remote->pipe_remote = NULL;
        vsf_linux_trigger_signal(priv_remote->trig, 0);
        return -1;
    }

    // 3. trigger remote again to complete connection
    vsf_linux_trigger_signal(priv_remote->trig, 0);
    vsf_linux_fd_set_status(&priv->use_as__vsf_linux_fd_priv_t, POLLOUT, vsf_protect_sched());
    return 0;
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
            priv_remote->pipe_remote = NULL;
            VSF_LINUX_ASSERT(priv_remote->trig != NULL);
            vsf_linux_trigger_signal(priv_remote->trig, 0);
            goto wait_next;
        } else {
            vsf_linux_fd_t *sfd_new = vsf_linux_fd_get(sockfd_new);
            vsf_linux_socket_unix_priv_t *priv_new = (vsf_linux_socket_unix_priv_t *)sfd_new->priv;

            priv_new->rw.listener = priv;
            priv_new->trig = &trig;
            priv_new->pipe_remote = &priv_remote->use_as__vsf_linux_pipe_priv_t;
            priv_remote->pipe_remote = &priv_new->use_as__vsf_linux_pipe_priv_t;

            // 2. init pipe and trigger remote
            if (__vsf_linux_tx_pipe_init(&priv_new->use_as__vsf_linux_pipe_priv_t, &priv_remote->use_as__vsf_linux_pipe_priv_t)) {
            close_sockfd_new_and_fail:
                close(sockfd_new);
                goto fail;
            }
            if (__vsf_linux_rx_pipe_init(&priv_new->use_as__vsf_linux_pipe_priv_t, NULL)) {
                goto close_sockfd_new_and_fail;
            }

            VSF_LINUX_ASSERT(priv_remote->trig != NULL);
            vsf_linux_trigger_signal(priv_remote->trig, 0);

            // 3. pend for connection complete
            if (vsf_linux_trigger_pend(&trig, -1) < 0) {
                goto close_sockfd_new_and_fail;
            }
            if (priv_new->pipe_remote != NULL) {
                // success
                orig = vsf_protect_sched();
                priv->listener.backlog--;
                vsf_dlist_add_to_tail(vsf_linux_socket_unix_priv_t, sock_node, &priv->listener.child_list, priv_new);
                vsf_linux_fd_set_status(sfd_new->priv, POLLOUT, orig);
                return sockfd_new;
            } else {
                goto close_sockfd_new_and_fail;
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

static int __vsf_linux_socket_unix_socketpair(vsf_linux_fd_t *sfd1, vsf_linux_fd_t *sfd2)
{
    vsf_linux_socket_unix_priv_t *priv1 = (vsf_linux_socket_unix_priv_t *)sfd1->priv;
    vsf_linux_socket_unix_priv_t *priv2 = (vsf_linux_socket_unix_priv_t *)sfd2->priv;

    if (    __vsf_linux_rx_pipe_init(&priv1->use_as__vsf_linux_pipe_priv_t, NULL)
        ||  __vsf_linux_rx_pipe_init(&priv2->use_as__vsf_linux_pipe_priv_t, NULL)
        ||  __vsf_linux_tx_pipe_init(&priv1->use_as__vsf_linux_pipe_priv_t, &priv2->use_as__vsf_linux_pipe_priv_t)
        ||  __vsf_linux_tx_pipe_init(&priv2->use_as__vsf_linux_pipe_priv_t, &priv1->use_as__vsf_linux_pipe_priv_t)) {
        return -1;
    }

    return 0;
}

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic pop
#endif

#endif      // VSF_USE_LINUX && VSF_LINUX_USE_SOCKET
