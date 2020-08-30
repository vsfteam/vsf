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

#include "../../../vsf_tcpip_cfg.h"

#if VSF_USE_TCPIP == ENABLED && VSF_USE_NETDRV_WPCAP == ENABLED

#define __VSF_NETDRV_CLASS_INHERIT_NETLINK__
#define __VSF_NETDRV_WPCAP_CLASS_IMPLEMENT
#include "../../vsf_netdrv.h"

#include "pcap.h"

/*============================ MACROS ========================================*/

#define VSF_NETDRV_WPCAP_CFG_TRACE              DISABLED

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

static vsf_err_t __vk_netdrv_wpcap_netlink_init(vk_netdrv_t *netdrv);
static vsf_err_t __vk_netdrv_wpcap_netlink_fini(vk_netdrv_t *netdrv);
static bool __vk_netdrv_wpcap_netlink_can_output(vk_netdrv_t *netdrv);
static vsf_err_t __vk_netdrv_wpcap_netlink_output(vk_netdrv_t *netdrv, void *netbuf);

/*============================ GLOBAL VARIABLES ==============================*/

const struct vk_netlink_op_t vk_netdrv_wpcap_netlink_op = {
    .init       = __vk_netdrv_wpcap_netlink_init,
    .fini       = __vk_netdrv_wpcap_netlink_fini,
    .can_output = __vk_netdrv_wpcap_netlink_can_output,
    .output     = __vk_netdrv_wpcap_netlink_output,
};

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void * __vk_netdrv_wpcap_open(char *name)
{
    char errbuf[PCAP_ERRBUF_SIZE + 1];
    pcap_if_t *alldevs;
    void *fp = NULL;

    if (pcap_findalldevs_ex("rpcap://", NULL, &alldevs, errbuf) == -1) {
        vsf_trace(VSF_TRACE_ERROR, "Error in pcap_findalldevs: %s\n", errbuf);
    } else {
        for (pcap_if_t *d = alldevs; d != NULL; d = d->next) {
            if (    strstr(d->description, name)
                ||  strstr(d->name, name)) {
                vsf_trace(VSF_TRACE_INFO, "wpcap: try to open %s at %s"VSF_TRACE_CFG_LINEEND, d->description, d->name);
                fp = pcap_open_live(d->name, 65536, 1, 1, errbuf);
                break;
            }
        }
    }
    pcap_freealldevs(alldevs);
    return fp;
}

