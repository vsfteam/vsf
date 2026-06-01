/*============================ INCLUDES ======================================*/

#include "vsf_test_rng_basic.h"
#include "vsf_test_suites.h"


/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_RNG_BASIC_ENABLE == ENABLED
const struct vsf_test_rng_basic_s {
    vsf_test_suite_t hdr;
    vsf_test_rng_basic_params_t          params[VSF_TEST_RNG_BASIC_CASE_COUNT];
} vsf_test_rng_basic = {
    .hdr = {
        .name            = "rng_basic",
        .jmp_fn          = vsf_test_rng_basic_run,
        .case_count      = VSF_TEST_RNG_BASIC_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_rng_basic_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_RNG,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_RNG_BASIC_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/



#if VSF_TEST_RNG_BASIC_ENABLE == ENABLED

/*============================ MACROS ========================================*/

/*============================ IMPLEMENTATION ================================*/

void vsf_test_rng_basic_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_rng_basic_params_t *p = tc->params;
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_RNG, rng);
    vsf_rng_t *rng = inst->fixture.rng;

    VSF_TEST_TRACE_INFO("RNG:BASIC: start (case=%u)" VSF_TRACE_CFG_LINEEND,
                        (unsigned)tc->case_idx);

    VSF_TEST_TRACE_DEBUG("RNG:BASIC: vsf_rng_init" VSF_TRACE_CFG_LINEEND);
    vsf_err_t err = vsf_rng_init(rng);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "RNG:BASIC: vsf_rng_init failed (err=%d)"
                             VSF_TRACE_CFG_LINEEND, (int)err);

    uint32_t buffer[VSF_TEST_RNG_BASIC_BUF_SIZE];
    uint8_t word_count = p->word_count > VSF_TEST_RNG_BASIC_BUF_SIZE ? VSF_TEST_RNG_BASIC_BUF_SIZE : p->word_count;

    VSF_TEST_TRACE_DEBUG("RNG:BASIC: vsf_rng_generate_request (word_count=%u)"
                         VSF_TRACE_CFG_LINEEND, (unsigned)word_count);
    err = vsf_rng_generate_request(rng, buffer, word_count, NULL, NULL);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "RNG:BASIC: vsf_rng_generate_request failed (err=%d) (word_count=%u)"
                             VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)word_count);

    /* Basic entropy sanity check: not all zeros and not all identical */
    bool all_zero = true;
    bool all_same = true;
    for (uint8_t i = 0; i < word_count; i++) {
        if (buffer[i] != 0) {
            all_zero = false;
        }
        if (i > 0 && buffer[i] != buffer[0]) {
            all_same = false;
        }
    }
    if (all_zero) {
        VSF_TEST_TRACE_ERROR("RNG:BASIC: all values are zero (word_count=%u)" VSF_TRACE_CFG_LINEEND, (unsigned)word_count);
    }
    VSF_TEST_ASSERT(!all_zero);
    if (all_same) {
        VSF_TEST_TRACE_ERROR("RNG:BASIC: all values identical (0x%08lX)"
                             VSF_TRACE_CFG_LINEEND, (unsigned long)buffer[0]);
    }
    VSF_TEST_ASSERT(!all_same);

    VSF_TEST_TRACE_INFO("RNG:BASIC: PASS (word_count=%u)" VSF_TRACE_CFG_LINEEND,
                        (unsigned)word_count);
}

#endif /* VSF_TEST_RNG_BASIC_ENABLE == ENABLED */

/* EOF */


