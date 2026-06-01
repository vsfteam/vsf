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

#include "vsf_test_dma_scatter_gather.h"
#include "vsf_test_suites.h"


/*============================ EMBEDDED SUITE ==============================*/

#if VSF_TEST_DMA_SCATTER_GATHER_ENABLE == ENABLED
const struct vsf_test_dma_scatter_gather_s {
    vsf_test_suite_t hdr;
    vsf_test_dma_scatter_gather_params_t          params[VSF_TEST_DMA_SCATTER_GATHER_CASE_COUNT];
} vsf_test_dma_scatter_gather = {
    .hdr = {
        .name            = "dma_scatter_gather",
        .jmp_fn          = vsf_test_dma_scatter_gather_run,
        .case_count      = VSF_TEST_DMA_SCATTER_GATHER_CASE_COUNT,
        .params_stride   = sizeof(vsf_test_dma_scatter_gather_params_t),
        .peripheral_type = VSF_PERIPHERAL_TYPE_DMA,
        .needs_ready     = false,
    },
    .params = { VSF_TEST_DMA_SCATTER_GATHER_PARAMS_INIT },
};
#endif

/*============================ IMPLEMENTATION ================================*/


/*============================ LOCAL VARIABLES ===============================*/


#if VSF_TEST_DMA_SCATTER_GATHER_ENABLE == ENABLED


/*============================ PROTOTYPES ====================================*/

static void __vsf_test_dma_sg_handler(void *target_ptr, vsf_dma_t *dma_ptr,
                                       int8_t channel, vsf_dma_irq_mask_t irq_mask);

/*============================ IMPLEMENTATION ================================*/

static void __vsf_test_dma_sg_handler(void *target_ptr, vsf_dma_t *dma_ptr,
                                       int8_t channel, vsf_dma_irq_mask_t irq_mask)
{
    VSF_UNUSED_PARAM(target_ptr);
    VSF_UNUSED_PARAM(dma_ptr);
    VSF_UNUSED_PARAM(channel);
    VSF_UNUSED_PARAM(irq_mask);
    vsf_test_suite_data.dma.dma_scatter_gather.sg_done = true;
}

static void __sg_prepare_buffers(void)
{
    for (uint16_t i = 0; i < sizeof(vsf_test_suite_data.dma.dma_scatter_gather.sg_src_buf); i++) {
        vsf_test_suite_data.dma.dma_scatter_gather.sg_src_buf[i] = (uint8_t)(VSF_TEST_DMA_SCATTER_GATHER_PATTERN_BYTE + i);
    }
    memset(vsf_test_suite_data.dma.dma_scatter_gather.sg_dst_buf, 0, sizeof(vsf_test_suite_data.dma.dma_scatter_gather.sg_dst_buf));
}

static bool __sg_verify_two_segment(void)
{
    for (uint16_t i = 0; i < VSF_TEST_DMA_SCATTER_GATHER_BUF_SIZE; i++) {
        if (vsf_test_suite_data.dma.dma_scatter_gather.sg_dst_buf[i] != vsf_test_suite_data.dma.dma_scatter_gather.sg_src_buf[i]) {
            VSF_TEST_TRACE_ERROR("dma_scatter_gather:two_segment:data mismatch at offset=%u (expected=0x%02x actual=0x%02x)"
                                 VSF_TRACE_CFG_LINEEND,
                                 (unsigned)i,
                                 vsf_test_suite_data.dma.dma_scatter_gather.sg_src_buf[i],
                                 vsf_test_suite_data.dma.dma_scatter_gather.sg_dst_buf[i]);
            return false;
        }
    }
    for (uint16_t i = 0; i < VSF_TEST_DMA_SCATTER_GATHER_BUF_SIZE; i++) {
        if (vsf_test_suite_data.dma.dma_scatter_gather.sg_dst_buf[VSF_TEST_DMA_SCATTER_GATHER_BUF_SIZE + i] != vsf_test_suite_data.dma.dma_scatter_gather.sg_src_buf[VSF_TEST_DMA_SCATTER_GATHER_BUF_SIZE + i]) {
            VSF_TEST_TRACE_ERROR("dma_scatter_gather:two_segment:data mismatch at offset=%u (expected=0x%02x actual=0x%02x)"
                                 VSF_TRACE_CFG_LINEEND,
                                 (unsigned)(VSF_TEST_DMA_SCATTER_GATHER_BUF_SIZE + i),
                                 vsf_test_suite_data.dma.dma_scatter_gather.sg_src_buf[VSF_TEST_DMA_SCATTER_GATHER_BUF_SIZE + i],
                                 vsf_test_suite_data.dma.dma_scatter_gather.sg_dst_buf[VSF_TEST_DMA_SCATTER_GATHER_BUF_SIZE + i]);
            return false;
        }
    }
    return true;
}

