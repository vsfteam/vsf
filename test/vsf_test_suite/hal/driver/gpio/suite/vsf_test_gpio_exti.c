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

#include "vsf_test_gpio_exti.h"
#include "vsf_test_suites.h"


/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_GPIO_EXTI_ENABLE == ENABLED
const struct vsf_test_gpio_exti_s {
    vsf_test_suite_t hdr;
    vsf_test_gpio_exti_params_t          params[VSF_TEST_GPIO_EXTI_CASE_COUNT];
} vsf_test_gpio_exti = {
    .hdr = {
        .name            = "gpio_exti",
        .jmp_fn          = vsf_test_gpio_exti_run,
        .case_count      = VSF_TEST_GPIO_EXTI_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_gpio_exti_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_GPIO,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_GPIO_EXTI_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/



/*============================ LOCAL VARIABLES ===============================*/


#if VSF_TEST_GPIO_EXTI_ENABLE == ENABLED

static void __exti_handler(void *target, vsf_gpio_t *gpio, vsf_gpio_pin_mask_t pin_mask)

{
    vsf_test_suite_t *suite = target;
    if (pin_mask & vsf_test_suite_data.gpio.gpio_exti.expected_pin) {
        vsf_test_suite_data.gpio.gpio_exti.count++;
        if (vsf_test_suite_data.gpio.gpio_exti.disable_on_fire) {
            /* Level-trigger ISR storm guard. Disable the source after the
             * first hit; the main thread re-enables if it wants more. */
            vsf_gpio_exti_irq_disable(gpio, vsf_test_suite_data.gpio.gpio_exti.expected_pin);
        }
    }
}

/*============================ IMPLEMENTATION ================================*/

void vsf_test_gpio_exti_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_gpio_exti_params_t *p = tc->params;
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_GPIO, gpio);
    vsf_gpio_t *gpio = inst->fixture.gpio;
    vsf_gpio_pin_mask_t out_mask = (vsf_gpio_pin_mask_t)1u << p->out_pin;
    vsf_gpio_pin_mask_t in_mask  = (vsf_gpio_pin_mask_t)1u << p->in_pin;
    bool self_loopback = (p->out_pin == p->in_pin);

    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware suites do not print them. */
    VSF_TEST_GPIO_ASSERT_CAPABILITY(gpio);

    VSF_TEST_TRACE_INFO("gpio_exti:start (case=%u out=%u in=%u trig=0x%x)" VSF_TRACE_CFG_LINEEND,
                        (unsigned)tc->case_idx, (unsigned)p->out_pin, (unsigned)p->in_pin,
                        (unsigned)p->trigger_mode);

    bool active_low = (p->trigger_mode == VSF_GPIO_EXTI_MODE_FALLING
                    || p->trigger_mode == VSF_GPIO_EXTI_MODE_LOW_LEVEL);
    bool level_trig = (p->trigger_mode == VSF_GPIO_EXTI_MODE_LOW_LEVEL
                    || p->trigger_mode == VSF_GPIO_EXTI_MODE_HIGH_LEVEL);

    /* Per-case state in suite: must be re-initialised before each run. */
    vsf_test_suite_data.gpio.gpio_exti.count           = 0;
    vsf_test_suite_data.gpio.gpio_exti.expected_pin    = in_mask;
    vsf_test_suite_data.gpio.gpio_exti.disable_on_fire = level_trig;

    /* Pre-park the output pin at the IDLE level (opposite of "active") so
     * configuring EXTI does not see a spurious trigger. */
    VSF_TEST_TRACE_DEBUG("gpio_exti:config out_pin" VSF_TRACE_CFG_LINEEND);
    vsf_gpio_port_config_pins(gpio, out_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_OUTPUT_PUSH_PULL | VSF_GPIO_NO_PULL_UP_DOWN,
    });
    if (active_low) {
        vsf_gpio_set(gpio, out_mask);
    } else {
        vsf_gpio_clear(gpio, out_mask);
    }
    vsf_test_busy_wait_ms(VSF_TEST_POLL_TICK_MS);

    /* Configure in_pin as EXTI input with the requested trigger mode.
     * For self-loopback, the output driver continues driving the pin
     * while EXTI watches transitions. */
    VSF_TEST_TRACE_DEBUG("gpio_exti:config exti in_pin" VSF_TRACE_CFG_LINEEND);
    vsf_err_t err = vsf_gpio_port_config_pins(gpio, in_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_EXTI | VSF_GPIO_PULL_UP | p->trigger_mode,
    });
    VSF_TEST_ASSERT_ERR_NONE(err,
        "gpio_exti:config exti in_pin failed (err=%d) (in_pin=%u in_mask=0x%x trigger=0x%x)"
                             VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)p->in_pin, (unsigned)in_mask, (unsigned)p->trigger_mode);

    VSF_TEST_TRACE_DEBUG("gpio_exti:irq_config" VSF_TRACE_CFG_LINEEND);
    err = vsf_gpio_exti_irq_config(gpio, &(vsf_gpio_exti_irq_cfg_t){
        .handler_fn = __exti_handler,
        .target_ptr = NULL,
        .prio       = VSF_TEST_GPIO_EXTI_PRIO,
    });
    VSF_TEST_ASSERT_ERR_NONE(err,
        "gpio_exti:irq_config failed (err=%d) (prio=%d)"
                             VSF_TRACE_CFG_LINEEND, (int)err, (int)VSF_TEST_GPIO_EXTI_PRIO);

    /* For self-loopback, restore output drive on the same pin. */
    if (self_loopback) {
        VSF_TEST_TRACE_DEBUG("gpio_exti:self_loopback set_output" VSF_TRACE_CFG_LINEEND);
        vsf_gpio_set_output(gpio, in_mask);
    }

    VSF_TEST_TRACE_DEBUG("gpio_exti:irq_enable" VSF_TRACE_CFG_LINEEND);
    err = vsf_gpio_exti_irq_enable(gpio, in_mask);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "gpio_exti:irq_enable failed (err=%d) (in_pin=%u in_mask=0x%x)"
                             VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)p->in_pin, (unsigned)in_mask);

    /* Clear any pending edge that the mode switch produced. */
    vsf_gpio_exti_irq_clear(gpio, in_mask);
    vsf_test_suite_data.gpio.gpio_exti.count = 0;

    /* Drive to ACTIVE state — trigger one event (edge) or sustained ISRs
     * (level, but handler self-disables after first hit). */
    VSF_TEST_TRACE_DEBUG("gpio_exti:drive active" VSF_TRACE_CFG_LINEEND);
    if (active_low) {
        vsf_gpio_clear(gpio, out_mask);
    } else {
        vsf_gpio_set(gpio, out_mask);
    }
    vsf_test_busy_wait_ms(VSF_TEST_POLL_TICK_MS);
    uint32_t after_active = vsf_test_suite_data.gpio.gpio_exti.count;
    if (after_active < 1) {
        VSF_TEST_TRACE_ERROR("gpio_exti:active count=%lu (expected >=1)"
                             VSF_TRACE_CFG_LINEEND, (unsigned long)after_active);
    }
    VSF_TEST_ASSERT(after_active >= 1);

    /* For dual-edge mode, also drive the opposite transition and expect
     * a second hit. */
    if (p->trigger_mode == VSF_GPIO_EXTI_MODE_RISING_FALLING) {
        VSF_TEST_TRACE_DEBUG("gpio_exti:dual-edge opposite transition" VSF_TRACE_CFG_LINEEND);
        vsf_gpio_exti_irq_clear(gpio, in_mask);
        if (active_low) {
            vsf_gpio_set(gpio, out_mask);
        } else {
            vsf_gpio_clear(gpio, out_mask);
        }
        vsf_test_busy_wait_ms(VSF_TEST_POLL_TICK_MS);
        uint32_t count_now = vsf_test_suite_data.gpio.gpio_exti.count;
        if (count_now <= after_active) {
            VSF_TEST_TRACE_ERROR("gpio_exti:dual-edge count=%lu (expected >%lu)"
                                 VSF_TRACE_CFG_LINEEND, (unsigned long)count_now, (unsigned long)after_active);
        }
        VSF_TEST_ASSERT(count_now > after_active);
    }

    /* For edge-triggered (single-edge) modes, also verify the IRQ truly
     * stays disabled after vsf_gpio_exti_irq_disable: drive the opposite
     * edge then the active edge again, expect no count change. */
    if (!level_trig && p->trigger_mode != VSF_GPIO_EXTI_MODE_RISING_FALLING) {
        VSF_TEST_TRACE_DEBUG("gpio_exti:irq_disable verify" VSF_TRACE_CFG_LINEEND);
        vsf_gpio_exti_irq_disable(gpio, in_mask);
        uint32_t baseline = vsf_test_suite_data.gpio.gpio_exti.count;
        /* idle → active → idle → active (extra noise) */
        if (active_low) {
            vsf_gpio_set(gpio, out_mask);
            vsf_test_busy_wait_ms(VSF_TEST_POLL_TICK_MS);
            vsf_gpio_clear(gpio, out_mask);
        } else {
            vsf_gpio_clear(gpio, out_mask);
            vsf_test_busy_wait_ms(VSF_TEST_POLL_TICK_MS);
            vsf_gpio_set(gpio, out_mask);
        }
        vsf_test_busy_wait_ms(VSF_TEST_POLL_TICK_MS);
        vsf_gpio_exti_irq_clear(gpio, in_mask);
        uint32_t count_now = vsf_test_suite_data.gpio.gpio_exti.count;
        if (count_now != baseline) {
            VSF_TEST_TRACE_ERROR("gpio_exti:disabled count=%lu (expected=%lu)"
                                 VSF_TRACE_CFG_LINEEND, (unsigned long)count_now, (unsigned long)baseline);
        }
        VSF_TEST_ASSERT(count_now == baseline);
    }

    /* Phase-3 API completeness check (usart-gpio-coverage-gaps PRD):
     * get_configuration() round-trip. */
    VSF_TEST_TRACE_DEBUG("gpio_exti:irq_get_configuration" VSF_TRACE_CFG_LINEEND);
    vsf_gpio_exti_irq_cfg_t got = {0};
    err = vsf_gpio_exti_irq_get_configuration(gpio, &got);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "gpio_exti:irq_get_configuration failed (err=%d) (in_pin=%u)"
                             VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)p->in_pin);
    if (got.handler_fn != __exti_handler) {
        VSF_TEST_TRACE_ERROR("gpio_exti:handler_fn mismatch (%p expected %p)"
                             VSF_TRACE_CFG_LINEEND, (void *)got.handler_fn, (void *)__exti_handler);
    }
    VSF_TEST_ASSERT(got.handler_fn == __exti_handler);

    /* Cleanup. */
    vsf_gpio_exti_irq_disable(gpio, in_mask);
    vsf_gpio_set_input(gpio, in_mask);

    VSF_TEST_TRACE_INFO("gpio_exti:pass trigger=0x%x count=%lu" VSF_TRACE_CFG_LINEEND,
                        (unsigned)p->trigger_mode, (unsigned long)vsf_test_suite_data.gpio.gpio_exti.count);
}

#endif /* VSF_TEST_GPIO_EXTI_ENABLE == ENABLED */

/* EOF */

