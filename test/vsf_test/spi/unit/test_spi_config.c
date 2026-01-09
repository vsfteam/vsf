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
#include "test_spi_config.h"

/*============================ MACROS ========================================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/**
 * @brief 测试 vsf_spi_get_configuration() 空指针参数
 */
void vsf_test_spi_get_config_null_ptr(void)
{
    vsf_spi_cfg_t cfg;
    vsf_err_t err;

    // 测试 spi_ptr 为 NULL
    err = vsf_spi_get_configuration(NULL, &cfg);
    VSF_TEST_ASSERT(err != VSF_ERR_NONE);

    // 测试 cfg_ptr 为 NULL（需要硬件实例）
    VSF_TEST_ASSERT(test_spi_instance != NULL);
    err = vsf_spi_get_configuration(test_spi_instance, NULL);
    VSF_TEST_ASSERT(err != VSF_ERR_NONE);
}

/**
 * @brief 测试 vsf_spi_get_configuration() 获取配置
 * @note 此测试需要硬件支持（hw_req=spi），如果硬件不可用，测试将失败
 */
void vsf_test_spi_get_config_normal(void)
{
    // 如果测试标记为 hw_req=spi，应该确保硬件可用
    VSF_TEST_ASSERT(test_spi_instance != NULL);

    vsf_spi_cfg_t cfg_set = TEST_SPI_DEFAULT_CFG;
    vsf_spi_cfg_t cfg_get;
    vsf_err_t err;

    // 初始化
    err = vsf_spi_init(test_spi_instance, &cfg_set);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    // 获取配置
    err = vsf_spi_get_configuration(test_spi_instance, &cfg_get);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    // 验证配置是否一致（至少模式应该一致）
    VSF_TEST_ASSERT(cfg_get.mode == cfg_set.mode);
    // 时钟频率可能被硬件调整，所以不强制检查

    // 清理
    vsf_spi_fini(test_spi_instance);
}

/**
 * @brief 测试不同模式的配置获取
 * @note 此测试需要硬件支持（hw_req=spi），如果硬件不可用，测试将失败
 */
void vsf_test_spi_get_config_different_modes(void)
{
    // 如果测试标记为 hw_req=spi，应该确保硬件可用
    VSF_TEST_ASSERT(test_spi_instance != NULL);

    vsf_spi_cfg_t cfg_set, cfg_get;
    vsf_err_t err;
    vsf_spi_mode_t modes[] = {
        VSF_SPI_MASTER | VSF_SPI_MODE_0 | VSF_SPI_DATASIZE_8,
        VSF_SPI_MASTER | VSF_SPI_MODE_1 | VSF_SPI_DATASIZE_8,
        VSF_SPI_MASTER | VSF_SPI_MODE_2 | VSF_SPI_DATASIZE_8,
        VSF_SPI_MASTER | VSF_SPI_MODE_3 | VSF_SPI_DATASIZE_8,
    };
    int i;

    for (i = 0; i < dimof(modes); i++) {
        cfg_set = test_spi_get_default_cfg();
        cfg_set.mode = modes[i];

        err = vsf_spi_init(test_spi_instance, &cfg_set);
        VSF_TEST_ASSERT(err == VSF_ERR_NONE);

        err = vsf_spi_get_configuration(test_spi_instance, &cfg_get);
        VSF_TEST_ASSERT(err == VSF_ERR_NONE);
        VSF_TEST_ASSERT(cfg_get.mode == cfg_set.mode);

        vsf_spi_fini(test_spi_instance);
    }
}

/**
 * @brief 测试不同数据位宽的配置获取
 * @note 此测试需要硬件支持（hw_req=spi），如果硬件不可用，测试将失败
 */