static bool __sg_verify_scatter_read(void)
{
    for (uint16_t seg = 0; seg < VSF_TEST_DMA_SCATTER_GATHER_SEGMENT_COUNT; seg++) {
        for (uint16_t i = 0; i < VSF_TEST_DMA_SCATTER_GATHER_BUF_SIZE; i++) {
            uint16_t src_idx = seg * VSF_TEST_DMA_SCATTER_GATHER_BUF_SIZE * 2 + i;
            uint16_t dst_idx = seg * VSF_TEST_DMA_SCATTER_GATHER_BUF_SIZE + i;
            if (vsf_test_suite_data.dma.dma_scatter_gather.sg_dst_buf[dst_idx] != vsf_test_suite_data.dma.dma_scatter_gather.sg_src_buf[src_idx]) {
                VSF_TEST_TRACE_ERROR("dma_scatter_gather:scatter_read:data mismatch at seg=%u offset=%u (src_idx=%u expected=0x%02x actual=0x%02x)"
                                     VSF_TRACE_CFG_LINEEND,
                                     (unsigned)seg, (unsigned)i, (unsigned)src_idx,
                                     vsf_test_suite_data.dma.dma_scatter_gather.sg_src_buf[src_idx],
                                     vsf_test_suite_data.dma.dma_scatter_gather.sg_dst_buf[dst_idx]);
                return false;
            }
        }
    }
    return true;
}

static bool __sg_verify_gather_write(void)
{
    for (uint16_t seg = 0; seg < VSF_TEST_DMA_SCATTER_GATHER_SEGMENT_COUNT; seg++) {
        for (uint16_t i = 0; i < VSF_TEST_DMA_SCATTER_GATHER_BUF_SIZE; i++) {
            uint16_t src_idx = seg * VSF_TEST_DMA_SCATTER_GATHER_BUF_SIZE + i;
            uint16_t dst_idx = seg * VSF_TEST_DMA_SCATTER_GATHER_BUF_SIZE * 2 + i;
            if (vsf_test_suite_data.dma.dma_scatter_gather.sg_dst_buf[dst_idx] != vsf_test_suite_data.dma.dma_scatter_gather.sg_src_buf[src_idx]) {
                VSF_TEST_TRACE_ERROR("dma_scatter_gather:gather_write:data mismatch at seg=%u offset=%u (dst_idx=%u expected=0x%02x actual=0x%02x)"
                                     VSF_TRACE_CFG_LINEEND,
                                     (unsigned)seg, (unsigned)i, (unsigned)dst_idx,
                                     vsf_test_suite_data.dma.dma_scatter_gather.sg_src_buf[src_idx],
                                     vsf_test_suite_data.dma.dma_scatter_gather.sg_dst_buf[dst_idx]);
                return false;
            }
        }
    }
    return true;
}

