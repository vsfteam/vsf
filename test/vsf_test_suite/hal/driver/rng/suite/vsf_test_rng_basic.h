#ifndef __VSF_TEST_RNG_BASIC_H__
#define __VSF_TEST_RNG_BASIC_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"
#   include "component/test/vsf_test/vsf_test.h"
#   include "test_params_generated.h"
#ifndef VSF_TEST_RNG_BASIC_BUF_SIZE
#   define VSF_TEST_RNG_BASIC_BUF_SIZE        16
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_RNG_BASIC_CASE_COUNT
#   define VSF_TEST_RNG_BASIC_CASE_COUNT       1
#endif

/*============================ PROTOTYPES ====================================*/

void vsf_test_rng_basic_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst);

typedef struct {
    uint8_t  idx;
    uint8_t  word_count;
} vsf_test_rng_basic_params_t;

struct vsf_test_rng_basic_s;
extern const struct vsf_test_rng_basic_s vsf_test_rng_basic;

#endif /* __VSF_TEST_RNG_BASIC_H__ */
/* EOF */
