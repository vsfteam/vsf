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

#include "./vsf_wifi.h"

#if VSF_USE_WIFI == ENABLED

#include "./vsf_wifi_priv.h"

#if VSF_WIFI_USE_WPA == ENABLED
#   include "./vsf_wifi_wpa.h"
#   include "./vsf_wifi_crypto.h"
#endif

/*============================ PROTOTYPES ====================================*/

static void __vsf_wifi_script_finish (vsf_wifi_t *wifi, vsf_err_t err);
static void __vsf_wifi_script_step   (vsf_wifi_t *wifi, vsf_err_t err);
static void __vsf_wifi_blob_step     (vsf_wifi_t *wifi, vsf_err_t err);
static void __vsf_wifi_read_poll_issue(vsf_wifi_t *wifi);
static void __vsf_wifi_read_poll_done (vsf_wifi_t *wifi, vsf_err_t err);
#if VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER == ENABLED
static void __vsf_wifi_read_poll_timer_cb(vsf_callback_timer_t *timer);
#endif

static void __vsf_wifi_on_fw_done       (vsf_wifi_t *wifi, vsf_err_t err);
static void __vsf_wifi_on_init_done     (vsf_wifi_t *wifi, vsf_err_t err);
static void __vsf_wifi_on_mac_set_done  (vsf_wifi_t *wifi, vsf_err_t err);
static void __vsf_wifi_on_rxfilter_done (vsf_wifi_t *wifi, vsf_err_t err);

/* ---- MLME (OPEN-system auth + association) state machine ---- */
#if VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER == ENABLED
static void __vsf_wifi_mlme_timer_cb (vsf_callback_timer_t *timer);
#endif
static void __vsf_wifi_mlme_connect_done(vsf_wifi_t *wifi, vsf_err_t err);
static void __vsf_wifi_mlme_send_auth (vsf_wifi_t *wifi);
static void __vsf_wifi_mlme_send_assoc(vsf_wifi_t *wifi);
static void __vsf_wifi_mlme_send_deauth(vsf_wifi_t *wifi, uint16_t reason);
static void __vsf_wifi_mlme_finish    (vsf_wifi_t *wifi, uint8_t reason);

/* Active scan: probe each channel during its dwell window so that silent /
 * hidden APs answer with a probe-response.  Set to DISABLED for pure passive
 * (beacon-only) scanning. */
#ifndef VSF_WIFI_CFG_SCAN_ACTIVE
#   define VSF_WIFI_CFG_SCAN_ACTIVE     ENABLED
#endif

static vsf_err_t __vsf_wifi_tx_frame  (vsf_wifi_t *wifi,
                                       const uint8_t *frame, uint16_t len);

#if VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER == ENABLED
static void __vsf_wifi_scan_timer_cb (vsf_callback_timer_t *timer);
static void __vsf_wifi_scan_hop_done (vsf_wifi_t *wifi, vsf_err_t err);
static void __vsf_wifi_scan_advance  (vsf_wifi_t *wifi);
static void __vsf_wifi_scan_finish   (vsf_wifi_t *wifi);
#if VSF_WIFI_CFG_SCAN_ACTIVE == ENABLED
static void __vsf_wifi_send_probe_req (vsf_wifi_t *wifi);
#endif

#define __VSF_WIFI_SCAN_DEFAULT_DWELL_MS    120
#endif

/*============================ WEAK CALLBACKS ================================*/

VSF_CAL_WEAK(vsf_wifi_on_new)
void vsf_wifi_on_new(vsf_wifi_t *wifi) { (void)wifi; }

VSF_CAL_WEAK(vsf_wifi_on_del)
void vsf_wifi_on_del(vsf_wifi_t *wifi) { (void)wifi; }

VSF_CAL_WEAK(vsf_wifi_on_rx)
void vsf_wifi_on_rx(vsf_wifi_t *wifi, uint8_t *frame, uint16_t len)
{ (void)wifi; (void)frame; (void)len; }

VSF_CAL_WEAK(vsf_wifi_on_ready)
void vsf_wifi_on_ready(vsf_wifi_t *wifi) { (void)wifi; }

VSF_CAL_WEAK(vsf_wifi_on_scan_result)
void vsf_wifi_on_scan_result(vsf_wifi_t *wifi,
        const vsf_wifi_scan_result_t *result)
{ (void)wifi; (void)result; }

VSF_CAL_WEAK(vsf_wifi_on_scan_done)
void vsf_wifi_on_scan_done(vsf_wifi_t *wifi) { (void)wifi; }

VSF_CAL_WEAK(vsf_wifi_on_link_up)
void vsf_wifi_on_link_up(vsf_wifi_t *wifi, const vsf_wifi_link_info_t *info)
{ (void)wifi; (void)info; }

VSF_CAL_WEAK(vsf_wifi_on_link_down)
void vsf_wifi_on_link_down(vsf_wifi_t *wifi, uint8_t reason)
{ (void)wifi; (void)reason; }

/*============================ HELPERS =======================================*/

static void __vsf_wifi_attach_fail(vsf_wifi_t *wifi, vsf_err_t err)
{
    vsf_trace_error("wifi: attach failed (err=%d)" VSF_TRACE_CFG_LINEEND, (int)err);
    if (wifi->attach_fail != NULL) {
        wifi->attach_fail(wifi, err);
    }
}

/*============================ SCRIPT DISPATCHER =============================*
 *
 * The whole driver runs every chip operation through one tiny state machine.
 * No matter what the user-facing call is (init / set_channel / connect ...),
 * the chip backend builds a sequence of (reg, val) ops and submits it via
 * vsf_wifi_run_script().  Each op is shipped through bus_ops->reg_write; on
 * completion the bus driver invokes our `__vsf_wifi_script_step` which
 * advances `script.idx` and fires the next op until the array is exhausted.
 *
 * `script_busy` enforces "one script (or blob) at a time per wifi".
 *==========================================================================*/

static void __vsf_wifi_script_finish(vsf_wifi_t *wifi, vsf_err_t err)
{
    vsf_wifi_done_t done = wifi->script_done;
    wifi->script_busy    = false;
    wifi->script_is_blob = false;
    wifi->script_done    = NULL;
#if VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER == ENABLED
    /* Cancel any read-poll spacing timer that may still be armed (no-op if
     * the dispatcher path that finished was script / blob).  Idempotent. */
    vsf_callback_timer_remove(&wifi->read_poll_timer);
#endif
    memset(&wifi->s, 0, sizeof(wifi->s));
    if (done != NULL) {
        done(wifi, err);
    }
}

static void __vsf_wifi_script_step(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (wifi->disconnecting) {
        wifi->script_busy = false;
        return;
    }
    if (VSF_ERR_NONE != err) {
        __vsf_wifi_script_finish(wifi, err);
        return;
    }

    wifi->s.script.idx++;
    if (wifi->s.script.idx >= wifi->s.script.count) {
        __vsf_wifi_script_finish(wifi, VSF_ERR_NONE);
        return;
    }
    const vsf_wifi_op_t *op = &wifi->s.script.ops[wifi->s.script.idx];
    err = wifi->bus_ops->reg_write(wifi, op->reg, op->val,
            __vsf_wifi_script_step);
    if (VSF_ERR_NONE != err) {
        __vsf_wifi_script_finish(wifi, err);
    }
}

vsf_err_t vsf_wifi_run_script(vsf_wifi_t *wifi,
        const vsf_wifi_op_t *ops, uint16_t count, vsf_wifi_done_t done)
{
    if (wifi->script_busy) return VSF_ERR_NOT_AVAILABLE;
    if (count == 0 || ops == NULL) {
        if (done != NULL) done(wifi, VSF_ERR_NONE);
        return VSF_ERR_NONE;
    }

    wifi->script_is_blob = false;
    wifi->s.script.ops   = ops;
    wifi->s.script.count = count;
    wifi->s.script.idx   = 0;
    wifi->script_done    = done;
    wifi->script_busy    = true;

    vsf_err_t err = wifi->bus_ops->reg_write(wifi, ops[0].reg, ops[0].val,
            __vsf_wifi_script_step);
    if (VSF_ERR_NONE != err) {
        wifi->script_busy = false;
        wifi->script_done = NULL;
        memset(&wifi->s, 0, sizeof(wifi->s));
    }
    return err;
}

/*============================ BLOB DISPATCHER ===============================*
 *
 * Two paths:
 *
 *   1. Fast path: bus_ops->reg_block_write available — hand the whole blob
 *      to the bus in one call (USB ep0 picks 64-byte chunks internally;
 *      SDIO CMD53 picks block size; ...).  Single completion callback.
 *
 *   2. Fallback: walk the blob 4 bytes at a time using reg_write.  This is
 *      portable but slow — 8 KiB firmware = 2048 single-register writes.
 *      Acceptable as a last-resort path; chip drivers that care about
 *      bring-up time will provide reg_block_write.
 *==========================================================================*/

