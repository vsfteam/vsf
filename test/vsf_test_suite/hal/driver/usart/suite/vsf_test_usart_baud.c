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

#include "vsf_test_usart_baud.h"
#include "vsf_test_suites.h"

/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_USART_BAUD_ENABLE == ENABLED
const struct vsf_test_usart_baud_s {
    vsf_test_suite_t hdr;
    vsf_test_usart_baud_params_t          params[VSF_TEST_USART_BAUD_CASE_COUNT];
} vsf_test_usart_baud = {
    .hdr = {
        .name            = "usart_baud",
        .jmp_fn          = vsf_test_usart_baud_run,
        .case_count      = VSF_TEST_USART_BAUD_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_usart_baud_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_USART,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_USART_BAUD_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/

/*============================ LOCAL VARIABLES ===============================*/

/*============================ LOCAL FUNCTIONS ===============================*/

static void __usart_send_str(vsf_usart_t *usart, const char *str)
{
    while (*str) {
        while (!vsf_usart_txfifo_get_free_count(usart));
        vsf_usart_txfifo_write(usart, (uint8_t *)str, 1);
        str++;
    }
}

static void __usart_send_bulk(vsf_usart_t *usart, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++) {
        uint8_t b = (uint8_t)(i & 0xFF);
        while (!vsf_usart_txfifo_get_free_count(usart));
        vsf_usart_txfifo_write(usart, &b, 1);
    }
}

#if VSF_TEST_USART_TX_BAUD_ENABLE == ENABLED

/*============================ MACROS ========================================*/

/*============================ IMPLEMENTATION ================================*/

void vsf_test_usart_baud_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_usart_baud_params_t *p = tc->params;
    VSF_TEST_TRACE_INFO("usart_baud:start (case=%u)" VSF_TRACE_CFG_LINEEND, (unsigned)tc->case_idx);

    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware suites do not print them. */
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_USART, usart);
    vsf_usart_capability_t cap = vsf_usart_capability(inst->fixture.usart);
    bool expect_pass = (p->baudrate >= cap.min_baudrate)
                    && (p->baudrate <= cap.max_baudrate)
                    && (p->baudrate != 0);

    VSF_TEST_TRACE_DEBUG("usart_baud:vsf_usart_init (baudrate=%lu)" VSF_TRACE_CFG_LINEEND, (unsigned long)p->baudrate);
    vsf_err_t err = vsf_usart_init(inst->fixture.usart, &(vsf_usart_cfg_t){
        .mode     = VSF_TEST_BAUD_DEFAULT_MODE,
        .baudrate = p->baudrate,
    });

    if (expect_pass) {
        VSF_TEST_ASSERT_ERR_NONE(err,
            "usart_baud:vsf_usart_init failed (err=%d) (baudrate=%lu mode=0x%lx)" VSF_TRACE_CFG_LINEEND,
                                  (int)err, (unsigned long)p->baudrate, (unsigned long)VSF_TEST_BAUD_DEFAULT_MODE);

        VSF_TEST_TRACE_DEBUG("usart_baud:vsf_usart_enable" VSF_TRACE_CFG_LINEEND);
        while (fsm_rt_cpl != vsf_usart_enable(inst->fixture.usart));

        /* Phase-3 API completeness check (usart-gpio-coverage-gaps PRD):
         * get_configuration() must round-trip the values we passed to init().
         * Catches drivers that "accept" a baudrate but quietly clamp or drop
         * mode bits without telling the caller. */
        vsf_usart_cfg_t got = {0};
        VSF_TEST_TRACE_DEBUG("usart_baud:vsf_usart_get_configuration" VSF_TRACE_CFG_LINEEND);
        vsf_err_t cfg_err = vsf_usart_get_configuration(inst->fixture.usart, &got);
        if (cfg_err == VSF_ERR_NONE) {
            if (got.baudrate != p->baudrate) {
                VSF_TEST_TRACE_ERROR("usart_baud:baudrate mismatch (expected=%lu actual=%lu)" VSF_TRACE_CFG_LINEEND, (unsigned long)p->baudrate, (unsigned long)got.baudrate);
            }
            VSF_TEST_ASSERT(got.baudrate == p->baudrate);
            if (got.mode != VSF_TEST_BAUD_DEFAULT_MODE) {
                VSF_TEST_TRACE_ERROR("usart_baud:mode mismatch (expected=0x%x actual=0x%x)" VSF_TRACE_CFG_LINEEND, (unsigned)VSF_TEST_BAUD_DEFAULT_MODE, (unsigned)got.mode);
            }
            VSF_TEST_ASSERT(got.mode == VSF_TEST_BAUD_DEFAULT_MODE);
        }

        if (p->data_size_bytes > 0) {
            VSF_TEST_TRACE_DEBUG("usart_baud:__usart_send_bulk (%lu bytes)" VSF_TRACE_CFG_LINEEND, (unsigned long)p->data_size_bytes);
            __usart_send_bulk(inst->fixture.usart, p->data_size_bytes);
            /* Scale drain: 10 bits/byte @ baudrate, ms = bytes * 10 * 1000 / baudrate */
            uint32_t drain_ms = (p->data_size_bytes * 10 * 1000) / p->baudrate;
            if (drain_ms < VSF_TEST_BAUD_MIN_DRAIN_MS) { drain_ms = VSF_TEST_BAUD_MIN_DRAIN_MS; }
            vsf_test_busy_wait_ms(drain_ms);
        } else {
            VSF_TEST_TRACE_DEBUG("usart_baud:__usart_send_str" VSF_TRACE_CFG_LINEEND);
            __usart_send_str(inst->fixture.usart, VSF_TEST_BAUD_PAYLOAD);
            vsf_test_busy_wait_ms(VSF_TEST_BAUD_PAYLOAD_DRAIN_MS);
        }

        /* Phase-3 API completeness check: after TX drain, status() must
         * report tx-fifo-empty and not-busy. Catches drivers that never
         * update status() after a TX completes. */
        vsf_usart_status_t st = vsf_usart_status(inst->fixture.usart);
        if (!st.txfe) {
            VSF_TEST_TRACE_ERROR("usart_baud:status.txfe not set" VSF_TRACE_CFG_LINEEND);
        }
        VSF_TEST_ASSERT(st.txfe);
        if (st.is_busy) {
            VSF_TEST_TRACE_ERROR("usart_baud:status.is_busy still set" VSF_TRACE_CFG_LINEEND);
        }
        VSF_TEST_ASSERT(!st.is_busy);

        VSF_TEST_TRACE_DEBUG("usart_baud:vsf_usart_disable" VSF_TRACE_CFG_LINEEND);
        while (fsm_rt_cpl != vsf_usart_disable(inst->fixture.usart));
    } else {
        if (err == VSF_ERR_NONE) {
            VSF_TEST_TRACE_ERROR("usart_baud:vsf_usart_init unexpectedly succeeded (err=%d) (baudrate=%lu mode=0x%lx)" VSF_TRACE_CFG_LINEEND,
                                  (int)err, (unsigned long)p->baudrate, (unsigned long)VSF_TEST_BAUD_DEFAULT_MODE);
        }
        VSF_TEST_ASSERT(err != VSF_ERR_NONE);
    }
    VSF_TEST_TRACE_DEBUG("usart_baud:vsf_usart_fini" VSF_TRACE_CFG_LINEEND);
    vsf_usart_fini(inst->fixture.usart);

    /* Phase-3 API completeness check (usart-gpio-coverage-gaps PRD):
     * fini/disable lifecycle — after a full disable+fini, a second init+
     * enable+disable+fini cycle must succeed. Catches drivers that leak
     * state between init() calls or fail to reset the peripheral on fini. */
    if (expect_pass) {
        VSF_TEST_TRACE_DEBUG("usart_baud:vsf_usart_init (lifecycle check)" VSF_TRACE_CFG_LINEEND);
        err = vsf_usart_init(inst->fixture.usart, &(vsf_usart_cfg_t){
            .mode     = VSF_TEST_BAUD_DEFAULT_MODE,
            .baudrate = p->baudrate,
        });
        VSF_TEST_ASSERT_ERR_NONE(err,
            "usart_baud:vsf_usart_init (lifecycle) failed (err=%d) (baudrate=%lu mode=0x%lx)" VSF_TRACE_CFG_LINEEND,
                                  (int)err, (unsigned long)p->baudrate, (unsigned long)VSF_TEST_BAUD_DEFAULT_MODE);
        VSF_TEST_TRACE_DEBUG("usart_baud:vsf_usart_enable (lifecycle check)" VSF_TRACE_CFG_LINEEND);
        while (fsm_rt_cpl != vsf_usart_enable(inst->fixture.usart));
        VSF_TEST_TRACE_DEBUG("usart_baud:vsf_usart_disable (lifecycle check)" VSF_TRACE_CFG_LINEEND);
        while (fsm_rt_cpl != vsf_usart_disable(inst->fixture.usart));
        VSF_TEST_TRACE_DEBUG("usart_baud:vsf_usart_fini (lifecycle check)" VSF_TRACE_CFG_LINEEND);
        vsf_usart_fini(inst->fixture.usart);
    }

    VSF_TEST_TRACE_INFO("usart_baud:pass (baudrate=%lu expect_pass=%u)" VSF_TRACE_CFG_LINEEND, (unsigned long)p->baudrate, (unsigned)expect_pass);
}

#endif /* VSF_TEST_USART_TX_BAUD_ENABLE == ENABLED */

/* EOF */


