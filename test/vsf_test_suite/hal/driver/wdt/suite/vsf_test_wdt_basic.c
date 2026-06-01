/*============================ INCLUDES ======================================*/

#include "vsf_test_wdt_basic.h"
#include "vsf_test_suites.h"

#if VSF_TEST_WDT_BASIC_ENABLE == ENABLED

#include "hal/vsf_hal.h"


/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_WDT_BASIC_ENABLE == ENABLED
const struct vsf_test_wdt_basic_s {
    vsf_test_suite_t hdr;
    vsf_test_wdt_basic_params_t          params[VSF_TEST_WDT_BASIC_CASE_COUNT];
} vsf_test_wdt_basic = {
    .hdr = {
        .name            = "wdt_basic",
        .jmp_fn          = vsf_test_wdt_basic_run,
        .case_count      = VSF_TEST_WDT_BASIC_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_wdt_basic_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_WDT,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_WDT_BASIC_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/



/*============================ MACROS ========================================*/


/*============================ LOCAL VARIABLES ===============================*/

/*============================ IMPLEMENTATION ================================*/

void vsf_test_wdt_basic_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_wdt_basic_params_t *p = tc->params;
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_WDT, wdt);
    vsf_wdt_t *wdt = inst->fixture.wdt;

    VSF_TEST_TRACE_INFO("WDT:BASIC: start (case=%u)" VSF_TRACE_CFG_LINEEND,
                        (unsigned)tc->case_idx);

    vsf_wdt_capability_t cap = vsf_wdt_capability(wdt);
    if (cap.support_reset_soc != VSF_TEST_WDT_BASIC_SUPPORT_RESET_SOC) {
        VSF_TEST_TRACE_ERROR("wdt_basic:support_reset_soc mismatch (expected=%u actual=%u)" VSF_TRACE_CFG_LINEEND,
                              (unsigned)VSF_TEST_WDT_BASIC_SUPPORT_RESET_SOC,
                              (unsigned)cap.support_reset_soc);
    }
    VSF_TEST_ASSERT(cap.support_reset_soc == VSF_TEST_WDT_BASIC_SUPPORT_RESET_SOC);
    if (cap.support_disable != VSF_TEST_WDT_BASIC_SUPPORT_DISABLE) {
        VSF_TEST_TRACE_ERROR("wdt_basic:support_disable mismatch (expected=%u actual=%u)" VSF_TRACE_CFG_LINEEND,
                              (unsigned)VSF_TEST_WDT_BASIC_SUPPORT_DISABLE,
                              (unsigned)cap.support_disable);
    }
    VSF_TEST_ASSERT(cap.support_disable == VSF_TEST_WDT_BASIC_SUPPORT_DISABLE);

    VSF_TEST_TRACE_DEBUG("WDT:BASIC: vsf_wdt_init (max_ms=500)" VSF_TRACE_CFG_LINEEND);
    vsf_err_t err = vsf_wdt_init(wdt, &(vsf_wdt_cfg_t){
        .mode   = VSF_TEST_WDT_BASIC_MODE,
        .max_ms = 500,
        .min_ms = 0,
    });
    VSF_TEST_ASSERT_ERR_NONE(err,
        "WDT:BASIC: vsf_wdt_init failed (err=%d) (max_ms=%lu)"
                             VSF_TRACE_CFG_LINEEND, (int)err, (unsigned long)500);

    VSF_TEST_TRACE_DEBUG("WDT:BASIC: enable" VSF_TRACE_CFG_LINEEND);
    while (fsm_rt_cpl != vsf_wdt_enable(wdt));

    for (uint8_t i = 0; i < p->feed_count; i++) {
        vsf_wdt_feed(wdt);
        VSF_TEST_TRACE_DEBUG("WDT:BASIC: feed %u" VSF_TRACE_CFG_LINEEND, i);
        vsf_test_busy_wait_ms(VSF_TEST_WDT_BASIC_BUSY_WAIT_MS);
    }

    VSF_TEST_TRACE_INFO("WDT:BASIC: PASS (feed_count=%u)" VSF_TRACE_CFG_LINEEND,
                        (unsigned)p->feed_count);
}

#endif /* VSF_TEST_WDT_BASIC_ENABLE == ENABLED */

/* EOF */


