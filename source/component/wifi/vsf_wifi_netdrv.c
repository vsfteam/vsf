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

#include "./vsf_wifi_cfg.h"

#if     VSF_USE_WIFI == ENABLED                                                 \
    &&  VSF_USE_TCPIP == ENABLED && VSF_USE_LWIP == ENABLED

#define __VSF_NETDRV_CLASS_INHERIT_NETLINK__
#define __VSF_WIFI_NETDRV_CLASS_IMPLEMENT
#include "./vsf_wifi_netdrv.h"
#include "./vsf_wifi_priv.h"

/*============================ MACROS ========================================*/

/* Worker-thread wake reasons, raised (OR-ed) in bus-EDA context by the wifi
 * netif hooks and drained by the worker under vsf_protect. */
#define __WIFI_NETDRV_PEND_RX           (1 << 0)
#define __WIFI_NETDRV_PEND_LINK_UP      (1 << 1)
#define __WIFI_NETDRV_PEND_LINK_DOWN    (1 << 2)

/* 802.11 LLC/SNAP header that prefixes the payload of a data frame. */
#define __WIFI_NETDRV_SNAP_SIZE         8

/* Ethernet II header size (DA + SA + ethertype). */
#define __WIFI_NETDRV_ETH_HDR_SIZE      14

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

static vsf_err_t __vk_netdrv_wifi_netlink_init(vk_netdrv_t *netdrv);
static vsf_err_t __vk_netdrv_wifi_netlink_fini(vk_netdrv_t *netdrv);
static void * __vk_netdrv_wifi_netlink_can_output(vk_netdrv_t *netdrv);
static vsf_err_t __vk_netdrv_wifi_netlink_output(vk_netdrv_t *netdrv, void *slot, void *netbuf);

static void __vk_netdrv_wifi_on_rx(void *param, vsf_wifi_t *wifi, uint8_t *frame, uint16_t len);
static void __vk_netdrv_wifi_on_link_up(void *param, vsf_wifi_t *wifi, const vsf_wifi_link_info_t *info);
static void __vk_netdrv_wifi_on_link_down(void *param, vsf_wifi_t *wifi, uint8_t reason);

/*============================ GLOBAL VARIABLES ==============================*/

const struct vk_netlink_op_t vk_netdrv_wifi_netlink_op = {
    .init       = __vk_netdrv_wifi_netlink_init,
    .fini       = __vk_netdrv_wifi_netlink_fini,
    .can_output = __vk_netdrv_wifi_netlink_can_output,
    .output     = __vk_netdrv_wifi_netlink_output,
};

/*============================ LOCAL VARIABLES ===============================*/

static const vsf_wifi_netif_ops_t __vk_netdrv_wifi_netif_ops = {
    .on_rx        = __vk_netdrv_wifi_on_rx,
    .on_link_up   = __vk_netdrv_wifi_on_link_up,
    .on_link_down = __vk_netdrv_wifi_on_link_down,
};

/*============================ IMPLEMENTATION ================================*/

/* ------------------------------------------------------------------------- *
 * Frame conversion helpers.
 *
 * The wifi core hands us a naked, decrypted 802.11 data frame (FC .. payload)
 * on RX and expects the same on TX (it adds CCMP encap internally).  lwIP only
 * speaks Ethernet II.  These two helpers bridge the two framings.
 * ------------------------------------------------------------------------- */

/* 802.11 data (FromDS, from AP) -> Ethernet II.  Returns the Ethernet frame
 * length written into `eth` (capacity `eth_cap`), or 0 if the input is not a
 * forwardable data frame.  Layout of an AP->STA data frame:
 *   addr1 = DA (our STA MAC), addr2 = BSSID, addr3 = SA.
 *   QoS subtype carries a 2-byte QoS control field (hdr 26 vs 24). */
