#ifndef __VSF_TEST_ARCH_PREEMPT_H__
#define __VSF_TEST_ARCH_PREEMPT_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"
#include "component/test/vsf_test/vsf_test.h"
#include "test_params_generated.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_ARCH_PREEMPT_CASE_COUNT
#   define VSF_TEST_ARCH_PREEMPT_CASE_COUNT   1
#endif
#ifndef VSF_TEST_ARCH_PREEMPT_TIMEOUT_MS
#   define VSF_TEST_ARCH_PREEMPT_TIMEOUT_MS               5000
#endif

#ifndef VSF_TEST_ARCH_PREEMPT_WORKER_COUNT
#   define VSF_TEST_ARCH_PREEMPT_WORKER_COUNT     2
#endif

#ifndef VSF_TEST_ARCH_PREEMPT_PARAMS_INIT
#   define VSF_TEST_ARCH_PREEMPT_PARAMS_INIT       { .idx = 0 }
#endif

/*============================ TYPES =========================================*/

typedef struct {
    volatile bool done[VSF_TEST_ARCH_PREEMPT_WORKER_COUNT];
} vsf_test_arch_preempt_data_t;

/*============================ PROTOTYPES ====================================*/

void vsf_test_arch_preempt_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst);

typedef struct {
    uint8_t  idx;
} vsf_test_arch_preempt_params_t;

struct vsf_test_arch_preempt_s;
extern const struct vsf_test_arch_preempt_s vsf_test_arch_preempt;

#endif /* __VSF_TEST_ARCH_PREEMPT_H__ */
/* EOF */
