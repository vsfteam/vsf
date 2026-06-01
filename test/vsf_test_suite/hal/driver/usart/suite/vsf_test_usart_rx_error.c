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

#include "vsf_test_usart_rx_error.h"
#include "vsf_test_suites.h"

#if VSF_TEST_USART_RX_PARITY_ERROR_ENABLE == ENABLED || VSF_TEST_USART_RX_FRAME_ERROR_ENABLE == ENABLED || VSF_TEST_USART_RX_BREAK_ERROR_ENABLE == ENABLED || VSF_TEST_USART_RX_OVERFLOW_ERROR_ENABLE == ENABLED

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_RX_ERROR_PRIO
// Must preempt PendSV — see note in vsf_test_usart_rx_irq.c.

#endif

/*============================ TYPES =========================================*/

typedef struct __rx_error_ctx_t {
    bool     parity_err;
    bool     frame_err;
    bool     break_err;
    bool     overflow_err;
} __rx_error_ctx_t;

/*============================ LOCAL FUNCTIONS ===============================*/

static void __rx_error_handler(void *target_ptr, vsf_usart_t *usart_ptr,
                            vsf_usart_irq_mask_t irq_mask)
{
    __rx_error_ctx_t *ctx = (__rx_error_ctx_t *)target_ptr;

    if (irq_mask & VSF_USART_IRQ_MASK_PARITY_ERR) {
        ctx->parity_err = true;
    }
    if (irq_mask & VSF_USART_IRQ_MASK_FRAME_ERR) {
        ctx->frame_err = true;
    }
    if (irq_mask & VSF_USART_IRQ_MASK_BREAK_ERR) {
        ctx->break_err = true;
    }
    if (irq_mask & VSF_USART_IRQ_MASK_RX_OVERFLOW_ERR) {
        ctx->overflow_err = true;
    }
}

/*============================ IMPLEMENTATION ================================*/

