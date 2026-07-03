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

#ifndef VSF_WIFI_CFG_CCMP_BUF_SIZE
#   define VSF_WIFI_CFG_CCMP_BUF_SIZE      1600
#endif

/*============================ TYPES =========================================*/

/*
 * Raw WiFi radio instance embedded in vsf_wifi_t.
 *
 * The public vsf_wifi_radio.h header only forward-declares this type; the
 * full definition is private so the layout can carry internal adapter state.
 */
struct vsf_wifi_radio_t {
    const vsf_wifi_radio_ops_t *ops;
    vsf_wifi_radio_rx_cb_t      rx_cb;
    void                       *rx_param;
    vsf_wifi_t                 *wifi;

    /* Internal adapter state: stores the user `done` callback when a raw
     * radio operation is delegated to a vsf_wifi_done_t-style hook. */
    vsf_wifi_radio_done_t       adapter_done;
};

/*
 * The wifi layer is chip-agnostic but knows about the optional register-bus
 * helper used by register-based chips (e.g. RT2X00).  reg_bus may be NULL for
 * chips that use a command/event-based bus (e.g. MediaTek mt76).  The bus
 * driver embeds vsf_wifi_t inside a larger struct (e.g. vk_usbh_wifi_t) and
 * recovers its outer instance via vsf_container_of().
 */
struct vsf_wifi_t {
    const vsf_wifi_chip_drv_t *drv;
    const vsf_wifi_reg_bus_t  *reg_bus;
    vsf_eda_t                 *post_eda;
    vsf_wifi_attach_fail_t     attach_fail;

    /* Optional network-stack backend (lwIP netdrv adapter, etc.) bound at
     * runtime via vsf_wifi_netdrv_attach().  When netif_ops != NULL the core
     * routes decrypted RX data frames and link up/down events here instead
     * of the weak vsf_wifi_on_* hooks. */
    const vsf_wifi_netif_ops_t *netif_ops;
    void                       *netif_param;

    uint8_t  channel;
    bool     is_ready;
    bool     bss_wmm;           /* current BSS supports WMM/QoS (set on assoc) */
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
    uint8_t  connect_bw;        /* WIFI_BW_xxx for next connect/set_channel*/
    uint8_t  mlme_retry;        /* retransmits left for current step      */

    /* Deferred hardware key install.  When the chip crypto backend reports
     * VSF_ERR_NOT_AVAILABLE because a previous register script is still in
     * flight, the handshake is retried automatically once that script
     * completes.  This avoids a race between the async connect script and the
     * 4-way handshake on register-based chips such as RT5572. */
    struct {
        bool ptk_pending;
        bool gtk_pending;
    } key_install;
    uint16_t mlme_aid;          /* association id from assoc-resp         */
#if VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER == ENABLED
    vsf_callback_timer_t mlme_timer;
#endif

#if VSF_WIFI_USE_WPA == ENABLED
    /* ---- WPA2-PSK security context ----
     *
     * wpa_auth is captured by vsf_wifi_set_auth_mode(); auth_mode == OPEN
     * means no RSN (the assoc-req carries no RSN IE and no 4-way handshake
     * runs).  wpa_rsn_ie holds the STA RSN IE bytes sent in the assoc-req so
     * the 4-way handshake M2 can echo them unchanged.  Handshake-specific
     * fields (PTK / nonces / GTK / replay / substate) are added by the WPA
     * state machine. */
    vsf_wifi_auth_cfg_t wpa_auth;
    uint8_t  wpa_rsn_ie[24];
    uint8_t  wpa_rsn_ie_len;

    /* 4-way handshake working state (vsf_wifi_wpa.c).  The PMK lives in
     * wpa_auth.psk; PTK = KCK(16)|KEK(16)|TK(16) is derived on M1.  anonce /
     * snonce feed the PRF; gtk holds the group key unwrapped from M3.
     * replay echoes the AP's EAPOL-Key replay counter; ptk_valid gates the
     * CCMP data path until the handshake installs keys. */
    uint8_t  wpa_ptk[48];
    uint8_t  wpa_anonce[32];
    uint8_t  wpa_snonce[32];
    uint8_t  wpa_gtk[32];
    uint8_t  wpa_gtk_len;
    uint8_t  wpa_gtk_keyidx;
    uint8_t  wpa_replay[8];
    bool     wpa_ptk_valid;

    /* CCMP data path (vsf_wifi.c).  wpa_tx_pn is the 48-bit packet-number
     * counter (little-endian, pn[0] = LSB) advanced once per encrypted TX
     * MPDU; it seeds the CCMP header and CCM nonce.  wpa_hw_crypto is set
     * when a chip crypto_ops backend installed the keys, in which case the
     * software CCMP encap/decap is bypassed (the chip does it in-line). */
    uint8_t  wpa_tx_pn[6];
    bool     wpa_hw_crypto;

