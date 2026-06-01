/*============================ INCLUDES ======================================*/

#include "vsf_test_adc_temperature.h"
#include "vsf_test_suites.h"


/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_ADC_TEMPERATURE_ENABLE == ENABLED
const struct vsf_test_adc_temperature_s {
    vsf_test_suite_t hdr;
    vsf_test_adc_temperature_params_t          params[VSF_TEST_ADC_TEMPERATURE_CASE_COUNT];
} vsf_test_adc_temperature = {
    .hdr = {
        .name            = "adc_temperature",
        .jmp_fn          = vsf_test_adc_temperature_run,
        .case_count      = VSF_TEST_ADC_TEMPERATURE_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_adc_temperature_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_ADC,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_ADC_TEMPERATURE_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/



#if VSF_TEST_ADC_TEMPERATURE_ENABLE == ENABLED

/*============================ MACROS ========================================*/

/* Temperature sensor raw range at room temperature (~15-35°C).
 * Values are per-chip and read from test_params YAML. */

/*============================ LOCAL VARIABLES ===============================*/

/*============================ IMPLEMENTATION ================================*/

void vsf_test_adc_temperature_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_adc_temperature_params_t *p = tc->params;
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_ADC, adc);
    vsf_adc_t *adc = inst->fixture.adc;

    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware suites do not print them. */

    VSF_TEST_TRACE_INFO("adc_temperature:start (case=%u)" VSF_TRACE_CFG_LINEEND,
                        (unsigned)tc->case_idx);

    vsf_adc_capability_t cap = vsf_adc_capability(adc);
    if (cap.max_data_bits != VSF_TEST_ADC_TEMPERATURE_EXPECTED_RESOLUTION_BITS) {
        VSF_TEST_TRACE_ERROR("adc_temperature:max_data_bits mismatch (expected=%u actual=%u)"
                             VSF_TRACE_CFG_LINEEND,
                             (unsigned)VSF_TEST_ADC_TEMPERATURE_EXPECTED_RESOLUTION_BITS,
                             (unsigned)cap.max_data_bits);
    }
    VSF_TEST_ASSERT(cap.max_data_bits == VSF_TEST_ADC_TEMPERATURE_EXPECTED_RESOLUTION_BITS);

    VSF_TEST_TRACE_DEBUG("adc_temperature:vsf_adc_init" VSF_TRACE_CFG_LINEEND);
    vsf_adc_cfg_t cfg = {
        .mode     = VSF_TEST_ADC_TEMPERATURE_MODE,
        .isr      = {NULL, NULL, 0},
        .clock_hz = VSF_TEST_ADC_CLOCK_HZ,
    };
    vsf_err_t err = vsf_adc_init(adc, &cfg);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "adc_temperature:vsf_adc_init failed (err=%d) (precision_bits=%u)"
                             VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)cap.max_data_bits);

    VSF_TEST_TRACE_DEBUG("adc_temperature:vsf_adc_enable" VSF_TRACE_CFG_LINEEND);
    while (fsm_rt_cpl != vsf_adc_enable(adc));

    /* Sample internal temperature sensor. */
    VSF_TEST_TRACE_DEBUG("adc_temperature:vsf_adc_channel_request_once (ch=%u)"
                         VSF_TRACE_CFG_LINEEND,
                         (unsigned)p->sensor_channel);
    vsf_adc_channel_cfg_t ch_cfg = {
        .channel       = p->sensor_channel,
        .mode          = VSF_TEST_ADC_TEMPERATURE_CHANNEL_MODE,
        .sample_cycles = VSF_TEST_ADC_TEMPERATURE_SAMPLE_CYCLES,
    };
    uint16_t sample = 0;
    err = vsf_adc_channel_request_once(adc, &ch_cfg, &sample);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "adc_temperature:vsf_adc_channel_request_once failed (err=%d) (ch=%u)"
                             VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)ch_cfg.channel);

    /* Verify raw value is within 12-bit range. Temperature varies by
     * environment and VREF tolerance, so no tight bounds. */
    if (sample > VSF_TEST_ADC_TEMPERATURE_MAX_SAMPLE) {
        VSF_TEST_TRACE_ERROR("adc_temperature:sample out of range (sample=0x%03x max=0x%03x)"
                             VSF_TRACE_CFG_LINEEND,
                             (unsigned)sample,
                             (unsigned)VSF_TEST_ADC_TEMPERATURE_MAX_SAMPLE);
    }
    VSF_TEST_ASSERT(sample <= VSF_TEST_ADC_TEMPERATURE_MAX_SAMPLE);

    VSF_TEST_TRACE_INFO("adc_temperature:pass sample=0x%03x" VSF_TRACE_CFG_LINEEND,
                        (unsigned)sample);

    while (fsm_rt_cpl != vsf_adc_disable(adc));
}

#endif /* VSF_TEST_ADC_TEMPERATURE_ENABLE == ENABLED */

/* EOF */


