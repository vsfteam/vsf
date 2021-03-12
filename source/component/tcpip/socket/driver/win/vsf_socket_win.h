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

#ifndef __VSF_SOCKET_WIN_H__
#define __VSF_SOCKET_WIN_H__

/*============================ INCLUDES ======================================*/

#include "component/tcpip/vsf_tcpip_cfg.h"

#if VSF_USE_TCPIP == ENABLED && defined(__WIN__)

#include "utilities/vsf_utilities.h"
#include "hal/arch/vsf_arch.h"
#include "kernel/vsf_kernel.h"

#if     defined(__VSF_SOCKET_WIN_CLASS_IMPLEMENT)
#   undef __VSF_SOCKET_WIN_CLASS_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

dcl_simple_class(vk_socket_win_t)

typedef enum vk_socket_win_request_t {
    VSF_SOCKET_WIN_BIND,
    VSF_SOCKET_WIN_LISTEN,
    VSF_SOCKET_WIN_CONNECT,
    VSF_SOCKET_WIN_ACCEPT,
    VSF_SOCKET_WIN_SEND,
    VSF_SOCKET_WIN_RECV,
    VSF_SOCKET_WIN_CLOSE,
} vk_socket_win_request_t;

def_simple_class(vk_socket_win_t) {
    public_member(
        implement(vk_socket_t)
    )
    private_member(
        bool is_inited;

        int family;
        int protocol;
        intalu_t socket;

        struct {
            vsf_arch_irq_thread_t irq_thread;
            vsf_arch_irq_request_t irq_request;
            bool is_busy;
            bool is_to_exit;

            vk_socket_win_request_t request;

            union {
                struct {
                    const vk_socket_addr_t *addr;
                } bind;
                struct {
                    int backlog;
                } listen;
                struct {
                    const vk_socket_addr_t *remote_addr;
                } connect;
                struct {
                    vk_socket_addr_t *remote_addr;
                } accept;
                struct {
                    void *buf;
                    size_t len;
                    int flags;
                    const vk_socket_addr_t *remote_addr;
                } send;
            } args;

            int ret;
            vsf_eda_t *eda;
        } tx;

        struct {
            vsf_arch_irq_thread_t irq_thread;
            vsf_arch_irq_request_t irq_request;
            bool is_busy;
            bool is_to_exit;

            vk_socket_win_request_t request;

            union {
                struct {
                    void *buf;
                    size_t len;
                    int flags;
                    const vk_socket_addr_t *remote_addr;
                } recv;
            } args;

            int ret;
            vsf_eda_t *eda;
        } rx;
    )
};

/*============================ INCLUDES ======================================*/
/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_socket_op_t vk_socket_win_op;

/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vk_socket_win_init(void);
extern vsf_err_t vk_socket_win_fini(void);

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_TCPIP
#endif      // __VSF_NETDRV_H__
