#define __BouffaloLab__
#define __BL616__

#include <FreeRTOS.h>
#include "board.h"
#include "bflb_uart.h"

struct bflb_device_s *console;
void (*console_on_rx)(char ch);
void console_receive_isr(int irq, void *arg)
{
    unsigned int intstatus = bflb_uart_get_intstatus(console);
    char ch;

    if (intstatus & (UART_INTSTS_RX_FIFO | UART_INTSTS_RTO)) {
        while (bflb_uart_rxavailable(console)) {
            ch = bflb_uart_getchar(console);
            if (console_on_rx != NULL) {
                console_on_rx(ch);
            }
        }
    }
}
void bflb_uart_set_console(struct bflb_device_s *dev)
{
    console = dev;
    bflb_uart_rxint_mask(console, false);
    bflb_irq_attach(console->irq_num, console_receive_isr, console);
    bflb_irq_enable(console->irq_num);
}

int main(void)
{
    board_init();

    extern void vsf_freertos_start(void);
    vsf_freertos_start();

    vTaskStartScheduler();
    while (1) {
    }
}
