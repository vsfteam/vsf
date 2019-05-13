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

#if     defined(VSF_NETDRV_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT
#elif   defined(VSF_NETDRV_INHERIT_NETLINK) || defined(VSF_NETDRV_INHERIT_NETIF)
#   define __PLOOC_CLASS_INHERIT
#endif

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_simple_class(vsf_netdrv_t)

struct vsf_netlink_op_t {
    vsf_err_t (*init)(vsf_netdrv_t *netdrv);
    vsf_err_t (*fini)(vsf_netdrv_t *netdrv);

    bool (*can_output)(vsf_netdrv_t *netdrv);
    vsf_err_t (*output)(vsf_netdrv_t *netdrv, void *netbuf);
};
typedef struct vsf_netlink_op_t vsf_netlink_op_t;

struct vsf_netdrv_adapter_t {
    vsf_err_t (*on_connect)(void *netif);
    void (*on_disconnect)(void *netif);
    void (*on_outputted)(void *netif, void *netbuf, vsf_err_t err);
    void (*on_inputted)(void *netif, void *netbuf, uint_fast32_t size);

    void * (*alloc_buf)(void *netif, uint_fast16_t len);
    void (*free_buf)(void *netbuf);
    void * (*read_buf)(void *netbuf, vsf_mem_t *mem);
    uint8_t * (*header)(void *netbuf, int32_t len);
};
typedef struct vsf_netdrv_adapter_t vsf_netdrv_adapter_t;

struct vsf_netdrv_addr_t {
    uint32_t size;
    union {
        uint32_t addr32;
        uint64_t addr64;
        uint8_t addr_buf[16];
    };
};
typedef struct vsf_netdrv_addr_t vsf_netdrv_addr_t;

def_simple_class(vsf_netdrv_t) {

    protected_member(
        vsf_netdrv_addr_t macaddr;
        uint16_t mac_header_size;
        uint16_t mtu;
        uint16_t hwtype;

        uint8_t is_to_free;
        uint8_t is_connected;
    )

#if defined(VSF_NETDRV_IMPLEMENT) || defined(VSF_NETDRV_INHERIT_NETIF)
    protected_member(
#else
    private_member(
#endif
        void *netif;
        const vsf_netdrv_adapter_t *adapter;
        vsf_eda_t *eda_pending;
    )

#if defined(VSF_NETDRV_IMPLEMENT) || defined(VSF_NETDRV_INHERIT_NETLINK)
    protected_member(
#else
    private_member(
#endif
        void *netlink_param;
        const vsf_netlink_op_t *netlink_op;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

void vsf_pnp_on_netdrv_new(vsf_netdrv_t *netdrv);
void vsf_pnp_on_netdrv_del(vsf_netdrv_t *netdrv);
void vsf_pnp_on_netdrv_connect(vsf_netdrv_t *netdrv);
void vsf_pnp_on_netdrv_connected(vsf_netdrv_t *netdrv);
void vsf_pnp_on_netdrv_disconnect(vsf_netdrv_t *netdrv);

#if defined(VSF_NETDRV_IMPLEMENT) || defined(VSF_NETDRV_INHERIT_NETLINK)
extern uint8_t * vsf_netdrv_header(vsf_netdrv_t *netdrv, void *netbuf, int32_t len);
extern vsf_err_t vsf_netdrv_connect(vsf_netdrv_t *netdrv);
extern void vsf_netdrv_disconnect(vsf_netdrv_t *netdrv);
extern bool vsf_netdrv_is_connected(vsf_netdrv_t *netdrv);
extern void * vsf_netdrv_read_buf(vsf_netdrv_t *netdrv, void *netbuf, vsf_mem_t *mem);
extern void * vsf_netdrv_alloc_buf(vsf_netdrv_t *netdrv);
extern void vsf_netdrv_on_outputted(vsf_netdrv_t *netdrv, void *netbuf, int_fast32_t size);
extern void vsf_netdrv_on_inputted(vsf_netdrv_t *netdrv, void *netbuf, int_fast32_t size);
#endif

#if defined(VSF_NETDRV_IMPLEMENT) || defined(VSF_NETDRV_INHERIT_NETIF)
extern vsf_err_t vsf_netdrv_init(vsf_netdrv_t *netdrv);
extern vsf_err_t vsf_netdrv_fini(vsf_netdrv_t *netdrv);
extern bool vsf_netdrv_can_output(vsf_netdrv_t *netdrv);
extern vsf_err_t vsf_netdrv_output(vsf_netdrv_t *netdrv, void *netbuf);
#endif

// for link driver


#undef VSF_NETDRV_IMPLEMENT
#undef VSF_NETDRV_INHERIT_NETLINK
#undef VSF_NETDRV_INHERIT_NETIF

#endif      // VSF_USE_TCPIP
#endif      // __VSF_NETDRV_H__
