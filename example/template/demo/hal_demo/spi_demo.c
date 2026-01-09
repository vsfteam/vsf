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

#if APP_USE_LINUX_DEMO == ENABLED && APP_USE_HAL_DEMO == ENABLED && APP_USE_HAL_SPI_DEMO == ENABLED && VSF_HAL_USE_SPI == ENABLED

#   include <string.h>

/*============================ INCLUDES FOR DMA TEST =========================*/
// New AT32F402_405 SPI DMA test code from vendor example
#   if VSF_HAL_USE_SPI == ENABLED && defined(__HAL_DRIVER_ATTERY_AT32F402_405_H__)
#       include "hal/driver/Artery/AT32F402_405/common/device.h"
#       include "hal/driver/Artery/AT32F402_405/common/vendor/libraries/drivers/inc/at32f402_405_dma.h"
#       include "hal/driver/Artery/AT32F402_405/common/vendor/libraries/drivers/inc/at32f402_405_spi.h"
#       include "hal/driver/Artery/AT32F402_405/common/vendor/libraries/drivers/inc/at32f402_405_gpio.h"
#       include "hal/driver/Artery/AT32F402_405/common/vendor/libraries/drivers/inc/at32f402_405_crm.h"
#   endif

/*============================ MACROS ========================================*/

#ifndef APP_SPI_DEMO_CFG_DEFAULT_INSTANCE
#   define APP_SPI_DEMO_CFG_DEFAULT_INSTANCE        vsf_hw_spi0
#endif

#ifndef APP_SPI_DEMO_CFG_IRQ_PRIO
#   define APP_SPI_DEMO_CFG_IRQ_PRIO                vsf_arch_prio_2
#endif

#ifndef APP_SPI_DEMO_CFG_CLOCK_HZ
#   define APP_SPI_DEMO_CFG_CLOCK_HZ                1000000
#endif

#ifndef APP_SPI_DEMO_CFG_CS_INDEX
#   define APP_SPI_DEMO_CFG_CS_INDEX                0
#endif

#ifndef APP_SPI_DEMO_CFG_CS_MODE
#   define APP_SPI_DEMO_CFG_CS_MODE                 VSF_SPI_CS_SOFTWARE_MODE
#endif

#define SPI_TEST_BUFFER_SIZE                        16
#define SPI_TEST_TIMEOUT_MS                         1000

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct spi_test_t {
    vsf_spi_t *spi;
    vsf_spi_capability_t cap;
    vsf_spi_cfg_t cfg;
    volatile bool transfer_complete;
    volatile bool transfer_error;
    volatile bool fifo_ready;
    volatile bool is_fifo_irq_mode;
    volatile uint_fast32_t tx_offset;
    volatile uint_fast32_t rx_offset;
    uint_fast32_t fifo_count;
    uint32_t cs_mode;  // CS mode: VSF_SPI_CS_SOFTWARE_MODE or VSF_SPI_CS_HARDWARE_MODE
    uint8_t tx_buffer[SPI_TEST_BUFFER_SIZE];
    uint8_t rx_buffer[SPI_TEST_BUFFER_SIZE];
} spi_test_t;

/*============================ PROTOTYPES ====================================*/

static void __spi_irq_handler(void *target_ptr, vsf_spi_t *spi_ptr, vsf_spi_irq_mask_t irq_mask);
static bool __spi_setup(spi_test_t *ctx);
static void __spi_cleanup(spi_test_t *ctx);
static bool __spi_test_fifo_poll_mode(spi_test_t *ctx);
static bool __spi_test_fifo_irq_mode(spi_test_t *ctx);
static bool __spi_test_request_mode(spi_test_t *ctx);

/*============================ IMPLEMENTATION ================================*/

