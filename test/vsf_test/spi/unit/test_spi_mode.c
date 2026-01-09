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
#include "test_spi_mode.h"

/*============================ MACROS ========================================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/**
 * @brief 测试 vsf_spi_mode_to_data_bits() 基本功能
 */
void vsf_test_spi_mode_to_data_bits_basic(void)
{
    uint8_t bits;
    vsf_spi_mode_t mode;

    // 测试 8 位数据
    mode = VSF_SPI_MASTER | VSF_SPI_MODE_0 | VSF_SPI_DATASIZE_8;
    bits = vsf_spi_mode_to_data_bits(mode);
    VSF_TEST_ASSERT(bits == 8);

    // 测试 16 位数据
    mode = VSF_SPI_MASTER | VSF_SPI_MODE_0 | VSF_SPI_DATASIZE_16;
    bits = vsf_spi_mode_to_data_bits(mode);
    VSF_TEST_ASSERT(bits == 16);

    // 测试 32 位数据
    mode = VSF_SPI_MASTER | VSF_SPI_MODE_0 | VSF_SPI_DATASIZE_32;
    bits = vsf_spi_mode_to_data_bits(mode);
    VSF_TEST_ASSERT(bits == 32);
}

/**
 * @brief 测试 vsf_spi_mode_to_data_bits() 不同 SPI 模式
 */
void vsf_test_spi_mode_to_data_bits_different_modes(void)
{
    uint8_t bits;
    vsf_spi_mode_t modes[] = {
        VSF_SPI_MASTER | VSF_SPI_MODE_0 | VSF_SPI_DATASIZE_8,
        VSF_SPI_MASTER | VSF_SPI_MODE_1 | VSF_SPI_DATASIZE_8,
        VSF_SPI_MASTER | VSF_SPI_MODE_2 | VSF_SPI_DATASIZE_8,
        VSF_SPI_MASTER | VSF_SPI_MODE_3 | VSF_SPI_DATASIZE_8,
    };
    int i;

    for (i = 0; i < dimof(modes); i++) {
        bits = vsf_spi_mode_to_data_bits(modes[i]);
        VSF_TEST_ASSERT(bits == 8);
    }
}

/**
 * @brief 测试 vsf_spi_mode_to_data_bits() 边界值
 */
void vsf_test_spi_mode_to_data_bits_boundary(void)
{
    uint8_t bits;
    vsf_spi_mode_t mode;

    // 测试最小数据位宽（8 位）
    mode = VSF_SPI_MASTER | VSF_SPI_MODE_0 | VSF_SPI_DATASIZE_8;
    bits = vsf_spi_mode_to_data_bits(mode);
    VSF_TEST_ASSERT(bits >= 8);

    // 测试最大数据位宽（32 位）
    mode = VSF_SPI_MASTER | VSF_SPI_MODE_0 | VSF_SPI_DATASIZE_32;
    bits = vsf_spi_mode_to_data_bits(mode);
    VSF_TEST_ASSERT(bits <= 32);
}

/**
 * @brief 测试 vsf_spi_data_bits_to_mode() 基本功能
 */
void vsf_test_spi_data_bits_to_mode_basic(void)
{
    vsf_spi_mode_t mode;
    uint8_t bits;

    // 测试 8 位
    mode = vsf_spi_data_bits_to_mode(8);
    bits = vsf_spi_mode_to_data_bits(mode);
    VSF_TEST_ASSERT(bits == 8);

    // 测试 16 位
    mode = vsf_spi_data_bits_to_mode(16);
    bits = vsf_spi_mode_to_data_bits(mode);
    VSF_TEST_ASSERT(bits == 16);

    // 测试 32 位
    mode = vsf_spi_data_bits_to_mode(32);
    bits = vsf_spi_mode_to_data_bits(mode);
    VSF_TEST_ASSERT(bits == 32);
}

/**
 * @brief 测试 vsf_spi_data_bits_to_mode() 往返转换
 */
void vsf_test_spi_data_bits_to_mode_roundtrip(void)
{
    vsf_spi_mode_t mode_original, mode_converted;
    uint8_t bits_original, bits_converted;
    uint8_t test_bits[] = {8, 16, 32};
    int i;

    for (i = 0; i < dimof(test_bits); i++) {
        // 位宽 -> 模式 -> 位宽
        mode_original = vsf_spi_data_bits_to_mode(test_bits[i]);
        bits_converted = vsf_spi_mode_to_data_bits(mode_original);
        VSF_TEST_ASSERT(bits_converted == test_bits[i]);

        // 模式 -> 位宽 -> 模式（需要保留其他位）
        mode_original = VSF_SPI_MASTER | VSF_SPI_MODE_0 | VSF_SPI_DATASIZE_8;
        bits_original = vsf_spi_mode_to_data_bits(mode_original);
        mode_converted = vsf_spi_data_bits_to_mode(bits_original);
        // 注意：data_bits_to_mode 可能只设置数据位部分，不保留其他位
        bits_converted = vsf_spi_mode_to_data_bits(mode_converted);
        VSF_TEST_ASSERT(bits_converted == bits_original);
    }
}

