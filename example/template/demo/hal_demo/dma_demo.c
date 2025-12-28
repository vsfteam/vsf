/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
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
 ****************************************************************************/

/*============================ INCLUDES ======================================*/

#include "vsf.h"

#if APP_USE_HAL_DEMO == ENABLED && APP_USE_HAL_DMA_DEMO == ENABLED && VSF_HAL_USE_DMA == ENABLED

#if APP_USE_LINUX_DEMO == ENABLED
#   include <string.h>
#endif
#include <inttypes.h>

/*============================ MACROS ========================================*/

#ifndef APP_DMA_DEMO_CFG_DEFAULT_INSTANCE
#   define APP_DMA_DEMO_CFG_DEFAULT_INSTANCE        vsf_hw_dma1
#endif

#ifndef APP_DMA_DEMO_CFG_IRQ_PRIO
#   define APP_DMA_DEMO_CFG_IRQ_PRIO                vsf_arch_prio_2
#endif

#define DMA_TEST_BUFFER_SIZE                       256
#define DMA_TEST_POLL_INTERVAL_MS                  10
#define DMA_TEST_TIMEOUT_MS                        1000

/*============================ MACROFIED FUNCTIONS ===========================*/

#define __DMA_TRACE_INFO(fmt, ...)                  do { \
                                                        uint64_t __dma_time_ms = (uint64_t)vsf_systimer_get_ms(); \
                                                        vsf_trace_info("[%" PRIu64 " ms] " fmt, __dma_time_ms, ##__VA_ARGS__); \
                                                    } while(0)
#define __DMA_TRACE_ERROR(fmt, ...)                 do { \
                                                        uint64_t __dma_time_ms = (uint64_t)vsf_systimer_get_ms(); \
                                                        vsf_trace_error("[%" PRIu64 " ms] " fmt, __dma_time_ms, ##__VA_ARGS__); \
                                                    } while(0)
#define __DMA_TRACE_WARNING(fmt, ...)               do { \
                                                        uint64_t __dma_time_ms = (uint64_t)vsf_systimer_get_ms(); \
                                                        vsf_trace_warning("[%" PRIu64 " ms] " fmt, __dma_time_ms, ##__VA_ARGS__); \
                                                    } while(0)

/*============================ TYPES =========================================*/

typedef struct dma_test_t {
    vsf_dma_t *dma;
    int8_t channel;
    vsf_dma_capability_t cap;
    vsf_dma_cfg_t cfg;
    volatile bool transfer_complete;
    volatile bool transfer_error;
    uint8_t src_buffer[DMA_TEST_BUFFER_SIZE];
    uint8_t dst_buffer[DMA_TEST_BUFFER_SIZE];
} dma_test_t;

/*============================ PROTOTYPES ====================================*/

static void __dma_irq_handler(void *target_ptr, vsf_dma_t *dma_ptr, int8_t channel, vsf_dma_irq_mask_t irq_mask);
static bool __dma_setup(dma_test_t *ctx);
static void __dma_cleanup(dma_test_t *ctx);
static bool __dma_test_memory_to_memory(dma_test_t *ctx);

/*============================ IMPLEMENTATION ================================*/

static void __dma_irq_handler(void *target_ptr, vsf_dma_t *dma_ptr, int8_t channel, vsf_dma_irq_mask_t irq_mask)
{
    dma_test_t *ctx = (dma_test_t *)target_ptr;

    __DMA_TRACE_INFO("DMA IRQ handler: channel=%d, irq_mask=0x%X" VSF_TRACE_CFG_LINEEND, channel, irq_mask);

    if (ctx != NULL) {
        if (irq_mask & VSF_DMA_IRQ_MASK_CPL) {
            __DMA_TRACE_INFO("DMA transfer completion interrupt received" VSF_TRACE_CFG_LINEEND);
            ctx->transfer_complete = true;
        }
        if (irq_mask & VSF_DMA_IRQ_MASK_HALF_CPL) {
            __DMA_TRACE_INFO("DMA transfer half completion interrupt received" VSF_TRACE_CFG_LINEEND);
        }
        if (irq_mask & VSF_DMA_IRQ_MASK_ERROR) {
            __DMA_TRACE_ERROR("DMA transfer error interrupt received" VSF_TRACE_CFG_LINEEND);
            ctx->transfer_error = true;
        }
    } else {
        __DMA_TRACE_WARNING("DMA IRQ handler called with NULL context" VSF_TRACE_CFG_LINEEND);
    }
}

