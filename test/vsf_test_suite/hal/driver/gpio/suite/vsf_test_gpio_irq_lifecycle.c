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

#include "vsf_test_gpio_irq_lifecycle.h"
#include "vsf_test_suites.h"


/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_GPIO_IRQ_LIFECYCLE_ENABLE == ENABLED
const struct vsf_test_gpio_irq_lifecycle_s {
    vsf_test_suite_t hdr;
    vsf_test_gpio_irq_lifecycle_params_t          params[VSF_TEST_GPIO_IRQ_LIFECYCLE_CASE_COUNT];
} vsf_test_gpio_irq_lifecycle = {
    .hdr = {
        .name            = "gpio_irq_lifecycle",
        .jmp_fn          = vsf_test_gpio_irq_lifecycle_run,
        .case_count      = VSF_TEST_GPIO_IRQ_LIFECYCLE_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_gpio_irq_lifecycle_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_GPIO,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_GPIO_IRQ_LIFECYCLE_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/



/*============================ LOCAL VARIABLES ===============================*/


#if VSF_TEST_GPIO_IRQ_LIFECYCLE_ENABLE == ENABLED

static void __lifecycle_handler(void *target, vsf_gpio_t *gpio, vsf_gpio_pin_mask_t pin_mask)
{
    vsf_test_suite_t *suite = target;
    if (pin_mask & vsf_test_suite_data.gpio.gpio_irq_lifecycle.lifecycle_pin) {
        vsf_test_suite_data.gpio.gpio_irq_lifecycle.lifecycle_count++;
    }
}

/*============================ IMPLEMENTATION ================================*/

void vsf_test_gpio_irq_lifecycle_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_gpio_irq_lifecycle_params_t *p = tc->params;
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_GPIO, gpio);
    vsf_gpio_t *gpio = inst->fixture.gpio;
    vsf_gpio_pin_mask_t pin_mask = (vsf_gpio_pin_mask_t)1u << p->pin;

    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware suites do not print them. */
    VSF_TEST_GPIO_ASSERT_CAPABILITY(gpio);

    VSF_TEST_TRACE_INFO("gpio_irq_lifecycle:start (case=%u pin=%u)" VSF_TRACE_CFG_LINEEND,
                         (unsigned)tc->case_idx, (unsigned)p->pin);

    /* Per-case state in suite: must be re-initialised before each run. */
    vsf_test_suite_data.gpio.gpio_irq_lifecycle.lifecycle_pin   = pin_mask;
    vsf_test_suite_data.gpio.gpio_irq_lifecycle.lifecycle_count = 0;

    /* config rising-edge */
    VSF_TEST_TRACE_DEBUG("gpio_irq_lifecycle:config exti" VSF_TRACE_CFG_LINEEND);
    vsf_err_t err = vsf_gpio_port_config_pins(gpio, pin_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_EXTI | VSF_GPIO_PULL_DOWN | VSF_GPIO_EXTI_MODE_RISING,
    });
    VSF_TEST_ASSERT_ERR_NONE(err,
        "gpio_irq_lifecycle:config exti failed (err=%d) (pin=%u pin_mask=0x%x)" VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)p->pin, (unsigned)pin_mask);

    VSF_TEST_TRACE_DEBUG("gpio_irq_lifecycle:irq_config" VSF_TRACE_CFG_LINEEND);
    err = vsf_gpio_exti_irq_config(gpio, &(vsf_gpio_exti_irq_cfg_t){
        .handler_fn = __lifecycle_handler,
        .target_ptr = NULL,
        .prio       = VSF_TEST_GPIO_IRQ_LIFECYCLE_PRIO,
    });
    VSF_TEST_ASSERT_ERR_NONE(err,
        "gpio_irq_lifecycle:irq_config failed (err=%d) (prio=%d)" VSF_TRACE_CFG_LINEEND, (int)err, (int)VSF_TEST_GPIO_IRQ_LIFECYCLE_PRIO);

    /* get_configuration round-trip */
    VSF_TEST_TRACE_DEBUG("gpio_irq_lifecycle:irq_get_configuration" VSF_TRACE_CFG_LINEEND);
    vsf_gpio_exti_irq_cfg_t got = {0};
    err = vsf_gpio_exti_irq_get_configuration(gpio, &got);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "gpio_irq_lifecycle:irq_get_configuration failed (err=%d) (pin=%u)" VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)p->pin);
    if (got.handler_fn != __lifecycle_handler) {
        VSF_TEST_TRACE_ERROR("gpio_irq_lifecycle:handler_fn mismatch (%p expected %p)" VSF_TRACE_CFG_LINEEND,
                              (void *)got.handler_fn, (void *)__lifecycle_handler);
    }
    VSF_TEST_ASSERT(got.handler_fn == __lifecycle_handler);

    /* Set the pin low, enable, then rising-edge trigger → count = 1. */
    VSF_TEST_TRACE_DEBUG("gpio_irq_lifecycle:trigger #1" VSF_TRACE_CFG_LINEEND);
    vsf_gpio_set_output(gpio, pin_mask);
    vsf_gpio_clear(gpio, pin_mask);
    vsf_test_busy_wait_ms(VSF_TEST_POLL_TICK_MS);
    vsf_gpio_exti_irq_clear(gpio, pin_mask);
    vsf_test_suite_data.gpio.gpio_irq_lifecycle.lifecycle_count = 0;
    err = vsf_gpio_exti_irq_enable(gpio, pin_mask);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "gpio_irq_lifecycle:irq_enable failed (err=%d) (pin=%u pin_mask=0x%x)" VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)p->pin, (unsigned)pin_mask);

    vsf_gpio_set(gpio, pin_mask);          /* rising edge */
    vsf_test_busy_wait_ms(VSF_TEST_POLL_TICK_MS);
    if (vsf_test_suite_data.gpio.gpio_irq_lifecycle.lifecycle_count != 1) {
        VSF_TEST_TRACE_ERROR("gpio_irq_lifecycle:after enable count=%lu (expected=1)" VSF_TRACE_CFG_LINEEND,
                              (unsigned long)vsf_test_suite_data.gpio.gpio_irq_lifecycle.lifecycle_count);
    }
    VSF_TEST_ASSERT(vsf_test_suite_data.gpio.gpio_irq_lifecycle.lifecycle_count == 1);

    /* Disable → next edge must NOT increment. */
    VSF_TEST_TRACE_DEBUG("gpio_irq_lifecycle:disable verify" VSF_TRACE_CFG_LINEEND);
    vsf_gpio_exti_irq_disable(gpio, pin_mask);
    vsf_gpio_clear(gpio, pin_mask);
    vsf_test_busy_wait_ms(VSF_TEST_POLL_TICK_MS);
    vsf_gpio_set(gpio, pin_mask);
    vsf_test_busy_wait_ms(VSF_TEST_POLL_TICK_MS);
    if (vsf_test_suite_data.gpio.gpio_irq_lifecycle.lifecycle_count != 1) {
        VSF_TEST_TRACE_ERROR("gpio_irq_lifecycle:after disable count=%lu (expected=1)" VSF_TRACE_CFG_LINEEND,
                              (unsigned long)vsf_test_suite_data.gpio.gpio_irq_lifecycle.lifecycle_count);
    }
    VSF_TEST_ASSERT(vsf_test_suite_data.gpio.gpio_irq_lifecycle.lifecycle_count == 1);

    /* Re-enable → next edge increments to 2. */
    VSF_TEST_TRACE_DEBUG("gpio_irq_lifecycle:re-enable #2" VSF_TRACE_CFG_LINEEND);
    vsf_gpio_clear(gpio, pin_mask);
    vsf_test_busy_wait_ms(VSF_TEST_POLL_TICK_MS);
    vsf_gpio_exti_irq_clear(gpio, pin_mask);
    err = vsf_gpio_exti_irq_enable(gpio, pin_mask);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "gpio_irq_lifecycle:re-enable failed (err=%d) (pin=%u pin_mask=0x%x)" VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)p->pin, (unsigned)pin_mask);
    vsf_gpio_set(gpio, pin_mask);
    vsf_test_busy_wait_ms(VSF_TEST_POLL_TICK_MS);
    if (vsf_test_suite_data.gpio.gpio_irq_lifecycle.lifecycle_count != 2) {
        VSF_TEST_TRACE_ERROR("gpio_irq_lifecycle:after re-enable count=%lu (expected=2)" VSF_TRACE_CFG_LINEEND,
                              (unsigned long)vsf_test_suite_data.gpio.gpio_irq_lifecycle.lifecycle_count);
    }
    VSF_TEST_ASSERT(vsf_test_suite_data.gpio.gpio_irq_lifecycle.lifecycle_count == 2);

    /* Clear API: write 1 to any pending bits, returns pre-clear mask. */
    vsf_gpio_exti_irq_disable(gpio, pin_mask);
    vsf_gpio_set_input(gpio, pin_mask);
    /* No pending after a clean disable+input switch — returning 0 here is OK. */
    (void)vsf_gpio_exti_irq_clear(gpio, pin_mask);

    VSF_TEST_TRACE_INFO("gpio_irq_lifecycle:pass count=%lu" VSF_TRACE_CFG_LINEEND,
                         (unsigned long)vsf_test_suite_data.gpio.gpio_irq_lifecycle.lifecycle_count);
}

#endif /* VSF_TEST_GPIO_IRQ_LIFECYCLE_ENABLE == ENABLED */

/* EOF */

