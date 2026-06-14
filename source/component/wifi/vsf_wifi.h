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

#ifndef __VSF_WIFI_H__
#define __VSF_WIFI_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_wifi_cfg.h"

#if VSF_USE_WIFI == ENABLED

#include "kernel/vsf_kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

/*
 * Auth / cipher constants (kept stable across chips so the application can
 * pass a single vsf_wifi_auth_cfg_t regardless of the underlying NIC).
 */
enum {
    WIFI_AUTH_OPEN              = 0,
    WIFI_AUTH_WEP               = 1,
    WIFI_AUTH_WPA_PSK           = 2,
    WIFI_AUTH_WPA2_PSK          = 3,
    WIFI_AUTH_WPA_WPA2_PSK      = 4,
    WIFI_AUTH_WPA2_ENTERPRISE   = 5,
};

enum {
    WIFI_CIPHER_NONE            = 0,
    WIFI_CIPHER_WEP40           = 1,
    WIFI_CIPHER_WEP104          = 2,
    WIFI_CIPHER_TKIP            = 3,
    WIFI_CIPHER_CCMP            = 4,
    WIFI_CIPHER_TKIP_CCMP       = 5,
};

#define WIFI_LINK_FLAG_CONNECTED        (1 << 0)
#define WIFI_LINK_FLAG_AUTHORIZED       (1 << 1)

#define WIFI_SCAN_FLAG_PRIVACY          (1 << 0)
#define WIFI_SCAN_FLAG_WPS              (1 << 1)
#define WIFI_SCAN_FLAG_WPA              (1 << 2)
#define WIFI_SCAN_FLAG_WPA2             (1 << 3)

/*
 * MLME connection state (OPEN-system auth + association).  Driven entirely
 * by the wifi layer; the chip layer only strips RX descriptors and routes
 * mgmt frames to vsf_wifi_mlme_rx().
 */
enum {
    WIFI_MLME_IDLE      = 0,    /* not connecting / disconnected         */
    WIFI_MLME_AUTH      = 1,    /* auth-req sent, awaiting auth-resp      */
    WIFI_MLME_ASSOC     = 2,    /* assoc-req sent, awaiting assoc-resp    */
    WIFI_MLME_4WAY      = 4,    /* associated, WPA2 4-way handshake running */
    WIFI_MLME_RUN       = 3,    /* associated (link up)                  */
};

/*
 * link-down reason codes passed to vsf_wifi_on_link_down().  Low values
 * mirror common 802.11 reason codes; high values are wifi-layer specific.
 */
enum {
    WIFI_REASON_UNSPECIFIED     = 1,
    WIFI_REASON_AUTH_LEAVING    = 3,    /* deauth: STA is leaving         */
    WIFI_REASON_MIC_FAILURE     = 14,   /* EAPOL-Key MIC verification failed */
    WIFI_REASON_DISASSOC_LEAVING= 8,    /* disassoc: STA is leaving       */
    WIFI_REASON_LOCAL_TIMEOUT   = 200,  /* handshake retries exhausted    */
    WIFI_REASON_AUTH_REJECTED   = 201,  /* auth-resp status != 0          */
    WIFI_REASON_ASSOC_REJECTED  = 202,  /* assoc-resp status != 0         */
    WIFI_REASON_LOCAL_DISCONNECT= 203,  /* user-requested disconnect      */
};

/*
 * Scratch op buffer size — caps how many ep0 register writes a parameterised
 * chip op (set_channel / connect / disconnect ...) can issue.  Static const
 * scripts (e.g. the chip init table) are not bounded by this.
 *
 * Worst case today is RT28xx connect: 2 BSSID + 4 TX cfg + 4 RF + 1 RX filter
 * = 11 ops.  Default 16 leaves headroom.
 */
#ifndef VSF_WIFI_CFG_SCRATCH_OPS
#   define VSF_WIFI_CFG_SCRATCH_OPS     16
#endif

/* Maximum number of channels in a single active scan (2.4 GHz + 5 GHz). */
#ifndef VSF_WIFI_CFG_SCAN_MAX_CHANNELS
#   define VSF_WIFI_CFG_SCAN_MAX_CHANNELS   40
#endif

/*============================ TYPES =========================================*/

