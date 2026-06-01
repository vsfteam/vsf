#ifndef __VSF_TEST_WDT_TIMEOUT_ACCURACY_H__
#define __VSF_TEST_WDT_TIMEOUT_ACCURACY_H__

#include "vsf.h"
#   include "component/test/vsf_test/vsf_test.h"
#   include "test_params_generated.h"

#if VSF_TEST_WDT_TIMEOUT_ACCURACY_ENABLE == ENABLED

#ifndef VSF_TEST_WDT_TIMEOUT_ACCURACY_CASE_COUNT
#   define VSF_TEST_WDT_TIMEOUT_ACCURACY_CASE_COUNT   1
#endif
#ifndef VSF_TEST_WDT_TIMEOUT_ACCURACY_TIMEOUT_MS
#   define VSF_TEST_WDT_TIMEOUT_ACCURACY_TIMEOUT_MS   500
#endif
#ifndef VSF_TEST_WDT_TIMEOUT_ACCURACY_MODE
#   define VSF_TEST_WDT_TIMEOUT_ACCURACY_MODE         (VSF_WDT_MODE_NO_EARLY_WAKEUP | VSF_WDT_MODE_RESET_SOC)
#endif
#ifndef VSF_TEST_WDT_SUPPORT_RESET_SOC
#   define VSF_TEST_WDT_SUPPORT_RESET_SOC             1
#endif
#ifndef VSF_TEST_WDT_TIMEOUT_ACCURACY_TOLERANCE_PCT
#   define VSF_TEST_WDT_TIMEOUT_ACCURACY_TOLERANCE_PCT 10
#endif
#ifndef VSF_TEST_WDT_TIMEOUT_MARGIN_MS
#   define VSF_TEST_WDT_TIMEOUT_MARGIN_MS             100
#endif

#ifdef __cplusplus
extern "C" {
#endif

void vsf_test_wdt_timeout_accuracy_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst);

typedef struct {
    uint8_t  idx;
    uint16_t timeout_ms;
    uint8_t  tolerance_pct;
} vsf_test_wdt_timeout_accuracy_params_t;

struct vsf_test_wdt_timeout_accuracy_s;
extern const struct vsf_test_wdt_timeout_accuracy_s vsf_test_wdt_timeout_accuracy;

#ifdef __cplusplus
}
#endif

#endif /* VSF_TEST_WDT_TIMEOUT_ACCURACY_ENABLE == ENABLED */

#endif /* __VSF_TEST_WDT_TIMEOUT_ACCURACY_H__ */
