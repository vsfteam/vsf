/*============================ INCLUDES ======================================*/

#include "vsf_test_pwm_dual_channel.h"
#include "vsf_test_suites.h"


/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_PWM_DUAL_CHANNEL_ENABLE == ENABLED
const struct vsf_test_pwm_dual_channel_s {
    vsf_test_suite_t hdr;
    vsf_test_pwm_dual_channel_params_t          params[VSF_TEST_PWM_DUAL_CHANNEL_CASE_COUNT];
} vsf_test_pwm_dual_channel = {
    .hdr = {
        .name            = "pwm_dual_channel",
        .jmp_fn          = vsf_test_pwm_dual_channel_run,
        .case_count      = VSF_TEST_PWM_DUAL_CHANNEL_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_pwm_dual_channel_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_PWM,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_PWM_DUAL_CHANNEL_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/



#if VSF_TEST_PWM_DUAL_CHANNEL_ENABLE == ENABLED

/*============================ MACROS ========================================*/


/*============================ LOCAL VARIABLES ===============================*/

/*============================ IMPLEMENTATION ================================*/

void vsf_test_pwm_dual_channel_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_pwm_dual_channel_params_t *p = tc->params;
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_PWM, pwm);
    vsf_pwm_t *pwm = inst->fixture.pwm;

    VSF_TEST_TRACE_INFO("PWM:DUAL_CHANNEL: start (case=%u)" VSF_TRACE_CFG_LINEEND,
                        (unsigned)tc->case_idx);

    /* Test capability */
    vsf_pwm_capability_t cap = vsf_pwm_capability(pwm);
    if (cap.max_freq == 0) {
        VSF_TEST_TRACE_ERROR("pwm_dual_channel:max_freq is 0 (value=%u)" VSF_TRACE_CFG_LINEEND,
                              (unsigned)cap.max_freq);
    }
    VSF_TEST_ASSERT(cap.max_freq > 0);
    if (cap.min_freq == 0) {
        VSF_TEST_TRACE_ERROR("pwm_dual_channel:min_freq is 0 (value=%u)" VSF_TRACE_CFG_LINEEND,
                              (unsigned)cap.min_freq);
    }
    VSF_TEST_ASSERT(cap.min_freq > 0);

    /* Initialize PWM at 1 kHz */
    VSF_TEST_TRACE_DEBUG("PWM:DUAL_CHANNEL: vsf_pwm_init" VSF_TRACE_CFG_LINEEND);
    vsf_err_t err = vsf_pwm_init(pwm, &(vsf_pwm_cfg_t){
        .freq = VSF_TEST_PWM_DUAL_CHANNEL_FREQ_HZ,
    });
    VSF_TEST_ASSERT_ERR_NONE(err,
        "PWM:DUAL_CHANNEL: vsf_pwm_init failed (err=%d) (freq=%lu)"
                             VSF_TRACE_CFG_LINEEND, (int)err, (unsigned long)VSF_TEST_PWM_DUAL_CHANNEL_FREQ_HZ);

    /* Channel A: 50% duty (period=100, pulse=50) */
    VSF_TEST_TRACE_DEBUG("PWM:DUAL_CHANNEL: vsf_pwm_set ch=A (period=%u pulse=%u)"
                         VSF_TRACE_CFG_LINEEND,
                         (unsigned)VSF_TEST_PWM_DUAL_CHANNEL_PERIOD,
                         (unsigned)VSF_TEST_PWM_DUAL_CHANNEL_DUTY_A);
    err = vsf_pwm_set(pwm, VSF_TEST_PWM_DUAL_CHANNEL_CHANNEL_A,
                      VSF_TEST_PWM_DUAL_CHANNEL_PERIOD,
                      VSF_TEST_PWM_DUAL_CHANNEL_DUTY_A);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "PWM:DUAL_CHANNEL: vsf_pwm_set ch=A failed (err=%d) (ch=%u period=%u pulse=%u)"
                             VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)VSF_TEST_PWM_DUAL_CHANNEL_CHANNEL_A, (unsigned)VSF_TEST_PWM_DUAL_CHANNEL_PERIOD, (unsigned)VSF_TEST_PWM_DUAL_CHANNEL_DUTY_A);

    /* Channel B: 25% duty (period=100, pulse=25) */
    VSF_TEST_TRACE_DEBUG("PWM:DUAL_CHANNEL: vsf_pwm_set ch=B (period=%u pulse=%u)"
                         VSF_TRACE_CFG_LINEEND,
                         (unsigned)VSF_TEST_PWM_DUAL_CHANNEL_PERIOD,
                         (unsigned)VSF_TEST_PWM_DUAL_CHANNEL_DUTY_B);
    err = vsf_pwm_set(pwm, VSF_TEST_PWM_DUAL_CHANNEL_CHANNEL_B,
                      VSF_TEST_PWM_DUAL_CHANNEL_PERIOD,
                      VSF_TEST_PWM_DUAL_CHANNEL_DUTY_B);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "PWM:DUAL_CHANNEL: vsf_pwm_set ch=B failed (err=%d) (ch=%u period=%u pulse=%u)"
                             VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)VSF_TEST_PWM_DUAL_CHANNEL_CHANNEL_B, (unsigned)VSF_TEST_PWM_DUAL_CHANNEL_PERIOD, (unsigned)VSF_TEST_PWM_DUAL_CHANNEL_DUTY_B);

    /* Enable PWM output */
    VSF_TEST_TRACE_DEBUG("PWM:DUAL_CHANNEL: enable" VSF_TRACE_CFG_LINEEND);
    while (fsm_rt_cpl != vsf_pwm_enable(pwm));

    /* Let it run for a short time */
    vsf_test_busy_wait_ms(VSF_TEST_PWM_DUAL_CHANNEL_BUSY_WAIT_MS);

    /* Verify get_freq returns non-zero */
    uint32_t freq = vsf_pwm_get_freq(pwm);
    if (freq == 0) {
        VSF_TEST_TRACE_ERROR("PWM:DUAL_CHANNEL: vsf_pwm_get_freq returned 0"
                             VSF_TRACE_CFG_LINEEND);
    }
    VSF_TEST_ASSERT(freq > 0);

    /* Disable PWM */
    while (fsm_rt_cpl != vsf_pwm_disable(pwm));

    VSF_TEST_TRACE_INFO("PWM:DUAL_CHANNEL: PASS (freq=%u)" VSF_TRACE_CFG_LINEEND,
                        (unsigned)freq);
}

#endif /* VSF_TEST_PWM_DUAL_CHANNEL_ENABLE == ENABLED */

/* EOF */