typedef struct vsf_wifi_t           vsf_wifi_t;
typedef struct vsf_wifi_chip_drv_t  vsf_wifi_chip_drv_t;
typedef struct vsf_wifi_reg_bus_t   vsf_wifi_reg_bus_t;

/*
 * Asynchronous-completion callback.  Invoked exactly once per async request
 * (script / blob / single bus op).  err == VSF_ERR_NONE on success.
 */
typedef void (*vsf_wifi_done_t)(vsf_wifi_t *wifi, vsf_err_t err);

/*
 * Predicate used by vsf_wifi_reg_read_poll to decide whether the value just
 * read from a register satisfies the wait condition.  Called from the bus
 * driver's EDA on every poll iteration; must be cheap and side-effect free.
 */
typedef bool (*vsf_wifi_reg_match_fn_t)(uint32_t val);

typedef struct vsf_wifi_scan_result_t {
    uint8_t  bssid[6];
    uint8_t  ssid[33];
    uint8_t  ssid_len;
    uint8_t  channel;
    int8_t   rssi;
    uint16_t capability;
    uint8_t  flags;
    /* Security parsed from the RSN IE (tag 48).  auth_mode == WIFI_AUTH_OPEN
     * means no RSN IE was present (open network / WEP).  Only WPA2-PSK is
     * recognised; pairwise/group are WIFI_CIPHER_xxx. */
    uint8_t  auth_mode;
    uint8_t  pairwise_cipher;
    uint8_t  group_cipher;
} vsf_wifi_scan_result_t;

typedef struct vsf_wifi_link_info_t {
    uint8_t  bssid[6];
    uint8_t  channel;
    int8_t   rssi;
    uint16_t tx_rate;
    uint16_t rx_rate;
    uint32_t flags;
} vsf_wifi_link_info_t;

typedef struct vsf_wifi_auth_cfg_t {
    uint8_t  auth_mode;
    uint8_t  pairwise_cipher;
    uint8_t  group_cipher;
    uint8_t  psk[32];
    uint8_t  psk_len;
} vsf_wifi_auth_cfg_t;

/*
 * One unit of register-access work — write `val` (LE32) to register `reg`.
 *
 * BBP / RF indirect writes are NOT a separate op type: chip backends pre-
 * encode them (see RT_OP_BBP / RT_OP_RF in chip/rt28xx/vsf_wifi_rt28xx.c).
 * This keeps the wifi state machine free of chip-specific knowledge.
 */
typedef struct vsf_wifi_reg_op_t {
    uint16_t reg;
    uint32_t val;
} vsf_wifi_reg_op_t;

/*
 * Bulk register-block payload (firmware blob, calibration table, etc.).
 * Each chunk is shipped via reg_bus->reg_block_write (or, when the bus
 * lacks that primitive, decomposed into multiple reg_write calls by the
 * generic layer).  `data` MUST stay valid until the completion callback
 * fires — pointing at .rodata is fine on every supported bus.
 */
typedef struct vsf_wifi_reg_blob_t {
    const uint8_t *data;
    uint32_t       len;
    uint16_t       base_reg;
    uint16_t       chunk_size;     /* hint; bus impl may override */
} vsf_wifi_reg_blob_t;

/*============================ REGISTER BUS OPS ==============================*
 *
 * Optional register-access helper for register-based WiFi chips (e.g.
 * Ralink RT2X00).  Chips that do NOT speak a register protocol — such as
 * MediaTek mt76, which uses firmware commands over bulk/interrupt endpoints
 * — can leave wifi->reg_bus NULL and implement their own transport inside
 * drv->tx / drv->init / etc.
 *
 * Concurrency contract: at most one reg_bus call may be in flight at any
 * time.  Issuing a second call before the previous `done` callback has
 * fired is a programming error; the bus driver MAY return VSF_ERR_NOT_
 * AVAILABLE in that case.
 *==========================================================================*/

struct vsf_wifi_reg_bus_t {
    /* Single-register 32-bit write.  REQUIRED.
     *
     * Returns VSF_ERR_NONE if the write was queued; completion is delivered
     * later via `done`.  `done` may be NULL for fire-and-forget.  Synchronous
     * errors (resource exhaustion, bus busy) are returned directly without
     * invoking `done`.
     */
    vsf_err_t (*reg_write)      (vsf_wifi_t *wifi, uint16_t reg, uint32_t val,
                                 vsf_wifi_done_t done);

