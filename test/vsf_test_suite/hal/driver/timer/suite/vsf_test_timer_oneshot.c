/*============================ INCLUDES ======================================*/

#include "vsf_test_timer_oneshot.h"
#include "vsf_test_suites.h"


/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_TIMER_ONESHOT_ENABLE == ENABLED
const struct vsf_test_timer_oneshot_s {
    vsf_test_suite_t hdr;
    vsf_test_timer_oneshot_params_t          params[VSF_TEST_TIMER_ONESHOT_CASE_COUNT];
} vsf_test_timer_oneshot = {
    .hdr = {
        .name            = "timer_oneshot",
        .jmp_fn          = vsf_test_timer_oneshot_run,
        .case_count      = VSF_TEST_TIMER_ONESHOT_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_timer_oneshot_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_TIMER,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_TIMER_ONESHOT_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/


/*============================ LOCAL VARIABLES ===============================*/


#if VSF_TEST_TIMER_ONESHOT_ENABLE == ENABLED

static void __timer_isr(void *target_ptr, vsf_timer_t *timer_ptr,
                        vsf_timer_irq_mask_t irq_mask)
{
    VSF_UNUSED_PARAM(timer_ptr);
    vsf_test_suite_t *suite = target_ptr;
    if (irq_mask & VSF_TIMER_IRQ_MASK_OVERFLOW) {
        vsf_test_suite_data.timer.timer_oneshot.fired = true;
    }
}



/*============================ MACROS ========================================*/


/*============================ IMPLEMENTATION ================================*/

void vsf_test_timer_oneshot_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_timer_oneshot_params_t *p = tc->params;
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_TIMER, timer);
    vsf_timer_t *timer = inst->fixture.timer;

    VSF_TEST_TRACE_INFO("timer_oneshot:start (case=%u)" VSF_TRACE_CFG_LINEEND,
                        (unsigned)tc->case_idx);

    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware suites do not print them. */

    vsf_test_suite_data.timer.timer_oneshot.fired = false;

    vsf_timer_capability_t cap = vsf_timer_capability(timer);
    if (cap.channel_cnt < VSF_TEST_TIMER_ONESHOT_MIN_CHANNEL_COUNT) {
        VSF_TEST_TRACE_ERROR("timer_oneshot:channel_cnt too low (min=%u actual=%u)" VSF_TRACE_CFG_LINEEND,
                              (unsigned)VSF_TEST_TIMER_ONESHOT_MIN_CHANNEL_COUNT,
                              (unsigned)cap.channel_cnt);
    }
    VSF_TEST_ASSERT(cap.channel_cnt >= VSF_TEST_TIMER_ONESHOT_MIN_CHANNEL_COUNT);
    if (cap.timer_bitlen != VSF_TEST_TIMER_ONESHOT_EXPECTED_BITLEN) {
        VSF_TEST_TRACE_ERROR("timer_oneshot:timer_bitlen mismatch (expected=%u actual=%u)" VSF_TRACE_CFG_LINEEND,
                              (unsigned)VSF_TEST_TIMER_ONESHOT_EXPECTED_BITLEN,
                              (unsigned)cap.timer_bitlen);
    }
    VSF_TEST_ASSERT(cap.timer_bitlen == VSF_TEST_TIMER_ONESHOT_EXPECTED_BITLEN);

    VSF_TEST_TRACE_DEBUG("timer_oneshot:init" VSF_TRACE_CFG_LINEEND);
    vsf_err_t err = vsf_timer_init(timer, &(vsf_timer_cfg_t){
        .period = VSF_TEST_TIMER_ONESHOT_PERIOD_US,
        .isr = {
            .handler_fn = __timer_isr,
            .target_ptr = NULL,
            .prio       = VSF_TEST_TIMER_ONESHOT_PRIO,
        },
    });
    VSF_TEST_ASSERT_ERR_NONE(err,
        "timer_oneshot:vsf_timer_init failed (err=%d) (period_us=%lu)"
                             VSF_TRACE_CFG_LINEEND, (int)err, (unsigned long)VSF_TEST_TIMER_ONESHOT_PERIOD_US);

    while (fsm_rt_cpl != vsf_timer_enable(timer));

    /* Configure channel 0 for one-shot mode */
    VSF_TEST_TRACE_DEBUG("timer_oneshot:channel_config" VSF_TRACE_CFG_LINEEND);
    err = vsf_timer_channel_config(timer, 0, &(vsf_timer_channel_cfg_t){
        .mode  = VSF_TEST_TIMER_ONESHOT_MODE,
        .pulse = VSF_TEST_TIMER_ONESHOT_PERIOD_US,
    });
    VSF_TEST_ASSERT_ERR_NONE(err,
        "timer_oneshot:vsf_timer_channel_config failed (err=%d) (ch=%u mode=0x%x)"
                             VSF_TRACE_CFG_LINEEND, (int)err, 0, (unsigned)VSF_TEST_TIMER_ONESHOT_MODE);

    /* Start the channel alarm */
    VSF_TEST_TRACE_DEBUG("timer_oneshot:channel_start" VSF_TRACE_CFG_LINEEND);
    err = vsf_timer_channel_start(timer, 0);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "timer_oneshot:vsf_timer_channel_start failed (err=%d) (ch=%u)"
                             VSF_TRACE_CFG_LINEEND, (int)err, 0);

    /* Wait up to ~150ms for the alarm to fire */
    VSF_TEST_TRACE_DEBUG("timer_oneshot:waiting for irq" VSF_TRACE_CFG_LINEEND);
    VSF_TEST_WAIT_FOR(vsf_test_suite_data.timer.timer_oneshot.fired, VSF_TEST_TIMER_ONESHOT_TIMEOUT_MS);

    if (!vsf_test_suite_data.timer.timer_oneshot.fired) {
        VSF_TEST_TRACE_ERROR("timer_oneshot:irq timeout after %u ms"
                             VSF_TRACE_CFG_LINEEND,
                             (unsigned)VSF_TEST_TIMER_ONESHOT_TIMEOUT_MS);
    }
    VSF_TEST_ASSERT(vsf_test_suite_data.timer.timer_oneshot.fired);

    VSF_TEST_TRACE_INFO("timer_oneshot:pass" VSF_TRACE_CFG_LINEEND);

    vsf_timer_channel_stop(timer, 0);
    while (fsm_rt_cpl != vsf_timer_disable(timer));
    vsf_timer_fini(timer);
}

#endif /* VSF_TEST_TIMER_ONESHOT_ENABLE == ENABLED */

/* EOF */