static void __vsf_wifi_blob_fast_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    __vsf_wifi_script_finish(wifi, err);
}

static void __vsf_wifi_blob_step(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (wifi->disconnecting) {
        wifi->script_busy = false;
        return;
    }
    if (VSF_ERR_NONE != err) {
        __vsf_wifi_script_finish(wifi, err);
        return;
    }

    wifi->s.blob.offset += 4;
    if (wifi->s.blob.offset >= wifi->s.blob.len) {
        __vsf_wifi_script_finish(wifi, VSF_ERR_NONE);
        return;
    }

    /* Pack next 4 bytes (LE32) — last group may be padded with zero. */
    uint32_t off = wifi->s.blob.offset;
    uint32_t val = 0;
    uint32_t left = wifi->s.blob.len - off;
    uint32_t take = left < 4 ? left : 4;
    for (uint32_t i = 0; i < take; i++) {
        val |= ((uint32_t)wifi->s.blob.data[off + i]) << (i * 8);
    }
    err = wifi->bus_ops->reg_write(wifi,
            (uint16_t)(wifi->s.blob.base_reg + off), val,
            __vsf_wifi_blob_step);
    if (VSF_ERR_NONE != err) {
        __vsf_wifi_script_finish(wifi, err);
    }
}

vsf_err_t vsf_wifi_run_blob(vsf_wifi_t *wifi,
        const vsf_wifi_blob_t *blob, vsf_wifi_done_t done)
{
    if (wifi->script_busy) return VSF_ERR_NOT_AVAILABLE;
    if (blob == NULL || blob->data == NULL || blob->len == 0) {
        if (done != NULL) done(wifi, VSF_ERR_NONE);
        return VSF_ERR_NONE;
    }

    wifi->script_is_blob   = true;
    wifi->s.blob.data       = blob->data;
    wifi->s.blob.len        = blob->len;
    wifi->s.blob.offset     = 0;
    wifi->s.blob.base_reg   = blob->base_reg;
    wifi->s.blob.chunk_size = blob->chunk_size;
    wifi->script_done       = done;
    wifi->script_busy       = true;

    vsf_err_t err;
    if (wifi->bus_ops->reg_block_write != NULL) {
        err = wifi->bus_ops->reg_block_write(wifi, blob->base_reg,
                blob->data, blob->len, __vsf_wifi_blob_fast_done);
    } else {
        /* Fallback: kick first 4-byte write; subsequent writes chained
         * by __vsf_wifi_blob_step. */
        uint32_t val = 0;
        uint32_t take = blob->len < 4 ? blob->len : 4;
        for (uint32_t i = 0; i < take; i++) {
            val |= ((uint32_t)blob->data[i]) << (i * 8);
        }
        err = wifi->bus_ops->reg_write(wifi, blob->base_reg, val,
                __vsf_wifi_blob_step);
    }
    if (VSF_ERR_NONE != err) {
        wifi->script_busy    = false;
        wifi->script_is_blob = false;
        wifi->script_done    = NULL;
        memset(&wifi->s, 0, sizeof(wifi->s));
    }
    return err;
}

/*============================ VENDOR-REQUEST DISPATCHER =====================*/

static void __vsf_wifi_vendor_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    __vsf_wifi_script_finish(wifi, err);
}

vsf_err_t vsf_wifi_run_vendor(vsf_wifi_t *wifi, uint8_t request,
        uint16_t value, uint16_t index, vsf_wifi_done_t done)
{
    if (wifi->script_busy) return VSF_ERR_NOT_AVAILABLE;
    if (wifi->bus_ops->vendor_request == NULL) {
        if (done != NULL) done(wifi, VSF_ERR_NOT_SUPPORT);
        return VSF_ERR_NOT_SUPPORT;
    }

    wifi->script_is_blob = false;
    wifi->script_done    = done;
    wifi->script_busy    = true;

    vsf_err_t err = wifi->bus_ops->vendor_request(wifi, request, value, index,
            __vsf_wifi_vendor_done);
    if (VSF_ERR_NONE != err) {
        wifi->script_busy = false;
        wifi->script_done = NULL;
        memset(&wifi->s, 0, sizeof(wifi->s));
    }
    return err;
}

vsf_wifi_op_t * vsf_wifi_get_scratch_ops(vsf_wifi_t *wifi)
{
    return wifi->scratch_ops;
}

/*============================ READ-POLL DISPATCHER ==========================*
 *
 * Repeatedly issues bus_ops->reg_read against a single register, feeding
 * each result into a chip-supplied predicate.  Used by chip drivers to
 * wait for hardware-side state transitions whose duration is unknown at
 * compile time (firmware boot, EEPROM bus completion, BBP wakeup, ...).
 *
 * Concurrency: shares the script_busy single-flight slot with run_script
 * and run_blob; only one of the three may be in flight at any time.
 * Cleanup goes through __vsf_wifi_script_finish (which also stops the
 * read-poll spacing timer).
 *==========================================================================*/

static void __vsf_wifi_read_poll_issue(vsf_wifi_t *wifi)
{
    if (wifi->disconnecting) {
        wifi->script_busy = false;
        return;
    }
    vsf_err_t err = wifi->bus_ops->reg_read(wifi,
            wifi->s.read_poll.reg, &wifi->s.read_poll.last_val,
            __vsf_wifi_read_poll_done);
    if (VSF_ERR_NONE != err) {
        __vsf_wifi_script_finish(wifi, err);
    }
}

static void __vsf_wifi_read_poll_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (wifi->disconnecting) {
        wifi->script_busy = false;
        return;
    }
    if (VSF_ERR_NONE != err) {
        vsf_trace_warning("wifi: read_poll reg=0x%04X bus err=%d" VSF_TRACE_CFG_LINEEND,
                (unsigned)wifi->s.read_poll.reg, (int)err);
        __vsf_wifi_script_finish(wifi, err);
        return;
    }
    if (wifi->s.read_poll.match != NULL
            && wifi->s.read_poll.match(wifi->s.read_poll.last_val)) {
        vsf_trace_info("wifi: read_poll reg=0x%04X matched val=0x%08X" VSF_TRACE_CFG_LINEEND,
                (unsigned)wifi->s.read_poll.reg,
                (unsigned)wifi->s.read_poll.last_val);
        __vsf_wifi_script_finish(wifi, VSF_ERR_NONE);
        return;
    }
    if (wifi->s.read_poll.retry_left == 0) {
        vsf_trace_warning("wifi: read_poll reg=0x%04X timeout, last=0x%08X" VSF_TRACE_CFG_LINEEND,
                (unsigned)wifi->s.read_poll.reg,
                (unsigned)wifi->s.read_poll.last_val);
        __vsf_wifi_script_finish(wifi, VSF_ERR_TIMEOUT);
        return;
    }
    wifi->s.read_poll.retry_left--;
#if VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER == ENABLED
    if (wifi->s.read_poll.interval_ms > 0) {
        vsf_callback_timer_add_ms(&wifi->read_poll_timer,
                wifi->s.read_poll.interval_ms);
        return;
    }
#endif
    /* No timer support or zero spacing: re-issue immediately. */
    __vsf_wifi_read_poll_issue(wifi);
}

#if VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER == ENABLED
static void __vsf_wifi_read_poll_timer_cb(vsf_callback_timer_t *timer)
{
    vsf_wifi_t *wifi = vsf_container_of(timer, vsf_wifi_t, read_poll_timer);
    if (wifi->disconnecting || !wifi->script_busy) return;
    __vsf_wifi_read_poll_issue(wifi);
}
#endif

vsf_err_t vsf_wifi_run_read_poll(vsf_wifi_t *wifi, uint16_t reg,
        vsf_wifi_match_fn_t match, uint16_t max_retry, uint16_t interval_ms,
        vsf_wifi_done_t done)
{
    if (wifi->script_busy)              return VSF_ERR_NOT_AVAILABLE;
    if (NULL == match)                  return VSF_ERR_INVALID_PARAMETER;
    if (NULL == wifi->bus_ops->reg_read) return VSF_ERR_NOT_SUPPORT;
    if (max_retry == 0) max_retry = 1;

    wifi->script_is_blob          = false;
    wifi->script_done             = done;
    memset(&wifi->s, 0, sizeof(wifi->s));
    wifi->s.read_poll.reg          = reg;
    wifi->s.read_poll.match        = match;
    wifi->s.read_poll.retry_left   = (uint16_t)(max_retry - 1);
    wifi->s.read_poll.interval_ms  = interval_ms;
    wifi->s.read_poll.last_val     = 0;
    wifi->script_busy              = true;

    vsf_trace_info("wifi: read_poll start reg=0x%04X retry=%u interval=%ums" VSF_TRACE_CFG_LINEEND,
            (unsigned)reg, (unsigned)max_retry, (unsigned)interval_ms);

    vsf_err_t err = wifi->bus_ops->reg_read(wifi, reg,
            &wifi->s.read_poll.last_val, __vsf_wifi_read_poll_done);
    if (VSF_ERR_NONE != err) {
        wifi->script_busy = false;
        wifi->script_done = NULL;
        memset(&wifi->s, 0, sizeof(wifi->s));
    }
    return err;
}

