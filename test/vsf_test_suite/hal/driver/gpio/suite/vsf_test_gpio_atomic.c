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

#include "vsf_test_gpio_atomic.h"
#include "vsf_test_suites.h"


/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_GPIO_ATOMIC_ENABLE == ENABLED
const struct vsf_test_gpio_atomic_s {
    vsf_test_suite_t hdr;
    vsf_test_gpio_atomic_params_t          params[VSF_TEST_GPIO_ATOMIC_CASE_COUNT];
} vsf_test_gpio_atomic = {
    .hdr = {
        .name            = "gpio_atomic",
        .jmp_fn          = vsf_test_gpio_atomic_run,
        .case_count      = VSF_TEST_GPIO_ATOMIC_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_gpio_atomic_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_GPIO,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_GPIO_ATOMIC_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/



#if VSF_TEST_GPIO_ATOMIC_ENABLE == ENABLED


/*============================ IMPLEMENTATION ================================*/

/* Functional-only check. LA glitch detection lives host-side. */
void vsf_test_gpio_atomic_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_gpio_atomic_params_t *p = tc->params;
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_GPIO, gpio);
    vsf_gpio_t *gpio = inst->fixture.gpio;
    vsf_gpio_pin_mask_t out_mask = (vsf_gpio_pin_mask_t)1u << p->out_pin;
    vsf_gpio_pin_mask_t in_mask  = (vsf_gpio_pin_mask_t)1u << p->in_pin;

    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware suites do not print them. */

    VSF_TEST_TRACE_INFO("gpio_atomic:start (case=%u out=%u in=%u)" VSF_TRACE_CFG_LINEEND,
                        (unsigned)tc->case_idx, (unsigned)p->out_pin, (unsigned)p->in_pin);

    vsf_gpio_capability_t cap = vsf_gpio_capability(gpio);
    if (!cap.support_output_and_set) {
        VSF_TEST_TRACE_ERROR("gpio_atomic:capability missing output_and_set (pin_mask=0x%x)" VSF_TRACE_CFG_LINEEND, (unsigned)cap.pin_mask);
    }
    VSF_TEST_ASSERT(cap.support_output_and_set);
    if (!cap.support_output_and_clear) {
        VSF_TEST_TRACE_ERROR("gpio_atomic:capability missing output_and_clear (pin_mask=0x%x)" VSF_TRACE_CFG_LINEEND, (unsigned)cap.pin_mask);
    }
    VSF_TEST_ASSERT(cap.support_output_and_clear);

    VSF_TEST_TRACE_DEBUG("gpio_atomic:config in_pin" VSF_TRACE_CFG_LINEEND);
    vsf_gpio_port_config_pins(gpio, in_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_INPUT | VSF_GPIO_NO_PULL_UP_DOWN,
    });
    VSF_TEST_TRACE_DEBUG("gpio_atomic:config out_pin" VSF_TRACE_CFG_LINEEND);
    vsf_gpio_port_config_pins(gpio, out_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_INPUT | VSF_GPIO_NO_PULL_UP_DOWN,
    });

    /* output_and_set: transition input → output-high atomically */
    VSF_TEST_TRACE_DEBUG("gpio_atomic:output_and_set" VSF_TRACE_CFG_LINEEND);
    vsf_gpio_output_and_set(gpio, out_mask);
    vsf_test_busy_wait_ms(VSF_TEST_POLL_TICK_MS);
    {
        uint32_t rd_val = vsf_gpio_read(gpio) & in_mask;
        if (rd_val != in_mask) {
            VSF_TEST_TRACE_ERROR("gpio_atomic:output_and_set read=%lu (expected=%lu)"
                                 VSF_TRACE_CFG_LINEEND, (unsigned long)rd_val, (unsigned long)in_mask);
        }
        VSF_TEST_ASSERT(rd_val == in_mask);
    }
    {
        vsf_gpio_pin_mask_t dir = vsf_gpio_get_direction(gpio, out_mask) & out_mask;
        if (dir != out_mask) {
            VSF_TEST_TRACE_ERROR("gpio_atomic:output_and_set dir=%lu (expected=%lu)"
                                 VSF_TRACE_CFG_LINEEND, (unsigned long)dir, (unsigned long)out_mask);
        }
        VSF_TEST_ASSERT(dir == out_mask);
    }

    /* Switch back to input */
    VSF_TEST_TRACE_DEBUG("gpio_atomic:set_input" VSF_TRACE_CFG_LINEEND);
    vsf_gpio_set_input(gpio, out_mask);

    /* output_and_clear: transition input → output-low atomically */
    VSF_TEST_TRACE_DEBUG("gpio_atomic:output_and_clear" VSF_TRACE_CFG_LINEEND);
    vsf_gpio_output_and_clear(gpio, out_mask);
    vsf_test_busy_wait_ms(VSF_TEST_POLL_TICK_MS);
    {
        uint32_t rd_val = vsf_gpio_read(gpio) & in_mask;
        if (rd_val != 0) {
            VSF_TEST_TRACE_ERROR("gpio_atomic:output_and_clear read=%lu (expected=0)"
                                 VSF_TRACE_CFG_LINEEND, (unsigned long)rd_val);
        }
        VSF_TEST_ASSERT(rd_val == 0);
    }
    {
        vsf_gpio_pin_mask_t dir = vsf_gpio_get_direction(gpio, out_mask) & out_mask;
        if (dir != out_mask) {
            VSF_TEST_TRACE_ERROR("gpio_atomic:output_and_clear dir=%lu (expected=%lu)"
                                 VSF_TRACE_CFG_LINEEND, (unsigned long)dir, (unsigned long)out_mask);
        }
        VSF_TEST_ASSERT(dir == out_mask);
    }

    VSF_TEST_TRACE_INFO("gpio_atomic:pass" VSF_TRACE_CFG_LINEEND);
}

#endif /* VSF_TEST_GPIO_ATOMIC_ENABLE == ENABLED */

/* EOF */
