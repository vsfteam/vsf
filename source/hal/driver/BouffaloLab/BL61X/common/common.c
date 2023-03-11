#define __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
#include "../__device.h"
#include "service/simple_stream/vsf_simple_stream.h"
#include "service/heap/vsf_heap.h"

#include "mem.h"
#include "bflb_uart.h"

/*----------------------------------------------------------------------------*
 * debug stream                                                               *
 *----------------------------------------------------------------------------*/

struct bflb_device_s *console;
static void (*__console_on_rx)(char ch);

void console_receive_isr(int irq, void *arg)
{
    uint32_t intstatus = bflb_uart_get_intstatus(console);
    char ch;

    if (intstatus & UART_INTSTS_RX_FIFO) {
        goto console_rx;
    }

    if (intstatus & UART_INTSTS_RTO) {
        bflb_uart_int_clear(console, UART_INTCLR_RTO);
        goto console_rx;
    }

    return;

console_rx:
    while (bflb_uart_rxavailable(console)) {
        ch = bflb_uart_getchar(console);
        if (__console_on_rx != NULL) {
            __console_on_rx(ch);
        }
    }
}
void bflb_uart_set_console(struct bflb_device_s *dev)
{
    console = dev;
    bflb_uart_rxint_mask(console, false);
    bflb_uart_txint_mask(console, true);
    bflb_irq_attach(console->irq_num, console_receive_isr, console);
    bflb_irq_enable(console->irq_num);
}

#define VSF_DEBUG_STREAM_CFG_RX_BUF_SIZE        32
static uint8_t __vsf_debug_stream_rx_buff[VSF_DEBUG_STREAM_CFG_RX_BUF_SIZE];
vsf_mem_stream_t VSF_DEBUG_STREAM_RX = {
    .op         = &vsf_mem_stream_op,
    .buffer     = __vsf_debug_stream_rx_buff,
    .size       = sizeof(__vsf_debug_stream_rx_buff),
};

static void __vsf_debug_stream_on_rx(char ch)
{
    VSF_STREAM_WRITE(&VSF_DEBUG_STREAM_RX, (uint8_t *)&ch, 1);
}

static void __VSF_DEBUG_STREAM_TX_INIT(void)
{
    vsf_stream_connect_tx(&VSF_DEBUG_STREAM_RX.use_as__vsf_stream_t);
    __console_on_rx = __vsf_debug_stream_on_rx;
}

static void __VSF_DEBUG_STREAM_TX_WRITE_BLOCKED(uint8_t *buf, uint_fast32_t size)
{
    extern struct bflb_device_s *console;
    VSF_ASSERT(console != NULL);
    bflb_uart_put(console, buf, size);
}

#include "hal/driver/common/debug_stream/debug_stream_tx_blocked.inc"

/*----------------------------------------------------------------------------*
 * Heap Implementation                                                        *
 *----------------------------------------------------------------------------*/

#if VSF_ARCH_PROVIDE_HEAP == ENABLED
void * vsf_arch_heap_malloc(uint_fast32_t size, uint_fast32_t alignment)
{
    return memalign(alignment, size);
}

void * vsf_arch_heap_realloc(void *buffer, uint_fast32_t size)
{
    return realloc(buffer, size);
}

void vsf_arch_heap_free(void *buffer)
{
    free(buffer);
}

unsigned int vsf_arch_heap_alignment(void)
{
    return 4;
}

uint_fast32_t vsf_arch_heap_size(void *buffer)
{
    return tlsf_block_size(buffer);
}

#   if VSF_ARCH_HEAP_HAS_STATISTICS == ENABLED
void vsf_arch_heap_statistics(vsf_arch_heap_statistics_t *statistics)
{
    struct meminfo info;
    bflb_mem_usage(KMEM_HEAP, &info);

    statistics->all_size = info.total_size;
    statistics->used_size = info.used_size;
}
#   endif
#endif