static bool __dma_setup(dma_test_t *ctx)
{
    if ((ctx == NULL) || (ctx->dma == NULL)) {
        __DMA_TRACE_ERROR("DMA test context or device is NULL!" VSF_TRACE_CFG_LINEEND);
        return false;
    }

    // Get DMA capability
    ctx->cap = vsf_dma_capability(ctx->dma);
    __DMA_TRACE_INFO("DMA Capability: channel_count=%d, irq_count=%d, max_transfer_count=%d, addr_alignment=%d, supported_modes=0x%X" VSF_TRACE_CFG_LINEEND,
                    ctx->cap.channel_count, ctx->cap.irq_count, ctx->cap.max_transfer_count, ctx->cap.addr_alignment, ctx->cap.supported_modes);

    // Check if memory-to-memory is supported
    // Note: VSF_DMA_MEMORY_TO_MEMORY = 0, so we can't directly check with bitwise AND
    // According to AT32F405 documentation, DMA supports memory-to-memory transfer
    // We check if the direction mask in supported_modes includes all directions
    // or if it's the full mode mask (which includes all directions)
    vsf_dma_channel_mode_t direction_bits = ctx->cap.supported_modes & VSF_DMA_DIRECTION_MASK;
    __DMA_TRACE_INFO("DMA direction_bits=0x%X, VSF_DMA_DIRECTION_MASK=0x%X" VSF_TRACE_CFG_LINEEND, direction_bits, VSF_DMA_DIRECTION_MASK);

    // According to AT32F405 documentation, memory-to-memory is supported
    // If supported_modes includes VSF_DMA_DIRECTION_MASK, all directions are supported
    // Otherwise, we trust the documentation and proceed (actual support will be verified during transfer)
    if ((direction_bits != VSF_DMA_DIRECTION_MASK) && (direction_bits != 0)) {
        // Some directions are supported but not all - this is unusual but we proceed
        __DMA_TRACE_WARNING("DMA direction support is partial, but proceeding with memory-to-memory test" VSF_TRACE_CFG_LINEEND);
    }

    // Initialize DMA
    ctx->cfg.prio = APP_DMA_DEMO_CFG_IRQ_PRIO;
    if (vsf_dma_init(ctx->dma, &ctx->cfg) != VSF_ERR_NONE) {
        __DMA_TRACE_ERROR("DMA init failed" VSF_TRACE_CFG_LINEEND);
        return false;
    }

    // Acquire a channel
    vsf_dma_channel_hint_t channel_hint = VSF_DMA_CHANNEL_HINT_INIT(-1, 0);
    if (vsf_dma_channel_acquire(ctx->dma, &channel_hint) != VSF_ERR_NONE) {
        __DMA_TRACE_ERROR("DMA channel acquire failed" VSF_TRACE_CFG_LINEEND);
        vsf_dma_fini(ctx->dma);
        return false;
    }
    ctx->channel = channel_hint.channel;
    __DMA_TRACE_INFO("DMA channel %d acquired" VSF_TRACE_CFG_LINEEND, ctx->channel);

    // Configure channel for memory-to-memory transfer
    vsf_dma_isr_t isr = {
        .handler_fn = __dma_irq_handler,
        .target_ptr = ctx,
    };
    vsf_dma_channel_cfg_t channel_cfg = VSF_DMA_CHANNEL_CFG_INIT(
        VSF_DMA_MEMORY_TO_MEMORY | VSF_DMA_SRC_ADDR_INCREMENT | VSF_DMA_DST_ADDR_INCREMENT |
        VSF_DMA_SRC_WIDTH_BYTE_1 | VSF_DMA_DST_WIDTH_BYTE_1 | VSF_DMA_PRIORITY_MEDIUM,
        isr,
        VSF_DMA_IRQ_MASK_CPL | VSF_DMA_IRQ_MASK_ERROR,
        0,  // src_request_idx (not used for memory-to-memory)
        0,  // dst_request_idx (not used for memory-to-memory)
        APP_DMA_DEMO_CFG_IRQ_PRIO  // prio: must be valid for AT32F405 DMA
    );

    if (vsf_dma_channel_config(ctx->dma, ctx->channel, &channel_cfg) != VSF_ERR_NONE) {
        __DMA_TRACE_ERROR("DMA channel config failed" VSF_TRACE_CFG_LINEEND);
        vsf_dma_channel_release(ctx->dma, ctx->channel);
        vsf_dma_fini(ctx->dma);
        return false;
    }

    ctx->transfer_complete = false;
    ctx->transfer_error = false;

    return true;
}

