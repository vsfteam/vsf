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

#include "vsf_test_gpio_open_drain.h"
#include "vsf_test_suites.h"


/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_GPIO_OPEN_DRAIN_ENABLE == ENABLED
const struct vsf_test_gpio_open_drain_s {
    vsf_test_suite_t hdr;
    vsf_test_gpio_open_drain_params_t          params[VSF_TEST_GPIO_OPEN_DRAIN_CASE_COUNT];
} vsf_test_gpio_open_drain = {
    .hdr = {
        .name            = "gpio_open_drain",
        .jmp_fn          = vsf_test_gpio_open_drain_run,
        .case_count      = VSF_TEST_GPIO_OPEN_DRAIN_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_gpio_open_drain_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_GPIO,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_GPIO_OPEN_DRAIN_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/



#if VSF_TEST_GPIO_OPEN_DRAIN_ENABLE == ENABLED


/*============================ IMPLEMENTATION ================================*/

void vsf_test_gpio_open_drain_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_gpio_open_drain_params_t *p = tc->params;
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_GPIO, gpio);
    vsf_gpio_t *gpio = inst->fixture.gpio;
    vsf_gpio_pin_mask_t out_mask = (vsf_gpio_pin_mask_t)1u << p->out_pin;
    vsf_gpio_pin_mask_t in_mask  = (vsf_gpio_pin_mask_t)1u << p->in_pin;

    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware suites do not print them. */
    VSF_TEST_GPIO_ASSERT_CAPABILITY(gpio);

    VSF_TEST_TRACE_INFO("gpio_open_drain:start (case=%u out=%u in=%u)" VSF_TRACE_CFG_LINEEND,
                         (unsigned)tc->case_idx, (unsigned)p->out_pin, (unsigned)p->in_pin);

    /* Use internal pull-up on the input pin as a fallback when no
     * external resistor is wired (the PRD-mandated fixture). */
    VSF_TEST_TRACE_DEBUG("gpio_open_drain:config output" VSF_TRACE_CFG_LINEEND);
    vsf_gpio_port_config_pins(gpio, out_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_OUTPUT_OPEN_DRAIN | VSF_GPIO_PULL_UP,
    });
    if (p->in_pin != p->out_pin) {
        VSF_TEST_TRACE_DEBUG("gpio_open_drain:config input" VSF_TRACE_CFG_LINEEND);
        vsf_gpio_port_config_pins(gpio, in_mask, &(vsf_gpio_cfg_t){
            .mode = VSF_GPIO_INPUT | VSF_GPIO_PULL_UP,
        });
    }

    /* OD writes 0 → actively drives low */
    VSF_TEST_TRACE_DEBUG("gpio_open_drain:write 0" VSF_TRACE_CFG_LINEEND);
    vsf_gpio_write(gpio, out_mask, 0);
    vsf_test_busy_wait_ms(VSF_TEST_POLL_TICK_MS);
    {
        uint32_t rd_val = vsf_gpio_read(gpio) & in_mask;
        if (rd_val != 0) {
            VSF_TEST_TRACE_ERROR("gpio_open_drain:write 0 read=%lu (expected=0)" VSF_TRACE_CFG_LINEEND,
                                  (unsigned long)rd_val);
        }
        VSF_TEST_ASSERT(rd_val == 0);
    }
    /* Phase-3 API completeness check (usart-gpio-coverage-gaps PRD):
     * read_output_register() vs read() — the driver toggles OE, not
     * gpio_out, so the output register may read differently from the line
     * state. This distinguishes "intent register" from "line state" for
     * callers. */
    {
        uint32_t rd_val = vsf_gpio_read_output_register(gpio) & out_mask;
        if (rd_val != 0) {
            VSF_TEST_TRACE_ERROR("gpio_open_drain:write 0 output_reg=%lu (expected=0)" VSF_TRACE_CFG_LINEEND,
                                  (unsigned long)rd_val);
        }
        VSF_TEST_ASSERT(rd_val == 0);
    }

    /* OD writes 1 → releases line, pull-up brings it high */
    VSF_TEST_TRACE_DEBUG("gpio_open_drain:write 1" VSF_TRACE_CFG_LINEEND);
    vsf_gpio_write(gpio, out_mask, out_mask);
    vsf_test_busy_wait_ms(VSF_TEST_POLL_TICK_MS);
    {
        uint32_t rd_val = vsf_gpio_read(gpio) & in_mask;
        if (rd_val != in_mask) {
            VSF_TEST_TRACE_ERROR("gpio_open_drain:write 1 read=%lu (expected=%lu)" VSF_TRACE_CFG_LINEEND,
                                  (unsigned long)rd_val, (unsigned long)in_mask);
        }
        VSF_TEST_ASSERT(rd_val == in_mask);
    }
    /* read() reports the externally-pulled high; read_output_register()
     * still reports 0 (driver keeps gpio_out=0 in OD mode). The two
     * differing values document the API distinction. */
    {
        uint32_t rd_val = vsf_gpio_read_output_register(gpio) & out_mask;
        if (rd_val != 0) {
            VSF_TEST_TRACE_ERROR("gpio_open_drain:write 1 output_reg=%lu (expected=0)" VSF_TRACE_CFG_LINEEND,
                                  (unsigned long)rd_val);
        }
        VSF_TEST_ASSERT(rd_val == 0);
    }

    VSF_TEST_TRACE_INFO("gpio_open_drain:pass" VSF_TRACE_CFG_LINEEND);
}

#endif /* VSF_TEST_GPIO_OPEN_DRAIN_ENABLE == ENABLED */

/* EOF */
