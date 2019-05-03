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

void stdout_init(void)
{
    uart_config();
}

char stdin_getchar(void)
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

int stdout_putchar(char txchar)
{
    if (txchar == 10) stdout_putchar((char) 13);

#if defined(IOTKIT_SECURE_UART0)
    while(IOTKIT_SECURE_UART0->STATE & IOTKIT_UART_STATE_TXBF_Msk);
    IOTKIT_SECURE_UART0->DATA = (uint32_t)txchar;
    return (int) txchar;
#elif defined(CMSDK_UART0)
    while(CMSDK_UART0->STATE & CMSDK_UART_STATE_TXBF_Msk);
    CMSDK_UART0->DATA = (uint32_t)txchar;
    return (int) txchar;
#else
#error No defined USART
#endif
}

#if __IS_COMPILER_ARM_COMPILER_6__

void _ttywrch(int ch)
{
    extern int stdout_putchar(char txchar);
    stdout_putchar(ch);
}

#endif

/**
   Writes the character specified by c (converted to an unsigned char) to
   the output stream pointed to by stream, at the position indicated by the
   associated file position indicator (if defined), and advances the
   indicator appropriately. If the file position indicator is not defined,
   the character is appended to the output stream.
 
  \param[in] c       Character
  \param[in] stream  Stream handle
 
  \return    The character written. If a write error occurs, the error
             indicator is set and fputc returns EOF.
*/
int fputc (int c, FILE * stream) {
#if (!defined(RTE_Compiler_IO_STDOUT) && !defined(RTE_Compiler_IO_STDERR))
  (void)c;
  (void)stream;
#endif
    return (stdout_putchar(c));
}

#if __IS_COMPILER_IAR__
ROOT
size_t __write(int handle, const unsigned char *buf, size_t bufSize)
{
    size_t nChars = 0;
    /* Check for the command to flush all handles */
    if (handle == -1) {
        return 0;
    }
    /* Check for stdout and stderr
    (only necessary if FILE descriptors are enabled.) */
    if (handle != 1 && handle != 2) {
        return -1;
    }
    for (/* Empty */; bufSize > 0; --bufSize) {
        stdout_putchar(*buf++);
        ++nChars;
    }
    return nChars;
}
ROOT
size_t __read(int handle, unsigned char *buf, size_t bufSize)
{
    size_t nChars = 0;
    /* Check for stdin
    (only necessary if FILE descriptors are enabled) */
    if (handle != 0) {
        return -1;
    }
    for (/*Empty*/; bufSize > 0; --bufSize) {
        uint8_t c = stdin_getchar();
        if (c == 0) { break; }
        *buf++ = c;
        ++nChars;
    }
    return nChars;
}

#endif

#if __IS_COMPILER_GCC__ || __IS_COMPILER_LLVM__
int _write (int handle, char *buf, int bufSize)
{
    size_t nChars = 0;
    /* Check for the command to flush all handles */
    if (handle == -1) {
        return 0;
    }
    /* Check for stdout and stderr
    (only necessary if FILE descriptors are enabled.) */
    if (handle != 1 && handle != 2) {
        return -1;
    }
    for (/* Empty */; bufSize > 0; --bufSize) {
        stdout_putchar(*buf++);
        ++nChars;
    }
    return nChars;
}

int _read (int handle, char *buf, int bufSize)
{
    size_t nChars = 0;
    /* Check for stdin
    (only necessary if FILE descriptors are enabled) */
    if (handle != 0) {
        return -1;
    }
    for (/*Empty*/; bufSize > 0; --bufSize) {
        uint8_t c = stdin_getchar();
        if (c == 0) { break; }
        *buf++ = c;
        ++nChars;
    }
    return nChars;
}

void _ttywrch(int ch) {
  /* Write one char "ch" to the default console
   * Need implementing with UART here. */
}
#endif

