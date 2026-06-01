#ifndef __TEST_ADC_TEMPERATURE_H__
#define __TEST_ADC_TEMPERATURE_H__

#include "vsf.h"
#   include "component/test/vsf_test/vsf_test.h"
#   include "test_params_generated.h"

#if VSF_TEST_ADC_TEMPERATURE_ENABLE == ENABLED

#ifndef VSF_TEST_ADC_TEMPERATURE_MODE
#   define VSF_TEST_ADC_TEMPERATURE_MODE        (VSF_ADC_REF_VDD_1 | VSF_ADC_DATA_ALIGN_RIGHT | VSF_ADC_SCAN_CONV_SINGLE_MODE)
#endif

#ifndef VSF_TEST_ADC_TEMPERATURE_CHANNEL_MODE
#   define VSF_TEST_ADC_TEMPERATURE_CHANNEL_MODE (VSF_ADC_CHANNEL_GAIN_1 | VSF_ADC_CHANNEL_REF_VDD_1)
#endif

#ifndef VSF_TEST_ADC_TEMPERATURE_CASE_COUNT
#   define VSF_TEST_ADC_TEMPERATURE_CASE_COUNT     1
#endif
#ifndef VSF_TEST_ADC_TEMPERATURE_EXPECTED_RESOLUTION_BITS
#   define VSF_TEST_ADC_TEMPERATURE_EXPECTED_RESOLUTION_BITS 12
#endif

#ifndef VSF_TEST_ADC_TEMPERATURE_MAX_SAMPLE
#   define VSF_TEST_ADC_TEMPERATURE_MAX_SAMPLE               0x0FFF
#endif

#ifndef VSF_TEST_ADC_TEMPERATURE_SAMPLE_CYCLES
#   define VSF_TEST_ADC_TEMPERATURE_SAMPLE_CYCLES             0
#endif


#ifndef VSF_TEST_ADC_CLOCK_HZ
#   define VSF_TEST_ADC_CLOCK_HZ                   48000000
#endif
#ifdef __cplusplus
extern "C" {
#endif

/*============================ PROTOTYPES ====================================*/

void vsf_test_adc_temperature_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst);

typedef struct {
    uint8_t  idx;
    uint8_t  channel_count;
    uint8_t  sensor_channel;
    uint16_t temp_raw_min;
    uint16_t temp_raw_max;
} vsf_test_adc_temperature_params_t;

struct vsf_test_adc_temperature_s;
extern const struct vsf_test_adc_temperature_s vsf_test_adc_temperature;

#ifdef __cplusplus
}
#endif

#endif /* VSF_TEST_ADC_TEMPERATURE_ENABLE == ENABLED */

#endif /* __TEST_ADC_TEMPERATURE_H__ */
/* EOF */
