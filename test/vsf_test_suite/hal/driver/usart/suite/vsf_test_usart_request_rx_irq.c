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

#include "vsf_test_usart_request_rx_irq.h"
#include "vsf_test_suites.h"


/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_USART_REQUEST_RX_IRQ_ENABLE == ENABLED
const struct vsf_test_usart_request_rx_irq_s {
    vsf_test_suite_t hdr;
    vsf_test_usart_request_rx_irq_params_t          params[VSF_TEST_USART_REQUEST_RX_IRQ_CASE_COUNT];
} vsf_test_usart_request_rx_irq = {
    .hdr = {
        .name            = "usart_request_rx_irq",
        .jmp_fn          = vsf_test_usart_request_rx_irq_run,
        .case_count      = VSF_TEST_USART_REQUEST_RX_IRQ_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_usart_request_rx_irq_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_USART,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_USART_REQUEST_RX_IRQ_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/



/*============================ LOCAL VARIABLES ===============================*/


#if VSF_TEST_USART_REQUEST_RX_IRQ_ENABLE == ENABLED

static void __req_rx_isr(void *target, vsf_usart_t *usart, vsf_usart_irq_mask_t irq_mask)

{
    vsf_test_suite_t *suite = target;
    vsf_test_suite_data.usart.usart_request_rx_irq.req_rx_irq_count++;
    if (irq_mask & VSF_USART_IRQ_MASK_RX_CPL) {
        vsf_test_suite_data.usart.usart_request_rx_irq.req_rx_cpl = true;
    }
}

/*============================ IMPLEMENTATION ================================*/

void vsf_test_usart_request_rx_irq_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_usart_request_rx_irq_params_t *p = tc->params;
    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware suites do not print them. */
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_USART, usart);
    vsf_usart_t *usart = inst->fixture.usart;

    VSF_TEST_TRACE_INFO("usart_request_rx_irq:start (case=%u)" VSF_TRACE_CFG_LINEEND,
                        (unsigned)tc->case_idx);

    vsf_usart_capability_t cap = vsf_usart_capability(usart);
    uint32_t total = (uint32_t)cap.rxfifo_depth * p->refill_target;
    if (total < 32) { total = 32; }
    if (total > sizeof(vsf_test_suite_data.usart.usart_request_rx_irq.req_rx_buf)) { total = sizeof(vsf_test_suite_data.usart.usart_request_rx_irq.req_rx_buf); }

    /* Per-case state in suite: must be re-initialised before each run. */
    vsf_test_suite_data.usart.usart_request_rx_irq.req_rx_cpl       = false;
    vsf_test_suite_data.usart.usart_request_rx_irq.req_rx_irq_count = 0;

    VSF_TEST_TRACE_DEBUG("usart_request_rx_irq:init" VSF_TRACE_CFG_LINEEND);
    vsf_err_t err = vsf_usart_init(usart, &(vsf_usart_cfg_t){
        .mode     = VSF_USART_8_BIT_LENGTH | VSF_USART_1_STOPBIT
                  | VSF_USART_NO_PARITY    | VSF_USART_RX_ENABLE
                  | VSF_USART_TX_ENABLE
                  | VSF_USART_RX_FIFO_THRESHOLD_HALF_FULL,
        .baudrate = VSF_TEST_USART_REQUEST_RX_IRQ_DEFAULT_BAUDRATE,
        .isr      = { .handler_fn = __req_rx_isr, .target_ptr = NULL,
                      .prio       = VSF_TEST_USART_REQUEST_RX_IRQ_PRIO },
    });
    VSF_TEST_ASSERT_ERR_NONE(err,
        "usart_request_rx_irq:vsf_usart_init failed (err=%d)" VSF_TRACE_CFG_LINEEND, (int)err);
    VSF_TEST_TRACE_DEBUG("usart_request_rx_irq:enable" VSF_TRACE_CFG_LINEEND);
    while (fsm_rt_cpl != vsf_usart_enable(usart));

    /* Host sends data via aux_serial after READY marker. RX_CPL IRQ fires
     * when the fifo2req adapter has drained all requested bytes. */
    VSF_TEST_TRACE_DEBUG("usart_request_rx_irq:irq_enable" VSF_TRACE_CFG_LINEEND);
    vsf_usart_irq_enable(usart, VSF_USART_IRQ_MASK_RX_CPL);

    VSF_TEST_TRACE_DEBUG("usart_request_rx_irq:request_rx (total=%lu)" VSF_TRACE_CFG_LINEEND,
                         (unsigned long)total);
    err = vsf_usart_request_rx(usart, vsf_test_suite_data.usart.usart_request_rx_irq.req_rx_buf, total);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "usart_request_rx_irq:vsf_usart_request_rx failed (err=%d)" VSF_TRACE_CFG_LINEEND, (int)err);

    /* Wait for host data. */
    uint32_t timeout_ms = (total * 10000 / 115200) + 2000;
    uint32_t waited = 0;
    VSF_TEST_TRACE_DEBUG("usart_request_rx_irq:waiting for host data" VSF_TRACE_CFG_LINEEND);
    while (!vsf_test_suite_data.usart.usart_request_rx_irq.req_rx_cpl && waited < timeout_ms) {
        vsf_test_busy_wait_ms(VSF_TEST_POLL_TICK_MS);
        waited++;
    }
    if (!vsf_test_suite_data.usart.usart_request_rx_irq.req_rx_cpl) {
        VSF_TEST_TRACE_ERROR("usart_request_rx_irq:timeout after %lu ms" VSF_TRACE_CFG_LINEEND,
                             (unsigned long)timeout_ms);
    }
    VSF_TEST_ASSERT(vsf_test_suite_data.usart.usart_request_rx_irq.req_rx_cpl);
    int_fast32_t cnt = vsf_usart_get_rx_count(usart);
    if (cnt != (int_fast32_t)total) {
        VSF_TEST_TRACE_ERROR("usart_request_rx_irq:rx_count mismatch (expected=%ld got=%ld)" VSF_TRACE_CFG_LINEEND,
                             (long)total, (long)cnt);
    }
    VSF_TEST_ASSERT(cnt == (int_fast32_t)total);
    VSF_TEST_TRACE_INFO("usart_request_rx_irq:irq=%lu count=%ld" VSF_TRACE_CFG_LINEEND,
                        (unsigned long)vsf_test_suite_data.usart.usart_request_rx_irq.req_rx_irq_count, (long)cnt);

    VSF_TEST_TRACE_DEBUG("usart_request_rx_irq:disable" VSF_TRACE_CFG_LINEEND);
    while (fsm_rt_cpl != vsf_usart_disable(usart));
    VSF_TEST_TRACE_DEBUG("usart_request_rx_irq:fini" VSF_TRACE_CFG_LINEEND);
    vsf_usart_fini(usart);

    VSF_TEST_TRACE_INFO("usart_request_rx_irq:pass" VSF_TRACE_CFG_LINEEND);
}

#endif /* VSF_TEST_USART_REQUEST_RX_IRQ_ENABLE == ENABLED */

/* EOF */