/**
 * @brief 测试 vsf_spi_data_bits_to_mode() 无效输入
 */
void vsf_test_spi_data_bits_to_mode_invalid(void)
{
    vsf_spi_mode_t mode;
    uint8_t bits;

    // 测试无效位宽（小于 8）
    mode = vsf_spi_data_bits_to_mode(0);
    bits = vsf_spi_mode_to_data_bits(mode);
    VSF_UNUSED_PARAM(bits);
    // 可能返回 0 或默认值，这里只检查不会崩溃

    // 测试无效位宽（大于 32）
    mode = vsf_spi_data_bits_to_mode(64);
    bits = vsf_spi_mode_to_data_bits(mode);
    VSF_UNUSED_PARAM(bits);
    // 可能返回 0 或默认值，这里只检查不会崩溃
}

/**
 * @brief 测试模式枚举值验证
 */
void vsf_test_spi_mode_enum_values(void)
{
    // 验证标准 SPI 模式值（使用位操作，因为 CPOL/CPHA 宏可能未定义）
    // MODE_0: CPOL=0, CPHA=0 → bits 2-3 = 0x00
    VSF_TEST_ASSERT((VSF_SPI_MODE_0 & 0x0C) == 0x00);
    // MODE_1: CPOL=0, CPHA=1 → bits 2-3 = 0x08 (bit 3 set)
    VSF_TEST_ASSERT((VSF_SPI_MODE_1 & 0x0C) == 0x08);
    // MODE_2: CPOL=1, CPHA=0 → bits 2-3 = 0x04 (bit 2 set)
    VSF_TEST_ASSERT((VSF_SPI_MODE_2 & 0x0C) == 0x04);
    // MODE_3: CPOL=1, CPHA=1 → bits 2-3 = 0x0C (bits 2-3 set)
    VSF_TEST_ASSERT((VSF_SPI_MODE_3 & 0x0C) == 0x0C);

    // 验证模式值互不相同
    VSF_TEST_ASSERT(VSF_SPI_MODE_0 != VSF_SPI_MODE_1);
    VSF_TEST_ASSERT(VSF_SPI_MODE_1 != VSF_SPI_MODE_2);
    VSF_TEST_ASSERT(VSF_SPI_MODE_2 != VSF_SPI_MODE_3);
    VSF_TEST_ASSERT(VSF_SPI_MODE_0 != VSF_SPI_MODE_2);
    VSF_TEST_ASSERT(VSF_SPI_MODE_0 != VSF_SPI_MODE_3);
    VSF_TEST_ASSERT(VSF_SPI_MODE_1 != VSF_SPI_MODE_3);

    // 验证主从模式
    VSF_TEST_ASSERT((VSF_SPI_MASTER & VSF_SPI_SLAVE) == 0);

    // 验证位顺序
    VSF_TEST_ASSERT((VSF_SPI_MSB_FIRST & VSF_SPI_LSB_FIRST) == 0);

    // 验证数据位宽
    VSF_TEST_ASSERT(VSF_SPI_DATASIZE_8 != VSF_SPI_DATASIZE_16);
    VSF_TEST_ASSERT(VSF_SPI_DATASIZE_16 != VSF_SPI_DATASIZE_32);
    VSF_TEST_ASSERT(VSF_SPI_DATASIZE_8 != VSF_SPI_DATASIZE_32);
}

/**
 * @brief 测试模式组合
 */
void vsf_test_spi_mode_combinations(void)
{
    vsf_spi_mode_t mode;
    uint8_t bits;

    // 测试主模式 + MODE 0 + 8 位
    mode = VSF_SPI_MASTER | VSF_SPI_MODE_0 | VSF_SPI_DATASIZE_8;
    bits = vsf_spi_mode_to_data_bits(mode);
    VSF_TEST_ASSERT(bits == 8);

    // 测试主模式 + MODE 1 + 16 位
    mode = VSF_SPI_MASTER | VSF_SPI_MODE_1 | VSF_SPI_DATASIZE_16;
    bits = vsf_spi_mode_to_data_bits(mode);
    VSF_TEST_ASSERT(bits == 16);

    // 测试主模式 + MODE 2 + 32 位
    mode = VSF_SPI_MASTER | VSF_SPI_MODE_2 | VSF_SPI_DATASIZE_32;
    bits = vsf_spi_mode_to_data_bits(mode);
    VSF_TEST_ASSERT(bits == 32);

    // 测试从模式 + MODE 3 + 8 位
    mode = VSF_SPI_SLAVE | VSF_SPI_MODE_3 | VSF_SPI_DATASIZE_8;
    bits = vsf_spi_mode_to_data_bits(mode);
    VSF_TEST_ASSERT(bits == 8);
}

/* EOF */

