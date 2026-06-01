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

#include "vsf_test_usart_break_signal.h"
#include "vsf_test_suites.h"


/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_USART_BREAK_SIGNAL_ENABLE == ENABLED
const struct vsf_test_usart_break_signal_s {
    vsf_test_suite_t hdr;
    vsf_test_usart_break_signal_params_t          params[VSF_TEST_USART_BREAK_SIGNAL_CASE_COUNT];
} vsf_test_usart_break_signal = {
    .hdr = {
        .name            = "usart_break_signal",
        .jmp_fn          = vsf_test_usart_break_signal_run,
        .case_count      = VSF_TEST_USART_BREAK_SIGNAL_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_usart_break_signal_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_USART,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_USART_BREAK_SIGNAL_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/



#if VSF_TEST_USART_BREAK_SIGNAL_ENABLE == ENABLED


/*============================ IMPLEMENTATION ================================*/

/* TX break signaling — exercise SET_BREAK/CLEAR_BREAK and SEND_BREAK
 * and let the LA verify the line stays low.
 *
 * Sequence (observed on uart1_tx):
 *   idle high → SET_BREAK → low for hold_ms → CLEAR_BREAK → idle high
 *
 * Then SEND_BREAK fires the auto-sequence (low for >= 1 frame, then
 * release). The LA decode finds the low pulses in the window.
 *
 * Note: the LA timing decoder reports edge timestamps, not high/low
 * levels. The decode-side asserts on the inter-edge gap (= line-low
 * duration when the previous edge was a fall). */
void vsf_test_usart_break_signal_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_usart_break_signal_params_t *p = tc->params;

    VSF_TEST_TRACE_INFO("usart_break_signal:start (case=%u baudrate=%lu hold_ms=%lu)"
                         VSF_TRACE_CFG_LINEEND,
                        (unsigned)tc->case_idx,
                        (unsigned long)p->baudrate, (unsigned long)p->hold_ms);

    VSF_TEST_TRACE_DEBUG("usart_break_signal:init" VSF_TRACE_CFG_LINEEND);
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_USART, usart);
    vsf_err_t err = vsf_usart_init(inst->fixture.usart, &(vsf_usart_cfg_t){
        .mode     = VSF_USART_NO_PARITY | VSF_USART_1_STOPBIT
                  | VSF_USART_8_BIT_LENGTH | VSF_USART_TX_ENABLE,
        .baudrate = p->baudrate,
    });
    VSF_TEST_ASSERT_ERR_NONE(err,
        "usart_break_signal:vsf_usart_init failed (err=%d)" VSF_TRACE_CFG_LINEEND, (int)err);
    VSF_TEST_TRACE_DEBUG("usart_break_signal:enable" VSF_TRACE_CFG_LINEEND);
    while (fsm_rt_cpl != vsf_usart_enable(inst->fixture.usart));

    /* Brief idle-high settle so the LA can latch the pre-break level. */
    vsf_test_busy_wait_ms(VSF_TEST_BREAK_SIGNAL_MIN_FRAME_MS);

    /* Manual: SET_BREAK → hold → CLEAR_BREAK. */
    VSF_TEST_TRACE_DEBUG("usart_break_signal:set_break (hold_ms=%lu)" VSF_TRACE_CFG_LINEEND,
                         (unsigned long)p->hold_ms);
    vsf_err_t ctrl_err = vsf_usart_ctrl(inst->fixture.usart, VSF_USART_CTRL_SET_BREAK, NULL);
    if (ctrl_err != VSF_ERR_NONE) {
        VSF_TEST_TRACE_ERROR("usart_break_signal:SET_BREAK failed (err=%d)" VSF_TRACE_CFG_LINEEND, (int)ctrl_err);
    }
    VSF_TEST_ASSERT(ctrl_err == VSF_ERR_NONE);
    vsf_test_busy_wait_ms(p->hold_ms);

    VSF_TEST_TRACE_DEBUG("usart_break_signal:clear_break" VSF_TRACE_CFG_LINEEND);
    ctrl_err = vsf_usart_ctrl(inst->fixture.usart, VSF_USART_CTRL_CLEAR_BREAK, NULL);
    if (ctrl_err != VSF_ERR_NONE) {
        VSF_TEST_TRACE_ERROR("usart_break_signal:CLEAR_BREAK failed (err=%d)" VSF_TRACE_CFG_LINEEND, (int)ctrl_err);
    }
    VSF_TEST_ASSERT(ctrl_err == VSF_ERR_NONE);

    /* Gap so the LA sees two distinct low pulses, not one merged pulse. */
    vsf_test_busy_wait_ms(VSF_TEST_BREAK_SIGNAL_DELIMITER_MS);

    /* Automated break: software-timed since the UART has no hardware auto-break.
     * SET_BREAK -> hold low for >= 1 frame (10 bits) -> CLEAR_BREAK. */
    VSF_TEST_TRACE_DEBUG("usart_break_signal:auto_break" VSF_TRACE_CFG_LINEEND);
    ctrl_err = vsf_usart_ctrl(inst->fixture.usart, VSF_USART_CTRL_SET_BREAK, NULL);
    if (ctrl_err != VSF_ERR_NONE) {
        VSF_TEST_TRACE_ERROR("usart_break_signal:SET_BREAK (auto) failed (err=%d)" VSF_TRACE_CFG_LINEEND, (int)ctrl_err);
    }
    VSF_TEST_ASSERT(ctrl_err == VSF_ERR_NONE);
    /* 1 frame at baudrate = 10 bits.  1000ms / baudrate * 10 = ms per frame. */
    uint32_t frame_ms = (1000 * 10) / p->baudrate;
    if (frame_ms < VSF_TEST_BREAK_SIGNAL_MIN_FRAME_MS) { frame_ms = VSF_TEST_BREAK_SIGNAL_MIN_FRAME_MS; }
    vsf_test_busy_wait_ms(frame_ms);
    ctrl_err = vsf_usart_ctrl(inst->fixture.usart, VSF_USART_CTRL_CLEAR_BREAK, NULL);
    if (ctrl_err != VSF_ERR_NONE) {
        VSF_TEST_TRACE_ERROR("usart_break_signal:CLEAR_BREAK (auto) failed (err=%d)" VSF_TRACE_CFG_LINEEND, (int)ctrl_err);
    }
    VSF_TEST_ASSERT(ctrl_err == VSF_ERR_NONE);

    /* Trailing settle so the second pulse's rising edge is fully captured. */
    vsf_test_busy_wait_ms(VSF_TEST_BREAK_SIGNAL_SETTLE_MS);

    VSF_TEST_TRACE_DEBUG("usart_break_signal:disable" VSF_TRACE_CFG_LINEEND);
    while (fsm_rt_cpl != vsf_usart_disable(inst->fixture.usart));
    VSF_TEST_TRACE_DEBUG("usart_break_signal:fini" VSF_TRACE_CFG_LINEEND);
    vsf_usart_fini(inst->fixture.usart);

    VSF_TEST_TRACE_INFO("usart_break_signal:pass" VSF_TRACE_CFG_LINEEND);
}

#endif /* VSF_TEST_USART_BREAK_SIGNAL_ENABLE == ENABLED */

/* EOF */


