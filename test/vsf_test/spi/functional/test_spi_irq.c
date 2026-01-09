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
#include "test_spi_irq.h"

/*============================ MACROS ========================================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/**
 * @brief 测试 vsf_spi_irq_enable() 空指针参数
 */
void vsf_test_spi_irq_enable_null_ptr(void)
{
    // irq_enable 函数应该能安全处理 NULL 指针（不崩溃）
    vsf_spi_irq_enable(NULL, 0);
    // 如果执行到这里，说明没有崩溃
}

/**
 * @brief 测试 vsf_spi_irq_disable() 空指针参数
 */
void vsf_test_spi_irq_disable_null_ptr(void)
{
    // irq_disable 函数应该能安全处理 NULL 指针（不崩溃）
    vsf_spi_irq_disable(NULL, 0);
    // 如果执行到这里，说明没有崩溃
}

/**
 * @brief 测试 vsf_spi_irq_clear() 空指针参数
 */
void vsf_test_spi_irq_clear_null_ptr(void)
{
    // irq_clear 函数应该能安全处理 NULL 指针（返回零值）
    vsf_spi_irq_mask_t mask = vsf_spi_irq_clear(NULL, 0);
    // 如果执行到这里，说明没有崩溃
    (void)mask;
}

/**
 * @brief 测试 vsf_spi_irq_enable() 基本功能
 * @note 此测试需要硬件支持（hw_req=spi），如果硬件不可用，测试将失败
 */
void vsf_test_spi_irq_enable_basic(void)
{
    // 如果测试标记为 hw_req=spi，应该确保硬件可用
    VSF_TEST_ASSERT(test_spi_instance != NULL);

    vsf_spi_cfg_t cfg = TEST_SPI_DEFAULT_CFG;
    vsf_err_t err;
    fsm_rt_t rt;

    // 初始化并使能
    err = vsf_spi_init(test_spi_instance, &cfg);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    rt = vsf_spi_enable(test_spi_instance);
    VSF_TEST_ASSERT((rt == fsm_rt_cpl) || (rt == fsm_rt_on_going));

    // 使能中断（测试所有中断类型）
    vsf_spi_irq_enable(test_spi_instance, VSF_SPI_IRQ_ALL_BITS_MASK);

    // 清理
    vsf_spi_irq_disable(test_spi_instance, VSF_SPI_IRQ_ALL_BITS_MASK);
    rt = vsf_spi_disable(test_spi_instance);
    (void)rt;
    vsf_spi_fini(test_spi_instance);
}

/**
 * @brief 测试 vsf_spi_irq_disable() 基本功能
 * @note 此测试需要硬件支持（hw_req=spi），如果硬件不可用，测试将失败
 */
void vsf_test_spi_irq_disable_basic(void)
{
    // 如果测试标记为 hw_req=spi，应该确保硬件可用
    VSF_TEST_ASSERT(test_spi_instance != NULL);

    vsf_spi_cfg_t cfg = TEST_SPI_DEFAULT_CFG;
    vsf_err_t err;
    fsm_rt_t rt;

    // 初始化并使能
    err = vsf_spi_init(test_spi_instance, &cfg);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    rt = vsf_spi_enable(test_spi_instance);
    VSF_TEST_ASSERT((rt == fsm_rt_cpl) || (rt == fsm_rt_on_going));

    // 使能中断
    vsf_spi_irq_enable(test_spi_instance, VSF_SPI_IRQ_ALL_BITS_MASK);

    // 禁用中断
    vsf_spi_irq_disable(test_spi_instance, VSF_SPI_IRQ_ALL_BITS_MASK);

    // 清理
    rt = vsf_spi_disable(test_spi_instance);
    VSF_TEST_ASSERT((rt == fsm_rt_cpl) || (rt == fsm_rt_on_going));
    vsf_spi_fini(test_spi_instance);
}

