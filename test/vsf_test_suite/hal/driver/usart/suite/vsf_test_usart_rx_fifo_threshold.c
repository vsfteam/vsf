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

#include "vsf_test_usart_rx_fifo_threshold.h"
#include "vsf_test_suites.h"

/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_USART_RX_FIFO_THRESHOLD_ENABLE == ENABLED
const struct vsf_test_usart_rx_fifo_threshold_s {
    vsf_test_suite_t hdr;
    vsf_test_usart_rx_fifo_threshold_params_t          params[VSF_TEST_USART_RX_FIFO_THRESHOLD_CASE_COUNT];
} vsf_test_usart_rx_fifo_threshold = {
    .hdr = {
        .name            = "usart_rx_fifo_threshold",
        .jmp_fn          = vsf_test_usart_rx_fifo_threshold_run,
        .case_count      = VSF_TEST_USART_RX_FIFO_THRESHOLD_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_usart_rx_fifo_threshold_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_USART,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_USART_RX_FIFO_THRESHOLD_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/

/*============================ LOCAL VARIABLES ===============================*/

#if VSF_TEST_USART_RX_FIFO_THRESHOLD_ENABLE == ENABLED

/*============================ MACROS ========================================*/

/*============================ LOCAL FUNCTIONS ===============================*/

static void __rx_fifo_threshold_handler(void *target, vsf_usart_t *usart,
                                        vsf_usart_irq_mask_t irq_mask)
{
    if (!(irq_mask & VSF_USART_IRQ_MASK_RX)) { return; }

    vsf_test_suite_t *suite = target;

    /* Drain the FIFO completely — RX interrupt is level-triggered;
     * if we read only 1 byte the level may drop below threshold and the
     * remaining bytes stall because no new data is arriving. */
    while (vsf_usart_rxfifo_get_data_count(usart) > 0) {
        uint_fast16_t want = vsf_test_suite_data.usart.usart_rx_fifo_threshold.target - vsf_test_suite_data.usart.usart_rx_fifo_threshold.received;
        if (want == 0) break;
        uint_fast16_t got = vsf_usart_rxfifo_read(
            usart, vsf_test_suite_data.usart.usart_rx_fifo_threshold.dst + vsf_test_suite_data.usart.usart_rx_fifo_threshold.received, want);
        if (got == 0) break;
        vsf_test_suite_data.usart.usart_rx_fifo_threshold.received += got;
    }

    /* Record total bytes received at the first threshold fire.  Because we
     * drain the entire FIFO in one ISR visit, this equals the threshold
     * level (assuming the host sent exactly that many bytes). */
    if (!vsf_test_suite_data.usart.usart_rx_fifo_threshold.threshold_fired) {
        vsf_test_suite_data.usart.usart_rx_fifo_threshold.threshold_fired = true;
        vsf_test_suite_data.usart.usart_rx_fifo_threshold.bytes_at_threshold = vsf_test_suite_data.usart.usart_rx_fifo_threshold.received;
    }

    vsf_test_suite_data.usart.usart_rx_fifo_threshold.isr_count++;

    if (vsf_test_suite_data.usart.usart_rx_fifo_threshold.received >= vsf_test_suite_data.usart.usart_rx_fifo_threshold.target) {
        vsf_usart_irq_disable(usart, VSF_USART_IRQ_MASK_RX);
        vsf_test_suite_data.usart.usart_rx_fifo_threshold.done = true;
    }
}

/*============================ IMPLEMENTATION ================================*/

void vsf_test_usart_rx_fifo_threshold_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_usart_rx_fifo_threshold_params_t *p = tc->params;
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_USART, usart);
    vsf_usart_t *usart = inst->fixture.usart;

    VSF_TEST_TRACE_INFO("usart_rx_fifo_threshold:start (case=%u)" VSF_TRACE_CFG_LINEEND, (unsigned)tc->case_idx);

    /* Per-case state must be re-initialised before each run. */
    vsf_test_suite_data.usart.usart_rx_fifo_threshold.dst                = vsf_test_suite_data.usart.usart_rx_fifo_threshold.rx_fifo_threshold_buf;
    vsf_test_suite_data.usart.usart_rx_fifo_threshold.target             = p->expected_bytes;
    vsf_test_suite_data.usart.usart_rx_fifo_threshold.received           = 0;
    vsf_test_suite_data.usart.usart_rx_fifo_threshold.isr_count          = 0;
    vsf_test_suite_data.usart.usart_rx_fifo_threshold.done               = false;
    vsf_test_suite_data.usart.usart_rx_fifo_threshold.threshold_fired    = false;
    vsf_test_suite_data.usart.usart_rx_fifo_threshold.bytes_at_threshold = 0;

    VSF_TEST_TRACE_DEBUG("usart_rx_fifo_threshold:vsf_usart_init (mode=0x%x baudrate=%lu)" VSF_TRACE_CFG_LINEEND,
                         (unsigned)(VSF_USART_8_BIT_LENGTH | VSF_USART_1_STOPBIT
                                   | VSF_USART_NO_PARITY    | VSF_USART_RX_ENABLE
                                   | p->threshold_mode),
                         (unsigned long)VSF_TEST_RX_FIFO_THRESHOLD_DEFAULT_BAUDRATE);
    vsf_err_t err = vsf_usart_init(usart, &(vsf_usart_cfg_t){
        .mode     = VSF_USART_8_BIT_LENGTH | VSF_USART_1_STOPBIT
                  | VSF_USART_NO_PARITY    | VSF_USART_RX_ENABLE
                  | p->threshold_mode,
        .baudrate = VSF_TEST_RX_FIFO_THRESHOLD_DEFAULT_BAUDRATE,
        .isr      = {
            .handler_fn = __rx_fifo_threshold_handler,
            .target_ptr = NULL,
            .prio       = VSF_TEST_USART_RX_FIFO_THRESHOLD_PRIO,
        },
    });
    VSF_TEST_ASSERT_ERR_NONE(err,
        "usart_rx_fifo_threshold:vsf_usart_init failed (err=%d) (mode=0x%x baudrate=%lu threshold=0x%x)" VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)(VSF_USART_8_BIT_LENGTH | VSF_USART_1_STOPBIT | VSF_USART_NO_PARITY | VSF_USART_RX_ENABLE), (unsigned long)VSF_TEST_RX_FIFO_THRESHOLD_DEFAULT_BAUDRATE, (unsigned)p->threshold_mode);

    VSF_TEST_TRACE_DEBUG("usart_rx_fifo_threshold:vsf_usart_enable" VSF_TRACE_CFG_LINEEND);
    while (fsm_rt_cpl != vsf_usart_enable(usart));

    /* Drain any residual bytes from prior scenarios before enabling the
     * RX threshold interrupt; otherwise a stale byte can trigger a
     * spurious immediate fire with bytes_at_threshold == 0. */
    {
        uint8_t junk[VSF_TEST_USART_RX_FIFO_THRESHOLD_JUNK_SIZE];
        while (vsf_usart_rxfifo_get_data_count(usart) > 0) {
            if (vsf_usart_rxfifo_read(usart, junk, sizeof(junk)) == 0) break;
        }
    }

    VSF_TEST_TRACE_DEBUG("usart_rx_fifo_threshold:vsf_usart_irq_enable mask=0x%x" VSF_TRACE_CFG_LINEEND, (unsigned)VSF_USART_IRQ_MASK_RX);
    vsf_usart_irq_enable(usart, VSF_USART_IRQ_MASK_RX);

    /* Wait for ISR to receive everything.
     * 10 bits/byte @ 115200 = ~87 us/byte.  32 bytes ~ 3 ms.
     * 1 s timeout is generous headroom for host-side scheduling. */
    uint32_t elapsed_ms = 0;
    while (!vsf_test_suite_data.usart.usart_rx_fifo_threshold.done && elapsed_ms < VSF_TEST_USART_RX_FIFO_THRESHOLD_WAIT_MS) {
        vsf_test_busy_wait_ms(VSF_TEST_POLL_TICK_MS);
        elapsed_ms++;
    }

    if (!vsf_test_suite_data.usart.usart_rx_fifo_threshold.done) {
        VSF_TEST_TRACE_ERROR("usart_rx_fifo_threshold:done not set (received=%lu target=%lu)" VSF_TRACE_CFG_LINEEND,
                             (unsigned long)vsf_test_suite_data.usart.usart_rx_fifo_threshold.received,
                             (unsigned long)vsf_test_suite_data.usart.usart_rx_fifo_threshold.target);
    }
    VSF_TEST_ASSERT(vsf_test_suite_data.usart.usart_rx_fifo_threshold.done);

    if (vsf_test_suite_data.usart.usart_rx_fifo_threshold.received != p->expected_bytes) {
        VSF_TEST_TRACE_ERROR("usart_rx_fifo_threshold:received mismatch (expected=%lu actual=%lu)" VSF_TRACE_CFG_LINEEND,
                             (unsigned long)p->expected_bytes,
                             (unsigned long)vsf_test_suite_data.usart.usart_rx_fifo_threshold.received);
    }
    VSF_TEST_ASSERT(vsf_test_suite_data.usart.usart_rx_fifo_threshold.received == p->expected_bytes);

    if (vsf_test_suite_data.usart.usart_rx_fifo_threshold.isr_count == 0) {
        VSF_TEST_TRACE_ERROR("usart_rx_fifo_threshold:isr_count is 0 (value=%lu)" VSF_TRACE_CFG_LINEEND,
                              (unsigned long)vsf_test_suite_data.usart.usart_rx_fifo_threshold.isr_count);
    }
    VSF_TEST_ASSERT(vsf_test_suite_data.usart.usart_rx_fifo_threshold.isr_count > 0);

    /* Core assertion: threshold IRQ fired at exactly the expected byte count. */
    if (vsf_test_suite_data.usart.usart_rx_fifo_threshold.bytes_at_threshold != p->expected_bytes) {
        VSF_TEST_TRACE_ERROR("usart_rx_fifo_threshold:bytes_at_threshold mismatch (expected=%lu actual=%lu)" VSF_TRACE_CFG_LINEEND,
                             (unsigned long)p->expected_bytes,
                             (unsigned long)vsf_test_suite_data.usart.usart_rx_fifo_threshold.bytes_at_threshold);
    }
    VSF_TEST_ASSERT(vsf_test_suite_data.usart.usart_rx_fifo_threshold.bytes_at_threshold == p->expected_bytes);

    /* Verify byte-level correctness: incrementing-counter pattern. */
    for (uint32_t i = 0; i < p->expected_bytes; i++) {
        if (vsf_test_suite_data.usart.usart_rx_fifo_threshold.rx_fifo_threshold_buf[i] != (uint8_t)(i & 0xFF)) {
            VSF_TEST_TRACE_ERROR("usart_rx_fifo_threshold:data mismatch at offset %lu (expected=0x%02x actual=0x%02x)" VSF_TRACE_CFG_LINEEND,
                                 (unsigned long)i, (unsigned)(uint8_t)(i & 0xFF),
                                 (unsigned)vsf_test_suite_data.usart.usart_rx_fifo_threshold.rx_fifo_threshold_buf[i]);
        }
        VSF_TEST_ASSERT(vsf_test_suite_data.usart.usart_rx_fifo_threshold.rx_fifo_threshold_buf[i] == (uint8_t)(i & 0xFF));
    }

    VSF_TEST_TRACE_DEBUG("usart_rx_fifo_threshold:vsf_usart_disable" VSF_TRACE_CFG_LINEEND);
    while (fsm_rt_cpl != vsf_usart_disable(usart));
    VSF_TEST_TRACE_DEBUG("usart_rx_fifo_threshold:vsf_usart_fini" VSF_TRACE_CFG_LINEEND);
    vsf_usart_fini(usart);

    VSF_TEST_TRACE_INFO("usart_rx_fifo_threshold:pass (expected=%lu isr=%lu bytes_at_threshold=%lu)" VSF_TRACE_CFG_LINEEND,
                        (unsigned long)p->expected_bytes,
                        (unsigned long)vsf_test_suite_data.usart.usart_rx_fifo_threshold.isr_count,
                        (unsigned long)vsf_test_suite_data.usart.usart_rx_fifo_threshold.bytes_at_threshold);
}

#endif /* VSF_TEST_USART_RX_FIFO_THRESHOLD_ENABLE == ENABLED */

/* EOF */


