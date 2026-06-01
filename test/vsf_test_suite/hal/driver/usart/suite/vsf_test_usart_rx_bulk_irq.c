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

#include "vsf_test_usart_rx_bulk_irq.h"
#include "vsf_test_suites.h"

/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_USART_RX_BULK_IRQ_ENABLE == ENABLED
const struct vsf_test_usart_rx_bulk_irq_s {
    vsf_test_suite_t hdr;
    vsf_test_usart_rx_bulk_irq_params_t          params[VSF_TEST_USART_RX_BULK_IRQ_CASE_COUNT];
} vsf_test_usart_rx_bulk_irq = {
    .hdr = {
        .name            = "usart_rx_bulk_irq",
        .jmp_fn          = vsf_test_usart_rx_bulk_irq_run,
        .case_count      = VSF_TEST_USART_RX_BULK_IRQ_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_usart_rx_bulk_irq_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_USART,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_USART_RX_BULK_IRQ_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/

/*============================ LOCAL VARIABLES ===============================*/

#if VSF_TEST_USART_RX_BULK_IRQ_ENABLE == ENABLED

/*============================ MACROS ========================================*/

/*============================ LOCAL FUNCTIONS ===============================*/

static void __rx_bulk_irq_handler(void *target, vsf_usart_t *usart,
                                  vsf_usart_irq_mask_t irq_mask)
{
    if (!(irq_mask & VSF_USART_IRQ_MASK_RX)) { return; }

    vsf_test_suite_t *suite = target;

    while (vsf_test_suite_data.usart.usart_rx_bulk_irq.received < vsf_test_suite_data.usart.usart_rx_bulk_irq.target) {
        uint_fast16_t avail = vsf_usart_rxfifo_get_data_count(usart);
        if (avail == 0) { break; }

        uint_fast16_t want = vsf_test_suite_data.usart.usart_rx_bulk_irq.target - vsf_test_suite_data.usart.usart_rx_bulk_irq.received;
        if (want > avail) { want = avail; }
        uint_fast16_t got = vsf_usart_rxfifo_read(
            usart, vsf_test_suite_data.usart.usart_rx_bulk_irq.dst + vsf_test_suite_data.usart.usart_rx_bulk_irq.received, want);
        vsf_test_suite_data.usart.usart_rx_bulk_irq.received += got;
        vsf_test_suite_data.usart.usart_rx_bulk_irq.isr_count++;

        if (got < want) { break; }
    }

    if (vsf_test_suite_data.usart.usart_rx_bulk_irq.received >= vsf_test_suite_data.usart.usart_rx_bulk_irq.target) {
        vsf_usart_irq_disable(usart, VSF_USART_IRQ_MASK_RX);
        vsf_test_suite_data.usart.usart_rx_bulk_irq.done = true;
    }
}

/*============================ IMPLEMENTATION ================================*/

void vsf_test_usart_rx_bulk_irq_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_usart_rx_bulk_irq_params_t *p = tc->params;
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_USART, usart);
    vsf_usart_t *usart = inst->fixture.usart;

    VSF_TEST_TRACE_INFO("usart_rx_bulk_irq:start (case=%u)" VSF_TRACE_CFG_LINEEND, (unsigned)tc->case_idx);

    /* Per-case state must be re-initialised before each run. */
    vsf_test_suite_data.usart.usart_rx_bulk_irq.dst       = vsf_test_suite_data.usart.usart_rx_bulk_irq.rx_bulk_irq_buf;
    vsf_test_suite_data.usart.usart_rx_bulk_irq.target    = p->data_size_bytes;
    vsf_test_suite_data.usart.usart_rx_bulk_irq.received  = 0;
    vsf_test_suite_data.usart.usart_rx_bulk_irq.isr_count = 0;
    vsf_test_suite_data.usart.usart_rx_bulk_irq.done      = false;

    VSF_TEST_TRACE_DEBUG("usart_rx_bulk_irq:vsf_usart_init (mode=0x%x baudrate=%lu)" VSF_TRACE_CFG_LINEEND,
                         (unsigned)(VSF_USART_8_BIT_LENGTH | VSF_USART_1_STOPBIT
                                   | VSF_USART_NO_PARITY    | VSF_USART_RX_ENABLE
                                   | VSF_USART_RX_FIFO_THRESHOLD_HALF_FULL),
                         (unsigned long)VSF_TEST_RX_BULK_IRQ_DEFAULT_BAUDRATE);
    vsf_err_t err = vsf_usart_init(usart, &(vsf_usart_cfg_t){
        .mode     = VSF_USART_8_BIT_LENGTH | VSF_USART_1_STOPBIT
                  | VSF_USART_NO_PARITY    | VSF_USART_RX_ENABLE
                  | VSF_USART_RX_FIFO_THRESHOLD_HALF_FULL,
        .baudrate = VSF_TEST_RX_BULK_IRQ_DEFAULT_BAUDRATE,
        .isr      = {
            .handler_fn = __rx_bulk_irq_handler,
            .target_ptr = NULL,
            .prio       = VSF_TEST_USART_RX_BULK_IRQ_PRIO,
        },
    });
    VSF_TEST_ASSERT_ERR_NONE(err,
        "usart_rx_bulk_irq:vsf_usart_init failed (err=%d) (mode=0x%x baudrate=%lu)" VSF_TRACE_CFG_LINEEND,
                              (int)err,
                              (unsigned)(VSF_USART_8_BIT_LENGTH | VSF_USART_1_STOPBIT
                                        | VSF_USART_NO_PARITY    | VSF_USART_RX_ENABLE
                                        | VSF_USART_RX_FIFO_THRESHOLD_HALF_FULL),
                              (unsigned long)VSF_TEST_RX_BULK_IRQ_DEFAULT_BAUDRATE);

    VSF_TEST_TRACE_DEBUG("usart_rx_bulk_irq:vsf_usart_enable" VSF_TRACE_CFG_LINEEND);
    while (fsm_rt_cpl != vsf_usart_enable(usart));

    VSF_TEST_TRACE_DEBUG("usart_rx_bulk_irq:vsf_usart_irq_enable mask=0x%x" VSF_TRACE_CFG_LINEEND, (unsigned)VSF_USART_IRQ_MASK_RX);
    vsf_usart_irq_enable(usart, VSF_USART_IRQ_MASK_RX);

    /* Wait for ISR to receive everything.  Scale timeout with data size:
     * 10 bits/byte @ 115200 = ~87 µs/byte.  4 KB ≈ 350 ms.
     * Factor of 10 gives comfortable headroom for ISR latency. */
    uint32_t max_ms = (p->data_size_bytes * 10 * 10) / (VSF_TEST_RX_BULK_IRQ_DEFAULT_BAUDRATE / 1000);
    if (max_ms < VSF_TEST_USART_RX_BULK_IRQ_MIN_TIMEOUT_MS) { max_ms = VSF_TEST_USART_RX_BULK_IRQ_MIN_TIMEOUT_MS; }
    uint32_t elapsed_ms = 0;
    while (!vsf_test_suite_data.usart.usart_rx_bulk_irq.done && elapsed_ms < max_ms) {
        vsf_test_busy_wait_ms(VSF_TEST_USART_RX_BULK_IRQ_BUSY_WAIT_MS);
        elapsed_ms += 10;
    }

    if (!vsf_test_suite_data.usart.usart_rx_bulk_irq.done) {
        VSF_TEST_TRACE_ERROR("usart_rx_bulk_irq:done not set (received=%lu target=%lu)" VSF_TRACE_CFG_LINEEND,
                             (unsigned long)vsf_test_suite_data.usart.usart_rx_bulk_irq.received,
                             (unsigned long)vsf_test_suite_data.usart.usart_rx_bulk_irq.target);
    }
    VSF_TEST_ASSERT(vsf_test_suite_data.usart.usart_rx_bulk_irq.done);

    if (vsf_test_suite_data.usart.usart_rx_bulk_irq.received != p->data_size_bytes) {
        VSF_TEST_TRACE_ERROR("usart_rx_bulk_irq:received mismatch (expected=%lu actual=%lu)" VSF_TRACE_CFG_LINEEND,
                             (unsigned long)p->data_size_bytes,
                             (unsigned long)vsf_test_suite_data.usart.usart_rx_bulk_irq.received);
    }
    VSF_TEST_ASSERT(vsf_test_suite_data.usart.usart_rx_bulk_irq.received == p->data_size_bytes);
    if (vsf_test_suite_data.usart.usart_rx_bulk_irq.isr_count == 0) {
        VSF_TEST_TRACE_ERROR("usart_rx_bulk_irq:isr_count is 0" VSF_TRACE_CFG_LINEEND);
    }
    VSF_TEST_ASSERT(vsf_test_suite_data.usart.usart_rx_bulk_irq.isr_count > 0);

    /* Verify byte-level correctness: incrementing-counter pattern. */
    for (uint32_t i = 0; i < p->data_size_bytes; i++) {
        if (vsf_test_suite_data.usart.usart_rx_bulk_irq.rx_bulk_irq_buf[i] != (uint8_t)(i & 0xFF)) {
            VSF_TEST_TRACE_ERROR("usart_rx_bulk_irq:data mismatch at offset %lu (expected=0x%02x actual=0x%02x)" VSF_TRACE_CFG_LINEEND,
                                 (unsigned long)i, (unsigned)(uint8_t)(i & 0xFF),
                                 (unsigned)vsf_test_suite_data.usart.usart_rx_bulk_irq.rx_bulk_irq_buf[i]);
        }
        VSF_TEST_ASSERT(vsf_test_suite_data.usart.usart_rx_bulk_irq.rx_bulk_irq_buf[i] == (uint8_t)(i & 0xFF));
    }

    VSF_TEST_TRACE_DEBUG("usart_rx_bulk_irq:vsf_usart_disable" VSF_TRACE_CFG_LINEEND);
    while (fsm_rt_cpl != vsf_usart_disable(usart));
    VSF_TEST_TRACE_DEBUG("usart_rx_bulk_irq:vsf_usart_fini" VSF_TRACE_CFG_LINEEND);
    vsf_usart_fini(usart);

    VSF_TEST_TRACE_INFO("usart_rx_bulk_irq:pass (size=%lu isr=%lu)" VSF_TRACE_CFG_LINEEND,
                        (unsigned long)p->data_size_bytes,
                        (unsigned long)vsf_test_suite_data.usart.usart_rx_bulk_irq.isr_count);
}

#endif /* VSF_TEST_USART_RX_BULK_IRQ_ENABLE == ENABLED */

/* EOF */


