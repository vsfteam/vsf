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

#ifndef __VSF_WIFI_PRIV_H__
#define __VSF_WIFI_PRIV_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_wifi.h"

#if VSF_USE_WIFI == ENABLED

/*============================ TYPES =========================================*/

/*
 * The wifi layer is intentionally bus-agnostic, so vsf_wifi_t holds NO
 * USB / SDIO / SPI specifics.  The bus driver embeds vsf_wifi_t inside a
 * larger struct (e.g. vk_usbh_wifi_t) and recovers its outer instance via
 * vsf_container_of() inside the bus_ops implementations.
 */
struct vsf_wifi_t {
    const vsf_wifi_chip_drv_t *drv;
    const vsf_wifi_bus_ops_t  *bus_ops;
    vsf_eda_t                 *post_eda;
    vsf_wifi_attach_fail_t     attach_fail;

    uint8_t  channel;
    bool     is_ready;
    bool     disconnecting;     /* set in fini() to gate stale callbacks */
    uint8_t  mac[6];            /* populated by chip-driver EEPROM stage; */
                                /* zero until firmware_load chain finishes */

    /* ---- Scan state (wifi-driven hop scheduler) ---- */
    bool     scanning;
    uint8_t  scan_channel_idx;
    uint8_t  scan_num_channels;
    uint8_t  scan_channels[VSF_WIFI_CFG_SCAN_MAX_CHANNELS];
    uint16_t scan_dwell_ms;
#if VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER == ENABLED
    vsf_callback_timer_t scan_timer;
    /* Read-poll spacing timer.  Logically independent of scan_timer:
     * read_poll runs during firmware_load (is_ready == false) while scan
     * runs after on_ready, but they could in principle overlap if a
     * future chip op uses run_read_poll mid-flight, so we keep two. */
    vsf_callback_timer_t read_poll_timer;
#endif

    /* ---- MLME connection state (OPEN-system auth + association) ----
     *
     * Driven by the wifi layer (vsf_wifi.c).  mlme_state walks
     * IDLE -> AUTH -> ASSOC -> RUN; each handshake step arms mlme_timer and
     * retransmits up to mlme_retry times before giving up. */
    uint8_t  mlme_state;        /* WIFI_MLME_xxx                          */
    uint8_t  mlme_bssid[6];     /* target AP BSSID                        */
    uint8_t  mlme_ssid[33];     /* target SSID (for assoc-req SSID IE)    */
    uint8_t  mlme_ssid_len;
    uint8_t  mlme_channel;      /* target channel                         */
    uint8_t  mlme_retry;        /* retransmits left for current step      */
    uint16_t mlme_aid;          /* association id from assoc-resp         */
#if VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER == ENABLED
    vsf_callback_timer_t mlme_timer;
#endif

    /* ---- Script / blob dispatcher state ----
     *
     * Only one outstanding script or blob is allowed per wifi (the bus_ops
     * concurrency contract enforces the same limit at the bus level).  The
     * dispatcher walks the op array via the chained __script_step_done /
     * __blob_step_done callbacks, each of which feeds the next op to
     * bus_ops.
     */
    bool                       script_busy;
    bool                       script_is_blob;
    vsf_wifi_done_t            script_done;
    union {
        struct {
            const vsf_wifi_op_t *ops;
            uint16_t             count;
            uint16_t             idx;
        } script;
        struct {
            const uint8_t *data;
            uint32_t       len;
            uint32_t       offset;  /* used by the reg_write fallback path */
            uint16_t       base_reg;
            uint16_t       chunk_size;
        } blob;
        struct {
            uint16_t              reg;
            uint16_t              retry_left;  /* attempts remaining after
                                                * the current in-flight read */
            uint16_t              interval_ms;
            uint16_t              reserved;
            uint32_t              last_val;    /* read landing zone */
            vsf_wifi_match_fn_t   match;
        } read_poll;
    } s;

    /*
     * Backend-level chain slot.  When a chip op needs to issue several
     * scripts back-to-back (firmware_load = run_blob + post-fw script),
     * it stashes the *outer* `done` here, then internally chains via local
     * helpers.  Only one chain may be in flight at a time — same constraint
     * as the script dispatcher itself.
     */
    vsf_wifi_done_t            backend_chain_done;

    vsf_wifi_op_t              scratch_ops[VSF_WIFI_CFG_SCRATCH_OPS];
};

#endif // VSF_USE_WIFI
#endif // __VSF_WIFI_PRIV_H__