static uint16_t __wifi_dot11_to_eth(const uint8_t *frame, uint16_t len,
        uint8_t *eth, uint16_t eth_cap)
{
    if (len < 24 + __WIFI_NETDRV_SNAP_SIZE) {
        return 0;
    }

    uint8_t  subtype = (frame[0] >> 4) & 0x0F;
    uint16_t hdr_len = (subtype & 0x08) ? 26 : 24;
    if (len <= hdr_len + __WIFI_NETDRV_SNAP_SIZE) {
        return 0;
    }

    const uint8_t *llc = frame + hdr_len;
    static const uint8_t snap_hdr[6] = { 0xAA, 0xAA, 0x03, 0x00, 0x00, 0x00 };
    if (memcmp(llc, snap_hdr, 6) != 0) {
        return 0;
    }

    const uint8_t *ethertype = llc + 6;             /* 2 bytes              */
    const uint8_t *payload   = llc + __WIFI_NETDRV_SNAP_SIZE;
    uint16_t payload_len = (uint16_t)(len - hdr_len - __WIFI_NETDRV_SNAP_SIZE);

    if ((uint32_t)(__WIFI_NETDRV_ETH_HDR_SIZE + payload_len) > eth_cap) {
        return 0;
    }

    memcpy(&eth[0], &frame[4],  6);                 /* DA = addr1           */
    memcpy(&eth[6], &frame[16], 6);                 /* SA = addr3           */
    eth[12] = ethertype[0];
    eth[13] = ethertype[1];
    memcpy(&eth[__WIFI_NETDRV_ETH_HDR_SIZE], payload, payload_len);

    return (uint16_t)(__WIFI_NETDRV_ETH_HDR_SIZE + payload_len);
}

/* Ethernet II -> 802.11 QoS Data (ToDS, to AP).  Returns the 802.11 frame
 * length written into `dot11` (capacity `dot11_cap`), or 0 on failure.
 *
 * VSF always advertises WMM/QoS in the association request, so the BSS is
 * treated as a QoS BSS.  In a QoS BSS, uplink data frames must use the QoS
 * Data subtype (0x88) with a 2-byte QoS Control field; plain Data frames are
 * not guaranteed to be accepted or routed to the correct AC by the AP.
 *
 * Note: this helper is generic and not chip-specific.  The MT76 chip driver
 * separately converts unprotected QoS Data EAPOL-Key frames to plain Data
 * to work around a firmware Duration-field quirk, but that does not apply
 * to normal post-handshake data frames. */
static uint16_t __wifi_eth_to_dot11(const uint8_t *bssid,
        const uint8_t *eth, uint16_t eth_len,
        uint8_t *dot11, uint16_t dot11_cap)
{
    if (eth_len < __WIFI_NETDRV_ETH_HDR_SIZE) {
        return 0;
    }

    uint16_t payload_len = (uint16_t)(eth_len - __WIFI_NETDRV_ETH_HDR_SIZE);
    uint16_t total = (uint16_t)(26 + __WIFI_NETDRV_SNAP_SIZE + payload_len);
    if (total > dot11_cap) {
        return 0;
    }

    memset(dot11, 0, 26);
    dot11[0] = 0x88;                                /* QoS Data             */
    dot11[1] = 0x01;                                /* ToDS                 */
    memcpy(&dot11[4],  bssid,    6);                /* addr1 = BSSID (RA)   */
    memcpy(&dot11[10], &eth[6],  6);                /* addr2 = SA  (TA)     */
    memcpy(&dot11[16], &eth[0],  6);                /* addr3 = DA           */
    /* dot11[24..25] = QoS Control (TID 0) */

    uint8_t *llc = &dot11[26];
    llc[0] = 0xAA; llc[1] = 0xAA; llc[2] = 0x03;
    llc[3] = 0x00; llc[4] = 0x00; llc[5] = 0x00;
    llc[6] = eth[12];                               /* ethertype hi         */
    llc[7] = eth[13];                               /* ethertype lo         */

    memcpy(&dot11[26 + __WIFI_NETDRV_SNAP_SIZE],
           &eth[__WIFI_NETDRV_ETH_HDR_SIZE], payload_len);

    return total;
}

/* ------------------------------------------------------------------------- *
 * Worker thread: runs in a stack-owner (real-thread) context so it can drive
 * the lwIP netdrv adapter, whose callbacks all assert stack ownership.  The
 * bus-EDA hooks below only stage data / events and kick this thread.
 * ------------------------------------------------------------------------- */

