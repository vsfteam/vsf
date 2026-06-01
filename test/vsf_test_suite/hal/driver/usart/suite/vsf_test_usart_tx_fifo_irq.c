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

#include "vsf_test_usart_tx_fifo_irq.h"
#include "vsf_test_suites.h"


/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_USART_TX_FIFO_IRQ_ENABLE == ENABLED
const struct vsf_test_usart_tx_fifo_irq_s {
    vsf_test_suite_t hdr;
    vsf_test_usart_tx_fifo_irq_params_t          params[VSF_TEST_USART_TX_FIFO_IRQ_CASE_COUNT];
} vsf_test_usart_tx_fifo_irq = {
    .hdr = {
        .name            = "usart_tx_fifo_irq",
        .jmp_fn          = vsf_test_usart_tx_fifo_irq_run,
        .case_count      = VSF_TEST_USART_TX_FIFO_IRQ_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_usart_tx_fifo_irq_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_USART,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_USART_TX_FIFO_IRQ_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/



/*============================ LOCAL VARIABLES ===============================*/


#if VSF_TEST_USART_TX_FIFO_IRQ_ENABLE == ENABLED

static void __tx_fifo_isr(void *target, vsf_usart_t *usart, vsf_usart_irq_mask_t irq_mask)
{
    if (!(irq_mask & VSF_USART_IRQ_MASK_TX)) { return; }
    vsf_test_suite_t *suite = target;
    vsf_test_suite_data.usart.usart_tx_fifo_irq.isr_count++;
    /* Refill in one large request — txfifo_write reports the actual count
     * written when the FIFO fills, even when get_free_count
     * returns only 0/1. */
    while (vsf_test_suite_data.usart.usart_tx_fifo_irq.remaining > 0) {
        uint_fast16_t want = (vsf_test_suite_data.usart.usart_tx_fifo_irq.remaining > 64) ? 64 : (uint_fast16_t)vsf_test_suite_data.usart.usart_tx_fifo_irq.remaining;
        uint_fast16_t wrote = vsf_usart_txfifo_write(usart, (void *)vsf_test_suite_data.usart.usart_tx_fifo_irq.src, want);
        vsf_test_suite_data.usart.usart_tx_fifo_irq.src       += wrote;
        vsf_test_suite_data.usart.usart_tx_fifo_irq.remaining -= wrote;
        if (wrote < want) { break; }  /* FIFO full */
    }
    if (vsf_test_suite_data.usart.usart_tx_fifo_irq.remaining == 0) {
        vsf_usart_irq_disable(usart, VSF_USART_IRQ_MASK_TX);
        vsf_test_suite_data.usart.usart_tx_fifo_irq.done = true;
    }
}

/*============================ IMPLEMENTATION ================================*/

void vsf_test_usart_tx_fifo_irq_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_usart_tx_fifo_irq_params_t *p = tc->params;
    VSF_TEST_TRACE_INFO("usart_tx_fifo_irq:start (case=%u)" VSF_TRACE_CFG_LINEEND, (unsigned)tc->case_idx);

    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware suites do not print them. */
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_USART, usart);
    vsf_usart_t *usart = inst->fixture.usart;

    vsf_usart_capability_t cap = vsf_usart_capability(usart);
    if (cap.txfifo_depth == 0) {
        VSF_TEST_TRACE_ERROR("usart_tx_fifo_irq:txfifo_depth is 0 (value=%u)" VSF_TRACE_CFG_LINEEND,
                              (unsigned)cap.txfifo_depth);
    }
    VSF_TEST_ASSERT(cap.txfifo_depth > 0);
    uint32_t total = (uint32_t)cap.txfifo_depth * p->refill_target;
    static uint8_t buf[VSF_TEST_USART_TX_FIFO_IRQ_BUF_SIZE];
    if (total > sizeof(buf)) { total = sizeof(buf); }
    for (uint32_t i = 0; i < total; i++) { buf[i] = (uint8_t)('A' + (i % 26)); }

    /* Per-case state in suite: must be re-initialised before each run. */
    vsf_test_suite_data.usart.usart_tx_fifo_irq.src       = buf;
    vsf_test_suite_data.usart.usart_tx_fifo_irq.remaining = total;
    vsf_test_suite_data.usart.usart_tx_fifo_irq.isr_count = 0;
    vsf_test_suite_data.usart.usart_tx_fifo_irq.done      = false;

    VSF_TEST_TRACE_DEBUG("usart_tx_fifo_irq:vsf_usart_init (mode=0x%x baudrate=%lu)" VSF_TRACE_CFG_LINEEND,
                         (unsigned)(VSF_USART_8_BIT_LENGTH | VSF_USART_1_STOPBIT
                                   | VSF_USART_NO_PARITY    | VSF_USART_TX_ENABLE
                                   | VSF_USART_TX_FIFO_THRESHOLD_HALF_EMPTY),
                         (unsigned long)VSF_TEST_USART_TX_FIFO_IRQ_DEFAULT_BAUDRATE);
    vsf_err_t err = vsf_usart_init(usart, &(vsf_usart_cfg_t){
        .mode     = VSF_USART_8_BIT_LENGTH | VSF_USART_1_STOPBIT
                  | VSF_USART_NO_PARITY    | VSF_USART_TX_ENABLE
                  | VSF_USART_TX_FIFO_THRESHOLD_HALF_EMPTY,
        .baudrate = VSF_TEST_USART_TX_FIFO_IRQ_DEFAULT_BAUDRATE,
        .isr      = { .handler_fn = __tx_fifo_isr, .target_ptr = NULL,
                      .prio       = VSF_TEST_USART_TX_FIFO_IRQ_PRIO },
    });
    VSF_TEST_ASSERT_ERR_NONE(err,
        "usart_tx_fifo_irq:vsf_usart_init failed (err=%d) (mode=0x%x baudrate=%lu)" VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)(VSF_USART_8_BIT_LENGTH | VSF_USART_1_STOPBIT | VSF_USART_NO_PARITY | VSF_USART_TX_ENABLE | VSF_USART_TX_FIFO_THRESHOLD_HALF_EMPTY), (unsigned long)VSF_TEST_USART_TX_FIFO_IRQ_DEFAULT_BAUDRATE);

    VSF_TEST_TRACE_DEBUG("usart_tx_fifo_irq:vsf_usart_enable" VSF_TRACE_CFG_LINEEND);
    while (fsm_rt_cpl != vsf_usart_enable(usart));

    /* Pre-fill the FIFO until it's full (above HALF_EMPTY threshold).
     * txfifo_get_free_count may return only 0 or 1 on hardware where the
     * status register has empty/full flags, not a counter, so we can't ask
     * the driver how many slots are free. Instead request `txfifo_depth`
     * bytes and rely on txfifo_write returning the partial count when the
     * FIFO fills. */
    uint_fast16_t prefill_request = cap.txfifo_depth;
    if (prefill_request > vsf_test_suite_data.usart.usart_tx_fifo_irq.remaining) {
        prefill_request = (uint_fast16_t)vsf_test_suite_data.usart.usart_tx_fifo_irq.remaining;
    }
    VSF_TEST_TRACE_DEBUG("usart_tx_fifo_irq:vsf_usart_txfifo_write (prefill %u bytes)" VSF_TRACE_CFG_LINEEND, (unsigned)prefill_request);
    uint_fast16_t wrote = vsf_usart_txfifo_write(usart, (void *)vsf_test_suite_data.usart.usart_tx_fifo_irq.src, prefill_request);
    vsf_test_suite_data.usart.usart_tx_fifo_irq.src       += wrote;
    vsf_test_suite_data.usart.usart_tx_fifo_irq.remaining -= wrote;

    VSF_TEST_TRACE_DEBUG("usart_tx_fifo_irq:vsf_usart_irq_enable mask=0x%x" VSF_TRACE_CFG_LINEEND, (unsigned)VSF_USART_IRQ_MASK_TX);
    vsf_usart_irq_enable(usart, VSF_USART_IRQ_MASK_TX);

    /* Wait for ISR to drain everything. Fixed iteration bound — immune to
     * CI scheduler jitter and avoids fragile baud-rate arithmetic. */
    for (uint32_t iter = 0; iter < VSF_TEST_USART_TX_FIFO_IRQ_POLL_MAX_ITER && !vsf_test_suite_data.usart.usart_tx_fifo_irq.done; iter++) {
        vsf_test_busy_wait_ms(VSF_TEST_POLL_TICK_MS);
    }

    if (!vsf_test_suite_data.usart.usart_tx_fifo_irq.done) {
        VSF_TEST_TRACE_ERROR("usart_tx_fifo_irq:done not set (remaining=%lu)" VSF_TRACE_CFG_LINEEND,
                             (unsigned long)vsf_test_suite_data.usart.usart_tx_fifo_irq.remaining);
    }
    VSF_TEST_ASSERT(vsf_test_suite_data.usart.usart_tx_fifo_irq.done);

    if (vsf_test_suite_data.usart.usart_tx_fifo_irq.isr_count == 0) {
        VSF_TEST_TRACE_ERROR("usart_tx_fifo_irq:isr_count is 0 (value=%lu)" VSF_TRACE_CFG_LINEEND,
                              (unsigned long)vsf_test_suite_data.usart.usart_tx_fifo_irq.isr_count);
    }
    VSF_TEST_ASSERT(vsf_test_suite_data.usart.usart_tx_fifo_irq.isr_count > 0);

    VSF_TEST_TRACE_DEBUG("usart_tx_fifo_irq:vsf_usart_disable" VSF_TRACE_CFG_LINEEND);
    while (fsm_rt_cpl != vsf_usart_disable(usart));
    VSF_TEST_TRACE_DEBUG("usart_tx_fifo_irq:vsf_usart_fini" VSF_TRACE_CFG_LINEEND);
    vsf_usart_fini(usart);

    VSF_TEST_TRACE_INFO("usart_tx_fifo_irq:pass (refill_target=%u isr=%lu total=%lu)" VSF_TRACE_CFG_LINEEND,
                        (unsigned)p->refill_target,
                        (unsigned long)vsf_test_suite_data.usart.usart_tx_fifo_irq.isr_count,
                        (unsigned long)total);
}

#endif /* VSF_TEST_USART_TX_FIFO_IRQ_ENABLE == ENABLED */

/* EOF */


