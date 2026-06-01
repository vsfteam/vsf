#ifndef __VSF_TEST_RTC_SET_GET_H__
#define __VSF_TEST_RTC_SET_GET_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"
#   include "component/test/vsf_test/vsf_test.h"
#   include "test_params_generated.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_RTC_SET_GET_CASE_COUNT
#   define VSF_TEST_RTC_SET_GET_CASE_COUNT     1
#endif
#ifndef VSF_TEST_RTC_SET_GET_TM_MON
#   define VSF_TEST_RTC_SET_GET_TM_MON                             1
#endif

#ifndef VSF_TEST_RTC_SET_GET_TM_HOUR
#   define VSF_TEST_RTC_SET_GET_TM_HOUR                            12
#endif

#ifndef VSF_TEST_RTC_SET_GET_TM_SEC
#   define VSF_TEST_RTC_SET_GET_TM_SEC                             0
#endif

#ifndef VSF_TEST_RTC_SET_GET_TM_MS
#   define VSF_TEST_RTC_SET_GET_TM_MS                              0
#endif

#ifndef VSF_TEST_RTC_SET_GET_TM_WDAY
#   define VSF_TEST_RTC_SET_GET_TM_WDAY                            1
#endif

#ifndef VSF_TEST_RTC_SET_GET_TM_YEAR
#   define VSF_TEST_RTC_SET_GET_TM_YEAR                            2024
#endif

#ifndef VSF_TEST_RTC_SET_GET_TM_MDAY
#   define VSF_TEST_RTC_SET_GET_TM_MDAY                            1
#endif

#ifndef VSF_TEST_RTC_SET_GET_TM_MIN
#   define VSF_TEST_RTC_SET_GET_TM_MIN                             0
#endif

#ifndef VSF_TEST_RTC_SET_GET_BUSY_WAIT_MS
#   define VSF_TEST_RTC_SET_GET_BUSY_WAIT_MS                  100
#endif


/*============================ PROTOTYPES ====================================*/

void vsf_test_rtc_set_get_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst);

typedef struct {
    uint8_t idx;
    uint8_t rtc_idx;
} vsf_test_rtc_set_get_params_t;

struct vsf_test_rtc_set_get_s;
extern const struct vsf_test_rtc_set_get_s vsf_test_rtc_set_get;

#endif /* __VSF_TEST_RTC_SET_GET_H__ */
/* EOF */
