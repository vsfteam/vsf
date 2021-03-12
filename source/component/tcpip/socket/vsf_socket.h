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

#ifndef __VSF_SOCKET_H__
#define __VSF_SOCKET_H__

/*============================ INCLUDES ======================================*/

#include "component/tcpip/vsf_tcpip_cfg.h"

#if VSF_USE_TCPIP == ENABLED

#include "utilities/vsf_utilities.h"
#include "component/tcpip/netdrv/vsf_netdrv.h"

#undef PUBLIC_CONST
#if     defined(__VSF_SOCKET_CLASS_IMPLEMENT)
#undef __VSF_SOCKET_CLASS_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT__
#   define PUBLIC_CONST
#elif   defined(__VSF_SOCKET_CLASS_INHERIT__)
#undef __VSF_SOCKET_CLASS_INHERIT__
#   define __PLOOC_CLASS_INHERIT__
#   define PUBLIC_CONST
#else
#   define PUBLIC_CONST             const
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum {
    VSF_SOCKET_AF_INET      = 2,
    VSF_SOCKET_AF_INET6     = 10,
};

enum {
    VSF_SOCKET_IPPROTO_TCP  = 6,
    VSF_SOCKET_IPPROTO_UDP  = 17,
};

dcl_simple_class(vk_socket_t)
dcl_simple_class(vk_socket_op_t)

typedef enum vk_socket_feature_t {
    VSF_SOCKET_THREAD   = 1 << 0,   // support thread mode
    VSF_SOCKET_PEDA     = 1 << 1,   // support peda mode
    // TODO: add more here
} vk_socket_feature_t;

typedef struct vk_socket_addr_t {
    uint16_t port;
    vk_netdrv_addr_t addr;
} vk_socket_addr_t;

def_simple_class(vk_socket_op_t) {
    protected_member(
        vk_socket_feature_t feature;
        vsf_err_t (*socket)(vk_socket_t *socket, int family, int protocol);
        vsf_err_t (*close)(vk_socket_t *socket);

        vsf_err_t (*bind)(vk_socket_t *socket, const vk_socket_addr_t *addr);
        vsf_err_t (*listen)(vk_socket_t *socket, int backlog);
        vsf_err_t (*connect)(vk_socket_t *socket, const vk_socket_addr_t *remote_addr);
        vsf_err_t (*accept)(vk_socket_t *socket, vk_socket_addr_t *remote_addr);
        vsf_err_t (*send)(vk_socket_t *socket, const void *buf, size_t len, int flags,
                                    const vk_socket_addr_t *remote_addr);
        vsf_err_t (*recv)(vk_socket_t *socket, void *buf, size_t len, int flags,
                                    const vk_socket_addr_t *remote_addr);

        struct {
            struct {
                vsf_err_t (*gethostbyname)(const char *name, vk_netdrv_addr_t *addr);
            } dns;
        } protocols;
    )
};

def_simple_class(vk_socket_t) {
    public_member(
        PUBLIC_CONST vk_socket_op_t *op;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vk_socket_open(vk_socket_t *socket, int domain, int protocol);
extern vsf_err_t vk_socket_close(vk_socket_t *socket);

extern vsf_err_t vk_socket_bind(vk_socket_t *socket, vk_socket_addr_t *addr);
extern vsf_err_t vk_socket_listen(vk_socket_t *socket, int backlog);
extern vsf_err_t vk_socket_connect(vk_socket_t *socket, vk_socket_addr_t *remote_addr);
extern vsf_err_t vk_socket_accept(vk_socket_t *socket, vk_socket_addr_t *remote_addr);

extern vsf_err_t vk_socket_send(vk_socket_t *socket, const void *buf, size_t len, int flags);
extern vsf_err_t vk_socket_recv(vk_socket_t *socket, void *buf, size_t len, int flags);

extern vsf_err_t vk_socket_sendto(vk_socket_t *socket, const void *buf, size_t len, int flags,
                                    const vk_socket_addr_t *remote_addr);
extern vsf_err_t vk_socket_recvfrom(vk_socket_t *socket, void *buf, size_t len, int flags,
                                    const vk_socket_addr_t *remote_addr);

extern void vk_socket_set_default_op(vk_socket_op_t *op);
extern vsf_err_t vk_dns_gethostbyname(const char *name, vk_netdrv_addr_t *addr);

#ifdef __cplusplus
}
#endif

/*============================ INCLUDES ======================================*/

#if VSF_USE_LWIP == ENABLED
#   include "./driver/lwip/vsf_socket_lwip.h"
#endif
#if defined(__WIN__)
#   include "./driver/win/vsf_socket_win.h"
#endif

#endif      // VSF_USE_TCPIP
#endif      // __VSF_NETDRV_H__