    /* Single-register 32-bit read.  REQUIRED.
     *
     * `out` is written before `done` fires; caller must keep it valid until
     * then.  Same async / error semantics as reg_write.
     */
    vsf_err_t (*reg_read)       (vsf_wifi_t *wifi, uint16_t reg, uint32_t *out,
                                 vsf_wifi_done_t done);

    /* Contiguous register-block write (firmware upload / calibration table
     * / TX descriptor RAM).  OPTIONAL — when NULL the generic layer falls
     * back to a sequence of reg_write calls.
     *
     * `base` is the starting register/memory address; the bus implementation
     * is free to fragment the payload internally (USB ep0 picks 64-byte
     * vendor-request chunks, SDIO CMD53 picks block size, etc.).  `data`
     * must stay valid until `done` fires.
     */
    vsf_err_t (*reg_block_write)(vsf_wifi_t *wifi, uint16_t base,
                                 const uint8_t *data, uint32_t len,
                                 vsf_wifi_done_t done);

    /* Raw ep0 vendor request with no data stage (wLength=0).  OPTIONAL —
     * NULL when the bus has no such concept.  Used by USB chips to issue
     * USB_DEVICE_MODE (start MCU firmware / reset the digital core), which
     * is NOT a register write.  `request`=bRequest, `value`=wValue,
     * `index`=wIndex.  Same async / error semantics as reg_write. */
    vsf_err_t (*vendor_request) (vsf_wifi_t *wifi, uint8_t request,
                                 uint16_t value, uint16_t index,
                                 vsf_wifi_done_t done);

    /* Bus-layer ready notification.  OPTIONAL — called when the wifi init
     * chain completes successfully (before vsf_wifi_on_ready).  The bus
     * driver uses this to enable the data path (e.g. submit bulk RX URBs).
     */
    void (*on_ready)(vsf_wifi_t *wifi);

    /* Bulk data-frame transmit for register-based buses that also have a
     * bulk data endpoint.  OPTIONAL — NULL when the register bus has no data
     * endpoint (pure ep0) or when the chip drv->tx handles data transmission
     * itself.  `data` points at a fully chip-encoded TX payload; `len` is its
     * total length.  Fire-and-forget: the bus driver owns completion internally
     * (TX URB pool).  Returns VSF_ERR_NONE when the frame was queued. */
    vsf_err_t (*data_tx)(vsf_wifi_t *wifi, uint8_t *data, uint16_t len);
};

/*============================ CHIP DRIVER VTABLE ============================*
 *
 * All chip operations are async: the chip driver builds an op-script (static
 * .rodata or wifi->scratch_ops) and submits it via vsf_wifi_reg_run_script.  The
 * outer `done` callback fires when the entire register script has been ACKed by the
 * device.  Returning VSF_ERR_NONE from these hooks means the script was
 * successfully queued; actual completion arrives later via `done`.
 *
 * The wifi layer never calls reg_bus directly — it always goes through the
 * chip vtable.  Register-based chips use the helpers vsf_wifi_reg_run_script /
 * run_blob / reg_read / reg_read_poll; command-based chips ignore reg_bus.
 *==========================================================================*/

#if VSF_WIFI_USE_WPA == ENABLED
/*
 * Optional hardware crypto backend.  When a chip driver supplies crypto_ops
 * with a non-NULL install_key, the wifi layer hands the negotiated PTK.TK /
 * GTK to the hardware engine and assumes the chip performs CCMP in-line; the
 * software CCMP path (wpa_hw_crypto == false) is then skipped on both TX and
 * RX.  Leaving crypto_ops (or install_key) NULL selects the built-in software
 * CCMP fallback.
 *
 *   install_key : program a key.  key_idx 0 + pairwise == the unicast TK;
 *                 key_idx 1..3 + !pairwise == a GTK.  `mac` is the peer for
 *                 pairwise keys, NULL for group keys.  Returns VSF_ERR_NONE.
 *   encrypt/decrypt : reserved per-frame overrides for chips that still need
 *                 wifi-layer framing with a chip-specific tweak; NULL means
 *                 "use the built-in software CCMP".
 */
