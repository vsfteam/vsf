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

#ifndef __VSF_TEST_SPI_LOOPBACK_H__
#define __VSF_TEST_SPI_LOOPBACK_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"
#   include "component/test/vsf_test/vsf_test.h"
#   include "test_params_generated.h"
#ifndef VSF_TEST_SPI_LOOPBACK_MAX_DATA_LEN
#   define VSF_TEST_SPI_LOOPBACK_MAX_DATA_LEN        256
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_SPI_LOOPBACK_CASE_COUNT
#   define VSF_TEST_SPI_LOOPBACK_CASE_COUNT     1
#endif
#ifndef VSF_TEST_SPI_LOOPBACK_PATTERN_BYTE
#   define VSF_TEST_SPI_LOOPBACK_PATTERN_BYTE                 0xA5
#endif

#ifndef VSF_TEST_SPI_LOOPBACK_PRIO
#   define VSF_TEST_SPI_LOOPBACK_PRIO               vsf_arch_prio_1
#endif


/*============================ PROTOTYPES ====================================*/

void vsf_test_spi_loopback_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst);

typedef struct {
    uint8_t  idx;
    uint32_t mode;
    uint32_t clock_hz;
    uint16_t data_len;
} vsf_test_spi_loopback_params_t;

struct vsf_test_spi_loopback_s;
extern const struct vsf_test_spi_loopback_s vsf_test_spi_loopback;

#endif /* __VSF_TEST_SPI_LOOPBACK_H__ */
/* EOF */
