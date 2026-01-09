/*****************************************************************************
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
 ****************************************************************************/

/*============================ INCLUDES ======================================*/

#include "../test_spi.h"
#include "test_spi_fifo.h"

/*============================ MACROS ========================================*/

#define SPI_POLLING_TIMEOUT_MS    1000    // 轮询超时时间（毫秒）

/*============================ TYPES =========================================*/

// 中断测试上下文
typedef struct {
    volatile bool transfer_complete;
    volatile bool transfer_error;
    vsf_spi_irq_mask_t irq_mask;
} spi_test_irq_ctx_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

static void __spi_irq_handler(void *target_ptr, vsf_spi_t *spi_ptr, vsf_spi_irq_mask_t irq_mask);

/*============================ IMPLEMENTATION ================================*/

/**
 * @brief 测试 vsf_spi_fifo_transfer() 空指针参数
 */
void vsf_test_spi_fifo_transfer_null_ptr(void)
{
    uint8_t tx[4] = {0x01, 0x02, 0x03, 0x04};
    uint8_t rx[4];
    uint_fast32_t out_offset = 0, in_offset = 0;

    // 测试 spi_ptr 为 NULL
    vsf_spi_fifo_transfer(NULL, tx, &out_offset, rx, &in_offset, 4);
    // 应该能安全处理 NULL 指针（不崩溃）
}

/**
 * @brief 测试 vsf_spi_fifo_transfer() 基本功能
 * @note 此测试需要硬件支持（hw_req=spi+assist），如果硬件不可用，测试将失败
 */
void vsf_test_spi_fifo_transfer_basic(void)
{
    // 如果测试标记为 hw_req=spi+assist，应该确保硬件可用
    VSF_TEST_ASSERT(test_spi_instance != NULL);

    vsf_spi_cfg_t cfg = TEST_SPI_DEFAULT_CFG;
    uint8_t tx[4] = {0x01, 0x02, 0x03, 0x04};
    uint8_t rx[4] = {0};
    uint_fast32_t out_offset = 0, in_offset = 0;
    vsf_err_t err;
    fsm_rt_t rt;

    // 初始化并使能
    err = vsf_spi_init(test_spi_instance, &cfg);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    rt = vsf_spi_enable(test_spi_instance);
    VSF_TEST_ASSERT((rt == fsm_rt_cpl) || (rt == fsm_rt_on_going));

    // FIFO 传输
    vsf_spi_fifo_transfer(test_spi_instance, tx, &out_offset, rx, &in_offset, sizeof(tx));
    // 验证不会崩溃

    // 清理
    rt = vsf_spi_disable(test_spi_instance);
    VSF_TEST_ASSERT((rt == fsm_rt_cpl) || (rt == fsm_rt_on_going));
    vsf_spi_fini(test_spi_instance);
}

/**
 * @brief 测试 FIFO 传输偏移量
 * @note 此测试需要硬件支持（hw_req=spi+assist），如果硬件不可用，测试将失败
 */
