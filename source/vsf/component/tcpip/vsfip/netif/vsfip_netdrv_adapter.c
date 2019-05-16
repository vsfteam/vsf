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

#define VSF_NETDRV_INHERIT_NETIF
#include "vsf.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static vsf_err_t vsfip_netdrv_adapter_on_connect(void *netif)
{
    return vsfip_netif_add((vsfip_netif_t *)netif);
}

static void vsfip_netdrv_adapter_on_disconnect(void *netif)
{
    vsfip_netif_remove((vsfip_netif_t *)netif);
}

static void vsfip_netdrv_adapter_on_outputted(void *netif, void *netbuf, vsf_err_t err)
{
    vsfip_netif_on_outputted((vsfip_netif_t *)netif, (vsfip_netbuf_t *)netbuf, err);
}

static void vsfip_netdrv_adapter_on_inputted(void *netif, void *netbuf, uint_fast32_t size)
{
    vsfip_netbuf_t *vsfip_netbuf = netbuf;

    ASSERT(vsfip_netbuf->buf.nSize >= size);
    vsfip_netbuf->buf.nSize = size;
    vsfip_netif_on_inputted((vsfip_netif_t *)netif, vsfip_netbuf);
}

static uint8_t * vsfip_netdrv_adapter_header(void *netbuf, int32_t len)
{
    return vsfip_netbuf_header((vsfip_netbuf_t *)netbuf, len);
}

static void * vsfip_netdrv_adapter_alloc_buf(void *netif, uint_fast32_t size)
{
    return vsfip_netbuf_get(size);
}

static void * vsfip_netdrv_adapter_read_buf(void *netbuf, vsf_mem_t *mem)
{
    *mem = ((vsfip_netbuf_t *)netbuf)->buf;
    return NULL;
}

static void vsfip_netdrv_adapter_free_buf(void *netbuf)
{
    vsfip_netbuf_deref((vsfip_netbuf_t *)netbuf);
}

static const vsf_netdrv_adapter_t vsfip_netdrv_adapter = {
    .on_connect     = vsfip_netdrv_adapter_on_connect,
    .on_disconnect  = vsfip_netdrv_adapter_on_disconnect,

    .alloc_buf      = vsfip_netdrv_adapter_alloc_buf,
    .free_buf       = vsfip_netdrv_adapter_free_buf,
    .read_buf       = vsfip_netdrv_adapter_read_buf,

    .header         = vsfip_netdrv_adapter_header,
    .on_outputted   = vsfip_netdrv_adapter_on_outputted,
    .on_inputted    = vsfip_netdrv_adapter_on_inputted,
};

void vsfip_netif_set_netdrv(vsfip_netif_t *netif, vsf_netdrv_t *netdrv)
{
    netif->netdrv = netdrv;
    netdrv->netif = netif;
    netdrv->adapter = &vsfip_netdrv_adapter;
}

vsf_netdrv_t * vsfip_netif_get_netdrv(vsfip_netif_t *netif)
{
    return netif->netdrv;
}

#endif      // VSF_USE_TCPIP
