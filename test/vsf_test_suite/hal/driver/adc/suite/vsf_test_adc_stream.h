#ifndef __VSF_TEST_ADC_STREAM_H__
#define __VSF_TEST_ADC_STREAM_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"
#   include "component/test/vsf_test/vsf_test.h"
#   include "test_params_generated.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_ADC_STREAM_MODE
#   define VSF_TEST_ADC_STREAM_MODE        (VSF_ADC_REF_VDD_1 | VSF_ADC_DATA_ALIGN_RIGHT | VSF_ADC_SCAN_CONV_SINGLE_MODE)
#endif

#ifndef VSF_TEST_ADC_STREAM_CHANNEL_MODE
#   define VSF_TEST_ADC_STREAM_CHANNEL_MODE (VSF_ADC_CHANNEL_GAIN_1 | VSF_ADC_CHANNEL_REF_VDD_1)
#endif

#ifndef VSF_TEST_ADC_STREAM_CASE_COUNT
#   define VSF_TEST_ADC_STREAM_CASE_COUNT   1
#endif
#ifndef VSF_TEST_ADC_STREAM_TIMEOUT_MS
#   define VSF_TEST_ADC_STREAM_TIMEOUT_MS                     500
#endif

#ifndef VSF_TEST_ADC_STREAM_RAPID_TIMEOUT_MS
#   define VSF_TEST_ADC_STREAM_RAPID_TIMEOUT_MS                100
#endif

#ifndef VSF_TEST_ADC_STREAM_EXPECTED_RESOLUTION_BITS
#   define VSF_TEST_ADC_STREAM_EXPECTED_RESOLUTION_BITS  12
#endif

#ifndef VSF_TEST_ADC_STREAM_MAX_SAMPLE
#   define VSF_TEST_ADC_STREAM_MAX_SAMPLE               0x0FFF
#endif

#ifndef VSF_TEST_ADC_STREAM_MIN_CHANNEL_COUNT
#   define VSF_TEST_ADC_STREAM_MIN_CHANNEL_COUNT         4
#endif

#ifndef VSF_TEST_ADC_STREAM_CHANNEL
#   define VSF_TEST_ADC_STREAM_CHANNEL                  4
#endif

#ifndef VSF_TEST_ADC_STREAM_SAMPLE_CYCLES
#   define VSF_TEST_ADC_STREAM_SAMPLE_CYCLES            0
#endif

#ifndef VSF_TEST_ADC_STREAM_PRIO
#   define VSF_TEST_ADC_STREAM_PRIO                 vsf_arch_prio_1
#endif


#ifndef VSF_TEST_ADC_CLOCK_HZ
#   define VSF_TEST_ADC_CLOCK_HZ            48000000
#endif

#ifndef VSF_TEST_ADC_STREAM_SAMPLE_COUNT
#   define VSF_TEST_ADC_STREAM_SAMPLE_COUNT 100
#endif

#ifndef VSF_TEST_ADC_STREAM_RAPID_COUNT
#   define VSF_TEST_ADC_STREAM_RAPID_COUNT  10
#endif


/*============================ TYPES =========================================*/

#if VSF_TEST_ADC_STREAM_ENABLE == ENABLED
typedef struct {
    volatile bool completed;
} vsf_test_adc_stream_data_t;
#endif

/*============================ PROTOTYPES ====================================*/

void vsf_test_adc_stream_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst);

typedef struct {
    uint8_t  idx;
    uint8_t  channel;
    uint16_t sample_count;
} vsf_test_adc_stream_params_t;

struct vsf_test_adc_stream_s;
extern const struct vsf_test_adc_stream_s vsf_test_adc_stream;

#endif /* __VSF_TEST_ADC_STREAM_H__ */
/* EOF */
