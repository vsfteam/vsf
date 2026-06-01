#ifndef __VSF_TEST_ARCH_SYSTIMER_FREQ_H__
#define __VSF_TEST_ARCH_SYSTIMER_FREQ_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"
#include "component/test/vsf_test/vsf_test.h"
#include "test_params_generated.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_ARCH_SYSTIMER_FREQ_CASE_COUNT
#   define VSF_TEST_ARCH_SYSTIMER_FREQ_CASE_COUNT   1
#endif
#ifndef VSF_TEST_ARCH_SYSTIMER_FREQ_TIMEOUT_MS
#   define VSF_TEST_ARCH_SYSTIMER_FREQ_TIMEOUT_MS               2000
#endif

#ifndef VSF_TEST_ARCH_SYSTIMER_FREQ_EXPECTED_HZ
#   define VSF_TEST_ARCH_SYSTIMER_FREQ_EXPECTED_HZ      1000000
#endif

#ifndef VSF_TEST_ARCH_SYSTIMER_FREQ_TOLERANCE_PCT
#   define VSF_TEST_ARCH_SYSTIMER_FREQ_TOLERANCE_PCT    10
#endif

/*! Systimer ticks between serial pulse markers.
 *  1000 ms @ 1 MHz → 1 000 000 ticks.  Host measures wall-clock gap between
 *  receiving the two markers to compute independent frequency. */
#ifndef VSF_TEST_ARCH_SYSTIMER_FREQ_SERIAL_WAIT_MS
#   define VSF_TEST_ARCH_SYSTIMER_FREQ_SERIAL_WAIT_MS  1000
#endif

#ifndef VSF_TEST_ARCH_SYSTIMER_FREQ_PARAMS_INIT
#   define VSF_TEST_ARCH_SYSTIMER_FREQ_PARAMS_INIT      { .idx = 0 }
#endif

/*============================ PROTOTYPES ====================================*/

void vsf_test_arch_systimer_freq_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst);

typedef struct {
    uint8_t  idx;
} vsf_test_arch_systimer_freq_params_t;

struct vsf_test_arch_systimer_freq_s;
extern const struct vsf_test_arch_systimer_freq_s vsf_test_arch_systimer_freq;

#endif /* __VSF_TEST_ARCH_SYSTIMER_FREQ_H__ */
/* EOF */
