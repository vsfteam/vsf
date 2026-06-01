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

#include "vsf_test_usart_rx_irq.h"
#include "vsf_test_suites.h"

/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_USART_RX_IRQ_ENABLE == ENABLED
const struct vsf_test_usart_rx_irq_s {
    vsf_test_suite_t hdr;
    vsf_test_usart_rx_irq_params_t          params[VSF_TEST_USART_RX_IRQ_CASE_COUNT];
} vsf_test_usart_rx_irq = {
    .hdr = {
        .name            = "usart_rx_irq",
        .jmp_fn          = vsf_test_usart_rx_irq_run,
        .case_count      = VSF_TEST_USART_RX_IRQ_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_usart_rx_irq_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_USART,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_USART_RX_IRQ_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/

/*============================ LOCAL VARIABLES ===============================*/

typedef struct __rx_irq_ctx_t {
    uint8_t  buf[VSF_TEST_USART_RX_IRQ_BUF_SIZE];
    uint16_t count;
    uint16_t expected_len;
    bool     done;
} __rx_irq_ctx_t;

static void __rx_irq_handler(void *target_ptr, vsf_usart_t *usart_ptr, vsf_usart_irq_mask_t irq_mask)
{
    __rx_irq_ctx_t *ctx = (__rx_irq_ctx_t *)target_ptr;

    if (irq_mask & (VSF_USART_IRQ_MASK_RX | VSF_USART_IRQ_MASK_RX_TIMEOUT)) {
        while (vsf_usart_rxfifo_get_data_count(usart_ptr) > 0 && ctx->count < sizeof(ctx->buf)) {
            uint_fast16_t read = vsf_usart_rxfifo_read(usart_ptr, &ctx->buf[ctx->count], sizeof(ctx->buf) - ctx->count);
            if (read == 0) break;
            ctx->count += read;
        }
        if (ctx->count >= ctx->expected_len) {
            ctx->done = true;
        }
    }
}

#if VSF_TEST_USART_RX_IRQ_ENABLE == ENABLED

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_RX_IRQ_PRIO
// Must be higher (numerically lower) than PendSV priority — test framework runs
// scenarios inside PendSV, so a same-priority IRQ cannot preempt and the handler
// would never run.

#endif

/*============================ IMPLEMENTATION ================================*/

void vsf_test_usart_rx_irq_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_usart_rx_irq_params_t *p = tc->params;
    __rx_irq_ctx_t ctx = { .count = 0, .expected_len = strlen(VSF_TEST_RX_IRQ_PAYLOAD), .done = false };

    VSF_TEST_TRACE_INFO("usart_rx_irq:start (case=%u)" VSF_TRACE_CFG_LINEEND, (unsigned)tc->case_idx);

    VSF_TEST_TRACE_DEBUG("usart_rx_irq:vsf_usart_init (mode=0x%x baudrate=%lu)" VSF_TRACE_CFG_LINEEND,
                         (unsigned)VSF_TEST_RX_IRQ_DEFAULT_MODE,
                         (unsigned long)VSF_TEST_RX_IRQ_DEFAULT_BAUDRATE);
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_USART, usart);
    vsf_err_t err = vsf_usart_init(inst->fixture.usart, &(vsf_usart_cfg_t){
        .mode     = VSF_TEST_RX_IRQ_DEFAULT_MODE,
        .baudrate = VSF_TEST_RX_IRQ_DEFAULT_BAUDRATE,
        .isr      = {
            .handler_fn = __rx_irq_handler,
            .target_ptr = &ctx,
            .prio       = VSF_TEST_RX_IRQ_PRIO,
        },
    });

    if (p->expect_pass) {
        VSF_TEST_ASSERT_ERR_NONE(err,
            "usart_rx_irq:vsf_usart_init failed (err=%d) (mode=0x%x baudrate=%lu)" VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)VSF_TEST_RX_IRQ_DEFAULT_MODE, (unsigned long)VSF_TEST_RX_IRQ_DEFAULT_BAUDRATE);

        VSF_TEST_TRACE_DEBUG("usart_rx_irq:vsf_usart_enable" VSF_TRACE_CFG_LINEEND);
        while (fsm_rt_cpl != vsf_usart_enable(inst->fixture.usart));

        // Drain residual bytes left in the RX FIFO by prior scenarios (e.g.
        // rx_frame_error / rx_parity_error inject framing errors that can
        // leave garbage in the FIFO across suite boundaries). Without this,
        // the ISR fires immediately on enable and pollutes ctx.buf.
        {
            uint8_t junk[VSF_TEST_USART_RX_IRQ_JUNK_SIZE];
            while (vsf_usart_rxfifo_get_data_count(inst->fixture.usart) > 0) {
                if (vsf_usart_rxfifo_read(inst->fixture.usart, junk, sizeof(junk)) == 0) break;
            }
        }

        VSF_TEST_TRACE_DEBUG("usart_rx_irq:vsf_usart_irq_enable mask=0x%x" VSF_TRACE_CFG_LINEEND, (unsigned)(VSF_USART_IRQ_MASK_RX | VSF_USART_IRQ_MASK_RX_TIMEOUT));
        vsf_usart_irq_enable(inst->fixture.usart, VSF_USART_IRQ_MASK_RX | VSF_USART_IRQ_MASK_RX_TIMEOUT);

        uint32_t elapsed_ms = 0;
        const uint32_t max_ms = VSF_TEST_RX_IRQ_PAYLOAD_DRAIN_MS * 10;
        while (!ctx.done && elapsed_ms < max_ms) {
            vsf_test_busy_wait_ms(VSF_TEST_USART_RX_IRQ_BUSY_WAIT_MS);
            elapsed_ms += 10;
        }

        VSF_TEST_TRACE_DEBUG("usart_rx_irq:vsf_usart_irq_disable mask=0x%x" VSF_TRACE_CFG_LINEEND, (unsigned)(VSF_USART_IRQ_MASK_RX | VSF_USART_IRQ_MASK_RX_TIMEOUT));
        vsf_usart_irq_disable(inst->fixture.usart, VSF_USART_IRQ_MASK_RX | VSF_USART_IRQ_MASK_RX_TIMEOUT);

        if (!ctx.done) {
            VSF_TEST_TRACE_ERROR("usart_rx_irq:ctx.done not set (count=%u expected_len=%u)" VSF_TRACE_CFG_LINEEND, (unsigned)ctx.count, (unsigned)ctx.expected_len);
        }
        VSF_TEST_ASSERT(ctx.done);

        if (ctx.count != ctx.expected_len) {
            VSF_TEST_TRACE_ERROR("usart_rx_irq:count mismatch (expected=%u actual=%u)" VSF_TRACE_CFG_LINEEND, (unsigned)ctx.expected_len, (unsigned)ctx.count);
        }
        VSF_TEST_ASSERT(ctx.count == ctx.expected_len);

        bool payload_match = true;
        for (uint16_t i = 0; i < ctx.expected_len; i++) {
            if (ctx.buf[i] != (uint8_t)VSF_TEST_RX_IRQ_PAYLOAD[i]) {
                VSF_TEST_TRACE_ERROR("usart_rx_irq:payload mismatch at offset=%u (expected=0x%02x actual=0x%02x)"
                                     VSF_TRACE_CFG_LINEEND,
                                     (unsigned)i, (uint8_t)VSF_TEST_RX_IRQ_PAYLOAD[i], ctx.buf[i]);
                payload_match = false;
                break;
            }
        }
        VSF_TEST_ASSERT(payload_match);

        VSF_TEST_TRACE_DEBUG("usart_rx_irq:vsf_usart_disable" VSF_TRACE_CFG_LINEEND);
        while (fsm_rt_cpl != vsf_usart_disable(inst->fixture.usart));
    } else {
        if (err == VSF_ERR_NONE) {
            VSF_TEST_TRACE_ERROR("usart_rx_irq:vsf_usart_init unexpectedly succeeded (err=%d) (mode=0x%x baudrate=%lu)" VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)VSF_TEST_RX_IRQ_DEFAULT_MODE, (unsigned long)VSF_TEST_RX_IRQ_DEFAULT_BAUDRATE);
        }
        VSF_TEST_ASSERT(err != VSF_ERR_NONE);
    }
    VSF_TEST_TRACE_DEBUG("usart_rx_irq:vsf_usart_fini" VSF_TRACE_CFG_LINEEND);
    vsf_usart_fini(inst->fixture.usart);

    VSF_TEST_TRACE_INFO("usart_rx_irq:pass (expect_pass=%u)" VSF_TRACE_CFG_LINEEND, (unsigned)p->expect_pass);
}

#endif /* VSF_TEST_USART_RX_IRQ_ENABLE == ENABLED */

/* EOF */