typedef struct vsf_wifi_crypto_ops_t {
    vsf_err_t (*install_key)(vsf_wifi_t *wifi, uint8_t key_idx, bool pairwise,
                             const uint8_t *key, uint8_t key_len,
                             const uint8_t *mac);
    vsf_err_t (*encrypt)    (vsf_wifi_t *wifi, uint8_t *dot11,
                             uint16_t *len, uint16_t cap);
    vsf_err_t (*decrypt)    (vsf_wifi_t *wifi, uint8_t *dot11, uint16_t *len);
} vsf_wifi_crypto_ops_t;
#endif

struct vsf_wifi_chip_drv_t {
    const char *name;

    /* Optional firmware uploader (rt2870.bin etc.).  Invoked once before
     * init() during attach.  Pure ops can use vsf_wifi_reg_run_script; bulk
     * blob uploads call vsf_wifi_reg_run_blob.  Leave NULL when the chip needs
     * no firmware. */
    vsf_err_t (*firmware_load)(vsf_wifi_t *wifi, vsf_wifi_done_t done);
    vsf_err_t (*init)         (vsf_wifi_t *wifi, vsf_wifi_done_t done);
    void      (*fini)         (vsf_wifi_t *wifi);
    vsf_err_t (*set_channel)  (vsf_wifi_t *wifi, uint8_t channel,
                               vsf_wifi_done_t done);
    vsf_err_t (*set_rx_filter)(vsf_wifi_t *wifi, uint32_t mask,
                               vsf_wifi_done_t done);
    vsf_err_t (*set_mac_addr) (vsf_wifi_t *wifi, const uint8_t mac[6],
                               vsf_wifi_done_t done);
    vsf_err_t (*set_bssid)    (vsf_wifi_t *wifi, const uint8_t bssid[6],
                               vsf_wifi_done_t done);
    vsf_err_t (*set_auth_mode)(vsf_wifi_t *wifi,
                               const vsf_wifi_auth_cfg_t *cfg,
                               vsf_wifi_done_t done);
    vsf_err_t (*connect)      (vsf_wifi_t *wifi,
                               const uint8_t bssid[6], const uint8_t *ssid,
                               uint8_t ssid_len, uint8_t channel,
                               vsf_wifi_done_t done);
    vsf_err_t (*disconnect)   (vsf_wifi_t *wifi, vsf_wifi_done_t done);
    vsf_err_t (*get_link_info)(vsf_wifi_t *wifi,
                               vsf_wifi_link_info_t *info);  /* sync */

    /* Optional RX-frame parser used during scan.  When non-NULL and the
     * driver is currently scanning, the wifi layer routes incoming frames
     * here instead of vsf_wifi_on_rx; the chip code extracts beacons /
     * probe responses and emits vsf_wifi_on_scan_result(). */
    void      (*parse_rx)     (vsf_wifi_t *wifi, uint8_t *frame, uint16_t len);

    /* Optional TX-descriptor builder.  Wraps a raw 802.11 frame into the
     * chip-specific on-wire TX layout (e.g. RT2800 TXINFO + TXWI + frame +
     * pad) by writing into `dst` (capacity `dst_cap`).  Returns the total
     * byte count to ship over the data bus, or 0 on failure (dst too small
     * / unsupported).  Used by drv->tx; may also be left NULL if tx builds
     * its descriptor internally. */
    uint16_t  (*build_tx)     (vsf_wifi_t *wifi, uint8_t *dst, uint16_t dst_cap,
                              const uint8_t *frame, uint16_t frame_len);

    /* Frame transmit hook.  The wifi layer routes all outbound 802.11 frames
     * (active-scan probe-request, auth, assoc, deauth and data frames from
     * the netdrv) through this hook.  The chip driver builds the on-wire
     * payload and ships it over the bus.  REQUIRED; NULL returns
     * VSF_ERR_NOT_SUPPORT from vsf_wifi_tx. */
    vsf_err_t (*tx)           (vsf_wifi_t *wifi, const uint8_t *frame, uint16_t len);

#if VSF_WIFI_USE_WPA == ENABLED
    /* Optional hardware crypto backend (see vsf_wifi_crypto_ops_t above).
     * NULL selects the software CCMP fallback. */
    const vsf_wifi_crypto_ops_t *crypto_ops;
#endif
};

/*============================ APPLICATION CALLBACKS =========================*
 *
 * Weak hooks supplied by the application.  All run in the bus driver's EDA
 * context (not from ISR).  Default no-op implementations live in vsf_wifi.c.
 *==========================================================================*/

