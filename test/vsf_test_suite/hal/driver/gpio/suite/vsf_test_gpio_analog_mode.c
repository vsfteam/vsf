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

#include "vsf_test_gpio_analog_mode.h"
#include "vsf_test_suites.h"


/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_GPIO_ANALOG_MODE_ENABLE == ENABLED
const struct vsf_test_gpio_analog_mode_s {
    vsf_test_suite_t hdr;
    vsf_test_gpio_analog_mode_params_t          params[VSF_TEST_GPIO_ANALOG_MODE_CASE_COUNT];
} vsf_test_gpio_analog_mode = {
    .hdr = {
        .name            = "gpio_analog_mode",
        .jmp_fn          = vsf_test_gpio_analog_mode_run,
        .case_count      = VSF_TEST_GPIO_ANALOG_MODE_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_gpio_analog_mode_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_GPIO,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_GPIO_ANALOG_MODE_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/



#if VSF_TEST_GPIO_ANALOG_MODE_ENABLE == ENABLED


/*============================ IMPLEMENTATION ================================*/

/* VSF_GPIO_ANALOG must disconnect the digital input buffer from the pad.
 * With the internal pull-up enabled, the pad voltage is HIGH, yet
 * vsf_gpio_read() must report 0 — proving the read path is truly cut
 * from the pad. With pull-down enabled, read() also returns 0 (sanity
 * check, line is low anyway).
 *
 * After re-configuring as INPUT with pull-up, read() must report 1 —
 * confirming we can recover the digital input path on the same pin. */
void vsf_test_gpio_analog_mode_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_gpio_analog_mode_params_t *p = tc->params;
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_GPIO, gpio);
    vsf_gpio_t *gpio = inst->fixture.gpio;
    vsf_gpio_pin_mask_t pin_mask = (vsf_gpio_pin_mask_t)1u << p->pin;

    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware suites do not print them. */
    VSF_TEST_GPIO_ASSERT_CAPABILITY(gpio);

    VSF_TEST_TRACE_INFO("gpio_analog_mode:start (case=%u pin=%u)" VSF_TRACE_CFG_LINEEND,
                        (unsigned)tc->case_idx, (unsigned)p->pin);

    /* ANALOG mode with internal pull-up. The pad is electrically high,
     * but the input buffer is OFF so vsf_gpio_read() must report 0. */
    VSF_TEST_TRACE_DEBUG("gpio_analog_mode:config analog+pu" VSF_TRACE_CFG_LINEEND);
    vsf_err_t err = vsf_gpio_port_config_pins(gpio, pin_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_ANALOG | VSF_GPIO_PULL_UP,
    });
    VSF_TEST_ASSERT_ERR_NONE(err,
        "gpio_analog_mode:config analog+pu failed (err=%d) (pin=%u)"
                             VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)p->pin);
    vsf_test_busy_wait_ms(VSF_TEST_POLL_TICK_MS);
    {
        uint32_t rd_val = vsf_gpio_read(gpio) & pin_mask;
        if (rd_val != 0) {
            VSF_TEST_TRACE_ERROR("gpio_analog_mode:analog+pu read=%lu (expected=0)"
                                 VSF_TRACE_CFG_LINEEND, (unsigned long)rd_val);
        }
        VSF_TEST_ASSERT(rd_val == 0);
    }

    /* ANALOG mode with pull-down — also reads 0. */
    VSF_TEST_TRACE_DEBUG("gpio_analog_mode:config analog+pd" VSF_TRACE_CFG_LINEEND);
    err = vsf_gpio_port_config_pins(gpio, pin_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_ANALOG | VSF_GPIO_PULL_DOWN,
    });
    VSF_TEST_ASSERT_ERR_NONE(err,
        "gpio_analog_mode:config analog+pd failed (err=%d) (pin=%u)"
                             VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)p->pin);
    vsf_test_busy_wait_ms(VSF_TEST_POLL_TICK_MS);
    {
        uint32_t rd_val = vsf_gpio_read(gpio) & pin_mask;
        if (rd_val != 0) {
            VSF_TEST_TRACE_ERROR("gpio_analog_mode:analog+pd read=%lu (expected=0)"
                                 VSF_TRACE_CFG_LINEEND, (unsigned long)rd_val);
        }
        VSF_TEST_ASSERT(rd_val == 0);
    }

    /* Lifecycle: switch back to digital INPUT with pull-up. The input
     * buffer must come back on — read() now reports 1. */
    VSF_TEST_TRACE_DEBUG("gpio_analog_mode:config input+pu" VSF_TRACE_CFG_LINEEND);
    err = vsf_gpio_port_config_pins(gpio, pin_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_INPUT | VSF_GPIO_PULL_UP,
    });
    VSF_TEST_ASSERT_ERR_NONE(err,
        "gpio_analog_mode:config input+pu failed (err=%d) (pin=%u)"
                             VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)p->pin);
    vsf_test_busy_wait_ms(VSF_TEST_POLL_TICK_MS);
    {
        uint32_t rd_val = vsf_gpio_read(gpio) & pin_mask;
        if (rd_val != pin_mask) {
            VSF_TEST_TRACE_ERROR("gpio_analog_mode:input+pu read=%lu (expected=%lu)"
                                 VSF_TRACE_CFG_LINEEND, (unsigned long)rd_val, (unsigned long)pin_mask);
        }
        VSF_TEST_ASSERT(rd_val == pin_mask);
    }

    /* get_pin_configuration round-trip: after switching back to ANALOG,
     * the read-back mode should reflect ANALOG. */
    VSF_TEST_TRACE_DEBUG("gpio_analog_mode:config analog+nopu" VSF_TRACE_CFG_LINEEND);
    err = vsf_gpio_port_config_pins(gpio, pin_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_ANALOG | VSF_GPIO_NO_PULL_UP_DOWN,
    });
    VSF_TEST_ASSERT_ERR_NONE(err,
        "gpio_analog_mode:config analog+nopu failed (err=%d) (pin=%u)"
                             VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)p->pin);

    VSF_TEST_TRACE_DEBUG("gpio_analog_mode:get_pin_configuration" VSF_TRACE_CFG_LINEEND);
    vsf_gpio_cfg_t got = { 0 };
    err = vsf_gpio_get_pin_configuration(gpio, p->pin, &got);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "gpio_analog_mode:get_pin_configuration failed (err=%d) (pin=%u)"
                             VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)p->pin);
    {
        uint32_t actual_mode = got.mode & VSF_GPIO_MODE_MASK;
        if (actual_mode != VSF_GPIO_ANALOG) {
            VSF_TEST_TRACE_ERROR("gpio_analog_mode:mode mismatch (actual=%lu expected=%lu)"
                                 VSF_TRACE_CFG_LINEEND, (unsigned long)actual_mode, (unsigned long)VSF_GPIO_ANALOG);
        }
        VSF_TEST_ASSERT(actual_mode == VSF_GPIO_ANALOG);
    }

    VSF_TEST_TRACE_INFO("gpio_analog_mode:pass" VSF_TRACE_CFG_LINEEND);

    /* Cleanup: leave the pin as plain INPUT so the next suite starts clean. */
    vsf_gpio_port_config_pins(gpio, pin_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_INPUT | VSF_GPIO_NO_PULL_UP_DOWN,
    });
}

#endif /* VSF_TEST_GPIO_ANALOG_MODE_ENABLE == ENABLED */

/* EOF */


