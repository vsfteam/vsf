#define __VSF_SIMPLE_STREAM_CLASS_INHERIT__
#include <unistd.h>
#include <fcntl.h>

// debug stream

#define VSF_DEBUG_STREAM_CFG_RX_BUF_SIZE        32
static uint8_t __vsf_debug_stream_rx_buff[VSF_DEBUG_STREAM_CFG_RX_BUF_SIZE];
vsf_mem_stream_t VSF_DEBUG_STREAM_RX = {
    .op         = &vsf_mem_stream_op,
    .buffer     = __vsf_debug_stream_rx_buff,
    .size       = sizeof(__vsf_debug_stream_rx_buff),
};

static void __vsf_debug_stream_on_rx(char ch)
{
    uint_fast32_t rx_free_size = VSF_STREAM_GET_FREE_SIZE(&VSF_DEBUG_STREAM_RX);
    VSF_STREAM_WRITE(&VSF_DEBUG_STREAM_RX, &ch, 1);
}

static void __VSF_DEBUG_STREAM_TX_INIT(void)
{
    vsf_stream_connect_tx(&VSF_DEBUG_STREAM_RX.use_as__vsf_stream_t);
    extern void (*console_on_rx)(char ch);
    console_on_rx = __vsf_debug_stream_on_rx;
}

static void __VSF_DEBUG_STREAM_TX_WRITE_BLOCKED(uint8_t *buf, uint_fast32_t size)
{
    extern struct bflb_device_s *console;
    VSF_ASSERT(console != NULL);
    bflb_uart_put(console, buf, size);
}

#include "hal/driver/common/debug_stream/debug_stream_tx_blocked.inc"

// debug stream end

int vsf_linux_create_fhs(void)
{
    // 0. devfs, busybox, etc
    vsf_linux_vfs_init();
    busybox_install();

    // 1. hardware driver related demo
    // 2. fs
    // 3. demos depends on fs after all fs mounted
    // 4. install executables
    return 0;
}

// TODO: SDL require that main need argc and argv
int VSF_USER_ENTRY(int argc, char *argv[])
{
    VSF_STREAM_INIT(&VSF_DEBUG_STREAM_RX);
    VSF_STREAM_INIT(&VSF_DEBUG_STREAM_TX);

#if VSF_USE_TRACE == ENABLED
    vsf_start_trace();
#endif
    vsf_trace_info("start linux..." VSF_TRACE_CFG_LINEEND);

    vsf_stream_t *stream_tx, *stream_rx;
    stream_tx = (vsf_stream_t *)&VSF_DEBUG_STREAM_TX;
    stream_rx = (vsf_stream_t *)&VSF_DEBUG_STREAM_RX;

    vsf_linux_stdio_stream_t stream = {
        .in     = stream_rx,
        .out    = stream_tx,
        .err    = stream_tx,
    };
    vsf_err_t err = vsf_linux_init(&stream);
    VSF_UNUSED_PARAM(err);
    VSF_ASSERT(VSF_ERR_NONE == err);
    return 0;
}
