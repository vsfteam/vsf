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

#include "vsf_test_gpio_multi_pin.h"
#include "vsf_test_suites.h"


/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_GPIO_MULTI_PIN_ENABLE == ENABLED
const struct vsf_test_gpio_multi_pin_s {
    vsf_test_suite_t hdr;
    vsf_test_gpio_multi_pin_params_t          params[VSF_TEST_GPIO_MULTI_PIN_CASE_COUNT];
} vsf_test_gpio_multi_pin = {
    .hdr = {
        .name            = "gpio_multi_pin",
        .jmp_fn          = vsf_test_gpio_multi_pin_run,
        .case_count      = VSF_TEST_GPIO_MULTI_PIN_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_gpio_multi_pin_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_GPIO,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_GPIO_MULTI_PIN_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/



#if VSF_TEST_GPIO_MULTI_PIN_ENABLE == ENABLED


/*============================ IMPLEMENTATION ================================*/

void vsf_test_gpio_multi_pin_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_gpio_multi_pin_params_t *p = tc->params;
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_GPIO, gpio);
    vsf_gpio_t *gpio = inst->fixture.gpio;
    vsf_gpio_pin_mask_t oa = (vsf_gpio_pin_mask_t)1u << p->out_pin_a;
    vsf_gpio_pin_mask_t ob = (vsf_gpio_pin_mask_t)1u << p->out_pin_b;
    vsf_gpio_pin_mask_t ia = (vsf_gpio_pin_mask_t)1u << p->in_pin_a;
    vsf_gpio_pin_mask_t ib = (vsf_gpio_pin_mask_t)1u << p->in_pin_b;
    vsf_gpio_pin_mask_t out_mask = oa | ob;
    vsf_gpio_pin_mask_t in_mask  = ia | ib;

    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware suites do not print them. */
    VSF_TEST_GPIO_ASSERT_CAPABILITY(gpio);

    VSF_TEST_TRACE_INFO("gpio_multi_pin:start (case=%u oa=%u ob=%u ia=%u ib=%u)" VSF_TRACE_CFG_LINEEND,
                         (unsigned)tc->case_idx, (unsigned)p->out_pin_a, (unsigned)p->out_pin_b,
                         (unsigned)p->in_pin_a, (unsigned)p->in_pin_b);

    VSF_TEST_TRACE_DEBUG("gpio_multi_pin:config outputs" VSF_TRACE_CFG_LINEEND);
    vsf_gpio_port_config_pins(gpio, out_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_OUTPUT_PUSH_PULL | VSF_GPIO_NO_PULL_UP_DOWN,
    });
    /* Only configure input pins that are NOT also output pins,
     * otherwise we'd overwrite the OUTPUT_PUSH_PULL config.
     * Some chips support simultaneous output + input on the same pin. */
    vsf_gpio_pin_mask_t exclusive_in_mask = in_mask & ~out_mask;
    if (exclusive_in_mask != 0) {
        VSF_TEST_TRACE_DEBUG("gpio_multi_pin:config inputs" VSF_TRACE_CFG_LINEEND);
        vsf_gpio_port_config_pins(gpio, exclusive_in_mask, &(vsf_gpio_cfg_t){
            .mode = VSF_GPIO_INPUT | VSF_GPIO_NO_PULL_UP_DOWN,
        });
    }

    /* All 4 patterns 00 / 01 / 10 / 11 */
    struct { vsf_gpio_pin_mask_t out_val; vsf_gpio_pin_mask_t expect_in; } steps[] = {
        {0,      0},
        {oa,     ia},
        {ob,     ib},
        {oa|ob,  ia|ib},
    };
    for (size_t k = 0; k < sizeof(steps)/sizeof(steps[0]); k++) {
        VSF_TEST_TRACE_DEBUG("gpio_multi_pin:write step=%zu out_val=%lu" VSF_TRACE_CFG_LINEEND,
                              k, (unsigned long)steps[k].out_val);
        vsf_gpio_write(gpio, out_mask, steps[k].out_val);
        vsf_test_busy_wait_ms(VSF_TEST_POLL_TICK_MS);
        uint32_t rd_val = vsf_gpio_read(gpio) & in_mask;
        if (rd_val != steps[k].expect_in) {
            VSF_TEST_TRACE_ERROR("gpio_multi_pin:step=%zu read=%lu (expected=%lu)" VSF_TRACE_CFG_LINEEND,
                                  k, (unsigned long)rd_val, (unsigned long)steps[k].expect_in);
        }
        VSF_TEST_ASSERT(rd_val == steps[k].expect_in);
    }

    VSF_TEST_TRACE_INFO("gpio_multi_pin:pass" VSF_TRACE_CFG_LINEEND);
}

#endif /* VSF_TEST_GPIO_MULTI_PIN_ENABLE == ENABLED */

/* EOF */
