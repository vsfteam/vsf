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
#include "test_spi_capability.h"

/*============================ MACROS ========================================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/**
 * @brief 测试 vsf_spi_capability() 空指针参数
 */
void vsf_test_spi_capability_null_ptr(void)
{
    // capability 函数应该能安全处理 NULL 指针（返回默认能力或零值）
    vsf_spi_capability_t cap = vsf_spi_capability(NULL);
    // 如果执行到这里，说明没有崩溃
    (void)cap;
}

/**
 * @brief 测试 vsf_spi_capability() 基本功能
 * @note 此测试需要硬件支持（hw_req=spi），如果硬件不可用，测试将失败
 */
void vsf_test_spi_capability_basic(void)
{
    // 如果测试标记为 hw_req=spi，应该确保硬件可用
    VSF_TEST_ASSERT(test_spi_instance != NULL);

    vsf_spi_capability_t cap;

    // 获取能力（不需要初始化）
    cap = vsf_spi_capability(test_spi_instance);

    // 验证能力结构体已填充（至少检查不会崩溃）
    (void)cap;
}

/**
 * @brief 测试 vsf_spi_capability() 多次调用一致性
 * @note 此测试需要硬件支持（hw_req=spi），如果硬件不可用，测试将失败
 */
void vsf_test_spi_capability_consistency(void)
{
    // 如果测试标记为 hw_req=spi，应该确保硬件可用
    VSF_TEST_ASSERT(test_spi_instance != NULL);

    vsf_spi_capability_t cap1, cap2;

    // 第一次获取
    cap1 = vsf_spi_capability(test_spi_instance);

    // 第二次获取
    cap2 = vsf_spi_capability(test_spi_instance);

    // 能力应该一致（比较结构体内容）
    VSF_TEST_ASSERT(memcmp(&cap1, &cap2, sizeof(vsf_spi_capability_t)) == 0);
}

/**
 * @brief 测试不同 SPI 实例的能力
 * @note 此测试需要硬件支持（hw_req=spi），如果硬件不可用，测试将失败
 */
void vsf_test_spi_capability_multi_instance(void)
{
    // 如果测试标记为 hw_req=spi，应该确保硬件可用
    VSF_TEST_ASSERT(test_spi_instance != NULL);

    // 测试传入的 SPI 实例的能力
    // 注意：多实例测试需要调用者传入不同的实例，这里只测试传入的实例
    vsf_spi_capability_t cap = vsf_spi_capability(test_spi_instance);
    // 验证不会崩溃
    (void)cap;
}

/**
 * @brief 测试能力标志位（如果定义了相关字段）
 * @note 此测试需要硬件支持（hw_req=spi），如果硬件不可用，测试将失败
 */
void vsf_test_spi_capability_flags(void)
{
    // 如果测试标记为 hw_req=spi，应该确保硬件可用
    VSF_TEST_ASSERT(test_spi_instance != NULL);

    vsf_spi_capability_t cap;

    cap = vsf_spi_capability(test_spi_instance);

    // 检查能力结构体的基本字段（如果定义了）
    // 注意：具体字段取决于 vsf_spi_capability_t 的定义
    // 这里只验证函数调用不会崩溃
    (void)cap;
}

/**
 * @brief 测试能力查询在初始化前后的一致性
 * @note 此测试需要硬件支持（hw_req=spi），如果硬件不可用，测试将失败
 */
void vsf_test_spi_capability_before_after_init(void)
{
    // 如果测试标记为 hw_req=spi，应该确保硬件可用
    VSF_TEST_ASSERT(test_spi_instance != NULL);

    vsf_spi_capability_t cap_before, cap_after;
    vsf_spi_cfg_t cfg = TEST_SPI_DEFAULT_CFG;

    // 初始化前获取能力
    cap_before = vsf_spi_capability(test_spi_instance);

    // 初始化
    vsf_err_t err = vsf_spi_init(test_spi_instance, &cfg);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    // 初始化后获取能力
    cap_after = vsf_spi_capability(test_spi_instance);

    // 能力应该一致（硬件能力不应该因初始化而改变）
    VSF_TEST_ASSERT(memcmp(&cap_before, &cap_after, sizeof(vsf_spi_capability_t)) == 0);

    // 清理
    vsf_spi_fini(test_spi_instance);
}

/* EOF */