/**
 * @brief 测试 vsf_spi_irq_clear() 基本功能
 * @note 此测试需要硬件支持（hw_req=spi），如果硬件不可用，测试将失败
 */
void vsf_test_spi_irq_clear_basic(void)
{
    // 如果测试标记为 hw_req=spi，应该确保硬件可用
    VSF_TEST_ASSERT(test_spi_instance != NULL);

    vsf_spi_cfg_t cfg = TEST_SPI_DEFAULT_CFG;
    vsf_spi_irq_mask_t mask;
    vsf_err_t err;
    fsm_rt_t rt;

    // 初始化并使能
    err = vsf_spi_init(test_spi_instance, &cfg);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    rt = vsf_spi_enable(test_spi_instance);
    VSF_TEST_ASSERT((rt == fsm_rt_cpl) || (rt == fsm_rt_on_going));

    // 清除中断标志
    mask = vsf_spi_irq_clear(test_spi_instance, VSF_SPI_IRQ_ALL_BITS_MASK);
    // 返回清除的中断标志
    (void)mask;

    // 清理
    rt = vsf_spi_disable(test_spi_instance);
    VSF_TEST_ASSERT((rt == fsm_rt_cpl) || (rt == fsm_rt_on_going));
    vsf_spi_fini(test_spi_instance);
}

/**
 * @brief 测试中断使能/禁用切换
 * @note 此测试需要硬件支持（hw_req=spi），如果硬件不可用，测试将失败
 */
void vsf_test_spi_irq_toggle(void)
{
    // 如果测试标记为 hw_req=spi，应该确保硬件可用
    VSF_TEST_ASSERT(test_spi_instance != NULL);

    vsf_spi_cfg_t cfg = TEST_SPI_DEFAULT_CFG;
    vsf_err_t err;
    fsm_rt_t rt;

    // 初始化并使能
    err = vsf_spi_init(test_spi_instance, &cfg);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    rt = vsf_spi_enable(test_spi_instance);
    VSF_TEST_ASSERT((rt == fsm_rt_cpl) || (rt == fsm_rt_on_going));

    // 多次切换中断使能/禁用
    for (int i = 0; i < 3; i++) {
        vsf_spi_irq_enable(test_spi_instance, VSF_SPI_IRQ_ALL_BITS_MASK);
        vsf_spi_irq_disable(test_spi_instance, VSF_SPI_IRQ_ALL_BITS_MASK);
    }

    // 清理
    rt = vsf_spi_disable(test_spi_instance);
    VSF_TEST_ASSERT((rt == fsm_rt_cpl) || (rt == fsm_rt_on_going));
    vsf_spi_fini(test_spi_instance);
}

/**
 * @brief 测试部分中断使能
 * @note 此测试需要硬件支持（hw_req=spi），如果硬件不可用，测试将失败
 */
void vsf_test_spi_irq_partial(void)
{
    // 如果测试标记为 hw_req=spi，应该确保硬件可用
    VSF_TEST_ASSERT(test_spi_instance != NULL);

    vsf_spi_cfg_t cfg = TEST_SPI_DEFAULT_CFG;
    vsf_err_t err;
    fsm_rt_t rt;

    // 初始化并使能
    err = vsf_spi_init(test_spi_instance, &cfg);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    rt = vsf_spi_enable(test_spi_instance);
    VSF_TEST_ASSERT((rt == fsm_rt_cpl) || (rt == fsm_rt_on_going));

    // 使能部分中断（如果定义了具体的中断类型）
    // 注意：具体的中断类型取决于硬件实现
    vsf_spi_irq_enable(test_spi_instance, 0x01);
    vsf_spi_irq_disable(test_spi_instance, 0x01);

    // 清理
    rt = vsf_spi_disable(test_spi_instance);
    VSF_TEST_ASSERT((rt == fsm_rt_cpl) || (rt == fsm_rt_on_going));
    vsf_spi_fini(test_spi_instance);
}

/* EOF */

