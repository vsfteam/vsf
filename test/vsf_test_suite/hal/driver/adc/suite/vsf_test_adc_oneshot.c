/*============================ INCLUDES ======================================*/

#include "vsf_test_adc_oneshot.h"
#include "vsf_test_suites.h"


/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_ADC_ONESHOT_ENABLE == ENABLED
const struct vsf_test_adc_oneshot_s {
    vsf_test_suite_t hdr;
    vsf_test_adc_oneshot_params_t          params[VSF_TEST_ADC_ONESHOT_CASE_COUNT];
} vsf_test_adc_oneshot = {
    .hdr = {
        .name            = "adc_oneshot",
        .jmp_fn          = vsf_test_adc_oneshot_run,
        .case_count      = VSF_TEST_ADC_ONESHOT_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_adc_oneshot_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_ADC,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_ADC_ONESHOT_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/



#if VSF_TEST_ADC_ONESHOT_ENABLE == ENABLED

/*============================ MACROS ========================================*/


/*============================ LOCAL VARIABLES ===============================*/

static volatile bool __adc_oneshot_completed = false;

/*============================ IMPLEMENTATION ================================*/

static void __adc_oneshot_isr(void *target_ptr, vsf_adc_t *adc_ptr,
                              vsf_adc_irq_mask_t irq_mask)
{
    VSF_UNUSED_PARAM(target_ptr);
    VSF_UNUSED_PARAM(adc_ptr);
    if (irq_mask & VSF_ADC_IRQ_MASK_CPL) {
        __adc_oneshot_completed = true;
    }
}

void vsf_test_adc_oneshot_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_adc_oneshot_params_t *p = tc->params;
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_ADC, adc);
    vsf_adc_t *adc = inst->fixture.adc;

    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware suites do not print them. */

    VSF_TEST_TRACE_INFO("adc_oneshot:start (case=%u)" VSF_TRACE_CFG_LINEEND,
                        (unsigned)tc->case_idx);

    // Verify capability
    vsf_adc_capability_t cap = vsf_adc_capability(adc);
    if (cap.max_data_bits != VSF_TEST_ADC_ONESHOT_EXPECTED_RESOLUTION_BITS) {
        VSF_TEST_TRACE_ERROR("adc_oneshot:max_data_bits mismatch (expected=%u actual=%u)"
                             VSF_TRACE_CFG_LINEEND,
                             (unsigned)VSF_TEST_ADC_ONESHOT_EXPECTED_RESOLUTION_BITS,
                             (unsigned)cap.max_data_bits);
    }
    VSF_TEST_ASSERT(cap.max_data_bits == VSF_TEST_ADC_ONESHOT_EXPECTED_RESOLUTION_BITS);
    if (cap.channel_count < VSF_TEST_ADC_ONESHOT_MIN_CHANNEL_COUNT) {
        VSF_TEST_TRACE_ERROR("adc_oneshot:channel_count too low (min=%u actual=%u)"
                             VSF_TRACE_CFG_LINEEND,
                             (unsigned)VSF_TEST_ADC_ONESHOT_MIN_CHANNEL_COUNT,
                             (unsigned)cap.channel_count);
    }
    VSF_TEST_ASSERT(cap.channel_count >= VSF_TEST_ADC_ONESHOT_MIN_CHANNEL_COUNT);

    // Init ADC
    VSF_TEST_TRACE_DEBUG("adc_oneshot:vsf_adc_init" VSF_TRACE_CFG_LINEEND);
    vsf_adc_cfg_t cfg = {
        .mode     = VSF_TEST_ADC_ONESHOT_MODE,
        .isr      = {
            .handler_fn = __adc_oneshot_isr,
            .target_ptr = NULL,
            .prio       = VSF_TEST_ADC_ONESHOT_PRIO,
        },
        .clock_hz = VSF_TEST_ADC_CLOCK_HZ,
    };
    vsf_err_t err = vsf_adc_init(adc, &cfg);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "adc_oneshot:vsf_adc_init failed (err=%d) (precision_bits=%u)"
                             VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)cap.max_data_bits);

    // Enable ADC
    VSF_TEST_TRACE_DEBUG("adc_oneshot:vsf_adc_enable" VSF_TRACE_CFG_LINEEND);
    while (fsm_rt_cpl != vsf_adc_enable(adc));

    // Sample on channel 0 (GPIO26) — oneshot, verify 12-bit range
    VSF_TEST_TRACE_DEBUG("adc_oneshot:vsf_adc_channel_request_once (ch=%u)"
                         VSF_TRACE_CFG_LINEEND,
                         (unsigned)VSF_TEST_ADC_ONESHOT_CHANNEL);
    vsf_adc_channel_cfg_t ch_cfg = {
        .channel       = VSF_TEST_ADC_ONESHOT_CHANNEL,
        .mode          = VSF_TEST_ADC_ONESHOT_CHANNEL_MODE,
        .sample_cycles = VSF_TEST_ADC_ONESHOT_SAMPLE_CYCLES,
    };
    uint16_t sample = 0;
    __adc_oneshot_completed = false;
    err = vsf_adc_channel_request_once(adc, &ch_cfg, &sample);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "adc_oneshot:vsf_adc_channel_request_once failed (err=%d) (ch=%u)"
                             VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)ch_cfg.channel);

    // vsf_adc_channel_request_once is asynchronous: wait for the conversion-complete
    // interrupt (VSF_ADC_IRQ_MASK_CPL) before reading the result.
    VSF_TEST_TRACE_DEBUG("adc_oneshot:waiting for completion" VSF_TRACE_CFG_LINEEND);
    VSF_TEST_WAIT_FOR(__adc_oneshot_completed, VSF_TEST_ADC_ONESHOT_TIMEOUT_MS);
    if (!__adc_oneshot_completed) {
        VSF_TEST_TRACE_ERROR("adc_oneshot:conversion timeout after %u ms"
                             VSF_TRACE_CFG_LINEEND,
                             (unsigned)VSF_TEST_ADC_ONESHOT_TIMEOUT_MS);
    }
    VSF_TEST_ASSERT(__adc_oneshot_completed);

    // Result must be within 12-bit range
    if (sample > VSF_TEST_ADC_ONESHOT_MAX_SAMPLE) {
        VSF_TEST_TRACE_ERROR("adc_oneshot:sample out of range (sample=0x%03x max=0x%03x)"
                             VSF_TRACE_CFG_LINEEND,
                             (unsigned)sample,
                             (unsigned)VSF_TEST_ADC_ONESHOT_MAX_SAMPLE);
    }
    VSF_TEST_ASSERT(sample <= VSF_TEST_ADC_ONESHOT_MAX_SAMPLE);

    VSF_TEST_TRACE_INFO("adc_oneshot:pass sample=0x%03x" VSF_TRACE_CFG_LINEEND,
                        (unsigned)sample);

    // Disable ADC
    while (fsm_rt_cpl != vsf_adc_disable(adc));
}

#endif /* VSF_TEST_ADC_ONESHOT_ENABLE == ENABLED */

/* EOF */


