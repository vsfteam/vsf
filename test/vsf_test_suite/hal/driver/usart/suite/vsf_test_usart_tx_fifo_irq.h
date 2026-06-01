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

#ifndef __TEST_USART_TX_FIFO_IRQ_H__
#define __TEST_USART_TX_FIFO_IRQ_H__

#include "vsf.h"
#   include "component/test/vsf_test/vsf_test.h"
#   include "test_params_generated.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_USART_TX_FIFO_IRQ_BUF_SIZE
#   define VSF_TEST_USART_TX_FIFO_IRQ_BUF_SIZE        256
#endif
#ifndef VSF_TEST_USART_TX_FIFO_IRQ_PRIO
#   define VSF_TEST_USART_TX_FIFO_IRQ_PRIO          vsf_arch_prio_highest
#endif


#ifndef VSF_TEST_USART_TX_FIFO_IRQ_POLL_MAX_ITER
#   define VSF_TEST_USART_TX_FIFO_IRQ_POLL_MAX_ITER   5000
#endif

/*============================ TYPES =========================================*/

#if VSF_TEST_USART_TX_FIFO_IRQ_ENABLE == ENABLED
typedef struct {
    volatile uint32_t isr_count;
    uint_fast16_t remaining;
    const uint8_t *src;
    volatile bool done;
} vsf_test_usart_tx_fifo_irq_data_t;
#endif

#ifndef VSF_TEST_USART_TX_FIFO_IRQ_DEFAULT_BAUDRATE
#   define VSF_TEST_USART_TX_FIFO_IRQ_DEFAULT_BAUDRATE             115200
#endif

/*============================ PROTOTYPES ====================================*/

void vsf_test_usart_tx_fifo_irq_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst);

typedef struct {
    uint8_t  idx;
    uint32_t refill_target;
} vsf_test_usart_tx_fifo_irq_params_t;

struct vsf_test_usart_tx_fifo_irq_s;
extern const struct vsf_test_usart_tx_fifo_irq_s vsf_test_usart_tx_fifo_irq;

#ifdef __cplusplus
}
#endif

#endif /* __TEST_USART_TX_FIFO_IRQ_H__ */
/* EOF */
