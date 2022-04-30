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

#include "../../../vsf_tcpip_cfg.h"

#if VSF_USE_TCPIP == ENABLED && VSF_NETDRV_USE_WPCAP == ENABLED

#define __VSF_NETDRV_CLASS_INHERIT_NETLINK__
#define __VSF_NETDRV_WPCAP_CLASS_IMPLEMENT
#include "component/tcpip/vsf_tcpip.h"

#include "pcap.h"

#ifdef __WIN__
#   define BPF_MAJOR_VERSION
#   include <Packet32.h>
#   include <Ntddndis.h>
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_NETDRV_WPCAP_CFG_TRACE
#   define VSF_NETDRV_WPCAP_CFG_TRACE           DISABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

static vsf_err_t __vk_netdrv_wpcap_netlink_init(vk_netdrv_t *netdrv);
static vsf_err_t __vk_netdrv_wpcap_netlink_fini(vk_netdrv_t *netdrv);
static void * __vk_netdrv_wpcap_netlink_can_output(vk_netdrv_t *netdrv);
static vsf_err_t __vk_netdrv_wpcap_netlink_output(vk_netdrv_t *netdrv, void *slot, void *netbuf);

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
        vsf_trace_error("Error in pcap_findalldevs: %s\n", errbuf);
    } else {
        pcap_if_t *dev_to_connect = NULL;
        for (pcap_if_t *d = alldevs; d != NULL; d = d->next) {
            if (NULL == name) {
                if (    !(d->flags & PCAP_IF_LOOPBACK)
                    &&  (d->flags & PCAP_IF_UP) && (d->flags & PCAP_IF_RUNNING)
                    &&  ((d->flags & PCAP_IF_CONNECTION_STATUS) == PCAP_IF_CONNECTION_STATUS_CONNECTED)
                    &&  (d->addresses != NULL)) {
                    dev_to_connect = d;
                    break;
                }
            } else {
                if (strstr(d->description, name) || strstr(d->name, name)) {
                    dev_to_connect = d;
                    break;
                }
            }
        }
        if (dev_to_connect != NULL) {
            vsf_trace_info("wpcap: try to open %s at %s"VSF_TRACE_CFG_LINEEND, dev_to_connect->description, dev_to_connect->name);
            fp = pcap_open_live(dev_to_connect->name, 65536, 1, 1, errbuf);
        }
    }
    pcap_freealldevs(alldevs);
    return fp;
}

static void __vk_netdrv_wpcap_on_input(vk_netdrv_wpcap_t *wpcap_netdrv, uint8_t *buffer, uint32_t len)
{
    void *netbuf = vk_netdrv_alloc_buf(&wpcap_netdrv->use_as__vk_netdrv_t);
    if (netbuf != NULL) {
        vsf_mem_t mem;
        void *netbuf_cur = netbuf;
        uint32_t cur_len = len;
        size_t cur_size;

        do {
            netbuf_cur = vk_netdrv_read_buf(&wpcap_netdrv->use_as__vk_netdrv_t, netbuf_cur, &mem);
            cur_size = vsf_min(mem.size, cur_len);
            memcpy(mem.buffer, buffer, cur_size);
            cur_len -= cur_size;
            buffer += cur_size;
        } while ((netbuf_cur != NULL) && (cur_len > 0));

        vk_netdrv_on_inputted(&wpcap_netdrv->use_as__vk_netdrv_t, netbuf, len);
    }
}

static void __vk_netdrv_wpcap_netlink_thread(void *param)
{
    vk_netdrv_wpcap_t *wpcap_netdrv = param;
    vsf_evt_t evt;

    while (true) {
        evt = vsf_thread_wait();
        VSF_TCPIP_ASSERT(VSF_EVT_USER == evt);

        __vk_netdrv_wpcap_on_input(wpcap_netdrv, wpcap_netdrv->cur_buffer, wpcap_netdrv->cur_size);

        wpcap_netdrv->cur_buffer = NULL;
        __vsf_arch_irq_request_send(&wpcap_netdrv->irq_request);
    }
}

