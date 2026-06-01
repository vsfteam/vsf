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

#include "vsf_test_gpio_io_check.h"
#include "vsf_test_suites.h"


/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_GPIO_IO_CHECK_ENABLE == ENABLED
const struct vsf_test_gpio_io_check_s {
    vsf_test_suite_t hdr;
    vsf_test_gpio_io_check_params_t          params[VSF_TEST_GPIO_IO_CHECK_CASE_COUNT];
} vsf_test_gpio_io_check = {
    .hdr = {
        .name            = "gpio_io_check",
        .jmp_fn          = vsf_test_gpio_io_check_run,
        .case_count      = VSF_TEST_GPIO_IO_CHECK_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_gpio_io_check_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_GPIO,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_GPIO_IO_CHECK_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/


/*============================ LOCAL VARIABLES ===============================*/

static void __gpio_bitbang_uart_byte(vsf_gpio_t *gpio,
                                     vsf_gpio_pin_mask_t pin_mask,
                                     uint8_t byte,
                                     uint32_t bit_period_us)
{
    /* Start bit (low). */
    vsf_gpio_clear(gpio, pin_mask);
    vsf_test_busy_wait_us(bit_period_us);

    /* 8 data bits, LSB first. */
    for (uint8_t bit = 0; bit < 8; bit++) {
        if (byte & (1u << bit)) {
            vsf_gpio_set(gpio, pin_mask);
        } else {
            vsf_gpio_clear(gpio, pin_mask);
        }
        vsf_test_busy_wait_us(bit_period_us);
    }

    /* Stop bit (high). */
    vsf_gpio_set(gpio, pin_mask);
    vsf_test_busy_wait_us(bit_period_us);
}



#if VSF_TEST_GPIO_IO_CHECK_ENABLE == ENABLED

/*============================ MACROS ========================================*/

/* Number of rounds to repeat the per-pin bit-bang sequence.
 * Multiple rounds ensure at least one full capture even if the LA
 * started mid-round. */

/*============================ IMPLEMENTATION ================================*/

void vsf_test_gpio_io_check_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_gpio_io_check_params_t *p = tc->params;
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_GPIO, gpio);
    vsf_gpio_t *gpio = inst->fixture.gpio;
    vsf_gpio_pin_mask_t pin_mask = (vsf_gpio_pin_mask_t)1u << p->pin;

    /* Bit period = 1e6 / baudrate (microseconds).
     * 115200 baud → ~8.68 µs.  Round-to-nearest to keep the actual baudrate
     * within the DSView UART decoder tolerance (≈ ±3 %).
     *
     * Pins declared in gpio.yml: GP8 (uart1_tx) and GP9 (uart1_rx).
     * Both are safe as GPIO output when UART1 is not in use. */
    uint32_t bit_period_us = (1000000u + p->baudrate / 2) / p->baudrate;
    uint8_t byte = VSF_TEST_GPIO_IO_CHECK_PATTERN_BYTE + p->pin;

    VSF_TEST_TRACE_INFO("gpio_io_check:start (case=%u pin=%u baudrate=%lu)" VSF_TRACE_CFG_LINEEND,
                         (unsigned)tc->case_idx, (unsigned)p->pin, (unsigned long)p->baudrate);

    /* Configure pin as push-pull output, idle high (UART idle state). */
    VSF_TEST_TRACE_DEBUG("gpio_io_check:config pin" VSF_TRACE_CFG_LINEEND);
    vsf_gpio_port_config_pins(gpio, pin_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_OUTPUT_PUSH_PULL | VSF_GPIO_NO_PULL_UP_DOWN,
    });
    vsf_gpio_set(gpio, pin_mask);

    /* Bit-bang the unique byte for this pin across multiple rounds. */
    for (uint8_t round = 0; round < VSF_TEST_GPIO_IO_CHECK_ROUNDS; round++) {
        __gpio_bitbang_uart_byte(gpio, pin_mask, byte, bit_period_us);
        vsf_test_busy_wait_us(VSF_TEST_GPIO_IO_CHECK_ROUND_GAP_US);
    }

    VSF_TEST_TRACE_INFO("gpio_io_check:pass pin=%u byte=0x%02x rounds=%u" VSF_TRACE_CFG_LINEEND,
                         (unsigned)p->pin, (unsigned)byte,
                         (unsigned)VSF_TEST_GPIO_IO_CHECK_ROUNDS);
}

#endif /* VSF_TEST_GPIO_IO_CHECK_ENABLE == ENABLED */

/* EOF */
