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

#if VSF_USE_LINUX == ENABLED && VSF_LINUX_USE_SOCKET == ENABLED && VSF_LINUX_SOCKET_USE_BLUETOOTH == ENABLED

// for protected members in vsf_linux_socket_priv_t
#define __VSF_LINUX_SOCKET_CLASS_INHERIT__
#define __VSF_LINUX_FS_CLASS_INHERIT__
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../../../include/unistd.h"
#   include "../../../include/errno.h"
#   include "../../../include/fcntl.h"
#   include "../../../include/poll.h"
#   include "../../../include/bluetooth/bluetooth.h"
#else
#   include <unistd.h>
#   include <errno.h>
#   include <fcntl.h>
#   include <poll.h>
#   include <bluetooth/bluetooth.h>
#endif
#include "../vsf_linux_socket.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_linux_socket_bluetooth_priv_t {
    implement(vsf_linux_socket_priv_t)

    union {
        struct {
            int             dev_fd;
        } hci;
    };
} vsf_linux_socket_bluetooth_priv_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern int vsf_linux_open(int dirfd, const char *pathname, int flags, mode_t mode);
extern int __vsf_linux_fd_close_ex(vsf_linux_process_t *process, int fd);
extern int __vsf_linux_pipe_fcntl(vsf_linux_fd_t *sfd, int cmd, uintptr_t arg);
extern int __vsf_linux_socket_stat(vsf_linux_fd_t *sfd, struct stat *buf);

static int __vsf_linux_socket_bluetooth_fcntl(vsf_linux_fd_t *sfd, int cmd, uintptr_t arg);
static int __vsf_linux_socket_bluetooth_close(vsf_linux_fd_t *sfd);

static ssize_t __vsf_linux_socket_bluetooth_read(vsf_linux_fd_t *sfd, void *buf, size_t count);
static ssize_t __vsf_linux_socket_bluetooth_write(vsf_linux_fd_t *sfd, const void *buf, size_t count);
static int __vsf_linux_socket_bluetooth_init(vsf_linux_fd_t *sfd);
static int __vsf_linux_socket_bluetooth_fini(vsf_linux_socket_priv_t *socket_priv, int how);
static int __vsf_linux_socket_bluetooth_bind(vsf_linux_socket_priv_t *socket_priv, const struct sockaddr *addr, socklen_t addrlen);

/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/

const vsf_linux_socket_op_t vsf_linux_socket_bluetooth_op = {
    .fdop               = {
        .priv_size      = sizeof(vsf_linux_socket_bluetooth_priv_t),
        .fn_fcntl       = __vsf_linux_socket_bluetooth_fcntl,
        .fn_read        = __vsf_linux_socket_bluetooth_read,
        .fn_write       = __vsf_linux_socket_bluetooth_write,
        .fn_close       = __vsf_linux_socket_bluetooth_close,
        .fn_stat        = __vsf_linux_socket_stat,
    },

    .fn_init            = __vsf_linux_socket_bluetooth_init,
    .fn_fini            = __vsf_linux_socket_bluetooth_fini,
    .fn_bind            = __vsf_linux_socket_bluetooth_bind,
};

/*============================ IMPLEMENTATION ================================*/

static int __vsf_linux_socket_bluetooth_fcntl(vsf_linux_fd_t *sfd, int cmd, uintptr_t arg)
{
    int result = -1;

    switch (cmd) {
    case HCIDEVDOWN:
    case HCIDEVUP:
        result = 0;
        break;
    default:
        return __vsf_linux_pipe_fcntl(sfd, cmd, arg);
    }

    return result;
}

static ssize_t __vsf_linux_socket_bluetooth_read(vsf_linux_fd_t *sfd, void *buf, size_t count)
{
    return 0;
}

static ssize_t __vsf_linux_socket_bluetooth_write(vsf_linux_fd_t *sfd, const void *buf, size_t count)
{
    return 0;
}

static int __vsf_linux_socket_bluetooth_close(vsf_linux_fd_t *sfd)
{
    vsf_linux_socket_bluetooth_priv_t *bt_priv = (vsf_linux_socket_bluetooth_priv_t *)sfd->priv;
    switch (bt_priv->protocol) {
    case BTPROTO_HCI:
        if (bt_priv->hci.dev_fd >= 0) {
            __vsf_linux_fd_close_ex(NULL, bt_priv->hci.dev_fd);
            bt_priv->hci.dev_fd = -1;
        }
        break;
    }
    return 0;
}

static int __vsf_linux_socket_bluetooth_init(vsf_linux_fd_t *sfd)
{
    vsf_linux_socket_bluetooth_priv_t *bt_priv = (vsf_linux_socket_bluetooth_priv_t *)sfd->priv;
    switch (bt_priv->protocol) {
    case BTPROTO_HCI:
        bt_priv->hci.dev_fd = -1;
        break;
    }
    return 0;
}

static int __vsf_linux_socket_bluetooth_fini(vsf_linux_socket_priv_t *socket_priv, int how)
{
    return 0;
}

static int __vsf_linux_socket_bluetooth_bind(vsf_linux_socket_priv_t *socket_priv, const struct sockaddr *addr, socklen_t addrlen)
{
    vsf_linux_socket_bluetooth_priv_t *bt_priv = (vsf_linux_socket_bluetooth_priv_t *)socket_priv;
    switch (bt_priv->protocol) {
    case BTPROTO_HCI: {
            struct sockaddr_hci *sa_hci = (struct sockaddr_hci *)addr;
            if (    (sa_hci->hci_family != AF_BLUETOOTH)
                ||  (   (sa_hci->hci_channel != HCI_CHANNEL_RAW)
                    &&  (sa_hci->hci_channel != HCI_CHANNEL_USER))) {
                return -1;
            }

            char devhci_path[sizeof(VSF_LINUX_BTHCI_PATH_PREFIX) + 16];
            snprintf(devhci_path, sizeof(devhci_path), VSF_LINUX_BTHCI_PATH_PREFIX"%d", sa_hci->hci_dev);

            if (bt_priv->hci.dev_fd >= 0) {
                __vsf_linux_fd_close_ex(NULL, bt_priv->hci.dev_fd);
                bt_priv->hci.dev_fd = -1;
            }
            bt_priv->hci.dev_fd = vsf_linux_open(-1, devhci_path, O_EXCL, 0);
            if (bt_priv->hci.dev_fd < 0) {
                return -1;
            }
        }
        break;
    default:
        return -1;
    }
    return 0;
}

#endif      // VSF_USE_LINUX && VSF_LINUX_USE_SOCKET && VSF_LINUX_SOCKET_USE_BLUETOOTH
