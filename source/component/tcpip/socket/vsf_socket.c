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

#include "component/tcpip/vsf_tcpip_cfg.h"

#if VSF_USE_TCPIP == ENABLED

#define __VSF_SOCKET_CLASS_IMPLEMENT
#include "./vsf_socket.h"
#include "kernel/vsf_kernel.h"

/*============================ MACROS ========================================*/

#if VSF_KERNEL_USE_SIMPLE_SHELL != ENABLED
#   error VSF_KERNEL_USE_SIMPLE_SHELL is needed!!!
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vk_socket_local_t {
    vk_socket_op_t *default_op;
} vk_socket_local_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static vk_socket_local_t __vk_socket;

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vk_socket_open(vk_socket_t *socket, int domain, int protocol)
{
    VSF_TCPIP_ASSERT(socket != NULL);
    VSF_TCPIP_ASSERT((socket->op != NULL) && (socket->op->socket != NULL));
    if (    (socket->op->feature & VSF_SOCKET_THREAD)
        &&  !(socket->op->feature & VSF_SOCKET_PEDA)) {
        VSF_TCPIP_ASSERT(vsf_eda_is_stack_owner(vsf_eda_get_cur()));
    }

    return socket->op->socket(socket, domain, protocol);
}

vsf_err_t vk_socket_close(vk_socket_t *socket)
{
    VSF_TCPIP_ASSERT(socket != NULL);
    VSF_TCPIP_ASSERT((socket->op != NULL) && (socket->op->close != NULL));
    if (    (socket->op->feature & VSF_SOCKET_THREAD)
        &&  !(socket->op->feature & VSF_SOCKET_PEDA)) {
        VSF_TCPIP_ASSERT(vsf_eda_is_stack_owner(vsf_eda_get_cur()));
    }

    return socket->op->close(socket);
}

vsf_err_t vk_socket_bind(vk_socket_t *socket, vk_socket_addr_t *addr)
{
    VSF_TCPIP_ASSERT(socket != NULL);
    VSF_TCPIP_ASSERT((socket->op != NULL) && (socket->op->bind != NULL));
    if (    (socket->op->feature & VSF_SOCKET_THREAD)
        &&  !(socket->op->feature & VSF_SOCKET_PEDA)) {
        VSF_TCPIP_ASSERT(vsf_eda_is_stack_owner(vsf_eda_get_cur()));
    }

    return socket->op->bind(socket, addr);
}

vsf_err_t vk_socket_listen(vk_socket_t *socket, int backlog)
{
    VSF_TCPIP_ASSERT(socket != NULL);
    VSF_TCPIP_ASSERT((socket->op != NULL) && (socket->op->listen != NULL));
    if (    (socket->op->feature & VSF_SOCKET_THREAD)
        &&  !(socket->op->feature & VSF_SOCKET_PEDA)) {
        VSF_TCPIP_ASSERT(vsf_eda_is_stack_owner(vsf_eda_get_cur()));
    }

    return socket->op->listen(socket, backlog);
}

vsf_err_t vk_socket_connect(vk_socket_t *socket, vk_socket_addr_t *remote_addr)
{
    VSF_TCPIP_ASSERT(socket != NULL);
    VSF_TCPIP_ASSERT((socket->op != NULL) && (socket->op->connect != NULL));
    if (    (socket->op->feature & VSF_SOCKET_THREAD)
        &&  !(socket->op->feature & VSF_SOCKET_PEDA)) {
        VSF_TCPIP_ASSERT(vsf_eda_is_stack_owner(vsf_eda_get_cur()));
    }

    return socket->op->connect(socket, remote_addr);
}

vsf_err_t vk_socket_accept(vk_socket_t *socket, vk_socket_addr_t *remote_addr)
{
    VSF_TCPIP_ASSERT(socket != NULL);
    VSF_TCPIP_ASSERT((socket->op != NULL) && (socket->op->accept != NULL));
    if (    (socket->op->feature & VSF_SOCKET_THREAD)
        &&  !(socket->op->feature & VSF_SOCKET_PEDA)) {
        VSF_TCPIP_ASSERT(vsf_eda_is_stack_owner(vsf_eda_get_cur()));
    }

    return socket->op->accept(socket, remote_addr);
}

vsf_err_t vk_socket_send(vk_socket_t *socket, const void *buf, size_t len, int flags)
{
    VSF_TCPIP_ASSERT(socket != NULL);
    VSF_TCPIP_ASSERT((socket->op != NULL) && (socket->op->send != NULL));
    if (    (socket->op->feature & VSF_SOCKET_THREAD)
        &&  !(socket->op->feature & VSF_SOCKET_PEDA)) {
        VSF_TCPIP_ASSERT(vsf_eda_is_stack_owner(vsf_eda_get_cur()));
    }

    return socket->op->send(socket, buf, len, flags, NULL);
}

vsf_err_t vk_socket_recv(vk_socket_t *socket, void *buf, size_t len, int flags)
{
    VSF_TCPIP_ASSERT(socket != NULL);
    VSF_TCPIP_ASSERT((socket->op != NULL) && (socket->op->recv != NULL));
    if (    (socket->op->feature & VSF_SOCKET_THREAD)
        &&  !(socket->op->feature & VSF_SOCKET_PEDA)) {
        VSF_TCPIP_ASSERT(vsf_eda_is_stack_owner(vsf_eda_get_cur()));
    }

    return socket->op->recv(socket, buf, len, flags, NULL);
}

vsf_err_t vk_socket_sendto(vk_socket_t *socket, const void *buf, size_t len, int flags,
                                    const vk_socket_addr_t *remote_addr)
{
    VSF_TCPIP_ASSERT(socket != NULL);
    VSF_TCPIP_ASSERT((socket->op != NULL) && (socket->op->send != NULL));
    if (    (socket->op->feature & VSF_SOCKET_THREAD)
        &&  !(socket->op->feature & VSF_SOCKET_PEDA)) {
        VSF_TCPIP_ASSERT(vsf_eda_is_stack_owner(vsf_eda_get_cur()));
    }

    return socket->op->send(socket, buf, len, flags, remote_addr);
}

vsf_err_t vk_socket_recvfrom(vk_socket_t *socket, void *buf, size_t len, int flags,
                                    const vk_socket_addr_t *remote_addr)
{
    VSF_TCPIP_ASSERT(socket != NULL);
    VSF_TCPIP_ASSERT((socket->op != NULL) && (socket->op->recv != NULL));
    if (    (socket->op->feature & VSF_SOCKET_THREAD)
        &&  !(socket->op->feature & VSF_SOCKET_PEDA)) {
        VSF_TCPIP_ASSERT(vsf_eda_is_stack_owner(vsf_eda_get_cur()));
    }

    return socket->op->recv(socket, buf, len, flags, remote_addr);
}

void vk_socket_set_default_op(vk_socket_op_t *op)
{
    __vk_socket.default_op = op;
}

vsf_err_t vk_dns_gethostbyname(const char *name, vk_netdrv_addr_t *addr)
{
    VSF_TCPIP_ASSERT(   (__vk_socket.default_op != NULL)
                    &&  (__vk_socket.default_op->protocols.dns.gethostbyname != NULL));
    if (    (__vk_socket.default_op->feature & VSF_SOCKET_THREAD)
        &&  !(__vk_socket.default_op->feature & VSF_SOCKET_PEDA)) {
        VSF_TCPIP_ASSERT(vsf_eda_is_stack_owner(vsf_eda_get_cur()));
    }

    return __vk_socket.default_op->protocols.dns.gethostbyname(name, addr);
}

#endif      // VSF_USE_TCPIP