// Shared setup and cleanup functions (used by both original and vendor implementations)
static bool __spi_setup(spi_test_t *ctx)
{
    if ((ctx == NULL) || (ctx->spi == NULL)) {
        vsf_trace_error("SPI test context or device is NULL!" VSF_TRACE_CFG_LINEEND);
        return false;
    }

    // Clear configuration structure to ensure all fields are initialized
    memset(&ctx->cfg, 0, sizeof(ctx->cfg));

    // Get SPI capability
    ctx->cap = vsf_spi_capability(ctx->spi);
    vsf_trace_info("SPI Capability: max_clock_hz=%d, min_clock_hz=%d, cs_count=%d" VSF_TRACE_CFG_LINEEND,
                    ctx->cap.max_clock_hz, ctx->cap.min_clock_hz, ctx->cap.cs_count);

    // Configure SPI
    ctx->cfg.mode = ((VSF_SPI_MASTER | VSF_SPI_MODE_0) | VSF_SPI_MSB_FIRST) |
                    (VSF_SPI_DATASIZE_8 | ctx->cs_mode);
    ctx->cfg.clock_hz = APP_SPI_DEMO_CFG_CLOCK_HZ;
    ctx->cfg.isr.handler_fn = __spi_irq_handler;
    ctx->cfg.isr.target_ptr = ctx;
    ctx->cfg.isr.prio = APP_SPI_DEMO_CFG_IRQ_PRIO;
    ctx->cfg.auto_cs_index = APP_SPI_DEMO_CFG_CS_INDEX;

    // Log CS mode configuration
    if ((ctx->cfg.mode & VSF_SPI_CS_MODE_MASK) == VSF_SPI_CS_SOFTWARE_MODE) {
        vsf_trace_info("SPI CS Mode: SOFTWARE (manual) - will call cs_active/cs_inactive manually" VSF_TRACE_CFG_LINEEND);
    } else {
        vsf_trace_info("SPI CS Mode: HARDWARE (auto) - CS controlled by hardware automatically" VSF_TRACE_CFG_LINEEND);
    }

    // Initialize SPI
    vsf_trace_info("SPI setup: initializing SPI" VSF_TRACE_CFG_LINEEND);
    if (vsf_spi_init(ctx->spi, &ctx->cfg) != VSF_ERR_NONE) {
        vsf_trace_error("SPI init failed" VSF_TRACE_CFG_LINEEND);
        return false;
    }
    vsf_trace_info("SPI setup: SPI initialized" VSF_TRACE_CFG_LINEEND);

    // Enable SPI (loop until completion)
    vsf_trace_info("SPI setup: enabling SPI" VSF_TRACE_CFG_LINEEND);
    uint32_t enable_loop_count = 0;
    while (fsm_rt_on_going == vsf_spi_enable(ctx->spi)) {
        enable_loop_count++;
        if (enable_loop_count > 1000) {
            vsf_trace_warning("SPI setup: enable timeout after %d loops" VSF_TRACE_CFG_LINEEND, enable_loop_count);
            break;
        }
        // Wait for enable to complete
    }
    if (enable_loop_count > 0) {
        vsf_trace_info("SPI setup: enabled after %d loops" VSF_TRACE_CFG_LINEEND, enable_loop_count);
    }

    ctx->transfer_complete = false;
    ctx->transfer_error = false;
    ctx->fifo_ready = false;
    ctx->is_fifo_irq_mode = false;
    ctx->tx_offset = 0;
    ctx->rx_offset = 0;
    ctx->fifo_count = 0;

    vsf_trace_info("SPI setup: all state variables initialized" VSF_TRACE_CFG_LINEEND);
    return true;
}

static void __spi_cleanup(spi_test_t *ctx)
{
    if ((ctx == NULL) || (ctx->spi == NULL)) {
        vsf_trace_warning("SPI cleanup: context or device is NULL" VSF_TRACE_CFG_LINEEND);
        return;
    }

    vsf_trace_info("SPI cleanup: starting cleanup process" VSF_TRACE_CFG_LINEEND);

    // Cancel any ongoing transfer
    vsf_err_t err = vsf_spi_cancel_transfer(ctx->spi);
    if (err != VSF_ERR_NONE) {
        vsf_trace_warning("SPI cancel transfer failed: %d" VSF_TRACE_CFG_LINEEND, err);
    } else {
        vsf_trace_info("SPI cleanup: transfer cancelled" VSF_TRACE_CFG_LINEEND);
    }

    // Disable SPI (loop until completion)
    vsf_trace_info("SPI cleanup: disabling SPI" VSF_TRACE_CFG_LINEEND);
    uint32_t disable_loop_count = 0;
    while (fsm_rt_on_going == vsf_spi_disable(ctx->spi)) {
        disable_loop_count++;
        if (disable_loop_count > 1000) {
            vsf_trace_warning("SPI cleanup: disable timeout after %d loops" VSF_TRACE_CFG_LINEEND, disable_loop_count);
            break;
        }
        // Wait for disable to complete
    }
    if (disable_loop_count > 0) {
        vsf_trace_info("SPI cleanup: disabled after %d loops" VSF_TRACE_CFG_LINEEND, disable_loop_count);
    }

    // Finalize SPI
    vsf_trace_info("SPI cleanup: finalizing SPI" VSF_TRACE_CFG_LINEEND);
    vsf_spi_fini(ctx->spi);
    vsf_trace_info("SPI cleanup: cleanup completed" VSF_TRACE_CFG_LINEEND);
}

