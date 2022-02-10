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

#ifndef __VSF_LINUX_SOCKET_INTERNAL_H__
#define __VSF_LINUX_SOCKET_INTERNAL_H__

/*============================ INCLUDES ======================================*/

#include "../vsf_linux_cfg.h"

#if VSF_USE_LINUX == ENABLED && VSF_LINUX_USE_SOCKET == ENABLED

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../../include/sys/socket.h"
#else
#   include <sys/socket.h>
#endif

#if     defined(__VSF_LINUX_SOCKET_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_LINUX_SOCKET_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

vsf_dcl_class(vsf_linux_socket_priv_t)

typedef struct vsf_linux_socket_op_t {
    vsf_linux_fd_op_t fdop;

    int (*fn_init)(vsf_linux_fd_t *sfd);
    int (*fn_fini)(vsf_linux_socket_priv_t *priv, int how);
    int (*fn_connect)(vsf_linux_socket_priv_t *priv, const struct sockaddr *addr, socklen_t addrlen);
    int (*fn_listen)(vsf_linux_socket_priv_t *priv, int backlog);
    int (*fn_accept)(vsf_linux_socket_priv_t *priv, struct sockaddr *addr, socklen_t *addr_len);
    int (*fn_bind)(vsf_linux_socket_priv_t *priv, const struct sockaddr *addr, socklen_t addrlen);

    int (*fn_getsockopt)(vsf_linux_socket_priv_t *priv, int level, int optname, void *optval, socklen_t *optlen);
    int (*fn_setsockopt)(vsf_linux_socket_priv_t *priv, int level, int optname,const void *optval, socklen_t optlen);

    int (*fn_getpeername)(vsf_linux_socket_priv_t *socket_priv, struct sockaddr *addr, socklen_t *addrlen);
    int (*fn_getsockname)(vsf_linux_socket_priv_t *socket_priv, struct sockaddr *addr, socklen_t *addrlen);
} vsf_linux_socket_op_t;

vsf_class(vsf_linux_socket_priv_t) {
    public_member(
        implement(vsf_linux_fd_priv_t)

        int domain;
        int type;
        int protocol;
        const vsf_linux_socket_op_t *sockop;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

#if VSF_LINUX_SOCKET_USE_UNIX == ENABLED
extern const vsf_linux_socket_op_t vsf_linux_socket_unix_op;
#endif
#if VSF_LINUX_SOCKET_USE_INET == ENABLED
extern const vsf_linux_socket_op_t vsf_linux_socket_inet_op;
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_LINUX && VSF_LINUX_USE_SOCKET
#endif      // __VSF_LINUX_SOCKET_INTERNAL_H__
/* EOF */