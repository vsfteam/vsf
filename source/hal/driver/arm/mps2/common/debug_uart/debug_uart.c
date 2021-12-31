/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
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

#include "vsf.h"
#include <stdio.h>

#if VSF_HAL_USE_DEBUG_STREAM == ENABLED

#if VSF_USE_SIMPLE_STREAM == ENABLED
#   define __VSF_SIMPLE_STREAM_CLASS_INHERIT__
#   include "service/vsf_service.h"
#endif

/*============================ MACROS ========================================*/

#if     VSF_USE_SIMPLE_STREAM == ENABLED
#   ifndef VSF_DEBUG_STREAM_CFG_RX_BUF_SIZE
#       define VSF_DEBUG_STREAM_CFG_RX_BUF_SIZE         32
#   endif
#   ifndef VSF_DEBUG_STREAM_CFG_TX_BUF_SIZE
#       define VSF_DEBUG_STREAM_CFG_TX_BUF_SIZE         (64 * 1024)
#   endif
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

#if     VSF_USE_SIMPLE_STREAM == ENABLED
static uint8_t __vsf_debug_stream_rx_buff[VSF_DEBUG_STREAM_CFG_RX_BUF_SIZE];
static uint8_t __vsf_debug_stream_tx_buff[VSF_DEBUG_STREAM_CFG_TX_BUF_SIZE];
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#if     VSF_USE_SIMPLE_STREAM == ENABLED
vsf_mem_stream_t VSF_DEBUG_STREAM_TX = {
    .op         = &vsf_mem_stream_op,
    .buffer     = __vsf_debug_stream_tx_buff,
    .size       = sizeof(__vsf_debug_stream_tx_buff),
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

static void __uart_config(void)
{
#if defined(IOTKIT_SECURE_UART0)
    IOTKIT_SECURE_UART0->CTRL = 0;         /* Disable UART when changing configuration */
    IOTKIT_SECURE_UART0->BAUDDIV = 651;    /* 50MHz / 38400 = 651 */
    IOTKIT_SECURE_UART0->CTRL = IOTKIT_UART_CTRL_TXEN_Msk| IOTKIT_UART_CTRL_RXEN_Msk;
     /* Update CTRL register to new value */
#elif defined(CMSDK_UART0)

    CMSDK_UART0->CTRL = 0;         /* Disable UART when changing configuration */
    CMSDK_UART0->BAUDDIV = 651;    /* 25MHz / 38400 = 651 */
    CMSDK_UART0->CTRL = CMSDK_UART_CTRL_TXEN_Msk|CMSDK_UART_CTRL_RXEN_Msk;
     /* Update CTRL register to new value */
#else
#error No defined USART
#endif
}

static bool __uart_is_tx_not_full(void)
{
#if defined(IOTKIT_SECURE_UART0)
    return !(IOTKIT_UART0->STATE & IOTKIT_UART_STATE_TXBF_Msk);
#elif defined(CMSDK_UART0)
    return !(CMSDK_UART0->STATE & CMSDK_UART_STATE_TXBF_Msk);
#else
#   error No defined USART
#endif
}

static void __uart_tx_write(uint8_t txchar)
{
#if defined(IOTKIT_SECURE_UART0)
    IOTKIT_SECURE_UART0->DATA = (uint32_t)txchar;
#elif defined(CMSDK_UART0)
    CMSDK_UART0->DATA = (uint32_t)txchar;
#else
#   error No defined USART
#endif
}

static bool __uart_is_rx_not_empty(void)
{
#if defined(IOTKIT_SECURE_UART0)
    return (IOTKIT_UART0->STATE & IOTKIT_UART_STATE_RXBF_Msk);
#elif defined(CMSDK_UART0)
    return (CMSDK_UART0->STATE & CMSDK_UART_STATE_RXBF_Msk);
#else
#   error No defined USART
#endif
}

static uint8_t __uart_rx_read(void)
{
#if defined(IOTKIT_SECURE_UART0)
    return (char)(IOTKIT_SECURE_UART0->DATA);
#elif defined(CMSDK_UART0)
    return (char)(CMSDK_UART0->DATA);
#else
#   error No defined USART
#endif
}

// TODO: use interrupt mode
void VSF_DEBUG_STREAM_POLL(void)
{
    uint_fast32_t tx_size = VSF_STREAM_GET_DATA_SIZE(&VSF_DEBUG_STREAM_TX);
    uint_fast32_t rx_free_size = VSF_STREAM_GET_FREE_SIZE(&VSF_DEBUG_STREAM_RX);
    uint8_t ch;

    static bool __is_init = false;

    if (!__is_init) {
        __is_init = true;
        __uart_config();
    }

    while (tx_size && __uart_is_tx_not_full()) {
        tx_size--;

        VSF_STREAM_READ(&VSF_DEBUG_STREAM_TX, &ch, 1);
        __uart_tx_write(ch);
    }

    while (rx_free_size && __uart_is_rx_not_empty()) {
        rx_free_size--;

        ch = __uart_rx_read();
        VSF_STREAM_WRITE(&VSF_DEBUG_STREAM_RX, &ch, 1);
    }
}
#elif   VSF_USE_STREAM == ENABLED
#endif

#endif


