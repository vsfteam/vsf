#ifndef __VSF_TEST_TIMER_ONESHOT_H__
#define __VSF_TEST_TIMER_ONESHOT_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"
#   include "component/test/vsf_test/vsf_test.h"
#   include "test_params_generated.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_TIMER_ONESHOT_CASE_COUNT
#   define VSF_TEST_TIMER_ONESHOT_CASE_COUNT   1
#endif
#ifndef VSF_TEST_TIMER_ONESHOT_TIMEOUT_MS
#   define VSF_TEST_TIMER_ONESHOT_TIMEOUT_MS                  150
#endif

#ifndef VSF_TEST_TIMER_ONESHOT_EXPECTED_BITLEN
#   define VSF_TEST_TIMER_ONESHOT_EXPECTED_BITLEN        32
#endif

#ifndef VSF_TEST_TIMER_ONESHOT_MIN_CHANNEL_COUNT
#   define VSF_TEST_TIMER_ONESHOT_MIN_CHANNEL_COUNT      1
#endif

#ifndef VSF_TEST_TIMER_ONESHOT_PRIO
#   define VSF_TEST_TIMER_ONESHOT_PRIO              vsf_arch_prio_1
#endif

#ifndef VSF_TEST_TIMER_ONESHOT_MODE
#   define VSF_TEST_TIMER_ONESHOT_MODE              (VSF_TIMER_CHANNEL_MODE_BASE | VSF_TIMER_BASE_ONESHOT)
#endif


#ifndef VSF_TEST_TIMER_ONESHOT_PERIOD_US
#   define VSF_TEST_TIMER_ONESHOT_PERIOD_US    50000
#endif


/*============================ TYPES =========================================*/

#if VSF_TEST_TIMER_ONESHOT_ENABLE == ENABLED
typedef struct {
    volatile bool fired;
} vsf_test_timer_oneshot_data_t;
#endif

/*============================ PROTOTYPES ====================================*/

void vsf_test_timer_oneshot_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst);

typedef struct {
    uint8_t  idx;
    uint8_t  timer_idx;
    uint8_t  channel;
    uint32_t period_us;
} vsf_test_timer_oneshot_params_t;

struct vsf_test_timer_oneshot_s;
extern const struct vsf_test_timer_oneshot_s vsf_test_timer_oneshot;

#endif /* __VSF_TEST_TIMER_ONESHOT_H__ */
/* EOF */