static void __spi_irq_handler(void *target_ptr, vsf_spi_t *spi_ptr, vsf_spi_irq_mask_t irq_mask)
{
    spi_test_t *ctx = (spi_test_t *)target_ptr;

    if (ctx != NULL) {
        if ((irq_mask & VSF_SPI_IRQ_MASK_CPL) != 0) {
            ctx->transfer_complete = true;
        }
        if ((irq_mask & VSF_SPI_IRQ_MASK_ERR) != 0) {
            ctx->transfer_error = true;
        }
        if (ctx->is_fifo_irq_mode) {
            // Handle FIFO threshold interrupts in interrupt handler
            if (((irq_mask & VSF_SPI_IRQ_MASK_TX) != 0) || ((irq_mask & VSF_SPI_IRQ_MASK_RX) != 0)) {
                // Continue FIFO transfer in interrupt handler
                vsf_spi_fifo_transfer(ctx->spi, ctx->tx_buffer, (uint_fast32_t *)&ctx->tx_offset,
                                     ctx->rx_buffer, (uint_fast32_t *)&ctx->rx_offset, ctx->fifo_count);
                // Check if transfer is complete
                if ((ctx->tx_offset >= ctx->fifo_count) && (ctx->rx_offset >= ctx->fifo_count)) {
                    ctx->transfer_complete = true;
                    // Disable interrupts immediately to prevent further triggering
                    vsf_spi_irq_disable(ctx->spi, (VSF_SPI_IRQ_MASK_TX | VSF_SPI_IRQ_MASK_RX));
                }
            }
        } else {
            // For request mode, just set flag
            if (((irq_mask & VSF_SPI_IRQ_MASK_TX) != 0) || ((irq_mask & VSF_SPI_IRQ_MASK_RX) != 0)) {
                ctx->fifo_ready = true;
            }
        }
    }
}

