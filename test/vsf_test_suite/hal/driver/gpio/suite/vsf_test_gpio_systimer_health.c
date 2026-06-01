/******************************************************************************
 *   Copyright(C)2009-2024 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 *****************************************************************************/

/*============================ INCLUDES ======================================*/

#include "vsf_test_gpio_systimer_health.h"
#include "vsf_test_suites.h"


/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_GPIO_SYSTIMER_HEALTH_ENABLE == ENABLED
const struct vsf_test_gpio_systimer_health_s {
    vsf_test_suite_t hdr;
    vsf_test_gpio_systimer_health_params_t          params[VSF_TEST_GPIO_SYSTIMER_HEALTH_CASE_COUNT];
} vsf_test_gpio_systimer_health = {
    .hdr = {
        .name            = "gpio_systimer_health",
        .jmp_fn          = vsf_test_gpio_systimer_health_run,
        .case_count      = VSF_TEST_GPIO_SYSTIMER_HEALTH_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_gpio_systimer_health_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_GPIO,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_GPIO_SYSTIMER_HEALTH_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/



#if VSF_TEST_GPIO_SYSTIMER_HEALTH_ENABLE == ENABLED


/*============================ IMPLEMENTATION ================================*/

/* Toggle a GPIO at known systimer-based intervals. The LA's sample clock
 * gives an external reference: the host decode counts edges and asserts
 * the on-wire period matches `interval_ms` within tolerance.
 *
 * If the chip's timer tick source is misconfigured, the timer block may run
 * at the wrong frequency and the on-wire interval stretches proportionally.
 * The LA decode catches that. The firmware-side trace cannot, because it
 * compares the systimer against itself.
 */
void vsf_test_gpio_systimer_health_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_gpio_systimer_health_params_t *p = tc->params;
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_GPIO, gpio);
    vsf_gpio_t *gpio = inst->fixture.gpio;
    vsf_gpio_pin_mask_t pin_mask = (vsf_gpio_pin_mask_t)1u << p->pin;

    VSF_TEST_GPIO_ASSERT_CAPABILITY(gpio);

    VSF_TEST_TRACE_INFO("gpio_systimer_health:start (case=%u pin=%u interval_ms=%lu)" VSF_TRACE_CFG_LINEEND,
                         (unsigned)tc->case_idx, (unsigned)p->pin, (unsigned long)p->interval_ms);

    VSF_TEST_TRACE_DEBUG("gpio_systimer_health:config pin" VSF_TRACE_CFG_LINEEND);
    vsf_gpio_port_config_pins(gpio, pin_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_TEST_GPIO_SYSTIMER_HEALTH_MODE,
    });
    vsf_gpio_clear(gpio, pin_mask);
    vsf_test_busy_wait_ms(VSF_TEST_POLL_TICK_MS);

    vsf_systimer_tick_t start = vsf_systimer_get();
    for (uint32_t i = 0; i < p->toggle_count; i++) {
        vsf_test_busy_wait_ms(p->interval_ms);
        vsf_gpio_toggle(gpio, pin_mask);
    }
    vsf_systimer_tick_t end = vsf_systimer_get();

    /* Diagnostic only — firmware cannot self-detect a broken systimer.
     * The host decode is the authoritative pass/fail. */
    uint64_t total_us = vsf_systimer_tick_to_us(end - start);
    VSF_TEST_TRACE_INFO("gpio_systimer_health:pass interval_ms=%lu toggle_count=%lu total_us=%llu"
                         VSF_TRACE_CFG_LINEEND,
                         (unsigned long)p->interval_ms,
                         (unsigned long)p->toggle_count,
                         (unsigned long long)total_us);
}

#endif /* VSF_TEST_GPIO_SYSTIMER_HEALTH_ENABLE == ENABLED */

/* EOF */