static void __dma_cleanup(dma_test_t *ctx)
{
    if ((ctx == NULL) || (ctx->dma == NULL)) {
        return;
    }

    // Cancel any ongoing transfer
    vsf_dma_channel_cancel(ctx->dma, ctx->channel);

    // Release channel
    vsf_dma_channel_release(ctx->dma, ctx->channel);

    // Finalize DMA
    vsf_dma_fini(ctx->dma);
}

static bool __dma_test_memory_to_memory(dma_test_t *ctx)
{
    __DMA_TRACE_INFO("=== DMA Memory-to-Memory Test ===" VSF_TRACE_CFG_LINEEND);

    if (!__dma_setup(ctx)) {
        return false;
    }

    // Initialize source buffer with test pattern
    for (uint32_t i = 0; i < DMA_TEST_BUFFER_SIZE; i++) {
        ctx->src_buffer[i] = (uint8_t)(i & 0xFF);
    }

    // Clear destination buffer
    for (uint32_t i = 0; i < DMA_TEST_BUFFER_SIZE; i++) {
        ctx->dst_buffer[i] = 0;
    }

    __DMA_TRACE_INFO("Starting DMA transfer: %d bytes from 0x%p to 0x%p" VSF_TRACE_CFG_LINEEND,
                    DMA_TEST_BUFFER_SIZE, ctx->src_buffer, ctx->dst_buffer);

    // Start DMA transfer
    vsf_err_t err = vsf_dma_channel_start(ctx->dma, ctx->channel,
                                         (vsf_dma_addr_t)ctx->src_buffer,
                                         (vsf_dma_addr_t)ctx->dst_buffer,
                                         DMA_TEST_BUFFER_SIZE);
    if (err != VSF_ERR_NONE) {
        __DMA_TRACE_ERROR("DMA channel start failed: %d" VSF_TRACE_CFG_LINEEND, err);
        __dma_cleanup(ctx);
        return false;
    }

    // Wait for transfer to complete
    vsf_systimer_tick_t start_tick = vsf_systimer_get_tick();
    vsf_systimer_tick_t timeout_tick = vsf_systimer_ms_to_tick(DMA_TEST_TIMEOUT_MS);
    uint32_t transferred_count = 0;
    bool status_checked = false;

    while (!ctx->transfer_complete && !ctx->transfer_error) {
        vsf_systimer_tick_t now = vsf_systimer_get_tick();
        if (now - start_tick > timeout_tick) {
            __DMA_TRACE_ERROR("DMA transfer timeout" VSF_TRACE_CFG_LINEEND);
            __dma_cleanup(ctx);
            return false;
        }

        // Check channel status
        vsf_dma_channel_status_t status = vsf_dma_channel_status(ctx->dma, ctx->channel);
        transferred_count = vsf_dma_channel_get_transferred_count(ctx->dma, ctx->channel);

        if (!status_checked) {
            __DMA_TRACE_INFO("DMA transfer started, is_busy=%d, transferred_count=%d" VSF_TRACE_CFG_LINEEND,
                           status.is_busy, transferred_count);
            status_checked = true;
        }

        if (!status.is_busy) {
            // Transfer completed
            transferred_count = vsf_dma_channel_get_transferred_count(ctx->dma, ctx->channel);
            __DMA_TRACE_INFO("DMA transfer completed, transferred_count=%d" VSF_TRACE_CFG_LINEEND, transferred_count);
            ctx->transfer_complete = true;
            break;
        }

        vsf_thread_delay_ms(DMA_TEST_POLL_INTERVAL_MS);
    }

    if (ctx->transfer_error) {
        __DMA_TRACE_ERROR("DMA transfer error occurred" VSF_TRACE_CFG_LINEEND);
        __dma_cleanup(ctx);
        return false;
    }

    // Get final transferred count
    transferred_count = vsf_dma_channel_get_transferred_count(ctx->dma, ctx->channel);
    vsf_dma_channel_status_t final_status = vsf_dma_channel_status(ctx->dma, ctx->channel);
    __DMA_TRACE_INFO("DMA transfer finished, is_busy=%d, transferred_count=%d, expected=%d" VSF_TRACE_CFG_LINEEND,
                    final_status.is_busy, transferred_count, DMA_TEST_BUFFER_SIZE);

    if (transferred_count != 0) {
        __DMA_TRACE_WARNING("DMA transfer may not be complete, transferred_count=%d, expected=%d" VSF_TRACE_CFG_LINEEND,
                           transferred_count, DMA_TEST_BUFFER_SIZE);
    }

    // Verify transferred data
    bool data_match = true;
    for (uint32_t i = 0; i < DMA_TEST_BUFFER_SIZE; i++) {
        if (ctx->dst_buffer[i] != ctx->src_buffer[i]) {
            __DMA_TRACE_ERROR("Data mismatch at index %d: expected 0x%02X, got 0x%02X" VSF_TRACE_CFG_LINEEND,
                            i, ctx->src_buffer[i], ctx->dst_buffer[i]);
            data_match = false;
            break;
        }
    }

    if (!data_match) {
        __DMA_TRACE_ERROR("DMA transfer data verification failed" VSF_TRACE_CFG_LINEEND);
        __dma_cleanup(ctx);
        return false;
    }

    // Get final transferred count for success message
    transferred_count = vsf_dma_channel_get_transferred_count(ctx->dma, ctx->channel);
    __DMA_TRACE_INFO("DMA transfer completed successfully, transferred count: %d" VSF_TRACE_CFG_LINEEND, transferred_count);

    __dma_cleanup(ctx);
    __DMA_TRACE_INFO("DMA memory-to-memory test passed" VSF_TRACE_CFG_LINEEND);
    return true;
}

#if APP_USE_LINUX_DEMO == ENABLED
int dma_main(int argc, char *argv[])
{
    dma_test_t ctx = {
        .dma = (vsf_dma_t *)&APP_DMA_DEMO_CFG_DEFAULT_INSTANCE,
        .channel = -1,
    };

    if (argc < 2) {
        __DMA_TRACE_INFO("Usage: dma-test [mem2mem]" VSF_TRACE_CFG_LINEEND);
        __DMA_TRACE_INFO("  mem2mem: Test DMA memory-to-memory transfer" VSF_TRACE_CFG_LINEEND);
        return -1;
    }

    if (strcmp(argv[1], "mem2mem") == 0) {
        return __dma_test_memory_to_memory(&ctx) ? 0 : -1;
    } else {
        __DMA_TRACE_ERROR("Unknown test: %s" VSF_TRACE_CFG_LINEEND, argv[1]);
        return -1;
    }

    return 0;
}
#else
int VSF_USER_ENTRY(void)
{
    dma_test_t ctx = {
        .dma = (vsf_dma_t *)&APP_DMA_DEMO_CFG_DEFAULT_INSTANCE,
        .channel = -1,
    };

    return __dma_test_memory_to_memory(&ctx) ? 0 : -1;
}
#endif
#endif

