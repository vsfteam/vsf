/*============================ INCLUDES ======================================*/

#include "vsf_test_wdt_reboot.h"
#include "vsf_test_suites.h"

#if VSF_TEST_WDT_REBOOT_ENABLE == ENABLED

#include "hal/vsf_hal.h"

/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_WDT_REBOOT_ENABLE == ENABLED
const struct vsf_test_wdt_reboot_s {
    vsf_test_suite_t hdr;
    vsf_test_wdt_reboot_params_t          params[VSF_TEST_WDT_REBOOT_CASE_COUNT];
} vsf_test_wdt_reboot = {
    .hdr = {
        .name            = "wdt_reboot",
        .jmp_fn          = vsf_test_wdt_reboot_run,
        .case_count      = VSF_TEST_WDT_REBOOT_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_wdt_reboot_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_WDT,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_WDT_REBOOT_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/

/*============================ MACROS ========================================*/

/*============================ LOCAL VARIABLES ===============================*/

/*============================ IMPLEMENTATION ================================*/

void vsf_test_wdt_reboot_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_wdt_reboot_params_t *p = tc->params;
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_WDT, wdt);
    vsf_wdt_t *wdt = inst->fixture.wdt;

    VSF_TEST_TRACE_INFO("WDT:REBOOT: start (case=%u)" VSF_TRACE_CFG_LINEEND,
                        (unsigned)tc->case_idx);

    vsf_wdt_capability_t cap = vsf_wdt_capability(wdt);
    if (cap.support_reset_soc != VSF_TEST_WDT_REBOOT_SUPPORT_RESET_SOC) {
        VSF_TEST_TRACE_ERROR("wdt_reboot:support_reset_soc mismatch (expected=%u actual=%u)" VSF_TRACE_CFG_LINEEND,
                              (unsigned)VSF_TEST_WDT_REBOOT_SUPPORT_RESET_SOC,
                              (unsigned)cap.support_reset_soc);
    }
    VSF_TEST_ASSERT(cap.support_reset_soc == VSF_TEST_WDT_REBOOT_SUPPORT_RESET_SOC);

    VSF_TEST_TRACE_DEBUG("WDT:REBOOT: vsf_wdt_init (timeout=%u)"
                         VSF_TRACE_CFG_LINEEND,
                         (unsigned)VSF_TEST_WDT_REBOOT_TIMEOUT_MS);
    vsf_err_t err = vsf_wdt_init(wdt, &(vsf_wdt_cfg_t){
        .mode   = VSF_TEST_WDT_REBOOT_MODE,
        .max_ms = VSF_TEST_WDT_REBOOT_TIMEOUT_MS,
        .min_ms = 0,
    });
    VSF_TEST_ASSERT_ERR_NONE(err,
        "WDT:REBOOT: vsf_wdt_init failed (err=%d) (max_ms=%lu)"
                             VSF_TRACE_CFG_LINEEND, (int)err, (unsigned long)VSF_TEST_WDT_REBOOT_TIMEOUT_MS);

    VSF_TEST_TRACE_DEBUG("WDT:REBOOT: enable" VSF_TRACE_CFG_LINEEND);
    while (fsm_rt_cpl != vsf_wdt_enable(wdt));

    vsf_wdt_feed(wdt);
    VSF_TEST_TRACE_DEBUG("WDT:REBOOT: ARMED timeout=%ums" VSF_TRACE_CFG_LINEEND,
                         (unsigned)VSF_TEST_WDT_REBOOT_TIMEOUT_MS);

    /* Stop feeding — WDT will expire and reset the chip. */
    vsf_test_busy_wait_ms(VSF_TEST_WDT_REBOOT_TIMEOUT_MS + VSF_TEST_WDT_REBOOT_TIMEOUT_MARGIN_MS);

    /* Should never reach here — if we do, WDT failed to reset. */
    VSF_TEST_TRACE_ERROR("WDT:REBOOT: WDT failed to reset chip" VSF_TRACE_CFG_LINEEND);
    VSF_TEST_ASSERT(false);
}

#endif /* VSF_TEST_WDT_REBOOT_ENABLE == ENABLED */

/* EOF */


