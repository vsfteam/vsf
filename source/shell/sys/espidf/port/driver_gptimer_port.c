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
 * Port implementation for "driver/gptimer.h" on VSF.
 *
 * Bridges the ESP-IDF general-purpose timer API onto
 *     hal/driver/common/template/vsf_template_timer.h
 *
 * The board/BSP is expected to define a small pool of vsf_timer_t*
 * instances -- typically a subset of the per-MCU vsf_hw_timerN symbols
 * -- and hand them in via vsf_espidf_cfg_t::gptimer. Each
 * gptimer_new_timer() call claims one pool entry and pins the on_alarm
 * callback to it; gptimer_del_timer() returns the entry to the pool.
 *
 * State machine (mirrors ESP-IDF v5.1 semantics):
 *
 *     [INIT] --register_event_callbacks/set_alarm_action--> [INIT]
 *     [INIT] --enable--> [ENABLE] --start--> [RUN]
 *     [RUN]  --stop--> [ENABLE] --disable--> [INIT]
 *     [INIT] --del--> [gone]
 *
 * Thread-safety: pool allocation is guarded by VSF's global scheduler
 * lock. Per-handle operations are not locked; the ESP-IDF contract
 * requires callers to serialise them themselves.
 */

/*============================ INCLUDES ======================================*/

#include "../vsf_espidf_cfg.h"

#if VSF_USE_ESPIDF == ENABLED && VSF_ESPIDF_CFG_USE_DRIVER_GPTIMER == ENABLED

#include "driver/gptimer.h"

#include "../vsf_espidf.h"
#include "kernel/vsf_kernel.h"
#include "hal/driver/driver.h"
#if defined(VSF_USE_HEAP) && VSF_USE_HEAP == ENABLED
#   include "service/heap/vsf_heap.h"
#else
#   error "VSF_ESPIDF_CFG_USE_DRIVER_GPTIMER requires VSF_USE_HEAP"
#endif

#include <string.h>

/*============================ TYPES =========================================*/

typedef enum {
    GPTIMER_FSM_INIT   = 0,         /*!< allocated, not yet enabled   */
    GPTIMER_FSM_ENABLE = 1,         /*!< enabled, counter not running */
    GPTIMER_FSM_RUN    = 2,         /*!< counter running              */
} gptimer_fsm_t;

struct gptimer_t {
    vsf_timer_t             *hw;            /*!< backing HAL instance (not owned) */
    uint16_t                 pool_idx;      /*!< index into the pool              */
    uint8_t                  fsm;
    uint8_t                  has_alarm : 1;
    uint8_t                  auto_reload : 1;
    uint8_t                  count_down : 1;/*!< GPTIMER_COUNT_DOWN               */
    uint32_t                 resolution_hz; /*!< requested / effective            */
    uint64_t                 alarm_count;   /*!< programmed via set_alarm_action  */
    uint64_t                 reload_count;  /*!< auto_reload reload value         */
    uint64_t                 alarm_fire_count; /*!< captured alarm event count    */

    gptimer_alarm_cb_t       on_alarm;
    void                    *user_ctx;
};

/*============================ LOCAL VARIABLES ===============================*/

static struct {
    bool                            is_inited;
    vsf_timer_t *const             *pool;
    uint16_t                        pool_count;
    uint32_t                        pool_busy;   /*!< 1 bit per entry, LSB = idx 0 */
} __vsf_espidf_gptimer = { 0 };

/*============================ PROTOTYPES ====================================*/

extern void vsf_espidf_gptimer_init(const vsf_espidf_gptimer_cfg_t *cfg);

/*============================ IMPLEMENTATION ================================*/

void vsf_espidf_gptimer_init(const vsf_espidf_gptimer_cfg_t *cfg)
{
    if (__vsf_espidf_gptimer.is_inited) {
        return;
    }
    if ((cfg != NULL) && (cfg->pool != NULL) && (cfg->pool_count > 0)) {
        __vsf_espidf_gptimer.pool       = cfg->pool;
        // Capped at 32 because the busy bitmap is a uint32_t. Anything
        // beyond the cap is treated as if it were not present.
        __vsf_espidf_gptimer.pool_count = (cfg->pool_count > 32U) ? 32U : cfg->pool_count;
    } else {
        __vsf_espidf_gptimer.pool       = NULL;
        __vsf_espidf_gptimer.pool_count = 0;
    }
    __vsf_espidf_gptimer.pool_busy = 0;
    __vsf_espidf_gptimer.is_inited = true;
}

static int __gptimer_pool_alloc(void)
{
    vsf_protect_t orig = vsf_protect_sched();
    for (uint16_t i = 0; i < __vsf_espidf_gptimer.pool_count; i++) {
        uint32_t bit = 1UL << i;
        if ((__vsf_espidf_gptimer.pool_busy & bit) == 0) {
            __vsf_espidf_gptimer.pool_busy |= bit;
            vsf_unprotect_sched(orig);
            return (int)i;
        }
    }
    vsf_unprotect_sched(orig);
    return -1;
}

