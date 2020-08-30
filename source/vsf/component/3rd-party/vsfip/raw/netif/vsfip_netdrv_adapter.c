/*****************************************************************************
 *   Copyright(C)2009-2019 by SimonQian                                      *
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

#if VSF_USE_TCPIP == ENABLED && VSF_USE_VSFIP == ENABLED

#define __VSF_NETDRV_CLASS_INHERIT_NETIF__
#include "component/tcpip/vsf_tcpip.h"
#include "../vsfip.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

static vsf_err_t __vsfip_netdrv_adapter_on_connect(void *netif);
static void __vsfip_netdrv_adapter_on_disconnect(void *netif);
static void __vsfip_netdrv_adapter_on_outputted(void *netif, void *netbuf, vsf_err_t err);
static void __vsfip_netdrv_adapter_on_inputted(void *netif, void *netbuf, uint_fast32_t size);
static uint8_t * __vsfip_netdrv_adapter_header(void *netbuf, int32_t len);
static void * __vsfip_netdrv_adapter_alloc_buf(void *netif, uint_fast32_t size);
static void * __vsfip_netdrv_adapter_read_buf(void *netbuf, vsf_mem_t *mem);
static void __vsfip_netdrv_adapter_free_buf(void *netbuf);

/*============================ LOCAL VARIABLES ===============================*/

static const vk_netdrv_adapter_op_t __vsfip_netdrv_adapter_op = {
    .on_connect     = __vsfip_netdrv_adapter_on_connect,
    .on_disconnect  = __vsfip_netdrv_adapter_on_disconnect,

    .alloc_buf      = __vsfip_netdrv_adapter_alloc_buf,
    .free_buf       = __vsfip_netdrv_adapter_free_buf,
    .read_buf       = __vsfip_netdrv_adapter_read_buf,

    .header         = __vsfip_netdrv_adapter_header,
    .on_outputted   = __vsfip_netdrv_adapter_on_outputted,
    .on_inputted    = __vsfip_netdrv_adapter_on_inputted,
};

/*============================ IMPLEMENTATION ================================*/

static vsf_err_t __vsfip_netdrv_adapter_on_connect(void *netif)
{
    return vsfip_netif_add((vsfip_netif_t *)netif);
}

static void __vsfip_netdrv_adapter_on_disconnect(void *netif)
{
    vsfip_netif_remove((vsfip_netif_t *)netif);
}

static void __vsfip_netdrv_adapter_on_outputted(void *netif, void *netbuf, vsf_err_t err)
{
    vsfip_netif_on_outputted((vsfip_netif_t *)netif, (vsfip_netbuf_t *)netbuf, err);
}

static void __vsfip_netdrv_adapter_on_inputted(void *netif, void *netbuf, uint_fast32_t size)
{
    vsfip_netbuf_t *vsfip_netbuf = netbuf;

    ASSERT(vsfip_netbuf->buf.size >= size);
    vsfip_netbuf->buf.size = size;
    vsfip_netif_on_inputted((vsfip_netif_t *)netif, vsfip_netbuf);
}

static uint8_t * __vsfip_netdrv_adapter_header(void *netbuf, int32_t len)
{
    return vsfip_netbuf_header((vsfip_netbuf_t *)netbuf, len);
}

static void * __vsfip_netdrv_adapter_alloc_buf(void *netif, uint_fast32_t size)
{
    return vsfip_netbuf_get(size);
}

static void * __vsfip_netdrv_adapter_read_buf(void *netbuf, vsf_mem_t *mem)
{
    *mem = ((vsfip_netbuf_t *)netbuf)->buf;
    return NULL;
}

static void __vsfip_netdrv_adapter_free_buf(void *netbuf)
{
    vsfip_netbuf_deref((vsfip_netbuf_t *)netbuf);
}

void vsfip_netif_set_netdrv(vsfip_netif_t *netif, vk_netdrv_t *netdrv)
{
    netif->netdrv = netdrv;
    netdrv->adapter.netif = netif;
    netdrv->adapter.op = &__vsfip_netdrv_adapter_op;
}

vk_netdrv_t * vsfip_netif_get_netdrv(vsfip_netif_t *netif)
{
    return netif->netdrv;
}

#endif      // VSF_USE_TCPIP
