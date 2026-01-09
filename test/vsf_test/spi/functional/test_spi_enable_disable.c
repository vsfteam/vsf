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
#include "test_spi_enable_disable.h"

/*============================ MACROS ========================================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/**
 * @brief 测试 vsf_spi_enable() 空指针参数
 */
void vsf_test_spi_enable_null_ptr(void)
{
    // enable 函数应该能安全处理 NULL 指针（不崩溃）
    fsm_rt_t rt = vsf_spi_enable(NULL);
    // 如果执行到这里，说明没有崩溃
    (void)rt;
}

/**
 * @brief 测试 vsf_spi_disable() 空指针参数
 */
void vsf_test_spi_disable_null_ptr(void)
{
    // disable 函数应该能安全处理 NULL 指针（不崩溃）
    fsm_rt_t rt = vsf_spi_disable(NULL);
    // 如果执行到这里，说明没有崩溃
    (void)rt;
}

/**
 * @brief 测试 vsf_spi_enable() 基本功能
 * @note 此测试需要硬件支持（hw_req=spi），如果硬件不可用，测试将失败
 */
void vsf_test_spi_enable_basic(void)
{
    // 如果测试标记为 hw_req=spi，应该确保硬件可用
    VSF_TEST_ASSERT(test_spi_instance != NULL);

    vsf_spi_cfg_t cfg = TEST_SPI_DEFAULT_CFG;
    fsm_rt_t rt;
    vsf_err_t err;

    // 初始化
    err = vsf_spi_init(test_spi_instance, &cfg);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    // 使能 SPI
    rt = vsf_spi_enable(test_spi_instance);
    // 应该返回 fsm_rt_cpl 或 fsm_rt_on_going
    VSF_TEST_ASSERT((rt == fsm_rt_cpl) || (rt == fsm_rt_on_going));

    // 清理
    rt = vsf_spi_disable(test_spi_instance);
    VSF_TEST_ASSERT((rt == fsm_rt_cpl) || (rt == fsm_rt_on_going));
    vsf_spi_fini(test_spi_instance);
}

/**
 * @brief 测试 vsf_spi_disable() 基本功能
 * @note 此测试需要硬件支持（hw_req=spi），如果硬件不可用，测试将失败
 */
void vsf_test_spi_disable_basic(void)
{
    // 如果测试标记为 hw_req=spi，应该确保硬件可用
    VSF_TEST_ASSERT(test_spi_instance != NULL);

    vsf_spi_cfg_t cfg = TEST_SPI_DEFAULT_CFG;
    fsm_rt_t rt;
    vsf_err_t err;

    // 初始化并使能
    err = vsf_spi_init(test_spi_instance, &cfg);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    rt = vsf_spi_enable(test_spi_instance);
    VSF_TEST_ASSERT((rt == fsm_rt_cpl) || (rt == fsm_rt_on_going));

    // 禁用 SPI
    rt = vsf_spi_disable(test_spi_instance);
    // 应该返回 fsm_rt_cpl 或 fsm_rt_on_going
    VSF_TEST_ASSERT((rt == fsm_rt_cpl) || (rt == fsm_rt_on_going));

    // 清理
    vsf_spi_fini(test_spi_instance);
}

/**
 * @brief 测试重复使能/禁用
 * @note 此测试需要硬件支持（hw_req=spi），如果硬件不可用，测试将失败
 */
void vsf_test_spi_enable_disable_repeat(void)
{
    // 如果测试标记为 hw_req=spi，应该确保硬件可用
    VSF_TEST_ASSERT(test_spi_instance != NULL);

    vsf_spi_cfg_t cfg = TEST_SPI_DEFAULT_CFG;
    fsm_rt_t rt;
    vsf_err_t err;

    // 初始化
    err = vsf_spi_init(test_spi_instance, &cfg);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    // 多次使能/禁用
    for (int i = 0; i < 3; i++) {
        rt = vsf_spi_enable(test_spi_instance);
        VSF_TEST_ASSERT((rt == fsm_rt_cpl) || (rt == fsm_rt_on_going));

        rt = vsf_spi_disable(test_spi_instance);
        VSF_TEST_ASSERT((rt == fsm_rt_cpl) || (rt == fsm_rt_on_going));
    }

    // 清理
    vsf_spi_fini(test_spi_instance);
}

/**
 * @brief 测试未初始化时使能/禁用
 * @note 此测试需要硬件支持（hw_req=spi），如果硬件不可用，测试将失败
 */
void vsf_test_spi_enable_disable_before_init(void)
{
    // 如果测试标记为 hw_req=spi，应该确保硬件可用
    VSF_TEST_ASSERT(test_spi_instance != NULL);

    fsm_rt_t rt;

    // 未初始化时使能（可能返回错误或忽略）
    rt = vsf_spi_enable(test_spi_instance);
    (void)rt;

    // 未初始化时禁用（可能返回错误或忽略）
    rt = vsf_spi_disable(test_spi_instance);
    // 未初始化时可能返回各种值，这里只检查不会崩溃
    (void)rt;
}

/* EOF */

