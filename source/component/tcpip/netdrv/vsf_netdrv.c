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

extern void vsf_pnp_on_netdrv_prepare(vk_netdrv_t *netdrv);
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

#ifndef WEAK_VSF_PNP_ON_NETDRV_PREPARE
WEAK(vsf_pnp_on_netdrv_prepare)
void vsf_pnp_on_netdrv_prepare(vk_netdrv_t *netdrv) {}
#endif

#ifndef WEAK_VSF_PNP_ON_NETDRV_CONNECTED
WEAK(vsf_pnp_on_netdrv_connected)
void vsf_pnp_on_netdrv_connected(vk_netdrv_t *netdrv) {}
#endif

#ifndef WEAK_VSF_PNP_ON_NETDRV_DISCONNECT
WEAK(vsf_pnp_on_netdrv_disconnect)
void vsf_pnp_on_netdrv_disconnect(vk_netdrv_t *netdrv) {}
#endif

void vk_netdrv_on_netbuf_outputted(vk_netdrv_t *netdrv, void *netbuf)
{
    VSF_TCPIP_ASSERT(   (netdrv != NULL) && (netdrv->adapter.op != NULL)
                     && (netdrv->adapter.op->on_netbuf_outputted != NULL)
                     && (netbuf != NULL));
    if (netdrv->is_connected) {
        netdrv->adapter.op->on_netbuf_outputted(netdrv->adapter.netif, netbuf);
    } else {
        netdrv->adapter.op->free_buf(netbuf);
    }
}

void vk_netdrv_on_netlink_outputted(vk_netdrv_t *netdrv, vsf_err_t err)
{
    VSF_TCPIP_ASSERT(   (netdrv != NULL) && (netdrv->adapter.op != NULL)
                     && (netdrv->adapter.op->on_netlink_outputted != NULL));
    netdrv->adapter.op->on_netlink_outputted(netdrv->adapter.netif, err);
}

void vk_netdrv_on_outputted(vk_netdrv_t *netdrv, void *netbuf, vsf_err_t err)
{
    VSF_TCPIP_ASSERT((netdrv != NULL) && (netbuf != NULL));
    vk_netdrv_on_netbuf_outputted(netdrv, netbuf);
    vk_netdrv_on_netlink_outputted(netdrv, err);
}

void vk_netdrv_on_inputted(vk_netdrv_t *netdrv, void *netbuf, int_fast32_t size)
{
    VSF_TCPIP_ASSERT((netdrv != NULL) && (netdrv->adapter.op != NULL));

    if ((size > 0) && netdrv->is_connected) {
        netdrv->adapter.op->on_inputted(netdrv->adapter.netif, netbuf, (uint32_t)size);
    } else {
        netdrv->adapter.op->free_buf(netbuf);
    }
}

void * vk_netdrv_alloc_buf(vk_netdrv_t *netdrv)
{
    VSF_TCPIP_ASSERT((netdrv != NULL) && (netdrv->adapter.op != NULL));
    if (netdrv->is_connected) {
        return netdrv->adapter.op->alloc_buf(netdrv->adapter.netif, netdrv->mac_header_size + netdrv->mtu);
    }
    return NULL;
}

vk_netdrv_feature_t vk_netdrv_feature(vk_netdrv_t *netdrv)
{
    VSF_TCPIP_ASSERT((netdrv != NULL) && (netdrv->adapter.op != NULL) && (netdrv->adapter.op->feature != NULL));
    return netdrv->adapter.op->feature();
}

void * vk_netdrv_thread(vk_netdrv_t *netdrv, void (*entry)(void *), void *param)
{
    VSF_TCPIP_ASSERT((netdrv != NULL) && (netdrv->adapter.op != NULL) && (netdrv->adapter.op->thread != NULL));
    return netdrv->adapter.op->thread(entry, param);
}

vsf_err_t vk_netdrv_init(vk_netdrv_t *netdrv)
{
    VSF_TCPIP_ASSERT((netdrv != NULL) && (netdrv->netlink.op != NULL));
    return netdrv->netlink.op->init(netdrv);
}

vsf_err_t vk_netdrv_fini(vk_netdrv_t *netdrv)
{
    VSF_TCPIP_ASSERT((netdrv != NULL) && (netdrv->netlink.op != NULL));
    return netdrv->netlink.op->fini(netdrv);
}

void * vk_netdrv_get_netif(vk_netdrv_t *netdrv)
{
    return netdrv->adapter.netif;
}

uint8_t * vk_netdrv_header(vk_netdrv_t *netdrv, void *netbuf, int32_t len)
{
    VSF_TCPIP_ASSERT((netdrv != NULL) && (netdrv->adapter.op != NULL) && (netbuf != NULL));
    return netdrv->adapter.op->header(netbuf, len);
}

void * vk_netdrv_read_buf(vk_netdrv_t *netdrv, void *netbuf, vsf_mem_t *mem)
{
    VSF_TCPIP_ASSERT((netdrv != NULL) && (netdrv->adapter.op != NULL) && (netbuf != NULL));
    return netdrv->adapter.op->read_buf(netbuf, mem);
}

void vk_netdrv_prepare(vk_netdrv_t *netdrv)
{
    VSF_TCPIP_ASSERT(!netdrv->is_connected);
    vsf_pnp_on_netdrv_prepare(netdrv);
    VSF_TCPIP_ASSERT(netdrv->adapter.op != NULL);
}

vsf_err_t vk_netdrv_connect(vk_netdrv_t *netdrv)
{
    VSF_TCPIP_ASSERT(netdrv != NULL);
    if (!netdrv->is_connected) {
        vsf_err_t err = VSF_ERR_NONE;
        if (netdrv->adapter.op != NULL) {
            err = netdrv->adapter.op->on_connect(netdrv->adapter.netif);
        }
        if (VSF_ERR_NONE == err) {
            netdrv->is_connected = true;
            vsf_pnp_on_netdrv_connected(netdrv);
        }
        return err;
    }
    return VSF_ERR_NONE;
}

void vk_netdrv_disconnect(vk_netdrv_t *netdrv)
{
    VSF_TCPIP_ASSERT(netdrv != NULL);
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
    VSF_TCPIP_ASSERT(netdrv != NULL);
    return netdrv->is_connected;
}

void * vk_netdrv_can_output(vk_netdrv_t *netdrv)
{
    return netdrv->netlink.op->can_output(netdrv);
}

vsf_err_t vk_netdrv_output(vk_netdrv_t *netdrv, void *slot, void *netbuf)
{
    VSF_TCPIP_ASSERT(   (netdrv != NULL) && (slot != NULL) && (netdrv->netlink.op != NULL)
                    &&  (netdrv->adapter.op != NULL) && (netbuf != NULL));
    vsf_err_t err = VSF_ERR_FAIL;
    if (netdrv->is_connected) {
        err = netdrv->netlink.op->output(netdrv, slot, netbuf);
    }

    if (err) {
        vk_netdrv_on_outputted(netdrv, netbuf, err);
    }
    return err;
}

#endif      // VSF_USE_TCPIP
