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

#ifndef __TEST_USART_BAUD_H__
#define __TEST_USART_BAUD_H__

#include "vsf.h"
#   include "component/test/vsf_test/vsf_test.h"
#   include "test_params_generated.h"

#if VSF_TEST_USART_TX_BAUD_ENABLE == ENABLED

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_BAUD_MIN_DRAIN_MS
#   define VSF_TEST_BAUD_MIN_DRAIN_MS             100
#endif

#ifndef VSF_TEST_BAUD_PAYLOAD
#   define VSF_TEST_BAUD_PAYLOAD                                   "Hello VSF\r\n"
#endif

#ifndef VSF_TEST_BAUD_PAYLOAD_DRAIN_MS
#   define VSF_TEST_BAUD_PAYLOAD_DRAIN_MS                          500
#endif

#ifndef VSF_TEST_BAUD_DEFAULT_MODE
#   define VSF_TEST_BAUD_DEFAULT_MODE                              (VSF_USART_NO_PARITY | VSF_USART_1_STOPBIT | VSF_USART_8_BIT_LENGTH | VSF_USART_TX_ENABLE)
#endif

/*============================ PROTOTYPES ====================================*/

void vsf_test_usart_baud_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst);

typedef struct {
    uint8_t  idx;
    uint32_t baudrate;
    uint32_t data_size_bytes;
    bool     expect_pass;
} vsf_test_usart_baud_params_t;

struct vsf_test_usart_baud_s;
extern const struct vsf_test_usart_baud_s vsf_test_usart_baud;

#ifdef __cplusplus
}
#endif

#endif /* VSF_TEST_USART_TX_BAUD_ENABLE == ENABLED */

#endif /* __TEST_USART_BAUD_H__ */
/* EOF */
