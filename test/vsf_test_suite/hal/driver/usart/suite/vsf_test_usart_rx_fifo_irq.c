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

#include "vsf_test_usart_rx_fifo_irq.h"
#include "vsf_test_suites.h"

/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_USART_RX_FIFO_IRQ_ENABLE == ENABLED
const struct vsf_test_usart_rx_fifo_irq_s {
    vsf_test_suite_t hdr;
    vsf_test_usart_rx_fifo_irq_params_t          params[VSF_TEST_USART_RX_FIFO_IRQ_CASE_COUNT];
} vsf_test_usart_rx_fifo_irq = {
    .hdr = {
        .name            = "usart_rx_fifo_irq",
        .jmp_fn          = vsf_test_usart_rx_fifo_irq_run,
        .case_count      = VSF_TEST_USART_RX_FIFO_IRQ_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_usart_rx_fifo_irq_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_USART,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_USART_RX_FIFO_IRQ_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/

/*============================ LOCAL VARIABLES ===============================*/

#if VSF_TEST_USART_RX_FIFO_IRQ_ENABLE == ENABLED

/*============================ MACROS ========================================*/

/*============================ LOCAL FUNCTIONS ===============================*/

static void __rx_fifo_isr(void *target, vsf_usart_t *usart, vsf_usart_irq_mask_t irq_mask)
{
    if (!(irq_mask & (VSF_USART_IRQ_MASK_RX | VSF_USART_IRQ_MASK_RX_TIMEOUT))) { return; }
    vsf_test_suite_t *suite = target;
    vsf_test_suite_data.usart.usart_rx_fifo_irq.isr_count++;
    while (vsf_test_suite_data.usart.usart_rx_fifo_irq.received < vsf_test_suite_data.usart.usart_rx_fifo_irq.target) {
        uint_fast16_t avail = vsf_usart_rxfifo_get_data_count(usart);
        if (avail == 0) { break; }
        uint_fast16_t want = vsf_test_suite_data.usart.usart_rx_fifo_irq.target - vsf_test_suite_data.usart.usart_rx_fifo_irq.received;
        if (want > avail) { want = avail; }
        uint_fast16_t got = vsf_usart_rxfifo_read(usart, vsf_test_suite_data.usart.usart_rx_fifo_irq.dst + vsf_test_suite_data.usart.usart_rx_fifo_irq.received, want);
        vsf_test_suite_data.usart.usart_rx_fifo_irq.received += got;
        if (got == 0) { break; }
    }
    if (vsf_test_suite_data.usart.usart_rx_fifo_irq.received >= vsf_test_suite_data.usart.usart_rx_fifo_irq.target) {
        vsf_usart_irq_disable(usart, VSF_USART_IRQ_MASK_RX | VSF_USART_IRQ_MASK_RX_TIMEOUT);
        vsf_test_suite_data.usart.usart_rx_fifo_irq.done = true;
    }
}

/*============================ IMPLEMENTATION ================================*/

void vsf_test_usart_rx_fifo_irq_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_usart_rx_fifo_irq_params_t *p = tc->params;
    VSF_TEST_TRACE_INFO("usart_rx_fifo_irq:start (case=%u)" VSF_TRACE_CFG_LINEEND, (unsigned)tc->case_idx);

    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware suites do not print them. */
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_USART, usart);
    vsf_usart_t *usart = inst->fixture.usart;

    vsf_usart_capability_t cap = vsf_usart_capability(usart);
    if (cap.rxfifo_depth == 0) {
        VSF_TEST_TRACE_ERROR("usart_rx_fifo_irq:rxfifo_depth is 0 (value=%u)" VSF_TRACE_CFG_LINEEND,
                              (unsigned)cap.rxfifo_depth);
    }
    VSF_TEST_ASSERT(cap.rxfifo_depth > 0);
    uint32_t total = (uint32_t)cap.rxfifo_depth * p->refill_target;
    static uint8_t buf[VSF_TEST_USART_RX_FIFO_IRQ_BUF_SIZE];
    if (total > sizeof(buf)) { total = sizeof(buf); }

    /* Per-case state in suite: must be re-initialised before each run. */
    vsf_test_suite_data.usart.usart_rx_fifo_irq.dst       = buf;
    vsf_test_suite_data.usart.usart_rx_fifo_irq.received  = 0;
    vsf_test_suite_data.usart.usart_rx_fifo_irq.target    = total;
    vsf_test_suite_data.usart.usart_rx_fifo_irq.isr_count = 0;
    vsf_test_suite_data.usart.usart_rx_fifo_irq.done      = false;

    /* Enable threshold IRQ at the requested level (no timeout) — distinguishes
     * from rx_irq and exercises NOT_EMPTY / HALF_FULL / FULL across cases. */
    VSF_TEST_TRACE_DEBUG("usart_rx_fifo_irq:vsf_usart_init (mode=0x%x baudrate=%lu threshold=0x%x)" VSF_TRACE_CFG_LINEEND,
                         (unsigned)(VSF_USART_8_BIT_LENGTH | VSF_USART_1_STOPBIT
                                   | VSF_USART_NO_PARITY    | VSF_USART_RX_ENABLE
                                   | VSF_USART_TX_ENABLE),
                         (unsigned long)VSF_TEST_RX_FIFO_IRQ_DEFAULT_BAUDRATE,
                         (unsigned)p->threshold_mode);
    vsf_err_t err = vsf_usart_init(usart, &(vsf_usart_cfg_t){
        .mode     = VSF_USART_8_BIT_LENGTH | VSF_USART_1_STOPBIT
                  | VSF_USART_NO_PARITY    | VSF_USART_RX_ENABLE
                  | VSF_USART_TX_ENABLE
                  | p->threshold_mode,
        .baudrate = VSF_TEST_RX_FIFO_IRQ_DEFAULT_BAUDRATE,
        .isr      = { .handler_fn = __rx_fifo_isr, .target_ptr = NULL,
                      .prio       = VSF_TEST_USART_RX_FIFO_IRQ_PRIO },
    });
    VSF_TEST_ASSERT_ERR_NONE(err,
        "usart_rx_fifo_irq:vsf_usart_init failed (err=%d) (mode=0x%x baudrate=%lu threshold=0x%x)" VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)(VSF_USART_8_BIT_LENGTH | VSF_USART_1_STOPBIT | VSF_USART_NO_PARITY | VSF_USART_RX_ENABLE | VSF_USART_TX_ENABLE), (unsigned long)VSF_TEST_RX_FIFO_IRQ_DEFAULT_BAUDRATE, (unsigned)p->threshold_mode);

    VSF_TEST_TRACE_DEBUG("usart_rx_fifo_irq:vsf_usart_enable" VSF_TRACE_CFG_LINEEND);
    while (fsm_rt_cpl != vsf_usart_enable(usart));

    /* Host sends data via aux_serial after READY marker. RX IRQ fires
     * as bytes arrive. */
    VSF_TEST_TRACE_DEBUG("usart_rx_fifo_irq:vsf_usart_irq_enable mask=0x%x" VSF_TRACE_CFG_LINEEND, (unsigned)(VSF_USART_IRQ_MASK_RX | VSF_USART_IRQ_MASK_RX_TIMEOUT));
    vsf_usart_irq_enable(usart, VSF_USART_IRQ_MASK_RX | VSF_USART_IRQ_MASK_RX_TIMEOUT);

    /* Wait for host data. Fixed iteration bound — immune to CI jitter. */
    for (uint32_t iter = 0; iter < VSF_TEST_USART_RX_FIFO_IRQ_POLL_MAX_ITER && !vsf_test_suite_data.usart.usart_rx_fifo_irq.done; iter++) {
        vsf_test_busy_wait_ms(VSF_TEST_POLL_TICK_MS);
    }

    if (!vsf_test_suite_data.usart.usart_rx_fifo_irq.done) {
        VSF_TEST_TRACE_ERROR("usart_rx_fifo_irq:done not set (received=%lu target=%lu)" VSF_TRACE_CFG_LINEEND,
                             (unsigned long)vsf_test_suite_data.usart.usart_rx_fifo_irq.received,
                             (unsigned long)vsf_test_suite_data.usart.usart_rx_fifo_irq.target);
    }
    VSF_TEST_ASSERT(vsf_test_suite_data.usart.usart_rx_fifo_irq.done);

    if (vsf_test_suite_data.usart.usart_rx_fifo_irq.isr_count == 0) {
        VSF_TEST_TRACE_ERROR("usart_rx_fifo_irq:isr_count==0" VSF_TRACE_CFG_LINEEND);
    }
    VSF_TEST_ASSERT(vsf_test_suite_data.usart.usart_rx_fifo_irq.isr_count > 0);

    VSF_TEST_TRACE_DEBUG("usart_rx_fifo_irq:vsf_usart_disable" VSF_TRACE_CFG_LINEEND);
    while (fsm_rt_cpl != vsf_usart_disable(usart));
    VSF_TEST_TRACE_DEBUG("usart_rx_fifo_irq:vsf_usart_fini" VSF_TRACE_CFG_LINEEND);
    vsf_usart_fini(usart);

    VSF_TEST_TRACE_INFO("usart_rx_fifo_irq:pass (refill_target=%u isr=%lu received=%lu)" VSF_TRACE_CFG_LINEEND,
                        (unsigned)p->refill_target,
                        (unsigned long)vsf_test_suite_data.usart.usart_rx_fifo_irq.isr_count,
                        (unsigned long)vsf_test_suite_data.usart.usart_rx_fifo_irq.received);
}

#endif /* VSF_TEST_USART_RX_FIFO_IRQ_ENABLE == ENABLED */

/* EOF */


