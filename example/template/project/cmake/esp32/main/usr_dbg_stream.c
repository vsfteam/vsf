#define __VSF_SIMPLE_STREAM_CLASS_INHERIT__
// need stack of vsf_arch_irq_thread_with_stack_t, which is private
#define __VSF_ARCH_RTOS_IMPLEMENT
#include "vsf.h"

static uint_fast32_t __usr_debug_stream_tx_write(vsf_stream_t* stream,
            uint8_t* buf, uint_fast32_t size)
{
    fwrite(buf, 1, size, stdout);
    fflush(stdout);
    return size;
}

static uint_fast32_t __usr_debug_stream_tx_get_data_length(vsf_stream_t* stream)
{
    return 0;
}

static uint_fast32_t __usr_debug_stream_tx_get_avail_length(vsf_stream_t* stream)
{
    return 0xFFFFFFFF;
}

static void __usr_debug_stream_rx_thread(void *param)
{
    char ch;
    size_t len;

    while (1) {
        len = fread(&ch, 1, 1, stdin);
        if (len > 0) {
            VSF_STREAM_WRITE(&VSF_DEBUG_STREAM_RX, (uint8_t *)&ch, 1);
        } else {
            __vsf_arch_delay_ms(10);
        }
    }
}

static void __usr_debug_stream_tx_init(vsf_stream_t *stream)
{
    static vsf_arch_irq_thread_with_stack_t thread;
    __vsf_arch_irq_thread_start(&thread.use_as__vsf_arch_irq_thread_t, "vsf_console",
        __usr_debug_stream_rx_thread, vsf_arch_prio_0, thread.stack, dimof(thread.stack));
}

static const vsf_stream_op_t __usr_debug_stream_tx_op = {
    .init               = __usr_debug_stream_tx_init,
    .get_data_length    = __usr_debug_stream_tx_get_data_length,
    .get_avail_length   = __usr_debug_stream_tx_get_avail_length,
    .write              = __usr_debug_stream_tx_write,
};
static uint8_t __usr_debug_stream_rx_buff[1024];

vsf_stream_t VSF_DEBUG_STREAM_TX = {
    .op         = &__usr_debug_stream_tx_op,
};

vsf_mem_stream_t VSF_DEBUG_STREAM_RX = {
    .op         = &vsf_mem_stream_op,
    .buffer     = __usr_debug_stream_rx_buff,
    .size       = sizeof(__usr_debug_stream_rx_buff),
};
