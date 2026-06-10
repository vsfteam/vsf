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
static void __vsf_wifi_on_rxfilter_done (vsf_wifi_t *wifi, vsf_err_t err);

#if VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER == ENABLED
static void __vsf_wifi_scan_timer_cb (vsf_callback_timer_t *timer);
static void __vsf_wifi_scan_hop_done (vsf_wifi_t *wifi, vsf_err_t err);
static void __vsf_wifi_scan_advance  (vsf_wifi_t *wifi);
static void __vsf_wifi_scan_finish   (vsf_wifi_t *wifi);

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
     * beacons / probe responses; otherwise hand it to the application. */
    if (wifi->scanning && wifi->drv->parse_rx != NULL) {
        wifi->drv->parse_rx(wifi, frame, len);
    } else {
        vsf_wifi_on_rx(wifi, frame, len);
    }
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
    /* Channel locked; arm dwell timer.  Beacons / probe responses arriving
     * during this window are routed to drv->parse_rx via the RX path. */
    vsf_callback_timer_add_ms(&wifi->scan_timer, wifi->scan_dwell_ms);
}

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
    return wifi->drv->connect(wifi, bssid, ssid, ssid_len, channel, NULL);
}

vsf_err_t vsf_wifi_disconnect(vsf_wifi_t *wifi)
{
    if (!wifi->is_ready || !wifi->drv->disconnect) return VSF_ERR_NOT_READY;
    if (wifi->scanning) return VSF_ERR_NOT_AVAILABLE;
    return wifi->drv->disconnect(wifi, NULL);
}

vsf_err_t vsf_wifi_get_link_info(vsf_wifi_t *wifi, vsf_wifi_link_info_t *info)
{
    if (!wifi->is_ready || !wifi->drv->get_link_info) return VSF_ERR_NOT_READY;
    return wifi->drv->get_link_info(wifi, info);
}

#endif      // VSF_USE_WIFI
