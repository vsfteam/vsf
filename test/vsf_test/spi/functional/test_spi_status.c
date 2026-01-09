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
#include "test_spi_status.h"
#include <string.h>

/*============================ MACROS ========================================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/**
 * @brief 测试 vsf_spi_status() 空指针参数
 */
void vsf_test_spi_status_null_ptr(void)
{
    // status 函数应该能安全处理 NULL 指针（返回默认状态或零值）
    vsf_spi_status_t status = vsf_spi_status(NULL);
    // 如果执行到这里，说明没有崩溃
    (void)status;
}

/**
 * @brief 测试 vsf_spi_status() 基本功能
 * @note 此测试需要硬件支持（hw_req=spi），如果硬件不可用，测试将失败
 */
void vsf_test_spi_status_basic(void)
{
    // 如果测试标记为 hw_req=spi，应该确保硬件可用
    VSF_TEST_ASSERT(test_spi_instance != NULL);

    vsf_spi_cfg_t cfg = TEST_SPI_DEFAULT_CFG;
    vsf_spi_status_t status;
    vsf_err_t err;
    fsm_rt_t rt;

    // 初始化前获取状态
    status = vsf_spi_status(test_spi_instance);
    (void)status;

    // 初始化后获取状态
    err = vsf_spi_init(test_spi_instance, &cfg);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    status = vsf_spi_status(test_spi_instance);
    (void)status;

    // 使能后获取状态
    rt = vsf_spi_enable(test_spi_instance);
    VSF_TEST_ASSERT((rt == fsm_rt_cpl) || (rt == fsm_rt_on_going));
    status = vsf_spi_status(test_spi_instance);
    (void)status;

    // 禁用后获取状态
    rt = vsf_spi_disable(test_spi_instance);
    VSF_TEST_ASSERT((rt == fsm_rt_cpl) || (rt == fsm_rt_on_going));
    status = vsf_spi_status(test_spi_instance);
    (void)status;

    // 清理
    vsf_spi_fini(test_spi_instance);
}

/**
 * @brief 测试状态在不同阶段的一致性
 * @note 此测试需要硬件支持（hw_req=spi），如果硬件不可用，测试将失败
 */
void vsf_test_spi_status_consistency(void)
{
    // 如果测试标记为 hw_req=spi，应该确保硬件可用
    VSF_TEST_ASSERT(test_spi_instance != NULL);

    vsf_spi_cfg_t cfg = TEST_SPI_DEFAULT_CFG;
    vsf_spi_status_t status1, status2;
    vsf_err_t err;

    // 初始化
    err = vsf_spi_init(test_spi_instance, &cfg);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    // 多次获取状态应该一致（在相同条件下）
    status1 = vsf_spi_status(test_spi_instance);
    status2 = vsf_spi_status(test_spi_instance);
    VSF_TEST_ASSERT(memcmp(&status1, &status2, sizeof(vsf_spi_status_t)) == 0);

    // 清理
    vsf_spi_fini(test_spi_instance);
}

/* EOF */

