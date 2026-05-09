/*****************************************************************************
 *   Copyright(C)2009-2026 by VSF Team                                       *
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

/*
 * Clean-room re-implementation of ESP-IDF public API "esp_timer.h".
 *
 * Authored from ESP-IDF v5.x public API only. No code copied from the
 * ESP-IDF source tree. The VSF port bridges to vsf_callback_timer.
 */

#ifndef __VSF_ESPIDF_ESP_TIMER_H__
#define __VSF_ESPIDF_ESP_TIMER_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ TYPES =========================================*/

struct esp_timer;
typedef struct esp_timer * esp_timer_handle_t;

typedef void (*esp_timer_cb_t)(void *arg);

typedef enum {
    ESP_TIMER_TASK  = 0,    /*!< Callback runs on the dedicated timer task.   */
    /* ISR dispatch is not modeled on VSF host; falls back to TASK. */
    ESP_TIMER_ISR   = 1,
} esp_timer_dispatch_t;

typedef struct {
    esp_timer_cb_t          callback;
    void *                  arg;
    esp_timer_dispatch_t    dispatch_method;
    const char *            name;
    bool                    skip_unhandled_events;
} esp_timer_create_args_t;

/*============================ PROTOTYPES ====================================*/

/* Lifecycle. On VSF both are no-ops because the timer backbone comes up as
 * part of vsf_start(); kept for API compatibility. */
esp_err_t esp_timer_init(void);
esp_err_t esp_timer_deinit(void);

/* Microseconds elapsed since the systimer started. Equivalent to
 * vsf_systimer_tick_to_us(vsf_systimer_get_tick()). */
int64_t esp_timer_get_time(void);

/* Time remaining until the next scheduled alarm, in microseconds. Returns
 * INT64_MAX if no timer is armed. VSF does not expose a soonest-due query
 * cheaply; this is a best-effort scan over the active timer list. */
int64_t esp_timer_get_next_alarm(void);

/* Create a one-shot or periodic timer. Stored in VSF heap; caller must
 * release via esp_timer_delete(). */
esp_err_t esp_timer_create(const esp_timer_create_args_t *create_args,
                            esp_timer_handle_t *out_handle);

/* Arm the timer for a single shot after timeout_us microseconds. */
esp_err_t esp_timer_start_once(esp_timer_handle_t timer, uint64_t timeout_us);

/* Arm the timer to fire every period_us microseconds. */
esp_err_t esp_timer_start_periodic(esp_timer_handle_t timer, uint64_t period_us);

/* Cancel a running timer. Safe to call on an unstarted timer. */
esp_err_t esp_timer_stop(esp_timer_handle_t timer);

/* Stop (if running) then destroy the timer. */
esp_err_t esp_timer_delete(esp_timer_handle_t timer);

/* Query whether the timer is currently armed. */
bool esp_timer_is_active(esp_timer_handle_t timer);

/* Arm the periodic timer to fire every period_us starting at absolute
 * time start_at_us (microseconds since boot). */
esp_err_t esp_timer_start_periodic_at(esp_timer_handle_t timer, uint64_t period_us, uint64_t start_at_us);

/* Arm the one-shot timer to fire at absolute time timeout_at_us. */
esp_err_t esp_timer_start_once_at(esp_timer_handle_t timer, uint64_t timeout_at_us);

/* Stop the periodic timer (if running), update its period, and re-arm
 * to fire at absolute time start_at_us. */
esp_err_t esp_timer_restart_at(esp_timer_handle_t timer, uint64_t new_period_us, uint64_t start_at_us);

/* Dump all active timers to the given FILE stream (mainly for debugging). */
esp_err_t esp_timer_dump(FILE *stream);

#ifdef __cplusplus
}
#endif

#endif      // __VSF_ESPIDF_ESP_TIMER_H__
