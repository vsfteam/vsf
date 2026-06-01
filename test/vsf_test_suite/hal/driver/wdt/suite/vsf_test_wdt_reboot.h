#ifndef __TEST_WDT_REBOOT_H__
#define __TEST_WDT_REBOOT_H__

#include "vsf.h"
#   include "component/test/vsf_test/vsf_test.h"
#   include "test_params_generated.h"

#if VSF_TEST_WDT_REBOOT_ENABLE == ENABLED

#ifndef VSF_TEST_WDT_REBOOT_CASE_COUNT
#   define VSF_TEST_WDT_REBOOT_CASE_COUNT      1
#endif
#ifndef VSF_TEST_WDT_REBOOT_TIMEOUT_MS
#   define VSF_TEST_WDT_REBOOT_TIMEOUT_MS                          200
#endif

#ifndef VSF_TEST_WDT_REBOOT_SUPPORT_RESET_SOC
#   define VSF_TEST_WDT_REBOOT_SUPPORT_RESET_SOC         1
#endif

#ifndef VSF_TEST_WDT_REBOOT_MODE
#   define VSF_TEST_WDT_REBOOT_MODE                 (VSF_WDT_MODE_NO_EARLY_WAKEUP | VSF_WDT_MODE_RESET_SOC)
#endif

#ifndef VSF_TEST_WDT_REBOOT_TIMEOUT_MARGIN_MS
#   define VSF_TEST_WDT_REBOOT_TIMEOUT_MARGIN_MS    100
#endif

#ifdef __cplusplus
extern "C" {
#endif

void vsf_test_wdt_reboot_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst);

typedef struct {
    uint8_t  idx;
    uint16_t timeout_ms;
} vsf_test_wdt_reboot_params_t;

struct vsf_test_wdt_reboot_s;
extern const struct vsf_test_wdt_reboot_s vsf_test_wdt_reboot;

#ifdef __cplusplus
}
#endif

#endif /* VSF_TEST_WDT_REBOOT_ENABLE == ENABLED */

#endif /* __TEST_WDT_REBOOT_H__ */
/* EOF */
