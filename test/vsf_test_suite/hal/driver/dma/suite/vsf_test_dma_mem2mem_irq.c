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

#include "vsf_test_dma_mem2mem_irq.h"
#include "vsf_test_suites.h"


/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_DMA_MEM2MEM_IRQ_ENABLE == ENABLED
const struct vsf_test_dma_mem2mem_irq_s {
    vsf_test_suite_t hdr;
    vsf_test_dma_mem2mem_irq_params_t          params[VSF_TEST_DMA_MEM2MEM_IRQ_CASE_COUNT];
} vsf_test_dma_mem2mem_irq = {
    .hdr = {
        .name            = "dma_mem2mem_irq",
        .jmp_fn          = vsf_test_dma_mem2mem_irq_run,
        .case_count      = VSF_TEST_DMA_MEM2MEM_IRQ_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_dma_mem2mem_irq_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_DMA,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_DMA_MEM2MEM_IRQ_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/



/*============================ LOCAL VARIABLES ===============================*/


#if VSF_TEST_DMA_MEM2MEM_IRQ_ENABLE == ENABLED

static void __dma_mem2mem_irq_handler(void *target_ptr, vsf_dma_t *dma_ptr,
                                       int8_t channel, vsf_dma_irq_mask_t irq_mask)
{
    VSF_UNUSED_PARAM(dma_ptr);
    VSF_UNUSED_PARAM(channel);
    VSF_UNUSED_PARAM(irq_mask);
    vsf_test_suite_t *suite = target_ptr;
    vsf_test_suite_data.dma.dma_mem2mem_irq.irq_fired = true;
}



/*============================ MACROS ========================================*/

/*============================ IMPLEMENTATION ================================*/

void vsf_test_dma_mem2mem_irq_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_dma_mem2mem_irq_params_t *p = tc->params;
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_DMA, dma);
    vsf_dma_t *dma = inst->fixture.dma;

    VSF_TEST_TRACE_INFO("dma_mem2mem_irq:start (case=%u)" VSF_TRACE_CFG_LINEEND,
                        (unsigned)tc->case_idx);

    uint8_t src_buf[VSF_TEST_DMA_MEM2MEM_IRQ_BUF_SIZE];
    uint8_t dst_buf[VSF_TEST_DMA_MEM2MEM_IRQ_BUF_SIZE] = {0};

    for (int i = 0; i < VSF_TEST_DMA_MEM2MEM_IRQ_BUF_SIZE; i++) {
        src_buf[i] = (uint8_t)(VSF_TEST_DMA_MEM2MEM_IRQ_PATTERN_BYTE + i);
    }
    VSF_TEST_TRACE_DEBUG("dma_mem2mem_irq:buffer prepared (%u bytes)"
                         VSF_TRACE_CFG_LINEEND,
                         (unsigned)VSF_TEST_DMA_MEM2MEM_IRQ_BUF_SIZE);

    vsf_test_suite_data.dma.dma_mem2mem_irq.irq_fired = false;

    /* ---- init ---- */
    VSF_TEST_TRACE_DEBUG("dma_mem2mem_irq:vsf_dma_init" VSF_TRACE_CFG_LINEEND);
    vsf_err_t err = vsf_dma_init(dma, &(vsf_dma_cfg_t){0});
    VSF_TEST_ASSERT_ERR_NONE(err,
        "dma_mem2mem_irq:vsf_dma_init failed (err=%d)"
                             VSF_TRACE_CFG_LINEEND, (int)err);

    /* ---- acquire channel ---- */
    vsf_dma_channel_hint_t hint = { .channel = -1 };
    err = vsf_dma_channel_acquire(dma, &hint);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "dma_mem2mem_irq:vsf_dma_channel_acquire failed (err=%d)"
                             VSF_TRACE_CFG_LINEEND, (int)err);
    uint8_t ch = (uint8_t)hint.channel;
    VSF_TEST_TRACE_DEBUG("dma_mem2mem_irq:vsf_dma_channel_acquire (ch=%u)"
                         VSF_TRACE_CFG_LINEEND, (unsigned)ch);

    /* ---- config channel ---- */
    VSF_TEST_TRACE_DEBUG("dma_mem2mem_irq:vsf_dma_channel_config"
                         VSF_TRACE_CFG_LINEEND);
    err = vsf_dma_channel_config(dma, ch, &(vsf_dma_channel_cfg_t){
        .mode = VSF_TEST_DMA_MEM2MEM_IRQ_MODE,
        .isr = {
            .handler_fn = __dma_mem2mem_irq_handler,
            .target_ptr = NULL,
        },
        .irq_mask = VSF_DMA_IRQ_MASK_CPL,
        .prio = VSF_TEST_DMA_MEM2MEM_IRQ_PRIO,
    });
    VSF_TEST_ASSERT_ERR_NONE(err,
        "dma_mem2mem_irq:vsf_dma_channel_config failed (err=%d) (mode=0x%x ch=%u)"
                             VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)VSF_TEST_DMA_MEM2MEM_IRQ_MODE, (unsigned)ch);

    /* ---- start transfer ---- */
    VSF_TEST_TRACE_DEBUG("dma_mem2mem_irq:vsf_dma_channel_start (%u bytes)"
                         VSF_TRACE_CFG_LINEEND,
                         (unsigned)VSF_TEST_DMA_MEM2MEM_IRQ_BUF_SIZE);
    err = vsf_dma_channel_start(dma, ch,
                                (vsf_dma_addr_t)src_buf,
                                (vsf_dma_addr_t)dst_buf,
                                VSF_TEST_DMA_MEM2MEM_IRQ_BUF_SIZE);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "dma_mem2mem_irq:vsf_dma_channel_start failed (err=%d) (ch=%u size=%u)"
                             VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)ch, (unsigned)VSF_TEST_DMA_MEM2MEM_IRQ_BUF_SIZE);

    /* ---- wait for IRQ ---- */
    VSF_TEST_TRACE_DEBUG("dma_mem2mem_irq:waiting for irq" VSF_TRACE_CFG_LINEEND);
    VSF_TEST_SPIN_FOR(vsf_test_suite_data.dma.dma_mem2mem_irq.irq_fired,
                      VSF_TEST_DMA_MEM2MEM_IRQ_SPIN_COUNT);
    if (!vsf_test_suite_data.dma.dma_mem2mem_irq.irq_fired) {
        VSF_TEST_TRACE_ERROR("dma_mem2mem_irq:irq timeout after %u spins"
                             VSF_TRACE_CFG_LINEEND,
                             (unsigned)VSF_TEST_DMA_MEM2MEM_IRQ_SPIN_COUNT);
    }
    VSF_TEST_ASSERT(vsf_test_suite_data.dma.dma_mem2mem_irq.irq_fired);

    /* ---- verify transfer count ---- */
    uint32_t transferred = vsf_dma_channel_get_transferred_count(dma, ch);
    VSF_TEST_TRACE_DEBUG("dma_mem2mem_irq:transferred=%u" VSF_TRACE_CFG_LINEEND,
                         (unsigned)transferred);
    if (transferred != VSF_TEST_DMA_MEM2MEM_IRQ_BUF_SIZE) {
        VSF_TEST_TRACE_ERROR("dma_mem2mem_irq:transfer count mismatch (expected=%u actual=%u)"
                             VSF_TRACE_CFG_LINEEND,
                             (unsigned)VSF_TEST_DMA_MEM2MEM_IRQ_BUF_SIZE, (unsigned)transferred);
    }
    VSF_TEST_ASSERT(transferred == VSF_TEST_DMA_MEM2MEM_IRQ_BUF_SIZE);

    /* ---- verify data ---- */
    bool match = true;
    for (int i = 0; i < VSF_TEST_DMA_MEM2MEM_IRQ_BUF_SIZE; i++) {
        if (dst_buf[i] != src_buf[i]) {
            VSF_TEST_TRACE_ERROR("dma_mem2mem_irq:data mismatch at offset=%d (expected=0x%02x actual=0x%02x)"
                                 VSF_TRACE_CFG_LINEEND,
                                 i, src_buf[i], dst_buf[i]);
            match = false;
            break;
        }
    }
    VSF_TEST_ASSERT(match);

    vsf_dma_channel_release(dma, ch);
    VSF_TEST_TRACE_DEBUG("dma_mem2mem_irq:vsf_dma_channel_release" VSF_TRACE_CFG_LINEEND);
    vsf_dma_fini(dma);
    VSF_TEST_TRACE_DEBUG("dma_mem2mem_irq:vsf_dma_fini" VSF_TRACE_CFG_LINEEND);

    VSF_TEST_TRACE_INFO("dma_mem2mem_irq:pass (transferred=%u bytes)"
                        VSF_TRACE_CFG_LINEEND, (unsigned)transferred);
}

#endif /* VSF_TEST_DMA_MEM2MEM_IRQ_ENABLE == ENABLED */

/* EOF */


