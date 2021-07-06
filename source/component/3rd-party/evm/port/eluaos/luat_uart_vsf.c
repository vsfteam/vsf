#include "vsf.h"

#if VSF_USE_EVM == ENABLED && VSF_EVM_USE_LUA == ENABLED

#include "luat_base.h"
#include "luat_msgbus.h"
#include "luat_uart.h"

int luat_uart_exist(int uartid) {
    return uartid >= 0;
}

int luat_uart_setup(luat_uart_t *uart) {
    return 0;
}

int luat_uart_write(int uartid, void *data, size_t length)
{
    return 0;
}

int luat_uart_read(int uartid, void *buffer, size_t length)
{
    return 0;
}

int luat_uart_close(int uartid)
{
    return 0;
}

int luat_setup_cb(int uartid, int received, int sent) {
    return 0;
}

#endif      // VSF_USE_EVM && VSF_EVM_USE_LUA
