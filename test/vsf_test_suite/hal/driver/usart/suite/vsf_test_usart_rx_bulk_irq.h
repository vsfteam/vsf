#ifndef __VSF_TEST_USART_RX_BULK_IRQ_H__
#define __VSF_TEST_USART_RX_BULK_IRQ_H__

#include "vsf.h"
#   include "component/test/vsf_test/vsf_test.h"
#   include "test_params_generated.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_USART_RX_BULK_IRQ_BUF_SIZE
#   define VSF_TEST_USART_RX_BULK_IRQ_BUF_SIZE        4096
#endif
#ifndef VSF_TEST_USART_RX_BULK_IRQ_PRIO
#   define VSF_TEST_USART_RX_BULK_IRQ_PRIO          vsf_arch_prio_highest
#endif


/*============================ TYPES =========================================*/

#if VSF_TEST_USART_RX_BULK_IRQ_ENABLE == ENABLED
typedef struct {
    uint8_t rx_bulk_irq_buf[VSF_TEST_USART_RX_BULK_IRQ_BUF_SIZE];
    volatile bool done;
    uint8_t *dst;
    volatile uint32_t isr_count;
    uint_fast16_t received;
    uint_fast16_t target;
} vsf_test_usart_rx_bulk_irq_data_t;
#endif

#ifndef VSF_TEST_USART_RX_BULK_IRQ_BUSY_WAIT_MS
#   define VSF_TEST_USART_RX_BULK_IRQ_BUSY_WAIT_MS            10
#endif

#ifndef VSF_TEST_RX_BULK_IRQ_DEFAULT_BAUDRATE
#   define VSF_TEST_RX_BULK_IRQ_DEFAULT_BAUDRATE                   115200
#endif

/*============================ PROTOTYPES ====================================*/
#define VSF_TEST_USART_RX_BULK_IRQ_MIN_TIMEOUT_MS     1000

void vsf_test_usart_rx_bulk_irq_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const vsf_test_inst_t *inst);

typedef struct {
    uint8_t  idx;
    uint32_t data_size_bytes;
} vsf_test_usart_rx_bulk_irq_params_t;

struct vsf_test_usart_rx_bulk_irq_s;
extern const struct vsf_test_usart_rx_bulk_irq_s vsf_test_usart_rx_bulk_irq;

#ifdef __cplusplus
}
#endif

#endif /* __VSF_TEST_USART_RX_BULK_IRQ_H__ */
/* EOF */