static void __vk_netdrv_wpcap_netlink_irqthread(void *arg)
{
    const uint8_t bcast_addr[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    const uint8_t ipv4mcast_addr[] = {0x01, 0x00, 0x5e};
    const uint8_t ipv6mcast_addr[] = {0x33, 0x33};

    vsf_arch_irq_thread_t *irq_thread = arg;
    vk_netdrv_wpcap_t *wpcap_netdrv = container_of(irq_thread, vk_netdrv_wpcap_t, irq_thread);
    struct pcap_pkthdr pkt_header;
    const u_char *pkt_data;

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
        vsf_trace_debug("wpcap_rx:" VSF_TRACE_CFG_LINEEND);
        vsf_trace_buffer(VSF_TRACE_DEBUG, (void *)pkt_data, pkt_header.len);
#endif

        if (vk_netdrv_feature(&wpcap_netdrv->use_as__vk_netdrv_t) & VSF_NETDRV_FEATURE_THREAD) {
            VSF_TCPIP_ASSERT(NULL == wpcap_netdrv->cur_buffer);
            wpcap_netdrv->cur_buffer = (uint8_t *)pkt_data;
            wpcap_netdrv->cur_size = (uint32_t)pkt_header.len;
            vsf_eda_post_evt(wpcap_netdrv->thread, VSF_EVT_USER);
            __vsf_arch_irq_end(irq_thread, false);

            __vsf_arch_irq_request_pend(&wpcap_netdrv->irq_request);
        } else {
            __vk_netdrv_wpcap_on_input(wpcap_netdrv, (uint8_t *)pkt_data, (uint32_t)pkt_header.len);
            __vsf_arch_irq_end(irq_thread, false);
        }
    }

    __vsf_arch_irq_fini(irq_thread);
}

static vsf_err_t __vk_netdrv_wpcap_netlink_init(vk_netdrv_t *netdrv)
{
    vk_netdrv_wpcap_t *wpcap_netdrv = (vk_netdrv_wpcap_t *)netdrv;

    wpcap_netdrv->fp = __vk_netdrv_wpcap_open(wpcap_netdrv->name);
    if (wpcap_netdrv->fp != NULL) {
#ifdef __WIN__
        // get mac address for WIN
        netdrv->macaddr.size = 6;
        int status = pcap_oid_get_request((pcap_t *)wpcap_netdrv->fp, OID_802_3_CURRENT_ADDRESS, &netdrv->macaddr.addr_buf, &netdrv->macaddr.size);
        VSF_UNUSED_PARAM(status);
        VSF_TCPIP_ASSERT(!status && (6 == netdrv->macaddr.size));
#endif
        netdrv->mac_header_size = TCPIP_ETH_HEADSIZE;
        netdrv->hwtype = TCPIP_ETH_HWTYPE;
        netdrv->mtu = 1500 + TCPIP_ETH_HEADSIZE;

        __vsf_arch_irq_request_init(&wpcap_netdrv->irq_request);
        if (vk_netdrv_feature(netdrv) & VSF_NETDRV_FEATURE_THREAD) {
            VSF_TCPIP_ASSERT(NULL == wpcap_netdrv->thread);
            wpcap_netdrv->thread = vk_netdrv_thread(netdrv, __vk_netdrv_wpcap_netlink_thread, wpcap_netdrv);
            VSF_TCPIP_ASSERT(wpcap_netdrv->thread != NULL);
        }
        __vsf_arch_irq_init(&wpcap_netdrv->irq_thread, "netdrv_wpcap", __vk_netdrv_wpcap_netlink_irqthread, VSF_NETDRV_WPCAP_CFG_HW_PRIORITY);
        return VSF_ERR_NONE;
    }

    vsf_trace_error("fail to open %s" VSF_TRACE_CFG_LINEEND, wpcap_netdrv->name);
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

static void * __vk_netdrv_wpcap_netlink_can_output(vk_netdrv_t *netdrv)
{
    return (void *)1;
}

static vsf_err_t __vk_netdrv_wpcap_netlink_output(vk_netdrv_t *netdrv, void *slot, void *netbuf)
{
    vk_netdrv_wpcap_t *wpcap_netdrv = (vk_netdrv_wpcap_t *)netdrv;
    uint8_t buffer[wpcap_netdrv->mtu];
    void *netbuf_cur = netbuf;
    uint_fast32_t tot_size = 0;
    vsf_mem_t mem;

    do {
        netbuf_cur = vk_netdrv_read_buf(netdrv, netbuf_cur, &mem);
        VSF_TCPIP_ASSERT(tot_size + mem.size < sizeof(buffer));
        memcpy(&buffer[tot_size], mem.buffer, mem.size);
        tot_size += mem.size;
    } while (netbuf_cur != NULL);

#if VSF_NETDRV_WPCAP_CFG_TRACE == ENABLED
    vsf_trace_debug("wpcap_tx:" VSF_TRACE_CFG_LINEEND);
    vsf_trace_buffer(VSF_TRACE_DEBUG, buffer, tot_size);
#endif

    pcap_sendpacket(wpcap_netdrv->fp, buffer, tot_size);
    vk_netdrv_on_outputted(&wpcap_netdrv->use_as__vk_netdrv_t, netbuf, VSF_ERR_NONE);
    return VSF_ERR_NONE;
}

#endif      // VSF_USE_TCPIP && VSF_NETDRV_USE_WPCAP
