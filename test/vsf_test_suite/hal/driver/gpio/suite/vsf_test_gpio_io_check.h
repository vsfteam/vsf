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

#ifndef __TEST_GPIO_IO_CHECK_H__
#define __TEST_GPIO_IO_CHECK_H__

#include "vsf.h"
#   include "component/test/vsf_test/vsf_test.h"
#   include "test_params_generated.h"

#if VSF_TEST_GPIO_IO_CHECK_ENABLE == ENABLED

#ifndef VSF_TEST_GPIO_IO_CHECK_CASE_COUNT
#   define VSF_TEST_GPIO_IO_CHECK_CASE_COUNT      1
#endif

#ifndef VSF_TEST_GPIO_IO_CHECK_ROUNDS
#   define VSF_TEST_GPIO_IO_CHECK_ROUNDS          3
#endif

#ifndef VSF_TEST_GPIO_IO_CHECK_ROUND_GAP_US
#   define VSF_TEST_GPIO_IO_CHECK_ROUND_GAP_US    1000
#endif

#ifdef __cplusplus
extern "C" {
#endif
#ifndef VSF_TEST_GPIO_IO_CHECK_PATTERN_BYTE
#   define VSF_TEST_GPIO_IO_CHECK_PATTERN_BYTE                0x50
#endif


void vsf_test_gpio_io_check_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst);

typedef struct {
    uint8_t  idx;
    uint8_t  pin;
    uint32_t baudrate;
} vsf_test_gpio_io_check_params_t;

struct vsf_test_gpio_io_check_s;
extern const struct vsf_test_gpio_io_check_s vsf_test_gpio_io_check;

#ifdef __cplusplus
}
#endif

#endif

#endif /* __TEST_GPIO_IO_CHECK_H__ */
/* EOF */
