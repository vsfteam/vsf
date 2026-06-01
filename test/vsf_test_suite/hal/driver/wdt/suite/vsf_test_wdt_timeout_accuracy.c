/*============================ INCLUDES ======================================*/

#include "vsf_test_wdt_timeout_accuracy.h"
#include "vsf_test_suites.h"

/* Magic value written to scratch[2] so the boot-time check knows this was
 * our WDT timeout-accuracy reset (not a power-on reset or other WDT reset). */
#define WDT_ACCURACY_SCRATCH_MAGIC  0x575F4143  /* "W_AC" */

/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_WDT_TIMEOUT_ACCURACY_ENABLE == ENABLED
const struct vsf_test_wdt_timeout_accuracy_s {
    vsf_test_suite_t hdr;
    vsf_test_wdt_timeout_accuracy_params_t params[VSF_TEST_WDT_TIMEOUT_ACCURACY_CASE_COUNT];
} vsf_test_wdt_timeout_accuracy = {
    .hdr = {
        .name            = "wdt_timeout_accuracy",
        .jmp_fn          = vsf_test_wdt_timeout_accuracy_run,
        .case_count      = VSF_TEST_WDT_TIMEOUT_ACCURACY_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_wdt_timeout_accuracy_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_WDT,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_WDT_TIMEOUT_ACCURACY_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/

#if VSF_TEST_WDT_TIMEOUT_ACCURACY_ENABLE == ENABLED

#include "hal/vsf_hal.h"

/* RP2040 hardware timer (always counting, survives WDT reset) and scratch
 * registers (survive WDT reset, cleared on power-on). These are board-specific
 * but necessary: the WDT has no HAL API for scratch registers, and no generic
 * timer survives WDT reset. */
#include "hardware/structs/timer.h"
#include "hardware/structs/watchdog.h"

void vsf_test_wdt_timeout_accuracy_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_wdt_timeout_accuracy_params_t *p = tc->params;
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_WDT, wdt);
    vsf_wdt_t *wdt = inst->fixture.wdt;

    VSF_TEST_TRACE_INFO("WDT:ACCURACY: start (case=%u timeout=%u tolerance=%u%%)"
                        VSF_TRACE_CFG_LINEEND,
                        (unsigned)tc->case_idx, (unsigned)p->timeout_ms,
                        (unsigned)p->tolerance_pct);

    /* Phase-1: save start time and expected timeout to scratch registers,
     * then arm the WDT and wait for reset. */
    watchdog_hw->scratch[0] = timer_hw->timerawl;
    watchdog_hw->scratch[1] = p->timeout_ms;
    watchdog_hw->scratch[2] = WDT_ACCURACY_SCRATCH_MAGIC;

    VSF_TEST_TRACE_DEBUG("WDT:ACCURACY: vsf_wdt_init (timeout=%u)"
                         VSF_TRACE_CFG_LINEEND,
                         (unsigned)p->timeout_ms);
    vsf_err_t err = vsf_wdt_init(wdt, &(vsf_wdt_cfg_t){
        .mode   = VSF_TEST_WDT_TIMEOUT_ACCURACY_MODE,
        .max_ms = p->timeout_ms,
        .min_ms = 0,
    });
    VSF_TEST_ASSERT_ERR_NONE(err,
        "WDT:ACCURACY: vsf_wdt_init failed (err=%d) (max_ms=%u)"
                             VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)p->timeout_ms);

    VSF_TEST_TRACE_DEBUG("WDT:ACCURACY: enable" VSF_TRACE_CFG_LINEEND);
    while (fsm_rt_cpl != vsf_wdt_enable(wdt));

    VSF_TEST_TRACE_INFO("WDT:ACCURACY:ARMED timeout=%u" VSF_TRACE_CFG_LINEEND,
                        (unsigned)p->timeout_ms);

    /* Wait for WDT reset. This path never returns normally. */
    vsf_test_busy_wait_ms(p->timeout_ms + VSF_TEST_WDT_TIMEOUT_MARGIN_MS);

    /* Should never reach here — WDT failed to reset. */
    watchdog_hw->scratch[2] = 0;  // clear marker, we didn't reset
    VSF_TEST_TRACE_ERROR("WDT:ACCURACY: WDT failed to reset chip" VSF_TRACE_CFG_LINEEND);
    VSF_TEST_ASSERT(false);
}

#endif /* VSF_TEST_WDT_TIMEOUT_ACCURACY_ENABLE == ENABLED */

/* EOF */
