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

#include "vsf_test_spi_async.h"
#include "vsf_test_suites.h"


/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_SPI_ASYNC_ENABLE == ENABLED
const struct vsf_test_spi_async_s {
    vsf_test_suite_t hdr;
    vsf_test_spi_async_params_t          params[VSF_TEST_SPI_ASYNC_CASE_COUNT];
} vsf_test_spi_async = {
    .hdr = {
        .name            = "spi_async",
        .jmp_fn          = vsf_test_spi_async_run,
        .case_count      = VSF_TEST_SPI_ASYNC_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_spi_async_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_SPI,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_SPI_ASYNC_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/


/*============================ LOCAL VARIABLES ===============================*/


#if VSF_TEST_SPI_ASYNC_ENABLE == ENABLED



/*============================ TYPES =========================================*/

typedef struct {
    volatile bool           done;
    vsf_spi_irq_mask_t      irq_mask;
} vsf_test_spi_async_ctx_t;

/*============================ PROTOTYPES ====================================*/

static void __vsf_test_spi_async_handler(void *target_ptr, vsf_spi_t *spi_ptr,
                                          vsf_spi_irq_mask_t irq_mask);

/*============================ IMPLEMENTATION ================================*/

static void __vsf_test_spi_async_handler(void *target_ptr, vsf_spi_t *spi_ptr,
                                          vsf_spi_irq_mask_t irq_mask)
{
    VSF_UNUSED_PARAM(spi_ptr);
    vsf_test_spi_async_ctx_t *ctx = (vsf_test_spi_async_ctx_t *)target_ptr;
    ctx->done = true;
    ctx->irq_mask = irq_mask;
}

static void __spi_async_prepare_buffers(uint16_t len)
{
    for (uint16_t i = 0; i < len; i++) {
        vsf_test_suite_data.spi.spi_async.spi_async_tx_buf[i] = (uint8_t)(VSF_TEST_SPI_ASYNC_PATTERN_BYTE + i);
        vsf_test_suite_data.spi.spi_async.spi_async_rx_buf[i] = 0;
    }
}

void vsf_test_spi_async_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_spi_async_params_t *p = tc->params;
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_SPI, spi);
    vsf_spi_t *spi = inst->fixture.spi;
    vsf_test_spi_async_ctx_t ctx = { .done = false, .irq_mask = 0 };

    uint16_t data_len = p->data_len;
    if (data_len == 0 || data_len > VSF_TEST_SPI_ASYNC_MAX_DATA_LEN) {
        data_len = VSF_TEST_SPI_ASYNC_MAX_DATA_LEN;
    }

    vsf_err_t err = vsf_spi_init(spi, &(vsf_spi_cfg_t){
        .mode      = VSF_SPI_MASTER | p->mode | VSF_SPI_DATASIZE_8,
        .clock_hz  = p->clock_hz,
        .isr       = {
            .handler_fn = __vsf_test_spi_async_handler,
            .target_ptr = &ctx,
        },
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    while (fsm_rt_cpl != vsf_spi_enable(spi));

    /* Activate CS (software) */
    vsf_spi_cs_active(spi, 0);

    bool pass = true;

    switch (p->test_type) {
    case 0: {
        /* --- Test 0: Full-duplex async transfer --- */
        VSF_TEST_TRACE_INFO("spi_async:full_duplex_start" VSF_TRACE_CFG_LINEEND);
        __spi_async_prepare_buffers(data_len);
        ctx.done = false;
        ctx.irq_mask = 0;

        err = vsf_spi_request_transfer(spi, vsf_test_suite_data.spi.spi_async.spi_async_tx_buf,
                                       vsf_test_suite_data.spi.spi_async.spi_async_rx_buf, data_len);
        VSF_TEST_ASSERT(err == VSF_ERR_NONE);

        /* Wait for completion */
        VSF_TEST_SPIN_FOR(ctx.done, VSF_TEST_SPI_ASYNC_SPIN_COUNT);
        VSF_TEST_ASSERT(ctx.done);
        VSF_TEST_ASSERT(ctx.irq_mask & (VSF_SPI_IRQ_MASK_TX_CPL | VSF_SPI_IRQ_MASK_RX_CPL));

        /* Verify data */
        for (uint16_t i = 0; i < data_len; i++) {
            if (vsf_test_suite_data.spi.spi_async.spi_async_rx_buf[i] != vsf_test_suite_data.spi.spi_async.spi_async_tx_buf[i]) {
                VSF_TEST_TRACE_ERROR("spi_async:full_duplex data mismatch at offset=%u (expected=0x%02x actual=0x%02x)"
                                     VSF_TRACE_CFG_LINEEND,
                                     (unsigned)i,
                                     vsf_test_suite_data.spi.spi_async.spi_async_tx_buf[i],
                                     vsf_test_suite_data.spi.spi_async.spi_async_rx_buf[i]);
                pass = false;
                break;
            }
        }
        if (pass) {
            VSF_TEST_TRACE_INFO("spi_async:full_duplex_pass" VSF_TRACE_CFG_LINEEND);
        }
        break;
    }

    case 1: {
        /* --- Test 1: TX-only async transfer --- */
        VSF_TEST_TRACE_INFO("spi_async:tx_only_start" VSF_TRACE_CFG_LINEEND);
        __spi_async_prepare_buffers(data_len);
        ctx.done = false;
        ctx.irq_mask = 0;

        err = vsf_spi_request_transfer(spi, vsf_test_suite_data.spi.spi_async.spi_async_tx_buf, NULL, data_len);
        VSF_TEST_ASSERT(err == VSF_ERR_NONE);

        VSF_TEST_SPIN_FOR(ctx.done, VSF_TEST_SPI_ASYNC_SPIN_COUNT);
        VSF_TEST_ASSERT(ctx.done);
        VSF_TEST_ASSERT(ctx.irq_mask & VSF_SPI_IRQ_MASK_TX_CPL);
        VSF_TEST_TRACE_INFO("spi_async:tx_only_pass" VSF_TRACE_CFG_LINEEND);
        break;
    }

    case 2: {
        /* --- Test 2: RX-only async transfer --- */
        VSF_TEST_TRACE_INFO("spi_async:rx_only_start" VSF_TRACE_CFG_LINEEND);
        for (uint16_t i = 0; i < data_len; i++) {
            vsf_test_suite_data.spi.spi_async.spi_async_rx_buf[i] = 0;
        }
        ctx.done = false;
        ctx.irq_mask = 0;

        err = vsf_spi_request_transfer(spi, NULL, vsf_test_suite_data.spi.spi_async.spi_async_rx_buf, data_len);
        VSF_TEST_ASSERT(err == VSF_ERR_NONE);

        VSF_TEST_SPIN_FOR(ctx.done, VSF_TEST_SPI_ASYNC_SPIN_COUNT);
        VSF_TEST_ASSERT(ctx.done);
        VSF_TEST_ASSERT(ctx.irq_mask & VSF_SPI_IRQ_MASK_RX_CPL);
        /* With loopback, RX should contain the dummy 0x00 bytes read back */
        VSF_TEST_TRACE_INFO("spi_async:rx_only_pass" VSF_TRACE_CFG_LINEEND);
        break;
    }

    case 3: {
        /* --- Test 3: Cancel during transfer --- */
        VSF_TEST_TRACE_INFO("spi_async:cancel_start" VSF_TRACE_CFG_LINEEND);
        __spi_async_prepare_buffers(data_len);
        ctx.done = false;
        ctx.irq_mask = 0;

        err = vsf_spi_request_transfer(spi, vsf_test_suite_data.spi.spi_async.spi_async_tx_buf,
                                       vsf_test_suite_data.spi.spi_async.spi_async_rx_buf, data_len);
        VSF_TEST_ASSERT(err == VSF_ERR_NONE);

        /* Cancel immediately (before completion) */
        vsf_spi_cancel_transfer(spi);

        /* Verify transfer was cancelled — ctx should NOT have completed
         * via the normal callback path since we cancelled. */
        VSF_TEST_ASSERT(!ctx.done);

        /* Verify SPI is no longer busy */
        vsf_spi_status_t status = vsf_spi_status(spi);
        VSF_TEST_ASSERT(!status.is_busy);

        VSF_TEST_TRACE_INFO("spi_async:cancel_pass" VSF_TRACE_CFG_LINEEND);
        break;
    }

    default:
        VSF_TEST_ASSERT(0);
        break;
    }

    vsf_spi_cs_inactive(spi, 0);
    while (fsm_rt_cpl != vsf_spi_disable(spi));
    vsf_spi_fini(spi);
}

#endif /* VSF_TEST_SPI_ASYNC_ENABLE == ENABLED */
/* EOF */