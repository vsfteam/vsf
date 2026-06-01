#ifndef __VSF_TEST_TIMER_PERIODIC_H__
#define __VSF_TEST_TIMER_PERIODIC_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"
#   include "component/test/vsf_test/vsf_test.h"
#   include "test_params_generated.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_TIMER_PERIODIC_CASE_COUNT
#   define VSF_TEST_TIMER_PERIODIC_CASE_COUNT   1
#endif
#ifndef VSF_TEST_TIMER_PERIODIC_TIMEOUT_MS
#   define VSF_TEST_TIMER_PERIODIC_TIMEOUT_MS                 200
#endif

#ifndef VSF_TEST_TIMER_PERIODIC_EXPECTED_BITLEN
#   define VSF_TEST_TIMER_PERIODIC_EXPECTED_BITLEN       32
#endif

#ifndef VSF_TEST_TIMER_PERIODIC_MIN_CHANNEL_COUNT
#   define VSF_TEST_TIMER_PERIODIC_MIN_CHANNEL_COUNT     1
#endif

#ifndef VSF_TEST_TIMER_PERIODIC_PRIO
#   define VSF_TEST_TIMER_PERIODIC_PRIO             vsf_arch_prio_1
#endif

#ifndef VSF_TEST_TIMER_PERIODIC_MODE
#   define VSF_TEST_TIMER_PERIODIC_MODE             (VSF_TIMER_CHANNEL_MODE_BASE | VSF_TIMER_BASE_CONTINUES)
#endif


#ifndef VSF_TEST_TIMER_PERIODIC_PERIOD_US
#   define VSF_TEST_TIMER_PERIODIC_PERIOD_US    10000
#endif

#ifndef VSF_TEST_TIMER_PERIODIC_COUNT
#   define VSF_TEST_TIMER_PERIODIC_COUNT        5
#endif


/*============================ TYPES =========================================*/

#if VSF_TEST_TIMER_PERIODIC_ENABLE == ENABLED
typedef struct {
    volatile uint32_t counter;
} vsf_test_timer_periodic_data_t;
#endif

/*============================ PROTOTYPES ====================================*/

void vsf_test_timer_periodic_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst);

typedef struct {
    uint8_t  idx;
    uint8_t  timer_idx;
    uint8_t  channel;
    uint32_t period_us;
    uint8_t  count;
} vsf_test_timer_periodic_params_t;

struct vsf_test_timer_periodic_s;
extern const struct vsf_test_timer_periodic_s vsf_test_timer_periodic;

#endif /* __VSF_TEST_TIMER_PERIODIC_H__ */
/* EOF */
