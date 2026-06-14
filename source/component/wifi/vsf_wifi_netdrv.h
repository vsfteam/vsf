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

#ifndef __VSF_WIFI_NETDRV_H__
#define __VSF_WIFI_NETDRV_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_wifi_cfg.h"

#if     VSF_USE_WIFI == ENABLED                                                 \
    &&  VSF_USE_TCPIP == ENABLED && VSF_USE_LWIP == ENABLED

#include "./vsf_wifi.h"
#include "component/tcpip/vsf_tcpip.h"

/*============================ LOGGING HELPERS ===============================*/

#if VSF_WIFI_CFG_NETDRV_LOG_LEVEL >= 1
#   define vsf_wifi_netdrv_trace_error(...)    vsf_trace_error(__VA_ARGS__)
#else
#   define vsf_wifi_netdrv_trace_error(...)    ((void)0)
#endif

#if VSF_WIFI_CFG_NETDRV_LOG_LEVEL >= 2
#   define vsf_wifi_netdrv_trace_info(...)     vsf_trace_info(__VA_ARGS__)
#else
#   define vsf_wifi_netdrv_trace_info(...)     ((void)0)
#endif

#if VSF_WIFI_CFG_NETDRV_LOG_LEVEL >= 4
#   define vsf_wifi_netdrv_trace_debug(...)    vsf_trace_info(__VA_ARGS__)
#else
#   define vsf_wifi_netdrv_trace_debug(...)    ((void)0)
#endif

#if     defined(__VSF_WIFI_NETDRV_CLASS_IMPLEMENT)
#   undef __VSF_WIFI_NETDRV_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

/* Number of inbound frames the bus-context RX hook can stage for the netdrv
 * worker thread.  Each slot holds one fully built Ethernet frame. */
#ifndef VSF_WIFI_NETDRV_CFG_RX_QUEUE_SIZE
#   define VSF_WIFI_NETDRV_CFG_RX_QUEUE_SIZE    8
#endif

/* Ethernet frame staging size: 14-byte Ethernet II header + 1500 MTU. */
#define VSF_WIFI_NETDRV_ETH_FRAME_SIZE          (14 + 1500)

/* 802.11 TX frame size: QoS data header(26) + LLC/SNAP(8) + 1500 payload. */
#define VSF_WIFI_NETDRV_DOT11_FRAME_SIZE        (26 + 8 + 1500)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vk_netdrv_wifi_rxslot_t {
    uint16_t len;
    uint8_t  buf[VSF_WIFI_NETDRV_ETH_FRAME_SIZE];
} vk_netdrv_wifi_rxslot_t;

vsf_class(vk_netdrv_wifi_t) {
    public_member(
        implement(vk_netdrv_t)
        vsf_wifi_t *wifi;
    )
    private_member(
        vsf_eda_t *thread;
        uint8_t    bssid[6];        /* cached on link-up, used as TX RA/addr1 */

        /* Single-producer (bus EDA RX hook) / single-consumer (worker thread)
         * ring of pre-built Ethernet frames.  head is advanced by the worker,
         * tail by the RX hook; both indices are touched under vsf_protect. */
        vk_netdrv_wifi_rxslot_t rx_slots[VSF_WIFI_NETDRV_CFG_RX_QUEUE_SIZE];
        uint8_t    rx_head;
        uint8_t    rx_tail;

        /* Event flags raised in bus-EDA context (link up/down, rx ready) and
         * drained by the worker thread under vsf_protect. */
        uint8_t    pending;

        /* TX scratch (worker/tcpip-thread context only, single-threaded). */
        uint8_t    tx_frame[VSF_WIFI_NETDRV_DOT11_FRAME_SIZE];
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const struct vk_netlink_op_t vk_netdrv_wifi_netlink_op;

/*============================ PROTOTYPES ====================================*/

/* Wire a wifi-core instance to a netdrv instance and hand it to the lwIP
 * pnp layer.  After this call decrypted RX data frames and link up/down
 * events from `wifi` are bridged to lwIP; the stack auto-starts DHCP on
 * link-up via the existing vsf_pnp_on_netdrv_connected hook.  `netdrv` must
 * stay valid for the lifetime of the binding. */
void vsf_wifi_netdrv_start(vk_netdrv_wifi_t *netdrv, vsf_wifi_t *wifi);

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_WIFI && VSF_USE_TCPIP && VSF_USE_LWIP
#endif      // __VSF_WIFI_NETDRV_H__
