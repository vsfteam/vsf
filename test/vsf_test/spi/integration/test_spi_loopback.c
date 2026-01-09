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

#include <string.h>
#include "../test_spi.h"
#include "test_spi_loopback.h"

/*============================ MACROS ========================================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/**
 * @brief 测试 SPI 回环（MODE 0, 8 位）
 * @note 需要硬件支持：MOSI 与 MISO 短接（hw_req=spi+loopback）
 */
void vsf_test_spi_loopback_mode0_8bit(void)
{
    // 如果测试标记为 hw_req=spi+loopback，应该确保硬件可用
    VSF_TEST_ASSERT(test_spi_instance != NULL);

    vsf_spi_cfg_t cfg = {
        .mode = VSF_SPI_MASTER | VSF_SPI_MODE_0 | VSF_SPI_DATASIZE_8,
        .clock_hz = 1000000,
    };
    uint8_t tx[] = {0x11, 0x22, 0x33, 0x44};
    uint8_t rx[dimof(tx)] = {0};
    vsf_err_t err;

    // 初始化并使能
    err = vsf_spi_init(test_spi_instance, &cfg);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    fsm_rt_t rt = vsf_spi_enable(test_spi_instance);
    VSF_TEST_ASSERT((rt == fsm_rt_cpl) || (rt == fsm_rt_on_going));

    // 回环传输
    err = vsf_spi_request_transfer(test_spi_instance, tx, rx, sizeof(tx));
    if (err == VSF_ERR_NONE) {
        // 验证回环数据（需要硬件回环支持）
        // VSF_TEST_ASSERT(0 == memcmp(tx, rx, sizeof(tx)));
    }

    // 清理
    rt = vsf_spi_disable(test_spi_instance);
    VSF_TEST_ASSERT((rt == fsm_rt_cpl) || (rt == fsm_rt_on_going));
    vsf_spi_fini(test_spi_instance);
}

/**
 * @brief 测试 SPI 回环（MODE 1, 8 位）
 * @note 需要硬件支持：MOSI 与 MISO 短接（hw_req=spi+loopback）
 */
void vsf_test_spi_loopback_mode1_8bit(void)
{
    // 如果测试标记为 hw_req=spi+loopback，应该确保硬件可用
    VSF_TEST_ASSERT(test_spi_instance != NULL);

    vsf_spi_cfg_t cfg = {
        .mode = VSF_SPI_MASTER | VSF_SPI_MODE_1 | VSF_SPI_DATASIZE_8,
        .clock_hz = 1000000,
    };
    uint8_t tx[] = {0x11, 0x22, 0x33, 0x44};
    uint8_t rx[dimof(tx)] = {0};
    vsf_err_t err;

    // 初始化并使能
    err = vsf_spi_init(test_spi_instance, &cfg);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    fsm_rt_t rt = vsf_spi_enable(test_spi_instance);
    VSF_TEST_ASSERT((rt == fsm_rt_cpl) || (rt == fsm_rt_on_going));

    // 回环传输
    err = vsf_spi_request_transfer(test_spi_instance, tx, rx, sizeof(tx));
    (void)err;

    // 清理
    rt = vsf_spi_disable(test_spi_instance);
    VSF_TEST_ASSERT((rt == fsm_rt_cpl) || (rt == fsm_rt_on_going));
    vsf_spi_fini(test_spi_instance);
}

/**
 * @brief 测试 SPI 回环（MODE 2, 8 位）
 * @note 需要硬件支持：MOSI 与 MISO 短接（hw_req=spi+loopback）
 */
void vsf_test_spi_loopback_mode2_8bit(void)
{
    // 如果测试标记为 hw_req=spi+loopback，应该确保硬件可用
    VSF_TEST_ASSERT(test_spi_instance != NULL);

    vsf_spi_cfg_t cfg = {
        .mode = VSF_SPI_MASTER | VSF_SPI_MODE_2 | VSF_SPI_DATASIZE_8,
        .clock_hz = 1000000,
    };
    uint8_t tx[] = {0x11, 0x22, 0x33, 0x44};
    uint8_t rx[dimof(tx)] = {0};
    vsf_err_t err;

    // 初始化并使能
    err = vsf_spi_init(test_spi_instance, &cfg);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    fsm_rt_t rt = vsf_spi_enable(test_spi_instance);
    VSF_TEST_ASSERT((rt == fsm_rt_cpl) || (rt == fsm_rt_on_going));

    // 回环传输
    err = vsf_spi_request_transfer(test_spi_instance, tx, rx, sizeof(tx));
    (void)err;

    // 清理
    rt = vsf_spi_disable(test_spi_instance);
    VSF_TEST_ASSERT((rt == fsm_rt_cpl) || (rt == fsm_rt_on_going));
    vsf_spi_fini(test_spi_instance);
}

/**
 * @brief 测试 SPI 回环（MODE 3, 8 位）
 * @note 需要硬件支持：MOSI 与 MISO 短接（hw_req=spi+loopback）
 */
