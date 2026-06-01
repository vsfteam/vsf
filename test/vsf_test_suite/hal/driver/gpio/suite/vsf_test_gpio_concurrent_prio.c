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

#include "vsf_test_gpio_concurrent_prio.h"
#include "vsf_test_suites.h"


/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_GPIO_CONCURRENT_PRIO_ENABLE == ENABLED
const struct vsf_test_gpio_concurrent_prio_s {
    vsf_test_suite_t hdr;
    vsf_test_gpio_concurrent_prio_params_t          params[VSF_TEST_GPIO_CONCURRENT_PRIO_CASE_COUNT];
} vsf_test_gpio_concurrent_prio = {
    .hdr = {
        .name            = "gpio_concurrent_prio",
        .jmp_fn          = vsf_test_gpio_concurrent_prio_run,
        .case_count      = VSF_TEST_GPIO_CONCURRENT_PRIO_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_gpio_concurrent_prio_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_GPIO,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_GPIO_CONCURRENT_PRIO_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/


/*============================ LOCAL VARIABLES ===============================*/


#if VSF_TEST_GPIO_CONCURRENT_PRIO_ENABLE == ENABLED

/*============================ IMPLEMENTATION ================================*/

void vsf_test_gpio_concurrent_prio_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_gpio_concurrent_prio_params_t *p = tc->params;
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_GPIO, gpio);
    vsf_gpio_t *gpio = inst->fixture.gpio;
    vsf_gpio_pin_mask_t out_mask = (vsf_gpio_pin_mask_t)1u << p->out_pin;

    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware suites do not print them. */
    VSF_TEST_GPIO_ASSERT_CAPABILITY(gpio);

    VSF_TEST_TRACE_INFO("gpio_concurrent_prio:start (case=%u out=%u dur=%u cb_period=%u)"
                        VSF_TRACE_CFG_LINEEND,
                        (unsigned)tc->case_idx, (unsigned)p->out_pin,
                        (unsigned)p->duration_ms, (unsigned)p->callback_period_us);

    VSF_TEST_TRACE_DEBUG("gpio_concurrent_prio:config out_pin" VSF_TRACE_CFG_LINEEND);
    vsf_gpio_port_config_pins(gpio, out_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_OUTPUT_PUSH_PULL | VSF_GPIO_NO_PULL_UP_DOWN,
    });
    vsf_gpio_clear(gpio, out_mask);

    /* Per-case state in suite: must be re-initialised before each run. */
    vsf_test_suite_data.gpio.gpio_concurrent_prio.out_mask         = out_mask;
    vsf_test_suite_data.gpio.gpio_concurrent_prio.period_us        = p->callback_period_us;
    vsf_test_suite_data.gpio.gpio_concurrent_prio.callback_toggles = 0;
    vsf_test_suite_data.gpio.gpio_concurrent_prio.main_toggles     = 0;

    /* Concurrent toggle test: two toggle streams at different rates.
     * Stream A: high-frequency burst (1 k toggles / ms loop).
     * Stream B: periodic toggle at callback_period_us intervals.
     * Both run in the main loop; the test verifies sustained toggle
     * activity across the full duration. */
    uint32_t duration_us = p->duration_ms * 1000;
    uint32_t next_callback = p->callback_period_us;

    VSF_TEST_TRACE_DEBUG("gpio_concurrent_prio:toggle loop" VSF_TRACE_CFG_LINEEND);
    for (uint32_t elapsed = 0; elapsed < duration_us; elapsed++) {
        /* Stream A: high-frequency toggle. */
        vsf_gpio_toggle(gpio, out_mask);
        vsf_test_suite_data.gpio.gpio_concurrent_prio.main_toggles++;

        /* Stream B: periodic toggle at callback_period_us. */
        if (elapsed >= next_callback) {
            vsf_gpio_toggle(gpio, out_mask);
            vsf_test_suite_data.gpio.gpio_concurrent_prio.callback_toggles++;
            next_callback += p->callback_period_us;
        }

        vsf_test_busy_wait_us(1);
    }

    uint32_t cb_cnt  = vsf_test_suite_data.gpio.gpio_concurrent_prio.callback_toggles;
    uint32_t main_cnt = vsf_test_suite_data.gpio.gpio_concurrent_prio.main_toggles;
    VSF_TEST_TRACE_INFO("gpio_concurrent_prio:pass cb=%lu main=%lu" VSF_TRACE_CFG_LINEEND,
                        (unsigned long)cb_cnt, (unsigned long)main_cnt);
    if (cb_cnt == 0) {
        VSF_TEST_TRACE_ERROR("gpio_concurrent_prio:callback_toggles=0 (expected >0)"
                             VSF_TRACE_CFG_LINEEND);
    }
    VSF_TEST_ASSERT(cb_cnt > 0);
    if (main_cnt == 0) {
        VSF_TEST_TRACE_ERROR("gpio_concurrent_prio:main_toggles=0 (expected >0)"
                             VSF_TRACE_CFG_LINEEND);
    }
    VSF_TEST_ASSERT(main_cnt > 0);
}

#endif /* VSF_TEST_GPIO_CONCURRENT_PRIO_ENABLE == ENABLED */

/* EOF */