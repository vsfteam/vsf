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

#ifndef __VSF_WIFI_RADIO_H__
#define __VSF_WIFI_RADIO_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_wifi_cfg.h"

#if VSF_USE_WIFI == ENABLED

#include "kernel/vsf_kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

/*============================ TYPES =========================================*/

/* Forward declarations: full definitions live in vsf_wifi_priv.h. */
typedef struct vsf_wifi_t       vsf_wifi_t;
typedef struct vsf_wifi_radio_t vsf_wifi_radio_t;

/* RX callback invoked for every 802.11 frame received in raw-radio mode. */
typedef void (*vsf_wifi_radio_rx_cb_t)(vsf_wifi_radio_t *radio, void *param,
                                       uint8_t *frame, uint16_t len);

/* Completion callback for async raw-radio operations. */
typedef void (*vsf_wifi_radio_done_t)(vsf_wifi_radio_t *radio, vsf_err_t err);

/*
 * Raw WiFi radio operations.
 *
 * These are low-level, 802.11-frame-oriented hooks that a WiFi chip driver can
 * expose when the standard station-mode state machine (MLME/WPA/DHCP) is not
 * needed.  They are intentionally minimal: init/fini, TX, channel/filter
 * control, and a power-save hook.
 *
 * All operations are asynchronous and MUST NOT block.  The caller is the
 * same EDA/interrupt context used by the rest of the WiFi subsystem.
 */
typedef struct vsf_wifi_radio_ops_t {
    /*
     * Prepare the radio for raw operation.  Called after the chip has already
     * been attached and firmware-loaded by the normal WiFi bring-up chain.
     * The implementation should enable RX with a sensible default filter and
     * leave the radio in a state where subsequent TX/RX/channel calls work.
     */
    vsf_err_t (*init)        (vsf_wifi_radio_t *radio,
                              vsf_wifi_radio_done_t done);

    /*
     * Shut down raw operation.  Typically disables RX / clears the filter.
     * After fini returns the instance can either be re-initialized as raw
     * radio or returned to standard WiFi mode.
     */
    vsf_err_t (*fini)        (vsf_wifi_radio_t *radio,
                              vsf_wifi_radio_done_t done);

    /*
     * Transmit a naked 802.11 frame.  The chip driver wraps it with its
     * on-wire descriptor (e.g. RT2800 TXINFO + TXWI) and ships it over the
     * bus.  Fire-and-forget: there is no per-frame TX completion callback.
     */
    vsf_err_t (*tx)          (vsf_wifi_radio_t *radio,
                              const uint8_t *frame, uint16_t len);

    /* Set the RF channel.  Completion via `done`. */
    vsf_err_t (*set_channel) (vsf_wifi_radio_t *radio, uint8_t ch,
                              vsf_wifi_radio_done_t done);

    /* Set the RX filter (chip-specific mask).  Completion via `done`. */
    vsf_err_t (*set_filter)  (vsf_wifi_radio_t *radio, uint32_t mask,
                              vsf_wifi_radio_done_t done);

    /*
     * Enter/leave chip power-save state.  This is a placeholder in the first
     * revision; implementations may return VSF_ERR_NONE until real RF
     * sleep/wakeup scripts are added.
     */
    vsf_err_t (*set_ps)      (vsf_wifi_radio_t *radio, bool sleep,
                              vsf_wifi_radio_done_t done);
} vsf_wifi_radio_ops_t;

/*============================ GLOBAL VARIABLES ==============================*/

/*============================ PROTOTYPES ====================================*/

/*
 * Obtain the raw-radio handle embedded in a vsf_wifi_t instance.
 * Returns NULL if the chip driver does not expose wifi_radio_ops.
 */
extern vsf_wifi_radio_t * vsf_wifi_radio_from_wifi(vsf_wifi_t *wifi);

/*
 * Register the RX callback that will receive 802.11 frames while raw radio
 * mode is active.  May be called before or after vsf_wifi_radio_init().
 */
extern void vsf_wifi_radio_register_rx(vsf_wifi_radio_t *radio,
                                       vsf_wifi_radio_rx_cb_t cb,
                                       void *param);

/*
 * Start raw-radio mode.  Mutually exclusive with standard WiFi connect/scan:
 * returns VSF_ERR_NOT_AVAILABLE if the WiFi layer is currently busy.
 * Completion via `done`.
 */
extern vsf_err_t vsf_wifi_radio_init        (vsf_wifi_radio_t *radio,
                                             vsf_wifi_radio_done_t done);
extern vsf_err_t vsf_wifi_radio_fini        (vsf_wifi_radio_t *radio,
                                             vsf_wifi_radio_done_t done);
extern vsf_err_t vsf_wifi_radio_tx          (vsf_wifi_radio_t *radio,
                                             const uint8_t *frame,
                                             uint16_t len);
extern vsf_err_t vsf_wifi_radio_set_channel (vsf_wifi_radio_t *radio,
                                             uint8_t ch,
                                             vsf_wifi_radio_done_t done);
extern vsf_err_t vsf_wifi_radio_set_filter  (vsf_wifi_radio_t *radio,
                                             uint32_t mask,
                                             vsf_wifi_radio_done_t done);
extern vsf_err_t vsf_wifi_radio_set_ps      (vsf_wifi_radio_t *radio,
                                             bool sleep,
                                             vsf_wifi_radio_done_t done);

/*
 * Chip drivers call this to deliver a received 802.11 frame to the registered
 * raw-radio RX callback.  Safe to call even when no callback is registered.
 */
extern void vsf_wifi_radio_on_rx(vsf_wifi_t *wifi, uint8_t *frame,
                                 uint16_t len);

/*
 * Internal adapter for chip drivers that delegate async work to the existing
 * vsf_wifi_* script helpers (which take a vsf_wifi_done_t callback).  The chip
 * driver stashes the user `done` with vsf_wifi_radio_adapter_done_set() and
 * passes vsf_wifi_radio_adapter_done() as the vsf_wifi_done_t callback.
 */
extern void vsf_wifi_radio_adapter_done_set(vsf_wifi_radio_t *radio,
                                            vsf_wifi_radio_done_t done);
extern void vsf_wifi_radio_adapter_done    (vsf_wifi_t *wifi,
                                            vsf_err_t err);

#ifdef __cplusplus
}
#endif

#endif      /* VSF_USE_WIFI */
#endif      /* __VSF_WIFI_RADIO_H__ */
