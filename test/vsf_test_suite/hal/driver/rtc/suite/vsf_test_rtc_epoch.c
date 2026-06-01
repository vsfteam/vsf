/*============================ INCLUDES ======================================*/

#include "vsf_test_rtc_epoch.h"
#include "vsf_test_suites.h"


/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_RTC_EPOCH_ENABLE == ENABLED
const struct vsf_test_rtc_epoch_s {
    vsf_test_suite_t hdr;
    vsf_test_rtc_epoch_params_t          params[VSF_TEST_RTC_EPOCH_CASE_COUNT];
} vsf_test_rtc_epoch = {
    .hdr = {
        .name            = "rtc_epoch",
        .jmp_fn          = vsf_test_rtc_epoch_run,
        .case_count      = VSF_TEST_RTC_EPOCH_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_rtc_epoch_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_RTC,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_RTC_EPOCH_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/



#if VSF_TEST_RTC_EPOCH_ENABLE == ENABLED

/*============================ MACROS ========================================*/

/*============================ LOCAL VARIABLES ===============================*/

/*============================ IMPLEMENTATION ================================*/

void vsf_test_rtc_epoch_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_rtc_epoch_params_t *p = tc->params;
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_RTC, rtc);
    vsf_rtc_t *rtc = inst->fixture.rtc;

    VSF_TEST_TRACE_INFO("RTC:EPOCH: start (case=%u)" VSF_TRACE_CFG_LINEEND,
                        (unsigned)tc->case_idx);

    /* Case 1: Set and get epoch time */
    VSF_TEST_TRACE_DEBUG("RTC:EPOCH: vsf_rtc_set_time (%llu)"
                         VSF_TRACE_CFG_LINEEND,
                         (unsigned long long)VSF_TEST_RTC_EPOCH_SET_SECONDS);
    vsf_rtc_time_t set_seconds = VSF_TEST_RTC_EPOCH_SET_SECONDS;
    vsf_err_t err = vsf_rtc_set_time(rtc, set_seconds, 0);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "RTC:EPOCH: vsf_rtc_set_time failed (err=%d) (seconds=%llu ms=%u)"
                             VSF_TRACE_CFG_LINEEND, (int)err, (unsigned long long)set_seconds, (unsigned)0);

    VSF_TEST_TRACE_DEBUG("RTC:EPOCH: enable" VSF_TRACE_CFG_LINEEND);
    while (fsm_rt_cpl != vsf_rtc_enable(rtc));

    vsf_rtc_time_t get_seconds = 0;
    vsf_rtc_time_t get_ms = VSF_TEST_RTC_EPOCH_MS_IRRELEVANT;
    err = vsf_rtc_get_time(rtc, &get_seconds, &get_ms);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "RTC:EPOCH: vsf_rtc_get_time failed (err=%d)"
                             VSF_TRACE_CFG_LINEEND, (int)err);
    if (get_seconds != set_seconds) {
        VSF_TEST_TRACE_ERROR("RTC:EPOCH: seconds mismatch (set=%llu get=%llu)"
                             VSF_TRACE_CFG_LINEEND,
                             (unsigned long long)set_seconds,
                             (unsigned long long)get_seconds);
    }
    VSF_TEST_ASSERT(get_seconds == set_seconds);
    if (get_ms != 0) {
        VSF_TEST_TRACE_ERROR("RTC:EPOCH: ms not zero (%u)" VSF_TRACE_CFG_LINEEND,
                             (unsigned)get_ms);
    }
    VSF_TEST_ASSERT(get_ms == 0);

    VSF_TEST_TRACE_DEBUG("RTC:EPOCH: set_get seconds=%llu ms=%u"
                         VSF_TRACE_CFG_LINEEND,
                         (unsigned long long)get_seconds, (unsigned)get_ms);

    /* Case 2: Increment after ~1.1 seconds */
    vsf_test_busy_wait_ms(VSF_TEST_RTC_EPOCH_BUSY_WAIT_MS);

    vsf_rtc_time_t later_seconds;
    err = vsf_rtc_get_time(rtc, &later_seconds, NULL);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "RTC:EPOCH: vsf_rtc_get_time failed (err=%d)"
                             VSF_TRACE_CFG_LINEEND, (int)err);
    if (later_seconds < set_seconds + 1) {
        VSF_TEST_TRACE_ERROR("RTC:EPOCH: time did not increment (%llu < %llu)"
                             VSF_TRACE_CFG_LINEEND,
                             (unsigned long long)later_seconds,
                             (unsigned long long)(set_seconds + 1));
    }
    VSF_TEST_ASSERT(later_seconds >= set_seconds + 1);

    VSF_TEST_TRACE_DEBUG("RTC:EPOCH: increment after 1.1s: %llu (expected >= %llu)"
                         VSF_TRACE_CFG_LINEEND,
                         (unsigned long long)later_seconds,
                         (unsigned long long)(set_seconds + 1));

    /* Case 3: Millisecond pointer NULL is OK */
    err = vsf_rtc_get_time(rtc, &get_seconds, NULL);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "RTC:EPOCH: vsf_rtc_get_time(NULL ms) failed (err=%d)"
                             VSF_TRACE_CFG_LINEEND, (int)err);

    VSF_TEST_TRACE_DEBUG("RTC:EPOCH: null_ms_ok" VSF_TRACE_CFG_LINEEND);

    VSF_TEST_TRACE_INFO("RTC:EPOCH: PASS (seconds=%llu)"
                        VSF_TRACE_CFG_LINEEND,
                        (unsigned long long)get_seconds);

    while (fsm_rt_cpl != vsf_rtc_disable(rtc));
}

#endif /* VSF_TEST_RTC_EPOCH_ENABLE == ENABLED */

/* EOF */


