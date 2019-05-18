/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
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
#include "Device.h"
#include <stdio.h>
void uart_config(void)
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

void vsf_stdout_init(void)
{
    uart_config();
}

void vsf_stdin_init(void)
{

}

char vsf_stdin_getchar(void)
{
#if defined(IOTKIT_SECURE_UART0)
    while(!(IOTKIT_UART0->STATE & IOTKIT_UART_STATE_RXBF_Msk));
    return (char)(IOTKIT_SECURE_UART0->DATA);
#elif defined(CMSDK_UART0)
    while(!(CMSDK_UART0->STATE & CMSDK_UART_STATE_RXBF_Msk));
    return (char)(CMSDK_UART0->DATA);
#else
#error No defined USART
#endif
}

int vsf_stdout_putchar(char txchar)
{
    if (txchar == 10) vsf_stdout_putchar((char) 13);

#if defined(IOTKIT_SECURE_UART0)
    while(IOTKIT_SECURE_UART0->STATE & IOTKIT_UART_STATE_TXBF_Msk);
    IOTKIT_SECURE_UART0->DATA = (uint32_t)txchar;
    return (int) txchar;
#elif defined(CMSDK_UART0)
    while(CMSDK_UART0->STATE & CMSDK_UART_STATE_TXBF_Msk);
    CMSDK_UART0->DATA = (uint32_t)txchar;
    
    (*(volatile uint32_t *)0x41000000) = txchar;
    
    return (int) txchar;
#else
#error No defined USART
#endif
}


