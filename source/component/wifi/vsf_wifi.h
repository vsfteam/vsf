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

/* Maximum number of channels in a single active scan (2.4 GHz band). */
#ifndef VSF_WIFI_CFG_SCAN_MAX_CHANNELS
#   define VSF_WIFI_CFG_SCAN_MAX_CHANNELS   14
#endif

/*============================ TYPES =========================================*/

typedef struct vsf_wifi_t           vsf_wifi_t;
typedef struct vsf_wifi_chip_drv_t  vsf_wifi_chip_drv_t;
typedef struct vsf_wifi_bus_ops_t   vsf_wifi_bus_ops_t;

/*
 * Asynchronous-completion callback.  Invoked exactly once per async request
 * (script / blob / single bus op).  err == VSF_ERR_NONE on success.
 */
typedef void (*vsf_wifi_done_t)(vsf_wifi_t *wifi, vsf_err_t err);

typedef struct vsf_wifi_scan_result_t {
    uint8_t  bssid[6];
    uint8_t  ssid[33];
    uint8_t  ssid_len;
    uint8_t  channel;
    int8_t   rssi;
    uint16_t capability;
    uint8_t  flags;
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
typedef struct vsf_wifi_op_t {
    uint16_t reg;
    uint32_t val;
} vsf_wifi_op_t;

/*
 * Bulk register-block payload (firmware blob, calibration table, etc.).
 * Each chunk is shipped via bus_ops->reg_block_write (or, when the bus
 * lacks that primitive, decomposed into multiple reg_write calls by the
 * generic layer).  `data` MUST stay valid until the completion callback
 * fires — pointing at .rodata is fine on every supported bus.
 */
typedef struct vsf_wifi_blob_t {
    const uint8_t *data;
    uint32_t       len;
    uint16_t       base_reg;
    uint16_t       chunk_size;     /* hint; bus impl may override */
} vsf_wifi_blob_t;

/*============================ BUS OPS =======================================*
 *
 * Only the *most stable* bus primitives live in this vtable: single-register
 * 32-bit write / read, plus an optional contiguous register-block write.
 * These are guaranteed to be expressible on any addressable wifi NIC bus
 * (USB ep0 vendor request, SDIO CMD52/CMD53, SPI register frame, MMIO).
 *
 * Higher-level concepts that vary across buses (bulk endpoint TX/RX, port
 * reset, PHY interrupts, doorbells) are intentionally NOT abstracted here
 * — those continue to live inside the bus driver until enough chip / bus
 * combinations exist to draw a clean line.
 *
 * Concurrency contract: at most one bus_ops call may be in flight at any
 * time.  Issuing a second call before the previous `done` callback has
 * fired is a programming error; the bus driver MAY return VSF_ERR_NOT_
 * AVAILABLE in that case.
 *==========================================================================*/

struct vsf_wifi_bus_ops_t {
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

    /* Bus-layer ready notification.  OPTIONAL — called when the wifi init
     * chain completes successfully (before vsf_wifi_on_ready).  The bus
     * driver uses this to enable the data path (e.g. submit bulk RX URBs).
     */
    void (*on_ready)(vsf_wifi_t *wifi);
};

/*============================ CHIP DRIVER VTABLE ============================*
 *
 * All chip operations are async: the chip driver builds an op-script (static
 * .rodata or wifi->scratch_ops) and submits it via vsf_wifi_run_script.  The
 * outer `done` callback fires when the entire script has been ACKed by the
 * device.  Returning VSF_ERR_NONE from these hooks means the script was
 * successfully queued; actual completion arrives later via `done`.
 *
 * The wifi layer never calls bus_ops directly — it always goes through this
 * chip vtable.  Conversely the chip code only sees bus_ops, never the bus
 * driver's private types.
 *==========================================================================*/

struct vsf_wifi_chip_drv_t {
    const char *name;

    /* Optional firmware uploader (rt2870.bin etc.).  Invoked once before
     * init() during attach.  Pure ops can use vsf_wifi_run_script; bulk
     * blob uploads call vsf_wifi_run_blob.  Leave NULL when the chip needs
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

/*============================ USER API ======================================*/

/* Inspection. */
uint8_t      vsf_wifi_get_channel  (vsf_wifi_t *wifi);
const char * vsf_wifi_get_chip_name(vsf_wifi_t *wifi);
bool         vsf_wifi_is_ready     (vsf_wifi_t *wifi);

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

/*============================ BUS-LAYER API (used by USB shim, SDIO, ...) ==*
 *
 * These are exported so a bus driver can drive the wifi state machine.
 * Application code does not call them directly.
 *==========================================================================*/

/*
 * One-shot construction.  After this call:
 *   - wifi->drv / wifi->bus_ops are bound;
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
                   const vsf_wifi_bus_ops_t  *bus_ops,
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

/* Bus driver invokes this from its EDA when the wifi-posted scan-hop event
 * lands; the wifi layer advances to the next channel or finishes the scan. */
void vsf_wifi_on_scan_hop_evt(vsf_wifi_t *wifi);

/* Custom EDA event used by vsf_wifi to trigger a scan hop on the bus EDA.
 * The bus driver's evt-handler must dispatch this value to
 * vsf_wifi_on_scan_hop_evt(). */
#define VSF_WIFI_EVT_SCAN_HOP           (VSF_EVT_USER + 0x40)

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
vsf_err_t vsf_wifi_run_script(vsf_wifi_t *wifi,
                              const vsf_wifi_op_t *ops, uint16_t count,
                              vsf_wifi_done_t done);

/*
 * Stream a contiguous register-block payload.  Internally calls
 * bus_ops->reg_block_write when available; otherwise decomposes into
 * ceil(len / 4) reg_write calls.
 *
 * Same concurrency rules as vsf_wifi_run_script: at most one script /
 * blob in flight per wifi.
 */
vsf_err_t vsf_wifi_run_blob(vsf_wifi_t *wifi,
                            const vsf_wifi_blob_t *blob,
                            vsf_wifi_done_t done);

/* Per-wifi scratch op buffer (shared by parameterised chip ops). */
vsf_wifi_op_t * vsf_wifi_get_scratch_ops(vsf_wifi_t *wifi);

/*
 * Firmware blob declared by chip/rt28xx/vsf_wifi_rt2870_firmware.c (weak
 * stub).  Link a strong override to ship the real rt2870.bin; the rt28xx
 * chip driver uses size > 0 as the trigger to invoke run_blob during
 * firmware_load.
 */
extern const uint8_t  __rt2870_firmware_data[];
extern const uint32_t __rt2870_firmware_size;

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_WIFI
#endif      // __VSF_WIFI_H__
