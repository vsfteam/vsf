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

#ifndef __VSF_NETDRV_H__
#define __VSF_NETDRV_H__

/*============================ INCLUDES ======================================*/

#include "component/tcpip/vsf_tcpip_cfg.h"

#if VSF_USE_TCPIP == ENABLED

#include "kernel/vsf_kernel.h"

#if     defined(__VSF_NETDRV_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT__
#elif   defined(__VSF_NETDRV_CLASS_INHERIT_NETLINK__) || defined(__VSF_NETDRV_CLASS_INHERIT_NETIF__)
#   define __PLOOC_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

dcl_simple_class(vk_netdrv_t)
dcl_simple_class(vk_netlink_op_t)
dcl_simple_class(vk_netdrv_adapter_op_t)

def_simple_class(vk_netlink_op_t) {
#if defined(__VSF_NETDRV_CLASS_IMPLEMENT) || defined(__VSF_NETDRV_CLASS_INHERIT_NETLINK__)
    protected_member(
#else
    private_member(
#endif
        vsf_err_t (*init)(vk_netdrv_t *netdrv);
        vsf_err_t (*fini)(vk_netdrv_t *netdrv);

        bool (*can_output)(vk_netdrv_t *netdrv);
        vsf_err_t (*output)(vk_netdrv_t *netdrv, void *netbuf);
    )
};

def_simple_class(vk_netdrv_adapter_op_t) {
#if defined(__VSF_NETDRV_CLASS_IMPLEMENT) || defined(__VSF_NETDRV_CLASS_INHERIT_NETIF__)
    protected_member(
#else
    private_member(
#endif
        vsf_err_t (*on_connect)(void *netif);
        void (*on_disconnect)(void *netif);
        void (*on_outputted)(void *netif, void *netbuf, vsf_err_t err);
        void (*on_inputted)(void *netif, void *netbuf, uint_fast32_t size);

        void * (*alloc_buf)(void *netif, uint_fast16_t len);
        void (*free_buf)(void *netbuf);
        void * (*read_buf)(void *netbuf, vsf_mem_t *mem);
        uint8_t * (*header)(void *netbuf, int32_t len);
    )
};

typedef struct vk_netdrv_addr_t {
    uint32_t size;
    union {
        uint32_t addr32;
        uint64_t addr64;
        uint8_t addr_buf[16];
    };
} vk_netdrv_addr_t;

def_simple_class(vk_netdrv_t) {

    protected_member(
        vk_netdrv_addr_t macaddr;
        uint16_t mac_header_size;
        uint16_t mtu;
        uint16_t hwtype;

        uint8_t is_to_free;
        uint8_t is_connected;
    )

#if defined(__VSF_NETDRV_CLASS_IMPLEMENT) || defined(__VSF_NETDRV_CLASS_INHERIT_NETIF__)
    protected_member(
#else
    private_member(
#endif
        struct {
            void *netif;
            const vk_netdrv_adapter_op_t *op;
            vsf_eda_t *eda_pending;
        } adapter;
    )

#if defined(__VSF_NETDRV_CLASS_IMPLEMENT) || defined(__VSF_NETDRV_CLASS_INHERIT_NETLINK__)
    protected_member(
#else
    private_member(
#endif
        struct {
            void *param;
            const vk_netlink_op_t *op;
        } netlink;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

void vsf_pnp_on_netdrv_new(vk_netdrv_t *netdrv);
void vsf_pnp_on_netdrv_del(vk_netdrv_t *netdrv);
void vsf_pnp_on_netdrv_connect(vk_netdrv_t *netdrv);
void vsf_pnp_on_netdrv_connected(vk_netdrv_t *netdrv);
void vsf_pnp_on_netdrv_disconnect(vk_netdrv_t *netdrv);

extern vsf_err_t vk_netdrv_connect(vk_netdrv_t *netdrv);
extern void vk_netdrv_disconnect(vk_netdrv_t *netdrv);
extern void vk_netdrv_set_netlink_op(vk_netdrv_t *netdrv, const vk_netlink_op_t *netlink_op, void *param);

#if defined(__VSF_NETDRV_CLASS_IMPLEMENT) || defined(__VSF_NETDRV_CLASS_INHERIT_NETLINK__)
extern uint8_t * vk_netdrv_header(vk_netdrv_t *netdrv, void *netbuf, int32_t len);
extern bool vk_netdrv_is_connected(vk_netdrv_t *netdrv);
extern void * vk_netdrv_read_buf(vk_netdrv_t *netdrv, void *netbuf, vsf_mem_t *mem);
extern void * vk_netdrv_alloc_buf(vk_netdrv_t *netdrv);
extern void vk_netdrv_on_outputted(vk_netdrv_t *netdrv, void *netbuf, int_fast32_t size);
extern void vk_netdrv_on_inputted(vk_netdrv_t *netdrv, void *netbuf, int_fast32_t size);
#endif

#if defined(__VSF_NETDRV_CLASS_IMPLEMENT) || defined(__VSF_NETDRV_CLASS_INHERIT_NETIF__)
extern vsf_err_t vk_netdrv_init(vk_netdrv_t *netdrv);
extern vsf_err_t vk_netdrv_fini(vk_netdrv_t *netdrv);
extern bool vk_netdrv_can_output(vk_netdrv_t *netdrv);
extern vsf_err_t vk_netdrv_output(vk_netdrv_t *netdrv, void *netbuf);
#endif

// for link driver

#ifdef __cplusplus
}
#endif

/*============================ INCLUDES ======================================*/

#if VSF_NETDRV_USE_WPCAP == ENABLED
#   include "./driver/wpcap/vsf_netdrv_wpcap.h"
#endif

#undef __VSF_NETDRV_CLASS_IMPLEMENT
#undef __VSF_NETDRV_CLASS_INHERIT_NETLINK__
#undef __VSF_NETDRV_CLASS_INHERIT_NETIF__

#endif      // VSF_USE_TCPIP
#endif      // __VSF_NETDRV_H__
