#define __BouffaloLab__
#define __BL616__

#include <FreeRTOS.h>
#include "board.h"

int main(void)
{
    board_init();

    extern void vsf_freertos_start(void);
    vsf_freertos_start();

    vTaskStartScheduler();
    while (1) {
    }
}
