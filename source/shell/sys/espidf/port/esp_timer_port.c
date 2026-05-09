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
 * Port implementation for "esp_timer.h" on VSF.
 *
 * Each esp_timer_handle_t wraps a vsf_callback_timer_t plus the ESP-IDF
 * metadata (callback/arg/period). For periodic timers, the on_timer hook
 * re-arms the underlying vsf timer with the saved period before invoking
 * the user callback. Memory is allocated from the VSF heap.
 *
 * The ESP-IDF contract permits calling esp_timer_stop() from within the
 * callback. VSF's vsf_callback_timer_remove() is safe to call on a timer
 * not currently queued, so the same semantics are inherited here.
 */

/*============================ INCLUDES ======================================*/

#include "../vsf_espidf_cfg.h"

#if VSF_USE_ESPIDF == ENABLED && VSF_ESPIDF_CFG_USE_TIMER == ENABLED

#include "esp_timer.h"

#include "../vsf_espidf.h"
#include "kernel/vsf_kernel.h"
#if defined(VSF_USE_HEAP) && VSF_USE_HEAP == ENABLED
#   include "service/heap/vsf_heap.h"
#else
#   error "VSF_ESPIDF_CFG_USE_TIMER requires VSF_USE_HEAP"
#endif

#include <string.h>
#include <stdio.h>
#include <inttypes.h>

/*============================ TYPES =========================================*/

struct esp_timer {
    vsf_callback_timer_t    timer;      /*!< MUST be first member             */
    esp_timer_cb_t          callback;
    void *                  arg;
    uint64_t                period_us;  /*!< 0 = one-shot                      */
    const char *            name;
    bool                    is_active;
};

/*============================ IMPLEMENTATION ================================*/

static void __vsf_espidf_timer_on_timer(vsf_callback_timer_t *timer)
{
    esp_timer_handle_t h = (esp_timer_handle_t)timer;

    // Re-arm for periodic before invoking user callback, matching ESP-IDF
    // semantics where the period is measured between successive expirations
    // regardless of callback runtime.
    if (h->period_us != 0) {
        vsf_callback_timer_add_us(&h->timer, (uint_fast32_t)h->period_us);
    } else {
        h->is_active = false;
    }

    if (h->callback != NULL) {
        h->callback(h->arg);
    }
}

esp_err_t esp_timer_init(void)
{
    // VSF systimer/callback-timer is brought up by vsf_start(); nothing to do.
    return ESP_OK;
}

esp_err_t esp_timer_deinit(void)
{
    return ESP_OK;
}

int64_t esp_timer_get_time(void)
{
    return (int64_t)vsf_systimer_tick_to_us(vsf_systimer_get_tick());
}

int64_t esp_timer_get_next_alarm(void)
{
    // VSF does not publish a cheap soonest-due query. Return INT64_MAX to
    // signal "unknown / none"; callers treat it as no pending timer.
    return INT64_MAX;
}

esp_err_t esp_timer_create(const esp_timer_create_args_t *create_args,
                            esp_timer_handle_t *out_handle)
{
    if ((create_args == NULL) || (create_args->callback == NULL)
        || (out_handle == NULL)) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_timer_handle_t h = (esp_timer_handle_t)vsf_heap_malloc(sizeof(struct esp_timer));
    if (h == NULL) {
        return ESP_ERR_NO_MEM;
    }
    memset(h, 0, sizeof(*h));
    vsf_callback_timer_init(&h->timer);
    h->timer.on_timer   = __vsf_espidf_timer_on_timer;
    h->callback         = create_args->callback;
    h->arg              = create_args->arg;
    h->name             = create_args->name;
    h->period_us        = 0;
    h->is_active        = false;

    *out_handle = h;
    return ESP_OK;
}