static void __gptimer_pool_free(uint16_t idx)
{
    vsf_protect_t orig = vsf_protect_sched();
    __vsf_espidf_gptimer.pool_busy &= ~(1UL << idx);
    vsf_unprotect_sched(orig);
}

// Hardware-level ISR callback installed via vsf_timer_cfg_t::isr. Forwards
// to the user's on_alarm, discarding the bool return value that ESP-IDF
// uses to signal "yield to woken task".
static void __gptimer_isr_thunk(void *target_ptr,
                                 vsf_timer_t *timer_ptr,
                                 vsf_timer_irq_mask_t irq_mask)
{
    gptimer_handle_t h = (gptimer_handle_t)target_ptr;

    (void)timer_ptr;
    (void)irq_mask;

    if ((h == NULL) || (h->on_alarm == NULL)) {
        return;
    }

    h->alarm_fire_count++;

    uint32_t cnt = vsf_timer_get_counter(h->hw);
    gptimer_alarm_event_data_t edata = {
        .count_value = cnt,
        .alarm_value = h->alarm_count,
    };
    (void)h->on_alarm(h, &edata, h->user_ctx);

    if (h->auto_reload && (h->reload_count != 0)) {
#ifdef VSF_TIMER_SET_COUNTER_IMMEDIATE
        vsf_timer_set_counter(h->hw, (uint32_t)h->reload_count,
                              VSF_TIMER_SET_COUNTER_IMMEDIATE);
#endif
    }
}

// Program the underlying vsf_timer and (re-)arm IRQ reporting.
// The caller must pass h->fsm != RUN; the function does not enforce that.
static esp_err_t __gptimer_arm(gptimer_handle_t h)
{
    if (!h->has_alarm) {
        // No alarm configured yet. Nothing to do; enable() without a
        // configured alarm is a legal idle state in ESP-IDF.
        return ESP_OK;
    }
    vsf_timer_cfg_t tcfg = {
        .period  = (uint32_t)((h->alarm_count > 0xFFFFFFFFULL) ? 0xFFFFFFFFUL
                                                                : (uint32_t)h->alarm_count),
        .freq    = h->resolution_hz,
        .isr     = {
            .handler_fn = __gptimer_isr_thunk,
            .target_ptr = h,
            .prio       = vsf_arch_prio_0,
        },
    };
    if (VSF_ERR_NONE != vsf_timer_init(h->hw, &tcfg)) {
        return ESP_FAIL;
    }
    vsf_timer_irq_enable(h->hw, VSF_TIMER_IRQ_MASK_OVERFLOW);
    return ESP_OK;
}

/*---------------------------- public API -----------------------------------*/

esp_err_t gptimer_new_timer(const gptimer_config_t *config,
                             gptimer_handle_t *ret_timer)
{
    if ((config == NULL) || (ret_timer == NULL)) {
        return ESP_ERR_INVALID_ARG;
    }
    if (config->resolution_hz == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!__vsf_espidf_gptimer.is_inited
        || (__vsf_espidf_gptimer.pool_count == 0)) {
        return ESP_ERR_INVALID_STATE;
    }

    int idx = __gptimer_pool_alloc();
    if (idx < 0) {
        return ESP_ERR_NOT_FOUND;
    }

    gptimer_handle_t h = (gptimer_handle_t)vsf_heap_malloc(sizeof(struct gptimer_t));
    if (h == NULL) {
        __gptimer_pool_free((uint16_t)idx);
        return ESP_ERR_NO_MEM;
    }
    memset(h, 0, sizeof(*h));
    h->hw            = __vsf_espidf_gptimer.pool[idx];
    h->pool_idx      = (uint16_t)idx;
    h->fsm           = GPTIMER_FSM_INIT;
    h->resolution_hz = config->resolution_hz;
    h->count_down    = (uint8_t)(config->direction == GPTIMER_COUNT_DOWN);

    *ret_timer = h;
    return ESP_OK;
}

