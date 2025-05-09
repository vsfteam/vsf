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

#if     VSF_USE_LINUX == ENABLED && VSF_LINUX_USE_SOCKET == ENABLED             \
    &&  VSF_LINUX_SOCKET_USE_BLUETOOTH == ENABLED && VSF_USE_BTSTACK == ENABLED

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

#include <btstack.h>

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_linux_socket_bluetooth_priv_t {
    implement(vsf_linux_socket_priv_t)

    union {
        struct {
            vsf_linux_fd_t                  *dev_sfd;
            vsf_linux_fd_priv_callback_t    *callback;
        } hci;
    };
} vsf_linux_socket_bluetooth_priv_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern int __vsf_linux_pipe_fcntl(vsf_linux_fd_t *sfd, int cmd, uintptr_t arg);
extern int __vsf_linux_socket_stat(vsf_linux_fd_t *sfd, struct stat *buf);

static int __vsf_linux_socket_bluetooth_fcntl(vsf_linux_fd_t *sfd, int cmd, uintptr_t arg);
static int __vsf_linux_socket_bluetooth_close(vsf_linux_fd_t *sfd);

static ssize_t __vsf_linux_socket_bluetooth_read(vsf_linux_fd_t *sfd, void *buf, size_t count);
static ssize_t __vsf_linux_socket_bluetooth_write(vsf_linux_fd_t *sfd, const void *buf, size_t count);
static int __vsf_linux_socket_bluetooth_bind(vsf_linux_socket_priv_t *socket_priv, const struct sockaddr *addr, socklen_t addrlen);

static int __vsf_linux_socket_bluetooth_init_chipset(int fd, const btstack_chipset_t *chipset_instance);

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
    vsf_linux_socket_bluetooth_priv_t *bt_priv = (vsf_linux_socket_bluetooth_priv_t *)sfd->priv;
    switch (bt_priv->protocol) {
    case BTPROTO_HCI:
        return read(bt_priv->hci.dev_sfd->fd, buf, count);
    }
    return -1;
}

static ssize_t __vsf_linux_socket_bluetooth_write(vsf_linux_fd_t *sfd, const void *buf, size_t count)
{
    vsf_linux_socket_bluetooth_priv_t *bt_priv = (vsf_linux_socket_bluetooth_priv_t *)sfd->priv;
    switch (bt_priv->protocol) {
    case BTPROTO_HCI:
        return write(bt_priv->hci.dev_sfd->fd, buf, count);
    }
    return -1;
}

static int __vsf_linux_socket_bluetooth_close(vsf_linux_fd_t *sfd)
{
    vsf_linux_socket_bluetooth_priv_t *bt_priv = (vsf_linux_socket_bluetooth_priv_t *)sfd->priv;
    switch (bt_priv->protocol) {
    case BTPROTO_HCI:
        if (bt_priv->hci.dev_sfd != NULL) {
            if (bt_priv->hci.callback != NULL) {
                vsf_linux_fd_release_calback(bt_priv->hci.dev_sfd->priv, bt_priv->hci.callback);
                bt_priv->hci.callback = NULL;
            }
            close(bt_priv->hci.dev_sfd->fd);
            bt_priv->hci.dev_sfd = NULL;
        }
        break;
    }
    return 0;
}