static void __vk_netdrv_wifi_drain_rx(vk_netdrv_wifi_t *wifi_netdrv)
{
    while (true) {
        vsf_protect_t orig = vsf_protect_sched();
        if (wifi_netdrv->rx_head == wifi_netdrv->rx_tail) {
            vsf_unprotect_sched(orig);
            break;
        }
        uint8_t idx = wifi_netdrv->rx_head;
        vsf_unprotect_sched(orig);

        vk_netdrv_wifi_rxslot_t *slot = &wifi_netdrv->rx_slots[idx];
        void *netbuf = vk_netdrv_alloc_buf(&wifi_netdrv->use_as__vk_netdrv_t);
        if (netbuf != NULL) {
            vsf_mem_t mem;
            void *netbuf_cur = netbuf;
            uint16_t cur_off = 0;

            do {
                netbuf_cur = vk_netdrv_read_buf(&wifi_netdrv->use_as__vk_netdrv_t,
                        netbuf_cur, &mem);
                uint16_t cur_size = (uint16_t)vsf_min(mem.size, slot->len - cur_off);
                memcpy(mem.buffer, &slot->buf[cur_off], cur_size);
                cur_off += cur_size;
            } while ((netbuf_cur != NULL) && (cur_off < slot->len));

            vk_netdrv_on_inputted(&wifi_netdrv->use_as__vk_netdrv_t, netbuf, slot->len);
        }

        orig = vsf_protect_sched();
        wifi_netdrv->rx_head = (uint8_t)((idx + 1) % VSF_WIFI_NETDRV_CFG_RX_QUEUE_SIZE);
        vsf_unprotect_sched(orig);
    }
}

static void __vk_netdrv_wifi_thread(void *param)
{
    vk_netdrv_wifi_t *wifi_netdrv = param;

    while (true) {
        vsf_evt_t evt = vsf_thread_wait();
        VSF_WIFI_ASSERT(VSF_EVT_USER == evt);

        vsf_protect_t orig = vsf_protect_sched();
        uint8_t pending = wifi_netdrv->pending;
        wifi_netdrv->pending = 0;
        vsf_unprotect_sched(orig);

        if (pending & __WIFI_NETDRV_PEND_LINK_UP) {
            vsf_wifi_netdrv_trace_info("wifi-netdrv: worker LINK_UP -> vk_netdrv_connect" VSF_TRACE_CFG_LINEEND);
            vsf_err_t cerr = vk_netdrv_connect(&wifi_netdrv->use_as__vk_netdrv_t);
            vsf_wifi_netdrv_trace_info("wifi-netdrv: vk_netdrv_connect err=%d" VSF_TRACE_CFG_LINEEND, (int)cerr);
            (void)cerr;
        }
        if (pending & __WIFI_NETDRV_PEND_RX) {
            __vk_netdrv_wifi_drain_rx(wifi_netdrv);
        }
        if (pending & __WIFI_NETDRV_PEND_LINK_DOWN) {
            vk_netdrv_disconnect(&wifi_netdrv->use_as__vk_netdrv_t);
        }
    }
}

/* ------------------------------------------------------------------------- *
 * netlink_op: the netdrv <-> wifi-hardware contract.  init/output run in the
 * stack-owner context (init via on_connect, output via lwIP linkoutput on the
 * tcpip thread).
 * ------------------------------------------------------------------------- */

static vsf_err_t __vk_netdrv_wifi_netlink_init(vk_netdrv_t *netdrv)
{
    vk_netdrv_wifi_t *wifi_netdrv = (vk_netdrv_wifi_t *)netdrv;
    const uint8_t *mac = vsf_wifi_get_mac(wifi_netdrv->wifi);

    netdrv->macaddr.size = 6;
    memcpy(netdrv->macaddr.addr_buf, mac, 6);
    netdrv->mac_header_size = TCPIP_ETH_HEADSIZE;
    netdrv->hwtype          = TCPIP_ETH_HWTYPE;
    netdrv->mtu             = 1500 + TCPIP_ETH_HEADSIZE;

    return VSF_ERR_NONE;
}

static vsf_err_t __vk_netdrv_wifi_netlink_fini(vk_netdrv_t *netdrv)
{
    return VSF_ERR_NONE;
}

static void * __vk_netdrv_wifi_netlink_can_output(vk_netdrv_t *netdrv)
{
    /* The wifi TX path is fire-and-forget (URB pool owned by the bus driver),
     * so a slot is always available. */
    return (void *)1;
}