#if VSF_TEST_USART_RX_PARITY_ERROR_ENABLE == ENABLED
void vsf_test_usart_rx_parity_error_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_usart_rx_parity_error_params_t *p = tc->params;
    __rx_error_ctx_t ctx = { .parity_err = false, .frame_err = false };

    VSF_TEST_TRACE_INFO("usart_rx_parity_error:start (case=%u)" VSF_TRACE_CFG_LINEEND, (unsigned)tc->case_idx);

    VSF_TEST_TRACE_DEBUG("usart_rx_parity_error:vsf_usart_init (mode=0x%x baudrate=%lu)" VSF_TRACE_CFG_LINEEND,
                         (unsigned)p->mode, (unsigned long)VSF_TEST_RX_ERROR_DEFAULT_BAUDRATE);
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_USART, usart);
    vsf_err_t err = vsf_usart_init(inst->fixture.usart, &(vsf_usart_cfg_t){
        .mode     = p->mode,
        .baudrate = VSF_TEST_RX_ERROR_DEFAULT_BAUDRATE,
        .isr      = {
            .handler_fn = __rx_error_handler,
            .target_ptr = &ctx,
            .prio       = VSF_TEST_RX_ERROR_PRIO,
        },
    });

    if (p->expect_pass) {
        VSF_TEST_ASSERT_ERR_NONE(err,
            "usart_rx_parity_error:vsf_usart_init failed (err=%d) (mode=0x%x baudrate=%lu)" VSF_TRACE_CFG_LINEEND,
                                  (int)err, (unsigned)p->mode, (unsigned long)VSF_TEST_RX_ERROR_DEFAULT_BAUDRATE);
        VSF_TEST_TRACE_DEBUG("usart_rx_parity_error:vsf_usart_enable" VSF_TRACE_CFG_LINEEND);
        while (fsm_rt_cpl != vsf_usart_enable(inst->fixture.usart));

        VSF_TEST_TRACE_DEBUG("usart_rx_parity_error:vsf_usart_irq_enable mask=0x%x" VSF_TRACE_CFG_LINEEND, (unsigned)VSF_USART_IRQ_MASK_PARITY_ERR);
        vsf_usart_irq_enable(inst->fixture.usart, VSF_USART_IRQ_MASK_PARITY_ERR);

        uint32_t elapsed_ms = 0;
        const uint32_t max_ms = VSF_TEST_RX_ERROR_PAYLOAD_DRAIN_MS * 10;
        while (!ctx.parity_err && elapsed_ms < max_ms) {
            vsf_test_busy_wait_ms(VSF_TEST_USART_RX_ERROR_BUSY_WAIT_MS);
            elapsed_ms += 10;
        }

        VSF_TEST_TRACE_DEBUG("usart_rx_parity_error:vsf_usart_irq_disable mask=0x%x" VSF_TRACE_CFG_LINEEND, (unsigned)VSF_USART_IRQ_MASK_PARITY_ERR);
        vsf_usart_irq_disable(inst->fixture.usart, VSF_USART_IRQ_MASK_PARITY_ERR);

        if (!ctx.parity_err) {
            VSF_TEST_TRACE_ERROR("usart_rx_parity_error:parity_err not set (parity_err=%u)" VSF_TRACE_CFG_LINEEND, (unsigned)ctx.parity_err);
        }
        VSF_TEST_ASSERT(ctx.parity_err);

        VSF_TEST_TRACE_DEBUG("usart_rx_parity_error:vsf_usart_disable" VSF_TRACE_CFG_LINEEND);
        while (fsm_rt_cpl != vsf_usart_disable(inst->fixture.usart));
    } else {
        if (err == VSF_ERR_NONE) {
            VSF_TEST_TRACE_ERROR("usart_rx_parity_error:vsf_usart_init unexpectedly succeeded (err=%d) (mode=0x%x baudrate=%lu)" VSF_TRACE_CFG_LINEEND,
                                  (int)err, (unsigned)p->mode, (unsigned long)VSF_TEST_RX_ERROR_DEFAULT_BAUDRATE);
        }
        VSF_TEST_ASSERT(err != VSF_ERR_NONE);
    }
    VSF_TEST_TRACE_DEBUG("usart_rx_parity_error:vsf_usart_fini" VSF_TRACE_CFG_LINEEND);
    vsf_usart_fini(inst->fixture.usart);

    VSF_TEST_TRACE_INFO("usart_rx_parity_error:pass (expect_pass=%u)" VSF_TRACE_CFG_LINEEND, (unsigned)p->expect_pass);
}
#endif /* VSF_TEST_USART_RX_PARITY_ERROR_ENABLE == ENABLED */