/*============================ READ DISPATCHER ===============================*
 *
 * Single-shot 32-bit register read.  Thin wrapper over bus_ops->reg_read;
 * its only job beyond the bus call is to honour the script_busy single-
 * flight contract so chip drivers can mix run_read with run_script /
 * run_blob / run_read_poll without colliding on the dispatcher slot.
 *==========================================================================*/

static void __vsf_wifi_read_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    __vsf_wifi_script_finish(wifi, err);
}

vsf_err_t vsf_wifi_run_read(vsf_wifi_t *wifi, uint16_t reg, uint32_t *out,
        vsf_wifi_done_t done)
{
    if (wifi->script_busy)               return VSF_ERR_NOT_AVAILABLE;
    if (NULL == out)                     return VSF_ERR_INVALID_PARAMETER;
    if (NULL == wifi->bus_ops->reg_read) return VSF_ERR_NOT_SUPPORT;

    wifi->script_is_blob = false;
    wifi->script_done    = done;
    memset(&wifi->s, 0, sizeof(wifi->s));
    wifi->script_busy    = true;

    vsf_err_t err = wifi->bus_ops->reg_read(wifi, reg, out, __vsf_wifi_read_done);
    if (VSF_ERR_NONE != err) {
        wifi->script_busy = false;
        wifi->script_done = NULL;
    }
    return err;
}

/*============================ START / FINI / INIT CHAIN =====================*/

void vsf_wifi_init(vsf_wifi_t *wifi,
        const vsf_wifi_chip_drv_t *drv,
        const vsf_wifi_bus_ops_t  *bus_ops,
        vsf_eda_t                 *post_eda)
{
    memset(wifi, 0, sizeof(*wifi));
    wifi->drv      = drv;
    wifi->bus_ops  = bus_ops;
    wifi->post_eda = post_eda;
    wifi->channel  = 1;
#if VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER == ENABLED
    vsf_callback_timer_init(&wifi->scan_timer);
    wifi->scan_timer.on_timer = __vsf_wifi_scan_timer_cb;
    vsf_callback_timer_init(&wifi->read_poll_timer);
    wifi->read_poll_timer.on_timer = __vsf_wifi_read_poll_timer_cb;
    vsf_callback_timer_init(&wifi->mlme_timer);
    wifi->mlme_timer.on_timer = __vsf_wifi_mlme_timer_cb;
#endif
}

void vsf_wifi_set_attach_fail(vsf_wifi_t *wifi, vsf_wifi_attach_fail_t hook)
{
    wifi->attach_fail = hook;
}

void vsf_wifi_start(vsf_wifi_t *wifi)
{
    vsf_trace_info("wifi: start: chip=%s, firmware=%s" VSF_TRACE_CFG_LINEEND,
            (wifi->drv->name != NULL) ? wifi->drv->name : "?",
            (wifi->drv->firmware_load != NULL) ? "yes" : "no");
    if (wifi->drv->firmware_load != NULL) {
        vsf_trace_info("wifi: uploading firmware ..." VSF_TRACE_CFG_LINEEND);
        if (VSF_ERR_NONE != wifi->drv->firmware_load(wifi,
                __vsf_wifi_on_fw_done)) {
            __vsf_wifi_attach_fail(wifi, VSF_ERR_FAIL);
        }
    } else {
        __vsf_wifi_on_fw_done(wifi, VSF_ERR_NONE);
    }
}

static void __vsf_wifi_on_fw_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (VSF_ERR_NONE != err) {
        vsf_trace_error("wifi: firmware upload failed (err=%d)" VSF_TRACE_CFG_LINEEND, (int)err);
        __vsf_wifi_attach_fail(wifi, err);
        return;
    }
    vsf_trace_info("wifi: firmware uploaded, running init script ..." VSF_TRACE_CFG_LINEEND);
    if (VSF_ERR_NONE != wifi->drv->init(wifi, __vsf_wifi_on_init_done)) {
        __vsf_wifi_attach_fail(wifi, VSF_ERR_FAIL);
    }
}

static void __vsf_wifi_on_init_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (VSF_ERR_NONE != err) {
        vsf_trace_error("wifi: init script failed (err=%d)" VSF_TRACE_CFG_LINEEND, (int)err);
        __vsf_wifi_attach_fail(wifi, err);
        return;
    }
    /* Program the MAC address (populated from EEPROM / fallback during init)
     * into the chip's unicast-filter registers so hardware ACKs work. */
    if (wifi->drv->set_mac_addr != NULL) {
        err = wifi->drv->set_mac_addr(wifi, wifi->mac, __vsf_wifi_on_mac_set_done);
        if (VSF_ERR_NONE != err) {
            __vsf_wifi_attach_fail(wifi, err);
        }
        return;
    }
    __vsf_wifi_on_mac_set_done(wifi, VSF_ERR_NONE);
}

static void __vsf_wifi_on_mac_set_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (VSF_ERR_NONE != err) {
        vsf_trace_warning("wifi: set_mac_addr failed (err=%d), continuing"
                VSF_TRACE_CFG_LINEEND, (int)err);
    }
    if (NULL == wifi->drv->set_rx_filter) {
        __vsf_wifi_on_rxfilter_done(wifi, VSF_ERR_NONE);
        return;
    }
    err = wifi->drv->set_rx_filter(wifi, 0, __vsf_wifi_on_rxfilter_done);
    if (VSF_ERR_NONE != err) {
        __vsf_wifi_attach_fail(wifi, err);
    }
}

static void __vsf_wifi_on_rxfilter_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (VSF_ERR_NONE != err) {
        __vsf_wifi_attach_fail(wifi, err);
        return;
    }
    wifi->is_ready = true;
    vsf_trace_info("wifi: MCU ready, chip is up" VSF_TRACE_CFG_LINEEND);
    if (wifi->bus_ops->on_ready != NULL) {
        wifi->bus_ops->on_ready(wifi);
    }
    vsf_wifi_on_ready(wifi);
}

void vsf_wifi_fini(vsf_wifi_t *wifi)
{
    /* Step 1: gate further events.  After this flag is set every async
     * callback bails out, so any in-flight bus completion racing us is
     * harmless. */
    wifi->disconnecting = true;
    wifi->is_ready      = false;
    wifi->scanning      = false;

#if VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER == ENABLED
    vsf_callback_timer_remove(&wifi->scan_timer);
    vsf_callback_timer_remove(&wifi->read_poll_timer);
    vsf_callback_timer_remove(&wifi->mlme_timer);
#endif

    /* Step 2: notify user before tearing down chip state. */
    vsf_wifi_on_del(wifi);

    if (wifi->drv != NULL && wifi->drv->fini != NULL) {
        wifi->drv->fini(wifi);
    }
}

/*============================ RX ROUTING ====================================*/

void vsf_wifi_on_rx_internal(vsf_wifi_t *wifi, uint8_t *frame, uint16_t len)
{
    if (wifi->disconnecting) return;
    /* During a scan the chip parser owns the frame so it can extract
     * beacons / probe responses; while an MLME handshake is in progress (or
     * the link is up) the parser also routes mgmt frames to vsf_wifi_mlme_rx
     * so auth / assoc-resp / deauth are observed.  Otherwise hand the raw
     * frame to the application. */
    if ((wifi->scanning || (wifi->mlme_state != WIFI_MLME_IDLE))
            && wifi->drv->parse_rx != NULL) {
        wifi->drv->parse_rx(wifi, frame, len);
    } else {
        vsf_wifi_on_rx(wifi, frame, len);
    }
}

#if VSF_WIFI_USE_WPA == ENABLED
/*============================ SOFTWARE CCMP DATA PATH =======================*
 * AES-CCMP encap/decap for unicast/multicast data MPDUs when no hardware
 * crypto backend is mounted (wpa_hw_crypto == false).  Layout produced /
 * consumed: [802.11 hdr][CCMP hdr 8B][cipher == plaintext_len][MIC 8B].
 *==========================================================================*/

#ifndef __VSF_WIFI_CCMP_BUF_SIZE
#   define __VSF_WIFI_CCMP_BUF_SIZE     1600
#endif

/* Build the CCMP AAD and CCM nonce for a 3-address data MPDU, matching the
 * IEEE 802.11 masking rules (see hostap wlantest reference):
 *   FC : subtype bits (0x70 of byte0) masked; Retry/PwrMgt/MoreData (byte1
 *        0x08|0x10|0x20) masked; Protected (0x40) forced on.
 *   SC : sequence number masked out, fragment number kept.
 * aad = FC(2)|A1(6)|A2(6)|A3(6)|SC(2)[|QC(2)]; nonce = prio(1)|A2(6)|PN(6).
 * pn[] is little-endian (pn[0] = LSB); the nonce carries PN MSB-first. */
