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

#include "vsf.h"

#if APP_USE_HAL_DEMO == ENABLED && APP_USE_HAL_WDT_DEMO == ENABLED && VSF_HAL_USE_WDT == ENABLED

#if APP_USE_LINUX_DEMO == ENABLED
#   include <string.h>
#endif
#include <inttypes.h>

/*============================ MACROS ========================================*/

#ifndef APP_WDT_DEMO_CFG_DEFAULT_INSTANCE
#   define APP_WDT_DEMO_CFG_DEFAULT_INSTANCE        vsf_hw_wwdt0
#endif

#ifndef APP_WDT_DEMO_CFG_IRQ_PRIO
#   define APP_WDT_DEMO_CFG_IRQ_PRIO                vsf_arch_prio_2
#endif

#define WDT_TEST_TIMEOUT_MS_MAX                     2000
#define WDT_TEST_TIMEOUT_RATIO                      80
#define WDT_TEST_WINDOW_RATIO                       40
#define WDT_TEST_FEED_INTERVAL_RATIO                50
#define WDT_TEST_SAFETY_MARGIN_RATIO                10
#define WDT_TEST_INTERRUPT_TIMEOUT_RATIO            150
#define WDT_TEST_FEED_DURATION_MULTIPLIER           3
#define WDT_TEST_FEED_DURATION_MIN_MS               100
#define WDT_TEST_MIN_TIME_MS                        1
#define WDT_TEST_POLL_INTERVAL_MS                   10

/*============================ MACROFIED FUNCTIONS ===========================*/

// Helper macro for trace with timestamp
// Note: Use PRIu64 for vsf_systimer_tick_t to avoid format mismatch
#define __WDT_TRACE_INFO(fmt, ...)                  do { \
                                                        uint64_t __wdt_time_ms = (uint64_t)vsf_systimer_get_ms(); \
                                                        vsf_trace_info("[%" PRIu64 " ms] " fmt, __wdt_time_ms, ##__VA_ARGS__); \
                                                    } while(0)
#define __WDT_TRACE_ERROR(fmt, ...)                 do { \
                                                        uint64_t __wdt_time_ms = (uint64_t)vsf_systimer_get_ms(); \
                                                        vsf_trace_error("[%" PRIu64 " ms] " fmt, __wdt_time_ms, ##__VA_ARGS__); \
                                                    } while(0)
#define __WDT_TRACE_WARNING(fmt, ...)               do { \
                                                        uint64_t __wdt_time_ms = (uint64_t)vsf_systimer_get_ms(); \
                                                        vsf_trace_warning("[%" PRIu64 " ms] " fmt, __wdt_time_ms, ##__VA_ARGS__); \
                                                    } while(0)
/*============================ TYPES =========================================*/

typedef struct wdt_test_t {
    vsf_wdt_t *wdt;
    vsf_wdt_capability_t cap;
    vsf_wdt_cfg_t cfg;
    volatile bool irq_triggered;
    volatile uint32_t irq_count;
    bool reset_expected;
} wdt_test_t;

/*============================ PROTOTYPES ====================================*/

static void __wdt_irq_handler(void *target_ptr, vsf_wdt_t *wdt_ptr);
static bool __wdt_setup(wdt_test_t *ctx, vsf_wdt_mode_t mode, bool use_irq, bool reset_expected, const char *mode_str, bool log_cap);
static void __wdt_cleanup(wdt_test_t *ctx);
static bool __wdt_supports_window(const wdt_test_t *ctx);
static uint32_t __wdt_apply_ratio(uint32_t base, uint32_t ratio_percent);
static uint32_t __wdt_clamp(uint32_t value, uint32_t min, uint32_t max);
static uint32_t __wdt_calc_feed_interval(const wdt_test_t *ctx);
static bool __wdt_pick_reset_mode(const vsf_wdt_capability_t *capability, vsf_wdt_mode_t *mode);
static bool __wdt_test_interrupt(wdt_test_t *ctx);
static bool __wdt_test_reset(wdt_test_t *ctx);
static bool __wdt_test_feed(wdt_test_t *ctx);

/*============================ IMPLEMENTATION ================================*/

static uint32_t __wdt_apply_ratio(uint32_t base, uint32_t ratio_percent)
{
    return (base * ratio_percent) / 100;
}

static uint32_t __wdt_clamp(uint32_t value, uint32_t min, uint32_t max)
{
    if (value < min) {
        value = min;
    }
    if (value > max) {
        value = max;
    }
    return value;
}

static bool __wdt_supports_window(const wdt_test_t *ctx)
{
    return ctx->cap.support_min_timeout && (ctx->cfg.min_ms > 0);
}