#if VSF_TEST_USART_RX_FRAME_ERROR_ENABLE == ENABLED
void vsf_test_usart_rx_frame_error_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_usart_rx_frame_error_params_t *p = tc->params;
    __rx_error_ctx_t ctx = { .parity_err = false, .frame_err = false };

    VSF_TEST_TRACE_INFO("usart_rx_frame_error:start (case=%u)" VSF_TRACE_CFG_LINEEND, (unsigned)tc->case_idx);

    VSF_TEST_TRACE_DEBUG("usart_rx_frame_error:vsf_usart_init (mode=0x%x baudrate=%lu)" VSF_TRACE_CFG_LINEEND,
                         (unsigned)p->mode, (unsigned long)VSF_TEST_RX_ERROR_DEFAULT_BAUDRATE);
    vsf_err_t err = vsf_usart_init(inst->fixture.usart, &(vsf_usart_cfg_t){
        .mode     = p->mode,
        .baudrate = VSF_TEST_RX_ERROR_DEFAULT_BAUDRATE,
        .isr      = {
            .handler_fn = __rx_error_handler,
            .target_ptr = &ctx,
            .prio       = VSF_TEST_RX_ERROR_PRIO,
        },
    });

    if (p->expect_pass) {
        VSF_TEST_ASSERT_ERR_NONE(err,
            "usart_rx_frame_error:vsf_usart_init failed (err=%d) (mode=0x%x baudrate=%lu)" VSF_TRACE_CFG_LINEEND,
                                  (int)err, (unsigned)p->mode, (unsigned long)VSF_TEST_RX_ERROR_DEFAULT_BAUDRATE);
        VSF_TEST_TRACE_DEBUG("usart_rx_frame_error:vsf_usart_enable" VSF_TRACE_CFG_LINEEND);
        while (fsm_rt_cpl != vsf_usart_enable(inst->fixture.usart));

        VSF_TEST_TRACE_DEBUG("usart_rx_frame_error:vsf_usart_irq_enable mask=0x%x" VSF_TRACE_CFG_LINEEND, (unsigned)VSF_USART_IRQ_MASK_FRAME_ERR);
        vsf_usart_irq_enable(inst->fixture.usart, VSF_USART_IRQ_MASK_FRAME_ERR);

        uint32_t elapsed_ms = 0;
        const uint32_t max_ms = VSF_TEST_RX_ERROR_PAYLOAD_DRAIN_MS * 10;
        while (!ctx.frame_err && elapsed_ms < max_ms) {
            vsf_test_busy_wait_ms(VSF_TEST_USART_RX_ERROR_BUSY_WAIT_MS);
            elapsed_ms += 10;
        }

        VSF_TEST_TRACE_DEBUG("usart_rx_frame_error:vsf_usart_irq_disable mask=0x%x" VSF_TRACE_CFG_LINEEND, (unsigned)VSF_USART_IRQ_MASK_FRAME_ERR);
        vsf_usart_irq_disable(inst->fixture.usart, VSF_USART_IRQ_MASK_FRAME_ERR);

        if (!ctx.frame_err) {
            VSF_TEST_TRACE_ERROR("usart_rx_frame_error:frame_err not set (frame_err=%u)" VSF_TRACE_CFG_LINEEND, (unsigned)ctx.frame_err);
        }
        VSF_TEST_ASSERT(ctx.frame_err);

        VSF_TEST_TRACE_DEBUG("usart_rx_frame_error:vsf_usart_disable" VSF_TRACE_CFG_LINEEND);
        while (fsm_rt_cpl != vsf_usart_disable(inst->fixture.usart));
    } else {
        if (err == VSF_ERR_NONE) {
            VSF_TEST_TRACE_ERROR("usart_rx_frame_error:vsf_usart_init unexpectedly succeeded (err=%d) (mode=0x%x baudrate=%lu)" VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)p->mode, (unsigned long)VSF_TEST_RX_ERROR_DEFAULT_BAUDRATE);
        }
        VSF_TEST_ASSERT(err != VSF_ERR_NONE);
    }
    VSF_TEST_TRACE_DEBUG("usart_rx_frame_error:vsf_usart_fini" VSF_TRACE_CFG_LINEEND);
    vsf_usart_fini(inst->fixture.usart);

    VSF_TEST_TRACE_INFO("usart_rx_frame_error:pass (expect_pass=%u)" VSF_TRACE_CFG_LINEEND, (unsigned)p->expect_pass);
}
#endif /* VSF_TEST_USART_RX_FRAME_ERROR_ENABLE == ENABLED */

