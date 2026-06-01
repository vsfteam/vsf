/*============================ INCLUDES ======================================*/

#include "vsf_test_rtc_alarm.h"
#include "vsf_test_suites.h"


/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_RTC_ALARM_ENABLE == ENABLED
const struct vsf_test_rtc_alarm_s {
    vsf_test_suite_t hdr;
    vsf_test_rtc_alarm_params_t          params[VSF_TEST_RTC_ALARM_CASE_COUNT];
} vsf_test_rtc_alarm = {
    .hdr = {
        .name            = "rtc_alarm",
        .jmp_fn          = vsf_test_rtc_alarm_run,
        .case_count      = VSF_TEST_RTC_ALARM_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_rtc_alarm_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_RTC,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_RTC_ALARM_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/


/*============================ LOCAL VARIABLES ===============================*/


#if VSF_TEST_RTC_ALARM_ENABLE == ENABLED

static void __rtc_alarm_isr(void *target_ptr, vsf_rtc_t *rtc_ptr,
                            vsf_rtc_irq_mask_t irq_mask)
{
    VSF_UNUSED_PARAM(rtc_ptr);
    vsf_test_suite_t *suite = target_ptr;
    if (irq_mask & VSF_RTC_IRQ_MASK_ALARM) {
        vsf_test_suite_data.rtc.rtc_alarm.alarm_triggered = true;
    }
}



/*============================ MACROS ========================================*/


/*============================ IMPLEMENTATION ================================*/

void vsf_test_rtc_alarm_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_rtc_alarm_params_t *p = tc->params;
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_RTC, rtc);
    vsf_rtc_t *rtc = inst->fixture.rtc;

    VSF_TEST_TRACE_INFO("RTC:ALARM: start (case=%u)" VSF_TRACE_CFG_LINEEND,
                        (unsigned)tc->case_idx);

    vsf_test_suite_data.rtc.rtc_alarm.alarm_triggered = false;

    // Set datetime to 2024-01-01 12:00:00 Monday
    vsf_rtc_tm_t set_tm = {
        .tm_year = 2024,
        .tm_mon  = 1,
        .tm_mday = 1,
        .tm_wday = 1,
        .tm_hour = 12,
        .tm_min  = 0,
        .tm_sec  = 0,
        .tm_ms   = 0,
    };

    VSF_TEST_TRACE_DEBUG("RTC:ALARM: vsf_rtc_init" VSF_TRACE_CFG_LINEEND);
    vsf_err_t err = vsf_rtc_init(rtc, &(vsf_rtc_cfg_t){
        .isr = {
            .handler_fn = __rtc_alarm_isr,
            .target_ptr = NULL,
            .prio       = VSF_TEST_RTC_ALARM_PRIO,
        },
    });
    VSF_TEST_ASSERT_ERR_NONE(err,
        "RTC:ALARM: vsf_rtc_init failed (err=%d)"
                             VSF_TRACE_CFG_LINEEND, (int)err);

    VSF_TEST_TRACE_DEBUG("RTC:ALARM: vsf_rtc_set (2024-01-01 12:00:00)"
                         VSF_TRACE_CFG_LINEEND);
    err = vsf_rtc_set(rtc, &set_tm);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "RTC:ALARM: vsf_rtc_set failed (err=%d) (year=%u mon=%u mday=%u hour=%u min=%u sec=%u)"
                             VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)set_tm.tm_year, (unsigned)set_tm.tm_mon, (unsigned)set_tm.tm_mday, (unsigned)set_tm.tm_hour, (unsigned)set_tm.tm_min, (unsigned)set_tm.tm_sec);

    VSF_TEST_TRACE_DEBUG("RTC:ALARM: enable" VSF_TRACE_CFG_LINEEND);
    while (fsm_rt_cpl != vsf_rtc_enable(rtc));

    // Set alarm for 2 seconds in the future: 12:00:02
    vsf_rtc_tm_t alarm_tm = {
        .tm_year = 2024,
        .tm_mon  = 1,
        .tm_mday = 1,
        .tm_wday = 1,
        .tm_hour = 12,
        .tm_min  = 0,
        .tm_sec  = 2,
        .tm_ms   = 0,
    };

    VSF_TEST_TRACE_DEBUG("RTC:ALARM: vsf_rtc_ctrl SET_ALARM (12:00:02)"
                         VSF_TRACE_CFG_LINEEND);
    err = vsf_rtc_ctrl(rtc, VSF_RTC_CTRL_SET_ALARM, &alarm_tm);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "RTC:ALARM: SET_ALARM failed (err=%d) (alarm_year=%u alarm_mon=%u alarm_mday=%u alarm_hour=%u alarm_min=%u alarm_sec=%u)"
                             VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)alarm_tm.tm_year, (unsigned)alarm_tm.tm_mon, (unsigned)alarm_tm.tm_mday, (unsigned)alarm_tm.tm_hour, (unsigned)alarm_tm.tm_min, (unsigned)alarm_tm.tm_sec);

    // Enable alarm IRQ
    VSF_TEST_TRACE_DEBUG("RTC:ALARM: IRQ_ENABLE" VSF_TRACE_CFG_LINEEND);
    err = vsf_rtc_ctrl(rtc, VSF_RTC_CTRL_IRQ_ENABLE, NULL);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "RTC:ALARM: IRQ_ENABLE failed (err=%d) (ctrl=VSF_RTC_CTRL_IRQ_ENABLE)"
                             VSF_TRACE_CFG_LINEEND, (int)err);

    // Wait up to ~3.5 seconds for alarm to fire
    VSF_TEST_TRACE_DEBUG("RTC:ALARM: waiting for alarm" VSF_TRACE_CFG_LINEEND);
    VSF_TEST_WAIT_FOR(vsf_test_suite_data.rtc.rtc_alarm.alarm_triggered,
                      VSF_TEST_RTC_ALARM_TIMEOUT_MS);

    if (!vsf_test_suite_data.rtc.rtc_alarm.alarm_triggered) {
        VSF_TEST_TRACE_ERROR("RTC:ALARM: alarm timeout after %u ms"
                             VSF_TRACE_CFG_LINEEND,
                             (unsigned)VSF_TEST_RTC_ALARM_TIMEOUT_MS);
    }
    VSF_TEST_ASSERT(vsf_test_suite_data.rtc.rtc_alarm.alarm_triggered);

    VSF_TEST_TRACE_INFO("RTC:ALARM: PASS" VSF_TRACE_CFG_LINEEND);

    // Disable alarm IRQ
    vsf_rtc_ctrl(rtc, VSF_RTC_CTRL_IRQ_DISABLE, NULL);
    while (fsm_rt_cpl != vsf_rtc_disable(rtc));
    vsf_rtc_fini(rtc);
}

#endif /* VSF_TEST_RTC_ALARM_ENABLE == ENABLED */

/* EOF */

