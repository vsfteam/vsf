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

#if VSF_USE_LINUX == ENABLED && VSF_LINUX_USE_SOCKET == ENABLED

#define __VSF_LINUX_CLASS_INHERIT__
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../../../include/unistd.h"
#   include "../../../include/errno.h"
#else
#   include <unistd.h>
#   include <errno.h>
#endif
#include "../vsf_linux_socket.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_linux_socket_unix_priv_t {
    implement(vsf_linux_socket_priv_t)

    union {
        struct {
            uint32_t is_listening : 1;
        };
        uint32_t flags;
    };

    union {
        struct {
            int dummy;
        } listener;
        struct {
            int dummy;
        } rw;
    };
} vsf_linux_socket_unix_priv_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

static int __vsf_linux_socket_unix_fcntl(vsf_linux_fd_t *sfd, int cmd, long arg);
static ssize_t __vsf_linux_socket_unix_read(vsf_linux_fd_t *sfd, void *buf, size_t count);
static ssize_t __vsf_linux_socket_unix_write(vsf_linux_fd_t *sfd, const void *buf, size_t count);
static int __vsf_linux_socket_unix_close(vsf_linux_fd_t *sfd);

static int __vsf_linux_socket_unix_init(vsf_linux_socket_priv_t *priv);
static int __vsf_linux_socket_unix_fini(vsf_linux_socket_priv_t *socket_priv, int how);
static int __vsf_linux_socket_unix_connect(vsf_linux_socket_priv_t *priv, const struct sockaddr *addr, socklen_t addrlen);
static int __vsf_linux_socket_unix_listen(vsf_linux_socket_priv_t *priv, int backlog);
static int __vsf_linux_socket_unix_accept(vsf_linux_socket_priv_t *priv, struct sockaddr *addr, socklen_t *addr_len);
static int __vsf_linux_socket_unix_bind(vsf_linux_socket_priv_t *priv, const struct sockaddr *addr, socklen_t addrlen);
static int __vsf_linux_socket_unix_getsockopt(vsf_linux_socket_priv_t *priv, int level, int optname, void *optval, socklen_t *optlen);
static int __vsf_linux_socket_unix_setsockopt(vsf_linux_socket_priv_t *priv, int level, int optname,const void *optval, socklen_t optlen);

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
    return 0;
}

static ssize_t __vsf_linux_socket_unix_read(vsf_linux_fd_t *sfd, void *buf, size_t count)
{
    return 0;
}

static ssize_t __vsf_linux_socket_unix_write(vsf_linux_fd_t *sfd, const void *buf, size_t count)
{
    return 0;
}

static int __vsf_linux_socket_unix_close(vsf_linux_fd_t *sfd)
{
    return 0;
}

static int __vsf_linux_socket_unix_init(vsf_linux_socket_priv_t *priv)
{
    return 0;
}

static int __vsf_linux_socket_unix_fini(vsf_linux_socket_priv_t *socket_priv, int how)
{
    return 0;
}

static int __vsf_linux_socket_unix_connect(vsf_linux_socket_priv_t *priv, const struct sockaddr *addr, socklen_t addrlen)
{
    return 0;
}

static int __vsf_linux_socket_unix_listen(vsf_linux_socket_priv_t *priv, int backlog)
{
    return 0;
}

static int __vsf_linux_socket_unix_accept(vsf_linux_socket_priv_t *priv, struct sockaddr *addr, socklen_t *addr_len)
{
    return 0;
}

static int __vsf_linux_socket_unix_bind(vsf_linux_socket_priv_t *priv, const struct sockaddr *addr, socklen_t addrlen)
{
    return 0;
}

static int __vsf_linux_socket_unix_getsockopt(vsf_linux_socket_priv_t *priv, int level, int optname, void *optval, socklen_t *optlen)
{
    return 0;
}

static int __vsf_linux_socket_unix_setsockopt(vsf_linux_socket_priv_t *priv, int level, int optname,const void *optval, socklen_t optlen)
{
    return 0;
}

#endif      // VSF_USE_LINUX && VSF_LINUX_USE_SOCKET
