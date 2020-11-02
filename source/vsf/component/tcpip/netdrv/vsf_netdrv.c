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

#define __VSF_NETDRV_CLASS_IMPLEMENT
#include "./vsf_netdrv.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern void vsf_pnp_on_netdrv_connect(vk_netdrv_t *netdrv);
extern void vsf_pnp_on_netdrv_connected(vk_netdrv_t *netdrv);
extern void vsf_pnp_on_netdrv_disconnect(vk_netdrv_t *netdrv);

/*============================ IMPLEMENTATION ================================*/

#ifndef WEAK_VSF_PNP_ON_NETDRV_NEW
WEAK(vsf_pnp_on_netdrv_new)
void vsf_pnp_on_netdrv_new(vk_netdrv_t *netdrv) {}
#endif

#ifndef WEAK_VSF_PNP_ON_NETDRV_DEL
WEAK(vsf_pnp_on_netdrv_del)
void vsf_pnp_on_netdrv_del(vk_netdrv_t *netdrv) {}
#endif

#ifndef WEAK_VSF_PNP_ON_NETDRV_CONNECT
WEAK(vsf_pnp_on_netdrv_connect)
void vsf_pnp_on_netdrv_connect(vk_netdrv_t *netdrv) {}
#endif

#ifndef WEAK_VSF_PNP_ON_NETDRV_CONNECTED
WEAK(vsf_pnp_on_netdrv_connected)
void vsf_pnp_on_netdrv_connected(vk_netdrv_t *netdrv) {}
#endif

#ifndef WEAK_VSF_PNP_ON_NETDRV_DISCONNECT
WEAK(vsf_pnp_on_netdrv_disconnect)
void vsf_pnp_on_netdrv_disconnect(vk_netdrv_t *netdrv) {}
#endif

void vk_netdrv_on_outputted(vk_netdrv_t *netdrv, void *netbuf, int_fast32_t size)
{
    ASSERT((netdrv != NULL) && (netdrv->adapter.op != NULL));

    if (netdrv->is_connected) {
        vsf_err_t err = size < 0 ? VSF_ERR_FAIL : VSF_ERR_NONE;
        netdrv->adapter.op->on_outputted(netdrv->adapter.netif, netbuf, err);
    } else {
        netdrv->adapter.op->free_buf(netbuf);
    }
}

void vk_netdrv_on_inputted(vk_netdrv_t *netdrv, void *netbuf, int_fast32_t size)
{
    ASSERT((netdrv != NULL) && (netdrv->adapter.op != NULL));

    if ((size > 0) && netdrv->is_connected) {
        netdrv->adapter.op->on_inputted(netdrv->adapter.netif, netbuf, (uint32_t)size);
    } else {
        netdrv->adapter.op->free_buf(netbuf);
    }
}

void * vk_netdrv_alloc_buf(vk_netdrv_t *netdrv)
{
    ASSERT((netdrv != NULL) && (netdrv->adapter.op != NULL));
    if (netdrv->is_connected) {
        return netdrv->adapter.op->alloc_buf(netdrv->adapter.netif, netdrv->mac_header_size + netdrv->mtu);
    }
    return NULL;
}

vsf_err_t vk_netdrv_init(vk_netdrv_t *netdrv)
{
    ASSERT((netdrv != NULL) && (netdrv->netlink.op != NULL));
    return netdrv->netlink.op->init(netdrv);
}

vsf_err_t vk_netdrv_fini(vk_netdrv_t *netdrv)
{
    ASSERT((netdrv != NULL) && (netdrv->netlink.op != NULL));
    return netdrv->netlink.op->fini(netdrv);
}

uint8_t * vk_netdrv_header(vk_netdrv_t *netdrv, void *netbuf, int32_t len)
{
    ASSERT((netdrv != NULL) && (netdrv->adapter.op != NULL) && (netbuf != NULL));
    return netdrv->adapter.op->header(netbuf, len);
}

void * vk_netdrv_read_buf(vk_netdrv_t *netdrv, void *netbuf, vsf_mem_t *mem)
{
    ASSERT((netdrv != NULL) && (netdrv->adapter.op != NULL) && (netbuf != NULL));
    return netdrv->adapter.op->read_buf(netbuf, mem);
}

vsf_err_t vk_netdrv_connect(vk_netdrv_t *netdrv)
{
    ASSERT(netdrv != NULL);
    if (!netdrv->is_connected) {
        netdrv->is_connected = true;
        vsf_pnp_on_netdrv_connect(netdrv);
        if (netdrv->adapter.op != NULL) {
            vsf_err_t err = netdrv->adapter.op->on_connect(netdrv->adapter.netif);
            if (!err) {
                vsf_pnp_on_netdrv_connected(netdrv);
            }
            return err;
        }
    }
    return VSF_ERR_NONE;
}

void vk_netdrv_disconnect(vk_netdrv_t *netdrv)
{
    ASSERT(netdrv != NULL);
    if (netdrv->is_connected) {
        netdrv->is_connected = false;
        vsf_pnp_on_netdrv_disconnect(netdrv);
        if (netdrv->adapter.op != NULL) {
            netdrv->adapter.op->on_disconnect(netdrv->adapter.netif);
        }
    }
}

void vk_netdrv_set_netlink_op(vk_netdrv_t *netdrv, const vk_netlink_op_t *netlink_op, void *param)
{
    netdrv->netlink.param = param;
    netdrv->netlink.op = netlink_op;
}

bool vk_netdrv_is_connected(vk_netdrv_t *netdrv)
{
    ASSERT(netdrv != NULL);
    return netdrv->is_connected;
}

bool vk_netdrv_can_output(vk_netdrv_t *netdrv)
{
    return netdrv->netlink.op->can_output(netdrv);
}

vsf_err_t vk_netdrv_output(vk_netdrv_t *netdrv, void *netbuf)
{
    ASSERT((netdrv != NULL) && (netdrv->netlink.op != NULL) && (netdrv->adapter.op != NULL) && (netbuf != NULL));
    vsf_err_t err = VSF_ERR_FAIL;
    if (netdrv->is_connected) {
        err = netdrv->netlink.op->output(netdrv, netbuf);
    }

    if (err) {
        netdrv->adapter.op->on_outputted(netdrv->adapter.netif, netbuf, err);
    }
    return err;
}

#endif      // VSF_USE_TCPIP
