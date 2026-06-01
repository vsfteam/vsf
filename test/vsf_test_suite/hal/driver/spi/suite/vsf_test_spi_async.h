/******************************************************************************
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
 *****************************************************************************/

#ifndef __VSF_TEST_SPI_ASYNC_H__
#define __VSF_TEST_SPI_ASYNC_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"
#   include "component/test/vsf_test/vsf_test.h"
#   include "test_params_generated.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_SPI_ASYNC_CASE_COUNT
#   define VSF_TEST_SPI_ASYNC_CASE_COUNT     1
#endif
#ifndef VSF_TEST_SPI_ASYNC_PATTERN_BYTE
#   define VSF_TEST_SPI_ASYNC_PATTERN_BYTE                    0xA5
#endif

#ifndef VSF_TEST_SPI_ASYNC_SPIN_COUNT
#   define VSF_TEST_SPI_ASYNC_SPIN_COUNT                      100000
#endif


#ifndef VSF_TEST_SPI_ASYNC_MAX_DATA_LEN
#   define VSF_TEST_SPI_ASYNC_MAX_DATA_LEN      256
#endif

/*============================ TYPES =========================================*/

#if VSF_TEST_SPI_ASYNC_ENABLE == ENABLED
typedef struct {
    uint8_t spi_async_tx_buf[VSF_TEST_SPI_ASYNC_MAX_DATA_LEN];
    uint8_t spi_async_rx_buf[VSF_TEST_SPI_ASYNC_MAX_DATA_LEN];
} vsf_test_spi_async_data_t;
#endif

/*============================ PROTOTYPES ====================================*/

void vsf_test_spi_async_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst);

typedef struct {
    uint8_t  idx;
    uint32_t mode;
    uint32_t clock_hz;
    uint16_t data_len;
    uint8_t  test_type;
} vsf_test_spi_async_params_t;

struct vsf_test_spi_async_s;
extern const struct vsf_test_spi_async_s vsf_test_spi_async;

#endif /* __VSF_TEST_SPI_ASYNC_H__ */
/* EOF */
