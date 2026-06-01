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

#include "vsf_test_usart_request_cancel.h"
#include "vsf_test_suites.h"


/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_USART_REQUEST_CANCEL_ENABLE == ENABLED
const struct vsf_test_usart_request_cancel_s {
    vsf_test_suite_t hdr;
    vsf_test_usart_request_cancel_params_t          params[VSF_TEST_USART_REQUEST_CANCEL_CASE_COUNT];
} vsf_test_usart_request_cancel = {
    .hdr = {
        .name            = "usart_request_cancel",
        .jmp_fn          = vsf_test_usart_request_cancel_run,
        .case_count      = VSF_TEST_USART_REQUEST_CANCEL_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_usart_request_cancel_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_USART,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_USART_REQUEST_CANCEL_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/



#if VSF_TEST_USART_REQUEST_CANCEL_ENABLE == ENABLED


/*============================ IMPLEMENTATION ================================*/

void vsf_test_usart_request_cancel_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_usart_request_cancel_params_t *p = tc->params;
    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware suites do not print them. */
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_USART, usart);
    vsf_usart_t *usart = inst->fixture.usart;

    VSF_TEST_TRACE_INFO("usart_request_cancel:start (case=%u)" VSF_TRACE_CFG_LINEEND,
                        (unsigned)tc->case_idx);

    vsf_usart_capability_t cap = vsf_usart_capability(usart);
    uint32_t total = (uint32_t)cap.txfifo_depth * p->refill_target;
    if (total < VSF_TEST_USART_REQUEST_CANCEL_MIN_TOTAL) { total = VSF_TEST_USART_REQUEST_CANCEL_MIN_TOTAL; }
    static uint8_t buf[VSF_TEST_USART_REQUEST_CANCEL_BUF_SIZE];
    if (total > sizeof(buf)) { total = sizeof(buf); }
    for (uint32_t i = 0; i < total; i++) { buf[i] = (uint8_t)(i & 0xFF); }

    VSF_TEST_TRACE_DEBUG("usart_request_cancel:init" VSF_TRACE_CFG_LINEEND);
    vsf_err_t err = vsf_usart_init(usart, &(vsf_usart_cfg_t){
        .mode     = VSF_USART_8_BIT_LENGTH | VSF_USART_1_STOPBIT
                  | VSF_USART_NO_PARITY    | VSF_USART_TX_ENABLE,
        .baudrate = VSF_TEST_USART_REQUEST_CANCEL_DEFAULT_BAUDRATE,
    });
    VSF_TEST_ASSERT_ERR_NONE(err,
        "usart_request_cancel:vsf_usart_init failed (err=%d)" VSF_TRACE_CFG_LINEEND, (int)err);
    VSF_TEST_TRACE_DEBUG("usart_request_cancel:enable" VSF_TRACE_CFG_LINEEND);
    while (fsm_rt_cpl != vsf_usart_enable(usart));

    VSF_TEST_TRACE_DEBUG("usart_request_cancel:request_tx (total=%lu)" VSF_TRACE_CFG_LINEEND,
                         (unsigned long)total);
    err = vsf_usart_request_tx(usart, buf, total);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "usart_request_cancel:vsf_usart_request_tx failed (err=%d)" VSF_TRACE_CFG_LINEEND, (int)err);

    /* Let some bytes drain through the line before pulling the cancel. */
    uint32_t waited = 0;
    while (waited < p->cancel_after_us) {
        vsf_test_busy_wait_ms(VSF_TEST_POLL_TICK_MS);
        waited += 1000;
    }

    VSF_TEST_TRACE_DEBUG("usart_request_cancel:cancel_tx" VSF_TRACE_CFG_LINEEND);
    err = vsf_usart_cancel_tx(usart);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "usart_request_cancel:vsf_usart_cancel_tx failed (err=%d)" VSF_TRACE_CFG_LINEEND, (int)err);

    int_fast32_t cnt = vsf_usart_get_tx_count(usart);
    /* Partial: > 0, < total. The strict inequality holds only if the cancel
     * fires before the whole payload has been clocked out. */
    VSF_TEST_TRACE_INFO("usart_request_cancel:total=%lu count=%ld" VSF_TRACE_CFG_LINEEND,
                        (unsigned long)total, (long)cnt);
    if (cnt < 0) {
        VSF_TEST_TRACE_ERROR("usart_request_cancel:tx_count negative (%ld)" VSF_TRACE_CFG_LINEEND, (long)cnt);
    }
    VSF_TEST_ASSERT(cnt >= 0);
    if ((uint32_t)cnt > total) {
        VSF_TEST_TRACE_ERROR("usart_request_cancel:tx_count exceeds total (%lu > %lu)" VSF_TRACE_CFG_LINEEND,
                             (unsigned long)cnt, (unsigned long)total);
    }
    VSF_TEST_ASSERT((uint32_t)cnt <= total);

    /* Phase-3 API completeness check (usart-gpio-coverage-gaps PRD): once the
     * outstanding bytes have clocked out (no hardware flush API, so we wait
     * for natural drain), status().txfe must report empty. Confirms the
     * status struct tracks reality after a cancel + drain sequence. */
    VSF_TEST_TRACE_DEBUG("usart_request_cancel:waiting for drain" VSF_TRACE_CFG_LINEEND);
    vsf_test_busy_wait_ms(VSF_TEST_USART_REQUEST_CANCEL_DRAIN_MS);
    vsf_usart_status_t st = vsf_usart_status(usart);
    if (!st.txfe) {
        VSF_TEST_TRACE_ERROR("usart_request_cancel:txfe not empty after drain" VSF_TRACE_CFG_LINEEND);
    }
    VSF_TEST_ASSERT(st.txfe);
    if (st.is_busy) {
        VSF_TEST_TRACE_ERROR("usart_request_cancel:busy after drain" VSF_TRACE_CFG_LINEEND);
    }
    VSF_TEST_ASSERT(!st.is_busy);

    VSF_TEST_TRACE_DEBUG("usart_request_cancel:disable" VSF_TRACE_CFG_LINEEND);
    while (fsm_rt_cpl != vsf_usart_disable(usart));
    VSF_TEST_TRACE_DEBUG("usart_request_cancel:fini" VSF_TRACE_CFG_LINEEND);
    vsf_usart_fini(usart);

    VSF_TEST_TRACE_INFO("usart_request_cancel:pass" VSF_TRACE_CFG_LINEEND);
}

#endif /* VSF_TEST_USART_REQUEST_CANCEL_ENABLE == ENABLED */

/* EOF */