extern void vsf_wifi_on_new        (vsf_wifi_t *wifi);
extern void vsf_wifi_on_del        (vsf_wifi_t *wifi);
extern void vsf_wifi_on_rx         (vsf_wifi_t *wifi,
                                    uint8_t *frame, uint16_t len);
extern void vsf_wifi_on_ready      (vsf_wifi_t *wifi);
extern void vsf_wifi_on_scan_result(vsf_wifi_t *wifi,
                                    const vsf_wifi_scan_result_t *result);
extern void vsf_wifi_on_scan_done  (vsf_wifi_t *wifi);
extern void vsf_wifi_on_link_up    (vsf_wifi_t *wifi,
                                    const vsf_wifi_link_info_t *info);
extern void vsf_wifi_on_link_down  (vsf_wifi_t *wifi, uint8_t reason);

/*============================ NETIF BINDING ================================*
 *
 * Runtime binding for a network-stack adapter (e.g. the lwIP netdrv driver
 * in vsf_wifi_netdrv.c).  Once a backend is attached via
 * vsf_wifi_netdrv_attach(), the core delivers decrypted RX data frames to
 * its on_rx (replacing the weak vsf_wifi_on_rx hook), and additionally
 * delivers link up/down events to its on_link_up / on_link_down.  Link
 * events still also fire the weak vsf_wifi_on_link_up / vsf_wifi_on_link_down
 * hooks: those are control-plane notifications (e.g. unblocking a connect
 * command) and run alongside the backend's data-plane handling.  With no
 * backend attached only the weak hooks are used (back-compat with the
 * diagnostic commands).  All callbacks run in the bus driver's EDA context,
 * exactly like the weak hooks.  `on_rx` receives a naked, decrypted 802.11
 * data frame (FC .. payload); the backend is responsible for any 802.11 <->
 * Ethernet framing. */
typedef struct vsf_wifi_netif_ops_t {
    void (*on_rx)       (void *param, vsf_wifi_t *wifi,
                         uint8_t *frame, uint16_t len);
    void (*on_link_up)  (void *param, vsf_wifi_t *wifi,
                         const vsf_wifi_link_info_t *info);
    void (*on_link_down)(void *param, vsf_wifi_t *wifi, uint8_t reason);
} vsf_wifi_netif_ops_t;

/* Register / unregister a netif backend.  `ops` (and the storage it points
 * at) must stay valid until detach; `param` is passed back verbatim to each
 * callback.  Passing ops == NULL is equivalent to detach. */
void vsf_wifi_netdrv_attach(vsf_wifi_t *wifi,
                            const vsf_wifi_netif_ops_t *ops, void *param);
void vsf_wifi_netdrv_detach(vsf_wifi_t *wifi);

/*============================ USER API ======================================*/

/* Inspection. */
uint8_t      vsf_wifi_get_channel  (vsf_wifi_t *wifi);
const char * vsf_wifi_get_chip_name(vsf_wifi_t *wifi);
bool         vsf_wifi_is_ready     (vsf_wifi_t *wifi);

/* Per-device MAC address discovered during chip bring-up (eFuse / EEPROM
 * read).  Returns a pointer to a 6-byte array inside the wifi struct;
 * all-zero until the chip driver finishes its EEPROM stage.  The pointer
 * stays valid for the lifetime of the wifi instance. */
const uint8_t * vsf_wifi_get_mac    (vsf_wifi_t *wifi);

/* Mutation.  Calls return VSF_ERR_NOT_READY before vsf_wifi_on_ready,
 * VSF_ERR_NOT_AVAILABLE while a scan is in progress, and the chip driver's
 * own error otherwise.  All async — completion is signalled by either the
 * relevant on_xxx callback or by the next observable side effect (RSSI,
 * link_info, etc.). */
void         vsf_wifi_set_channel  (vsf_wifi_t *wifi, uint8_t channel);
vsf_err_t    vsf_wifi_set_mac      (vsf_wifi_t *wifi, const uint8_t mac[6]);
vsf_err_t    vsf_wifi_set_bssid    (vsf_wifi_t *wifi, const uint8_t bssid[6]);
vsf_err_t    vsf_wifi_set_auth_mode(vsf_wifi_t *wifi,
                                    const vsf_wifi_auth_cfg_t *cfg);

