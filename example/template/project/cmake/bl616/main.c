#define __BouffaloLab__
#define __BL616__

#include <FreeRTOS.h>
#include "board.h"

#include <vsf.h>

int main(void)
{
    // initialize debug stream before board_init,
    //  because board_init will call printf, which depends on debug_stream by default
    VSF_STREAM_INIT(&VSF_DEBUG_STREAM_RX);
    VSF_STREAM_INIT(&VSF_DEBUG_STREAM_TX);
#if VSF_USE_TRACE == ENABLED
    vsf_start_trace();
#endif

    board_init();

    extern void vsf_freertos_start(void);
    vsf_freertos_start();

    vTaskStartScheduler();
    while (1) {
    }
}
