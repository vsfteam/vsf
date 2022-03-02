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

#ifdef __USE_COMMON_RETARGET_IO_C__
#undef __USE_COMMON_RETARGET_IO_C__

#include <stdio.h>
#include "utilities/vsf_utilities_cfg.h"
#include "utilities/vsf_utilities.h"

#ifndef VSF_UNUSED_PARAM
#   define VSF_UNUSED_PARAM(__VAL)      (__VAL) = (__VAL)
#endif

#if VSF_USE_POSIX == ENABLED
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

void vsf_stdio_init(void)
{
    
}

SECTION(".vsf.utilities.stdio.__vsf_stdio_write")
size_t __vsf_stdio_write(int handle, const unsigned char *buf, size_t buf_size)
{
    return write(handle, (void *)buf, buf_size);
}

SECTION(".vsf.utilities.stdio.iar.__vsf_stdio_read")
size_t __vsf_stdio_read(int handle, unsigned char *buf, size_t buf_size)
{
    return read(handle, buf, buf_size);
}

#else

extern int vsf_stdout_putchar(char ch);
extern int vsf_stderr_putchar(char ch);
extern int vsf_stdin_getchar(void);

extern void vsf_stdout_init(void);
extern void vsf_stdin_init(void);

WEAK(vsf_stderr_putchar)
int vsf_stderr_putchar(char ch)
{
    return vsf_stdout_putchar(ch);
}

void vsf_stdio_init(void)
{
    vsf_stdout_init();
    vsf_stdin_init();
}

SECTION(".vsf.utilities.stdio.__vsf_stdio_write")
size_t __vsf_stdio_write(int handle, const unsigned char *buf, size_t buf_size)
{
    size_t nChars = 0;
    /* Check for the command to flush all handles */
    if (handle == -1) {
        return 0;
    }
    /* Check for stdout and stderr
    (only necessary if FILE descriptors are enabled.) */
    if (handle != 1 && handle != 2) {
        return 0;
    }
    for (/* Empty */; buf_size > 0; --buf_size) {
        vsf_stdout_putchar(*buf++);
        ++nChars;
    }
    return nChars;
}

SECTION(".vsf.utilities.stdio.__vsf_stdio_read")
size_t __vsf_stdio_read(int handle, unsigned char *buf, size_t buf_size)
{
    size_t nChars = 0;
    /* Check for stdin
    (only necessary if FILE descriptors are enabled) */
    if (handle != 0) {
        return 0;
    }
    for (/*Empty*/; buf_size > 0; --buf_size) {
        uint8_t c = vsf_stdin_getchar();
        if (c == 0) { break; }
        *buf++ = c;
        ++nChars;
    }
    return nChars;
}

#endif

#if     __IS_COMPILER_IAR__
#   define __USE_COMMON_RETARGET_IO_IAR_C__
#   include "./__retarget_io/__retarget_io_iar.c"
#elif   __IS_COMPILER_GCC__ || __IS_COMPILER_LLVM__
#   define __USE_COMMON_RETARGET_IO_GCC_LLVM_C__
#   include "./__retarget_io/__retarget_io_gcc_llvm.c"
#endif

#endif      //__VSF_USE_COMMON_RETARGET_IO_C__
