/*============================ INCLUDES ======================================*/

#include "vsf_test_adc_stream.h"
#include "vsf_test_suites.h"


/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_ADC_STREAM_ENABLE == ENABLED
const struct vsf_test_adc_stream_s {
    vsf_test_suite_t hdr;
    vsf_test_adc_stream_params_t          params[VSF_TEST_ADC_STREAM_CASE_COUNT];
} vsf_test_adc_stream = {
    .hdr = {
        .name            = "adc_stream",
        .jmp_fn          = vsf_test_adc_stream_run,
        .case_count      = VSF_TEST_ADC_STREAM_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_adc_stream_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_ADC,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_ADC_STREAM_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/


/*============================ LOCAL VARIABLES ===============================*/


#if VSF_TEST_ADC_STREAM_ENABLE == ENABLED

static void __adc_isr(void *target_ptr, vsf_adc_t *adc_ptr,

                      vsf_adc_irq_mask_t irq_mask)
{
    VSF_UNUSED_PARAM(adc_ptr);
    vsf_test_suite_t *suite = target_ptr;
    if (irq_mask & VSF_ADC_IRQ_MASK_CPL) {
        vsf_test_suite_data.adc.adc_stream.completed = true;
    }
}



/*============================ MACROS ========================================*/

/*============================ LOCAL VARIABLES ===============================*/


/*============================ IMPLEMENTATION ================================*/

void vsf_test_adc_stream_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_adc_stream_params_t *p = tc->params;
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_ADC, adc);
    vsf_adc_t *adc = inst->fixture.adc;

    VSF_TEST_TRACE_INFO("adc_stream:start (case=%u)" VSF_TRACE_CFG_LINEEND,
                        (unsigned)tc->case_idx);

    vsf_adc_capability_t cap = vsf_adc_capability(adc);
    if (cap.max_data_bits != VSF_TEST_ADC_STREAM_EXPECTED_RESOLUTION_BITS) {
        VSF_TEST_TRACE_ERROR("adc_stream:max_data_bits mismatch (expected=%u actual=%u)"
                             VSF_TRACE_CFG_LINEEND,
                             (unsigned)VSF_TEST_ADC_STREAM_EXPECTED_RESOLUTION_BITS,
                             (unsigned)cap.max_data_bits);
    }
    VSF_TEST_ASSERT(cap.max_data_bits == VSF_TEST_ADC_STREAM_EXPECTED_RESOLUTION_BITS);
    if (cap.channel_count < VSF_TEST_ADC_STREAM_MIN_CHANNEL_COUNT) {
        VSF_TEST_TRACE_ERROR("adc_stream:channel_count too low (min=%u actual=%u)"
                             VSF_TRACE_CFG_LINEEND,
                             (unsigned)VSF_TEST_ADC_STREAM_MIN_CHANNEL_COUNT,
                             (unsigned)cap.channel_count);
    }
    VSF_TEST_ASSERT(cap.channel_count >= VSF_TEST_ADC_STREAM_MIN_CHANNEL_COUNT);

    VSF_TEST_TRACE_DEBUG("adc_stream:vsf_adc_init" VSF_TRACE_CFG_LINEEND);
    vsf_adc_cfg_t cfg = {
        .mode     = VSF_TEST_ADC_STREAM_MODE,
        .isr      = {
            .handler_fn = __adc_isr,
            .target_ptr = NULL,
            .prio       = VSF_TEST_ADC_STREAM_PRIO,
        },
        .clock_hz = VSF_TEST_ADC_CLOCK_HZ,
    };
    vsf_err_t err = vsf_adc_init(adc, &cfg);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "adc_stream:vsf_adc_init failed (err=%d) (precision_bits=%u)"
                             VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)cap.max_data_bits);

    VSF_TEST_TRACE_DEBUG("adc_stream:vsf_adc_enable" VSF_TRACE_CFG_LINEEND);
    while (fsm_rt_cpl != vsf_adc_enable(adc));

    /* --- Test 1: Stream 100 samples from temp sensor --- */
    vsf_test_suite_data.adc.adc_stream.completed = false;

    VSF_TEST_TRACE_DEBUG("adc_stream:vsf_adc_channel_config (ch=%u, count=%u)"
                         VSF_TRACE_CFG_LINEEND,
                         (unsigned)VSF_TEST_ADC_STREAM_CHANNEL,
                         (unsigned)1);
    vsf_adc_channel_cfg_t ch_cfg = {
        .channel       = VSF_TEST_ADC_STREAM_CHANNEL,
        .mode          = VSF_TEST_ADC_STREAM_CHANNEL_MODE,
        .sample_cycles = VSF_TEST_ADC_STREAM_SAMPLE_CYCLES,
    };
    err = vsf_adc_channel_config(adc, &ch_cfg, 1);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "adc_stream:vsf_adc_channel_config failed (err=%d) (ch=%u)"
                             VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)ch_cfg.channel);

    uint16_t samples[VSF_TEST_ADC_STREAM_SAMPLE_COUNT] = {0};
    VSF_TEST_TRACE_DEBUG("adc_stream:vsf_adc_channel_request (count=%u)"
                         VSF_TRACE_CFG_LINEEND,
                         (unsigned)VSF_TEST_ADC_STREAM_SAMPLE_COUNT);
    err = vsf_adc_channel_request(adc, samples, VSF_TEST_ADC_STREAM_SAMPLE_COUNT);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "adc_stream:vsf_adc_channel_request failed (err=%d) (ch=%u sample_count=%u)"
                             VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)ch_cfg.channel, (unsigned)VSF_TEST_ADC_STREAM_SAMPLE_COUNT);

    VSF_TEST_TRACE_DEBUG("adc_stream:waiting for completion" VSF_TRACE_CFG_LINEEND);
    VSF_TEST_WAIT_FOR(vsf_test_suite_data.adc.adc_stream.completed, VSF_TEST_ADC_STREAM_TIMEOUT_MS);
    if (!vsf_test_suite_data.adc.adc_stream.completed) {
        VSF_TEST_TRACE_ERROR("adc_stream:stream timeout after %u ms"
                             VSF_TRACE_CFG_LINEEND,
                             (unsigned)VSF_TEST_ADC_STREAM_TIMEOUT_MS);
    }
    VSF_TEST_ASSERT(vsf_test_suite_data.adc.adc_stream.completed);

    for (uint32_t i = 0; i < VSF_TEST_ADC_STREAM_SAMPLE_COUNT; i++) {
        if (samples[i] > VSF_TEST_ADC_STREAM_MAX_SAMPLE) {
            VSF_TEST_TRACE_ERROR("adc_stream:sample[%u] out of range (value=0x%03x max=0x%03x)"
                                 VSF_TRACE_CFG_LINEEND,
                                 (unsigned)i, (unsigned)samples[i],
                                 (unsigned)VSF_TEST_ADC_STREAM_MAX_SAMPLE);
        }
        VSF_TEST_ASSERT(samples[i] <= VSF_TEST_ADC_STREAM_MAX_SAMPLE);
    }

    /* --- Test 2: Rapid fire 1-sample requests --- */
    for (uint32_t i = 0; i < VSF_TEST_ADC_STREAM_RAPID_COUNT; i++) {
        vsf_test_suite_data.adc.adc_stream.completed = false;
        uint16_t single = 0;
        VSF_TEST_TRACE_DEBUG("adc_stream:vsf_adc_channel_request (rapid #%u)"
                             VSF_TRACE_CFG_LINEEND,
                             (unsigned)i);
        err = vsf_adc_channel_request(adc, &single, 1);
        VSF_TEST_ASSERT_ERR_NONE(err,
            "adc_stream:vsf_adc_channel_request failed (err=%d) (ch=%u sample_count=%u)"
                                 VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)ch_cfg.channel, (unsigned)1);

        VSF_TEST_TRACE_DEBUG("adc_stream:waiting for rapid #%u" VSF_TRACE_CFG_LINEEND,
                             (unsigned)i);
        VSF_TEST_WAIT_FOR(vsf_test_suite_data.adc.adc_stream.completed, VSF_TEST_ADC_STREAM_RAPID_TIMEOUT_MS);
        if (!vsf_test_suite_data.adc.adc_stream.completed) {
            VSF_TEST_TRACE_ERROR("adc_stream:rapid #%u timeout after %u ms"
                                 VSF_TRACE_CFG_LINEEND,
                                 (unsigned)i,
                                 (unsigned)VSF_TEST_ADC_STREAM_RAPID_TIMEOUT_MS);
        }
        VSF_TEST_ASSERT(vsf_test_suite_data.adc.adc_stream.completed);
        if (single > VSF_TEST_ADC_STREAM_MAX_SAMPLE) {
            VSF_TEST_TRACE_ERROR("adc_stream:rapid #%u sample out of range (value=0x%03x max=0x%03x)"
                                 VSF_TRACE_CFG_LINEEND,
                                 (unsigned)i, (unsigned)single,
                                 (unsigned)VSF_TEST_ADC_STREAM_MAX_SAMPLE);
        }
        VSF_TEST_ASSERT(single <= VSF_TEST_ADC_STREAM_MAX_SAMPLE);
    }

    VSF_TEST_TRACE_INFO("adc_stream:pass" VSF_TRACE_CFG_LINEEND);

    while (fsm_rt_cpl != vsf_adc_disable(adc));
}

#endif /* VSF_TEST_ADC_STREAM_ENABLE == ENABLED */

/* EOF */


