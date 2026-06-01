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

#include "vsf_test_gpio_pinmux.h"
#include "vsf_test_suites.h"
#include "vsf_board.h"


/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_GPIO_PINMUX_ENABLE == ENABLED
const struct vsf_test_gpio_pinmux_s {
    vsf_test_suite_t hdr;
    vsf_test_gpio_pinmux_params_t          params[VSF_TEST_GPIO_PINMUX_CASE_COUNT];
} vsf_test_gpio_pinmux = {
    .hdr = {
        .name            = "gpio_pinmux",
        .jmp_fn          = vsf_test_gpio_pinmux_run,
        .case_count      = VSF_TEST_GPIO_PINMUX_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_gpio_pinmux_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_GPIO_PINMUX,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_GPIO_PINMUX_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/



#if VSF_TEST_GPIO_PINMUX_ENABLE == ENABLED


/*============================ IMPLEMENTATION ================================*/

void vsf_test_gpio_pinmux_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_gpio_pinmux_params_t *p = tc->params;
    VSF_TEST_ASSERT(inst != NULL);
    VSF_TEST_ASSERT(inst->peripheral_type == VSF_PERIPHERAL_TYPE_GPIO_PINMUX);
    VSF_TEST_ASSERT(inst->fixture.gpio_pinmux != NULL);
    VSF_TEST_ASSERT(inst->fixture.gpio_pinmux->gpio != NULL);
    VSF_TEST_ASSERT(inst->fixture.gpio_pinmux->usart != NULL);
    VSF_TEST_ASSERT(inst->name != NULL);
    const vsf_test_gpio_pinmux_ctx_t *ctx = inst->fixture.gpio_pinmux;
    vsf_gpio_t *gpio = ctx->gpio;
    vsf_usart_t *usart = ctx->usart;
    vsf_gpio_pin_mask_t tx_mask = (vsf_gpio_pin_mask_t)1u << p->tx_pin;
    vsf_gpio_pin_mask_t rx_mask = (vsf_gpio_pin_mask_t)1u << p->rx_pin;

    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware suites do not print them. */
    VSF_TEST_GPIO_ASSERT_CAPABILITY(gpio);

    VSF_TEST_TRACE_INFO("gpio_pinmux:start (case=%u tx=%u rx=%u)" VSF_TRACE_CFG_LINEEND,
                         (unsigned)tc->case_idx, (unsigned)p->tx_pin, (unsigned)p->rx_pin);

    /* Step 1: drive the pins as plain GPIO output to prove they are
     * controllable before we hand them to the UART peripheral. */
    VSF_TEST_TRACE_DEBUG("gpio_pinmux:drive as gpio" VSF_TRACE_CFG_LINEEND);
    vsf_gpio_port_config_pins(gpio, tx_mask | rx_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_OUTPUT_PUSH_PULL | VSF_GPIO_NO_PULL_UP_DOWN,
    });
    vsf_gpio_set(gpio, tx_mask | rx_mask);
    vsf_test_busy_wait_ms(VSF_TEST_POLL_TICK_MS);
    vsf_gpio_clear(gpio, tx_mask | rx_mask);
    vsf_test_busy_wait_ms(VSF_TEST_POLL_TICK_MS);

    /* Step 2: configure to UART AF via the alternate_function field. */
    VSF_TEST_TRACE_DEBUG("gpio_pinmux:config af" VSF_TRACE_CFG_LINEEND);
    vsf_err_t err = vsf_gpio_port_config_pins(gpio, tx_mask | rx_mask, &(vsf_gpio_cfg_t){
        .mode = VSF_GPIO_AF,
        .alternate_function = vsf_board_get_uart_funcsel(),
    });
    VSF_TEST_ASSERT_ERR_NONE(err,
        "gpio_pinmux:config af failed (err=%d) (tx_pin=%u rx_pin=%u)" VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)p->tx_pin, (unsigned)p->rx_pin);

    /* Step 3: bring up UART and send a small payload. We do not assert
     * loopback receive here because the host script handles the
     * post-condition (UART line bytes captured on LA / serial). */
    VSF_TEST_TRACE_DEBUG("gpio_pinmux:uart init" VSF_TRACE_CFG_LINEEND);
    err = vsf_usart_init(usart, &(vsf_usart_cfg_t){
        .mode     = VSF_USART_8_BIT_LENGTH | VSF_USART_1_STOPBIT
                  | VSF_USART_NO_PARITY    | VSF_USART_TX_ENABLE,
        .baudrate = VSF_TEST_GPIO_PINMUX_DEFAULT_BAUDRATE,
    });
    VSF_TEST_ASSERT_ERR_NONE(err,
        "gpio_pinmux:uart init failed (err=%d) (baudrate=%lu)" VSF_TRACE_CFG_LINEEND, (int)err, (unsigned long)VSF_TEST_GPIO_PINMUX_DEFAULT_BAUDRATE);
    while (fsm_rt_cpl != vsf_usart_enable(usart));

    VSF_TEST_TRACE_DEBUG("gpio_pinmux:send payload" VSF_TRACE_CFG_LINEEND);
    const char *payload = "PINMUX\r\n";
    while (*payload) {
        while (!vsf_usart_txfifo_get_free_count(usart));
        vsf_usart_txfifo_write(usart, (uint8_t *)payload, 1);
        payload++;
    }
    vsf_test_busy_wait_ms(VSF_TEST_GPIO_PINMUX_LOOPBACK_SETTLE_MS);

    /* Tear down so subsequent suites get UART1 in a clean state. Without
     * this, later RX-on-UART1 scenarios (rx_baud, rx_data, ...) fail
     * because the peripheral is left enabled in TX-only mode. */
    VSF_TEST_TRACE_DEBUG("gpio_pinmux:uart fini" VSF_TRACE_CFG_LINEEND);
    while (fsm_rt_cpl != vsf_usart_disable(usart));
    vsf_usart_fini(usart);

    VSF_TEST_TRACE_INFO("gpio_pinmux:pass" VSF_TRACE_CFG_LINEEND);
}

#endif /* VSF_TEST_GPIO_PINMUX_ENABLE == ENABLED */

/* EOF */