static void __wdt_print_capability(const vsf_wdt_capability_t *cap)
{
    __WDT_TRACE_INFO("WDT Capability: early_wakeup=%d, reset_none=%d, reset_cpu=%d, reset_soc=%d, disable=%d, min_timeout=%d" VSF_TRACE_CFG_LINEEND,
                    cap->support_early_wakeup,
                    cap->support_reset_none,
                    cap->support_reset_cpu,
                    cap->support_reset_soc,
                    cap->support_disable,
                    cap->support_min_timeout);
}

static void __wdt_print_config(const wdt_test_t *ctx, const char *mode_str)
{
    if (__wdt_supports_window(ctx)) {
        __WDT_TRACE_INFO("WDT initialized with %s mode, timeout: max=%d ms, min=%d ms (window)" VSF_TRACE_CFG_LINEEND,
                        mode_str, ctx->cfg.max_ms, ctx->cfg.min_ms);
    } else {
        __WDT_TRACE_INFO("WDT initialized with %s mode, timeout: %d ms" VSF_TRACE_CFG_LINEEND,
                        mode_str, ctx->cfg.max_ms);
    }
}

static void __wdt_irq_handler(void *target_ptr, vsf_wdt_t *wdt_ptr)
{
    wdt_test_t *ctx = (wdt_test_t *)target_ptr;

    if (ctx != NULL) {
        ctx->irq_triggered = true;
        ctx->irq_count++;
        if (!ctx->reset_expected) {
            vsf_wdt_feed(wdt_ptr);
        }
    }
}

static bool __wdt_pick_reset_mode(const vsf_wdt_capability_t *capability, vsf_wdt_mode_t *mode)
{
    if (capability->support_reset_cpu) {
        *mode = VSF_WDT_MODE_RESET_CPU;
        return true;
    }
    if (capability->support_reset_soc) {
        *mode = VSF_WDT_MODE_RESET_SOC;
        return true;
    }
    return false;
}

static bool __wdt_setup(wdt_test_t *ctx, vsf_wdt_mode_t mode, bool use_irq, bool reset_expected, const char *mode_str, bool log_cap)
{
    if ((ctx == NULL) || (ctx->wdt == NULL)) {
        __WDT_TRACE_ERROR("WDT test context or device is NULL!" VSF_TRACE_CFG_LINEEND);
        return false;
    }

    ctx->cap = vsf_wdt_capability(ctx->wdt);
    ctx->cfg = (vsf_wdt_cfg_t){ 0 };
    ctx->cfg.mode = mode;
    ctx->cfg.isr.handler_fn = use_irq ? __wdt_irq_handler : NULL;
    ctx->cfg.isr.target_ptr = use_irq ? ctx : NULL;
    ctx->cfg.isr.prio = APP_WDT_DEMO_CFG_IRQ_PRIO;
    ctx->irq_triggered = false;
    ctx->irq_count = 0;
    ctx->reset_expected = reset_expected;

    uint32_t max_ms = __wdt_apply_ratio(ctx->cap.max_timeout_ms, WDT_TEST_TIMEOUT_RATIO);
    ctx->cfg.max_ms = __wdt_clamp(max_ms, WDT_TEST_MIN_TIME_MS, WDT_TEST_TIMEOUT_MS_MAX);

    if (ctx->cap.support_min_timeout) {
        uint32_t min_ms = __wdt_apply_ratio(ctx->cap.max_timeout_ms, WDT_TEST_WINDOW_RATIO);
        ctx->cfg.min_ms = __wdt_clamp(min_ms, WDT_TEST_MIN_TIME_MS, ctx->cfg.max_ms);
        if (ctx->cfg.min_ms >= ctx->cfg.max_ms) {
            ctx->cfg.min_ms = ctx->cfg.max_ms / 2;
        }
    }

    if (log_cap) {
        __wdt_print_capability(&ctx->cap);
    }

    if (vsf_wdt_init(ctx->wdt, &ctx->cfg) != VSF_ERR_NONE) {
        __WDT_TRACE_ERROR("WDT init failed" VSF_TRACE_CFG_LINEEND);
        return false;
    }
    if (vsf_wdt_enable(ctx->wdt) != fsm_rt_cpl) {
        __WDT_TRACE_ERROR("WDT enable failed" VSF_TRACE_CFG_LINEEND);
        vsf_wdt_fini(ctx->wdt);
        return false;
    }

    __wdt_print_config(ctx, mode_str);
    return true;
}

static void __wdt_cleanup(wdt_test_t *ctx)
{
    if ((ctx == NULL) || (ctx->wdt == NULL)) {
        return;
    }

    if (ctx->cap.support_disable) {
        fsm_rt_t fsm_rt = vsf_wdt_disable(ctx->wdt);
        if (fsm_rt != fsm_rt_cpl) {
            __WDT_TRACE_ERROR("WDT disable failed: %d" VSF_TRACE_CFG_LINEEND, fsm_rt);
        }
        vsf_wdt_fini(ctx->wdt);
    } else {
        __WDT_TRACE_WARNING("WDT does not support disable, it will continue running!" VSF_TRACE_CFG_LINEEND);
        vsf_wdt_feed(ctx->wdt);
    }
}

