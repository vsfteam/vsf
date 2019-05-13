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

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_TCPIP == ENABLED

#define VSF_NETDRV_IMPLEMENT
#include "vsf.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

WEAK void vsf_pnp_on_netdrv_new(vsf_netdrv_t *netdrv) {}
WEAK void vsf_pnp_on_netdrv_del(vsf_netdrv_t *netdrv) {}
WEAK void vsf_pnp_on_netdrv_connect(vsf_netdrv_t *netdrv) {}
WEAK void vsf_pnp_on_netdrv_connected(vsf_netdrv_t *netdrv) {}
WEAK void vsf_pnp_on_netdrv_disconnect(vsf_netdrv_t *netdrv) {}

void vsf_netdrv_on_outputted(vsf_netdrv_t *netdrv, void *netbuf, int_fast32_t size)
{
    ASSERT(netdrv != NULL);

    if (netdrv->is_connected) {
        vsf_err_t err = size < 0 ? VSF_ERR_FAIL : VSF_ERR_NONE;
        netdrv->adapter->on_outputted(netdrv->netif, netbuf, err);
    } else {
        netdrv->adapter->free_buf(netbuf);
    }
}

void vsf_netdrv_on_inputted(vsf_netdrv_t *netdrv, void *netbuf, int_fast32_t size)
{
    ASSERT(netdrv != NULL);

    if ((size > 0) && netdrv->is_connected) {
        netdrv->adapter->on_inputted(netdrv->netif, netbuf, (uint32_t)size);
    } else {
        netdrv->adapter->free_buf(netbuf);
    }
}

void * vsf_netdrv_alloc_buf(vsf_netdrv_t *netdrv)
{
    if (netdrv->is_connected) {
        return netdrv->adapter->alloc_buf(netdrv->netif, netdrv->mac_header_size + netdrv->mtu);
    }
    return NULL;
}

vsf_err_t vsf_netdrv_init(vsf_netdrv_t *netdrv)
{
    ASSERT(netdrv != NULL);
    return netdrv->netlink_op->init(netdrv);
}

vsf_err_t vsf_netdrv_fini(vsf_netdrv_t *netdrv)
{
    ASSERT(netdrv != NULL);
    return netdrv->netlink_op->fini(netdrv);
}

uint8_t * vsf_netdrv_header(vsf_netdrv_t *netdrv, void *netbuf, int32_t len)
{
    ASSERT((netdrv != NULL) && (netbuf != NULL));
    return netdrv->adapter->header(netbuf, len);
}

void * vsf_netdrv_read_buf(vsf_netdrv_t *netdrv, void *netbuf, vsf_mem_t *mem)
{
    ASSERT((netdrv != NULL) && (netbuf != NULL));
    return netdrv->adapter->read_buf(netbuf, mem);
}

vsf_err_t vsf_netdrv_connect(vsf_netdrv_t *netdrv)
{
    ASSERT(netdrv != NULL);
    if (!netdrv->is_connected) {
        netdrv->is_connected = true;
        vsf_pnp_on_netdrv_connect(netdrv);
        if (netdrv->adapter != NULL) {
            vsf_err_t err = netdrv->adapter->on_connect(netdrv->netif);
            if (!err) {
                vsf_pnp_on_netdrv_connected(netdrv);
            }
            return err;
        }
    }
    return VSF_ERR_NONE;
}

void vsf_netdrv_disconnect(vsf_netdrv_t *netdrv)
{
    ASSERT(netdrv != NULL);
    if (netdrv->is_connected) {
        netdrv->is_connected = false;
        vsf_pnp_on_netdrv_disconnect(netdrv);
        if (netdrv->adapter != NULL) {
            netdrv->adapter->on_disconnect(netdrv->netif);
        }
    }
}

bool vsf_netdrv_is_connected(vsf_netdrv_t *netdrv)
{
    ASSERT(netdrv != NULL);
    return netdrv->is_connected;
}

bool vsf_netdrv_can_output(vsf_netdrv_t *netdrv)
{
    return netdrv->netlink_op->can_output(netdrv);
}

vsf_err_t vsf_netdrv_output(vsf_netdrv_t *netdrv, void *netbuf)
{
    ASSERT((netdrv != NULL) && (netbuf != NULL));
    vsf_err_t err = VSF_ERR_FAIL;
    if (netdrv->is_connected) {
        err = netdrv->netlink_op->output(netdrv, netbuf);
    }

    if (err) {
        netdrv->adapter->on_outputted(netdrv->netif, netbuf, err);
    }
    return err;
}

#endif      // VSF_USE_TCPIP
