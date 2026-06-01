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

#include "vsf_test_gpio_toggle_stress.h"
#include "vsf_test_suites.h"


/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_GPIO_TOGGLE_STRESS_ENABLE == ENABLED
const struct vsf_test_gpio_toggle_stress_s {
    vsf_test_suite_t hdr;
    vsf_test_gpio_toggle_stress_params_t          params[VSF_TEST_GPIO_TOGGLE_STRESS_CASE_COUNT];
} vsf_test_gpio_toggle_stress = {
    .hdr = {
        .name            = "gpio_toggle_stress",
        .jmp_fn          = vsf_test_gpio_toggle_stress_run,
        .case_count      = VSF_TEST_GPIO_TOGGLE_STRESS_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_gpio_toggle_stress_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_GPIO,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_GPIO_TOGGLE_STRESS_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/



#if VSF_TEST_GPIO_TOGGLE_STRESS_ENABLE == ENABLED


/*============================ IMPLEMENTATION ================================*/

void vsf_test_gpio_toggle_stress_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_gpio_toggle_stress_params_t *p = tc->params;
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_GPIO, gpio);
    vsf_gpio_t *gpio = inst->fixture.gpio;
    vsf_gpio_pin_mask_t out_mask = (vsf_gpio_pin_mask_t)1u << p->out_pin;
    vsf_gpio_pin_mask_t in_mask  = (vsf_gpio_pin_mask_t)1u << p->in_pin;

    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware suites do not print them. */
    VSF_TEST_GPIO_ASSERT_CAPABILITY(gpio);

    VSF_TEST_TRACE_INFO("gpio_toggle_stress:start (case=%u out=%u in=%u count=%lu)" VSF_TRACE_CFG_LINEEND,
                         (unsigned)tc->case_idx, (unsigned)p->out_pin, (unsigned)p->in_pin,
                         (unsigned long)p->stress_count);

    VSF_TEST_TRACE_DEBUG("gpio_toggle_stress:config output" VSF_TRACE_CFG_LINEEND);
    vsf_gpio_port_config_pins(gpio, out_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_OUTPUT_PUSH_PULL | VSF_GPIO_NO_PULL_UP_DOWN,
    });
    if (p->in_pin != p->out_pin) {
        VSF_TEST_TRACE_DEBUG("gpio_toggle_stress:config input" VSF_TRACE_CFG_LINEEND);
        vsf_gpio_port_config_pins(gpio, in_mask, &(vsf_gpio_cfg_t){
            .mode = VSF_GPIO_INPUT | VSF_GPIO_NO_PULL_UP_DOWN,
        });
    }

    /* Seed: drive low. expected_high tracks the post-toggle level. */
    VSF_TEST_TRACE_DEBUG("gpio_toggle_stress:seed low" VSF_TRACE_CFG_LINEEND);
    vsf_gpio_clear(gpio, out_mask);
    vsf_test_busy_wait_ms(VSF_TEST_POLL_TICK_MS);
    bool expected_high = false;
    uint32_t miss = 0;
    for (uint32_t i = 0; i < p->stress_count; i++) {
        vsf_gpio_toggle(gpio, out_mask);
        expected_high = !expected_high;
        /* Read on every iteration so a missed edge is caught immediately. */
        vsf_gpio_pin_mask_t in_val = vsf_gpio_read(gpio) & in_mask;
        if (expected_high) {
            if (in_val != in_mask) { miss++; }
        } else {
            if (in_val != 0)        { miss++; }
        }
    }
    if (miss != 0) {
        VSF_TEST_TRACE_ERROR("gpio_toggle_stress:miss=%lu (expected=0)" VSF_TRACE_CFG_LINEEND,
                              (unsigned long)miss);
    }
    VSF_TEST_TRACE_INFO("gpio_toggle_stress:pass count=%lu miss=%lu" VSF_TRACE_CFG_LINEEND,
                         (unsigned long)p->stress_count, (unsigned long)miss);
    VSF_TEST_ASSERT(miss == 0);
}

#endif /* VSF_TEST_GPIO_TOGGLE_STRESS_ENABLE == ENABLED */

/* EOF */
