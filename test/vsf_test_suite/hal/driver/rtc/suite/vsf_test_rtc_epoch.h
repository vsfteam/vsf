#ifndef __VSF_TEST_RTC_EPOCH_H__
#define __VSF_TEST_RTC_EPOCH_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"
#   include "component/test/vsf_test/vsf_test.h"
#   include "test_params_generated.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_RTC_EPOCH_CASE_COUNT
#   define VSF_TEST_RTC_EPOCH_CASE_COUNT     1
#endif
#ifndef VSF_TEST_RTC_EPOCH_BUSY_WAIT_MS
#   define VSF_TEST_RTC_EPOCH_BUSY_WAIT_MS                    1100
#endif

#ifndef VSF_TEST_RTC_EPOCH_SET_SECONDS
#   define VSF_TEST_RTC_EPOCH_SET_SECONDS                     1700000000
#endif

#ifndef VSF_TEST_RTC_EPOCH_MS_IRRELEVANT
#   define VSF_TEST_RTC_EPOCH_MS_IRRELEVANT                   0xFF
#endif


/*============================ PROTOTYPES ====================================*/

void vsf_test_rtc_epoch_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst);

typedef struct {
    uint8_t idx;
    uint8_t rtc_idx;
} vsf_test_rtc_epoch_params_t;

struct vsf_test_rtc_epoch_s;
extern const struct vsf_test_rtc_epoch_s vsf_test_rtc_epoch;

#endif /* __VSF_TEST_RTC_EPOCH_H__ */
/* EOF */
