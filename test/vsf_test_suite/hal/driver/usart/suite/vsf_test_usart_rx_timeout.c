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

#include "vsf_test_usart_rx_timeout.h"
#include "vsf_test_suites.h"

/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_USART_RX_TIMEOUT_ENABLE == ENABLED
const struct vsf_test_usart_rx_timeout_s {
    vsf_test_suite_t hdr;
    vsf_test_usart_rx_timeout_params_t          params[VSF_TEST_USART_RX_TIMEOUT_CASE_COUNT];
} vsf_test_usart_rx_timeout = {
    .hdr = {
        .name            = "usart_rx_timeout",
        .jmp_fn          = vsf_test_usart_rx_timeout_run,
        .case_count      = VSF_TEST_USART_RX_TIMEOUT_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_usart_rx_timeout_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_USART,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_USART_RX_TIMEOUT_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/

/*============================ LOCAL VARIABLES ===============================*/

typedef struct __rx_timeout_ctx_t {
    bool     timeout_triggered;
} __rx_timeout_ctx_t;

static void __rx_timeout_handler(void *target_ptr, vsf_usart_t *usart_ptr, vsf_usart_irq_mask_t irq_mask)
{
    __rx_timeout_ctx_t *ctx = (__rx_timeout_ctx_t *)target_ptr;

    if (irq_mask & VSF_USART_IRQ_MASK_RX_TIMEOUT) {
        ctx->timeout_triggered = true;
    }
}

#if VSF_TEST_USART_RX_TIMEOUT_ENABLE == ENABLED

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_RX_TIMEOUT_PRIO
// Must preempt PendSV — see note in vsf_test_usart_rx_irq.c.

#endif
#ifndef VSF_TEST_RX_TIMEOUT_US
// RX-idle timeout in microseconds.

#endif

/*============================ IMPLEMENTATION ================================*/

void vsf_test_usart_rx_timeout_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_usart_rx_timeout_params_t *p = tc->params;
    __rx_timeout_ctx_t ctx = { .timeout_triggered = false };

    VSF_TEST_TRACE_INFO("usart_rx_timeout:start (case=%u)" VSF_TRACE_CFG_LINEEND, (unsigned)tc->case_idx);

    VSF_TEST_TRACE_DEBUG("usart_rx_timeout:vsf_usart_init (mode=0x%x baudrate=%lu rx_timeout=%lu)" VSF_TRACE_CFG_LINEEND,
                         (unsigned)VSF_TEST_RX_TIMEOUT_DEFAULT_MODE,
                         (unsigned long)VSF_TEST_RX_TIMEOUT_DEFAULT_BAUDRATE,
                         (unsigned long)VSF_TEST_RX_TIMEOUT_US);
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_USART, usart);
    vsf_err_t err = vsf_usart_init(inst->fixture.usart, &(vsf_usart_cfg_t){
        .mode       = VSF_TEST_RX_TIMEOUT_DEFAULT_MODE,
        .baudrate   = VSF_TEST_RX_TIMEOUT_DEFAULT_BAUDRATE,
        .rx_timeout = VSF_TEST_RX_TIMEOUT_US,
        .isr        = {
            .handler_fn = __rx_timeout_handler,
            .target_ptr = &ctx,
            .prio       = VSF_TEST_RX_TIMEOUT_PRIO,
        },
    });

    if (p->expect_pass) {
        VSF_TEST_ASSERT_ERR_NONE(err,
            "usart_rx_timeout:vsf_usart_init failed (err=%d) (mode=0x%x baudrate=%lu rx_timeout=%lu)" VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)VSF_TEST_RX_TIMEOUT_DEFAULT_MODE, (unsigned long)VSF_TEST_RX_TIMEOUT_DEFAULT_BAUDRATE, (unsigned long)VSF_TEST_RX_TIMEOUT_US);

        VSF_TEST_TRACE_DEBUG("usart_rx_timeout:vsf_usart_enable" VSF_TRACE_CFG_LINEEND);
        while (fsm_rt_cpl != vsf_usart_enable(inst->fixture.usart));

        VSF_TEST_TRACE_DEBUG("usart_rx_timeout:vsf_usart_irq_enable mask=0x%x" VSF_TRACE_CFG_LINEEND, (unsigned)VSF_USART_IRQ_MASK_RX_TIMEOUT);
        vsf_usart_irq_enable(inst->fixture.usart, VSF_USART_IRQ_MASK_RX_TIMEOUT);

        uint32_t elapsed_ms = 0;
        const uint32_t max_ms = VSF_TEST_RX_TIMEOUT_PAYLOAD_DRAIN_MS * 10;
        while (!ctx.timeout_triggered && elapsed_ms < max_ms) {
            vsf_test_busy_wait_ms(VSF_TEST_USART_RX_TIMEOUT_BUSY_WAIT_MS);
            elapsed_ms += 10;
        }

        VSF_TEST_TRACE_DEBUG("usart_rx_timeout:vsf_usart_irq_disable mask=0x%x" VSF_TRACE_CFG_LINEEND, (unsigned)VSF_USART_IRQ_MASK_RX_TIMEOUT);
        vsf_usart_irq_disable(inst->fixture.usart, VSF_USART_IRQ_MASK_RX_TIMEOUT);

        if (!ctx.timeout_triggered) {
            VSF_TEST_TRACE_ERROR("usart_rx_timeout:timeout_triggered not set" VSF_TRACE_CFG_LINEEND);
        }
        VSF_TEST_ASSERT(ctx.timeout_triggered);

        VSF_TEST_TRACE_DEBUG("usart_rx_timeout:vsf_usart_disable" VSF_TRACE_CFG_LINEEND);
        while (fsm_rt_cpl != vsf_usart_disable(inst->fixture.usart));
    } else {
        if (err == VSF_ERR_NONE) {
            VSF_TEST_TRACE_ERROR("usart_rx_timeout:vsf_usart_init unexpectedly succeeded (err=%d) (mode=0x%x baudrate=%lu rx_timeout=%lu)" VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)VSF_TEST_RX_TIMEOUT_DEFAULT_MODE, (unsigned long)VSF_TEST_RX_TIMEOUT_DEFAULT_BAUDRATE, (unsigned long)VSF_TEST_RX_TIMEOUT_US);
        }
        VSF_TEST_ASSERT(err != VSF_ERR_NONE);
    }
    VSF_TEST_TRACE_DEBUG("usart_rx_timeout:vsf_usart_fini" VSF_TRACE_CFG_LINEEND);
    vsf_usart_fini(inst->fixture.usart);

    VSF_TEST_TRACE_INFO("usart_rx_timeout:pass (expect_pass=%u)" VSF_TRACE_CFG_LINEEND, (unsigned)p->expect_pass);
}

#endif /* VSF_TEST_USART_RX_TIMEOUT_ENABLE == ENABLED */

/* EOF */