static bool __spi_test_request_mode(spi_test_t *ctx)
{
    vsf_trace_info("=== SPI Request Mode Test ===" VSF_TRACE_CFG_LINEEND);

    // Reset all state variables before setup
    ctx->transfer_complete = false;
    ctx->transfer_error = false;
    ctx->fifo_ready = false;
    ctx->is_fifo_irq_mode = false;
    ctx->tx_offset = 0;
    ctx->rx_offset = 0;
    ctx->fifo_count = 0;

    if (!(__spi_setup(ctx))) {
        return false;
    }

    // Initialize test data
    for (uint32_t i = 0; i < SPI_TEST_BUFFER_SIZE; i++) {
        ctx->tx_buffer[i] = (uint8_t)((0x55 + i) & 0xFF);
        ctx->rx_buffer[i] = 0;
    }

    vsf_trace_info("Starting request transfer: %d bytes" VSF_TRACE_CFG_LINEEND, SPI_TEST_BUFFER_SIZE);

    // Activate CS (only for software CS mode)
    if ((ctx->cfg.mode & VSF_SPI_CS_MODE_MASK) == VSF_SPI_CS_SOFTWARE_MODE) {
        vsf_trace_info("SPI: Manual CS mode - calling cs_active" VSF_TRACE_CFG_LINEEND);
        if (vsf_spi_cs_active(ctx->spi, APP_SPI_DEMO_CFG_CS_INDEX) != VSF_ERR_NONE) {
            vsf_trace_error("SPI CS active failed" VSF_TRACE_CFG_LINEEND);
            __spi_cleanup(ctx);
            return false;
        }
    } else {
        vsf_trace_info("SPI: Hardware CS mode - CS will be controlled automatically" VSF_TRACE_CFG_LINEEND);
    }

    // Enable completion and error interrupts for request mode
    vsf_spi_irq_enable(ctx->spi, (VSF_SPI_IRQ_MASK_CPL | VSF_SPI_IRQ_MASK_ERR));

    // Request transfer
    ctx->transfer_complete = false;
    ctx->transfer_error = false;
    if (vsf_spi_request_transfer(ctx->spi, ctx->tx_buffer, ctx->rx_buffer, SPI_TEST_BUFFER_SIZE) != VSF_ERR_NONE) {
        vsf_trace_error("SPI request transfer failed" VSF_TRACE_CFG_LINEEND);
        // Deactivate CS (only for software CS mode)
        if ((ctx->cfg.mode & VSF_SPI_CS_MODE_MASK) == VSF_SPI_CS_SOFTWARE_MODE) {
            vsf_err_t err = vsf_spi_cs_inactive(ctx->spi, APP_SPI_DEMO_CFG_CS_INDEX);
            if (err != VSF_ERR_NONE) {
                vsf_trace_warning("SPI CS inactive failed: %d" VSF_TRACE_CFG_LINEEND, err);
            }
        }
        __spi_cleanup(ctx);
        return false;
    }

    // Wait for transfer to complete
    vsf_systimer_tick_t start_tick = vsf_systimer_get_tick();
    vsf_systimer_tick_t timeout_tick = vsf_systimer_ms_to_tick(SPI_TEST_TIMEOUT_MS);

    while ((!ctx->transfer_complete) && (!ctx->transfer_error)) {
        vsf_systimer_tick_t now = vsf_systimer_get_tick();
        if ((now - start_tick) > timeout_tick) {
            vsf_trace_error("SPI request transfer timeout" VSF_TRACE_CFG_LINEEND);
            vsf_err_t err = vsf_spi_cancel_transfer(ctx->spi);
            if (err != VSF_ERR_NONE) {
                vsf_trace_warning("SPI cancel transfer failed: %d" VSF_TRACE_CFG_LINEEND, err);
            }
            // Deactivate CS (only for software CS mode)
            if ((ctx->cfg.mode & VSF_SPI_CS_MODE_MASK) == VSF_SPI_CS_SOFTWARE_MODE) {
                vsf_trace_info("SPI: Manual CS mode - calling cs_inactive (timeout)" VSF_TRACE_CFG_LINEEND);
                err = vsf_spi_cs_inactive(ctx->spi, APP_SPI_DEMO_CFG_CS_INDEX);
                if (err != VSF_ERR_NONE) {
                    vsf_trace_warning("SPI CS inactive failed: %d" VSF_TRACE_CFG_LINEEND, err);
                }
            }
            __spi_cleanup(ctx);
            return false;
        }

        // Check status
        vsf_spi_status_t status = vsf_spi_status(ctx->spi);
        if ((!status.is_busy) && (ctx->transfer_complete)) {
            break;
        }

        vsf_thread_delay_ms(1);
    }

    if (ctx->transfer_error) {
        vsf_trace_error("SPI request transfer error occurred" VSF_TRACE_CFG_LINEEND);
        // Deactivate CS (only for software CS mode)
        if ((ctx->cfg.mode & VSF_SPI_CS_MODE_MASK) == VSF_SPI_CS_SOFTWARE_MODE) {
            vsf_trace_info("SPI: Manual CS mode - calling cs_inactive (error)" VSF_TRACE_CFG_LINEEND);
            vsf_err_t err = vsf_spi_cs_inactive(ctx->spi, APP_SPI_DEMO_CFG_CS_INDEX);
            if (err != VSF_ERR_NONE) {
                vsf_trace_warning("SPI CS inactive failed: %d" VSF_TRACE_CFG_LINEEND, err);
            }
        }
        __spi_cleanup(ctx);
        return false;
    }

    // Get transferred count
    uint_fast32_t sent_count = 0;
    uint_fast32_t received_count = 0;
    vsf_spi_get_transferred_count(ctx->spi, &sent_count, &received_count);

    // Deactivate CS (only for software CS mode)
    if ((ctx->cfg.mode & VSF_SPI_CS_MODE_MASK) == VSF_SPI_CS_SOFTWARE_MODE) {
        vsf_trace_info("SPI: Manual CS mode - calling cs_inactive" VSF_TRACE_CFG_LINEEND);
        vsf_err_t err = vsf_spi_cs_inactive(ctx->spi, APP_SPI_DEMO_CFG_CS_INDEX);
        if (err != VSF_ERR_NONE) {
            vsf_trace_warning("SPI CS inactive failed: %d" VSF_TRACE_CFG_LINEEND, err);
        }
    } else {
        vsf_trace_info("SPI: Hardware CS mode - CS will be deactivated automatically" VSF_TRACE_CFG_LINEEND);
    }

    vsf_trace_info("Request transfer completed: sent=%d, received=%d" VSF_TRACE_CFG_LINEEND,
                    sent_count, received_count);

    // Print received data
    if (received_count > 0) {
        vsf_trace_info("Received data (%d bytes):" VSF_TRACE_CFG_LINEEND, received_count);
        vsf_trace_buffer(VSF_TRACE_INFO, ctx->rx_buffer, received_count);
    }

    __spi_cleanup(ctx);
    vsf_trace_info("SPI request mode test passed" VSF_TRACE_CFG_LINEEND);
    return true;
}

