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

#include "component/test/vsf_test/vsf_test.h"
#include "test_spi.h"
#include "unit/test_spi_init.h"
#include "unit/test_spi_config.h"
#include "unit/test_spi_capability.h"
#include "unit/test_spi_mode.h"
#include "functional/test_spi_enable_disable.h"
#include "functional/test_spi_status.h"
#include "functional/test_spi_cs.h"
#include "functional/test_spi_irq.h"
#include "functional/test_spi_transfer.h"
#include "functional/test_spi_fifo.h"
#include "functional/test_spi_ctrl.h"
#include "integration/test_spi_loopback.h"

/*============================ MACROS ========================================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

//! \brief 测试使用的 SPI 实例
vsf_spi_t *test_spi_instance = NULL;

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/**
 * @brief 初始化 SPI 测试并添加测试用例
 * @param cfg SPI 测试配置，包含 SPI 实例指针
 */
void vsf_test_spi_init(const vsf_test_spi_cfg_t *cfg)
{
    VSF_ASSERT(cfg != NULL);

    // 设置 SPI 测试实例
    test_spi_instance = cfg->spi_instance;

    // 使用 vsf_test_add 和 vsf_test_add_ex 添加测试用例
    // 初始化测试
    vsf_test_add_expect_assert_case(vsf_test_spi_init_null_ptr,
                                    "spi_init_null_ptr purpose=config hw_req=none", 0);
    vsf_test_add_simple_case( vsf_test_spi_init_invalid_mode,
                 "spi_init_invalid_mode purpose=config hw_req=none");
    vsf_test_add_simple_case( vsf_test_spi_init_invalid_clock,
                 "spi_init_invalid_clock purpose=config hw_req=none");
    vsf_test_add_simple_case( vsf_test_spi_init_valid,
                 "spi_init_valid purpose=config hw_req=spi");
    vsf_test_add_simple_case( vsf_test_spi_init_repeat,
                 "spi_init_repeat purpose=config hw_req=spi");
    vsf_test_add_expect_assert_case(vsf_test_spi_fini_null_ptr,
                                    "spi_fini_null_ptr purpose=config hw_req=none", 0);
    vsf_test_add_simple_case( vsf_test_spi_fini_normal,
                 "spi_fini_normal purpose=config hw_req=spi");

    // 配置测试
    vsf_test_add_expect_assert_case(vsf_test_spi_get_config_null_ptr,
                                    "spi_get_config_null_ptr purpose=config hw_req=none", 0);
    vsf_test_add_simple_case( vsf_test_spi_get_config_normal,
                 "spi_get_config_normal purpose=config hw_req=spi");
    vsf_test_add_simple_case( vsf_test_spi_get_config_different_modes,
                 "spi_get_config_different_modes purpose=config hw_req=spi");
    vsf_test_add_simple_case( vsf_test_spi_get_config_different_data_sizes,
                 "spi_get_config_different_data_sizes purpose=config hw_req=spi");
    vsf_test_add_simple_case( vsf_test_spi_get_config_different_clocks,
                 "spi_get_config_different_clocks purpose=config hw_req=spi");
    vsf_test_add_simple_case( vsf_test_spi_get_config_boundary,
                 "spi_get_config_boundary purpose=config hw_req=spi");

    // 能力查询测试
    vsf_test_add_expect_assert_case(vsf_test_spi_capability_null_ptr,
                                    "spi_capability_null_ptr purpose=config hw_req=none", 0);
    vsf_test_add_simple_case( vsf_test_spi_capability_basic,
                 "spi_capability_basic purpose=config hw_req=spi");
    vsf_test_add_simple_case( vsf_test_spi_capability_consistency,
                 "spi_capability_consistency purpose=config hw_req=spi");
    vsf_test_add_simple_case( vsf_test_spi_capability_multi_instance,
                 "spi_capability_multi_instance purpose=config hw_req=spi");
    vsf_test_add_simple_case( vsf_test_spi_capability_flags,
                 "spi_capability_flags purpose=config hw_req=spi");
    vsf_test_add_simple_case( vsf_test_spi_capability_before_after_init,
                 "spi_capability_before_after_init purpose=config hw_req=spi");

    // 模式转换测试
    vsf_test_add_simple_case( vsf_test_spi_mode_to_data_bits_basic,
                 "spi_mode_to_data_bits_basic purpose=config hw_req=none");
    vsf_test_add_simple_case( vsf_test_spi_mode_to_data_bits_different_modes,
                 "spi_mode_to_data_bits_different_modes purpose=config hw_req=none");
    vsf_test_add_simple_case( vsf_test_spi_mode_to_data_bits_boundary,
                 "spi_mode_to_data_bits_boundary purpose=config hw_req=none");
    vsf_test_add_simple_case( vsf_test_spi_data_bits_to_mode_basic,
                 "spi_data_bits_to_mode_basic purpose=config hw_req=none");
    vsf_test_add_simple_case( vsf_test_spi_data_bits_to_mode_roundtrip,
                 "spi_data_bits_to_mode_roundtrip purpose=config hw_req=none");
    vsf_test_add_simple_case( vsf_test_spi_data_bits_to_mode_invalid,
                 "spi_data_bits_to_mode_invalid purpose=config hw_req=none");
    vsf_test_add_simple_case( vsf_test_spi_mode_enum_values,
                 "spi_mode_enum_values purpose=config hw_req=none");
    vsf_test_add_simple_case( vsf_test_spi_mode_combinations,
                 "spi_mode_combinations purpose=config hw_req=none");

    // 使能/禁用测试
    vsf_test_add_simple_case( vsf_test_spi_enable_null_ptr,
                 "spi_enable_null_ptr purpose=config hw_req=none");
    vsf_test_add_simple_case( vsf_test_spi_disable_null_ptr,
                 "spi_disable_null_ptr purpose=config hw_req=none");
    vsf_test_add_simple_case( vsf_test_spi_enable_basic,
                 "spi_enable_basic purpose=config hw_req=spi");
    vsf_test_add_simple_case( vsf_test_spi_disable_basic,
                 "spi_disable_basic purpose=config hw_req=spi");
    vsf_test_add_simple_case( vsf_test_spi_enable_disable_repeat,
                 "spi_enable_disable_repeat purpose=config hw_req=spi");
    vsf_test_add_simple_case( vsf_test_spi_enable_disable_before_init,
                 "spi_enable_disable_before_init purpose=config hw_req=spi");

    // 状态查询测试
    vsf_test_add_simple_case( vsf_test_spi_status_null_ptr,
                 "spi_status_null_ptr purpose=config hw_req=none");
    vsf_test_add_simple_case( vsf_test_spi_status_basic,
                 "spi_status_basic purpose=config hw_req=spi");
    vsf_test_add_simple_case( vsf_test_spi_status_consistency,
                 "spi_status_consistency purpose=config hw_req=spi");

    // 片选控制测试
    vsf_test_add_expect_assert_case(vsf_test_spi_cs_active_null_ptr,
                                    "spi_cs_active_null_ptr purpose=config hw_req=none", 0);
    vsf_test_add_expect_assert_case(vsf_test_spi_cs_inactive_null_ptr,
                                    "spi_cs_inactive_null_ptr purpose=config hw_req=none", 0);
    vsf_test_add_simple_case( vsf_test_spi_cs_active_basic,
                 "spi_cs_active_basic purpose=cs-timing hw_req=spi");
    vsf_test_add_simple_case( vsf_test_spi_cs_inactive_basic,
                 "spi_cs_inactive_basic purpose=cs-timing hw_req=spi");
    vsf_test_add_simple_case( vsf_test_spi_cs_switch,
                 "spi_cs_switch purpose=cs-timing hw_req=spi");
    vsf_test_add_simple_case( vsf_test_spi_cs_boundary,
                 "spi_cs_boundary purpose=cs-timing hw_req=spi");

    // 中断测试
    vsf_test_add_simple_case( vsf_test_spi_irq_enable_null_ptr,
                 "spi_irq_enable_null_ptr purpose=irq hw_req=none");
    vsf_test_add_simple_case( vsf_test_spi_irq_disable_null_ptr,
                 "spi_irq_disable_null_ptr purpose=irq hw_req=none");
    vsf_test_add_simple_case( vsf_test_spi_irq_clear_null_ptr,
                 "spi_irq_clear_null_ptr purpose=irq hw_req=none");
    vsf_test_add_simple_case( vsf_test_spi_irq_enable_basic,
                 "spi_irq_enable_basic purpose=irq hw_req=spi");
    vsf_test_add_simple_case( vsf_test_spi_irq_disable_basic,
                 "spi_irq_disable_basic purpose=irq hw_req=spi");
    vsf_test_add_simple_case( vsf_test_spi_irq_clear_basic,
                 "spi_irq_clear_basic purpose=irq hw_req=spi");
    vsf_test_add_simple_case( vsf_test_spi_irq_toggle,
                 "spi_irq_toggle purpose=irq hw_req=spi");
    vsf_test_add_simple_case( vsf_test_spi_irq_partial,
                 "spi_irq_partial purpose=irq hw_req=spi");

    // 传输测试
    vsf_test_add_expect_assert_case(vsf_test_spi_request_transfer_null_ptr,
                                    "spi_request_transfer_null_ptr purpose=data-path hw_req=none", 0);
    vsf_test_add_expect_assert_case(vsf_test_spi_cancel_transfer_null_ptr,
                                    "spi_cancel_transfer_null_ptr purpose=data-path hw_req=none", 0);
    vsf_test_add_simple_case( vsf_test_spi_get_transferred_count_null_ptr,
                 "spi_get_transferred_count_null_ptr purpose=data-path hw_req=none");
    vsf_test_add_simple_case( vsf_test_spi_request_transfer_basic,
                 "spi_request_transfer_basic purpose=data-path hw_req=spi+assist");
    vsf_test_add_simple_case( vsf_test_spi_cancel_transfer_basic,
                 "spi_cancel_transfer_basic purpose=data-path hw_req=spi+assist");
    vsf_test_add_simple_case( vsf_test_spi_get_transferred_count_basic,
                 "spi_get_transferred_count_basic purpose=data-path hw_req=spi");
    vsf_test_add_simple_case( vsf_test_spi_transfer_tx_only,
                 "spi_transfer_tx_only purpose=data-path hw_req=spi+assist");
    vsf_test_add_simple_case( vsf_test_spi_transfer_rx_only,
                 "spi_transfer_rx_only purpose=data-path hw_req=spi+assist");
    vsf_test_add_simple_case( vsf_test_spi_transfer_different_lengths,
                 "spi_transfer_different_lengths purpose=data-path hw_req=spi+assist");
    vsf_test_add_simple_case( vsf_test_spi_transfer_zero_length,
                 "spi_transfer_zero_length purpose=data-path hw_req=spi");
    vsf_test_add_simple_case( vsf_test_spi_request_polling_transfer,
                 "spi_request_polling_transfer purpose=data-path hw_req=spi+assist");
    vsf_test_add_simple_case( vsf_test_spi_request_irq_transfer,
                 "spi_request_irq_transfer purpose=data-path hw_req=spi+assist");

    // FIFO 传输测试
    vsf_test_add_simple_case( vsf_test_spi_fifo_transfer_null_ptr,
                 "spi_fifo_transfer_null_ptr purpose=data-path hw_req=none");
    vsf_test_add_simple_case( vsf_test_spi_fifo_transfer_basic,
                 "spi_fifo_transfer_basic purpose=data-path hw_req=spi+assist");
    vsf_test_add_simple_case( vsf_test_spi_fifo_transfer_offset,
                 "spi_fifo_transfer_offset purpose=data-path hw_req=spi+assist");
    vsf_test_add_simple_case( vsf_test_spi_fifo_polling_transfer,
                 "spi_fifo_polling_transfer purpose=data-path hw_req=spi+assist");
    vsf_test_add_simple_case( vsf_test_spi_fifo_irq_transfer,
                 "spi_fifo_irq_transfer purpose=data-path hw_req=spi+assist");

    // 控制命令测试
    vsf_test_add_expect_assert_case(vsf_test_spi_ctrl_null_ptr,
                                    "spi_ctrl_null_ptr purpose=config hw_req=none", 0);
    vsf_test_add_simple_case( vsf_test_spi_ctrl_basic,
                 "spi_ctrl_basic purpose=config hw_req=spi");
    vsf_test_add_simple_case( vsf_test_spi_ctrl_invalid_cmd,
                 "spi_ctrl_invalid_cmd purpose=config hw_req=spi");

    // 回环测试
    vsf_test_add_simple_case( vsf_test_spi_loopback_mode0_8bit,
                 "spi_loopback_mode0_8bit purpose=mode hw_req=spi+loopback");
    vsf_test_add_simple_case( vsf_test_spi_loopback_mode1_8bit,
                 "spi_loopback_mode1_8bit purpose=mode hw_req=spi+loopback");
    vsf_test_add_simple_case( vsf_test_spi_loopback_mode2_8bit,
                 "spi_loopback_mode2_8bit purpose=mode hw_req=spi+loopback");
    vsf_test_add_simple_case( vsf_test_spi_loopback_mode3_8bit,
                 "spi_loopback_mode3_8bit purpose=mode hw_req=spi+loopback");
    vsf_test_add_simple_case( vsf_test_spi_loopback_16bit,
                 "spi_loopback_16bit purpose=mode hw_req=spi+loopback");
    vsf_test_add_simple_case( vsf_test_spi_loopback_32bit,
                 "spi_loopback_32bit purpose=mode hw_req=spi+loopback");
    vsf_test_add_simple_case( vsf_test_spi_loopback_large_data,
                 "spi_loopback_large_data purpose=mode hw_req=spi+loopback");
}

/* EOF */
