#ifndef __VSF_TEST_TIMER_ASYNC_H__
#define __VSF_TEST_TIMER_ASYNC_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"
#   include "component/test/vsf_test/vsf_test.h"
#   include "test_params_generated.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_TIMER_ASYNC_CASE_COUNT
#   define VSF_TEST_TIMER_ASYNC_CASE_COUNT   1
#endif
#ifndef VSF_TEST_TIMER_ASYNC_TIMEOUT_MS
#   define VSF_TEST_TIMER_ASYNC_TIMEOUT_MS                    150
#endif

#ifndef VSF_TEST_TIMER_ASYNC_PERIOD_TIMEOUT_MS
#   define VSF_TEST_TIMER_ASYNC_PERIOD_TIMEOUT_MS             200
#endif

#ifndef VSF_TEST_TIMER_ASYNC_STOP_TIMEOUT_MS
#   define VSF_TEST_TIMER_ASYNC_STOP_TIMEOUT_MS               50
#endif

#ifndef VSF_TEST_TIMER_ASYNC_EXPECTED_BITLEN
#   define VSF_TEST_TIMER_ASYNC_EXPECTED_BITLEN          32
#endif

#ifndef VSF_TEST_TIMER_ASYNC_MIN_CHANNEL_COUNT
#   define VSF_TEST_TIMER_ASYNC_MIN_CHANNEL_COUNT        1
#endif

#ifndef VSF_TEST_TIMER_ASYNC_PRIO
#   define VSF_TEST_TIMER_ASYNC_PRIO                vsf_arch_prio_1
#endif

#ifndef VSF_TEST_TIMER_ASYNC_MODE
#   define VSF_TEST_TIMER_ASYNC_MODE                (VSF_TIMER_CHANNEL_MODE_BASE | VSF_TIMER_BASE_CONTINUES)
#endif

#ifndef VSF_TEST_TIMER_ASYNC_MAX_COUNTER
#   define VSF_TEST_TIMER_ASYNC_MAX_COUNTER         255
#endif


#ifndef VSF_TEST_TIMER_ASYNC_PERIOD_US
#   define VSF_TEST_TIMER_ASYNC_PERIOD_US    10000
#endif

#ifndef VSF_TEST_TIMER_ASYNC_COUNT
#   define VSF_TEST_TIMER_ASYNC_COUNT        10
#endif


/*============================ TYPES =========================================*/

#if VSF_TEST_TIMER_ASYNC_ENABLE == ENABLED
typedef struct {
    volatile uint8_t counter;
} vsf_test_timer_async_data_t;
#endif

/*============================ PROTOTYPES ====================================*/

void vsf_test_timer_async_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst);

typedef struct {
    uint8_t  idx;
    uint8_t  timer_idx;
    uint8_t  channel;
    uint32_t period_us;
} vsf_test_timer_async_params_t;

struct vsf_test_timer_async_s;
extern const struct vsf_test_timer_async_s vsf_test_timer_async;

#endif /* __VSF_TEST_TIMER_ASYNC_H__ */
/* EOF */