static void __vk_netdrv_wpcap_netlink_thread(void *arg)
{
    const uint8_t bcast_addr[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    const uint8_t ipv4mcast_addr[] = {0x01, 0x00, 0x5e};
    const uint8_t ipv6mcast_addr[] = {0x33, 0x33};

    vsf_arch_irq_thread_t *irq_thread = arg;
    vk_netdrv_wpcap_t *wpcap_netdrv = container_of(irq_thread, vk_netdrv_wpcap_t, irq_thread);
    struct pcap_pkthdr pkt_header;
    const u_char *pkt_data;
    void *netbuf;

    __vsf_arch_irq_set_background(irq_thread);

    VSF_TCPIP_ASSERT(6 == wpcap_netdrv->macaddr.size);
    while (wpcap_netdrv->fp != NULL) {
        do {
            pkt_data = pcap_next(wpcap_netdrv->fp, &pkt_header);
        } while (NULL == pkt_data);

        // source is self, feedback packets?
        if (!memcmp(pkt_data + 6, wpcap_netdrv->macaddr.addr_buf, 6)) {
            continue;
        }

        // my mac or broad-cast/multi-case
        if (    memcmp(pkt_data, wpcap_netdrv->macaddr.addr_buf, 6)
            &&  memcmp(pkt_data, bcast_addr, 6)
            &&  memcmp(pkt_data, ipv6mcast_addr, 2)
            &&  (memcmp(pkt_data, ipv4mcast_addr, 3) || (pkt_data[3] & 0x80))) {
            continue;
        }

        __vsf_arch_irq_start(irq_thread);

#if VSF_NETDRV_WPCAP_CFG_TRACE == ENABLED
            vsf_trace(VSF_TRACE_DEBUG, "wpcap_rx:" VSF_TRACE_CFG_LINEEND);
            vsf_trace_buffer(VSF_TRACE_DEBUG, pkt_data, pkt_header.len);
#endif

            netbuf = vk_netdrv_alloc_buf(&wpcap_netdrv->use_as__vk_netdrv_t);
            if (netbuf != NULL) {
                vsf_mem_t mem;
                void *netbuf_cur = netbuf;
                int_fast32_t len = pkt_header.len;
                size_t cur_size;

                do {
                    netbuf_cur = vk_netdrv_read_buf(&wpcap_netdrv->use_as__vk_netdrv_t, netbuf_cur, &mem);
                    cur_size = min(mem.size, len);
                    memcpy(mem.buffer, pkt_data, cur_size);
                    len -= cur_size;
                    pkt_data += cur_size;
                } while ((netbuf_cur != NULL) && (len > 0));

                vk_netdrv_on_inputted(&wpcap_netdrv->use_as__vk_netdrv_t, netbuf, pkt_header.len);
            }
        __vsf_arch_irq_end(irq_thread, false);
    }

    __vsf_arch_irq_fini(irq_thread);
}

static vsf_err_t __vk_netdrv_wpcap_netlink_init(vk_netdrv_t *netdrv)
{
    vk_netdrv_wpcap_t *wpcap_netdrv = (vk_netdrv_wpcap_t *)netdrv;
    char errbuf[PCAP_ERRBUF_SIZE];

    wpcap_netdrv->fp = __vk_netdrv_wpcap_open(wpcap_netdrv->name);
    if (wpcap_netdrv->fp != NULL) {
        __vsf_arch_irq_init(&wpcap_netdrv->irq_thread, "netdrv_wpcap", __vk_netdrv_wpcap_netlink_thread, VSF_NETDRV_WPCAP_CFG_HW_PRIORITY);
        return VSF_ERR_NONE;
    }

    vsf_trace(VSF_TRACE_ERROR, "%s" VSF_TRACE_CFG_LINEEND, errbuf);
    return VSF_ERR_FAIL;
}

static vsf_err_t __vk_netdrv_wpcap_netlink_fini(vk_netdrv_t *netdrv)
{
    vk_netdrv_wpcap_t *wpcap_netdrv = (vk_netdrv_wpcap_t *)netdrv;

    if (wpcap_netdrv->fp != NULL) {
        pcap_breakloop(wpcap_netdrv->fp);
        pcap_close(wpcap_netdrv->fp);
        wpcap_netdrv->fp = NULL;
    }
    return VSF_ERR_NONE;
}

static bool __vk_netdrv_wpcap_netlink_can_output(vk_netdrv_t *netdrv)
{
    return true;
}

static vsf_err_t __vk_netdrv_wpcap_netlink_output(vk_netdrv_t *netdrv, void *netbuf)
{
    vk_netdrv_wpcap_t *wpcap_netdrv = (vk_netdrv_wpcap_t *)netdrv;
    uint8_t buffer[wpcap_netdrv->mtu];
    vsf_mem_t mem;

    if (vk_netdrv_read_buf(netdrv, netbuf, &mem) != NULL) {
        VSF_TCPIP_ASSERT(false);
        return VSF_ERR_FAIL;
    }

#if VSF_NETDRV_WPCAP_CFG_TRACE == ENABLED
    vsf_trace(VSF_TRACE_DEBUG, "wpcap_tx:" VSF_TRACE_CFG_LINEEND);
    vsf_trace_buffer(VSF_TRACE_DEBUG, mem.buffer, mem.size);
#endif

    VSF_TCPIP_ASSERT(sizeof(buffer) >= mem.size);
    memcpy(buffer, mem.buffer, mem.size);
    if (mem.size < 60) {
        memset(&buffer[mem.size], 0, 60 - mem.size);
        mem.size = 60;
    }
    pcap_sendpacket(wpcap_netdrv->fp, buffer, mem.size);
    return VSF_ERR_NONE;
}

#endif      // VSF_USE_TCPIP && VSF_USE_NETDRV_WPCAP