static void __ccmp_aad_nonce(const uint8_t *dot11, uint16_t hdr_len, bool qos,
        const uint8_t pn[6], uint8_t *aad, uint16_t *aad_len, uint8_t nonce[13])
{
    uint8_t prio = 0;
    uint16_t n = 0;

    aad[n++] = dot11[0] & ~0x70;
    aad[n++] = (dot11[1] & ~(0x08 | 0x10 | 0x20)) | 0x40;
    memcpy(&aad[n], &dot11[4],  6); n += 6;     /* A1 */
    memcpy(&aad[n], &dot11[10], 6); n += 6;     /* A2 */
    memcpy(&aad[n], &dot11[16], 6); n += 6;     /* A3 */
    aad[n++] = dot11[22] & 0x0F;                /* SC: keep frag#, drop seq# */
    aad[n++] = 0;
    if (qos) {
        prio     = dot11[24] & 0x0F;            /* QoS Control TID */
        aad[n++] = prio;
        aad[n++] = 0;
    }
    *aad_len = n;

    nonce[0] = prio;
    memcpy(&nonce[1], &dot11[10], 6);           /* A2 */
    nonce[7]  = pn[5];
    nonce[8]  = pn[4];
    nonce[9]  = pn[3];
    nonce[10] = pn[2];
    nonce[11] = pn[1];
    nonce[12] = pn[0];
}

/* CCMP-encrypt a plaintext data MPDU `frame` (802.11 hdr + payload) into
 * `out` (capacity `cap`).  Advances wifi->wpa_tx_pn.  Returns the encrypted
 * MPDU length, or 0 on failure. */
static uint16_t __vsf_wifi_ccmp_encap(vsf_wifi_t *wifi,
        const uint8_t *frame, uint16_t len, uint8_t *out, uint16_t cap)
{
    bool     qos     = ((frame[0] >> 4) & 0x0F) & 0x08;
    uint16_t hdr_len = qos ? 26 : 24;
    if (len <= hdr_len)                          return 0;

    uint16_t payload_len = len - hdr_len;
    uint16_t total       = hdr_len + 8 + payload_len + 8;
    if (total > cap)                             return 0;

    /* Advance the 48-bit PN (little-endian). */
    uint8_t *pn = wifi->wpa_tx_pn;
    for (int i = 0; i < 6; i++) { if (++pn[i] != 0) break; }

    /* Header with Protected bit set. */
    memcpy(out, frame, hdr_len);
    out[1] |= 0x40;

    /* CCMP header (Ext IV, key id 0). */
    uint8_t *cc = out + hdr_len;
    cc[0] = pn[0];
    cc[1] = pn[1];
    cc[2] = 0;
    cc[3] = 0x20;
    cc[4] = pn[2];
    cc[5] = pn[3];
    cc[6] = pn[4];
    cc[7] = pn[5];

    uint8_t  aad[32], nonce[13], mic[8];
    uint16_t aad_len;
    __ccmp_aad_nonce(out, hdr_len, qos, pn, aad, &aad_len, nonce);

    const uint8_t *tk = wifi->wpa_ptk + 32;
    if (vsf_wifi_ccmp_encrypt(tk, aad, aad_len, nonce,
            frame + hdr_len, payload_len, cc + 8, mic) != VSF_ERR_NONE) {
        return 0;
    }
    memcpy(cc + 8 + payload_len, mic, 8);
    return total;
}

/* CCMP-decrypt an encrypted data MPDU `dot11` into `out` (capacity `cap`),
 * reconstructing a plaintext 802.11 frame (Protected bit cleared).  Returns
 * the plaintext MPDU length, or 0 on MIC failure / malformed input. */
static uint16_t __vsf_wifi_ccmp_decap(vsf_wifi_t *wifi,
        const uint8_t *dot11, uint16_t len, uint8_t *out, uint16_t cap)
{
    bool     qos     = ((dot11[0] >> 4) & 0x0F) & 0x08;
    uint16_t hdr_len = qos ? 26 : 24;
    if (len < (hdr_len + 8 + 8))                 return 0;

    const uint8_t *cc = dot11 + hdr_len;
    if ((cc[3] & 0x20) == 0)                     return 0;   /* Ext IV required */

    uint8_t pn[6];
    pn[0] = cc[0];
    pn[1] = cc[1];
    pn[2] = cc[4];
    pn[3] = cc[5];
    pn[4] = cc[6];
    pn[5] = cc[7];

    uint16_t cipher_len = len - hdr_len - 8 - 8;
    const uint8_t *cipher = cc + 8;
    const uint8_t *mic    = cipher + cipher_len;
    if ((hdr_len + cipher_len) > cap)            return 0;

    uint8_t  aad[32], nonce[13];
    uint16_t aad_len;
    __ccmp_aad_nonce(dot11, hdr_len, qos, pn, aad, &aad_len, nonce);

    /* Unicast (A1 individual addr) uses the pairwise TK; group-addressed
     * frames (A1 bit0 == 1) use the GTK installed from M3. */
    const uint8_t *tk = (dot11[4] & 0x01) ? wifi->wpa_gtk : (wifi->wpa_ptk + 32);

    memcpy(out, dot11, hdr_len);
    out[1] &= ~0x40;
    if (vsf_wifi_ccmp_decrypt(tk, aad, aad_len, nonce,
            cipher, cipher_len, out + hdr_len, mic) != VSF_ERR_NONE) {
        return 0;
    }
    return hdr_len + cipher_len;
}
#endif      // VSF_WIFI_USE_WPA

/*
 * Data-frame RX (post-association).  The chip parser hands us a naked 802.11
 * data frame (starting at the FC field) once the link is up.  We:
 *   - compute the data header length (24B, or 26B for QoS data);
 *   - on a plaintext frame, detect EAPOL (LLC/SNAP 'AA AA 03 00 00 00' +
 *     ethertype 0x888E) and route it to the 4-way handshake;
 *   - encrypted (Protected) frames are CCMP-decrypted in Task 5; for now
 *     they are dropped.
 * Non-EAPOL plaintext data is handed to the application via vsf_wifi_on_rx.
 */
void vsf_wifi_data_rx(vsf_wifi_t *wifi, const uint8_t *dot11, uint16_t len)
{
    if (wifi->disconnecting) return;
    if (len < 24) return;

    uint8_t  fc1     = dot11[1];
    uint8_t  subtype = (dot11[0] >> 4) & 0x0F;
    bool     prot    = (fc1 & 0x40) != 0;       /* FC byte1 bit6 = Protected */

    /* QoS data (subtype bit 3) carries a 2-byte QoS Control field. */
    uint16_t hdr_len = 24;
    if (subtype & 0x08) {
        hdr_len += 2;
    }
    if (len <= hdr_len) return;

    const uint8_t *payload     = dot11 + hdr_len;
    uint16_t       payload_len = len - hdr_len;

    if (prot) {
#if VSF_WIFI_USE_WPA == ENABLED
        /* CCMP-encrypted business frame.  When the keys live in a hardware
         * crypto engine the chip already decrypted it (or the chip parser
         * never sets Protected), so just forward; otherwise software-decrypt
         * and hand the recovered plaintext frame to the application. */
        if (!wifi->wpa_ptk_valid) return;
        if (wifi->wpa_hw_crypto) {
            vsf_wifi_on_rx(wifi, (uint8_t *)dot11, len);
            return;
        }
        {
            static uint32_t __ccmp_rx32[(__VSF_WIFI_CCMP_BUF_SIZE + 3) / 4];
            uint8_t *out = (uint8_t *)__ccmp_rx32;
            uint16_t plen = __vsf_wifi_ccmp_decap(wifi, dot11, len,
                    out, (uint16_t)sizeof(__ccmp_rx32));
            if (plen == 0) return;              /* MIC failure / malformed */
            vsf_wifi_on_rx(wifi, out, plen);
        }
        return;
#else
        /* No WPA support compiled in: drop encrypted frames. */
        return;
#endif
    }

#if VSF_WIFI_USE_WPA == ENABLED
    /* Plaintext EAPOL detection: LLC/SNAP header + 0x888E ethertype.
     * 4-way handshake EAPOL-Key frames are always sent in the clear. */
    static const uint8_t __snap_eapol[8] = {
        0xAA, 0xAA, 0x03, 0x00, 0x00, 0x00, 0x88, 0x8E
    };
    if ((payload_len > 8) && (memcmp(payload, __snap_eapol, 8) == 0)) {
        vsf_wifi_eapol_rx(wifi, payload + 8, payload_len - 8);
        return;
    }
#endif

    /* Non-EAPOL plaintext data: hand the naked 802.11 frame to the app. */
    vsf_wifi_on_rx(wifi, (uint8_t *)dot11, len);
}

