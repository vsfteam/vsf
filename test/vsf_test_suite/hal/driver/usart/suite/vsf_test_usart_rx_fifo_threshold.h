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

#ifndef __VSF_TEST_USART_RX_FIFO_THRESHOLD_H__
#define __VSF_TEST_USART_RX_FIFO_THRESHOLD_H__

#include "vsf.h"
#   include "component/test/vsf_test/vsf_test.h"
#   include "test_params_generated.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_USART_RX_FIFO_THRESHOLD_BUF_SIZE
#   define VSF_TEST_USART_RX_FIFO_THRESHOLD_BUF_SIZE        64
#endif
#ifndef VSF_TEST_USART_RX_FIFO_THRESHOLD_PRIO
#   define VSF_TEST_USART_RX_FIFO_THRESHOLD_PRIO    vsf_arch_prio_highest
#endif


#ifndef VSF_TEST_USART_RX_FIFO_THRESHOLD_JUNK_SIZE
#   define VSF_TEST_USART_RX_FIFO_THRESHOLD_JUNK_SIZE        16
#endif

/*============================ TYPES =========================================*/

#if VSF_TEST_USART_RX_FIFO_THRESHOLD_ENABLE == ENABLED
typedef struct {
    uint8_t rx_fifo_threshold_buf[VSF_TEST_USART_RX_FIFO_THRESHOLD_BUF_SIZE];
    volatile bool threshold_fired;
    volatile uint32_t bytes_at_threshold;
    volatile uint32_t isr_count;
    volatile bool done;
    uint8_t *dst;
    uint32_t target;
    volatile uint32_t received;
} vsf_test_usart_rx_fifo_threshold_data_t;
#endif

#ifndef VSF_TEST_RX_FIFO_THRESHOLD_DEFAULT_BAUDRATE
#   define VSF_TEST_RX_FIFO_THRESHOLD_DEFAULT_BAUDRATE             115200
#endif

/*============================ PROTOTYPES ====================================*/
#define VSF_TEST_USART_RX_FIFO_THRESHOLD_WAIT_MS      1000

void vsf_test_usart_rx_fifo_threshold_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst);

typedef struct {
    uint8_t          idx;
    vsf_usart_mode_t threshold_mode;
    uint32_t         expected_bytes;
} vsf_test_usart_rx_fifo_threshold_params_t;

struct vsf_test_usart_rx_fifo_threshold_s;
extern const struct vsf_test_usart_rx_fifo_threshold_s vsf_test_usart_rx_fifo_threshold;

#ifdef __cplusplus
}
#endif

#endif /* __VSF_TEST_USART_RX_FIFO_THRESHOLD_H__ */
/* EOF */