    /* Software CCMP working buffers.  Placed in the instance instead of local
     * static arrays so the code remains re-entrant when multiple wifi instances
     * exist or when an async path could overlap. */
    uint32_t wpa_ccmp_tx_buf[(VSF_WIFI_CFG_CCMP_BUF_SIZE + 3) / 4];
    uint32_t wpa_ccmp_rx_buf[(VSF_WIFI_CFG_CCMP_BUF_SIZE + 3) / 4];
#if VSF_WIFI_CFG_CCMP_TX_SELF_VERIFY == ENABLED
    uint32_t wpa_ccmp_verify_buf[(VSF_WIFI_CFG_CCMP_BUF_SIZE + 3) / 4];
    uint8_t  wpa_ccmp_verify_cnt;
#endif
    uint8_t  wpa_ccmp_fail_cnt;
#endif

    /* ---- Script / blob dispatcher state ----
     *
     * Only one outstanding script or blob is allowed per wifi (the reg_bus
     * concurrency contract enforces the same limit at the bus level).  The
     * dispatcher walks the op array via the chained __script_step_done /
     * __blob_step_done callbacks, each of which feeds the next op to
     * reg_bus.
     */
    bool                       script_busy;
    bool                       script_is_blob;
    vsf_wifi_done_t            script_done;
    union {
        struct {
            const vsf_wifi_reg_op_t *ops;
            uint16_t             count;
            uint16_t             idx;
        } script;
        struct {
            const uint8_t *data;
            uint32_t       len;
            uint32_t       offset;  /* used by the reg_write fallback path */
            uint32_t       base_reg;
            uint16_t       chunk_size;
        } blob;
        struct {
            uint32_t              reg;
            uint16_t              retry_left;  /* attempts remaining after
                                                * the current in-flight read */
            uint16_t              interval_ms;
            uint16_t              reserved;
            uint32_t              last_val;    /* read landing zone */
            vsf_wifi_reg_match_fn_t   match;
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

    /* BSS Max Idle Period from assoc-resp IE 90 (units of 1000 TU).
     * 0 means the AP did not advertise it. */
    uint16_t                   bss_max_idle_period;

#if     (VSF_WIFI_CFG_KEEPALIVE_PERIOD_MS > 0)                    \
    &&  (VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER == ENABLED)
    /* 802.11 null-data-frame keepalive state.  Started on link-up and stopped
     * on link-down / fini to keep the AP from deauthenticating an idle STA. */
    vsf_callback_timer_t       keepalive_timer;
    uint32_t                   keepalive_period_ms;
#endif

    vsf_wifi_reg_op_t              scratch_ops[VSF_WIFI_CFG_SCRATCH_OPS];

    /* Chip-driver private data pointer.  Set by the bus driver after
     * vsf_wifi_init() for command/event-based chips that need per-instance
     * state beyond the generic wifi struct (e.g. MediaTek mt76).  The generic
     * layer never touches this field. */
    void                          *chip_priv;

    /* Chip-specific bus operations.
     *
     * This abstracts the *communication semantics* between the chip driver and
     * the host, not the underlying physical bus.  Register-based chips (e.g.
     * Ralink RT28xx) use reg_bus above: the chip driver only knows "read/write
     * a register", and the bus driver implements that with USB ep0 vendor
     * requests, SDIO CMD53, etc.
     *
     * Command/event-based chips (e.g. MediaTek mt76) store a chip-defined
     * vtable here (cast by the chip driver to the appropriate type).  The
     * operations are named after what the *chip* does (send MCU command,
     * submit TX frame, start RX, ...), so the same chip driver can plug into
     * different physical buses without modification.
     *
     * Design rule: bus_ops MUST NOT contain physical-bus-specific primitives
     * such as USB ep0 vendor_request.  If a chip genuinely needs a bus-specific
     * control primitive, define a chip-private vtable whose FIRST member is
     * vsf_wifi_reg_bus_t (when register semantics exist) or another chip-defined
     * base, and place the private primitive after the shared part.  The bus
     * driver supplies the private vtable; the chip driver casts bus_ops to the
     * chip-private type. */
    const void                    *bus_ops;

    /* ---- Raw WiFi radio state ----
     *
     * When raw_radio_active is true the instance is in raw-radio mode and the
     * standard MLME/scan/WPA state machine is gated out.  Received frames are
     * delivered to raw_radio.rx_cb instead of the normal netif/weak hooks. */
    bool                           raw_radio_active;
    vsf_wifi_radio_t               raw_radio;
};

#if VSF_WIFI_USE_WPA == ENABLED
/* CCMP-encrypt a plaintext data MPDU using a caller-supplied 48-bit PN
 * counter (little-endian, pn[0] = LSB).  If `pn` is NULL the function falls
 * back to wifi->wpa_tx_pn.  Returns the encrypted MPDU length, or 0 on
 * failure.  Used by chip drivers that need to software-encrypt specific
 * frames (e.g. MT76 broadcast/multicast) while leaving unicast traffic to
 * the hardware crypto backend. */
extern uint16_t vsf_wifi_ccmp_encap_with_pn(vsf_wifi_t *wifi,
        const uint8_t *frame, uint16_t len, uint8_t *out, uint16_t cap,
        uint8_t pn[6]);
#endif

#endif // VSF_USE_WIFI
#endif // __VSF_WIFI_PRIV_H__
