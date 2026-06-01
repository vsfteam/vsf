#ifndef __VSF_TEST_WDT_BASIC_H__
#define __VSF_TEST_WDT_BASIC_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"
#   include "component/test/vsf_test/vsf_test.h"
#   include "test_params_generated.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_WDT_BASIC_CASE_COUNT
#   define VSF_TEST_WDT_BASIC_CASE_COUNT       1
#endif
#ifndef VSF_TEST_WDT_BASIC_BUSY_WAIT_MS
#   define VSF_TEST_WDT_BASIC_BUSY_WAIT_MS                    10
#endif

#ifndef VSF_TEST_WDT_BASIC_SUPPORT_RESET_SOC
#   define VSF_TEST_WDT_BASIC_SUPPORT_RESET_SOC          1
#endif

#ifndef VSF_TEST_WDT_BASIC_SUPPORT_DISABLE
#   define VSF_TEST_WDT_BASIC_SUPPORT_DISABLE            0
#endif

#ifndef VSF_TEST_WDT_BASIC_MODE
#   define VSF_TEST_WDT_BASIC_MODE                  (VSF_WDT_MODE_NO_EARLY_WAKEUP | VSF_WDT_MODE_RESET_SOC)
#endif


/*============================ PROTOTYPES ====================================*/

void vsf_test_wdt_basic_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst);

typedef struct {
    uint8_t  idx;
    uint16_t timeout_ms;
    uint8_t  feed_count;
    uint16_t feed_interval_ms;
} vsf_test_wdt_basic_params_t;

struct vsf_test_wdt_basic_s;
extern const struct vsf_test_wdt_basic_s vsf_test_wdt_basic;

#endif /* __VSF_TEST_WDT_BASIC_H__ */
/* EOF */