/*
 * Active scan: hop through the supplied channel list, dwelling `dwell_ms`
 * milliseconds on each channel.  Beacon / probe-response frames are routed
 * to the chip parser (drv->parse_rx) which feeds vsf_wifi_on_scan_result.
 * vsf_wifi_on_scan_done() is fired exactly once when every channel has
 * been visited.  dwell_ms == 0 falls back to a sensible default (120 ms).
 */
vsf_err_t    vsf_wifi_scan         (vsf_wifi_t *wifi,
                                    const uint8_t *channels,
                                    uint8_t num_channels, uint16_t dwell_ms);
vsf_err_t    vsf_wifi_scan_stop    (vsf_wifi_t *wifi);

vsf_err_t    vsf_wifi_connect      (vsf_wifi_t *wifi,
                                    const uint8_t bssid[6],
                                    const uint8_t *ssid, uint8_t ssid_len,
                                    uint8_t channel);
vsf_err_t    vsf_wifi_disconnect   (vsf_wifi_t *wifi);
vsf_err_t    vsf_wifi_get_link_info(vsf_wifi_t *wifi,
                                    vsf_wifi_link_info_t *info);

/*
 * Transmit a raw 802.11 frame.  The chip driver (drv->tx) is responsible for
 * building the on-wire TX descriptor and shipping it over the bus.  Returns
 * VSF_ERR_NOT_READY before vsf_wifi_on_ready, VSF_ERR_NOT_SUPPORTED when the
 * chip driver has no tx hook, or a bus-level error.  Fire-and-forget: there
 * is no per-frame completion callback (chip drivers use TX_STA_FIFO /
 * TX_STA_CNT to inspect results). */
vsf_err_t    vsf_wifi_tx           (vsf_wifi_t *wifi,
                                    const uint8_t *frame, uint16_t len);

/*============================ REGISTER-BUS LAYER API =========================*
 *
 * These are exported so a bus driver can drive the wifi state machine.
 * Application code does not call them directly.
 *==========================================================================*/

/*
 * One-shot construction.  After this call:
 *   - wifi->drv is bound;
 *   - wifi->reg_bus is bound (may be NULL for non-register-based chips);
 *   - all state fields are zero / cleared;
 *   - scan timer is initialised but not armed;
 *   - the wifi is NOT started yet (call vsf_wifi_start when ready).
 *
 * `post_eda` is the EDA on which the wifi layer will post follow-up events
 * (scan-hop callbacks).  Typically the bus driver passes its own EDA so
 * that timer-driven hops are handled in the same context as bus completions.
 */
void vsf_wifi_init(vsf_wifi_t *wifi,
                   const vsf_wifi_chip_drv_t *drv,
                   const vsf_wifi_reg_bus_t  *reg_bus,
                   vsf_eda_t                 *post_eda);

/*
 * Kick off the chip bring-up chain (firmware_load -> init -> set_rx_filter).
 * On success vsf_wifi_on_ready fires when the chain finishes; on any error
 * the bus driver's `on_attach_fail` (passed via vsf_wifi_set_attach_fail)
 * is invoked so it can tear down the device.
 *
 * Must be called from the bus driver's EDA context, typically on
 * VSF_EVT_INIT.
 */
void vsf_wifi_start(vsf_wifi_t *wifi);

/*
 * Bus driver tear-down.  Cancels pending scans / scripts, marks the wifi
 * as disconnecting so no further callbacks fire, and invokes the chip
 * driver's fini hook.  After this call the wifi struct can be freed.
 */
void vsf_wifi_fini(vsf_wifi_t *wifi);

/*
 * Set a callback that the wifi layer invokes when the bring-up chain or a
 * scan reports an unrecoverable error.  The bus driver typically removes
 * the device interface from inside this hook.  Default: NULL (errors are
 * logged but otherwise ignored).
 */
typedef void (*vsf_wifi_attach_fail_t)(vsf_wifi_t *wifi, vsf_err_t err);
void vsf_wifi_set_attach_fail(vsf_wifi_t *wifi, vsf_wifi_attach_fail_t hook);

/* Bus driver hands an inbound RX frame to the wifi layer; the layer routes
 * it to drv->parse_rx (during scan) or vsf_wifi_on_rx (otherwise).  */
void vsf_wifi_on_rx_internal(vsf_wifi_t *wifi, uint8_t *frame, uint16_t len);

