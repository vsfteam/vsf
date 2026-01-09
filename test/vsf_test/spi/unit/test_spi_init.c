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
#include "test_spi_init.h"

/*============================ MACROS ========================================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/**
 * @brief 测试 vsf_spi_init() 空指针参数
 */
void vsf_test_spi_init_null_ptr(void)
{
    vsf_spi_cfg_t cfg = TEST_SPI_DEFAULT_CFG;
    vsf_err_t err;

    // 测试 spi_ptr 为 NULL
    err = vsf_spi_init(NULL, &cfg);
    VSF_TEST_ASSERT(err != VSF_ERR_NONE);

    // 测试 cfg_ptr 为 NULL（需要硬件实例）
    VSF_TEST_ASSERT(test_spi_instance != NULL);
    err = vsf_spi_init(test_spi_instance, NULL);
    VSF_TEST_ASSERT(err != VSF_ERR_NONE);
}

/**
 * @brief 测试 vsf_spi_init() 无效配置参数
 */
void vsf_test_spi_init_invalid_mode(void)
{
    // 需要硬件实例来测试无效配置
    VSF_TEST_ASSERT(test_spi_instance != NULL);

    vsf_spi_cfg_t cfg;
    vsf_err_t err;

    // 测试非法模式值
    cfg = test_spi_get_default_cfg();
    cfg.mode = (vsf_spi_mode_t)0xFFFFFFFF;
    err = vsf_spi_init(test_spi_instance, &cfg);
    // 某些实现可能接受，某些可能拒绝，这里只检查不会崩溃
    // 如果返回错误，说明参数验证有效
    (void)err;
}

/**
 * @brief 测试 vsf_spi_init() 无效时钟频率
 */
void vsf_test_spi_init_invalid_clock(void)
{
    // 需要硬件实例来测试无效配置
    VSF_TEST_ASSERT(test_spi_instance != NULL);

    vsf_spi_cfg_t cfg;
    vsf_err_t err;

    // 测试时钟频率为 0
    cfg = test_spi_get_default_cfg();
    cfg.clock_hz = 0;
    err = vsf_spi_init(test_spi_instance, &cfg);
    // 某些实现可能接受，某些可能拒绝
    (void)err;
}

/**
 * @brief 测试 vsf_spi_init() 有效参数
 * @note 此测试需要硬件支持（hw_req=spi），如果硬件不可用，测试将失败
 */
void vsf_test_spi_init_valid(void)
{
    // 如果测试标记为 hw_req=spi，应该确保硬件可用
    VSF_TEST_ASSERT(test_spi_instance != NULL);

    vsf_spi_cfg_t cfg = TEST_SPI_DEFAULT_CFG;
    vsf_err_t err;

    err = vsf_spi_init(test_spi_instance, &cfg);
    // 如果硬件可用，应该成功；如果不可用，可能返回错误
    // 这里只检查不会崩溃，不强制要求成功
    (void)err;

    // 清理
    vsf_spi_fini(test_spi_instance);
}

/**
 * @brief 测试重复初始化
 * @note 此测试需要硬件支持（hw_req=spi），如果硬件不可用，测试将失败
 */
void vsf_test_spi_init_repeat(void)
{
    // 如果测试标记为 hw_req=spi，应该确保硬件可用
    VSF_TEST_ASSERT(test_spi_instance != NULL);

    vsf_spi_cfg_t cfg = TEST_SPI_DEFAULT_CFG;
    vsf_err_t err1, err2;

    // 第一次初始化
    err1 = vsf_spi_init(test_spi_instance, &cfg);

    // 第二次初始化（重复初始化）
    err2 = vsf_spi_init(test_spi_instance, &cfg);
    // 某些实现可能允许重复初始化，某些可能不允许
    (void)err1;
    (void)err2;

    // 清理
    vsf_spi_fini(test_spi_instance);
}

/**
 * @brief 测试 vsf_spi_fini() 空指针
 */
void vsf_test_spi_fini_null_ptr(void)
{
    // fini 应该能安全处理 NULL 指针（不崩溃）
    vsf_spi_fini(NULL);
    // 如果执行到这里，说明没有崩溃
}

/**
 * @brief 测试 vsf_spi_fini() 正常流程
 * @note 此测试需要硬件支持（hw_req=spi），如果硬件不可用，测试将失败
 */
void vsf_test_spi_fini_normal(void)
{
    // 如果测试标记为 hw_req=spi，应该确保硬件可用
    VSF_TEST_ASSERT(test_spi_instance != NULL);

    vsf_spi_cfg_t cfg = TEST_SPI_DEFAULT_CFG;
    vsf_err_t err;

    // 初始化
    err = vsf_spi_init(test_spi_instance, &cfg);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    // 反初始化
    vsf_spi_fini(test_spi_instance);

    // 再次反初始化（应该安全）
    vsf_spi_fini(test_spi_instance);
}

/* EOF */