esp_err_t gptimer_del_timer(gptimer_handle_t timer)
{
    if (timer == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (timer->fsm != GPTIMER_FSM_INIT) {
        // ESP-IDF requires the timer to be disabled before deletion.
        return ESP_ERR_INVALID_STATE;
    }
    if (timer->has_alarm) {
        vsf_timer_fini(timer->hw);
    }
    __gptimer_pool_free(timer->pool_idx);
    vsf_heap_free(timer);
    return ESP_OK;
}

esp_err_t gptimer_register_event_callbacks(gptimer_handle_t timer,
                                            const gptimer_event_callbacks_t *cbs,
                                            void *user_data)
{
    if ((timer == NULL) || (cbs == NULL)) {
        return ESP_ERR_INVALID_ARG;
    }
    if (timer->fsm != GPTIMER_FSM_INIT) {
        return ESP_ERR_INVALID_STATE;
    }
    timer->on_alarm = cbs->on_alarm;
    timer->user_ctx = user_data;
    return ESP_OK;
}

esp_err_t gptimer_set_alarm_action(gptimer_handle_t timer,
                                    const gptimer_alarm_config_t *config)
{
    if (timer == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (config == NULL) {
        // Disable future alarms. Matches ESP-IDF contract: counter keeps
        // running but no IRQ is generated.
        if (timer->has_alarm && (timer->fsm != GPTIMER_FSM_INIT)) {
            vsf_timer_irq_disable(timer->hw, VSF_TIMER_IRQ_MASK_OVERFLOW);
        }
        timer->has_alarm     = false;
        timer->auto_reload   = false;
        timer->alarm_count   = 0;
        timer->reload_count  = 0;
        return ESP_OK;
    }

    if (config->alarm_count == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    timer->alarm_count  = config->alarm_count;
    timer->reload_count = config->reload_count;
    timer->auto_reload  = (uint8_t)config->flags.auto_reload_on_alarm;
    timer->has_alarm    = 1;

    // If the timer is already running, apply the new period on the fly.
    // Otherwise the next enable()/start() will pick it up.
    if (timer->fsm == GPTIMER_FSM_RUN) {
        (void)vsf_timer_set_period(timer->hw, (uint32_t)timer->alarm_count);
    }
    return ESP_OK;
}

esp_err_t gptimer_enable(gptimer_handle_t timer)
{
    if (timer == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (timer->fsm != GPTIMER_FSM_INIT) {
        return ESP_ERR_INVALID_STATE;
    }
    esp_err_t err = __gptimer_arm(timer);
    if (err != ESP_OK) {
        return err;
    }
    timer->fsm = GPTIMER_FSM_ENABLE;
    return ESP_OK;
}

esp_err_t gptimer_disable(gptimer_handle_t timer)
{
    if (timer == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (timer->fsm != GPTIMER_FSM_ENABLE) {
        return ESP_ERR_INVALID_STATE;
    }
    if (timer->has_alarm) {
        vsf_timer_irq_disable(timer->hw, VSF_TIMER_IRQ_MASK_OVERFLOW);
        vsf_timer_fini(timer->hw);
    }
    timer->fsm = GPTIMER_FSM_INIT;
    return ESP_OK;
}

esp_err_t gptimer_start(gptimer_handle_t timer)
{
    if (timer == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (timer->fsm != GPTIMER_FSM_ENABLE) {
        return ESP_ERR_INVALID_STATE;
    }
    if (!timer->has_alarm) {
        // Nothing to count against; ESP-IDF itself is permissive here.
        timer->fsm = GPTIMER_FSM_RUN;
        return ESP_OK;
    }
    // vsf_timer_enable is a state-machine call that may need polling on
    // some back-ends. For hardware timers the typical contract is that
    // a single call returns fsm_rt_cpl; loop defensively.
    fsm_rt_t rt;
    uint32_t guard = 0;
    do {
        rt = vsf_timer_enable(timer->hw);
        if (rt < 0) {
            return ESP_FAIL;
        }
        if (++guard > 1000U) {
            return ESP_ERR_TIMEOUT;
        }
    } while (rt != fsm_rt_cpl);

    timer->fsm = GPTIMER_FSM_RUN;
    return ESP_OK;
}

esp_err_t gptimer_stop(gptimer_handle_t timer)
{
    if (timer == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (timer->fsm != GPTIMER_FSM_RUN) {
        return ESP_ERR_INVALID_STATE;
    }
    if (timer->has_alarm) {
        fsm_rt_t rt;
        uint32_t guard = 0;
        do {
            rt = vsf_timer_disable(timer->hw);
            if (rt < 0) {
                return ESP_FAIL;
            }
            if (++guard > 1000U) {
                return ESP_ERR_TIMEOUT;
            }
        } while (rt != fsm_rt_cpl);
    }
    timer->fsm = GPTIMER_FSM_ENABLE;
    return ESP_OK;
}

esp_err_t gptimer_set_raw_count(gptimer_handle_t timer, uint64_t value)
{
    if (timer == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
#ifdef VSF_TIMER_SET_COUNTER_IMMEDIATE
    vsf_err_t err = vsf_timer_set_counter(timer->hw, (uint32_t)value,
                                          VSF_TIMER_SET_COUNTER_IMMEDIATE);
    return (err == VSF_ERR_NONE) ? ESP_OK : ESP_ERR_NOT_SUPPORTED;
#else
    (void)value;
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

esp_err_t gptimer_get_raw_count(gptimer_handle_t timer, uint64_t *value)
{
    if ((timer == NULL) || (value == NULL)) {
        return ESP_ERR_INVALID_ARG;
    }
    *value = (uint64_t)vsf_timer_get_counter(timer->hw);
    return ESP_OK;
}

esp_err_t gptimer_get_resolution(gptimer_handle_t timer, uint32_t *out_resolution)
{
    if ((timer == NULL) || (out_resolution == NULL)) {
        return ESP_ERR_INVALID_ARG;
    }
    *out_resolution = timer->resolution_hz;
    return ESP_OK;
}

esp_err_t gptimer_get_captured_count(gptimer_handle_t timer, uint64_t *value)
{
    if ((timer == NULL) || (value == NULL)) {
        return ESP_ERR_INVALID_ARG;
    }
    *value = timer->alarm_fire_count;
    return ESP_OK;
}

#endif      // VSF_USE_ESPIDF && VSF_ESPIDF_CFG_USE_DRIVER_GPTIMER