static bool __spi_test_fifo_poll_mode(spi_test_t *ctx)
{
    vsf_trace_info("=== SPI FIFO Poll Mode Test ===" VSF_TRACE_CFG_LINEEND);

    // Reset all state variables before setup
    ctx->transfer_complete = false;
    ctx->transfer_error = false;
    ctx->fifo_ready = false;
    ctx->is_fifo_irq_mode = false;
    ctx->tx_offset = 0;
    ctx->rx_offset = 0;
    ctx->fifo_count = 0;

    if (!(__spi_setup(ctx))) {
        return false;
    }

    // Poll mode does not use interrupts, so no need to enable any

    // Initialize test data
    for (uint32_t i = 0; i < SPI_TEST_BUFFER_SIZE; i++) {
        ctx->tx_buffer[i] = (uint8_t)((0xAA + i) & 0xFF);
        ctx->rx_buffer[i] = 0;
    }

    vsf_trace_info("Starting FIFO transfer: %d bytes" VSF_TRACE_CFG_LINEEND, SPI_TEST_BUFFER_SIZE);

    // Activate CS based on CS mode
    // In software CS mode: must call cs_active before fifo_transfer
    // In hardware CS mode: fifo_transfer will automatically call cs_active
    if ((ctx->cfg.mode & VSF_SPI_CS_MODE_MASK) == VSF_SPI_CS_SOFTWARE_MODE) {
        // Software CS mode: use cs_active to enable SPI and activate CS
        vsf_trace_info("SPI: Manual CS mode - calling cs_active (enables SPI and activates CS)" VSF_TRACE_CFG_LINEEND);
        if (vsf_spi_cs_active(ctx->spi, APP_SPI_DEMO_CFG_CS_INDEX) != VSF_ERR_NONE) {
            vsf_trace_error("SPI cs_active failed" VSF_TRACE_CFG_LINEEND);
            __spi_cleanup(ctx);
            return false;
        }
    } else {
        // Hardware CS mode: fifo_transfer will automatically call cs_active when needed
        vsf_trace_info("SPI: Hardware CS mode - fifo_transfer will auto-manage CS" VSF_TRACE_CFG_LINEEND);
    }

    // Perform FIFO transfer (non-blocking, must call repeatedly until complete)
    uint_fast32_t tx_offset = 0;
    uint_fast32_t rx_offset = 0;
    vsf_systimer_tick_t start_tick = vsf_systimer_get_tick();
    vsf_systimer_tick_t timeout_tick = vsf_systimer_ms_to_tick(SPI_TEST_TIMEOUT_MS);

    uint32_t loop_count = 0;
    while ((tx_offset < SPI_TEST_BUFFER_SIZE) || (rx_offset < SPI_TEST_BUFFER_SIZE)) {
        // Call fifo_transfer repeatedly until all data is transferred
        vsf_spi_fifo_transfer(ctx->spi, ctx->tx_buffer, &tx_offset, ctx->rx_buffer, &rx_offset, SPI_TEST_BUFFER_SIZE);

        loop_count++;
        if ((loop_count % 100) == 0) {
            vsf_spi_status_t status = vsf_spi_status(ctx->spi);
            vsf_trace_info("SPI FIFO loop %d: tx_offset=%d, rx_offset=%d, is_busy=%d" VSF_TRACE_CFG_LINEEND,
                            loop_count, tx_offset, rx_offset, status.is_busy);
        }

        // Check timeout
        vsf_systimer_tick_t now = vsf_systimer_get_tick();
        if ((now - start_tick) > timeout_tick) {
            vsf_spi_status_t status = vsf_spi_status(ctx->spi);
            vsf_trace_error("SPI FIFO transfer timeout: tx_offset=%d, rx_offset=%d, is_busy=%d, loop_count=%d" VSF_TRACE_CFG_LINEEND,
                            tx_offset, rx_offset, status.is_busy, loop_count);
            // Deactivate CS (only for software CS mode)
            if ((ctx->cfg.mode & VSF_SPI_CS_MODE_MASK) == VSF_SPI_CS_SOFTWARE_MODE) {
                vsf_trace_info("SPI: Manual CS mode - calling cs_inactive (timeout)" VSF_TRACE_CFG_LINEEND);
                vsf_err_t err = vsf_spi_cs_inactive(ctx->spi, APP_SPI_DEMO_CFG_CS_INDEX);
                if (err != VSF_ERR_NONE) {
                    vsf_trace_warning("SPI CS inactive failed: %d" VSF_TRACE_CFG_LINEEND, err);
                }
            }
            __spi_cleanup(ctx);
            return false;
        }

        // Check if transfer is complete
        vsf_spi_status_t status = vsf_spi_status(ctx->spi);
        if ((!status.is_busy) && (tx_offset >= SPI_TEST_BUFFER_SIZE) && (rx_offset >= SPI_TEST_BUFFER_SIZE)) {
            break;
        }

        // Small delay to avoid busy loop
        vsf_thread_delay_ms(1);
    }

    // Deactivate CS (only for software CS mode)
    if ((ctx->cfg.mode & VSF_SPI_CS_MODE_MASK) == VSF_SPI_CS_SOFTWARE_MODE) {
        vsf_trace_info("SPI: Manual CS mode - calling cs_inactive" VSF_TRACE_CFG_LINEEND);
        vsf_err_t err = vsf_spi_cs_inactive(ctx->spi, APP_SPI_DEMO_CFG_CS_INDEX);
        if (err != VSF_ERR_NONE) {
            vsf_trace_warning("SPI CS inactive failed: %d" VSF_TRACE_CFG_LINEEND, err);
        }
    } else {
        vsf_trace_info("SPI: Hardware CS mode - CS will be deactivated automatically" VSF_TRACE_CFG_LINEEND);
    }

    vsf_trace_info("FIFO transfer completed: tx_offset=%d, rx_offset=%d" VSF_TRACE_CFG_LINEEND,
                    tx_offset, rx_offset);

    // Print received data
    if (rx_offset > 0) {
        vsf_trace_info("Received data (%d bytes):" VSF_TRACE_CFG_LINEEND, rx_offset);
        vsf_trace_buffer(VSF_TRACE_INFO, ctx->rx_buffer, rx_offset);
    }

    __spi_cleanup(ctx);
    vsf_trace_info("SPI FIFO poll mode test passed" VSF_TRACE_CFG_LINEEND);
    return true;
}

