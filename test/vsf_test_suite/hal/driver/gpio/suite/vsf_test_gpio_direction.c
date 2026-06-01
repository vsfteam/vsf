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

#include "vsf_test_gpio_direction.h"
#include "vsf_test_suites.h"


/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_GPIO_DIRECTION_ENABLE == ENABLED
const struct vsf_test_gpio_direction_s {
    vsf_test_suite_t hdr;
    vsf_test_gpio_direction_params_t          params[VSF_TEST_GPIO_DIRECTION_CASE_COUNT];
} vsf_test_gpio_direction = {
    .hdr = {
        .name            = "gpio_direction",
        .jmp_fn          = vsf_test_gpio_direction_run,
        .case_count      = VSF_TEST_GPIO_DIRECTION_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_gpio_direction_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_GPIO,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_GPIO_DIRECTION_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/



#if VSF_TEST_GPIO_DIRECTION_ENABLE == ENABLED


/*============================ IMPLEMENTATION ================================*/

void vsf_test_gpio_direction_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_gpio_direction_params_t *p = tc->params;
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_GPIO, gpio);
    vsf_gpio_t *gpio = inst->fixture.gpio;
    vsf_gpio_pin_mask_t pin_mask = (vsf_gpio_pin_mask_t)1u << p->pin;

    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware suites do not print them. */
    VSF_TEST_GPIO_ASSERT_CAPABILITY(gpio);

    VSF_TEST_TRACE_INFO("gpio_direction:start (case=%u pin=%u)" VSF_TRACE_CFG_LINEEND,
                        (unsigned)tc->case_idx, (unsigned)p->pin);

    /* Configure as output and verify direction reads back as output */
    VSF_TEST_TRACE_DEBUG("gpio_direction:config output" VSF_TRACE_CFG_LINEEND);
    vsf_err_t err = vsf_gpio_port_config_pins(gpio, pin_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_OUTPUT_PUSH_PULL | VSF_GPIO_NO_PULL_UP_DOWN,
    });
    VSF_TEST_ASSERT_ERR_NONE(err,
        "gpio_direction:config output failed (err=%d) (pin=%u pin_mask=0x%x)"
                             VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)p->pin, (unsigned)pin_mask);
    {
        vsf_gpio_pin_mask_t dir = vsf_gpio_get_direction(gpio, pin_mask) & pin_mask;
        if (dir != pin_mask) {
            VSF_TEST_TRACE_ERROR("gpio_direction:config output dir=%lu (expected=%lu)"
                                 VSF_TRACE_CFG_LINEEND, (unsigned long)dir, (unsigned long)pin_mask);
        }
        VSF_TEST_ASSERT(dir == pin_mask);
    }

    /* Switch direction → expect input */
    VSF_TEST_TRACE_DEBUG("gpio_direction:switch_direction #1" VSF_TRACE_CFG_LINEEND);
    vsf_gpio_switch_direction(gpio, pin_mask);
    {
        vsf_gpio_pin_mask_t dir = vsf_gpio_get_direction(gpio, pin_mask) & pin_mask;
        if (dir != 0) {
            VSF_TEST_TRACE_ERROR("gpio_direction:switch #1 dir=%lu (expected=0)"
                                 VSF_TRACE_CFG_LINEEND, (unsigned long)dir);
        }
        VSF_TEST_ASSERT(dir == 0);
    }

    /* Switch again → back to output */
    VSF_TEST_TRACE_DEBUG("gpio_direction:switch_direction #2" VSF_TRACE_CFG_LINEEND);
    vsf_gpio_switch_direction(gpio, pin_mask);
    {
        vsf_gpio_pin_mask_t dir = vsf_gpio_get_direction(gpio, pin_mask) & pin_mask;
        if (dir != pin_mask) {
            VSF_TEST_TRACE_ERROR("gpio_direction:switch #2 dir=%lu (expected=%lu)"
                                 VSF_TRACE_CFG_LINEEND, (unsigned long)dir, (unsigned long)pin_mask);
        }
        VSF_TEST_ASSERT(dir == pin_mask);
    }

    /* set_input / set_output explicit calls */
    VSF_TEST_TRACE_DEBUG("gpio_direction:set_input" VSF_TRACE_CFG_LINEEND);
    vsf_gpio_set_input(gpio, pin_mask);
    {
        vsf_gpio_pin_mask_t dir = vsf_gpio_get_direction(gpio, pin_mask) & pin_mask;
        if (dir != 0) {
            VSF_TEST_TRACE_ERROR("gpio_direction:set_input dir=%lu (expected=0)"
                                 VSF_TRACE_CFG_LINEEND, (unsigned long)dir);
        }
        VSF_TEST_ASSERT(dir == 0);
    }
    VSF_TEST_TRACE_DEBUG("gpio_direction:set_output" VSF_TRACE_CFG_LINEEND);
    vsf_gpio_set_output(gpio, pin_mask);
    {
        vsf_gpio_pin_mask_t dir = vsf_gpio_get_direction(gpio, pin_mask) & pin_mask;
        if (dir != pin_mask) {
            VSF_TEST_TRACE_ERROR("gpio_direction:set_output dir=%lu (expected=%lu)"
                                 VSF_TRACE_CFG_LINEEND, (unsigned long)dir, (unsigned long)pin_mask);
        }
        VSF_TEST_ASSERT(dir == pin_mask);
    }

    /* get_pin_configuration round-trip: configure as input pull-up, verify */
    VSF_TEST_TRACE_DEBUG("gpio_direction:config input+pu" VSF_TRACE_CFG_LINEEND);
    err = vsf_gpio_port_config_pins(gpio, pin_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_INPUT | VSF_GPIO_PULL_UP,
    });
    VSF_TEST_ASSERT_ERR_NONE(err,
        "gpio_direction:config input+pu failed (err=%d) (pin=%u pin_mask=0x%x)"
                             VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)p->pin, (unsigned)pin_mask);

    VSF_TEST_TRACE_DEBUG("gpio_direction:get_pin_configuration" VSF_TRACE_CFG_LINEEND);
    vsf_gpio_cfg_t got = { 0 };
    err = vsf_gpio_get_pin_configuration(gpio, p->pin, &got);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "gpio_direction:get_pin_configuration failed (err=%d) (pin=%u)"
                             VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)p->pin);
    {
        uint32_t actual_mode = got.mode & VSF_GPIO_MODE_MASK;
        if (actual_mode != VSF_GPIO_INPUT) {
            VSF_TEST_TRACE_ERROR("gpio_direction:mode mismatch (actual=%lu expected=%lu)"
                                 VSF_TRACE_CFG_LINEEND, (unsigned long)actual_mode, (unsigned long)VSF_GPIO_INPUT);
        }
        VSF_TEST_ASSERT(actual_mode == VSF_GPIO_INPUT);
    }
    {
        uint32_t actual_pull = got.mode & VSF_GPIO_PULL_UP_DOWN_MASK;
        if (actual_pull != VSF_GPIO_PULL_UP) {
            VSF_TEST_TRACE_ERROR("gpio_direction:pull mismatch (actual=%lu expected=%lu)"
                                 VSF_TRACE_CFG_LINEEND, (unsigned long)actual_pull, (unsigned long)VSF_GPIO_PULL_UP);
        }
        VSF_TEST_ASSERT(actual_pull == VSF_GPIO_PULL_UP);
    }

    VSF_TEST_TRACE_INFO("gpio_direction:pass" VSF_TRACE_CFG_LINEEND);
}

#endif /* VSF_TEST_GPIO_DIRECTION_ENABLE == ENABLED */

/* EOF */


