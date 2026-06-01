/*============================ INCLUDES ======================================*/

#include "vsf_test_timer_async.h"
#include "vsf_test_suites.h"


/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_TIMER_ASYNC_ENABLE == ENABLED
const struct vsf_test_timer_async_s {
    vsf_test_suite_t hdr;
    vsf_test_timer_async_params_t          params[VSF_TEST_TIMER_ASYNC_CASE_COUNT];
} vsf_test_timer_async = {
    .hdr = {
        .name            = "timer_async",
        .jmp_fn          = vsf_test_timer_async_run,
        .case_count      = VSF_TEST_TIMER_ASYNC_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_timer_async_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_TIMER,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_TIMER_ASYNC_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/


/*============================ LOCAL VARIABLES ===============================*/


#if VSF_TEST_TIMER_ASYNC_ENABLE == ENABLED

static void __timer_isr(void *target_ptr, vsf_timer_t *timer_ptr,
                        vsf_timer_irq_mask_t irq_mask)
{
    VSF_UNUSED_PARAM(timer_ptr);
    vsf_test_suite_t *suite = target_ptr;
    if (irq_mask & VSF_TIMER_IRQ_MASK_OVERFLOW) {
        if (vsf_test_suite_data.timer.timer_async.counter < VSF_TEST_TIMER_ASYNC_MAX_COUNTER) {
            vsf_test_suite_data.timer.timer_async.counter++;
        }
    }
}



/*============================ MACROS ========================================*/

/*============================ IMPLEMENTATION ================================*/

void vsf_test_timer_async_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_timer_async_params_t *p = tc->params;
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_TIMER, timer);
    vsf_timer_t *timer = inst->fixture.timer;

    VSF_TEST_TRACE_INFO("timer_async:start (case=%u)" VSF_TRACE_CFG_LINEEND,
                        (unsigned)tc->case_idx);

    vsf_test_suite_data.timer.timer_async.counter = 0;

    vsf_timer_capability_t cap = vsf_timer_capability(timer);
    if (cap.channel_cnt < VSF_TEST_TIMER_ASYNC_MIN_CHANNEL_COUNT) {
        VSF_TEST_TRACE_ERROR("timer_async:channel_cnt too low (min=%u actual=%u)" VSF_TRACE_CFG_LINEEND,
                              (unsigned)VSF_TEST_TIMER_ASYNC_MIN_CHANNEL_COUNT,
                              (unsigned)cap.channel_cnt);
    }
    VSF_TEST_ASSERT(cap.channel_cnt >= VSF_TEST_TIMER_ASYNC_MIN_CHANNEL_COUNT);
    if (cap.timer_bitlen != VSF_TEST_TIMER_ASYNC_EXPECTED_BITLEN) {
        VSF_TEST_TRACE_ERROR("timer_async:timer_bitlen mismatch (expected=%u actual=%u)" VSF_TRACE_CFG_LINEEND,
                              (unsigned)VSF_TEST_TIMER_ASYNC_EXPECTED_BITLEN,
                              (unsigned)cap.timer_bitlen);
    }
    VSF_TEST_ASSERT(cap.timer_bitlen == VSF_TEST_TIMER_ASYNC_EXPECTED_BITLEN);

    VSF_TEST_TRACE_DEBUG("timer_async:init" VSF_TRACE_CFG_LINEEND);
    vsf_err_t err = vsf_timer_init(timer, &(vsf_timer_cfg_t){
        .period = VSF_TEST_TIMER_ASYNC_PERIOD_US,
        .isr = {
            .handler_fn = __timer_isr,
            .target_ptr = NULL,
            .prio       = VSF_TEST_TIMER_ASYNC_PRIO,
        },
    });
    VSF_TEST_ASSERT_ERR_NONE(err,
        "timer_async:vsf_timer_init failed (err=%d) (period_us=%lu)"
                             VSF_TRACE_CFG_LINEEND, (int)err, (unsigned long)VSF_TEST_TIMER_ASYNC_PERIOD_US);
    while (fsm_rt_cpl != vsf_timer_enable(timer));

    /* --- Test 1: Async oneshot via channel_request_start --- */
    VSF_TEST_TRACE_DEBUG("timer_async:test1 oneshot start" VSF_TRACE_CFG_LINEEND);
    vsf_test_suite_data.timer.timer_async.counter = 0;
    uint32_t period_buf = VSF_TEST_TIMER_ASYNC_PERIOD_US;
    err = vsf_timer_channel_request_start(timer, 0, &(vsf_timer_channel_request_t){
        .length = 1,
        .period_buffer = &period_buf,
    });
    VSF_TEST_ASSERT_ERR_NONE(err,
        "timer_async:test1 vsf_timer_channel_request_start failed (err=%d) (ch=%u)"
                             VSF_TRACE_CFG_LINEEND, (int)err, 0);

    /* status should report busy while channel is running */
    vsf_timer_status_t status = vsf_timer_status(timer);
    VSF_TEST_ASSERT(status.value != 0);

    VSF_TEST_TRACE_DEBUG("timer_async:test1 waiting for irq" VSF_TRACE_CFG_LINEEND);
    VSF_TEST_WAIT_FOR(vsf_test_suite_data.timer.timer_async.counter >= 1, VSF_TEST_TIMER_ASYNC_TIMEOUT_MS);
    if (vsf_test_suite_data.timer.timer_async.counter < 1) {
        VSF_TEST_TRACE_ERROR("timer_async:test1 irq timeout after %u ms"
                             VSF_TRACE_CFG_LINEEND,
                             (unsigned)VSF_TEST_TIMER_ASYNC_TIMEOUT_MS);
    }
    VSF_TEST_ASSERT(vsf_test_suite_data.timer.timer_async.counter == 1);

    /* irq_clear should return overflow mask after alarm fired, then 0 */
    VSF_TEST_TRACE_DEBUG("timer_async:test1 irq_clear" VSF_TRACE_CFG_LINEEND);
    vsf_timer_irq_mask_t cleared = vsf_timer_irq_clear(timer, VSF_TIMER_IRQ_MASK_OVERFLOW);
    if (cleared != VSF_TIMER_IRQ_MASK_OVERFLOW) {
        VSF_TEST_TRACE_ERROR("timer_async:test1 first irq_clear failed (got=0x%x expected=0x%x)"
                             VSF_TRACE_CFG_LINEEND,
                             (unsigned)cleared, (unsigned)VSF_TIMER_IRQ_MASK_OVERFLOW);
    }
    VSF_TEST_ASSERT(cleared == VSF_TIMER_IRQ_MASK_OVERFLOW);
    cleared = vsf_timer_irq_clear(timer, VSF_TIMER_IRQ_MASK_OVERFLOW);
    if (cleared != 0) {
        VSF_TEST_TRACE_ERROR("timer_async:test1 second irq_clear failed (got=0x%x expected=0)"
                             VSF_TRACE_CFG_LINEEND, (unsigned)cleared);
    }
    VSF_TEST_ASSERT(cleared == 0);

    /* status should be idle after oneshot completes */
    status = vsf_timer_status(timer);
    if (status.value != 0) {
        VSF_TEST_TRACE_ERROR("timer_async:test1 status not idle (got=0x%x)"
                             VSF_TRACE_CFG_LINEEND, (unsigned)status.value);
    }
    VSF_TEST_ASSERT(status.value == 0);

    /* --- Test 2: Async periodic via channel_request_start --- */
    VSF_TEST_TRACE_DEBUG("timer_async:test2 periodic start" VSF_TRACE_CFG_LINEEND);
    vsf_test_suite_data.timer.timer_async.counter = 0;
    err = vsf_timer_channel_config(timer, 0, &(vsf_timer_channel_cfg_t){
        .mode  = VSF_TEST_TIMER_ASYNC_MODE,
        .pulse = VSF_TEST_TIMER_ASYNC_PERIOD_US,
    });
    VSF_TEST_ASSERT_ERR_NONE(err,
        "timer_async:test2 vsf_timer_channel_config failed (err=%d) (ch=%u mode=0x%x)"
                             VSF_TRACE_CFG_LINEEND, (int)err, 0, (unsigned)VSF_TEST_TIMER_ASYNC_MODE);

    err = vsf_timer_channel_request_start(timer, 0, &(vsf_timer_channel_request_t){
        .length = 1,
        .period_buffer = &period_buf,
    });
    VSF_TEST_ASSERT_ERR_NONE(err,
        "timer_async:test2 vsf_timer_channel_request_start failed (err=%d) (ch=%u)"
                             VSF_TRACE_CFG_LINEEND, (int)err, 0);

    VSF_TEST_TRACE_DEBUG("timer_async:test2 waiting for periodic irqs" VSF_TRACE_CFG_LINEEND);
    VSF_TEST_WAIT_FOR(vsf_test_suite_data.timer.timer_async.counter >= VSF_TEST_TIMER_ASYNC_COUNT, VSF_TEST_TIMER_ASYNC_PERIOD_TIMEOUT_MS);
    if (vsf_test_suite_data.timer.timer_async.counter < VSF_TEST_TIMER_ASYNC_COUNT) {
        VSF_TEST_TRACE_ERROR("timer_async:test2 periodic count timeout (got=%u expected=%u)"
                             VSF_TRACE_CFG_LINEEND,
                             (unsigned)vsf_test_suite_data.timer.timer_async.counter,
                             (unsigned)VSF_TEST_TIMER_ASYNC_COUNT);
    }
    VSF_TEST_ASSERT(vsf_test_suite_data.timer.timer_async.counter == VSF_TEST_TIMER_ASYNC_COUNT);

    /* --- Test 3: Async stop via channel_request_stop --- */
    VSF_TEST_TRACE_DEBUG("timer_async:test3 stop start" VSF_TRACE_CFG_LINEEND);
    vsf_test_suite_data.timer.timer_async.counter = 0;
    err = vsf_timer_channel_request_start(timer, 0, &(vsf_timer_channel_request_t){
        .length = 1,
        .period_buffer = &period_buf,
    });
    VSF_TEST_ASSERT_ERR_NONE(err,
        "timer_async:test3 vsf_timer_channel_request_start failed (err=%d) (ch=%u)"
                             VSF_TRACE_CFG_LINEEND, (int)err, 0);

    VSF_TEST_TRACE_DEBUG("timer_async:test3 waiting for irqs then stop" VSF_TRACE_CFG_LINEEND);
    VSF_TEST_WAIT_FOR(vsf_test_suite_data.timer.timer_async.counter >= 3, VSF_TEST_TIMER_ASYNC_STOP_TIMEOUT_MS);
    if (vsf_test_suite_data.timer.timer_async.counter < 3) {
        VSF_TEST_TRACE_ERROR("timer_async:test3 irq count timeout (got=%u expected>=3)"
                             VSF_TRACE_CFG_LINEEND,
                             (unsigned)vsf_test_suite_data.timer.timer_async.counter);
    }
    VSF_TEST_ASSERT(vsf_test_suite_data.timer.timer_async.counter >= 3);

    VSF_TEST_TRACE_DEBUG("timer_async:test3 channel_request_stop" VSF_TRACE_CFG_LINEEND);
    err = vsf_timer_channel_request_stop(timer, 0);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "timer_async:test3 vsf_timer_channel_request_stop failed (err=%d) (ch=%u)"
                             VSF_TRACE_CFG_LINEEND, (int)err, 0);

    uint8_t count_after_stop = vsf_test_suite_data.timer.timer_async.counter;
    VSF_TEST_TRACE_DEBUG("timer_async:test3 verify stop (count=%u)" VSF_TRACE_CFG_LINEEND,
                          (unsigned)count_after_stop);
    vsf_test_busy_wait_ms(VSF_TEST_TIMER_ASYNC_STOP_TIMEOUT_MS);
    if (vsf_test_suite_data.timer.timer_async.counter != count_after_stop) {
        VSF_TEST_TRACE_ERROR("timer_async:test3 counter changed after stop (before=%u after=%u)"
                             VSF_TRACE_CFG_LINEEND,
                             (unsigned)count_after_stop,
                             (unsigned)vsf_test_suite_data.timer.timer_async.counter);
    }
    VSF_TEST_ASSERT(vsf_test_suite_data.timer.timer_async.counter == count_after_stop);

    VSF_TEST_TRACE_INFO("timer_async:pass" VSF_TRACE_CFG_LINEEND);

    vsf_timer_channel_stop(timer, 0);
    while (fsm_rt_cpl != vsf_timer_disable(timer));
    vsf_timer_fini(timer);
}

#endif /* VSF_TEST_TIMER_ASYNC_ENABLE == ENABLED */

/* EOF */

