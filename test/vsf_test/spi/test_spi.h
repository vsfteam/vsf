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
 *  limitations under the License.                                          *
 *                                                                           *
 ****************************************************************************/

#ifndef __TEST_SPI_H__
#define __TEST_SPI_H__

/*============================ INCLUDES ======================================*/

#   include "vsf.h"
#   include "component/test/vsf_test/vsf_test.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ TYPES =========================================*/

//! \brief 默认 SPI 配置（用于初始化）
#define TEST_SPI_DEFAULT_CFG                                                      \
    {                                                                              \
        .mode = VSF_SPI_MASTER | VSF_SPI_MODE_0 | VSF_SPI_DATASIZE_8,            \
        .clock_hz = 1000000,                                                      \
    }

//! \brief 获取默认 SPI 配置的辅助函数
static inline vsf_spi_cfg_t test_spi_get_default_cfg(void)
{
    return (vsf_spi_cfg_t)TEST_SPI_DEFAULT_CFG;
}

//! \brief SPI 测试配置结构
typedef struct vsf_test_spi_cfg_t {
    //! \brief SPI 实例指针，用于测试
    vsf_spi_t *spi_instance;
} vsf_test_spi_cfg_t;

/*============================ GLOBAL VARIABLES ==============================*/

//! \brief 测试使用的 SPI 实例（由测试主函数设置）
extern vsf_spi_t *test_spi_instance;

/*============================ PROTOTYPES ====================================*/

/**
 * @brief 初始化 SPI 测试并添加测试用例
 * @param cfg SPI 测试配置，包含 SPI 实例指针
 */
void vsf_test_spi_init(const vsf_test_spi_cfg_t *cfg);

#ifdef __cplusplus
}
#endif

#endif /* __TEST_SPI_H__ */
/* EOF */