void vsf_test_dma_scatter_gather_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst)
{
    vsf_test_dma_scatter_gather_params_t *p = tc->params;
    VSF_TEST_ASSERT_INST(inst, VSF_PERIPHERAL_TYPE_DMA, dma);
    vsf_dma_t *dma = inst->fixture.dma;

    VSF_TEST_TRACE_INFO("dma_scatter_gather:start (case=%u)" VSF_TRACE_CFG_LINEEND,
                        (unsigned)tc->case_idx);

    /* ---- init ---- */
    VSF_TEST_TRACE_DEBUG("dma_scatter_gather:vsf_dma_init" VSF_TRACE_CFG_LINEEND);
    vsf_err_t err = vsf_dma_init(dma, &(vsf_dma_cfg_t){0});
    VSF_TEST_ASSERT_ERR_NONE(err,
        "dma_scatter_gather:vsf_dma_init failed (err=%d)"
                             VSF_TRACE_CFG_LINEEND, (int)err);

    /* ---- acquire channel ---- */
    vsf_dma_channel_hint_t hint = { .channel = -1 };
    err = vsf_dma_channel_acquire(dma, &hint);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "dma_scatter_gather:vsf_dma_channel_acquire failed (err=%d)"
                             VSF_TRACE_CFG_LINEEND, (int)err);
    uint8_t ch = (uint8_t)hint.channel;
    VSF_TEST_TRACE_DEBUG("dma_scatter_gather:vsf_dma_channel_acquire (ch=%u)"
                         VSF_TRACE_CFG_LINEEND, (unsigned)ch);

    /* ---- config channel ---- */
    vsf_dma_channel_cfg_t ch_cfg = {
        .mode = VSF_TEST_DMA_SCATTER_GATHER_MODE
              | VSF_DMA_DST_WIDTH_BYTE_1,
        .irq_mask = VSF_DMA_IRQ_MASK_CPL,
    };
    VSF_TEST_TRACE_DEBUG("dma_scatter_gather:vsf_dma_channel_config"
                         VSF_TRACE_CFG_LINEEND);
    err = vsf_dma_channel_config(dma, ch, &ch_cfg);
    VSF_TEST_ASSERT_ERR_NONE(err,
        "dma_scatter_gather:vsf_dma_channel_config failed (err=%d) (mode=0x%x ch=%u)"
                             VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)ch_cfg.mode, (unsigned)ch);

    __sg_prepare_buffers();
    VSF_TEST_TRACE_DEBUG("dma_scatter_gather:buffer prepared (%u bytes x %u segments)"
                         VSF_TRACE_CFG_LINEEND,
                         (unsigned)VSF_TEST_DMA_SCATTER_GATHER_BUF_SIZE,
                         (unsigned)VSF_TEST_DMA_SCATTER_GATHER_SEGMENT_COUNT);

    bool pass = true;

    switch (p->idx) {
    case 0: {
        /* --- Test 0: Two-segment M2M --- */
        VSF_TEST_TRACE_DEBUG("dma_scatter_gather:two_segment_start"
                             VSF_TRACE_CFG_LINEEND);

        vsf_dma_channel_sg_desc_t descs[2] = {
            {
                .mode = ch_cfg.mode,
                .src_address = (vsf_dma_addr_t)&vsf_test_suite_data.dma.dma_scatter_gather.sg_src_buf[0],
                .dst_address = (vsf_dma_addr_t)&vsf_test_suite_data.dma.dma_scatter_gather.sg_dst_buf[0],
                .count = VSF_TEST_DMA_SCATTER_GATHER_BUF_SIZE,
            },
            {
                .mode = ch_cfg.mode,
                .src_address = (vsf_dma_addr_t)&vsf_test_suite_data.dma.dma_scatter_gather.sg_src_buf[VSF_TEST_DMA_SCATTER_GATHER_BUF_SIZE],
                .dst_address = (vsf_dma_addr_t)&vsf_test_suite_data.dma.dma_scatter_gather.sg_dst_buf[VSF_TEST_DMA_SCATTER_GATHER_BUF_SIZE],
                .count = VSF_TEST_DMA_SCATTER_GATHER_BUF_SIZE,
            },
        };

        vsf_test_suite_data.dma.dma_scatter_gather.sg_done = false;
        err = vsf_dma_channel_sg_config_desc(dma, ch,
            (vsf_dma_isr_t){ .handler_fn = __vsf_test_dma_sg_handler, .target_ptr = NULL },
            descs, dimof(descs));
        VSF_TEST_ASSERT_ERR_NONE(err,
            "dma_scatter_gather:two_segment:sg_config_desc failed (err=%d) (ch=%u)"
                                 VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)ch);

        err = vsf_dma_channel_sg_start(dma, ch);
        VSF_TEST_ASSERT_ERR_NONE(err,
            "dma_scatter_gather:two_segment:sg_start failed (err=%d) (ch=%u)"
                                 VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)ch);

        VSF_TEST_SPIN_FOR(vsf_test_suite_data.dma.dma_scatter_gather.sg_done, VSF_TEST_DMA_SCATTER_GATHER_SPIN_COUNT);
        if (!vsf_test_suite_data.dma.dma_scatter_gather.sg_done) {
            VSF_TEST_TRACE_ERROR("dma_scatter_gather:two_segment:sg timeout after %u spins"
                                 VSF_TRACE_CFG_LINEEND,
                                 (unsigned)VSF_TEST_DMA_SCATTER_GATHER_SPIN_COUNT);
        }
        VSF_TEST_ASSERT(vsf_test_suite_data.dma.dma_scatter_gather.sg_done);

        pass = __sg_verify_two_segment();
        if (!pass) {
            VSF_TEST_TRACE_ERROR("dma_scatter_gather:two_segment:data mismatch"
                                 VSF_TRACE_CFG_LINEEND);
        }
        VSF_TEST_TRACE_DEBUG("dma_scatter_gather:two_segment_%s"
                             VSF_TRACE_CFG_LINEEND,
                             pass ? "pass" : "fail");
        break;
    }

    case 1: {
        /* --- Test 1: Scatter read --- */
        VSF_TEST_TRACE_DEBUG("dma_scatter_gather:scatter_read_start"
                             VSF_TRACE_CFG_LINEEND);

        vsf_dma_channel_sg_desc_t descs[4];
        for (uint8_t seg = 0; seg < VSF_TEST_DMA_SCATTER_GATHER_SEGMENT_COUNT; seg++) {
            descs[seg].mode = ch_cfg.mode;
            descs[seg].src_address = (vsf_dma_addr_t)&vsf_test_suite_data.dma.dma_scatter_gather.sg_src_buf[seg * VSF_TEST_DMA_SCATTER_GATHER_BUF_SIZE * 2];
            descs[seg].dst_address = (vsf_dma_addr_t)&vsf_test_suite_data.dma.dma_scatter_gather.sg_dst_buf[seg * VSF_TEST_DMA_SCATTER_GATHER_BUF_SIZE];
            descs[seg].count = VSF_TEST_DMA_SCATTER_GATHER_BUF_SIZE;
        }

        vsf_test_suite_data.dma.dma_scatter_gather.sg_done = false;
        err = vsf_dma_channel_sg_config_desc(dma, ch,
            (vsf_dma_isr_t){ .handler_fn = __vsf_test_dma_sg_handler, .target_ptr = NULL },
            descs, dimof(descs));
        VSF_TEST_ASSERT_ERR_NONE(err,
            "dma_scatter_gather:scatter_read:sg_config_desc failed (err=%d) (ch=%u)"
                                 VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)ch);

        err = vsf_dma_channel_sg_start(dma, ch);
        VSF_TEST_ASSERT_ERR_NONE(err,
            "dma_scatter_gather:scatter_read:sg_start failed (err=%d) (ch=%u)"
                                 VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)ch);

        VSF_TEST_SPIN_FOR(vsf_test_suite_data.dma.dma_scatter_gather.sg_done, VSF_TEST_DMA_SCATTER_GATHER_SPIN_COUNT);
        if (!vsf_test_suite_data.dma.dma_scatter_gather.sg_done) {
            VSF_TEST_TRACE_ERROR("dma_scatter_gather:scatter_read:sg timeout after %u spins"
                                 VSF_TRACE_CFG_LINEEND,
                                 (unsigned)VSF_TEST_DMA_SCATTER_GATHER_SPIN_COUNT);
        }
        VSF_TEST_ASSERT(vsf_test_suite_data.dma.dma_scatter_gather.sg_done);

        pass = __sg_verify_scatter_read();
        if (!pass) {
            VSF_TEST_TRACE_ERROR("dma_scatter_gather:scatter_read:data mismatch"
                                 VSF_TRACE_CFG_LINEEND);
        }
        VSF_TEST_TRACE_DEBUG("dma_scatter_gather:scatter_read_%s"
                             VSF_TRACE_CFG_LINEEND,
                             pass ? "pass" : "fail");
        break;
    }

    case 2: {
        /* --- Test 2: Gather write --- */
        VSF_TEST_TRACE_DEBUG("dma_scatter_gather:gather_write_start"
                             VSF_TRACE_CFG_LINEEND);

        vsf_dma_channel_sg_desc_t descs[4];
        for (uint8_t seg = 0; seg < VSF_TEST_DMA_SCATTER_GATHER_SEGMENT_COUNT; seg++) {
            descs[seg].mode = ch_cfg.mode;
            descs[seg].src_address = (vsf_dma_addr_t)&vsf_test_suite_data.dma.dma_scatter_gather.sg_src_buf[seg * VSF_TEST_DMA_SCATTER_GATHER_BUF_SIZE];
            descs[seg].dst_address = (vsf_dma_addr_t)&vsf_test_suite_data.dma.dma_scatter_gather.sg_dst_buf[seg * VSF_TEST_DMA_SCATTER_GATHER_BUF_SIZE * 2];
            descs[seg].count = VSF_TEST_DMA_SCATTER_GATHER_BUF_SIZE;
        }

        vsf_test_suite_data.dma.dma_scatter_gather.sg_done = false;
        err = vsf_dma_channel_sg_config_desc(dma, ch,
            (vsf_dma_isr_t){ .handler_fn = __vsf_test_dma_sg_handler, .target_ptr = NULL },
            descs, dimof(descs));
        VSF_TEST_ASSERT_ERR_NONE(err,
            "dma_scatter_gather:gather_write:sg_config_desc failed (err=%d) (ch=%u)"
                                 VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)ch);

        err = vsf_dma_channel_sg_start(dma, ch);
        VSF_TEST_ASSERT_ERR_NONE(err,
            "dma_scatter_gather:gather_write:sg_start failed (err=%d) (ch=%u)"
                                 VSF_TRACE_CFG_LINEEND, (int)err, (unsigned)ch);

        VSF_TEST_SPIN_FOR(vsf_test_suite_data.dma.dma_scatter_gather.sg_done, VSF_TEST_DMA_SCATTER_GATHER_SPIN_COUNT);
        if (!vsf_test_suite_data.dma.dma_scatter_gather.sg_done) {
            VSF_TEST_TRACE_ERROR("dma_scatter_gather:gather_write:sg timeout after %u spins"
                                 VSF_TRACE_CFG_LINEEND,
                                 (unsigned)VSF_TEST_DMA_SCATTER_GATHER_SPIN_COUNT);
        }
        VSF_TEST_ASSERT(vsf_test_suite_data.dma.dma_scatter_gather.sg_done);

        pass = __sg_verify_gather_write();
        if (!pass) {
            VSF_TEST_TRACE_ERROR("dma_scatter_gather:gather_write:data mismatch"
                                 VSF_TRACE_CFG_LINEEND);
        }
        VSF_TEST_TRACE_DEBUG("dma_scatter_gather:gather_write_%s"
                             VSF_TRACE_CFG_LINEEND,
                             pass ? "pass" : "fail");
        break;
    }

    default:
        VSF_TEST_TRACE_ERROR("dma_scatter_gather:invalid case index=%u"
                             VSF_TRACE_CFG_LINEEND, (unsigned)p->idx);
        VSF_TEST_ASSERT(0);
        break;
    }

    VSF_TEST_ASSERT(pass);

    vsf_dma_channel_release(dma, ch);
    VSF_TEST_TRACE_DEBUG("dma_scatter_gather:vsf_dma_channel_release" VSF_TRACE_CFG_LINEEND);
    vsf_dma_fini(dma);
    VSF_TEST_TRACE_DEBUG("dma_scatter_gather:vsf_dma_fini" VSF_TRACE_CFG_LINEEND);

    VSF_TEST_TRACE_INFO("dma_scatter_gather:pass (case=%u)"
                        VSF_TRACE_CFG_LINEEND, (unsigned)tc->case_idx);
}

#endif /* VSF_TEST_DMA_SCATTER_GATHER_ENABLE == ENABLED */
/* EOF */

