#ifndef __VSF_TEST_RTC_ALARM_H__
#define __VSF_TEST_RTC_ALARM_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"
#   include "component/test/vsf_test/vsf_test.h"
#   include "test_params_generated.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_RTC_ALARM_CASE_COUNT
#   define VSF_TEST_RTC_ALARM_CASE_COUNT       1
#endif
#ifndef VSF_TEST_RTC_ALARM_TM_MON
#   define VSF_TEST_RTC_ALARM_TM_MON                               1
#endif

#ifndef VSF_TEST_RTC_ALARM_TM_HOUR
#   define VSF_TEST_RTC_ALARM_TM_HOUR                              12
#endif

#ifndef VSF_TEST_RTC_ALARM_TM_MS
#   define VSF_TEST_RTC_ALARM_TM_MS                                0
#endif

#ifndef VSF_TEST_RTC_ALARM_TM_WDAY
#   define VSF_TEST_RTC_ALARM_TM_WDAY                              1
#endif

#ifndef VSF_TEST_RTC_ALARM_TM_SEC
#   define VSF_TEST_RTC_ALARM_TM_SEC                               2
#endif

#ifndef VSF_TEST_RTC_ALARM_TM_YEAR
#   define VSF_TEST_RTC_ALARM_TM_YEAR                              2024
#endif

#ifndef VSF_TEST_RTC_ALARM_TM_MDAY
#   define VSF_TEST_RTC_ALARM_TM_MDAY                              1
#endif

#ifndef VSF_TEST_RTC_ALARM_TM_MIN
#   define VSF_TEST_RTC_ALARM_TM_MIN                               0
#endif

#ifndef VSF_TEST_RTC_ALARM_TIMEOUT_MS
#   define VSF_TEST_RTC_ALARM_TIMEOUT_MS                      3500
#endif

#ifndef VSF_TEST_RTC_ALARM_PRIO
#   define VSF_TEST_RTC_ALARM_PRIO                  vsf_arch_prio_1
#endif



/*============================ TYPES =========================================*/

#if VSF_TEST_RTC_ALARM_ENABLE == ENABLED
typedef struct {
    volatile bool alarm_triggered;
} vsf_test_rtc_alarm_data_t;
#endif

/*============================ PROTOTYPES ====================================*/

void vsf_test_rtc_alarm_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst);

typedef struct {
    uint8_t idx;
    uint8_t rtc_idx;
} vsf_test_rtc_alarm_params_t;

struct vsf_test_rtc_alarm_s;
extern const struct vsf_test_rtc_alarm_s vsf_test_rtc_alarm;

#endif /* __VSF_TEST_RTC_ALARM_H__ */
/* EOF */
