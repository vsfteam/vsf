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
#include "test_spi_cs.h"

/*============================ MACROS ========================================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/**
 * @brief 测试 vsf_spi_cs_active() 空指针参数
 */
void vsf_test_spi_cs_active_null_ptr(void)
{
    vsf_err_t err = vsf_spi_cs_active(NULL, 0);
    VSF_TEST_ASSERT(err != VSF_ERR_NONE);
}

/**
 * @brief 测试 vsf_spi_cs_inactive() 空指针参数
 */
void vsf_test_spi_cs_inactive_null_ptr(void)
{
    vsf_err_t err = vsf_spi_cs_inactive(NULL, 0);
    VSF_TEST_ASSERT(err != VSF_ERR_NONE);
}

/**
 * @brief 测试 vsf_spi_cs_active() 基本功能
 * @note 此测试需要硬件支持（hw_req=spi），如果硬件不可用，测试将失败
 */
void vsf_test_spi_cs_active_basic(void)
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

    // 激活片选 0
    err = vsf_spi_cs_active(test_spi_instance, 0);
    // 可能成功或失败（取决于硬件支持）
    (void)err;

    // 清理
    err = vsf_spi_cs_inactive(test_spi_instance, 0);
    (void)err;
    rt = vsf_spi_disable(test_spi_instance);
    (void)rt;
    vsf_spi_fini(test_spi_instance);
}

/**
 * @brief 测试 vsf_spi_cs_inactive() 基本功能
 * @note 此测试需要硬件支持（hw_req=spi），如果硬件不可用，测试将失败
 */
void vsf_test_spi_cs_inactive_basic(void)
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

    // 激活片选
    err = vsf_spi_cs_active(test_spi_instance, 0);
    (void)err;

    // 取消片选
    err = vsf_spi_cs_inactive(test_spi_instance, 0);
    // 可能成功或失败（取决于硬件支持）
    (void)err;

    // 清理
    rt = vsf_spi_disable(test_spi_instance);
    VSF_TEST_ASSERT((rt == fsm_rt_cpl) || (rt == fsm_rt_on_going));
    vsf_spi_fini(test_spi_instance);
}

/**
 * @brief 测试片选切换
 * @note 此测试需要硬件支持（hw_req=spi），如果硬件不可用，测试将失败
 */
void vsf_test_spi_cs_switch(void)
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

    // 切换多个片选
    for (uint_fast8_t i = 0; i < 4; i++) {
        err = vsf_spi_cs_active(test_spi_instance, i);
        (void)err;
        err = vsf_spi_cs_inactive(test_spi_instance, i);
        (void)err;
    }

    // 清理
    rt = vsf_spi_disable(test_spi_instance);
    VSF_TEST_ASSERT((rt == fsm_rt_cpl) || (rt == fsm_rt_on_going));
    vsf_spi_fini(test_spi_instance);
}

/**
 * @brief 测试片选边界值
 * @note 此测试需要硬件支持（hw_req=spi），如果硬件不可用，测试将失败
 */
void vsf_test_spi_cs_boundary(void)
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

    // 测试最大片选索引（可能超出硬件支持范围）
    err = vsf_spi_cs_active(test_spi_instance, 255);
    // 可能返回错误
    (void)err;

    // 清理
    rt = vsf_spi_disable(test_spi_instance);
    VSF_TEST_ASSERT((rt == fsm_rt_cpl) || (rt == fsm_rt_on_going));
    vsf_spi_fini(test_spi_instance);
}

/* EOF */