static bool __spi_test_fifo_irq_mode(spi_test_t *ctx)
{
    vsf_trace_info("=== SPI FIFO IRQ Mode Test ===" VSF_TRACE_CFG_LINEEND);

    // Reset all state variables before setup
    ctx->transfer_complete = false;
    ctx->transfer_error = false;
    ctx->fifo_ready = false;
    ctx->is_fifo_irq_mode = false;
    ctx->tx_offset = 0;
    ctx->rx_offset = 0;
    ctx->fifo_count = 0;

    if (!(__spi_setup(ctx))) {
        return false;
    }

    // Initialize test data
    for (uint32_t i = 0; i < SPI_TEST_BUFFER_SIZE; i++) {
        ctx->tx_buffer[i] = (uint8_t)((0xCC + i) & 0xFF);
        ctx->rx_buffer[i] = 0;
    }

    vsf_trace_info("Starting FIFO IRQ transfer: %d bytes" VSF_TRACE_CFG_LINEEND, SPI_TEST_BUFFER_SIZE);

    // Activate CS (only for software CS mode)
    if ((ctx->cfg.mode & VSF_SPI_CS_MODE_MASK) == VSF_SPI_CS_SOFTWARE_MODE) {
        vsf_trace_info("SPI: Manual CS mode - calling cs_active" VSF_TRACE_CFG_LINEEND);
        if (vsf_spi_cs_active(ctx->spi, APP_SPI_DEMO_CFG_CS_INDEX) != VSF_ERR_NONE) {
            vsf_trace_error("SPI CS active failed" VSF_TRACE_CFG_LINEEND);
            __spi_cleanup(ctx);
            return false;
        }
    } else {
        vsf_trace_info("SPI: Hardware CS mode - CS will be controlled automatically" VSF_TRACE_CFG_LINEEND);
    }

    // Setup FIFO IRQ mode context
    ctx->is_fifo_irq_mode = true;
    ctx->tx_offset = 0;
    ctx->rx_offset = 0;
    ctx->fifo_count = SPI_TEST_BUFFER_SIZE;
    ctx->transfer_complete = false;

    // Enable FIFO threshold interrupts
    vsf_spi_irq_enable(ctx->spi, (VSF_SPI_IRQ_MASK_TX | VSF_SPI_IRQ_MASK_RX));

    // Initial transfer to start the process (subsequent transfers will be in interrupt handler)
    vsf_spi_fifo_transfer(ctx->spi, ctx->tx_buffer, (uint_fast32_t *)&ctx->tx_offset,
                         ctx->rx_buffer, (uint_fast32_t *)&ctx->rx_offset, ctx->fifo_count);

    // Wait for transfer to complete (handled in interrupt handler)
    vsf_systimer_tick_t start_tick = vsf_systimer_get_tick();
    vsf_systimer_tick_t timeout_tick = vsf_systimer_ms_to_tick(SPI_TEST_TIMEOUT_MS);

    while (!ctx->transfer_complete && !ctx->transfer_error) {
        vsf_systimer_tick_t now = vsf_systimer_get_tick();
        if ((now - start_tick) > timeout_tick) {
            vsf_trace_error("SPI FIFO IRQ transfer timeout: tx_offset=%d, rx_offset=%d" VSF_TRACE_CFG_LINEEND,
                            ctx->tx_offset, ctx->rx_offset);
            // Disable FIFO threshold interrupts
            vsf_spi_irq_disable(ctx->spi, (VSF_SPI_IRQ_MASK_TX | VSF_SPI_IRQ_MASK_RX));
            // Deactivate CS (only for software CS mode)
            if ((ctx->cfg.mode & VSF_SPI_CS_MODE_MASK) == VSF_SPI_CS_SOFTWARE_MODE) {
                vsf_trace_info("SPI: Manual CS mode - calling cs_inactive (timeout)" VSF_TRACE_CFG_LINEEND);
                vsf_err_t err = vsf_spi_cs_inactive(ctx->spi, APP_SPI_DEMO_CFG_CS_INDEX);
                if (err != VSF_ERR_NONE) {
                    vsf_trace_warning("SPI CS inactive failed: %d" VSF_TRACE_CFG_LINEEND, err);
                }
            }
            ctx->is_fifo_irq_mode = false;
            __spi_cleanup(ctx);
            return false;
        }

        vsf_thread_delay_ms(1);
    }

    if (ctx->transfer_error) {
        vsf_trace_error("SPI FIFO IRQ transfer error occurred" VSF_TRACE_CFG_LINEEND);
        // Disable FIFO threshold interrupts
        vsf_spi_irq_disable(ctx->spi, (VSF_SPI_IRQ_MASK_TX | VSF_SPI_IRQ_MASK_RX));
        // Deactivate CS (only for software CS mode)
        if ((ctx->cfg.mode & VSF_SPI_CS_MODE_MASK) == VSF_SPI_CS_SOFTWARE_MODE) {
            vsf_trace_info("SPI: Manual CS mode - calling cs_inactive (error)" VSF_TRACE_CFG_LINEEND);
            vsf_err_t err = vsf_spi_cs_inactive(ctx->spi, APP_SPI_DEMO_CFG_CS_INDEX);
            if (err != VSF_ERR_NONE) {
                vsf_trace_warning("SPI CS inactive failed: %d" VSF_TRACE_CFG_LINEEND, err);
            }
        }
        ctx->is_fifo_irq_mode = false;
        __spi_cleanup(ctx);
        return false;
    }

    // Disable FIFO threshold interrupts
    vsf_spi_irq_disable(ctx->spi, (VSF_SPI_IRQ_MASK_TX | VSF_SPI_IRQ_MASK_RX));

    // Deactivate CS (only for software CS mode)
    if ((ctx->cfg.mode & VSF_SPI_CS_MODE_MASK) == VSF_SPI_CS_SOFTWARE_MODE) {
        vsf_trace_info("SPI: Manual CS mode - calling cs_inactive" VSF_TRACE_CFG_LINEEND);
        vsf_err_t err = vsf_spi_cs_inactive(ctx->spi, APP_SPI_DEMO_CFG_CS_INDEX);
        if (err != VSF_ERR_NONE) {
            vsf_trace_warning("SPI CS inactive failed: %d" VSF_TRACE_CFG_LINEEND, err);
        }
    } else {
        vsf_trace_info("SPI: Hardware CS mode - CS will be deactivated automatically" VSF_TRACE_CFG_LINEEND);
    }

    vsf_trace_info("FIFO IRQ transfer completed: tx_offset=%d, rx_offset=%d" VSF_TRACE_CFG_LINEEND,
                    ctx->tx_offset, ctx->rx_offset);

    // Print received data
    if (ctx->rx_offset > 0) {
        vsf_trace_info("Received data (%d bytes):" VSF_TRACE_CFG_LINEEND, ctx->rx_offset);
        vsf_trace_buffer(VSF_TRACE_INFO, ctx->rx_buffer, ctx->rx_offset);
    }

    ctx->is_fifo_irq_mode = false;
    __spi_cleanup(ctx);
    vsf_trace_info("SPI FIFO IRQ mode test passed" VSF_TRACE_CFG_LINEEND);
    return true;
}