/* MLME management-frame entry point.  The chip parser (drv->parse_rx) calls
 * this with a de-descriptored, naked 802.11 management frame (starting at
 * the FC field) when its subtype is auth / assoc-resp / deauth / disassoc.
 * The wifi-layer MLME state machine advances the OPEN-system connection. */
void vsf_wifi_mlme_rx(vsf_wifi_t *wifi, const uint8_t *dot11, uint16_t len);

/* Data-frame entry point.  The chip parser (drv->parse_rx) calls this with a
 * de-descriptored, naked 802.11 data frame (starting at the FC field) once
 * the link is associated (mlme_state RUN / 4WAY).  The wifi layer parses the
 * data header, detects EAPOL (LLC/SNAP + ethertype 0x888E) for the 4-way
 * handshake, and forwards decrypted business payloads to vsf_wifi_on_rx. */
void vsf_wifi_data_rx(vsf_wifi_t *wifi, const uint8_t *dot11, uint16_t len);

#if VSF_WIFI_USE_WPA == ENABLED
/* Helpers exported to the WPA 4-way handshake (vsf_wifi_wpa.c).  The wifi
 * layer owns the MLME state, the retry timer and the chip TX path; the WPA
 * module owns EAPOL-Key parsing / building and key derivation, and drives
 * the connection through these calls:
 *   - mlme_tx          : transmit a fully formed 802.11 frame (EAPOL).
 *   - mlme_arm_timer   : (re)arm the handshake timeout.
 *   - handshake_done   : keys installed -> state RUN + vsf_wifi_on_link_up.
 *   - handshake_fail   : abort the handshake -> link down with `reason`. */
vsf_err_t vsf_wifi_mlme_tx(vsf_wifi_t *wifi, const uint8_t *frame, uint16_t len);
void vsf_wifi_mlme_arm_timer(vsf_wifi_t *wifi, uint16_t ms);
void vsf_wifi_mlme_handshake_done(vsf_wifi_t *wifi);
void vsf_wifi_mlme_handshake_fail(vsf_wifi_t *wifi, uint8_t reason);
#endif

/* Bus driver invokes this from its EDA when the wifi-posted scan-hop event
 * lands; the wifi layer advances to the next channel or finishes the scan. */
void vsf_wifi_on_scan_hop_evt(vsf_wifi_t *wifi);

/* Bus driver invokes this from its EDA when the wifi-posted MLME-retry event
 * lands; the wifi layer retransmits the current handshake frame (auth-req /
 * assoc-req) or declares a link-down timeout once retries are exhausted.
 * Bounced through the bus EDA (like scan-hop) so the TX runs in the same
 * context as bus completions. */
void vsf_wifi_on_mlme_retry_evt(vsf_wifi_t *wifi);

/* Custom EDA event used by vsf_wifi to trigger a scan hop on the bus EDA.
 * The bus driver's evt-handler must dispatch this value to
 * vsf_wifi_on_scan_hop_evt(). */
#define VSF_WIFI_EVT_SCAN_HOP           (VSF_EVT_USER + 0x40)

/* Custom EDA event used by vsf_wifi to trigger an MLME handshake retransmit
 * on the bus EDA.  The bus driver's evt-handler must dispatch this value to
 * vsf_wifi_on_mlme_retry_evt(). */
#define VSF_WIFI_EVT_MLME_RETRY         (VSF_EVT_USER + 0x41)

/*============================ CHIP <-> WIFI INTERNAL API ====================*/

/*
 * Run a sequence of register writes asynchronously.
 *
 *   ops    : array of (reg, val) pairs.  May point to .rodata or to
 *            wifi->scratch_ops.  Caller MUST keep `ops` valid until
 *            `done` fires.
 *   count  : number of ops to execute (must be > 0 for a real submission).
 *   done   : completion callback (NULL = fire-and-forget).
 *
 * Returns VSF_ERR_NONE on accepted submission (completion via `done`),
 * VSF_ERR_NOT_AVAILABLE when another script / blob is in flight, or a bus-
 * level synchronous error otherwise.
 */
vsf_err_t vsf_wifi_reg_run_script(vsf_wifi_t *wifi,
                              const vsf_wifi_reg_op_t *ops, uint16_t count,
                              vsf_wifi_done_t done);