void vsf_test_spi_get_config_different_data_sizes(void)
{
    // 如果测试标记为 hw_req=spi，应该确保硬件可用
    VSF_TEST_ASSERT(test_spi_instance != NULL);

    vsf_spi_cfg_t cfg_set, cfg_get;
    vsf_err_t err;
    vsf_spi_mode_t data_sizes[] = {
        VSF_SPI_MASTER | VSF_SPI_MODE_0 | VSF_SPI_DATASIZE_8,
        VSF_SPI_MASTER | VSF_SPI_MODE_0 | VSF_SPI_DATASIZE_16,
        VSF_SPI_MASTER | VSF_SPI_MODE_0 | VSF_SPI_DATASIZE_32,
    };
    int i;

    for (i = 0; i < dimof(data_sizes); i++) {
        cfg_set = test_spi_get_default_cfg();
        cfg_set.mode = data_sizes[i];

        err = vsf_spi_init(test_spi_instance, &cfg_set);
        VSF_TEST_ASSERT(err == VSF_ERR_NONE);

        err = vsf_spi_get_configuration(test_spi_instance, &cfg_get);
        VSF_TEST_ASSERT(err == VSF_ERR_NONE);
        VSF_TEST_ASSERT(cfg_get.mode == cfg_set.mode);

        vsf_spi_fini(test_spi_instance);
    }
}

/**
 * @brief 测试不同时钟频率的配置获取
 * @note 此测试需要硬件支持（hw_req=spi），如果硬件不可用，测试将失败
 */
void vsf_test_spi_get_config_different_clocks(void)
{
    // 如果测试标记为 hw_req=spi，应该确保硬件可用
    VSF_TEST_ASSERT(test_spi_instance != NULL);

    vsf_spi_cfg_t cfg_set, cfg_get;
    vsf_err_t err;
    uint32_t clocks[] = {100000, 500000, 1000000, 5000000, 10000000};
    int i;

    for (i = 0; i < dimof(clocks); i++) {
        cfg_set = test_spi_get_default_cfg();
        cfg_set.clock_hz = clocks[i];

        err = vsf_spi_init(test_spi_instance, &cfg_set);
        if (err == VSF_ERR_NONE) {
            err = vsf_spi_get_configuration(test_spi_instance, &cfg_get);
            VSF_TEST_ASSERT(err == VSF_ERR_NONE);
            // 时钟频率可能被硬件调整，所以不强制检查完全一致

            vsf_spi_fini(test_spi_instance);
        }
    }
}

/**
 * @brief 测试配置边界值
 * @note 此测试需要硬件支持（hw_req=spi），如果硬件不可用，测试将失败
 */
void vsf_test_spi_get_config_boundary(void)
{
    // 如果测试标记为 hw_req=spi，应该确保硬件可用
    VSF_TEST_ASSERT(test_spi_instance != NULL);

    vsf_spi_cfg_t cfg_set, cfg_get;
    vsf_err_t err;

    // 测试最小时钟频率（如果硬件支持）
    cfg_set = test_spi_get_default_cfg();
    cfg_set.clock_hz = 1;
    err = vsf_spi_init(test_spi_instance, &cfg_set);
    // 可能失败，也可能被硬件限制到最小值
    if (err == VSF_ERR_NONE) {
        err = vsf_spi_get_configuration(test_spi_instance, &cfg_get);
        VSF_TEST_ASSERT(err == VSF_ERR_NONE);
        vsf_spi_fini(test_spi_instance);
    }

    // 测试最大时钟频率（如果硬件支持）
    cfg_set = test_spi_get_default_cfg();
    cfg_set.clock_hz = UINT32_MAX;
    err = vsf_spi_init(test_spi_instance, &cfg_set);
    // 可能失败，也可能被硬件限制到最大值
    if (err == VSF_ERR_NONE) {
        err = vsf_spi_get_configuration(test_spi_instance, &cfg_get);
        VSF_TEST_ASSERT(err == VSF_ERR_NONE);
        vsf_spi_fini(test_spi_instance);
    }
}

/* EOF */

