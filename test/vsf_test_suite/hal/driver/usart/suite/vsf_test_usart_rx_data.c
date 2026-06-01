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

#include "vsf_test_usart_rx_data.h"
#include "vsf_test_suites.h"

/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_USART_RX_DATA_ENABLE == ENABLED
const struct vsf_test_usart_rx_data_s {
    vsf_test_suite_t hdr;
    vsf_test_usart_rx_data_params_t          params[VSF_TEST_USART_RX_DATA_CASE_COUNT];
} vsf_test_usart_rx_data = {
    .hdr = {
        .name            = "usart_rx_data",
        .jmp_fn          = vsf_test_usart_rx_data_run,
        .case_count      = VSF_TEST_USART_RX_DATA_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_usart_rx_data_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_USART,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_USART_RX_DATA_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/

/*============================ LOCAL VARIABLES ===============================*/

#if VSF_TEST_USART_RX_DATA_ENABLE == ENABLED

/*============================ MACROS ========================================*/

/*============================ IMPLEMENTATION ================================*/

void vsf_test_usart_rx_data_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_usart_rx_data_params_t *p = tc->params;

    VSF_TEST_TRACE_INFO("usart_rx_data:start (case=%u)" VSF_TRACE_CFG_LINEEND, (unsigned)tc->case_idx);

    VSF_TEST_TRACE_DEBUG("usart_rx_data:vsf_usart_init (mode=0x%x baudrate=%lu)" VSF_TRACE_CFG_LINEEND,
                         (unsigned)VSF_TEST_RX_DATA_DEFAULT_MODE,
                         (unsigned long)VSF_TEST_RX_DATA_DEFAULT_BAUDRATE);
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_USART, usart);
    vsf_err_t err = vsf_usart_init(inst->fixture.usart, &(vsf_usart_cfg_t){
        .mode     = VSF_TEST_RX_DATA_DEFAULT_MODE,
        .baudrate = VSF_TEST_RX_DATA_DEFAULT_BAUDRATE,
    });

    if (p->expect_pass) {
        VSF_TEST_ASSERT_ERR_NONE(err,
            "usart_rx_data:vsf_usart_init failed (err=%d) (mode=0x%x baudrate=%lu)" VSF_TRACE_CFG_LINEEND,
                                  (int)err, (unsigned)VSF_TEST_RX_DATA_DEFAULT_MODE, (unsigned long)VSF_TEST_RX_DATA_DEFAULT_BAUDRATE);
        VSF_TEST_TRACE_DEBUG("usart_rx_data:vsf_usart_enable" VSF_TRACE_CFG_LINEEND);
        while (fsm_rt_cpl != vsf_usart_enable(inst->fixture.usart));

        if (p->data_size_bytes > 0) {
            /* Bulk transfer: incrementing-counter pattern */
            uint32_t rx_len = 0;
            uint32_t expected_len = p->data_size_bytes;

            /* Scale timeout: 10 bits/byte @ baudrate, factor of 2 margin */
            uint32_t max_ms = (expected_len * 10 * 2) / (VSF_TEST_RX_DATA_DEFAULT_BAUDRATE / 1000);
            if (max_ms < VSF_TEST_USART_RX_DATA_MIN_TIMEOUT_MS) { max_ms = VSF_TEST_USART_RX_DATA_MIN_TIMEOUT_MS; }
            uint32_t elapsed_ms = 0;

            while (rx_len < expected_len && elapsed_ms < max_ms) {
                uint_fast16_t count = vsf_usart_rxfifo_get_data_count(inst->fixture.usart);
                if (count > 0) {
                    uint_fast16_t want = expected_len - rx_len;
                    if (want > count) { want = count; }
                    uint_fast16_t got = vsf_usart_rxfifo_read(
                        inst->fixture.usart, &vsf_test_suite_data.usart.usart_rx_data.rx_data_buf[rx_len], want);
                    rx_len += got;
                } else {
                    vsf_test_busy_wait_ms(VSF_TEST_POLL_TICK_MS);
                    elapsed_ms += 1;
                }
            }

            VSF_TEST_TRACE_DEBUG("usart_rx_data:vsf_usart_rxfifo_read (expected_len=%lu rx_len=%lu)" VSF_TRACE_CFG_LINEEND,
                                 (unsigned long)expected_len, (unsigned long)rx_len);
            if (rx_len != expected_len) {
                VSF_TEST_TRACE_ERROR("usart_rx_data:rx_len mismatch (expected=%lu actual=%lu)" VSF_TRACE_CFG_LINEEND,
                                     (unsigned long)expected_len, (unsigned long)rx_len);
            }
            VSF_TEST_ASSERT(rx_len == expected_len);

            for (uint32_t i = 0; i < expected_len; i++) {
                if (vsf_test_suite_data.usart.usart_rx_data.rx_data_buf[i] != (uint8_t)(i & 0xFF)) {
                    VSF_TEST_TRACE_ERROR("usart_rx_data:data mismatch at offset %lu (expected=0x%02x actual=0x%02x)" VSF_TRACE_CFG_LINEEND,
                                         (unsigned long)i, (unsigned)(uint8_t)(i & 0xFF),
                                         (unsigned)vsf_test_suite_data.usart.usart_rx_data.rx_data_buf[i]);
                }
                VSF_TEST_ASSERT(vsf_test_suite_data.usart.usart_rx_data.rx_data_buf[i] == (uint8_t)(i & 0xFF));
            }
        } else {
            /* Original string-based behavior (case idx 0) */
            uint8_t rx_buf[VSF_TEST_USART_RX_DATA_STACK_BUF_SIZE];
            uint16_t rx_len = 0;
            const char *expected = VSF_TEST_RX_DATA_PAYLOAD;
            uint16_t expected_len = strlen(expected);

            uint32_t elapsed_ms = 0;
            const uint32_t max_ms = VSF_TEST_RX_DATA_PAYLOAD_DRAIN_MS * 10;
            while (rx_len < expected_len && elapsed_ms < max_ms) {
                uint_fast16_t count = vsf_usart_rxfifo_get_data_count(inst->fixture.usart);
                while (count-- > 0 && rx_len < sizeof(rx_buf)) {
                    vsf_usart_rxfifo_read(inst->fixture.usart, &rx_buf[rx_len], 1);
                    rx_len++;
                }
                vsf_test_busy_wait_ms(VSF_TEST_USART_RX_DATA_BUSY_WAIT_MS);
                elapsed_ms += 10;
            }

            VSF_TEST_TRACE_DEBUG("usart_rx_data:vsf_usart_rxfifo_read (expected_len=%u rx_len=%u)" VSF_TRACE_CFG_LINEEND,
                                 (unsigned)expected_len, (unsigned)rx_len);
            if (rx_len != expected_len) {
                VSF_TEST_TRACE_ERROR("usart_rx_data:rx_len mismatch (expected=%u actual=%u)" VSF_TRACE_CFG_LINEEND,
                                     (unsigned)expected_len, (unsigned)rx_len);
            }
            VSF_TEST_ASSERT(rx_len == expected_len);
            if (memcmp(rx_buf, expected, expected_len) != 0) {
                VSF_TEST_TRACE_ERROR("usart_rx_data:payload mismatch" VSF_TRACE_CFG_LINEEND);
            }
            VSF_TEST_ASSERT(memcmp(rx_buf, expected, expected_len) == 0);
        }

        VSF_TEST_TRACE_DEBUG("usart_rx_data:vsf_usart_disable" VSF_TRACE_CFG_LINEEND);
        while (fsm_rt_cpl != vsf_usart_disable(inst->fixture.usart));
    } else {
        if (err == VSF_ERR_NONE) {
            VSF_TEST_TRACE_ERROR("usart_rx_data:vsf_usart_init unexpectedly succeeded (err=%d) (mode=0x%x baudrate=%lu)" VSF_TRACE_CFG_LINEEND,
                                  (int)err, (unsigned)VSF_TEST_RX_DATA_DEFAULT_MODE, (unsigned long)VSF_TEST_RX_DATA_DEFAULT_BAUDRATE);
        }
        VSF_TEST_ASSERT(err != VSF_ERR_NONE);
    }
    VSF_TEST_TRACE_DEBUG("usart_rx_data:vsf_usart_fini" VSF_TRACE_CFG_LINEEND);
    vsf_usart_fini(inst->fixture.usart);

    VSF_TEST_TRACE_INFO("usart_rx_data:pass (expect_pass=%u data_size_bytes=%lu)" VSF_TRACE_CFG_LINEEND,
                        (unsigned)p->expect_pass, (unsigned long)p->data_size_bytes);
}

#endif /* VSF_TEST_USART_RX_DATA_ENABLE == ENABLED */

/* EOF */