static vsf_err_t __vk_netdrv_wifi_netlink_output(vk_netdrv_t *netdrv, void *slot, void *netbuf)
{
    vk_netdrv_wifi_t *wifi_netdrv = (vk_netdrv_wifi_t *)netdrv;
    uint8_t  eth[VSF_WIFI_NETDRV_ETH_FRAME_SIZE];
    void    *netbuf_cur = netbuf;
    uint16_t eth_len = 0;
    vsf_mem_t mem;

    /* Flatten the pbuf chain into a contiguous Ethernet frame. */
    do {
        netbuf_cur = vk_netdrv_read_buf(netdrv, netbuf_cur, &mem);
        if ((uint32_t)eth_len + mem.size > sizeof(eth)) {
            break;
        }
        memcpy(&eth[eth_len], mem.buffer, mem.size);
        eth_len += (uint16_t)mem.size;
    } while (netbuf_cur != NULL);

    /* Diagnostic: dump outgoing Ethernet frame (DA/SA/ethertype, and for IPv4
     * the L4 proto + UDP ports) to confirm DHCP DISCOVER is emitted correctly. */
    {
        static uint32_t __tx_cnt = 0;
        if (++__tx_cnt <= 16) {
            if ((eth_len >= 38) && (eth[12] == 0x08) && (eth[13] == 0x00)) {
                uint8_t  ihl   = (eth[14] & 0x0F) * 4;
                uint8_t  proto = eth[23];
                uint16_t sport = (eth[14 + ihl] << 8) | eth[14 + ihl + 1];
                uint16_t dport = (eth[14 + ihl + 2] << 8) | eth[14 + ihl + 3];
                vsf_wifi_netdrv_trace_info("wifi-netdrv: TX #%u eth_len=%u DA=%02X%02X%02X%02X%02X%02X"
                        " SA=%02X%02X%02X%02X%02X%02X IPv4 proto=%u sport=%u dport=%u"
                        VSF_TRACE_CFG_LINEEND,
                        (unsigned)__tx_cnt, (unsigned)eth_len,
                        eth[0],eth[1],eth[2],eth[3],eth[4],eth[5],
                        eth[6],eth[7],eth[8],eth[9],eth[10],eth[11],
                        (unsigned)proto, (unsigned)sport, (unsigned)dport);
                (void)proto; (void)sport; (void)dport;
            } else {
                vsf_wifi_netdrv_trace_info("wifi-netdrv: TX #%u eth_len=%u DA=%02X%02X%02X%02X%02X%02X"
                        " et=%02X%02X" VSF_TRACE_CFG_LINEEND,
                        (unsigned)__tx_cnt, (unsigned)eth_len,
                        eth[0],eth[1],eth[2],eth[3],eth[4],eth[5],
                        eth[12], eth[13]);
            }
        }
    }

    uint16_t dot11_len = __wifi_eth_to_dot11(wifi_netdrv->bssid, eth, eth_len,
            wifi_netdrv->tx_frame, sizeof(wifi_netdrv->tx_frame));
    if (dot11_len > 0) {
        vsf_wifi_tx(wifi_netdrv->wifi, wifi_netdrv->tx_frame, dot11_len);
    }

    vk_netdrv_on_outputted(netdrv, netbuf, VSF_ERR_NONE);
    return VSF_ERR_NONE;
}

/* ------------------------------------------------------------------------- *
 * vsf_wifi_netif_ops: invoked by the wifi core in bus-EDA context.  These
 * stage work and wake the worker thread; they never touch lwIP directly.
 * ------------------------------------------------------------------------- */