/*============================ SCAN SCHEDULER ================================*/

#if VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER == ENABLED

static void __vsf_wifi_scan_finish(vsf_wifi_t *wifi)
{
    if (!wifi->scanning) return;
    wifi->scanning          = false;
    wifi->scan_channel_idx  = 0;
    wifi->scan_num_channels = 0;
    vsf_wifi_on_scan_done(wifi);
}

/* Called from kernel timer EDA — bounce back to the bus driver's EDA so the
 * actual hop runs in the same context as bus completions. */
static void __vsf_wifi_scan_timer_cb(vsf_callback_timer_t *timer)
{
    vsf_wifi_t *wifi = vsf_container_of(timer, vsf_wifi_t, scan_timer);
    if (wifi->disconnecting || !wifi->scanning) return;
    if (wifi->post_eda != NULL) {
        vsf_eda_post_evt(wifi->post_eda, VSF_WIFI_EVT_SCAN_HOP);
    }
}

/* Done callback from drv->set_channel(channels[idx], ...). */
static void __vsf_wifi_scan_hop_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (wifi->disconnecting || !wifi->scanning) return;
    if (VSF_ERR_NONE != err) {
        __vsf_wifi_scan_finish(wifi);
        return;
    }
#if VSF_WIFI_CFG_SCAN_ACTIVE == ENABLED
    /* Active scan: probe the freshly-locked channel so silent / hidden APs
     * answer with a probe-response (parsed by drv->parse_rx, same path as
     * beacons).  Best-effort: a tx failure must not abort the scan. */
    __vsf_wifi_send_probe_req(wifi);
#endif
    /* Channel locked; arm dwell timer.  Beacons / probe responses arriving
     * during this window are routed to drv->parse_rx via the RX path. */
    vsf_callback_timer_add_ms(&wifi->scan_timer, wifi->scan_dwell_ms);
}

#if VSF_WIFI_CFG_SCAN_ACTIVE == ENABLED
/* Build + send a broadcast (wildcard-SSID) probe-request on the current
 * channel.  Bypasses the scanning guard via __vsf_wifi_tx_frame so it can
 * run from inside the scan scheduler. */
static void __vsf_wifi_send_probe_req(vsf_wifi_t *wifi)
{
    uint8_t  frame[64];     /* probe-req is 36 bytes; round up with margin */
    uint16_t i = 0;

    /* 802.11 probe-request MAC header (24 bytes). */
    frame[i++] = 0x40;  frame[i++] = 0x00;          /* FC: mgmt, probe-req */
    frame[i++] = 0x00;  frame[i++] = 0x00;          /* duration            */
    memset(&frame[i], 0xFF, 6);      i += 6;        /* DA = broadcast      */
    memcpy(&frame[i], wifi->mac, 6); i += 6;        /* SA = our MAC        */
    memset(&frame[i], 0xFF, 6);      i += 6;        /* BSSID = broadcast   */
    frame[i++] = 0x00;  frame[i++] = 0x00;          /* seq ctrl (hw fills) */
    /* IE: SSID (wildcard, length 0). */
    frame[i++] = 0x00;  frame[i++] = 0x00;
    /* IE: supported rates 1/2/5.5/11/6/12/24/36 Mbps. */
    frame[i++] = 0x01;  frame[i++] = 0x08;
    frame[i++] = 0x82;  frame[i++] = 0x84;  frame[i++] = 0x8B;  frame[i++] = 0x96;
    frame[i++] = 0x0C;  frame[i++] = 0x18;  frame[i++] = 0x30;  frame[i++] = 0x48;

    vsf_err_t err = __vsf_wifi_tx_frame(wifi, frame, i);
    if (VSF_ERR_NONE != err) {
        vsf_trace_warning("wifi: active-scan probe-req tx failed (err=%d)"
                VSF_TRACE_CFG_LINEEND, (int)err);
    }
}
#endif

/* Issued from the bus driver's EDA when VSF_WIFI_EVT_SCAN_HOP lands —
 * step to next channel or end. */
static void __vsf_wifi_scan_advance(vsf_wifi_t *wifi)
{
    if (!wifi->scanning) return;
    wifi->scan_channel_idx++;
    if (wifi->scan_channel_idx >= wifi->scan_num_channels) {
        __vsf_wifi_scan_finish(wifi);
        return;
    }
    if (NULL == wifi->drv->set_channel) {
        __vsf_wifi_scan_finish(wifi);
        return;
    }
    uint8_t ch = wifi->scan_channels[wifi->scan_channel_idx];
    wifi->channel = ch;
    if (VSF_ERR_NONE != wifi->drv->set_channel(wifi, ch,
            __vsf_wifi_scan_hop_done)) {
        __vsf_wifi_scan_finish(wifi);
    }
}

void vsf_wifi_on_scan_hop_evt(vsf_wifi_t *wifi)
{
    if (wifi->disconnecting) return;
    __vsf_wifi_scan_advance(wifi);
}

#else  /* VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER == ENABLED */

void vsf_wifi_on_scan_hop_evt(vsf_wifi_t *wifi) { (void)wifi; }

#endif

/*============================ MLME STATE MACHINE ===========================*
 *
 * OPEN-system authentication + association, driven entirely by the wifi
 * layer.  Sequence (each TX runs in the bus EDA / a bus-completion cb):
 *
 *   vsf_wifi_connect (any ctx)
 *     -> drv->connect (lock channel + bssid + rx-filter, ep0 script)
 *     -> __vsf_wifi_mlme_connect_done (bus EDA): state=AUTH, send auth-req,
 *        arm mlme_timer
 *     -> auth-resp(status=0) via vsf_wifi_mlme_rx (bus EDA): state=ASSOC,
 *        send assoc-req, arm timer
 *     -> assoc-resp(status=0): state=RUN, vsf_wifi_on_link_up
 *
 * On each handshake timeout the mlme_timer cb bounces VSF_WIFI_EVT_MLME_RETRY
 * to the bus EDA, which retransmits up to __VSF_WIFI_MLME_MAX_RETRY times
 * before declaring on_link_down(timeout).
 *==========================================================================*/

#define __VSF_WIFI_MLME_MAX_RETRY       3
#define __VSF_WIFI_MLME_TIMEOUT_MS      400
#if VSF_WIFI_USE_WPA == ENABLED
/* Whole-handshake budget: the AP retransmits M1/M3 on its own schedule, so a
 * single generous timeout covers all four messages; expiry aborts the link. */
#define __VSF_WIFI_WPA_TIMEOUT_MS       3000
#endif

/* 802.11 mgmt subtypes (high nibble of FC byte0). */
#define __DOT11_STYPE_ASSOC_REQ         0x0
#define __DOT11_STYPE_ASSOC_RESP        0x1
#define __DOT11_STYPE_PROBE_RESP        0x5
#define __DOT11_STYPE_BEACON            0x8
#define __DOT11_STYPE_DISASSOC          0xA
#define __DOT11_STYPE_AUTH              0xB
#define __DOT11_STYPE_DEAUTH            0xC

/* Build the 24-byte 802.11 mgmt header into buf and return the next write
 * offset (24).  fc0 = first FC byte (subtype<<4 | type(=mgmt,0)<<2).  All
 * three addresses are the target BSSID except addr2 (our MAC). */
static uint16_t __vsf_wifi_mlme_hdr(vsf_wifi_t *wifi, uint8_t *buf, uint8_t fc0)
{
    uint16_t i = 0;
    buf[i++] = fc0;  buf[i++] = 0x00;               /* FC                  */
    buf[i++] = 0x00; buf[i++] = 0x00;               /* duration            */
    memcpy(&buf[i], wifi->mlme_bssid, 6); i += 6;   /* addr1 = DA = BSSID  */
    memcpy(&buf[i], wifi->mac,        6); i += 6;   /* addr2 = SA = our MAC*/
    memcpy(&buf[i], wifi->mlme_bssid, 6); i += 6;   /* addr3 = BSSID       */
    buf[i++] = 0x00; buf[i++] = 0x00;               /* seq ctrl (hw fills) */
    return i;
}

/* OPEN-system auth-request (30 bytes). */
static void __vsf_wifi_mlme_send_auth(vsf_wifi_t *wifi)
{
    uint8_t  frame[64];
    uint16_t i = __vsf_wifi_mlme_hdr(wifi, frame, 0xB0);  /* subtype 0xB */
    frame[i++] = 0x00; frame[i++] = 0x00;   /* auth algorithm = open(0)   */
    frame[i++] = 0x01; frame[i++] = 0x00;   /* transaction seq    = 1     */
    frame[i++] = 0x00; frame[i++] = 0x00;   /* status code        = 0     */
    vsf_err_t err = __vsf_wifi_tx_frame(wifi, frame, i);
    if (VSF_ERR_NONE != err) {
        vsf_trace_warning("wifi: mlme auth-req tx failed (err=%d)"
                VSF_TRACE_CFG_LINEEND, (int)err);
    }
#if VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER == ENABLED
    vsf_callback_timer_add_ms(&wifi->mlme_timer, __VSF_WIFI_MLME_TIMEOUT_MS);
#endif
}

