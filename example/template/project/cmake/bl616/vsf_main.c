#define __VSF_SIMPLE_STREAM_CLASS_INHERIT__
#include <unistd.h>
#include <fcntl.h>

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
