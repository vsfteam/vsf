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

#include "vsf_test_usart_rx_mode.h"
#include "vsf_test_suites.h"

/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_USART_RX_MODE_ENABLE == ENABLED
const struct vsf_test_usart_rx_mode_s {
    vsf_test_suite_t hdr;
    vsf_test_usart_rx_mode_params_t          params[VSF_TEST_USART_RX_MODE_CASE_COUNT];
} vsf_test_usart_rx_mode = {
    .hdr = {
        .name            = "usart_rx_mode",
        .jmp_fn          = vsf_test_usart_rx_mode_run,
        .case_count      = VSF_TEST_USART_RX_MODE_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_usart_rx_mode_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_USART,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_USART_RX_MODE_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/

#if VSF_TEST_USART_RX_MODE_ENABLE == ENABLED

/*============================ MACROS ========================================*/

/*============================ LOCAL VARIABLES ===============================*/

/*============================ IMPLEMENTATION ================================*/

void vsf_test_usart_rx_mode_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_usart_rx_mode_params_t *p = tc->params;

    VSF_TEST_TRACE_INFO("usart_rx_mode:start (case=%u)" VSF_TRACE_CFG_LINEEND, (unsigned)tc->case_idx);

    VSF_TEST_TRACE_DEBUG("usart_rx_mode:vsf_usart_init (mode=0x%x baudrate=%lu)" VSF_TRACE_CFG_LINEEND,
                         (unsigned)p->mode, (unsigned long)VSF_TEST_RX_MODE_DEFAULT_BAUDRATE);
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_USART, usart);
    vsf_err_t err = vsf_usart_init(inst->fixture.usart, &(vsf_usart_cfg_t){
        .mode     = p->mode,
        .baudrate = VSF_TEST_RX_MODE_DEFAULT_BAUDRATE,
    });

    if (p->expect_pass) {
        VSF_TEST_ASSERT_ERR_NONE(err,
            "usart_rx_mode:vsf_usart_init failed (err=%d) (mode=0x%x baudrate=%lu)" VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)p->mode, (unsigned long)VSF_TEST_RX_MODE_DEFAULT_BAUDRATE);
        VSF_TEST_TRACE_DEBUG("usart_rx_mode:vsf_usart_enable" VSF_TRACE_CFG_LINEEND);
        while (fsm_rt_cpl != vsf_usart_enable(inst->fixture.usart));

        uint8_t rx_buf[32];
        uint16_t rx_len = 0;
        const char *expected = VSF_TEST_RX_MODE_PAYLOAD;
        uint16_t expected_len = strlen(expected);

        uint32_t elapsed_ms = 0;
        const uint32_t max_ms = VSF_TEST_RX_MODE_PAYLOAD_DRAIN_MS * 10;
        while (rx_len < expected_len && elapsed_ms < max_ms) {
            uint_fast16_t count = vsf_usart_rxfifo_get_data_count(inst->fixture.usart);
            while (count-- > 0 && rx_len < sizeof(rx_buf)) {
                vsf_usart_rxfifo_read(inst->fixture.usart, &rx_buf[rx_len], 1);
                rx_len++;
            }
            vsf_test_busy_wait_ms(VSF_TEST_USART_RX_MODE_BUSY_WAIT_MS);
            elapsed_ms += 10;
        }

        VSF_TEST_TRACE_DEBUG("usart_rx_mode:vsf_usart_rxfifo_read (expected_len=%u rx_len=%u)" VSF_TRACE_CFG_LINEEND,
                             (unsigned)expected_len, (unsigned)rx_len);
        if (rx_len != expected_len) {
            VSF_TEST_TRACE_ERROR("usart_rx_mode:rx_len mismatch (expected=%u actual=%u)" VSF_TRACE_CFG_LINEEND,
                                 (unsigned)expected_len, (unsigned)rx_len);
        }
        VSF_TEST_ASSERT(rx_len == expected_len);
        if (memcmp(rx_buf, expected, expected_len) != 0) {
            VSF_TEST_TRACE_ERROR("usart_rx_mode:payload mismatch" VSF_TRACE_CFG_LINEEND);
        }
        VSF_TEST_ASSERT(memcmp(rx_buf, expected, expected_len) == 0);

        VSF_TEST_TRACE_DEBUG("usart_rx_mode:vsf_usart_disable" VSF_TRACE_CFG_LINEEND);
        while (fsm_rt_cpl != vsf_usart_disable(inst->fixture.usart));
    } else {
        if (err == VSF_ERR_NONE) {
            VSF_TEST_TRACE_ERROR("usart_rx_mode:vsf_usart_init unexpectedly succeeded (err=%d) (mode=0x%x baudrate=%lu)" VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)p->mode, (unsigned long)VSF_TEST_RX_MODE_DEFAULT_BAUDRATE);
        }
        VSF_TEST_ASSERT(err != VSF_ERR_NONE);
    }
    VSF_TEST_TRACE_DEBUG("usart_rx_mode:vsf_usart_fini" VSF_TRACE_CFG_LINEEND);
    vsf_usart_fini(inst->fixture.usart);

    VSF_TEST_TRACE_INFO("usart_rx_mode:pass (expect_pass=%u mode=0x%x)" VSF_TRACE_CFG_LINEEND, (unsigned)p->expect_pass, (unsigned)p->mode);
}

#endif /* VSF_TEST_USART_RX_MODE_ENABLE == ENABLED */

/* EOF */


