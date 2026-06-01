/*============================ INCLUDES ======================================*/

#include "vsf_test_arch_systimer_freq.h"
#include "vsf_test_suites.h"

/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_ARCH_SYSTIMER_FREQ_ENABLE == ENABLED
const struct vsf_test_arch_systimer_freq_s {
    vsf_test_suite_t hdr;
    vsf_test_arch_systimer_freq_params_t        params[VSF_TEST_ARCH_SYSTIMER_FREQ_CASE_COUNT];
} vsf_test_arch_systimer_freq = {
    .hdr = {
        .name            = "arch_systimer_freq",
        .jmp_fn          = vsf_test_arch_systimer_freq_run,
        .case_count      = VSF_TEST_ARCH_SYSTIMER_FREQ_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_arch_systimer_freq_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_ARCH,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_ARCH_SYSTIMER_FREQ_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/

#if VSF_TEST_ARCH_SYSTIMER_FREQ_ENABLE == ENABLED

void vsf_test_arch_systimer_freq_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    VSF_UNUSED_PARAM(suite);
    VSF_UNUSED_PARAM(tc);
    VSF_UNUSED_PARAM(inst);

    VSF_TEST_TRACE_INFO("arch_systimer_freq:start" VSF_TRACE_CFG_LINEEND);

    /* Verify systimer frequency via serial pulse markers.
     *
     * Firmware prints a marker, busy-waits a known number of systimer ticks,
     * then prints a second marker.  The host run() function timestamps the
     * wall-clock arrival of each marker via time.monotonic() and computes the
     * independent frequency::
     *
     *   measured_freq = expected_ticks / (t1 - t0)
     *
     * If the clock tree feeding the systimer is misconfigured, the host-
     * measured marker gap will reveal the actual tick rate independently
     * of what vsf_systimer_get_freq() reports.
     */
    uint32_t freq = vsf_systimer_get_freq();
    vsf_systimer_tick_t wait_ticks = vsf_systimer_ms_to_tick(
        VSF_TEST_ARCH_SYSTIMER_FREQ_SERIAL_WAIT_MS);

    VSF_TEST_TRACE_INFO("arch_systimer_freq:reported_freq=%lu Hz"
                        VSF_TRACE_CFG_LINEEND, (unsigned long)freq);
    VSF_TEST_TRACE_INFO("arch_systimer_freq:pulse_start wait_ms=%lu wait_ticks=%llu"
                        VSF_TRACE_CFG_LINEEND,
                        (unsigned long)VSF_TEST_ARCH_SYSTIMER_FREQ_SERIAL_WAIT_MS,
                        (unsigned long long)wait_ticks);

    vsf_systimer_tick_t target = vsf_systimer_get() + wait_ticks;
    while (vsf_systimer_get() < target);

    VSF_TEST_TRACE_INFO("arch_systimer_freq:pulse_end" VSF_TRACE_CFG_LINEEND);
    VSF_TEST_TRACE_INFO("arch_systimer_freq:pass" VSF_TRACE_CFG_LINEEND);
}

#endif /* VSF_TEST_ARCH_SYSTIMER_FREQ_ENABLE == ENABLED */

/* EOF */
