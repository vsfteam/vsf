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

#include "vsf_test_gpio_output_input.h"
#include "vsf_test_suites.h"


/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_GPIO_OUTPUT_INPUT_ENABLE == ENABLED
const struct vsf_test_gpio_output_input_s {
    vsf_test_suite_t hdr;
    vsf_test_gpio_output_input_params_t          params[VSF_TEST_GPIO_OUTPUT_INPUT_CASE_COUNT];
} vsf_test_gpio_output_input = {
    .hdr = {
        .name            = "gpio_output_input",
        .jmp_fn          = vsf_test_gpio_output_input_run,
        .case_count      = VSF_TEST_GPIO_OUTPUT_INPUT_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_gpio_output_input_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_GPIO,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_GPIO_OUTPUT_INPUT_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/



#if VSF_TEST_GPIO_OUTPUT_INPUT_ENABLE == ENABLED

/*============================ MACROS ========================================*/


/*============================ LOCAL VARIABLES ===============================*/

/*============================ IMPLEMENTATION ================================*/

void vsf_test_gpio_output_input_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_gpio_output_input_params_t *p = tc->params;
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_GPIO, gpio);
    vsf_gpio_t *gpio = inst->fixture.gpio;
    vsf_gpio_pin_mask_t out_mask = (vsf_gpio_pin_mask_t)1u << p->out_pin;
    vsf_gpio_pin_mask_t in_mask  = (vsf_gpio_pin_mask_t)1u << p->in_pin;

    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware suites do not print them. */

    VSF_TEST_TRACE_INFO("gpio_output_input:start (case=%u out=%u in=%u)" VSF_TRACE_CFG_LINEEND,
                         (unsigned)tc->case_idx, (unsigned)p->out_pin, (unsigned)p->in_pin);

    vsf_gpio_capability_t cap = vsf_gpio_capability(gpio);
    if ((cap.pin_mask & out_mask) == 0) {
        VSF_TEST_TRACE_ERROR("gpio_output_input:out_pin=%u not in capability mask" VSF_TRACE_CFG_LINEEND,
                              (unsigned)p->out_pin);
    }
    VSF_TEST_ASSERT((cap.pin_mask & out_mask) != 0);
    if ((cap.pin_mask & in_mask) == 0) {
        VSF_TEST_TRACE_ERROR("gpio_output_input:in_pin=%u not in capability mask" VSF_TRACE_CFG_LINEEND,
                              (unsigned)p->in_pin);
    }
    VSF_TEST_ASSERT((cap.pin_mask & in_mask)  != 0);

    /* Configure pin A as push-pull output. */
    VSF_TEST_TRACE_DEBUG("gpio_output_input:config output" VSF_TRACE_CFG_LINEEND);
    vsf_err_t err = vsf_gpio_port_config_pins(gpio, out_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_OUTPUT_PUSH_PULL | VSF_GPIO_NO_PULL_UP_DOWN,
    });
    VSF_TEST_ASSERT_ERR_NONE(err,
        "gpio_output_input:config output failed (err=%d) (out_pin=%u out_mask=0x%x)" VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)p->out_pin, (unsigned)out_mask);

    /* Phase-3 API completeness check (usart-gpio-coverage-gaps PRD):
     * get_pin_configuration() must report the output mode we just set.
     * Catches drivers that "accept" the cfg without actually applying it. */
    VSF_TEST_TRACE_DEBUG("gpio_output_input:get_pin_configuration" VSF_TRACE_CFG_LINEEND);
    vsf_gpio_cfg_t got = {0};
    err = vsf_gpio_get_pin_configuration(gpio, p->out_pin, &got);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "gpio_output_input:get_pin_configuration failed (err=%d) (pin=%u)" VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)p->out_pin);
    if ((got.mode & VSF_GPIO_MODE_MASK) != (VSF_GPIO_OUTPUT_PUSH_PULL & VSF_GPIO_MODE_MASK)) {
        VSF_TEST_TRACE_ERROR("gpio_output_input:mode mismatch (got=0x%x expected=0x%x)" VSF_TRACE_CFG_LINEEND,
                              (unsigned)got.mode, (unsigned)VSF_GPIO_OUTPUT_PUSH_PULL);
    }
    VSF_TEST_ASSERT((got.mode & VSF_GPIO_MODE_MASK) ==
                    (VSF_GPIO_OUTPUT_PUSH_PULL & VSF_GPIO_MODE_MASK));

    /* Configure pin B as input only if it's a different pin (otherwise we'd
     * overwrite the OUTPUT_PUSH_PULL config). Self-loopback exploits the
     * fact that some chips support simultaneous output + input on one pin. */
    if (p->in_pin != p->out_pin) {
        VSF_TEST_TRACE_DEBUG("gpio_output_input:config input" VSF_TRACE_CFG_LINEEND);
        err = vsf_gpio_port_config_pins(gpio, in_mask, &(vsf_gpio_cfg_t){
            .mode = VSF_GPIO_INPUT | VSF_GPIO_NO_PULL_UP_DOWN,
        });
        VSF_TEST_ASSERT_ERR_NONE(err,
            "gpio_output_input:config input failed (err=%d) (in_pin=%u in_mask=0x%x)" VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)p->in_pin, (unsigned)in_mask);
    }

    /* Drive high via write, observe via input pin. */
    VSF_TEST_TRACE_DEBUG("gpio_output_input:write high" VSF_TRACE_CFG_LINEEND);
    vsf_gpio_write(gpio, out_mask, out_mask);
    vsf_test_busy_wait_ms(VSF_TEST_POLL_TICK_MS);
    {
        uint32_t rd_val = vsf_gpio_read(gpio) & in_mask;
        if (rd_val != in_mask) {
            VSF_TEST_TRACE_ERROR("gpio_output_input:write high read=%lu (expected=%lu)" VSF_TRACE_CFG_LINEEND,
                                  (unsigned long)rd_val, (unsigned long)in_mask);
        }
        VSF_TEST_ASSERT(rd_val == in_mask);
    }
    {
        uint32_t rd_val = vsf_gpio_read_output_register(gpio) & out_mask;
        if (rd_val != out_mask) {
            VSF_TEST_TRACE_ERROR("gpio_output_input:write high output_reg=%lu (expected=%lu)" VSF_TRACE_CFG_LINEEND,
                                  (unsigned long)rd_val, (unsigned long)out_mask);
        }
        VSF_TEST_ASSERT(rd_val == out_mask);
    }

    /* Drive low via write. */
    VSF_TEST_TRACE_DEBUG("gpio_output_input:write low" VSF_TRACE_CFG_LINEEND);
    vsf_gpio_write(gpio, out_mask, 0);
    vsf_test_busy_wait_ms(VSF_TEST_POLL_TICK_MS);
    {
        uint32_t rd_val = vsf_gpio_read(gpio) & in_mask;
        if (rd_val != 0) {
            VSF_TEST_TRACE_ERROR("gpio_output_input:write low read=%lu (expected=0)" VSF_TRACE_CFG_LINEEND,
                                  (unsigned long)rd_val);
        }
        VSF_TEST_ASSERT(rd_val == 0);
    }
    {
        uint32_t rd_val = vsf_gpio_read_output_register(gpio) & out_mask;
        if (rd_val != 0) {
            VSF_TEST_TRACE_ERROR("gpio_output_input:write low output_reg=%lu (expected=0)" VSF_TRACE_CFG_LINEEND,
                                  (unsigned long)rd_val);
        }
        VSF_TEST_ASSERT(rd_val == 0);
    }

    /* set() / clear() should behave the same as write(high/low). */
    VSF_TEST_TRACE_DEBUG("gpio_output_input:set" VSF_TRACE_CFG_LINEEND);
    vsf_gpio_set(gpio, out_mask);
    vsf_test_busy_wait_ms(VSF_TEST_POLL_TICK_MS);
    {
        uint32_t rd_val = vsf_gpio_read(gpio) & in_mask;
        if (rd_val != in_mask) {
            VSF_TEST_TRACE_ERROR("gpio_output_input:set read=%lu (expected=%lu)" VSF_TRACE_CFG_LINEEND,
                                  (unsigned long)rd_val, (unsigned long)in_mask);
        }
        VSF_TEST_ASSERT(rd_val == in_mask);
    }

    VSF_TEST_TRACE_DEBUG("gpio_output_input:clear" VSF_TRACE_CFG_LINEEND);
    vsf_gpio_clear(gpio, out_mask);
    vsf_test_busy_wait_ms(VSF_TEST_POLL_TICK_MS);
    {
        uint32_t rd_val = vsf_gpio_read(gpio) & in_mask;
        if (rd_val != 0) {
            VSF_TEST_TRACE_ERROR("gpio_output_input:clear read=%lu (expected=0)" VSF_TRACE_CFG_LINEEND,
                                  (unsigned long)rd_val);
        }
        VSF_TEST_ASSERT(rd_val == 0);
    }

    VSF_TEST_TRACE_INFO("gpio_output_input:pass" VSF_TRACE_CFG_LINEEND);
}

#endif /* VSF_TEST_GPIO_OUTPUT_INPUT_ENABLE == ENABLED */

/* EOF */