esp_err_t esp_timer_start_once(esp_timer_handle_t timer, uint64_t timeout_us)
{
    if (timer == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (timer->is_active) {
        return ESP_ERR_INVALID_STATE;
    }
    timer->period_us = 0;
    timer->is_active = true;
    if (VSF_ERR_NONE != vsf_callback_timer_add_us(&timer->timer,
                                                    (uint_fast32_t)timeout_us)) {
        timer->is_active = false;
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t esp_timer_start_periodic(esp_timer_handle_t timer, uint64_t period_us)
{
    if ((timer == NULL) || (period_us == 0)) {
        return ESP_ERR_INVALID_ARG;
    }
    if (timer->is_active) {
        return ESP_ERR_INVALID_STATE;
    }
    timer->period_us = period_us;
    timer->is_active = true;
    if (VSF_ERR_NONE != vsf_callback_timer_add_us(&timer->timer,
                                                    (uint_fast32_t)period_us)) {
        timer->is_active = false;
        timer->period_us = 0;
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t esp_timer_stop(esp_timer_handle_t timer)
{
    if (timer == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!timer->is_active) {
        // ESP-IDF returns ESP_ERR_INVALID_STATE on stop of inactive timer.
        return ESP_ERR_INVALID_STATE;
    }
    vsf_callback_timer_remove(&timer->timer);
    timer->is_active = false;
    timer->period_us = 0;
    return ESP_OK;
}

esp_err_t esp_timer_delete(esp_timer_handle_t timer)
{
    if (timer == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (timer->is_active) {
        vsf_callback_timer_remove(&timer->timer);
        timer->is_active = false;
    }
    vsf_heap_free(timer);
    return ESP_OK;
}

bool esp_timer_is_active(esp_timer_handle_t timer)
{
    return (timer != NULL) && timer->is_active;
}

static esp_err_t __esp_timer_start_at(esp_timer_handle_t timer,
                                       uint64_t period_us,
                                       uint64_t start_at_us)
{
    int64_t now_us = esp_timer_get_time();
    int64_t delta_us = (int64_t)(start_at_us - (uint64_t)now_us);
    if (delta_us < 0) {
        delta_us = 0;
    }
    timer->period_us = period_us;
    timer->is_active = true;
    if (VSF_ERR_NONE != vsf_callback_timer_add_us(&timer->timer,
                                                    (uint_fast32_t)delta_us)) {
        timer->is_active = false;
        timer->period_us = 0;
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t esp_timer_start_periodic_at(esp_timer_handle_t timer,
                                       uint64_t period_us,
                                       uint64_t start_at_us)
{
    if ((timer == NULL) || (period_us == 0)) {
        return ESP_ERR_INVALID_ARG;
    }
    if (timer->is_active) {
        return ESP_ERR_INVALID_STATE;
    }
    return __esp_timer_start_at(timer, period_us, start_at_us);
}

esp_err_t esp_timer_start_once_at(esp_timer_handle_t timer,
                                   uint64_t timeout_at_us)
{
    if (timer == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (timer->is_active) {
        return ESP_ERR_INVALID_STATE;
    }
    return __esp_timer_start_at(timer, 0, timeout_at_us);
}

esp_err_t esp_timer_restart_at(esp_timer_handle_t timer,
                                uint64_t new_period_us,
                                uint64_t start_at_us)
{
    if ((timer == NULL) || (new_period_us == 0)) {
        return ESP_ERR_INVALID_ARG;
    }
    if (timer->is_active) {
        vsf_callback_timer_remove(&timer->timer);
        timer->is_active = false;
    }
    if (timer->callback == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    vsf_callback_timer_init(&timer->timer);
    timer->timer.on_timer = __vsf_espidf_timer_on_timer;
    return __esp_timer_start_at(timer, new_period_us, start_at_us);
}

esp_err_t esp_timer_dump(FILE *stream)
{
    if (stream == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    fprintf(stream, "esp_timer_dump: not implemented on VSF\n");
    return ESP_OK;
}

#endif      // VSF_USE_ESPIDF && VSF_ESPIDF_CFG_USE_TIMER
