/*****************************************************************************
 *   Copyright(C)2009-2020 by VSF Team                                       *
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
 ****************************************************************************/

/*============================ INCLUDES ======================================*/

#include "./debug_uart.h"

#if VSF_HAL_USE_DEBUG_STREAM == ENABLED

#if VSF_USE_SIMPLE_STREAM == ENABLED
#   define __VSF_SIMPLE_STREAM_CLASS_INHERIT__
#   include "service/vsf_service.h"
#endif

#include "stdio_uart.h"
#include "reg_ipc_mutex.h"

/*============================ MACROS ========================================*/

#if     VSF_USE_SIMPLE_STREAM == ENABLED
#   ifndef VSF_DEBUG_STREAM_CFG_RX_BUF_SIZE
#       define VSF_DEBUG_STREAM_CFG_RX_BUF_SIZE         32
#   endif
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

#if     VSF_USE_SIMPLE_STREAM == ENABLED
static void __vsf_rx_stream_init(vsf_stream_t *stream);
static uint_fast32_t __vsf_tx_stream_write(vsf_stream_t* stream, uint8_t* buf, uint_fast32_t size);
static uint_fast32_t __vsf_tx_stream_get_data_length(vsf_stream_t* stream);
static uint_fast32_t __vsf_tx_stream_get_avail_length(vsf_stream_t* stream);
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

#if     VSF_USE_SIMPLE_STREAM == ENABLED
static uint8_t __vsf_debug_stream_rx_buff[VSF_DEBUG_STREAM_CFG_RX_BUF_SIZE];

static const vsf_stream_op_t __vsf_tx_stream_op = {
    .init               = __vsf_rx_stream_init,
    .write              = __vsf_tx_stream_write,
    .get_data_length    = __vsf_tx_stream_get_data_length,
    .get_avail_length   = __vsf_tx_stream_get_avail_length,
};
#endif

/*============================ GLOBAL VARIABLES ==============================*/
#if     VSF_USE_SIMPLE_STREAM == ENABLED
vsf_stream_t VSF_DEBUG_STREAM_TX = {
    .op         = &__vsf_tx_stream_op,
};

vsf_mem_stream_t VSF_DEBUG_STREAM_RX = {
    .op         = &vsf_mem_stream_op,
    .buffer     = __vsf_debug_stream_rx_buff,
    .size       = sizeof(__vsf_debug_stream_rx_buff),
};

#elif   VSF_USE_STREAM == ENABLED
#endif

/*============================ IMPLEMENTATION ================================*/

#if VSF_USE_SIMPLE_STREAM == ENABLED
static void __vsf_rx_stream_on_uart_rx(void)
{
    uint_fast32_t rx_free_size = VSF_STREAM_GET_FREE_SIZE(&VSF_DEBUG_STREAM_RX);
    uint8_t ch;

    while (rx_free_size && !stdio_uart_rx_dbuf_empty_getf()) {
        rx_free_size--;

        ch = stdio_uart_rxdata_getf();
        VSF_STREAM_WRITE(&VSF_DEBUG_STREAM_RX, &ch, 1);
    }
}

static void __vsf_rx_stream_init(vsf_stream_t *stream)
{
    register_stdio_uart_rx_function(__vsf_rx_stream_on_uart_rx);
}

static uint_fast32_t __vsf_tx_stream_write(vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size)
{
    while(!ipc_mutex_get(IPC_MUTEX_UART_OUTPUT));
        for (uint_fast32_t i = 0; i < size; i++) {
            stdio_uart_putc(*buf++);
        }
    ipc_mutex_set(IPC_MUTEX_UART_OUTPUT, 1);
    return size;
}

static uint_fast32_t __vsf_tx_stream_get_data_length(vsf_stream_t* stream)
{
    return 0;
}

static uint_fast32_t __vsf_tx_stream_get_avail_length(vsf_stream_t* stream)
{
    return 0xFFFFFFFF;
}

#elif   VSF_USE_STREAM == ENABLED
#endif

#endif
