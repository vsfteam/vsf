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

#include "vsf_test_gpio_toggle_freq.h"
#include "vsf_test_suites.h"


/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_GPIO_TOGGLE_FREQ_ENABLE == ENABLED
const struct vsf_test_gpio_toggle_freq_s {
    vsf_test_suite_t hdr;
    vsf_test_gpio_toggle_freq_params_t          params[VSF_TEST_GPIO_TOGGLE_FREQ_CASE_COUNT];
} vsf_test_gpio_toggle_freq = {
    .hdr = {
        .name            = "gpio_toggle_freq",
        .jmp_fn          = vsf_test_gpio_toggle_freq_run,
        .case_count      = VSF_TEST_GPIO_TOGGLE_FREQ_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_gpio_toggle_freq_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_GPIO,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_GPIO_TOGGLE_FREQ_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/



#if VSF_TEST_GPIO_TOGGLE_FREQ_ENABLE == ENABLED


/*============================ IMPLEMENTATION ================================*/

void vsf_test_gpio_toggle_freq_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_gpio_toggle_freq_params_t *p = tc->params;
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_GPIO, gpio);
    vsf_gpio_t *gpio = inst->fixture.gpio;
    vsf_gpio_pin_mask_t pin_mask = (vsf_gpio_pin_mask_t)1u << p->pin;

    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware suites do not print them. */
    VSF_TEST_GPIO_ASSERT_CAPABILITY(gpio);

    VSF_TEST_TRACE_INFO("gpio_toggle_freq:start (case=%u pin=%u count=%lu)" VSF_TRACE_CFG_LINEEND,
                         (unsigned)tc->case_idx, (unsigned)p->pin, (unsigned long)p->toggle_count);

    VSF_TEST_TRACE_DEBUG("gpio_toggle_freq:config pin" VSF_TRACE_CFG_LINEEND);
    vsf_gpio_port_config_pins(gpio, pin_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_OUTPUT_PUSH_PULL | VSF_GPIO_NO_PULL_UP_DOWN,
    });
    vsf_gpio_clear(gpio, pin_mask);

    vsf_systimer_tick_t start = vsf_systimer_get();
    for (uint32_t i = 0; i < p->toggle_count; i++) {
        vsf_gpio_toggle(gpio, pin_mask);
    }
    vsf_systimer_tick_t end = vsf_systimer_get();

    uint64_t total_us = vsf_systimer_tick_to_us(end - start);
    /* Avoid divide-by-zero on impossibly fast loops. */
    uint32_t period_ns = (p->toggle_count == 0) ? 0
                        : (uint32_t)((total_us * 1000ULL) / p->toggle_count);

    VSF_TEST_TRACE_INFO("gpio_toggle_freq:pass count=%lu total_us=%llu period_ns=%lu" VSF_TRACE_CFG_LINEEND,
                         (unsigned long)p->toggle_count, (unsigned long long)total_us,
                         (unsigned long)period_ns);

    /* No firmware-side timing assertion: the authoritative pass/fail comes
     * from the host-side decode(), which measures the on-wire toggle
     * frequency against an independent reference (the LA's sample clock).
     * A systimer-based assertion compares the systimer against itself and
     * cannot detect a misconfigured watchdog tick. */
}

#endif /* VSF_TEST_GPIO_TOGGLE_FREQ_ENABLE == ENABLED */

/* EOF */