#if VSF_TEST_USART_RX_BREAK_ERROR_ENABLE == ENABLED
void vsf_test_usart_rx_break_error_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_usart_rx_break_error_params_t *p = tc->params;
    __rx_error_ctx_t ctx = { .parity_err = false, .frame_err = false, .break_err = false };

    VSF_TEST_TRACE_INFO("usart_rx_break_error:start (case=%u)" VSF_TRACE_CFG_LINEEND, (unsigned)tc->case_idx);

    VSF_TEST_TRACE_DEBUG("usart_rx_break_error:vsf_usart_init (mode=0x%x baudrate=%lu)" VSF_TRACE_CFG_LINEEND,
                         (unsigned)p->mode, (unsigned long)VSF_TEST_RX_ERROR_DEFAULT_BAUDRATE);
    vsf_err_t err = vsf_usart_init(inst->fixture.usart, &(vsf_usart_cfg_t){
        .mode     = p->mode,
        .baudrate = VSF_TEST_RX_ERROR_DEFAULT_BAUDRATE,
        .isr      = {
            .handler_fn = __rx_error_handler,
            .target_ptr = &ctx,
            .prio       = VSF_TEST_RX_ERROR_PRIO,
        },
    });

    if (p->expect_pass) {
        VSF_TEST_ASSERT_ERR_NONE(err,
            "usart_rx_break_error:vsf_usart_init failed (err=%d) (mode=0x%x baudrate=%lu)" VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)p->mode, (unsigned long)VSF_TEST_RX_ERROR_DEFAULT_BAUDRATE);
        VSF_TEST_TRACE_DEBUG("usart_rx_break_error:vsf_usart_enable" VSF_TRACE_CFG_LINEEND);
        while (fsm_rt_cpl != vsf_usart_enable(inst->fixture.usart));

        VSF_TEST_TRACE_DEBUG("usart_rx_break_error:vsf_usart_irq_enable mask=0x%x" VSF_TRACE_CFG_LINEEND, (unsigned)VSF_USART_IRQ_MASK_BREAK_ERR);
        vsf_usart_irq_enable(inst->fixture.usart, VSF_USART_IRQ_MASK_BREAK_ERR);

        uint32_t elapsed_ms = 0;
        const uint32_t max_ms = VSF_TEST_RX_ERROR_PAYLOAD_DRAIN_MS * 10;
        while (!ctx.break_err && elapsed_ms < max_ms) {
            vsf_test_busy_wait_ms(VSF_TEST_USART_RX_ERROR_BUSY_WAIT_MS);
            elapsed_ms += 10;
        }

        VSF_TEST_TRACE_DEBUG("usart_rx_break_error:vsf_usart_irq_disable mask=0x%x" VSF_TRACE_CFG_LINEEND, (unsigned)VSF_USART_IRQ_MASK_BREAK_ERR);
        vsf_usart_irq_disable(inst->fixture.usart, VSF_USART_IRQ_MASK_BREAK_ERR);

        if (!ctx.break_err) {
            VSF_TEST_TRACE_ERROR("usart_rx_break_error:break_err not set (break_err=%u)" VSF_TRACE_CFG_LINEEND, (unsigned)ctx.break_err);
        }
        VSF_TEST_ASSERT(ctx.break_err);

        VSF_TEST_TRACE_DEBUG("usart_rx_break_error:vsf_usart_disable" VSF_TRACE_CFG_LINEEND);
        while (fsm_rt_cpl != vsf_usart_disable(inst->fixture.usart));
    } else {
        if (err == VSF_ERR_NONE) {
            VSF_TEST_TRACE_ERROR("usart_rx_break_error:vsf_usart_init unexpectedly succeeded (err=%d) (mode=0x%x baudrate=%lu)" VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)p->mode, (unsigned long)VSF_TEST_RX_ERROR_DEFAULT_BAUDRATE);
        }
        VSF_TEST_ASSERT(err != VSF_ERR_NONE);
    }
    VSF_TEST_TRACE_DEBUG("usart_rx_break_error:vsf_usart_fini" VSF_TRACE_CFG_LINEEND);
    vsf_usart_fini(inst->fixture.usart);

    VSF_TEST_TRACE_INFO("usart_rx_break_error:pass (expect_pass=%u)" VSF_TRACE_CFG_LINEEND, (unsigned)p->expect_pass);
}
#endif /* VSF_TEST_USART_RX_BREAK_ERROR_ENABLE == ENABLED */