void vsf_test_spi_fifo_transfer_offset(void)
{
    // 如果测试标记为 hw_req=spi+assist，应该确保硬件可用
    VSF_TEST_ASSERT(test_spi_instance != NULL);

    vsf_spi_cfg_t cfg = TEST_SPI_DEFAULT_CFG;
    uint8_t tx[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    uint8_t rx[8] = {0};
    uint_fast32_t out_offset = 0, in_offset = 0;
    vsf_err_t err;
    fsm_rt_t rt;

    // 初始化并使能
    err = vsf_spi_init(test_spi_instance, &cfg);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    rt = vsf_spi_enable(test_spi_instance);
    VSF_TEST_ASSERT((rt == fsm_rt_cpl) || (rt == fsm_rt_on_going));

    // 分多次 FIFO 传输
    vsf_spi_fifo_transfer(test_spi_instance, tx, &out_offset, rx, &in_offset, 4);
    vsf_spi_fifo_transfer(test_spi_instance, tx, &out_offset, rx, &in_offset, 4);

    // 清理
    rt = vsf_spi_disable(test_spi_instance);
    VSF_TEST_ASSERT((rt == fsm_rt_cpl) || (rt == fsm_rt_on_going));
    vsf_spi_fini(test_spi_instance);
}

/**
 * @brief SPI 中断处理函数（用于测试）
 */
static void __spi_irq_handler(void *target_ptr, vsf_spi_t *spi_ptr, vsf_spi_irq_mask_t irq_mask)
{
    spi_test_irq_ctx_t *ctx = (spi_test_irq_ctx_t *)target_ptr;
    if (ctx == NULL) {
        return;
    }

    ctx->irq_mask |= irq_mask;

    // 检查传输完成
    if (irq_mask & VSF_SPI_IRQ_MASK_RX_CPL) {
        ctx->transfer_complete = true;
    }

    // 检查错误
    if (irq_mask & VSF_SPI_IRQ_MASK_ERR) {
        ctx->transfer_error = true;
    }
}

/**
 * @brief 测试轮询 FIFO 收发
 * @note 此测试需要硬件支持（hw_req=spi+assist），如果硬件不可用，测试将失败
 */
void vsf_test_spi_fifo_polling_transfer(void)
{
    // 如果测试标记为 hw_req=spi+assist，应该确保硬件可用
    VSF_TEST_ASSERT(test_spi_instance != NULL);

    vsf_spi_cfg_t cfg = TEST_SPI_DEFAULT_CFG;
    uint8_t tx[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                      0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10};
    uint8_t rx[16] = {0};
    uint_fast32_t tx_offset = 0, rx_offset = 0;
    vsf_err_t err;
    fsm_rt_t rt;
    vsf_spi_status_t status;
    uint32_t timeout_count = 0;
    const uint32_t max_timeout = SPI_POLLING_TIMEOUT_MS * 1000; // 转换为循环次数（假设每次循环约1us）

    // 初始化并使能
    err = vsf_spi_init(test_spi_instance, &cfg);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    rt = vsf_spi_enable(test_spi_instance);
    VSF_TEST_ASSERT((rt == fsm_rt_cpl) || (rt == fsm_rt_on_going));

    // 轮询 FIFO 传输
    while ((tx_offset < sizeof(tx)) || (rx_offset < sizeof(rx))) {
        // 调用 FIFO 传输
        vsf_spi_fifo_transfer(test_spi_instance, tx, &tx_offset, rx, &rx_offset, sizeof(tx));

        // 检查状态
        status = vsf_spi_status(test_spi_instance);
        if (!status.is_busy && (tx_offset >= sizeof(tx)) && (rx_offset >= sizeof(rx))) {
            break; // 传输完成
        }

        // 超时检查
        timeout_count++;
        if (timeout_count > max_timeout) {
            VSF_TEST_ASSERT(false); // 超时失败
            break;
        }
    }

    // 验证传输完成
    status = vsf_spi_status(test_spi_instance);
    VSF_TEST_ASSERT(!status.is_busy);
    VSF_TEST_ASSERT(tx_offset >= sizeof(tx));
    VSF_TEST_ASSERT(rx_offset >= sizeof(rx));

    // 清理
    rt = vsf_spi_disable(test_spi_instance);
    VSF_TEST_ASSERT((rt == fsm_rt_cpl) || (rt == fsm_rt_on_going));
    vsf_spi_fini(test_spi_instance);
}

/**
 * @brief 测试中断 FIFO 收发
 * @note 此测试需要硬件支持（hw_req=spi+assist），如果硬件不可用，测试将失败
 */
void vsf_test_spi_fifo_irq_transfer(void)
{
    // 如果测试标记为 hw_req=spi+assist，应该确保硬件可用
    VSF_TEST_ASSERT(test_spi_instance != NULL);

    spi_test_irq_ctx_t irq_ctx = {
        .transfer_complete = false,
        .transfer_error = false,
        .irq_mask = 0,
    };

    vsf_spi_cfg_t cfg = TEST_SPI_DEFAULT_CFG;
    cfg.isr.handler_fn = __spi_irq_handler;
    cfg.isr.target_ptr = &irq_ctx;
    cfg.isr.prio = vsf_arch_prio_0;

    uint8_t tx[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                      0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10};
    uint8_t rx[16] = {0};
    uint_fast32_t tx_offset = 0, rx_offset = 0;
    vsf_err_t err;
    fsm_rt_t rt;
    uint32_t timeout_count = 0;
    const uint32_t max_timeout = SPI_POLLING_TIMEOUT_MS * 1000;

    // 初始化并使能
    err = vsf_spi_init(test_spi_instance, &cfg);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    rt = vsf_spi_enable(test_spi_instance);
    VSF_TEST_ASSERT((rt == fsm_rt_cpl) || (rt == fsm_rt_on_going));

    // 使能中断
    vsf_spi_irq_enable(test_spi_instance, VSF_SPI_IRQ_MASK_RX_CPL | VSF_SPI_IRQ_MASK_ERR);

    // 启动 FIFO 传输
    while ((tx_offset < sizeof(tx)) || (rx_offset < sizeof(rx))) {
        // 调用 FIFO 传输
        vsf_spi_fifo_transfer(test_spi_instance, tx, &tx_offset, rx, &rx_offset, sizeof(tx));

        // 检查是否完成（通过中断标志）
        if (irq_ctx.transfer_complete || irq_ctx.transfer_error) {
            break;
        }

        // 超时检查
        timeout_count++;
        if (timeout_count > max_timeout) {
            VSF_TEST_ASSERT(false); // 超时失败
            break;
        }
    }

    // 验证传输完成
    VSF_TEST_ASSERT(!irq_ctx.transfer_error);
    VSF_TEST_ASSERT(tx_offset >= sizeof(tx));
    VSF_TEST_ASSERT(rx_offset >= sizeof(rx));

    // 禁用中断
    vsf_spi_irq_disable(test_spi_instance, VSF_SPI_IRQ_MASK_RX_CPL | VSF_SPI_IRQ_MASK_ERR);

    // 清理
    rt = vsf_spi_disable(test_spi_instance);
    VSF_TEST_ASSERT((rt == fsm_rt_cpl) || (rt == fsm_rt_on_going));
    vsf_spi_fini(test_spi_instance);
}

/* EOF */