static uint32_t __wdt_calc_feed_interval(const wdt_test_t *ctx)
{
    uint32_t interval;

    if (__wdt_supports_window(ctx)) {
        interval = __wdt_apply_ratio(ctx->cfg.min_ms, WDT_TEST_FEED_INTERVAL_RATIO);
        interval = __wdt_clamp(interval, ctx->cfg.min_ms, ctx->cfg.max_ms);
    } else {
        interval = __wdt_apply_ratio(ctx->cfg.max_ms, WDT_TEST_FEED_INTERVAL_RATIO);
    }

    return __wdt_clamp(interval, WDT_TEST_MIN_TIME_MS, WDT_TEST_TIMEOUT_MS_MAX);
}

static bool __wdt_test_interrupt(wdt_test_t *ctx)
{
    __WDT_TRACE_INFO("=== WDT Interrupt Test ===" VSF_TRACE_CFG_LINEEND);

    if (!ctx->wdt) {
        __WDT_TRACE_ERROR("WDT device is NULL!" VSF_TRACE_CFG_LINEEND);
        return false;
    }

    if (!vsf_wdt_capability(ctx->wdt).support_reset_none) {
        __WDT_TRACE_ERROR("WDT does not support interrupt mode (RESET_NONE)!" VSF_TRACE_CFG_LINEEND);
        return false;
    }

    if (!__wdt_setup(ctx, VSF_WDT_MODE_RESET_NONE, true, false, "interrupt", true)) {
        return false;
    }

    vsf_systimer_tick_t start_tick = vsf_systimer_get_tick();
    vsf_systimer_tick_t timeout_tick = vsf_systimer_ms_to_tick(__wdt_apply_ratio(ctx->cfg.max_ms, WDT_TEST_INTERRUPT_TIMEOUT_RATIO));

    uint32_t feed_interval_ms = __wdt_calc_feed_interval(ctx);
    vsf_systimer_tick_t feed_interval_tick = vsf_systimer_ms_to_tick(feed_interval_ms);
    vsf_systimer_tick_t last_feed = start_tick;

    __WDT_TRACE_INFO("Waiting for interrupt, feed every %d ms to avoid reset..." VSF_TRACE_CFG_LINEEND, feed_interval_ms);

    while (!ctx->irq_triggered) {
        vsf_systimer_tick_t now = vsf_systimer_get_tick();

        if (now - last_feed >= feed_interval_tick) {
            vsf_wdt_feed(ctx->wdt);
            last_feed = now;
            __WDT_TRACE_INFO("WDT fed while waiting for interrupt" VSF_TRACE_CFG_LINEEND);
        }

        if (now - start_tick > timeout_tick) {
            __WDT_TRACE_ERROR("WDT interrupt test timeout" VSF_TRACE_CFG_LINEEND);
            __wdt_cleanup(ctx);
            return false;
        }
        vsf_thread_delay_ms(WDT_TEST_POLL_INTERVAL_MS);
    }

    __WDT_TRACE_INFO("WDT interrupt triggered, count=%d" VSF_TRACE_CFG_LINEEND, ctx->irq_count);
    __wdt_cleanup(ctx);
    __WDT_TRACE_INFO("WDT interrupt test passed" VSF_TRACE_CFG_LINEEND);
    return true;
}

static bool __wdt_test_reset(wdt_test_t *ctx)
{
    __WDT_TRACE_INFO("=== WDT Reset Test ===" VSF_TRACE_CFG_LINEEND);
    __WDT_TRACE_INFO("WARNING: This test will cause system reset!" VSF_TRACE_CFG_LINEEND);

    vsf_wdt_mode_t mode;
    vsf_wdt_capability_t cap = vsf_wdt_capability(ctx->wdt);

    if (!__wdt_pick_reset_mode(&cap, &mode)) {
        __WDT_TRACE_ERROR("WDT does not support reset mode!" VSF_TRACE_CFG_LINEEND);
        return false;
    }

    if (!__wdt_setup(ctx, mode, false, true, "reset", false)) {
        return false;
    }

    uint32_t wait_ms = ctx->cfg.max_ms + __wdt_apply_ratio(ctx->cfg.max_ms, WDT_TEST_SAFETY_MARGIN_RATIO);
    if (__wdt_supports_window(ctx)) {
        __WDT_TRACE_INFO("System will reset within [%d, %d] ms window" VSF_TRACE_CFG_LINEEND, ctx->cfg.min_ms, ctx->cfg.max_ms);
    } else {
        __WDT_TRACE_INFO("System will reset in about %d ms" VSF_TRACE_CFG_LINEEND, ctx->cfg.max_ms);
    }

    vsf_thread_delay_ms(wait_ms);

    __WDT_TRACE_ERROR("WDT reset test failed - system did not reset" VSF_TRACE_CFG_LINEEND);
    __wdt_cleanup(ctx);
    return false;
}

