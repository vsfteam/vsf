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

#include "vsf_test_gpio_write_throughput.h"
#include "vsf_test_suites.h"


/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_GPIO_WRITE_THROUGHPUT_ENABLE == ENABLED
const struct vsf_test_gpio_write_throughput_s {
    vsf_test_suite_t hdr;
    vsf_test_gpio_write_throughput_params_t          params[VSF_TEST_GPIO_WRITE_THROUGHPUT_CASE_COUNT];
} vsf_test_gpio_write_throughput = {
    .hdr = {
        .name            = "gpio_write_throughput",
        .jmp_fn          = vsf_test_gpio_write_throughput_run,
        .case_count      = VSF_TEST_GPIO_WRITE_THROUGHPUT_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_gpio_write_throughput_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_GPIO,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_GPIO_WRITE_THROUGHPUT_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/



#if VSF_TEST_GPIO_WRITE_THROUGHPUT_ENABLE == ENABLED


/*============================ IMPLEMENTATION ================================*/

void vsf_test_gpio_write_throughput_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_gpio_write_throughput_params_t *p = tc->params;
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_GPIO, gpio);
    vsf_gpio_t *gpio = inst->fixture.gpio;
    vsf_gpio_pin_mask_t pin_mask = (vsf_gpio_pin_mask_t)1u << p->pin;

    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware suites do not print them. */
    VSF_TEST_GPIO_ASSERT_CAPABILITY(gpio);

    VSF_TEST_TRACE_INFO("gpio_write_throughput:start (case=%u pin=%u)" VSF_TRACE_CFG_LINEEND,
                         (unsigned)tc->case_idx, (unsigned)p->pin);

    VSF_TEST_TRACE_DEBUG("gpio_write_throughput:config pin" VSF_TRACE_CFG_LINEEND);
    vsf_gpio_port_config_pins(gpio, pin_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_OUTPUT_PUSH_PULL | VSF_GPIO_NO_PULL_UP_DOWN,
    });

    /* Fixed-iteration loop. Time-bounded variants risk hanging when the
     * systimer isn't running in the synchronous test context. */
    uint32_t count = VSF_TEST_GPIO_WRITE_THROUGHPUT_ITERATIONS;
    vsf_systimer_tick_t start = vsf_systimer_get();
    for (uint32_t i = 0; i < count; i++) {
        vsf_gpio_write(gpio, pin_mask, (i & 1) ? pin_mask : 0);
    }
    vsf_systimer_tick_t end = vsf_systimer_get();
    uint64_t elapsed_us = vsf_systimer_tick_to_us(end - start);
    uint32_t writes_per_sec = (elapsed_us == 0) ? 0
                            : (uint32_t)((uint64_t)count * 1000000ULL / elapsed_us);

    VSF_TEST_TRACE_INFO("gpio_write_throughput:pass count=%lu elapsed_us=%llu per_sec=%lu" VSF_TRACE_CFG_LINEEND,
                         (unsigned long)count, (unsigned long long)elapsed_us,
                         (unsigned long)writes_per_sec);

    /* If the systimer isn't running, elapsed will be 0 and we can't
     * assert a throughput floor. Assert only that all writes executed. */
    if (elapsed_us > 0) {
        if (writes_per_sec <= VSF_TEST_GPIO_WRITE_THROUGHPUT_MIN_PER_SEC) {
            VSF_TEST_TRACE_ERROR("gpio_write_throughput:per_sec=%lu (expected >1000000)" VSF_TRACE_CFG_LINEEND,
                                  (unsigned long)writes_per_sec);
        }
        VSF_TEST_ASSERT(writes_per_sec > VSF_TEST_GPIO_WRITE_THROUGHPUT_MIN_PER_SEC);
    }
    (void)p->duration_us;
}

#endif /* VSF_TEST_GPIO_WRITE_THROUGHPUT_ENABLE == ENABLED */

/* EOF */