static void __vk_netdrv_wifi_on_rx(void *param, vsf_wifi_t *wifi,
        uint8_t *frame, uint16_t len)
{
    vk_netdrv_wifi_t *wifi_netdrv = param;

    vsf_protect_t orig = vsf_protect_sched();
    uint8_t next = (uint8_t)((wifi_netdrv->rx_tail + 1) % VSF_WIFI_NETDRV_CFG_RX_QUEUE_SIZE);
    if (next == wifi_netdrv->rx_head) {
        /* Queue full — drop (lwIP / TCP will recover via retransmit). */
        vsf_unprotect_sched(orig);
        return;
    }
    vk_netdrv_wifi_rxslot_t *slot = &wifi_netdrv->rx_slots[wifi_netdrv->rx_tail];
    vsf_unprotect_sched(orig);

    uint16_t eth_len = __wifi_dot11_to_eth(frame, len, slot->buf, sizeof(slot->buf));
    if (eth_len == 0) {
        return;
    }
    slot->len = eth_len;

    {
        /* Make the bridged Ethernet frames visible: et=0x0800 IPv4, 0x0806 ARP.
         * DHCP OFFER/ACK ride IPv4/UDP, so this confirms data reaches lwIP.
         * For IPv4/UDP also dump proto + src/dst ports + dst IP so we can tell
         * a DHCP OFFER (sport 67 -> dport 68) from other traffic. */
        static uint32_t __rx_cnt = 0;
        if (++__rx_cnt <= 16) {
            uint8_t *b = slot->buf;
            if ((b[12] == 0x08) && (b[13] == 0x00) && (eth_len >= 38)) {
                uint8_t  ihl   = (b[14] & 0x0F) * 4;
                uint8_t  proto = b[23];
                uint16_t sport = (b[14 + ihl] << 8) | b[14 + ihl + 1];
                uint16_t dport = (b[14 + ihl + 2] << 8) | b[14 + ihl + 3];
                vsf_wifi_netdrv_trace_info("wifi-netdrv: rx->lwIP #%u eth_len=%u IPv4"
                        " proto=%u sport=%u dport=%u dst=%u.%u.%u.%u"
                        VSF_TRACE_CFG_LINEEND, (unsigned)__rx_cnt,
                        (unsigned)eth_len, proto, sport, dport,
                        b[30], b[31], b[32], b[33]);
                (void)proto; (void)sport; (void)dport;
            } else {
                vsf_wifi_netdrv_trace_info("wifi-netdrv: rx->lwIP #%u eth_len=%u et=%02X%02X"
                        VSF_TRACE_CFG_LINEEND, (unsigned)__rx_cnt,
                        (unsigned)eth_len, b[12], b[13]);
            }
        }
    }

    orig = vsf_protect_sched();
    wifi_netdrv->rx_tail = next;
    wifi_netdrv->pending |= __WIFI_NETDRV_PEND_RX;
    vsf_unprotect_sched(orig);

    vsf_eda_post_evt(wifi_netdrv->thread, VSF_EVT_USER);
}

static void __vk_netdrv_wifi_on_link_up(void *param, vsf_wifi_t *wifi,
        const vsf_wifi_link_info_t *info)
{
    vk_netdrv_wifi_t *wifi_netdrv = param;

    vsf_wifi_netdrv_trace_info("wifi-netdrv: on_link_up hook fired, posting to worker" VSF_TRACE_CFG_LINEEND);
    memcpy(wifi_netdrv->bssid, info->bssid, 6);

    vsf_protect_t orig = vsf_protect_sched();
    wifi_netdrv->pending |= __WIFI_NETDRV_PEND_LINK_UP;
    vsf_unprotect_sched(orig);

    vsf_eda_post_evt(wifi_netdrv->thread, VSF_EVT_USER);
}

static void __vk_netdrv_wifi_on_link_down(void *param, vsf_wifi_t *wifi, uint8_t reason)
{
    vk_netdrv_wifi_t *wifi_netdrv = param;

    vsf_protect_t orig = vsf_protect_sched();
    wifi_netdrv->pending |= __WIFI_NETDRV_PEND_LINK_DOWN;
    vsf_unprotect_sched(orig);

    vsf_eda_post_evt(wifi_netdrv->thread, VSF_EVT_USER);
}

/* ------------------------------------------------------------------------- *
 * Public wiring.
 * ------------------------------------------------------------------------- */

void vsf_wifi_netdrv_start(vk_netdrv_wifi_t *netdrv, vsf_wifi_t *wifi)
{
    VSF_WIFI_ASSERT((netdrv != NULL) && (wifi != NULL));
    vsf_wifi_netdrv_trace_info("wifi-netdrv: start (bridge wifi->lwIP)" VSF_TRACE_CFG_LINEEND);

    memset(netdrv, 0, sizeof(*netdrv));
    netdrv->wifi = wifi;

    vk_netdrv_set_netlink_op(&netdrv->use_as__vk_netdrv_t,
            &vk_netdrv_wifi_netlink_op, netdrv);

    /* Let the application pnp layer bind the lwIP netif + adapter ops. */
    vsf_pnp_on_netdrv_new(&netdrv->use_as__vk_netdrv_t);
    vk_netdrv_prepare(&netdrv->use_as__vk_netdrv_t);

    /* Worker thread (stack-owner context) — created after prepare so the
     * adapter thread factory is available. */
    netdrv->thread = vk_netdrv_thread(&netdrv->use_as__vk_netdrv_t,
            __vk_netdrv_wifi_thread, netdrv);

    /* Route decrypted RX data frames and link events from the wifi core to
     * this driver. */
    vsf_wifi_netdrv_attach(wifi, &__vk_netdrv_wifi_netif_ops, netdrv);
}

#endif      // VSF_USE_WIFI && VSF_USE_TCPIP && VSF_USE_LWIP