/* Association-request (variable length: header + fixed fields + IEs). */
static void __vsf_wifi_mlme_send_assoc(vsf_wifi_t *wifi)
{
    uint8_t  frame[128];
    uint16_t i = __vsf_wifi_mlme_hdr(wifi, frame, 0x00);  /* subtype 0x0 */
    frame[i++] = 0x01; frame[i++] = 0x00;   /* capability info: ESS       */
    frame[i++] = 0x0A; frame[i++] = 0x00;   /* listen interval = 10       */
    /* SSID IE. */
    frame[i++] = 0x00; frame[i++] = wifi->mlme_ssid_len;
    if (wifi->mlme_ssid_len > 0) {
        memcpy(&frame[i], wifi->mlme_ssid, wifi->mlme_ssid_len);
        i += wifi->mlme_ssid_len;
    }
    /* Supported-rates IE: 1/2/5.5/11/6/12/24/36 Mbps. */
    frame[i++] = 0x01; frame[i++] = 0x08;
    frame[i++] = 0x82; frame[i++] = 0x84; frame[i++] = 0x8B; frame[i++] = 0x96;
    frame[i++] = 0x0C; frame[i++] = 0x18; frame[i++] = 0x30; frame[i++] = 0x48;
#if VSF_WIFI_USE_WPA == ENABLED
    /* RSN IE (tag 48) for WPA2-PSK / CCMP.  Suite OUI = 00-0F-AC; group +
     * pairwise = CCMP(4), AKM = PSK(2).  Cache the exact bytes so the 4-way
     * handshake M2 can echo this IE unchanged. */
    if (wifi->wpa_auth.auth_mode == WIFI_AUTH_WPA2_PSK) {
        static const uint8_t __rsn_ie[] = {
            0x30, 0x14,                 /* tag 48, length 20                 */
            0x01, 0x00,                 /* RSN version 1                     */
            0x00, 0x0F, 0xAC, 0x04,     /* group cipher  = CCMP              */
            0x01, 0x00,                 /* pairwise count = 1                */
            0x00, 0x0F, 0xAC, 0x04,     /* pairwise      = CCMP              */
            0x01, 0x00,                 /* AKM count     = 1                 */
            0x00, 0x0F, 0xAC, 0x02,     /* AKM           = PSK               */
            0x00, 0x00,                 /* RSN capabilities                  */
        };
        memcpy(&frame[i], __rsn_ie, sizeof(__rsn_ie));
        i += sizeof(__rsn_ie);
        memcpy(wifi->wpa_rsn_ie, __rsn_ie, sizeof(__rsn_ie));
        wifi->wpa_rsn_ie_len = (uint8_t)sizeof(__rsn_ie);
    }
#endif
    vsf_err_t err = __vsf_wifi_tx_frame(wifi, frame, i);
    if (VSF_ERR_NONE != err) {
        vsf_trace_warning("wifi: mlme assoc-req tx failed (err=%d)"
                VSF_TRACE_CFG_LINEEND, (int)err);
    }
#if VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER == ENABLED
    vsf_callback_timer_add_ms(&wifi->mlme_timer, __VSF_WIFI_MLME_TIMEOUT_MS);
#endif
}

/* Deauthentication (26 bytes).  Best-effort; fire-and-forget. */
static void __vsf_wifi_mlme_send_deauth(vsf_wifi_t *wifi, uint16_t reason)
{
    uint8_t  frame[64];
    uint16_t i = __vsf_wifi_mlme_hdr(wifi, frame, 0xC0);  /* subtype 0xC */
    frame[i++] = reason & 0xFF; frame[i++] = (reason >> 8) & 0xFF;
    (void)__vsf_wifi_tx_frame(wifi, frame, i);
}

/* Reset MLME to IDLE and notify the application of a link-down.  Called for
 * every non-RUN exit (timeout / rejected / deauth / user disconnect). */
static void __vsf_wifi_mlme_finish(vsf_wifi_t *wifi, uint8_t reason)
{
#if VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER == ENABLED
    vsf_callback_timer_remove(&wifi->mlme_timer);
#endif
    bool was_active = (wifi->mlme_state != WIFI_MLME_IDLE);
    wifi->mlme_state = WIFI_MLME_IDLE;
    wifi->mlme_retry = 0;
    wifi->mlme_aid   = 0;
#if VSF_WIFI_USE_WPA == ENABLED
    wifi->wpa_ptk_valid = false;
#endif
    if (was_active) {
        vsf_wifi_on_link_down(wifi, reason);
    }
}

/* drv->connect completion: channel/bssid/rx-filter locked -> kick off the
 * OPEN-system handshake by sending the auth-request.  Runs in the bus EDA. */
static void __vsf_wifi_mlme_connect_done(vsf_wifi_t *wifi, vsf_err_t err)
{
    if (wifi->disconnecting) return;
    if (wifi->mlme_state != WIFI_MLME_AUTH) return;
    if (VSF_ERR_NONE != err) {
        vsf_trace_error("wifi: mlme connect prep failed (err=%d)"
                VSF_TRACE_CFG_LINEEND, (int)err);
        __vsf_wifi_mlme_finish(wifi, WIFI_REASON_UNSPECIFIED);
        return;
    }
    wifi->mlme_retry = __VSF_WIFI_MLME_MAX_RETRY;
    __vsf_wifi_mlme_send_auth(wifi);
}

#if VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER == ENABLED
/* mlme_timer expiry (kernel timer EDA) — bounce to the bus EDA so the
 * retransmit TX runs in the same context as bus completions. */
static void __vsf_wifi_mlme_timer_cb(vsf_callback_timer_t *timer)
{
    vsf_wifi_t *wifi = vsf_container_of(timer, vsf_wifi_t, mlme_timer);
    if (wifi->disconnecting) return;
    if ((wifi->mlme_state != WIFI_MLME_AUTH)
            && (wifi->mlme_state != WIFI_MLME_ASSOC)
            && (wifi->mlme_state != WIFI_MLME_4WAY)) {
        return;
    }
    if (wifi->post_eda != NULL) {
        vsf_eda_post_evt(wifi->post_eda, VSF_WIFI_EVT_MLME_RETRY);
    }
}
#endif

void vsf_wifi_on_mlme_retry_evt(vsf_wifi_t *wifi)
{
    if (wifi->disconnecting) return;
#if VSF_WIFI_USE_WPA == ENABLED
    /* The 4-way handshake has no per-message retransmit on our side (the AP
     * retries M1/M3); a timer expiry here means the handshake stalled. */
    if (wifi->mlme_state == WIFI_MLME_4WAY) {
        vsf_trace_warning("wifi: 4-way handshake timeout" VSF_TRACE_CFG_LINEEND);
        __vsf_wifi_mlme_finish(wifi, WIFI_REASON_LOCAL_TIMEOUT);
        return;
    }
#endif
    if ((wifi->mlme_state != WIFI_MLME_AUTH)
            && (wifi->mlme_state != WIFI_MLME_ASSOC)) {
        return;
    }
    if (wifi->mlme_retry == 0) {
        vsf_trace_warning("wifi: mlme handshake timeout (state=%u)"
                VSF_TRACE_CFG_LINEEND, (unsigned)wifi->mlme_state);
        __vsf_wifi_mlme_finish(wifi, WIFI_REASON_LOCAL_TIMEOUT);
        return;
    }
    wifi->mlme_retry--;
    if (wifi->mlme_state == WIFI_MLME_AUTH) {
        __vsf_wifi_mlme_send_auth(wifi);
    } else {
        __vsf_wifi_mlme_send_assoc(wifi);
    }
}

/* Read a little-endian uint16 from a byte pointer. */
static uint16_t __vsf_wifi_rd16(const uint8_t *p)
{
    return (uint16_t)(p[0] | ((uint16_t)p[1] << 8));
}

