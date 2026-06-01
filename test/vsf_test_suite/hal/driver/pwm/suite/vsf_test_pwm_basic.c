/*============================ INCLUDES ======================================*/

#include "vsf_test_pwm_basic.h"
#include "vsf_test_suites.h"


/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_PWM_BASIC_ENABLE == ENABLED
const struct vsf_test_pwm_basic_s {
    vsf_test_suite_t hdr;
    vsf_test_pwm_basic_params_t          params[VSF_TEST_PWM_BASIC_CASE_COUNT];
} vsf_test_pwm_basic = {
    .hdr = {
        .name            = "pwm_basic",
        .jmp_fn          = vsf_test_pwm_basic_run,
        .case_count      = VSF_TEST_PWM_BASIC_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_pwm_basic_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_PWM,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_PWM_BASIC_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/



#if VSF_TEST_PWM_BASIC_ENABLE == ENABLED

/*============================ MACROS ========================================*/


/*============================ IMPLEMENTATION ================================*/

void vsf_test_pwm_basic_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_pwm_basic_params_t *p = tc->params;
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_PWM, pwm);
    vsf_pwm_t *pwm = inst->fixture.pwm;

    VSF_TEST_TRACE_INFO("PWM:BASIC: start (case=%u)" VSF_TRACE_CFG_LINEEND,
                        (unsigned)tc->case_idx);

    /* Test capability */
    vsf_pwm_capability_t cap = vsf_pwm_capability(pwm);
    if (cap.max_freq == 0) {
        VSF_TEST_TRACE_ERROR("pwm_basic:max_freq is 0 (value=%u)" VSF_TRACE_CFG_LINEEND,
                              (unsigned)cap.max_freq);
    }
    VSF_TEST_ASSERT(cap.max_freq > 0);
    if (cap.min_freq == 0) {
        VSF_TEST_TRACE_ERROR("pwm_basic:min_freq is 0 (value=%u)" VSF_TRACE_CFG_LINEEND,
                              (unsigned)cap.min_freq);
    }
    VSF_TEST_ASSERT(cap.min_freq > 0);

    /* Initialize PWM at 1 kHz */
    VSF_TEST_TRACE_DEBUG("PWM:BASIC: vsf_pwm_init" VSF_TRACE_CFG_LINEEND);
    vsf_err_t err = vsf_pwm_init(pwm, &(vsf_pwm_cfg_t){
        .freq = VSF_TEST_PWM_BASIC_FREQ_HZ,
    });
    VSF_TEST_ASSERT_ERR_NONE(err,
        "PWM:BASIC: vsf_pwm_init failed (err=%d) (freq=%lu)"
                             VSF_TRACE_CFG_LINEEND, (int)err, (unsigned long)VSF_TEST_PWM_BASIC_FREQ_HZ);

    /* Set 50% duty: period=100 counts, pulse=50 counts */
    VSF_TEST_TRACE_DEBUG("PWM:BASIC: vsf_pwm_set (ch=%u period=%u pulse=%u)"
                         VSF_TRACE_CFG_LINEEND,
                         (unsigned)VSF_TEST_PWM_BASIC_CHANNEL,
                         (unsigned)VSF_TEST_PWM_BASIC_PERIOD,
                         (unsigned)VSF_TEST_PWM_BASIC_DUTY);
    err = vsf_pwm_set(pwm, VSF_TEST_PWM_BASIC_CHANNEL,
                      VSF_TEST_PWM_BASIC_PERIOD, VSF_TEST_PWM_BASIC_DUTY);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "PWM:BASIC: vsf_pwm_set failed (err=%d) (ch=%u period=%u pulse=%u)"
                             VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)VSF_TEST_PWM_BASIC_CHANNEL, (unsigned)VSF_TEST_PWM_BASIC_PERIOD, (unsigned)VSF_TEST_PWM_BASIC_DUTY);

    /* Enable PWM output */
    VSF_TEST_TRACE_DEBUG("PWM:BASIC: enable" VSF_TRACE_CFG_LINEEND);
    while (fsm_rt_cpl != vsf_pwm_enable(pwm));

    /* Let it run for a short time */
    vsf_test_busy_wait_ms(VSF_TEST_PWM_BASIC_BUSY_WAIT_MS);

    /* Verify get_freq returns non-zero */
    uint32_t freq = vsf_pwm_get_freq(pwm);
    if (freq == 0) {
        VSF_TEST_TRACE_ERROR("PWM:BASIC: vsf_pwm_get_freq returned 0"
                             VSF_TRACE_CFG_LINEEND);
    }
    VSF_TEST_ASSERT(freq > 0);

    /* Disable PWM */
    while (fsm_rt_cpl != vsf_pwm_disable(pwm));

    VSF_TEST_TRACE_INFO("PWM:BASIC: PASS (freq=%u)" VSF_TRACE_CFG_LINEEND,
                        (unsigned)freq);
}

#endif /* VSF_TEST_PWM_BASIC_ENABLE == ENABLED */

/* EOF */