/*
 * Stream a contiguous register-block payload.  Internally calls
 * reg_bus->reg_block_write when available; otherwise decomposes into
 * ceil(len / 4) reg_write calls.
 *
 * Same concurrency rules as vsf_wifi_reg_run_script: at most one script /
 * blob in flight per wifi.
 */
vsf_err_t vsf_wifi_reg_run_blob(vsf_wifi_t *wifi,
                            const vsf_wifi_reg_blob_t *blob,
                            vsf_wifi_done_t done);

/*
 * Issue a single ep0 vendor request with no data stage (wLength=0) via
 * reg_bus->vendor_request.  Used by USB chips for control commands that are
 * not register writes (e.g. RT2800 USB_DEVICE_MODE to start the MCU firmware
 * or reset the digital core).  Completes with VSF_ERR_NOT_SUPPORTED when the
 * bus has no vendor_request primitive.
 *
 * Same concurrency rules as vsf_wifi_reg_run_script: at most one script / blob /
 * vendor request in flight per wifi.
 */
vsf_err_t vsf_wifi_reg_run_vendor(vsf_wifi_t *wifi, uint8_t request,
                              uint16_t value, uint16_t index,
                              vsf_wifi_done_t done);

/*
 * Periodically read `reg` until `match(val)` returns true.
 *
 *   reg          : MAC register to poll.
 *   match        : predicate; the read value is fed in on every iteration.
 *   max_retry    : upper bound on read attempts (>= 1).  The first read
 *                  counts as attempt #1.
 *   interval_ms  : delay between consecutive reads.  0 means "re-issue
 *                  immediately on completion" (use only if the bus already
 *                  paces reads, e.g. USB ep0 vendor request).
 *   done         : completion callback.
 *                    VSF_ERR_NONE      : predicate matched.
 *                    VSF_ERR_TIMEOUT   : retries exhausted without match.
 *                    other             : bus-level error.
 *
 * Same single-flight constraint as run_script / run_blob.
 */
vsf_err_t vsf_wifi_reg_read_poll(vsf_wifi_t *wifi, uint16_t reg,
                                 vsf_wifi_reg_match_fn_t match,
                                 uint16_t max_retry, uint16_t interval_ms,
                                 vsf_wifi_done_t done);

/*
 * Single 32-bit register read.  The result is stored at *out before
 * `done` fires; caller must keep `out` valid until then.  Same single-
 * flight constraint as run_script / run_blob / run_read_poll.
 */
vsf_err_t vsf_wifi_reg_read(vsf_wifi_t *wifi, uint16_t reg, uint32_t *out,
                            vsf_wifi_done_t done);

/* Per-wifi scratch op buffer (shared by parameterised chip ops). */
vsf_wifi_reg_op_t * vsf_wifi_reg_get_scratch_ops(vsf_wifi_t *wifi);

/*============================ CHIP-SPECIFIC HEADERS =========================*
 *
 * Concrete chip families live in chip/xxx/ and expose their own firmware
 * symbols, logging helpers and (if needed) public constants.  The generic
 * wifi header only pulls in the chips that are enabled at compile time.
 *===========================================================================*/

#if VSF_WIFI_USE_RT28XX == ENABLED
#   include "./chip/rt28xx/vsf_wifi_rt28xx.h"
#endif

/*============================ LOGGING HELPERS ===============================*
 *
 * Compile-time log level gates for the WiFi subsystem and its sub-modules.
 * Levels are defined in vsf_wifi_cfg.h; sub-module macros fall back to the
 * global VSF_WIFI_CFG_LOG_LEVEL when not explicitly set.
 *==========================================================================*/

#if VSF_WIFI_CFG_LOG_LEVEL >= 1
#   define vsf_wifi_trace_error(...)    vsf_trace_error(__VA_ARGS__)
#else
#   define vsf_wifi_trace_error(...)    ((void)0)
#endif

#if VSF_WIFI_CFG_LOG_LEVEL >= 2
#   define vsf_wifi_trace_info(...)     vsf_trace_info(__VA_ARGS__)
#else
#   define vsf_wifi_trace_info(...)     ((void)0)
#endif

#if VSF_WIFI_CFG_LOG_LEVEL >= 4
#   define vsf_wifi_trace_debug(...)    vsf_trace_info(__VA_ARGS__)
#else
#   define vsf_wifi_trace_debug(...)    ((void)0)
#endif


#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_WIFI
#endif      // __VSF_WIFI_H__