void vsf_wifi_mlme_rx(vsf_wifi_t *wifi, const uint8_t *dot11, uint16_t len)
{
    if (wifi->disconnecting) return;
    if (wifi->mlme_state == WIFI_MLME_IDLE) return;
    if ((dot11 == NULL) || (len < 24)) return;

    uint8_t  subtype = (dot11[0] >> 4) & 0x0F;
    const uint8_t *body = dot11 + 24;       /* skip 24-byte mgmt header */
    uint16_t       body_len = len - 24;

    switch (subtype) {
    case __DOT11_STYPE_AUTH:
        /* auth-resp: algorithm(2)=0, seq(2)=2, status(2)=0. */
        if (wifi->mlme_state != WIFI_MLME_AUTH) return;
        if (body_len < 6) return;
        if (__vsf_wifi_rd16(&body[0]) != 0) return;     /* algorithm = open */
        if (__vsf_wifi_rd16(&body[2]) != 2) return;     /* expect seq 2     */
        {
            uint16_t status = __vsf_wifi_rd16(&body[4]);
            if (status != 0) {
                vsf_trace_warning("wifi: auth rejected (status=%u)"
                        VSF_TRACE_CFG_LINEEND, (unsigned)status);
                __vsf_wifi_mlme_finish(wifi, WIFI_REASON_AUTH_REJECTED);
                return;
            }
        }
#if VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER == ENABLED
        vsf_callback_timer_remove(&wifi->mlme_timer);
#endif
        vsf_trace_info("wifi: auth ok, sending assoc-req" VSF_TRACE_CFG_LINEEND);
        wifi->mlme_state = WIFI_MLME_ASSOC;
        wifi->mlme_retry = __VSF_WIFI_MLME_MAX_RETRY;
        __vsf_wifi_mlme_send_assoc(wifi);
        break;

    case __DOT11_STYPE_ASSOC_RESP:
        /* assoc-resp: capability(2), status(2), AID(2). */
        if (wifi->mlme_state != WIFI_MLME_ASSOC) return;
        if (body_len < 6) return;
        {
            uint16_t status = __vsf_wifi_rd16(&body[2]);
            if (status != 0) {
                vsf_trace_warning("wifi: assoc rejected (status=%u)"
                        VSF_TRACE_CFG_LINEEND, (unsigned)status);
                __vsf_wifi_mlme_finish(wifi, WIFI_REASON_ASSOC_REJECTED);
                return;
            }
            wifi->mlme_aid = __vsf_wifi_rd16(&body[4]) & 0x3FFF;
        }
#if VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER == ENABLED
        vsf_callback_timer_remove(&wifi->mlme_timer);
#endif
        wifi->mlme_retry = 0;
#if VSF_WIFI_USE_WPA == ENABLED
        /* WPA2-PSK: associate first, then run the 4-way handshake before the
         * link is usable.  Stay in 4WAY (no link-up yet) and wait for the
         * AP's EAPOL-Key M1; vsf_wifi_data_rx routes it to the WPA module. */
        if ((wifi->wpa_auth.auth_mode == WIFI_AUTH_WPA2_PSK)
                && (wifi->wpa_auth.psk_len == VSF_WIFI_PMK_LEN)) {
            wifi->wpa_ptk_valid = false;
            wifi->mlme_state    = WIFI_MLME_4WAY;
            vsf_trace_info("wifi: associated, aid=%u (4-way handshake)"
                    VSF_TRACE_CFG_LINEEND, (unsigned)wifi->mlme_aid);
            vsf_wifi_mlme_arm_timer(wifi, __VSF_WIFI_WPA_TIMEOUT_MS);
            break;
        }
#endif
        wifi->mlme_state = WIFI_MLME_RUN;
        vsf_trace_info("wifi: associated, aid=%u (link up)"
                VSF_TRACE_CFG_LINEEND, (unsigned)wifi->mlme_aid);
        {
            vsf_wifi_link_info_t info;
            memset(&info, 0, sizeof(info));
            memcpy(info.bssid, wifi->mlme_bssid, 6);
            info.channel = wifi->mlme_channel;
            info.flags   = WIFI_LINK_FLAG_CONNECTED | WIFI_LINK_FLAG_AUTHORIZED;
            vsf_wifi_on_link_up(wifi, &info);
        }
        break;

    case __DOT11_STYPE_DEAUTH:
    case __DOT11_STYPE_DISASSOC: {
        uint16_t reason = (body_len >= 2) ? __vsf_wifi_rd16(&body[0])
                                          : WIFI_REASON_UNSPECIFIED;
        vsf_trace_info("wifi: received %s (reason=%u)" VSF_TRACE_CFG_LINEEND,
                (subtype == __DOT11_STYPE_DEAUTH) ? "deauth" : "disassoc",
                (unsigned)reason);
        __vsf_wifi_mlme_finish(wifi, (uint8_t)reason);
        break;
    }

    default:
        break;
    }
}

/*============================ USER API ======================================*/

bool vsf_wifi_is_ready(vsf_wifi_t *wifi)
{
    return wifi->is_ready;
}

uint8_t vsf_wifi_get_channel(vsf_wifi_t *wifi)
{
    return wifi->channel;
}

const char * vsf_wifi_get_chip_name(vsf_wifi_t *wifi)
{
    return wifi->drv ? wifi->drv->name : "unknown";
}

const uint8_t * vsf_wifi_get_mac(vsf_wifi_t *wifi)
{
    return wifi->mac;
}

void vsf_wifi_set_channel(vsf_wifi_t *wifi, uint8_t channel)
{
    if (channel < 1)   channel = 1;
    if (channel > 196) channel = 196;   /* 2.4 GHz + 5 GHz; driver validates */
    wifi->channel = channel;
    /* Scan owner has set_channel scheduled — bumping it from outside would
     * corrupt scratch_ops mid-script. */
    if (wifi->scanning) return;
    if (wifi->is_ready && wifi->drv->set_channel) {
        wifi->drv->set_channel(wifi, channel, NULL);
    }
}

vsf_err_t vsf_wifi_set_mac(vsf_wifi_t *wifi, const uint8_t mac[6])
{
    if (!wifi->is_ready || !wifi->drv->set_mac_addr) return VSF_ERR_NOT_READY;
    if (wifi->scanning) return VSF_ERR_NOT_AVAILABLE;
    return wifi->drv->set_mac_addr(wifi, mac, NULL);
}

vsf_err_t vsf_wifi_set_bssid(vsf_wifi_t *wifi, const uint8_t bssid[6])
{
    if (!wifi->is_ready || !wifi->drv->set_bssid) return VSF_ERR_NOT_READY;
    if (wifi->scanning) return VSF_ERR_NOT_AVAILABLE;
    return wifi->drv->set_bssid(wifi, bssid, NULL);
}

vsf_err_t vsf_wifi_set_auth_mode(vsf_wifi_t *wifi,
        const vsf_wifi_auth_cfg_t *cfg)
{
    if (!wifi->is_ready || !wifi->drv->set_auth_mode) return VSF_ERR_NOT_READY;
    if (wifi->scanning) return VSF_ERR_NOT_AVAILABLE;
#if VSF_WIFI_USE_WPA == ENABLED
    /* Remember the requested security so the MLME can build the assoc-req RSN
     * IE and (for WPA2-PSK) run the 4-way handshake with the supplied PMK. */
    if (cfg != NULL) {
        wifi->wpa_auth = *cfg;
    } else {
        memset(&wifi->wpa_auth, 0, sizeof(wifi->wpa_auth));
    }
    wifi->wpa_rsn_ie_len = 0;
#endif
    return wifi->drv->set_auth_mode(wifi, cfg, NULL);
}

vsf_err_t vsf_wifi_scan(vsf_wifi_t *wifi,
        const uint8_t *channels, uint8_t num_channels, uint16_t dwell_ms)
{
    if (!wifi->is_ready) return VSF_ERR_NOT_READY;
#if VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER == ENABLED
    if (wifi->scanning)                     return VSF_ERR_NOT_AVAILABLE;
    if (NULL == wifi->drv->set_channel)     return VSF_ERR_NOT_SUPPORT;
    if (channels == NULL || num_channels == 0
            || num_channels > dimof(wifi->scan_channels))
        return VSF_ERR_INVALID_PARAMETER;

    memcpy(wifi->scan_channels, channels, num_channels);
    wifi->scan_num_channels = num_channels;
    wifi->scan_channel_idx  = 0;
    wifi->scan_dwell_ms     = dwell_ms ? dwell_ms
                                       : __VSF_WIFI_SCAN_DEFAULT_DWELL_MS;
    wifi->scanning          = true;
    wifi->channel           = channels[0];

    vsf_err_t err = wifi->drv->set_channel(wifi, channels[0],
            __vsf_wifi_scan_hop_done);
    if (VSF_ERR_NONE != err) {
        wifi->scanning = false;
    }
    return err;
#else
    (void)channels; (void)num_channels; (void)dwell_ms;
    return VSF_ERR_NOT_SUPPORT;
#endif
}

vsf_err_t vsf_wifi_scan_stop(vsf_wifi_t *wifi)
{
    if (!wifi->is_ready) return VSF_ERR_NOT_READY;
#if VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER == ENABLED
    if (!wifi->scanning) return VSF_ERR_NONE;
    vsf_callback_timer_remove(&wifi->scan_timer);
    __vsf_wifi_scan_finish(wifi);
    return VSF_ERR_NONE;
#else
    return VSF_ERR_NOT_SUPPORT;
#endif
}

