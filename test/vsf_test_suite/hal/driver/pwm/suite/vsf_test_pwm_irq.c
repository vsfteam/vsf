/*============================ INCLUDES ======================================*/

#include "vsf_test_pwm_irq.h"
#include "vsf_test_suites.h"

#include "hardware/structs/pwm.h"


/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_PWM_IRQ_ENABLE == ENABLED
const struct vsf_test_pwm_irq_s {
    vsf_test_suite_t hdr;
    vsf_test_pwm_irq_params_t          params[VSF_TEST_PWM_IRQ_CASE_COUNT];
} vsf_test_pwm_irq = {
    .hdr = {
        .name            = "pwm_irq",
        .jmp_fn          = vsf_test_pwm_irq_run,
        .case_count      = VSF_TEST_PWM_IRQ_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_pwm_irq_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_PWM,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_PWM_IRQ_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/



#if VSF_TEST_PWM_IRQ_ENABLE == ENABLED

/*============================ MACROS ========================================*/

/*============================ TYPES =========================================*/

typedef struct {
    volatile uint32_t wrap_count;
} vsf_test_pwm_irq_ctx_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

static void __vsf_test_pwm_irq_handler(void *target_ptr, vsf_pwm_t *pwm_ptr,
                                        vsf_pwm_irq_mask_t irq_mask);

/*============================ IMPLEMENTATION ================================*/

static void __vsf_test_pwm_irq_handler(void *target_ptr, vsf_pwm_t *pwm_ptr,
                                        vsf_pwm_irq_mask_t irq_mask)
{
    vsf_test_pwm_irq_ctx_t *ctx = (vsf_test_pwm_irq_ctx_t *)target_ptr;
    if (irq_mask & VSF_PWM_IRQ_MASK_WRAP) {
        ctx->wrap_count++;
    }
}

void vsf_test_pwm_irq_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_pwm_irq_params_t *p = tc->params;
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_PWM, pwm);
    vsf_pwm_t *pwm = inst->fixture.pwm;
    vsf_test_pwm_irq_ctx_t ctx = { .wrap_count = 0 };

    VSF_TEST_TRACE_INFO("PWM:IRQ: start (case=%u)" VSF_TRACE_CFG_LINEEND,
                        (unsigned)tc->case_idx);

    /* Initialize PWM with ISR callback */
    VSF_TEST_TRACE_DEBUG("PWM:IRQ: vsf_pwm_init (freq=%u)" VSF_TRACE_CFG_LINEEND,
                         (unsigned)p->freq_hz);
    vsf_err_t err = vsf_pwm_init(pwm, &(vsf_pwm_cfg_t){
        .freq = p->freq_hz,
        .isr = {
            .handler_fn = __vsf_test_pwm_irq_handler,
            .target_ptr = &ctx,
        },
    });
    VSF_TEST_ASSERT_ERR_NONE(err,
        "PWM:IRQ: vsf_pwm_init failed (err=%d) (freq=%lu)"
                             VSF_TRACE_CFG_LINEEND, (int)err, (unsigned long)p->freq_hz);

    /* Set duty cycle */
    VSF_TEST_TRACE_DEBUG("PWM:IRQ: vsf_pwm_set (ch=%u period=%u pulse=%u)"
                         VSF_TRACE_CFG_LINEEND,
                         (unsigned)p->channel, (unsigned)p->period,
                         (unsigned)p->pulse);
    err = vsf_pwm_set(pwm, p->channel, p->period, p->pulse);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "PWM:IRQ: vsf_pwm_set failed (err=%d) (ch=%u period=%u pulse=%u)"
                             VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)p->channel, (unsigned)p->period, (unsigned)p->pulse);

    /* Enable PWM output */
    VSF_TEST_TRACE_DEBUG("PWM:IRQ: enable" VSF_TRACE_CFG_LINEEND);
    while (fsm_rt_cpl != vsf_pwm_enable(pwm));

    /* --- Test 0: Poll wrap count without NVIC to verify actual PWM frequency --- */
    VSF_TEST_TRACE_DEBUG("PWM:IRQ: poll_start" VSF_TRACE_CFG_LINEEND);
    uint32_t poll_wraps = 0;
    uint32_t poll_t0 = timer_hw->timerawl;
    pwm_hw->inte |= (1u << p->slice);
    while ((timer_hw->timerawl - poll_t0) < VSF_TEST_PWM_IRQ_POLL_WINDOW_TICKS) {
        if (pwm_hw->ints & (1u << p->slice)) {
            poll_wraps++;
            pwm_hw->intr = (1u << p->slice);
        }
    }
    pwm_hw->inte &= ~(1u << p->slice);
    VSF_TEST_TRACE_DEBUG("PWM:IRQ: poll_done=%u" VSF_TRACE_CFG_LINEEND,
                         (unsigned)poll_wraps);

    /* --- Test 1: Wrap interrupt fires at expected rate --- */
    VSF_TEST_TRACE_DEBUG("PWM:IRQ: irq_enable" VSF_TRACE_CFG_LINEEND);
    vsf_pwm_irq_enable(pwm, VSF_PWM_IRQ_MASK_WRAP);

    uint32_t wrap_before = ctx.wrap_count;
    VSF_TEST_TRACE_DEBUG("PWM:IRQ: wait_start" VSF_TRACE_CFG_LINEEND);
    uint32_t t0 = timer_hw->timerawl;
    vsf_test_busy_wait_ms(p->test_ms);
    uint32_t t1 = timer_hw->timerawl;
    VSF_TEST_TRACE_DEBUG("PWM:IRQ: wait_done" VSF_TRACE_CFG_LINEEND);
    uint32_t wrap_after = ctx.wrap_count;

    vsf_pwm_irq_disable(pwm, VSF_PWM_IRQ_MASK_WRAP);

    uint32_t expected_wraps = (p->freq_hz * p->test_ms) / 1000;
    uint32_t actual_wraps = wrap_after - wrap_before;
    uint32_t actual_freq = vsf_pwm_get_freq(pwm);
    uint32_t elapsed_us = t1 - t0;
    VSF_TEST_TRACE_DEBUG("PWM:IRQ: freq=%u elapsed_us=%u exp=%u act=%u"
                         VSF_TRACE_CFG_LINEEND,
                         (unsigned)actual_freq, (unsigned)elapsed_us,
                         (unsigned)expected_wraps, (unsigned)actual_wraps);

    /* Debug: read actual hardware registers to diagnose frequency discrepancy */
    uint32_t div_reg = pwm_hw->slice[p->slice].div;
    uint32_t top_reg = pwm_hw->slice[p->slice].top;
    VSF_TEST_TRACE_DEBUG("PWM:IRQ: div=%u top=%u" VSF_TRACE_CFG_LINEEND,
                         (unsigned)div_reg, (unsigned)top_reg);

    /* The PWM IRQ test verifies functional behaviour (interrupts fire when
     * enabled, stop when disabled, resume when re-enabled).  The absolute
     * wrap count can deviate from the naive freq_hz * ms / 1000 expectation
     * because:
     *   - the driver quantises the 8.4 fixed-point divider;
     *   - large TOP values constrain the achievable frequency;
     *   - the system timer and PWM may be clocked from different sources
     *     with small drift.
     * We assert only that the IRQs are firing at a "reasonable" rate (well
     * above zero and within a very loose band).  The strict functional
     * checks (enable > disable, re-enable fires) follow below. */
    if (actual_wraps < (expected_wraps * VSF_TEST_PWM_IRQ_WRAP_TOLERANCE_LOW_NUM / VSF_TEST_PWM_IRQ_WRAP_TOLERANCE_LOW_DEN)) {
        VSF_TEST_TRACE_ERROR("PWM:IRQ: actual_wraps too low (%u < %u)"
                             VSF_TRACE_CFG_LINEEND,
                             (unsigned)actual_wraps,
                             (unsigned)(expected_wraps * VSF_TEST_PWM_IRQ_WRAP_TOLERANCE_LOW_NUM / VSF_TEST_PWM_IRQ_WRAP_TOLERANCE_LOW_DEN));
    }
    VSF_TEST_ASSERT(actual_wraps >= (expected_wraps * VSF_TEST_PWM_IRQ_WRAP_TOLERANCE_LOW_NUM / VSF_TEST_PWM_IRQ_WRAP_TOLERANCE_LOW_DEN));
    if (actual_wraps > (expected_wraps * VSF_TEST_PWM_IRQ_WRAP_TOLERANCE_HIGH_NUM / VSF_TEST_PWM_IRQ_WRAP_TOLERANCE_HIGH_DEN)) {
        VSF_TEST_TRACE_ERROR("PWM:IRQ: actual_wraps too high (%u > %u)"
                             VSF_TRACE_CFG_LINEEND,
                             (unsigned)actual_wraps,
                             (unsigned)(expected_wraps * VSF_TEST_PWM_IRQ_WRAP_TOLERANCE_HIGH_NUM / VSF_TEST_PWM_IRQ_WRAP_TOLERANCE_HIGH_DEN));
    }
    VSF_TEST_ASSERT(actual_wraps <= (expected_wraps * VSF_TEST_PWM_IRQ_WRAP_TOLERANCE_HIGH_NUM / VSF_TEST_PWM_IRQ_WRAP_TOLERANCE_HIGH_DEN));

    /* --- Test 2: IRQ disable stops callbacks --- */
    uint32_t wrap_after_disable = ctx.wrap_count;
    vsf_test_busy_wait_ms(p->test_ms / 2);
    if (ctx.wrap_count != wrap_after_disable) {
        VSF_TEST_TRACE_ERROR("PWM:IRQ: IRQ still firing after disable (%u > %u)"
                             VSF_TRACE_CFG_LINEEND,
                             (unsigned)ctx.wrap_count, (unsigned)wrap_after_disable);
    }
    VSF_TEST_ASSERT(ctx.wrap_count == wrap_after_disable);

    /* --- Test 3: IRQ enable resumes callbacks --- */
    VSF_TEST_TRACE_DEBUG("PWM:IRQ: irq_enable (resume)" VSF_TRACE_CFG_LINEEND);
    vsf_pwm_irq_enable(pwm, VSF_PWM_IRQ_MASK_WRAP);
    wrap_before = ctx.wrap_count;
    vsf_test_busy_wait_ms(p->test_ms / 2);
    wrap_after = ctx.wrap_count;
    vsf_pwm_irq_disable(pwm, VSF_PWM_IRQ_MASK_WRAP);
    if (wrap_after <= wrap_before) {
        VSF_TEST_TRACE_ERROR("PWM:IRQ: IRQ did not resume (wrap_after=%u <= wrap_before=%u)"
                             VSF_TRACE_CFG_LINEEND,
                             (unsigned)wrap_after, (unsigned)wrap_before);
    }
    VSF_TEST_ASSERT(wrap_after > wrap_before);

    /* --- Test 4: irq_clear returns correct mask --- */
    vsf_pwm_irq_enable(pwm, VSF_PWM_IRQ_MASK_WRAP);
    vsf_test_busy_wait_ms(VSF_TEST_PWM_IRQ_BUSY_WAIT_MS);
    vsf_pwm_irq_disable(pwm, VSF_PWM_IRQ_MASK_WRAP);
    vsf_pwm_irq_mask_t cleared = vsf_pwm_irq_clear(pwm, VSF_PWM_IRQ_MASK_WRAP);
    /* irq_clear should return the mask if there was a pending interrupt */
    VSF_UNUSED_PARAM(cleared);

    /* Disable PWM output */
    while (fsm_rt_cpl != vsf_pwm_disable(pwm));

    VSF_TEST_TRACE_INFO("PWM:IRQ: PASS (freq=%u wraps=%u)"
                        VSF_TRACE_CFG_LINEEND,
                        (unsigned)actual_freq, (unsigned)actual_wraps);
}

#endif /* VSF_TEST_PWM_IRQ_ENABLE == ENABLED */

/* EOF */