#if VSF_TEST_USART_RX_OVERFLOW_ERROR_ENABLE == ENABLED
void vsf_test_usart_rx_overflow_error_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_usart_rx_overflow_error_params_t *p = tc->params;
    /* Only the OVERFLOW IRQ is enabled (no RX/RX_TIMEOUT), so the FIFO is
     * never drained — host's burst quickly exceeds the hardware RX FIFO
     * and OVERRUN fires. */
    __rx_error_ctx_t ctx = { 0 };

    VSF_TEST_TRACE_INFO("usart_rx_overflow_error:start (case=%u)" VSF_TRACE_CFG_LINEEND, (unsigned)tc->case_idx);

    VSF_TEST_TRACE_DEBUG("usart_rx_overflow_error:vsf_usart_init (mode=0x%x baudrate=%lu)" VSF_TRACE_CFG_LINEEND,
                         (unsigned)p->mode, (unsigned long)VSF_TEST_RX_ERROR_DEFAULT_BAUDRATE);
    vsf_err_t err = vsf_usart_init(inst->fixture.usart, &(vsf_usart_cfg_t){
        .mode     = p->mode,
        .baudrate = VSF_TEST_RX_ERROR_DEFAULT_BAUDRATE,
        .isr      = {
            .handler_fn = __rx_error_handler,
            .target_ptr = &ctx,
            .prio       = VSF_TEST_RX_ERROR_PRIO,
        },
    });

    if (p->expect_pass) {
        VSF_TEST_ASSERT_ERR_NONE(err,
            "usart_rx_overflow_error:vsf_usart_init failed (err=%d) (mode=0x%x baudrate=%lu)" VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)p->mode, (unsigned long)VSF_TEST_RX_ERROR_DEFAULT_BAUDRATE);
        VSF_TEST_TRACE_DEBUG("usart_rx_overflow_error:vsf_usart_enable" VSF_TRACE_CFG_LINEEND);
        while (fsm_rt_cpl != vsf_usart_enable(inst->fixture.usart));

        VSF_TEST_TRACE_DEBUG("usart_rx_overflow_error:vsf_usart_irq_enable mask=0x%x" VSF_TRACE_CFG_LINEEND, (unsigned)VSF_USART_IRQ_MASK_RX_OVERFLOW_ERR);
        vsf_usart_irq_enable(inst->fixture.usart, VSF_USART_IRQ_MASK_RX_OVERFLOW_ERR);

        uint32_t elapsed_ms = 0;
        const uint32_t max_ms = VSF_TEST_RX_ERROR_PAYLOAD_DRAIN_MS * 10;
        while (!ctx.overflow_err && elapsed_ms < max_ms) {
            vsf_test_busy_wait_ms(VSF_TEST_USART_RX_ERROR_BUSY_WAIT_MS);
            elapsed_ms += 10;
        }

        VSF_TEST_TRACE_DEBUG("usart_rx_overflow_error:vsf_usart_irq_disable mask=0x%x" VSF_TRACE_CFG_LINEEND, (unsigned)VSF_USART_IRQ_MASK_RX_OVERFLOW_ERR);
        vsf_usart_irq_disable(inst->fixture.usart, VSF_USART_IRQ_MASK_RX_OVERFLOW_ERR);

        if (!ctx.overflow_err) {
            VSF_TEST_TRACE_ERROR("usart_rx_overflow_error:overflow_err not set (overflow_err=%u)" VSF_TRACE_CFG_LINEEND, (unsigned)ctx.overflow_err);
        }
        VSF_TEST_ASSERT(ctx.overflow_err);

        VSF_TEST_TRACE_DEBUG("usart_rx_overflow_error:vsf_usart_disable" VSF_TRACE_CFG_LINEEND);
        while (fsm_rt_cpl != vsf_usart_disable(inst->fixture.usart));
    } else {
        if (err == VSF_ERR_NONE) {
            VSF_TEST_TRACE_ERROR("usart_rx_overflow_error:vsf_usart_init unexpectedly succeeded (err=%d) (mode=0x%x baudrate=%lu)" VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)p->mode, (unsigned long)VSF_TEST_RX_ERROR_DEFAULT_BAUDRATE);
        }
        VSF_TEST_ASSERT(err != VSF_ERR_NONE);
    }
    VSF_TEST_TRACE_DEBUG("usart_rx_overflow_error:vsf_usart_fini" VSF_TRACE_CFG_LINEEND);
    vsf_usart_fini(inst->fixture.usart);

    VSF_TEST_TRACE_INFO("usart_rx_overflow_error:pass (expect_pass=%u)" VSF_TRACE_CFG_LINEEND, (unsigned)p->expect_pass);
}
#endif /* VSF_TEST_USART_RX_OVERFLOW_ERROR_ENABLE == ENABLED */

#endif /* VSF_TEST_USART_RX_PARITY_ERROR_ENABLE == ENABLED || VSF_TEST_USART_RX_FRAME_ERROR_ENABLE == ENABLED || VSF_TEST_USART_RX_BREAK_ERROR_ENABLE == ENABLED || VSF_TEST_USART_RX_OVERFLOW_ERROR_ENABLE == ENABLED */

/* EOF */