void __vsf_linux_bthci_on_events(vsf_linux_fd_priv_t *priv, void *param, short events, vsf_protect_t orig)
{
    if ((priv->events_callback[1].cb != NULL) && (events & priv->events_callback[1].pendind_events))  {
        priv->events_callback[1].cb(priv, priv->events_callback[1].param, events, orig);
    } else {
        priv->events &= ~(events & ~priv->sticky_events);
        vsf_unprotect_sched(orig);
    }

    vsf_linux_socket_bluetooth_priv_t *bt_priv = (vsf_linux_socket_bluetooth_priv_t *)param;
    vsf_linux_fd_set_status(&bt_priv->use_as__vsf_linux_fd_priv_t, events, vsf_protect_sched());
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

            if (bt_priv->hci.dev_sfd != NULL) {
                close(bt_priv->hci.dev_sfd->fd);
                bt_priv->hci.dev_sfd = NULL;
            }
            int dev_fd = open(devhci_path, O_EXCL | O_NOCTTY | O_RDWR);
            if (dev_fd < 0) {
                return -1;
            }

            vsf_linux_bthci_t *bthci;
            if (    (vsf_linux_fd_get_target(dev_fd, (void **)&bthci) < 0)
                ||  (__vsf_linux_socket_bluetooth_init_chipset(dev_fd, bthci->chipset_instance) < 0)) {
                close(dev_fd);
                return -1;
            }

            bt_priv->hci.dev_sfd = vsf_linux_fd_get(dev_fd);
            bt_priv->hci.callback = vsf_linux_fd_claim_calback(bt_priv->hci.dev_sfd->priv);
            vsf_linux_fd_priv_callback_t *callback = bt_priv->hci.callback;
            callback->pendind_events = 0xFFFF;
            callback->param = bt_priv;
            callback->cb = __vsf_linux_bthci_on_events;
        }
        break;
    default:
        return -1;
    }
    return 0;
}

static int __vsf_linux_socket_bluetooth_rxevt(int fd, uint8_t *buffer, uint16_t buffer_len)
{
    uint8_t *ptr = buffer;
    int result = 0, total = 3;

    while (result < total) {
        result += read(fd, ptr, total - result);
    }
    total = 3 + buffer[2];
    ptr += 3;
    while (result < total) {
        result += read(fd, ptr, total - result);
    }
    return result;
}

static int __vsf_linux_socket_bluetooth_init_chipset(int fd, const btstack_chipset_t *chipset_instance)
{
    uint8_t hci_buffer[4 + 255];

    // hci_reset: 01 03 0c 00
    static const uint8_t hci_reset[] = {0x01, 0x03, 0x0c, 0x00};
    write(fd, hci_reset, sizeof(hci_reset));
    // hci_reset response: 04 0e 04 05 03 0c 00
    __vsf_linux_socket_bluetooth_rxevt(fd, hci_buffer, sizeof(hci_buffer));

    // hci_read_local_version_information: 01 01 10 00
    static const uint8_t hci_read_local_version_information[] = {0x01, 0x01, 0x10, 0x00};
    write(fd, hci_read_local_version_information, sizeof(hci_read_local_version_information));
    __vsf_linux_socket_bluetooth_rxevt(fd, hci_buffer, sizeof(hci_buffer));

    // hci_read_local_name: 01 14 0c 00
    static const uint8_t hci_read_local_name[] = {0x01, 0x14, 0x0c, 0x00};
    write(fd, hci_read_local_name, sizeof(hci_read_local_name));
    __vsf_linux_socket_bluetooth_rxevt(fd, hci_buffer, sizeof(hci_buffer));

    if (chipset_instance != NULL && chipset_instance->next_command != NULL) {
        btstack_chipset_result_t result;
        while (true) {
            result = chipset_instance->next_command(hci_buffer + 1);
            if (BTSTACK_CHIPSET_VALID_COMMAND == result) {
                hci_buffer[0] = HCI_COMMAND_DATA_PACKET;
                write(fd, hci_buffer, 4 + hci_buffer[3]);
                __vsf_linux_socket_bluetooth_rxevt(fd, hci_buffer, sizeof(hci_buffer));
            } else if (BTSTACK_CHIPSET_WARMSTART_REQUIRED == result) {
                VSF_LINUX_ASSERT(false);
            } else {
                break;
            }
        }
    }

    return 0;
}

#endif      // VSF_USE_LINUX && VSF_LINUX_USE_SOCKET && VSF_LINUX_SOCKET_USE_BLUETOOTH && VSF_USE_BTSTACK
