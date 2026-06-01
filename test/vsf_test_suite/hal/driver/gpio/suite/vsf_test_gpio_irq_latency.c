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

#include "vsf_test_gpio_irq_latency.h"
#include "vsf_test_suites.h"


/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_GPIO_IRQ_LATENCY_ENABLE == ENABLED
const struct vsf_test_gpio_irq_latency_s {
    vsf_test_suite_t hdr;
    vsf_test_gpio_irq_latency_params_t          params[VSF_TEST_GPIO_IRQ_LATENCY_CASE_COUNT];
} vsf_test_gpio_irq_latency = {
    .hdr = {
        .name            = "gpio_irq_latency",
        .jmp_fn          = vsf_test_gpio_irq_latency_run,
        .case_count      = VSF_TEST_GPIO_IRQ_LATENCY_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_gpio_irq_latency_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_GPIO,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_GPIO_IRQ_LATENCY_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/



/*============================ LOCAL VARIABLES ===============================*/


#if VSF_TEST_GPIO_IRQ_LATENCY_ENABLE == ENABLED

static void __attribute__((section(".time_critical"))) __latency_handler(void *target, vsf_gpio_t *gpio, vsf_gpio_pin_mask_t pin_mask)
{
    vsf_test_suite_t *suite = target;
    if (pin_mask & vsf_test_suite_data.gpio.gpio_irq_latency.expected_pin) {
        vsf_test_suite_data.gpio.gpio_irq_latency.isr_tick = vsf_systimer_get();
        vsf_test_suite_data.gpio.gpio_irq_latency.fired = true;
    }
}

/*============================ IMPLEMENTATION ================================*/

void vsf_test_gpio_irq_latency_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_gpio_irq_latency_params_t *p = tc->params;
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_GPIO, gpio);
    vsf_gpio_t *gpio = inst->fixture.gpio;
    vsf_gpio_pin_mask_t pin_mask = (vsf_gpio_pin_mask_t)1u << p->pin;

    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware suites do not print them. */
    VSF_TEST_GPIO_ASSERT_CAPABILITY(gpio);

    VSF_TEST_TRACE_INFO("gpio_irq_latency:start (case=%u pin=%u)" VSF_TRACE_CFG_LINEEND,
                         (unsigned)tc->case_idx, (unsigned)p->pin);

    /* Per-case state in suite: must be re-initialised before each run. */
    vsf_test_suite_data.gpio.gpio_irq_latency.expected_pin = pin_mask;
    vsf_test_suite_data.gpio.gpio_irq_latency.fired        = false;
    vsf_test_suite_data.gpio.gpio_irq_latency.isr_tick     = 0;
    vsf_test_suite_data.gpio.gpio_irq_latency.trigger_tick = 0;

    /* Configure pin as EXTI rising edge — driven by GPIO output from the same
     * test (self-trigger; no external wiring needed). */
    VSF_TEST_TRACE_DEBUG("gpio_irq_latency:config exti" VSF_TRACE_CFG_LINEEND);
    vsf_err_t err = vsf_gpio_port_config_pins(gpio, pin_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_EXTI | VSF_GPIO_PULL_DOWN | VSF_GPIO_EXTI_MODE_RISING,
    });
    VSF_TEST_ASSERT_ERR_NONE(err,
        "gpio_irq_latency:config exti failed (err=%d) (pin=%u pin_mask=0x%x)" VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)p->pin, (unsigned)pin_mask);

    VSF_TEST_TRACE_DEBUG("gpio_irq_latency:irq_config" VSF_TRACE_CFG_LINEEND);
    err = vsf_gpio_exti_irq_config(gpio, &(vsf_gpio_exti_irq_cfg_t){
        .handler_fn = __latency_handler,
        .target_ptr = NULL,
        .prio       = VSF_TEST_GPIO_IRQ_LATENCY_PRIO,
    });
    VSF_TEST_ASSERT_ERR_NONE(err,
        "gpio_irq_latency:irq_config failed (err=%d) (prio=%d)" VSF_TRACE_CFG_LINEEND, (int)err, (int)VSF_TEST_GPIO_IRQ_LATENCY_PRIO);

    VSF_TEST_TRACE_DEBUG("gpio_irq_latency:enable" VSF_TRACE_CFG_LINEEND);
    vsf_gpio_set_output(gpio, pin_mask);
    vsf_gpio_clear(gpio, pin_mask);
    vsf_test_busy_wait_ms(VSF_TEST_POLL_TICK_MS);
    vsf_gpio_exti_irq_clear(gpio, pin_mask);
    err = vsf_gpio_exti_irq_enable(gpio, pin_mask);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "gpio_irq_latency:irq_enable failed (err=%d) (pin=%u pin_mask=0x%x)" VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)p->pin, (unsigned)pin_mask);

    /* Repeat the measurement several times and record every iteration. */
    uint32_t deltas[VSF_TEST_GPIO_IRQ_LATENCY_ITERATIONS];
    uint32_t worst_ticks = 0;
    for (uint32_t i = 0; i < VSF_TEST_GPIO_IRQ_LATENCY_ITERATIONS; i++) {
        vsf_test_suite_data.gpio.gpio_irq_latency.fired = false;
        vsf_test_suite_data.gpio.gpio_irq_latency.isr_tick = 0;
        vsf_gpio_clear(gpio, pin_mask);
        vsf_test_busy_wait_ms(VSF_TEST_POLL_TICK_MS);
        vsf_gpio_exti_irq_clear(gpio, pin_mask);

        vsf_test_suite_data.gpio.gpio_irq_latency.trigger_tick = vsf_systimer_get();
        vsf_gpio_set(gpio, pin_mask);   /* rising edge → EXTI fires */
        /* Spin until ISR captures its tick. */
        VSF_TEST_SPIN_FOR(vsf_test_suite_data.gpio.gpio_irq_latency.fired, VSF_TEST_GPIO_IRQ_LATENCY_SPIN_COUNT);
        if (!vsf_test_suite_data.gpio.gpio_irq_latency.fired) {
            VSF_TEST_TRACE_ERROR("gpio_irq_latency:iteration %u timeout" VSF_TRACE_CFG_LINEEND, (unsigned)i);
        }
        VSF_TEST_ASSERT(vsf_test_suite_data.gpio.gpio_irq_latency.fired);
        uint32_t delta = (uint32_t)(vsf_test_suite_data.gpio.gpio_irq_latency.isr_tick - vsf_test_suite_data.gpio.gpio_irq_latency.trigger_tick);
        deltas[i] = delta;
        if (delta > worst_ticks) { worst_ticks = delta; }
    }

    /* Compute median ticks (copy array so original order is preserved). */
    uint32_t sorted[VSF_TEST_GPIO_IRQ_LATENCY_ITERATIONS];
    for (uint32_t i = 0; i < VSF_TEST_GPIO_IRQ_LATENCY_ITERATIONS; i++) {
        sorted[i] = deltas[i];
    }
    for (uint32_t i = 0; i < VSF_TEST_GPIO_IRQ_LATENCY_ITERATIONS - 1; i++) {
        for (uint32_t j = i + 1; j < VSF_TEST_GPIO_IRQ_LATENCY_ITERATIONS; j++) {
            if (sorted[i] > sorted[j]) {
                uint32_t tmp = sorted[i];
                sorted[i] = sorted[j];
                sorted[j] = tmp;
            }
        }
    }
    uint32_t median_ticks;
    if (VSF_TEST_GPIO_IRQ_LATENCY_ITERATIONS % 2 == 1) {
        median_ticks = sorted[VSF_TEST_GPIO_IRQ_LATENCY_ITERATIONS / 2];
    } else {
        median_ticks = (sorted[VSF_TEST_GPIO_IRQ_LATENCY_ITERATIONS / 2 - 1]
                      + sorted[VSF_TEST_GPIO_IRQ_LATENCY_ITERATIONS / 2]) / 2;
    }

    uint64_t median_us = vsf_systimer_tick_to_us(median_ticks);
    uint32_t median_ns = (uint32_t)(median_us * 1000ULL);
    uint32_t anomaly_threshold_ticks = median_ticks * 2;

    /* Print every iteration and warn on anomalies that are still within max. */
    for (uint32_t i = 0; i < VSF_TEST_GPIO_IRQ_LATENCY_ITERATIONS; i++) {
        uint64_t iter_us = vsf_systimer_tick_to_us(deltas[i]);
        uint32_t iter_ns = (uint32_t)(iter_us * 1000ULL);
        VSF_TEST_TRACE_INFO("gpio_irq_latency:iter[%u]=%lu ns" VSF_TRACE_CFG_LINEEND,
                            (unsigned)i, (unsigned long)iter_ns);
        if (deltas[i] > anomaly_threshold_ticks && iter_ns <= p->max_latency_ns) {
            VSF_TEST_TRACE_WARNING(
                "gpio_irq_latency:iter[%u]=%lu ns > 2x median (possible cache miss)" VSF_TRACE_CFG_LINEEND,
                (unsigned)i, (unsigned long)iter_ns);
        }
    }

    uint64_t worst_us = vsf_systimer_tick_to_us(worst_ticks);
    uint32_t worst_ns = (uint32_t)(worst_us * 1000ULL);

    VSF_TEST_TRACE_INFO("gpio_irq_latency:median=%lu ns worst=%lu ns warn=%lu ns max=%lu ns" VSF_TRACE_CFG_LINEEND,
                         (unsigned long)median_ns,
                         (unsigned long)worst_ns,
                         (unsigned long)p->warn_latency_ns,
                         (unsigned long)p->max_latency_ns);
    if (worst_ns > p->max_latency_ns) {
        VSF_TEST_TRACE_ERROR("gpio_irq_latency:latency exceeds max (%lu > %lu)" VSF_TRACE_CFG_LINEEND,
                              (unsigned long)worst_ns, (unsigned long)p->max_latency_ns);
        VSF_TEST_ASSERT(false);
    } else if (worst_ns > p->warn_latency_ns) {
        VSF_TEST_TRACE_WARNING("gpio_irq_latency:latency exceeds warn (%lu > %lu), but within max" VSF_TRACE_CFG_LINEEND,
                                (unsigned long)worst_ns, (unsigned long)p->warn_latency_ns);
    }

    vsf_gpio_exti_irq_disable(gpio, pin_mask);
    vsf_gpio_set_input(gpio, pin_mask);
}

#endif /* VSF_TEST_GPIO_IRQ_LATENCY_ENABLE == ENABLED */

/* EOF */