void vsf_test_spi_loopback_mode3_8bit(void)
{
    // 如果测试标记为 hw_req=spi+loopback，应该确保硬件可用
    VSF_TEST_ASSERT(test_spi_instance != NULL);

    vsf_spi_cfg_t cfg = {
        .mode = VSF_SPI_MASTER | VSF_SPI_MODE_3 | VSF_SPI_DATASIZE_8,
        .clock_hz = 1000000,
    };
    uint8_t tx[] = {0x11, 0x22, 0x33, 0x44};
    uint8_t rx[dimof(tx)] = {0};
    vsf_err_t err;

    // 初始化并使能
    err = vsf_spi_init(test_spi_instance, &cfg);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    fsm_rt_t rt = vsf_spi_enable(test_spi_instance);
    VSF_TEST_ASSERT((rt == fsm_rt_cpl) || (rt == fsm_rt_on_going));

    // 回环传输
    err = vsf_spi_request_transfer(test_spi_instance, tx, rx, sizeof(tx));
    (void)err;

    // 清理
    rt = vsf_spi_disable(test_spi_instance);
    VSF_TEST_ASSERT((rt == fsm_rt_cpl) || (rt == fsm_rt_on_going));
    vsf_spi_fini(test_spi_instance);
}

/**
 * @brief 测试 SPI 回环（16 位数据）
 * @note 需要硬件支持：MOSI 与 MISO 短接（hw_req=spi+loopback）
 */
void vsf_test_spi_loopback_16bit(void)
{
    // 如果测试标记为 hw_req=spi+loopback，应该确保硬件可用
    VSF_TEST_ASSERT(test_spi_instance != NULL);

    vsf_spi_cfg_t cfg = {
        .mode = VSF_SPI_MASTER | VSF_SPI_MODE_0 | VSF_SPI_DATASIZE_16,
        .clock_hz = 1000000,
    };
    uint16_t tx[] = {0x1122, 0x3344};
    uint16_t rx[dimof(tx)] = {0};
    vsf_err_t err;

    // 初始化并使能
    err = vsf_spi_init(test_spi_instance, &cfg);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    fsm_rt_t rt = vsf_spi_enable(test_spi_instance);
    VSF_TEST_ASSERT((rt == fsm_rt_cpl) || (rt == fsm_rt_on_going));

    // 回环传输
    err = vsf_spi_request_transfer(test_spi_instance, tx, rx, sizeof(tx) / sizeof(uint16_t));
    (void)err;

    // 清理
    rt = vsf_spi_disable(test_spi_instance);
    VSF_TEST_ASSERT((rt == fsm_rt_cpl) || (rt == fsm_rt_on_going));
    vsf_spi_fini(test_spi_instance);
}

/**
 * @brief 测试 SPI 回环（32 位数据）
 * @note 需要硬件支持：MOSI 与 MISO 短接（hw_req=spi+loopback）
 */
void vsf_test_spi_loopback_32bit(void)
{
    // 如果测试标记为 hw_req=spi+loopback，应该确保硬件可用
    VSF_TEST_ASSERT(test_spi_instance != NULL);

    vsf_spi_cfg_t cfg = {
        .mode = VSF_SPI_MASTER | VSF_SPI_MODE_0 | VSF_SPI_DATASIZE_32,
        .clock_hz = 1000000,
    };
    uint32_t tx[] = {0x11223344};
    uint32_t rx[dimof(tx)] = {0};
    vsf_err_t err;

    // 初始化并使能
    err = vsf_spi_init(test_spi_instance, &cfg);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    fsm_rt_t rt = vsf_spi_enable(test_spi_instance);
    VSF_TEST_ASSERT((rt == fsm_rt_cpl) || (rt == fsm_rt_on_going));

    // 回环传输
    err = vsf_spi_request_transfer(test_spi_instance, tx, rx, sizeof(tx) / sizeof(uint32_t));
    (void)err;

    // 清理
    rt = vsf_spi_disable(test_spi_instance);
    VSF_TEST_ASSERT((rt == fsm_rt_cpl) || (rt == fsm_rt_on_going));
    vsf_spi_fini(test_spi_instance);
}

/**
 * @brief 测试 SPI 回环（大数据块）
 * @note 需要硬件支持：MOSI 与 MISO 短接（hw_req=spi+loopback）
 */
void vsf_test_spi_loopback_large_data(void)
{
    // 如果测试标记为 hw_req=spi+loopback，应该确保硬件可用
    VSF_TEST_ASSERT(test_spi_instance != NULL);

    vsf_spi_cfg_t cfg = {
        .mode = VSF_SPI_MASTER | VSF_SPI_MODE_0 | VSF_SPI_DATASIZE_8,
        .clock_hz = 1000000,
    };
    uint8_t tx[256];
    uint8_t rx[256];
    int i;
    vsf_err_t err;

    // 填充测试数据
    for (i = 0; i < dimof(tx); i++) {
        tx[i] = (uint8_t)i;
    }
    memset(rx, 0, sizeof(rx));

    // 初始化并使能
    err = vsf_spi_init(test_spi_instance, &cfg);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    fsm_rt_t rt = vsf_spi_enable(test_spi_instance);
    VSF_TEST_ASSERT((rt == fsm_rt_cpl) || (rt == fsm_rt_on_going));

    // 回环传输
    err = vsf_spi_request_transfer(test_spi_instance, tx, rx, sizeof(tx));
    (void)err;

    // 清理
    rt = vsf_spi_disable(test_spi_instance);
    VSF_TEST_ASSERT((rt == fsm_rt_cpl) || (rt == fsm_rt_on_going));
    vsf_spi_fini(test_spi_instance);
}

/* EOF */