static bool __wdt_test_feed(wdt_test_t *ctx)
{
    __WDT_TRACE_INFO("=== WDT Feed Test ===" VSF_TRACE_CFG_LINEEND);

    vsf_wdt_mode_t mode;
    vsf_wdt_capability_t cap = vsf_wdt_capability(ctx->wdt);

    if (!__wdt_pick_reset_mode(&cap, &mode)) {
        __WDT_TRACE_ERROR("WDT does not support reset mode!" VSF_TRACE_CFG_LINEEND);
        return false;
    }

    if (!__wdt_setup(ctx, mode, false, false, "feed", false)) {
        return false;
    }

    uint32_t interval_ms = __wdt_calc_feed_interval(ctx);
    uint32_t duration_ms = ctx->cfg.max_ms * WDT_TEST_FEED_DURATION_MULTIPLIER;
    if (duration_ms < WDT_TEST_FEED_DURATION_MIN_MS) {
        duration_ms = WDT_TEST_FEED_DURATION_MIN_MS;
    }

    if (__wdt_supports_window(ctx)) {
        uint32_t wait_ms = ctx->cfg.min_ms + __wdt_apply_ratio(ctx->cfg.min_ms, WDT_TEST_SAFETY_MARGIN_RATIO);
        __WDT_TRACE_INFO("Waiting %d ms to enter feed window [%d, %d]" VSF_TRACE_CFG_LINEEND, wait_ms, ctx->cfg.min_ms, ctx->cfg.max_ms);
        vsf_thread_delay_ms(wait_ms);
    }

    __WDT_TRACE_INFO("Feeding every %d ms for %d ms..." VSF_TRACE_CFG_LINEEND, interval_ms, duration_ms);

    uint32_t feed_count = 0;
    vsf_systimer_tick_t start = vsf_systimer_get_tick();
    vsf_systimer_tick_t duration = vsf_systimer_ms_to_tick(duration_ms);
    vsf_systimer_tick_t interval_tick = vsf_systimer_ms_to_tick(interval_ms);

    while (vsf_systimer_get_tick() - start < duration) {
        vsf_wdt_feed(ctx->wdt);
        feed_count++;
        __WDT_TRACE_INFO("WDT fed, count=%d" VSF_TRACE_CFG_LINEEND, feed_count);
        vsf_thread_delay_ms(interval_ms);
    }

    __WDT_TRACE_INFO("WDT feed test completed, total feeds: %d" VSF_TRACE_CFG_LINEEND, feed_count);
    __wdt_cleanup(ctx);
    __WDT_TRACE_INFO("WDT feed test passed" VSF_TRACE_CFG_LINEEND);
    return true;
}

#if APP_USE_LINUX_DEMO == ENABLED
int wdt_main(int argc, char *argv[])
{
    wdt_test_t ctx = {
        .wdt = (vsf_wdt_t *)&APP_WDT_DEMO_CFG_DEFAULT_INSTANCE,
    };

    if (argc < 2) {
        __WDT_TRACE_INFO("Usage: wdt-test [interrupt|reset|feed]" VSF_TRACE_CFG_LINEEND);
        __WDT_TRACE_INFO("  interrupt: Test WDT interrupt functionality" VSF_TRACE_CFG_LINEEND);
        __WDT_TRACE_INFO("  reset:     Test WDT reset functionality (WARNING: will reset system)" VSF_TRACE_CFG_LINEEND);
        __WDT_TRACE_INFO("  feed:      Test WDT feed functionality" VSF_TRACE_CFG_LINEEND);
        return -1;
    }

    if (strcmp(argv[1], "interrupt") == 0) {
        return __wdt_test_interrupt(&ctx) ? 0 : -1;
    } else if (strcmp(argv[1], "reset") == 0) {
        return __wdt_test_reset(&ctx) ? 0 : -1;
    } else if (strcmp(argv[1], "feed") == 0) {
        return __wdt_test_feed(&ctx) ? 0 : -1;
    } else {
        __WDT_TRACE_ERROR("Unknown test: %s" VSF_TRACE_CFG_LINEEND, argv[1]);
        return -1;
    }

    return 0;
}
#else
int VSF_USER_ENTRY(void)
{
    // WDT demo is only available in Linux demo mode
    // Use wdt_main() function in Linux demo mode instead
    return 0;
}
#endif
#endif