int spi_main(int argc, char *argv[])
{
    spi_test_t ctx;

    // Initialize all fields to zero first
    memset(&ctx, 0, sizeof(ctx));

    // Set default values
    ctx.spi = (vsf_spi_t *)&APP_SPI_DEMO_CFG_DEFAULT_INSTANCE;
    ctx.cs_mode = VSF_SPI_CS_HARDWARE_MODE;  // Default to hardware CS mode

    if (argc < 2) {
        vsf_trace_info("Usage: spi-test [fifo-poll|fifo-irq|request] [--cs-mode=software|hardware]" VSF_TRACE_CFG_LINEEND);
        vsf_trace_info("  fifo-poll: Test SPI FIFO poll mode transfer" VSF_TRACE_CFG_LINEEND);
        vsf_trace_info("  fifo-irq: Test SPI FIFO IRQ mode transfer" VSF_TRACE_CFG_LINEEND);
        vsf_trace_info("  request: Test SPI request mode transfer" VSF_TRACE_CFG_LINEEND);
        vsf_trace_info("" VSF_TRACE_CFG_LINEEND);
        vsf_trace_info("CS Mode Options:" VSF_TRACE_CFG_LINEEND);
        vsf_trace_info("  --cs-mode=software: Software CS (manual) - call cs_active/cs_inactive manually" VSF_TRACE_CFG_LINEEND);
        vsf_trace_info("  --cs-mode=hardware: Hardware CS (auto) - CS controlled by hardware automatically" VSF_TRACE_CFG_LINEEND);
        vsf_trace_info("  Default: --cs-mode=hardware" VSF_TRACE_CFG_LINEEND);
        return -1;
    }

    // Parse command line arguments
    for (int i = 2; i < argc; i++) {
        if (strncmp(argv[i], "--cs-mode=", 10) == 0) {
            const char *mode_str = argv[i] + 10;
            if (strcmp(mode_str, "software") == 0) {
                ctx.cs_mode = VSF_SPI_CS_SOFTWARE_MODE;
            } else if (strcmp(mode_str, "hardware") == 0) {
                ctx.cs_mode = VSF_SPI_CS_HARDWARE_MODE;
            } else {
                vsf_trace_error("Invalid CS mode: %s (use 'software' or 'hardware')" VSF_TRACE_CFG_LINEEND, mode_str);
                return -1;
            }
        } else {
            vsf_trace_error("Unknown option: %s" VSF_TRACE_CFG_LINEEND, argv[i]);
            return -1;
        }
    }

    bool result = false;
    if (strcmp(argv[1], "fifo-poll") == 0) {
        result = __spi_test_fifo_poll_mode(&ctx);
    } else if (strcmp(argv[1], "fifo-irq") == 0) {
        result = __spi_test_fifo_irq_mode(&ctx);
    } else if (strcmp(argv[1], "request") == 0) {
        result = __spi_test_request_mode(&ctx);
    } else {
        vsf_trace_error("Unknown test: %s" VSF_TRACE_CFG_LINEEND, argv[1]);
        return -1;
    }

    return (result ? 0 : -1);
}

#endif  /* APP_USE_LINUX_DEMO == ENABLED && APP_USE_HAL_DEMO == ENABLED && APP_USE_HAL_SPI_DEMO == ENABLED && VSF_HAL_USE_SPI == ENABLED */
