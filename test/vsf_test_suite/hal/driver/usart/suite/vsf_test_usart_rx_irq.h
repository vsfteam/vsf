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

#ifndef __TEST_USART_RX_IRQ_H__
#define __TEST_USART_RX_IRQ_H__

#include "vsf.h"
#   include "component/test/vsf_test/vsf_test.h"
#   include "test_params_generated.h"
#ifndef VSF_TEST_USART_RX_IRQ_JUNK_SIZE
#   define VSF_TEST_USART_RX_IRQ_JUNK_SIZE        16
#endif

#if VSF_TEST_USART_RX_IRQ_ENABLE == ENABLED

#ifdef __cplusplus
extern "C" {
#endif

#ifndef VSF_TEST_USART_RX_IRQ_PRIO
#   define VSF_TEST_USART_RX_IRQ_PRIO               vsf_arch_prio_1
#endif

#ifndef VSF_TEST_USART_RX_IRQ_BUSY_WAIT_MS
#   define VSF_TEST_USART_RX_IRQ_BUSY_WAIT_MS                 10
#endif

#ifndef VSF_TEST_RX_IRQ_PAYLOAD
#   define VSF_TEST_RX_IRQ_PAYLOAD                                 "Hello VSF\r\n"
#endif

#ifndef VSF_TEST_RX_IRQ_PAYLOAD_DRAIN_MS
#   define VSF_TEST_RX_IRQ_PAYLOAD_DRAIN_MS                        500
#endif

#ifndef VSF_TEST_RX_IRQ_DEFAULT_MODE
#   define VSF_TEST_RX_IRQ_DEFAULT_MODE                            (VSF_USART_NO_PARITY | VSF_USART_1_STOPBIT | VSF_USART_8_BIT_LENGTH | VSF_USART_RX_ENABLE)
#endif

#ifndef VSF_TEST_RX_IRQ_DEFAULT_BAUDRATE
#   define VSF_TEST_RX_IRQ_DEFAULT_BAUDRATE                        115200
#endif

#ifndef VSF_TEST_RX_IRQ_PRIO
#   define VSF_TEST_RX_IRQ_PRIO                                    VSF_TEST_USART_RX_IRQ_PRIO
#endif

/*============================ PROTOTYPES ====================================*/
#define VSF_TEST_USART_RX_IRQ_BUF_SIZE                32

void vsf_test_usart_rx_irq_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst);

typedef struct {
    uint8_t  idx;
    bool     expect_pass;
} vsf_test_usart_rx_irq_params_t;

struct vsf_test_usart_rx_irq_s;
extern const struct vsf_test_usart_rx_irq_s vsf_test_usart_rx_irq;

#ifdef __cplusplus
}
#endif

#endif /* VSF_TEST_USART_RX_IRQ_ENABLE == ENABLED */

#endif /* __TEST_USART_RX_IRQ_H__ */
/* EOF */