vsf_err_t vsf_wifi_connect(vsf_wifi_t *wifi,
        const uint8_t bssid[6], const uint8_t *ssid, uint8_t ssid_len,
        uint8_t channel)
{
    if (!wifi->is_ready || !wifi->drv->connect) return VSF_ERR_NOT_READY;
    if (wifi->scanning) return VSF_ERR_NOT_AVAILABLE;
    if (wifi->mlme_state != WIFI_MLME_IDLE) return VSF_ERR_NOT_AVAILABLE;
    if ((bssid == NULL) || (channel == 0)) return VSF_ERR_INVALID_PARAMETER;

    /* Stash the target so the MLME state machine can build auth / assoc
     * frames and so a retransmit has the same parameters. */
    memcpy(wifi->mlme_bssid, bssid, 6);
    if (ssid_len > sizeof(wifi->mlme_ssid) - 1) {
        ssid_len = sizeof(wifi->mlme_ssid) - 1;
    }
    if ((ssid != NULL) && (ssid_len > 0)) {
        memcpy(wifi->mlme_ssid, ssid, ssid_len);
    }
    wifi->mlme_ssid_len = ssid_len;
    wifi->mlme_channel  = channel;
    wifi->mlme_aid      = 0;
    wifi->mlme_retry    = __VSF_WIFI_MLME_MAX_RETRY;
    wifi->mlme_state    = WIFI_MLME_AUTH;

    /* drv->connect locks channel + BSSID + RX filter; its completion kicks
     * off the OPEN-system handshake (auth-req) in the bus EDA. */
    vsf_err_t err = wifi->drv->connect(wifi, bssid, ssid, ssid_len, channel,
            __vsf_wifi_mlme_connect_done);
    if (VSF_ERR_NONE != err) {
        wifi->mlme_state = WIFI_MLME_IDLE;
    }
    return err;
}

vsf_err_t vsf_wifi_disconnect(vsf_wifi_t *wifi)
{
    if (!wifi->is_ready || !wifi->drv->disconnect) return VSF_ERR_NOT_READY;
    if (wifi->scanning) return VSF_ERR_NOT_AVAILABLE;
    if (wifi->mlme_state == WIFI_MLME_IDLE) return VSF_ERR_NONE;

    /* Best-effort deauth so the AP frees our association immediately. */
    __vsf_wifi_mlme_send_deauth(wifi, WIFI_REASON_AUTH_LEAVING);

    /* Tear down chip-side BSSID / RX filter; ignore its async completion. */
    vsf_err_t err = wifi->drv->disconnect(wifi, NULL);

    /* Drop the link locally regardless of the deauth/disconnect outcome. */
    __vsf_wifi_mlme_finish(wifi, WIFI_REASON_LOCAL_DISCONNECT);
    return err;
}

vsf_err_t vsf_wifi_get_link_info(vsf_wifi_t *wifi, vsf_wifi_link_info_t *info)
{
    if (!wifi->is_ready || !wifi->drv->get_link_info) return VSF_ERR_NOT_READY;
    return wifi->drv->get_link_info(wifi, info);
}

/* Staging buffer for the chip-encoded TX descriptor + frame.  A uint32_t
 * array guarantees the 4-byte alignment the RT2800 TXINFO/TXWI layout
 * expects.  Single-instance for now (the bus driver memcpy's it into its
 * own TX URB, so it is free to reuse immediately after data_tx returns). */
#ifndef VSF_WIFI_CFG_TX_BUF_SIZE
#   define VSF_WIFI_CFG_TX_BUF_SIZE     1600
#endif

/* Core TX path shared by vsf_wifi_tx (user API) and the active-scan probe-
 * request.  Deliberately does NOT check wifi->scanning so the scan scheduler
 * can transmit mid-scan; callers that must respect scanning check it first. */
static vsf_err_t __vsf_wifi_tx_frame(vsf_wifi_t *wifi,
        const uint8_t *frame, uint16_t len)
{
    static uint32_t __tx_buf32[(VSF_WIFI_CFG_TX_BUF_SIZE + 3) / 4];
    uint8_t *tx_buf = (uint8_t *)__tx_buf32;

    if (!wifi->is_ready)                return VSF_ERR_NOT_READY;
    if (NULL == wifi->drv->build_tx)    return VSF_ERR_NOT_SUPPORT;
    if (NULL == wifi->bus_ops->data_tx) return VSF_ERR_NOT_SUPPORT;
    if ((NULL == frame) || (0 == len))  return VSF_ERR_INVALID_PARAMETER;

    uint16_t total = wifi->drv->build_tx(wifi, tx_buf,
            (uint16_t)sizeof(__tx_buf32), frame, len);
    if (0 == total) return VSF_ERR_FAIL;

    return wifi->bus_ops->data_tx(wifi, tx_buf, total);
}

vsf_err_t vsf_wifi_tx(vsf_wifi_t *wifi, const uint8_t *frame, uint16_t len)
{
    if (wifi->scanning) return VSF_ERR_NOT_AVAILABLE;

#if VSF_WIFI_USE_WPA == ENABLED
    /* Software CCMP encap for plaintext unicast/multicast data frames once
     * the 4-way handshake installed keys.  Skipped when a hardware crypto
     * backend owns encryption (wpa_hw_crypto), and never applied to frames
     * that are already Protected or are not data type. */
    if (wifi->wpa_ptk_valid && !wifi->wpa_hw_crypto &&
            (len >= 2) && (((frame[0] >> 2) & 0x03) == 2) &&
            ((frame[1] & 0x40) == 0)) {
        static uint32_t __ccmp_tx32[(__VSF_WIFI_CCMP_BUF_SIZE + 3) / 4];
        uint8_t *enc = (uint8_t *)__ccmp_tx32;
        uint16_t total = __vsf_wifi_ccmp_encap(wifi, frame, len,
                enc, (uint16_t)sizeof(__ccmp_tx32));
        if (total == 0) return VSF_ERR_FAIL;
        return __vsf_wifi_tx_frame(wifi, enc, total);
    }
#endif

    return __vsf_wifi_tx_frame(wifi, frame, len);
}

#if VSF_WIFI_USE_WPA == ENABLED
/*============================ WPA HANDSHAKE GLUE ============================*
 * Thin wrappers letting vsf_wifi_wpa.c drive the connection without touching
 * the MLME internals directly (TX path, retry timer, state transitions).
 *==========================================================================*/

vsf_err_t vsf_wifi_mlme_tx(vsf_wifi_t *wifi, const uint8_t *frame, uint16_t len)
{
    return __vsf_wifi_tx_frame(wifi, frame, len);
}

void vsf_wifi_mlme_arm_timer(vsf_wifi_t *wifi, uint16_t ms)
{
#if VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER == ENABLED
    vsf_callback_timer_remove(&wifi->mlme_timer);
    vsf_callback_timer_add_ms(&wifi->mlme_timer, ms);
#else
    (void)wifi; (void)ms;
#endif
}

void vsf_wifi_mlme_handshake_done(vsf_wifi_t *wifi)
{
    if (wifi->mlme_state != WIFI_MLME_4WAY) return;
#if VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER == ENABLED
    vsf_callback_timer_remove(&wifi->mlme_timer);
#endif

    /* Install the negotiated keys.  A hardware crypto backend takes the TK /
     * GTK directly (wpa_hw_crypto = true, software CCMP bypassed); otherwise
     * fall back to the software CCMP path with a fresh TX PN counter. */
    if ((wifi->drv->crypto_ops != NULL) &&
            (wifi->drv->crypto_ops->install_key != NULL)) {
        wifi->wpa_hw_crypto = true;
        wifi->drv->crypto_ops->install_key(wifi, 0, true,
                wifi->wpa_ptk + 32, VSF_WIFI_TK_LEN, wifi->mlme_bssid);
        if (wifi->wpa_gtk_len > 0) {
            wifi->drv->crypto_ops->install_key(wifi, wifi->wpa_gtk_keyidx,
                    false, wifi->wpa_gtk, wifi->wpa_gtk_len, NULL);
        }
    } else {
        wifi->wpa_hw_crypto = false;
        memset(wifi->wpa_tx_pn, 0, sizeof(wifi->wpa_tx_pn));
    }

    wifi->mlme_state = WIFI_MLME_RUN;
    wifi->mlme_retry = 0;
    vsf_trace_info("wifi: 4-way handshake complete, aid=%u (link up)"
            VSF_TRACE_CFG_LINEEND, (unsigned)wifi->mlme_aid);
    {
        vsf_wifi_link_info_t info;
        memset(&info, 0, sizeof(info));
        memcpy(info.bssid, wifi->mlme_bssid, 6);
        info.channel = wifi->mlme_channel;
        info.flags   = WIFI_LINK_FLAG_CONNECTED | WIFI_LINK_FLAG_AUTHORIZED;
        vsf_wifi_on_link_up(wifi, &info);
    }
}

void vsf_wifi_mlme_handshake_fail(vsf_wifi_t *wifi, uint8_t reason)
{
    __vsf_wifi_mlme_finish(wifi, reason);
}
#endif

#endif      // VSF_USE_WIFI
